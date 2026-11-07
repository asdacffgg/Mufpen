"""Offline tests for the update catalog builder; no credentials or network."""

import base64
import importlib.util
import json
from pathlib import Path
import tempfile
import unittest

from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric import ec, utils

spec = importlib.util.spec_from_file_location("catalog", Path(__file__).with_name("prepare_update_catalog.py"))
catalog = importlib.util.module_from_spec(spec)
spec.loader.exec_module(catalog)


class CatalogTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.key = ec.generate_private_key(ec.SECP256R1())
        numbers = self.key.public_key().public_numbers()
        self.public_key = self.root / "key.txt"
        self.public_key.write_text((bytes.fromhex("4543533120000000") + numbers.x.to_bytes(32, "big") + numbers.y.to_bytes(32, "big")).hex())
        self.assets = self.root / "assets"
        self.assets.mkdir()
        for arch in ("x64", "arm64"):
            installer = self.assets / f"Mufpen-Setup-1.2.3-windows-{arch}.exe"
            installer.write_bytes((arch + " fixture").encode() * 100)
            payload = json.dumps({"schema": 1, "version": "1.2.3", "architecture": arch, "fileName": installer.name,
                                  "size": installer.stat().st_size, "sha256": catalog.sha256(installer)}).encode()
            r, s = utils.decode_dss_signature(self.key.sign(payload, ec.ECDSA(hashes.SHA256())))
            installer.with_suffix(".update.json").write_text(json.dumps({"payload": base64.b64encode(payload).decode(),
                "signature": base64.b64encode(r.to_bytes(32, "big") + s.to_bytes(32, "big")).decode()}))
        (self.assets / "Mufpen-v1.2.3-linux-x64.tar.gz").write_bytes(b"linux payload")
        self.files = catalog.artifact_files(self.assets, "v1.2.3")

    def test_signed_windows_packages_are_verified(self):
        catalog.verify_windows_packages(self.files, "v1.2.3", self.public_key)
        installer = next(path for path in self.files.values() if path.name.endswith("-x64.exe"))
        installer.write_bytes(b"modified after signing")
        with self.assertRaises(catalog.CatalogError):
            catalog.verify_windows_packages(self.files, "v1.2.3", self.public_key)

    def test_signature_from_another_publisher_is_rejected(self):
        other = ec.generate_private_key(ec.SECP256R1()).public_key().public_numbers()
        self.public_key.write_text((bytes.fromhex("4543533120000000") + other.x.to_bytes(32, "big") + other.y.to_bytes(32, "big")).hex())
        with self.assertRaises(catalog.CatalogError):
            catalog.verify_windows_packages(self.files, "v1.2.3", self.public_key)

    def test_missing_manifest_is_rejected(self):
        manifest = self.assets / "Mufpen-Setup-1.2.3-windows-arm64.update.json"
        manifest.unlink()
        files = catalog.artifact_files(self.assets, "v1.2.3")
        with self.assertRaises(catalog.CatalogError):
            catalog.verify_windows_packages(files, "v1.2.3", self.public_key)

    def test_catalog_points_at_this_repository(self):
        obj = catalog.catalog(self.files, "v1.2.3")
        self.assertEqual(obj["html_url"], "https://github.com/asdacffgg/Mufpen/releases/tag/v1.2.3")
        urls = {asset["name"]: asset["browser_download_url"] for asset in obj["assets"]}
        self.assertEqual(
            urls["Mufpen-Setup-1.2.3-windows-x64.exe"],
            "https://github.com/asdacffgg/Mufpen/releases/download/v1.2.3/Mufpen-Setup-1.2.3-windows-x64.exe")
        self.assertEqual(
            urls["Mufpen-Setup-1.2.3-windows-x64.update.json"],
            "https://github.com/asdacffgg/Mufpen/releases/download/v1.2.3/Mufpen-Setup-1.2.3-windows-x64.update.json")
        self.assertFalse(obj["draft"] or obj["prerelease"])
        written = catalog.write_catalog(self.root / "out" / "github", obj)
        self.assertEqual(written.name, "mufpen-update.json")

    def test_ambiguous_or_mixed_artifacts_are_rejected(self):
        nested = self.assets / "duplicate"
        nested.mkdir()
        first = next(iter(self.files))
        (nested / first).write_bytes(b"duplicate")
        with self.assertRaises(catalog.CatalogError):
            catalog.artifact_files(self.assets, "v1.2.3")
        for tag in ("v1.2.3-beta", "v01.2.3", "v1.2.3/other"):
            with self.assertRaises(catalog.CatalogError):
                catalog.version(tag)
        (self.assets / "Muffin-Setup-1.2.3-windows-x64.exe").write_bytes(b"upstream name")
        with self.assertRaises(catalog.CatalogError):
            catalog.artifact_files(self.assets, "v1.2.3")


if __name__ == "__main__":
    unittest.main()
