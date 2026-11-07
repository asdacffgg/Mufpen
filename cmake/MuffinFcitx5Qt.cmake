# Bundles a fcitx5 input-method plugin into the Linux dist tree.
#
# The dist bundle ships its own Qt and points qt.conf's Plugins entry at the bundle, so Qt
# never looks at the distro's plugin directory. Copying the distro's fcitx5 plugin in is not
# an option either: Qt refuses plugins built against a different minor version ("uses
# incompatible Qt library", verified with a Qt 6.10.2 plugin under Qt 6.11.1). Qt 6.11 also
# dropped XIM from the xcb platform plugin, so there is no fallback path -- without a bundled
# platforminputcontext plugin the app cannot talk to any input method, and no CJK input works
# at all.
#
# fcitx5-qt is therefore built here against the exact Qt this build links. Two options keep
# the result simple:
#   BUILD_ONLY_PLUGIN=ON  skips the wrappers/editor and drops the Fcitx5Utils requirement, so
#                         no fcitx5 host development packages are needed to build it.
#   BUILD_STATIC_PLUGIN=OFF keeps the output a loadable module, while the Qt DBus addon is a
#                         object library whose objects are folded into the plugin -- the
#                         bundle only carries libfcitx5platforminputcontextplugin.so.

include(ExternalProject)

set(MUFFIN_FCITX5_QT_VERSION "5.1.13" CACHE STRING "fcitx5-qt release used for the bundled IME plugin")
set(MUFFIN_FCITX5_QT_URL
    "https://github.com/fcitx/fcitx5-qt/archive/refs/tags/${MUFFIN_FCITX5_QT_VERSION}.tar.gz"
    CACHE STRING "fcitx5-qt source archive")
set(MUFFIN_FCITX5_QT_URL_HASH
    "SHA256=74b2c43ca865ec9ede257bcd3bdadafc20f571900cb2440a4488335e4d4e91cc"
    CACHE STRING "fcitx5-qt source archive hash")

# Adds the plugin build and returns the built .so path in <out_plugin_file>.
function(muffin_add_fcitx5_qt_plugin out_plugin_file)
  find_package(ECM QUIET)
  if(NOT ECM_FOUND)
    message(FATAL_ERROR
      "Bundling the fcitx5 input-method plugin needs Extra CMake Modules "
      "(fcitx5-qt calls find_package(ECM 1.4.0 REQUIRED)).\n"
      "  Ubuntu/Debian: sudo apt install extra-cmake-modules\n"
      "  Fedora:        sudo dnf install extra-cmake-modules\n"
      "Pass -DMUFFIN_BUNDLE_FCITX5_PLUGIN=OFF to ship a bundle without IME support instead.")
  endif()

  set(source_dir "${CMAKE_BINARY_DIR}/fcitx5-qt/src")
  set(binary_dir "${CMAKE_BINARY_DIR}/fcitx5-qt/build")
  set(plugin_file "${binary_dir}/qt6/platforminputcontext/libfcitx5platforminputcontextplugin.so")

  # The nested configure must resolve Qt exactly like this build does: a Conan Qt only
  # exists through the generated configs in CMAKE_PREFIX_PATH, so hand the toolchain file
  # (which prepends them) and the escaped prefix list over verbatim.
  set(toolchain_arg "")
  if(CMAKE_TOOLCHAIN_FILE)
    set(toolchain_arg "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")
  endif()
  string(REPLACE ";" "\\;" prefix_path_arg "${CMAKE_PREFIX_PATH}")

  ExternalProject_Add(muffin_fcitx5_qt
    URL "${MUFFIN_FCITX5_QT_URL}"
    URL_HASH "${MUFFIN_FCITX5_QT_URL_HASH}"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    SOURCE_DIR "${source_dir}"
    BINARY_DIR "${binary_dir}"
    INSTALL_DIR "${CMAKE_BINARY_DIR}/fcitx5-qt/install"
    # The plugin is taken straight from the build tree; fcitx5-qt's own install() targets
    # the *host* Qt plugin directory (queried from qmake), which must not be written to.
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "${plugin_file}"
    BUILD_COMMAND "${CMAKE_COMMAND}" --build <BINARY_DIR> --parallel
    EXCLUDE_FROM_ALL TRUE
    CMAKE_ARGS
      "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
      "-DCMAKE_PREFIX_PATH=${prefix_path_arg}"
      "-DQt6_DIR=${Qt6_DIR}"
      "${toolchain_arg}"
      -DENABLE_QT4=OFF
      -DENABLE_QT5=OFF
      -DENABLE_QT6=ON
      -DENABLE_X11=ON
      # The Wayland workaround needs Qt6::WaylandClientPrivate from the qtwayland module,
      # which this build does not ship (the app runs on xcb, also on Wayland sessions).
      -DENABLE_QT6_WAYLAND_WORKAROUND=OFF
      -DBUILD_ONLY_PLUGIN=ON
      -DBUILD_STATIC_PLUGIN=OFF
  )
  set(${out_plugin_file} "${plugin_file}" PARENT_SCOPE)
endfunction()
