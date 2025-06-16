from conan import ConanFile
from conan.tools.cmake import CMakeToolchain
from conan.tools.cmake import CMakeDeps


class TrogondbConanFile(ConanFile):
    name = 'trogondb'
    version = '1.0'
    settings = 'os', 'compiler', 'build_type', 'arch'

    def configure(self):
        pass

    def requirements(self):
        self.requires('fmt/11.1.3')
        self.requires('spdlog/1.15.1')
        self.requires('asio/1.34.2')
        self.requires('yaml-cpp/0.8.0')
        # self.requires('boost/1.88.0')

    def build_requirements(self):
        self.tool_requires('cmake/3.31.6')

    def layout(self):
        # cmake_layout(self)
        self.folders.source = 'src'
        self.folders.build = 'build'
        self.folders.generators = 'build'

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = False
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
