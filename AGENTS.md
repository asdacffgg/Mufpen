# AGENTS.md

## Build acceleration

`ccache` and `ninja` are installed locally. The current `build/Release` tree uses
the **Ninja** generator (see Conan profile note in the Build section below) with
ccache enabled. ccache was enabled once at configure time via:

```bash
cmake --preset conan-release \
  -DCMAKE_C_COMPILER_LAUNCHER=ccache \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

The first build after enabling ccache may still compile everything; later
rebuilds hit the cache. Prefer target-specific incremental builds. For how to
pick a `-j` value for the current machine, see
`docs/development-retrospective-2026-11-09.zh.md` §2.2.

Do not run the unscoped `cmake --build --preset conan-release` (it builds all
test targets) unless a full build is intended.

### MSVC environment (Windows)

`cl.exe` is not on the parent PATH, so a bare `cmake --preset conan-release` or
`cmake --build` fails with "cl is not a full path". Run the Conan-generated
`conanvcvars.bat` first, in the same command, to init the VS environment:

```powershell
cmd /c 'call "build\Release\generators\conanvcvars.bat" && cmake --preset conan-release ...'
cmd /c 'call "build\Release\generators\conanvcvars.bat" && cmake --build --preset conan-release --target Mufpen -j 16'
```

## Build

This project uses Conan-generated CMake presets. Use the Release preset for local verification:

Install the build-time font conversion dependencies once with
`python -m pip install -r requirements-build.txt`. CMake converts the original
WOFF/WOFF2 fonts into `build/theme-fonts/`; do not commit generated TTF files.
PySide6 is also required (`pip install PySide6`): CMake copies the `qtbase_*.qm`
translations from it because Conan's Qt package ships no translations.

### Local Conan profile (this workspace, Windows / VS 2026)

The machine has **Visual Studio 2026 Insiders** (`cl` 19.51, Conan
`compiler.version=195`). ConanCenter has no prebuilt Qt/freetype/icu/etc.
binaries for MSVC 19.5 + `cppstd=20`, so all 34 dependencies are built from
source once and then cached. The global profile `~/.conan2/profiles/default`
carries these fixes (keep them — removing any one breaks the source build):

```ini
[conf]
tools.cmake.cmaketoolchain:generator=Ninja      # CMake 3.30/3.31 can't emit "Visual Studio 18 2026"
tools.build:jobs=16
tools.build:cxxflags=["/utf-8"]                  # avoid C4819 under codepage 936 (/WX in yoga)
tools.build:cflags=["/utf-8"]
qt/*:tools.cmake.cmaketoolchain:user_toolchain=["D:/conan2/freetype-fix-toolchain.cmake"]

[buildenv]
MSYS2_ARG_CONV_EXCL=/utf-8                       # MSYS2 must not rewrite the /utf-8 flag
```

`D:/conan2/freetype-fix-toolchain.cmake` + `D:/conan2/my-freetype-config/` pin
`Freetype_DIR`/`freetype_DIR` to Conan's freetype. Without this, Qt's
`find_package(Freetype CONFIG)` picks up Anaconda's incompatible
`D:/Anaconda3/Library/lib/cmake/freetype` and fails on `ft2build.h`.

The Conan cache lives on **D:** (`C:\Users\asus\.conan2` is a junction to
`D:\conan2`) to keep the 16+ GB cache off C:. The ccache cache is also on D:
(`CCACHE_DIR=D:\ccache`, set as a user env var). PySide6 (~0.6 GB) lives in the
user site-packages on C: and is needed at configure time to source `qtbase_*.qm`
translations; those .qm files are gitignored in `translations/`, so keep PySide6
installed unless you re-source the translations.

### Q_OBJECT export-macro rule (source of a past build break)

A `Q_OBJECT` class whose `.cpp` is compiled only into `Mufpen.exe` / test
targets must **not** carry `MUFPEN_UI_EXPORT` or `MUFPEN_CORE_EXPORT`. In the exe
those macros expand to `Q_DECL_IMPORT`, so moc's generated `staticMetaObject`
definition fails with C2491/C2127. Only classes actually compiled into
`MufpenUi.dll` (listed in `MUFPEN_UI_SOURCES`) may use `MUFPEN_UI_EXPORT`.
`ImageView` was wrongly marked `MUFPEN_UI_EXPORT` while its `.cpp` lives only in
the exe/test targets; the macro was removed to fix the build.

CMake's ICU-data `FetchContent` step downloads a zip from GitHub at configure
time; if curl resets the connection, download
`icu4c-74_2-data-bin-l.zip` manually into
`build/Release/_deps/mufpen_icu_data-subbuild/mufpen_icu_data-populate-prefix/src/`
and reconfigure (SHA256 is verified by `cmake/MufpenIcuData.cmake`).

```powershell
conan install . -s build_type=Release -s compiler.cppstd=20 --build=missing
cmake --preset conan-default
cmake --build --preset conan-release
```

Do not use plain `cmake --build build` for verification on this workspace. The generated Visual Studio project may default to Debug, while the Conan Qt environment is configured for Release, which can fail with missing Qt headers such as `QString`.

On Linux, pass `-s compiler.cppstd=gnu20` (the Linux CI matrix already does) and configure with `--preset conan-release`: Conan generates a single-config preset there, and GCC 15 fails to build Qt in strict `-std=c++20` mode (`Qt needs to be compiled in a mode that enables INT128`).

## Test

Run the full test suite with:

```powershell
ctest --preset conan-release --output-on-failure
```

Before reporting a change as verified, prefer running both:

```powershell
cmake --build --preset conan-release
ctest --preset conan-release --output-on-failure
```

## Verification checklist

These rules come from the 2026-11-07..09 development session; see
`docs/development-retrospective-2026-11-09.zh.md` for the cases behind them.

- Capture the real exit code when piping build/test output. A bare
  `... | tail -n` reports `tail`'s status:

  ```bash
  ctest --preset conan-release --output-on-failure 2>&1 | tail -20
  rc=${PIPESTATUS[0]}
  ```

- Choose the test platform by dependency: `QT_QPA_PLATFORM=minimal` for geometry
  and layout; `QT_QPA_PLATFORM=offscreen` for Mermaid, image fonts, and
  rasterization. Mermaid font registration crashes under `minimal`.
- Do not measure memory or performance under `offscreen`; it has no real
  viewport and takes the full-layout path. Use Xvfb or the real desktop and sum
  PSS across the whole process tree.
- Before debugging runtime behaviour, confirm which binary is running
  (`readlink -f "$(command -v mufpen)"`, `pgrep -a -x Mufpen`).
  `build/Release`, `build/dist`, and `~/.local/opt/Mufpen` are different trees.
- After moving or renaming the checkout, reconfigure CMake and rebuild fully
  (the build tree embeds absolute source paths), then run `codegraph index .`.
- After installing or replacing icons, refresh the GTK cache:
  `gtk-update-icon-cache -f -t ~/.local/share/icons/hicolor` (the user-local
  hicolor has no `index.theme`, so a stale cache hides new icons).
- Only call a failure pre-existing after reproducing it on the previous binary
  or the previous CI run; do not infer it from the message alone.

## Package And Run

MufpenCore/MufpenUi build as SHARED libraries by default (`MUFPEN_LIB_TYPE`); `Mufpen.exe` and `mufpen-mmdc.exe` are thin executables that need `MufpenCore.dll`/`MufpenUi.dll` beside them. The dist target and the MSI assume this layout. Run the packaged app (or any test exe) from its own directory — the DLLs sit next to it.

`cmake --build --preset conan-release` updates `build\Release\Mufpen.exe`, but it does not update `build\dist\Mufpen.exe`.

To refresh the distributable app bundle, run the dist target after building:

```powershell
cmake --build --preset conan-release
cmake --build --preset conan-release --target dist
```

The packaged executable is:

```powershell
.\build\dist\Mufpen.exe
```

If `build\dist\Mufpen.exe` is older than `build\Release\Mufpen.exe`, run the dist target again before testing the packaged app.

### Linux input methods (CJK input)

The dist bundle carries its own Qt and `qt.conf` points `Plugins` at the bundle, so Qt never looks at the distribution's plugin directory. Qt also refuses plugins built against a different minor version (`uses incompatible Qt library`), and Qt 6.11 removed the XIM fallback from the xcb platform plugin. A Linux bundle without a `platforminputcontexts/` plugin therefore cannot talk to fcitx5/ibus at all — no Chinese/Japanese/Korean input, silently.

`cmake/MufpenFcitx5Qt.cmake` closes that hole: the dist target builds fcitx5-qt (`5.1.13`, `BUILD_ONLY_PLUGIN=ON`, `BUILD_STATIC_PLUGIN=OFF`, so one self-contained `libfcitx5platforminputcontextplugin.so`) against the exact Qt this build links and copies it into `dist/platforminputcontexts/`. It needs `extra-cmake-modules` on the build machine (`sudo apt install extra-cmake-modules`) and a Qt built with DBus — `conanfile.py` therefore sets `qt/*:with_dbus=True` on Linux, because the plugin talks to fcitx5 over QtDBus. Pass `-DMUFPEN_BUNDLE_FCITX5_PLUGIN=OFF` to skip the plugin and ship a bundle without IME support.

## Application Icons

The committed raster icons in `resources/app-icons/` (PNGs at 16–1024px, `mufpen.ico`, `mufpen.icns`) are embedded by CMake automatically — the `.ico` into the Windows `.exe` via `cmake/mufpen.rc.in`, the `.icns` into the macOS `.app` bundle, and a 256px PNG as the runtime window icon (all platforms, primary on Linux). No GitHub Actions change is needed; the build picks them up on every platform.

They are generated from `logo.svg`. Regenerate after editing the logo:

```powershell
python scripts/generate_icons.py
```

This requires PySide6 (uses `QSvgRenderer`, the same Qt SVG engine the app links) and Pillow. Both ship in CI; locally `pip install PySide6 Pillow`. Do not hand-edit the generated files.

## Translation (i18n)

### lupdate namespace context bug

`lupdate` fails to track namespace prefixes across `namespace mufpen { }` wrappers in `.cpp` files. It generates context `ClassName` instead of the correct `mufpen::ClassName`, causing translations to never match at runtime (where `tr()` uses `metaObject()->className()` = `mufpen::ClassName`).

**Rule:** A `.cpp` file in `MUFPEN_TRANSLATABLE_SOURCES` that contains `tr()` calls MUST NOT contain *any* `namespace mufpen { }` block — not even one wrapping `tr()`-free helpers. A single such block anywhere in the file corrupts `lupdate`'s namespace state, so every `tr()` afterward lands under the wrong (un-prefixed) context. Define methods with fully-qualified names (e.g. `mufpen::MainWindow::setupFileMenu()`) and put `tr()`-free file-local helpers in an *anonymous* namespace (`namespace { ... }`) at file scope — never inside `namespace mufpen { }`.

When adding a new `.cpp` file with `tr()` calls to the translatable sources, do NOT wrap it in `namespace mufpen { }` — not even partially. Use `mufpen::ClassName::method()` for all method definitions and keep helpers in an anonymous namespace. `src/app/HelpViewerDialog.cpp` is a worked example: `mufpen::HelpViewerDialog::method()` definitions plus a `namespace { ... }` helper, which `lupdate` correctly attributes to the `mufpen::HelpViewerDialog` context.

### Updating translations

After modifying translatable strings:

```powershell
cmake --build --preset conan-release --target update_translations
cmake --build --preset conan-release --target release_translations
cmake --build --preset conan-release --target dist
```

Use subagents to modify `.ts` files in parallel — avoid bulk Python/sed scripts for XML manipulation as they are error-prone (losing closing tags, removing valid entries, etc.).


其他注意点：
* 当你自己commit的时候，**严禁把co author加上claude code**
