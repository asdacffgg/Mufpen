"""Build the static update catalog the app polls for new versions.

Reads the artifacts produced by a release build, re-verifies the signed Windows
update envelopes against the public key embedded in the app, and writes
mufpen-update.json alongside them. Uploading that file is the release
workflow's job: this script never touches the network.
"""

import argparse
import base64
import hashlib
import json
from pathlib import Path
import re
import sys
import urllib.parse

REPO = "asdacffgg/Mufpen"
GITHUB = f"https://github.com/{REPO}/releases"
CATALOG = "mufpen-update.json"
SUFFIXES = (".7z", ".tar.gz", ".msi", ".exe", ".dmg", ".update.json")


class CatalogError(RuntimeError):
    pass


def sha256(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def version(tag):
    if not re.fullmatch(r"v(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)", tag):
        raise CatalogError("Expected a stable vX.Y.Z release tag")
    return tuple(map(int, tag[1:].split(".")))


def artifact_files(directory, tag):
    release_version = tag[1:]
    result = {}
    for path in sorted(directory.rglob("*")):
        if not path.is_file() or not path.name.endswith(SUFFIXES):
            continue
        if not path.name.startswith((f"Mufpen-{tag}-", f"Mufpen-{release_version}-", f"Mufpen-Setup-{release_version}-")):
            raise CatalogError(f"Artifact belongs to a different release: {path.name}")
        if path.name in result:
            raise CatalogError(f"Duplicate artifact: {path.name}")
        result[path.name] = path
    return result


def verify_windows_packages(files, tag, public_key_path):
    from cryptography.hazmat.primitives import hashes
    from cryptography.hazmat.primitives.asymmetric import ec, utils

    blob = bytes.fromhex(public_key_path.read_text().strip())
    if len(blob) != 72 or blob[:8] != bytes.fromhex("4543533120000000"):
        raise CatalogError("Invalid embedded update public key")
    key = ec.EllipticCurvePublicNumbers(int.from_bytes(blob[8:40], "big"), int.from_bytes(blob[40:], "big"), ec.SECP256R1()).public_key()
    for arch in ("x64", "arm64"):
        stem = f"Mufpen-Setup-{tag[1:]}-windows-{arch}"
        installer, manifest = files.get(stem + ".exe"), files.get(stem + ".update.json")
        if not installer or not manifest:
            raise CatalogError(f"Missing signed Windows {arch} update")
        envelope = json.loads(manifest.read_bytes())
        payload = base64.b64decode(envelope["payload"], validate=True)
        signature = base64.b64decode(envelope["signature"], validate=True)
        if len(signature) != 64:
            raise CatalogError(f"Invalid {arch} signature length")
        try:
            key.verify(utils.encode_dss_signature(int.from_bytes(signature[:32], "big"), int.from_bytes(signature[32:], "big")), payload, ec.ECDSA(hashes.SHA256()))
        except Exception as error:
            raise CatalogError(f"Invalid {arch} update signature") from error
        expected = {"schema": 1, "version": tag[1:], "architecture": arch, "fileName": installer.name,
                    "size": installer.stat().st_size, "sha256": sha256(installer)}
        if json.loads(payload) != expected:
            raise CatalogError(f"Signed {arch} metadata does not match the installer")


def catalog(files, tag):
    return {"schema": 1, "tag_name": tag, "html_url": f"{GITHUB}/tag/{tag}", "draft": False, "prerelease": False,
            "assets": [{"name": name, "size": path.stat().st_size,
                        "browser_download_url": f"{GITHUB}/download/{tag}/{urllib.parse.quote(name)}"}
                       for name, path in sorted(files.items())]}


def write_catalog(directory, obj):
    directory.mkdir(parents=True, exist_ok=True)
    path = directory / CATALOG
    path.write_text(json.dumps(obj, ensure_ascii=True, separators=(",", ":")) + "\n", encoding="utf-8")
    return path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--tag", required=True)
    parser.add_argument("--assets", type=Path, required=True)
    parser.add_argument("--output", type=Path, default=Path("update-metadata"))
    parser.add_argument("--public-key", type=Path,
                        default=Path(__file__).resolve().parents[1] / "resources/update-public-key.txt")
    args = parser.parse_args()
    version(args.tag)
    files = artifact_files(args.assets, args.tag)
    verify_windows_packages(files, args.tag, args.public_key)
    path = write_catalog(args.output / "github", catalog(files, args.tag))
    print(f"Wrote {path}")


if __name__ == "__main__":
    try:
        main()
    except CatalogError as error:
        print(f"catalog: {error}", file=sys.stderr)
        sys.exit(1)
