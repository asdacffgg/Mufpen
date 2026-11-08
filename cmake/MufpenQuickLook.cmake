# Modern Finder previews are executable .appex bundles, not legacy .qlgenerator
# plug-ins. The preview links only the existing static Markdown parser + AppKit.
enable_language(OBJCXX)
find_library(MUFPEN_APPKIT_FRAMEWORK AppKit REQUIRED)
find_library(MUFPEN_QUICKLOOK_FRAMEWORK QuickLookUI REQUIRED)
find_program(MUFPEN_CODESIGN_EXECUTABLE codesign REQUIRED)

add_library(MufpenQuickLookText STATIC src/quicklook/MarkdownPreview.mm)
set_target_properties(MufpenQuickLookText PROPERTIES
  AUTOMOC OFF AUTOUIC OFF AUTORCC OFF OBJCXX_STANDARD 20 OBJCXX_STANDARD_REQUIRED YES)
target_compile_options(MufpenQuickLookText PRIVATE -fobjc-arc -fapplication-extension)
target_include_directories(MufpenQuickLookText PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src"
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/cmark-gfm/src"
          "${CMAKE_CURRENT_SOURCE_DIR}/third_party/cmark-gfm/extensions"
          "${CMAKE_CURRENT_BINARY_DIR}/third_party/cmark-gfm/src")
target_link_libraries(MufpenQuickLookText PUBLIC "${MUFPEN_APPKIT_FRAMEWORK}"
  PRIVATE libcmark-gfm-extensions_static libcmark-gfm_static)

add_executable(MufpenQuickLook MACOSX_BUNDLE src/quicklook/PreviewController.mm)
set_target_properties(MufpenQuickLook PROPERTIES
  AUTOMOC OFF AUTOUIC OFF AUTORCC OFF OBJCXX_STANDARD 20 OBJCXX_STANDARD_REQUIRED YES
  BUNDLE_EXTENSION appex
  MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QuickLook-Info.plist.in"
  XCODE_ATTRIBUTE_APPLICATION_EXTENSION_API_ONLY YES
  XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO)
target_compile_options(MufpenQuickLook PRIVATE -fobjc-arc -fapplication-extension)
target_link_options(MufpenQuickLook PRIVATE -fapplication-extension "LINKER:-e,_NSExtensionMain")
target_link_libraries(MufpenQuickLook PRIVATE MufpenQuickLookText "${MUFPEN_QUICKLOOK_FRAMEWORK}")
add_custom_command(TARGET MufpenQuickLook POST_BUILD
  COMMAND "${MUFPEN_CODESIGN_EXECUTABLE}" --force --sign - --timestamp=none
    --entitlements "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QuickLook.entitlements"
    "$<TARGET_BUNDLE_DIR:MufpenQuickLook>"
  VERBATIM)

# This target also runs when only the extension changes and the host executable
# does not need relinking. Both the default build and dist embed the fresh copy.
add_custom_target(mufpen-quicklook-embed ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_CONTENT_DIR:Mufpen>/PlugIns"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_BUNDLE_DIR:MufpenQuickLook>"
    "$<TARGET_BUNDLE_CONTENT_DIR:Mufpen>/PlugIns/MufpenQuickLook.appex"
  DEPENDS Mufpen MufpenQuickLook
  VERBATIM)

function(mufpen_add_quicklook_tests)
  add_executable(MufpenQuickLookTest tests/quicklook/MarkdownPreviewTest.mm src/quicklook/PreviewController.mm)
  set_target_properties(MufpenQuickLookTest PROPERTIES
    AUTOMOC OFF AUTOUIC OFF AUTORCC OFF OBJCXX_STANDARD 20 OBJCXX_STANDARD_REQUIRED YES)
  target_compile_options(MufpenQuickLookTest PRIVATE -fobjc-arc)
  target_link_libraries(MufpenQuickLookTest PRIVATE MufpenQuickLookText "${MUFPEN_QUICKLOOK_FRAMEWORK}")
  add_test(NAME MufpenQuickLookTest COMMAND MufpenQuickLookTest)
  set_tests_properties(MufpenQuickLookTest PROPERTIES RESOURCE_LOCK MufpenQtGui)
endfunction()
