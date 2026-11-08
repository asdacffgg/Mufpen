# Author only webfonts; native font containers and their QRC live in the build tree.
execute_process(
  COMMAND "${Python3_EXECUTABLE}" -c "from fontTools.ttLib import TTFont; import brotli"
  RESULT_VARIABLE _mufpen_font_tools_result
  ERROR_VARIABLE _mufpen_font_tools_error
)
if(NOT _mufpen_font_tools_result EQUAL 0)
  message(FATAL_ERROR
    "Theme font conversion requires FontTools and Brotli for ${Python3_EXECUTABLE}. "
    "Run: \"${Python3_EXECUTABLE}\" -m pip install -r \"${CMAKE_CURRENT_SOURCE_DIR}/requirements-build.txt\"\n"
    "${_mufpen_font_tools_error}")
endif()

file(GLOB_RECURSE MUFPEN_THEME_WEBFONTS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/resources/themes/*.woff"
  "${CMAKE_CURRENT_SOURCE_DIR}/resources/themes/*.woff2")
list(SORT MUFPEN_THEME_WEBFONTS)
set(MUFPEN_THEME_NATIVE_FONTS)
set(_mufpen_font_entries "")
foreach(_source IN LISTS MUFPEN_THEME_WEBFONTS)
  file(RELATIVE_PATH _alias "${CMAKE_CURRENT_SOURCE_DIR}/resources/themes" "${_source}")
  string(REGEX REPLACE "\\.woff2?$" ".ttf" _native_relative "${_alias}")
  set(_native "${CMAKE_CURRENT_BINARY_DIR}/theme-fonts/${_native_relative}")
  if(_native IN_LIST MUFPEN_THEME_NATIVE_FONTS)
    message(FATAL_ERROR "Theme fonts share a native output: ${_native_relative}")
  endif()
  add_custom_command(
    OUTPUT "${_native}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/prepare_theme_fonts.py"
      --input "${_source}" --output "${_native}"
    DEPENDS "${_source}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/prepare_theme_fonts.py"
      "${CMAKE_CURRENT_SOURCE_DIR}/requirements-build.txt"
    COMMENT "Convert theme font ${_alias}"
    VERBATIM)
  list(APPEND MUFPEN_THEME_NATIVE_FONTS "${_native}")
  # QRC aliases preserve authored CSS URLs even though their bytes are native SFNT.
  string(REPLACE "&" "&amp;" _native_xml "${_native}")
  string(REPLACE "&" "&amp;" _alias_xml "${_alias}")
  string(APPEND _mufpen_font_entries "    <file alias=\"${_alias_xml}\">${_native_xml}</file>\n")
endforeach()
add_custom_target(mufpen_theme_fonts DEPENDS ${MUFPEN_THEME_NATIVE_FONTS})

set(MUFPEN_THEMES_QRC "${CMAKE_CURRENT_BINARY_DIR}/themes.qrc")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/themes.qrc")
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/src/themes.qrc" _mufpen_theme_qrc)
string(REPLACE "&" "&amp;" _mufpen_resources_xml "${CMAKE_CURRENT_SOURCE_DIR}/resources/")
string(REPLACE "../resources/" "${_mufpen_resources_xml}" _mufpen_theme_qrc "${_mufpen_theme_qrc}")
string(REPLACE "  </qresource>" "${_mufpen_font_entries}  </qresource>" _mufpen_theme_qrc "${_mufpen_theme_qrc}")
file(CONFIGURE OUTPUT "${MUFPEN_THEMES_QRC}" CONTENT "${_mufpen_theme_qrc}" @ONLY)

function(mufpen_use_theme_fonts target)
  add_dependencies(${target} mufpen_theme_fonts)
  set_property(TARGET ${target} APPEND PROPERTY AUTOGEN_TARGET_DEPENDS mufpen_theme_fonts)
endfunction()
