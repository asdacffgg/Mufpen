# CLAUDE.md

## Build

This project uses Conan-generated CMake presets. Use the Release preset for local verification:

Install the build-time font conversion dependencies once with
`python -m pip install -r requirements-build.txt`. CMake converts the original
WOFF/WOFF2 fonts into `build/theme-fonts/`; do not commit generated TTF files.

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

## Package And Run

MuffinCore/MuffinUi build as SHARED libraries by default (`MUFFIN_LIB_TYPE`); `Muffin.exe` and `muffin-mmdc.exe` are thin executables that need `MuffinCore.dll`/`MuffinUi.dll` beside them. The dist target and the MSI assume this layout. Run the packaged app (or any test exe) from its own directory — the DLLs sit next to it.

`cmake --build --preset conan-release` updates `build\Release\Muffin.exe`, but it does not update `build\dist\Muffin.exe`.

To refresh the distributable app bundle, run the dist target after building:

```powershell
cmake --build --preset conan-release
cmake --build --preset conan-release --target dist
```

The packaged executable is:

```powershell
.\build\dist\Muffin.exe
```

If `build\dist\Muffin.exe` is older than `build\Release\Muffin.exe`, run the dist target again before testing the packaged app.

### Linux input methods (CJK input)

The dist bundle carries its own Qt and `qt.conf` points `Plugins` at the bundle, so Qt never looks at the distribution's plugin directory. Qt also refuses plugins built against a different minor version (`uses incompatible Qt library`), and Qt 6.11 removed the XIM fallback from the xcb platform plugin. A Linux bundle without a `platforminputcontexts/` plugin therefore cannot talk to fcitx5/ibus at all — no Chinese/Japanese/Korean input, silently.

`cmake/MuffinFcitx5Qt.cmake` closes that hole: the dist target builds fcitx5-qt (`5.1.13`, `BUILD_ONLY_PLUGIN=ON`, `BUILD_STATIC_PLUGIN=OFF`, so one self-contained `libfcitx5platforminputcontextplugin.so`) against the exact Qt this build links and copies it into `dist/platforminputcontexts/`. It needs `extra-cmake-modules` on the build machine (`sudo apt install extra-cmake-modules`) and a Qt built with DBus — `conanfile.py` therefore sets `qt/*:with_dbus=True` on Linux, because the plugin talks to fcitx5 over QtDBus. Pass `-DMUFFIN_BUNDLE_FCITX5_PLUGIN=OFF` to skip the plugin and ship a bundle without IME support.

## Application Icons

The committed raster icons in `resources/app-icons/` (PNGs at 16–1024px, `mufpen.ico`, `mufpen.icns`) are embedded by CMake automatically — the `.ico` into the Windows `.exe` via `cmake/muffin.rc.in`, the `.icns` into the macOS `.app` bundle, and a 256px PNG as the runtime window icon (all platforms, primary on Linux). No GitHub Actions change is needed; the build picks them up on every platform.

They are generated from `logo.svg`. Regenerate after editing the logo:

```powershell
python scripts/generate_icons.py
```

This requires PySide6 (uses `QSvgRenderer`, the same Qt SVG engine the app links) and Pillow. Both ship in CI; locally `pip install PySide6 Pillow`. Do not hand-edit the generated files.

## Translation (i18n)

### lupdate namespace context bug

`lupdate` fails to track namespace prefixes across `namespace muffin { }` wrappers in `.cpp` files. It generates context `ClassName` instead of the correct `muffin::ClassName`, causing translations to never match at runtime (where `tr()` uses `metaObject()->className()` = `muffin::ClassName`).

**Rule:** A `.cpp` file in `MUFFIN_TRANSLATABLE_SOURCES` that contains `tr()` calls MUST NOT contain *any* `namespace muffin { }` block — not even one wrapping `tr()`-free helpers. A single such block anywhere in the file corrupts `lupdate`'s namespace state, so every `tr()` afterward lands under the wrong (un-prefixed) context. Define methods with fully-qualified names (e.g. `muffin::MainWindow::setupFileMenu()`) and put `tr()`-free file-local helpers in an *anonymous* namespace (`namespace { ... }`) at file scope — never inside `namespace muffin { }`.

When adding a new `.cpp` file with `tr()` calls to the translatable sources, do NOT wrap it in `namespace muffin { }` — not even partially. Use `muffin::ClassName::method()` for all method definitions and keep helpers in an anonymous namespace. `src/app/HelpViewerDialog.cpp` is a worked example: `muffin::HelpViewerDialog::method()` definitions plus a `namespace { ... }` helper, which `lupdate` correctly attributes to the `muffin::HelpViewerDialog` context.

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
