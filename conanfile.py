from conan import ConanFile
from conan.tools.cmake import cmake_layout


class MuffinDependencies(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"
    default_options = {
        "qt/*:shared": True,
        "qt/*:widgets": True,
        "qt/*:gui": True,
        "qt/*:qtsvg": True,
        "qt/*:qtdeclarative": False,
        "qt/*:with_odbc": False,
        "qt/*:with_pq": False,
        "icu/*:shared": False,
    }

    def configure(self):
        if self.settings.os != "Windows":
            # CMake deploys ICU's matching official .dat and empty data stub.
            # Keep Conan from exporting ICU_DATA process-wide, which would
            # also redirect Apple's incompatible system ICU.
            self.options["icu/*"].data_packaging = "static"
        if self.settings.os == "Linux":
            # The bundled fcitx5 input-method plugin (cmake/MuffinFcitx5Qt.cmake) talks to
            # fcitx5 over QtDBus, and the recipe ships Qt without DBus by default. Without
            # this the plugin cannot be built at all (Qt6::DBus target missing).
            self.options["qt/*"].with_dbus = True

    def requirements(self):
        self.requires("qt/6.11.1")
        # Qt's recipe pins 10.42. Use one PCRE2 version throughout the graph,
        # including Qt/GLib and Muffin's native workspace matcher.
        self.requires("pcre2/10.49", force=True)
        # qt/6.11.1 pins brotli/1.1.0 while freetype/2.13.2 requires brotli >= 1.1.1, so the
        # graph is unsatisfiable as soon as conancenter bumps either recipe. Brotli keeps its
        # 1.x API, so pin a single copy for Qt, FreeType and Qt's own WOFF path instead of
        # turning one side's brotli support off.
        self.requires("brotli/1.2.1", force=True)
        # Qt 6.11's QTextMarkdownImporter static_asserts its DialectGitHub flags against the
        # md4c 0.5 layout, but the recipe accepts md4c/[>=0.4.8 <1] and now resolves 0.6.0,
        # whose new admonition/footnote flags change MD_DIALECT_GITHUB and break the build.
        # Pin 0.5.3 (its flags match Qt's enum exactly) instead of disabling with_md4c: that
        # would turn Qt's textmarkdownreader feature off and remove
        # QTextDocument::setMarkdown, which HelpViewerDialog uses.
        self.requires("md4c/0.5.3", force=True)
        # Direct requirement also emits the config needed by Qt's dependency.
        self.requires("harfbuzz/12.3.0")
        self.requires("freetype/2.13.2")
        self.requires("icu/74.2")
        self.requires("libwebp/1.6.0")
        self.requires("dav1d/1.5.3")
        self.requires("libjpeg/9f")
        self.requires("libpng/1.6.51")
        self.requires("lexbor/2.6.0")
        self.requires("yoga/3.2.0")
        self.requires("yaml-cpp/0.9.0")

    def layout(self):
        cmake_layout(self)
