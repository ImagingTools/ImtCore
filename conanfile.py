#
# For using sparse checkout from the local repo set
# git config --global uploadpack.allowFilter true
#

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.env import Environment, VirtualBuildEnv, VirtualRunEnv
from conan.tools.build import can_run
from conan.tools.files import collect_libs, patch, rmdir, update_conandata, move_folder_contents
from conan.tools.scm import Git
from conan.errors import ConanInvalidConfiguration, ConanException
from conans.tools import SVN  # TODO: no svn in conan v2
import os
import sys
import shutil
from pathlib import Path
import json


class ImtCoreConan(ConanFile):
    name = "ImtCore"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "qt_package": ["system", "conan"],
        "qt_version": "ANY",
        "USE_QT3D": [True, False],
        "USE_QTDB": [True, False],
        "USE_QTQML": [True, False],
        "BUILD_TESTING": [True, False]
    }

    default_options = {
        "qt_package": "conan",
        "qt_version": None,
        "USE_QT3D": True,
        "USE_QTDB": True,
        "USE_QTQML": True,
        "BUILD_TESTING": False
    }

    description = "ImagingTools Core Framework"
    url = "http://b035a0a.online-server.cloud/svn/ImtReleases/ImtCore"
    license = "LGPL"
    author = "ImagingTools"
    topics = None
    short_paths = True
    exports_sources = ["patches/*"]
    no_copy_source = False
    generators = "CMakeDeps"
    python_requires = "conantools/0.1.1@gmg/stable"

    @property
    def _gmgtools(self):
        return self.python_requires["conantools"].module

    def set_version(self):
        if self.version is not None:
            self.output.warning(f'Version overriden')
            return self.version
        git = Git(self)
        try:
            self.version = 'git.' + git.get_commit()[:8]
            if git.is_dirty():
                self.version += '-dirty'
            self.output.info(f'Detected version {self.version}')
        except ConanException:
            self.output.error('Unable to detect version')
            self.version = 'unknown'

    def export(self):
        git = Git(self)
        url, commit = git.get_url_and_commit()
        subfolder = self.recipe_path.relative_to(Path(git.get_repo_root())).as_posix()
        # we store the current url and commit in conandata.yml
        update_conandata(self, {"sources": {"commit": commit, "url": url, "subfolder": subfolder}})

    def validate(self):
        if self.settings.arch == "x86":
            raise ConanInvalidConfiguration("Not supported arch")

    def requirements(self):
        if self.options.qt_package == "conan":
            self.requires("qt/6.2.4-r2@gmg/system")

        self.requires("quazip/[~1]@gmg/stable")
        self.requires("openssl/1.1.1u")
        self.requires("AcfPublic/[>=1.2.480-r0 <1.3.0]@gmg/stable", include_prerelease=True)
        self.requires("zlib/1.2.11-r1@gmg/stable", override=True)

    def build_requirements(self):
        if self.settings.os == "Linux":
            self.build_requires("patchelf/0.13")

    def configure(self):
        if self.options.qt_package == "system":
            self.options.qt_version = self._gmgtools.qmake_query("QT_VERSION")
            self.output.info(f"Auto-detected Qt version: {self.options.qt_version}")
        else:
            del self.options.qt_version

    def layout(self):
        # FIXME: ImtCore cmake files done wrong and expect sources to be located in the ImtCore subdirectory
        cmake_layout(self, build_folder='out', src_folder='.', generator='Ninja')
        for layoutData in (self.layouts.source, self.layouts.package):
            layoutData.buildenv_info.define_path("IMTCOREDIR", ".")

        self.layouts.build.buildenv_info.define_path("IMTCOREDIR_BUILD", ".")

    def source(self):
        git = Git(self)
        sources = self.conan_data["sources"]
        url = sources["url"]
        commit = sources["commit"]
        subfolder = sources["subfolder"]
        git.run('init')
        git.run(f'remote add origin "{url}"')
        git.run(f'fetch --depth=1 --filter=tree:0 origin "{commit}"')
        git.run(f'sparse-checkout set "{subfolder}"')
        git.checkout(commit)

        # move_folder_contents works only one level down, iterate through levels
        for sub in os.path.split(subfolder):
            self.output.info(f'moving {sub} -> {self.source_folder}')
            move_folder_contents(self, sub, self.source_folder)

    def _print_env(self, env):
        for variable, value in env.items():
            self.output.info(f'{variable}={value}')

    def _write_env_to_presets(self, env):
        presetsPath = self.generators_path / 'CMakePresets.json'
        with open(presetsPath) as f:
            data = json.load(f)
        for conf in data['configurePresets']:
            if conf['name'].endswith(str(self.settings.build_type).lower()):
                if 'environment' not in conf:
                    conf['environment'] = {}
                for var, val in env.items():
                    conf['environment'][var] = val
        with open(presetsPath, 'w') as f:
            json.dump(data, f, indent=2)

    def _get_qt_version(self):
        if self.options.qt_package == "system":
            return self.options.qt_version
        else:
            return str(self.deps_cpp_info["qt"].version)

    def generate(self):
        if self.options.qt_package == "conan":
            qtDir = str(self.deps_cpp_info["qt"].rootpath)
        else:
            qtDir = self._gmgtools.detect_qtdir(self)
        self.output.info(f"QTDIR: {qtDir}")

        tc = CMakeToolchain(self, generator='Ninja')
        tc.user_presets_path = 'Build/CMake'
        tc.variables["CMAKE_FIND_PACKAGE_PREFER_CONFIG"] = True
        tc.variables["ENABLE_TESTS_IMTCORE"] = False
        tc.variables["USE_QT3D"] = self.options.USE_QT3D
        tc.variables["USE_QTDB"] = self.options.USE_QTDB
        tc.variables["USE_QTQML"] = self.options.USE_QTQML
        tc.variables["BUILD_TESTING"] = self.options.BUILD_TESTING
        tc.variables["USE_EJ"] = False
        tc.variables["USE_GEO"] = False
        tc.variables["USE_FIND_PACKAGE"] = True
        tc.variables["CMAKE_CXX_STANDARD"] = 17
        tc.variables['BUILDDIR'] = self.build_path.as_posix()
        tc.variables['IMTCOREDIR_BUILD'] = self.build_path.as_posix()
        tc.variables['PYTHONEXE'] = Path(sys.executable).as_posix()
        # fvisibility hidden
        tc.variables['CMAKE_POLICY_DEFAULT_CMP0063'] = 'NEW'
        tc.variables['CMAKE_VISIBILITY_INLINES_HIDDEN'] = True
        tc.variables['CMAKE_CXX_VISIBILITY_PRESET'] = 'hidden'
        tc.generate()

        env = Environment()
        env.define('IMTCOREDIR', self.source_folder)
        env.define('QTDIR', qtDir)
        env = env.vars(self, scope="build")
        env.save_script("acfenv")

        buildEnv = VirtualBuildEnv(self)
        buildEnv.generate()
        self._print_env(buildEnv.environment().vars(self))

        env = buildEnv.environment().compose_env(env).vars(self)
        self._write_env_to_presets({
            var: val for var, val in env.items()
            if var.startswith('ACF') or var.startswith('IACF') or var.startswith('IMTCORE')})

        # AcfPublic shall be both build_requires() and requires() dependency because of the Arxc compiler tool
        # However, we want to make things simple and use only requires()
        if not can_run(self):
            self.output.error("Cross compiling may not be able to run Arxc")
        if self.settings.os == "Linux":
            # On win and mac Acf tools are bundled with deployqt tool and do not require linker env variables to locate dependencies
            # On Linux we use VirtualRunEnv at build stage. This does not work with cross compiling
            runEnv = VirtualRunEnv(self)
            runEnv.generate(scope="build")

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_script_folder='Build/CMake')
        cmake.build()

    def package_id(self):
        # AcfPublic may break binary compatibility with any change in the version
        self.info.requires['AcfPublic'].full_version_mode()
        # qt may break binary compatibility with any change in the version
        # because of patches or repackaging
        self.info.requires['qt'].full_version_mode()

    def _build_folder_suffix(self, binary=True):
        # folder naming in acf and imt
        if self.settings.build_type == 'Release':
            build_type = 'Release'
        else:
            build_type = 'Debug'

        qt_major = self._get_qt_version().split(".")[0]

        if self.settings.os == 'Windows':
            if self.settings.compiler == 'Visual Studio':
                compiler = 'VC' + str(self.settings.compiler.version)
            else:
                compiler = 'Clang'
        elif self.settings.os == 'Linux':
            if self.settings.compiler == 'gcc':
                compiler = 'GCCLinux'
            else:
                compiler = 'ClangLinux'
        else:
            compiler = 'ClangOSX'

        if self.settings.arch == 'x86_64':
            arch = 'x64'
        elif self.settings.arch == 'armv8':
            arch = 'arm64'
        else:
            arch = 'x86'

        if binary:
            return f'{build_type}_Qt{qt_major}_{compiler}_{arch}'
        else:
            return f'Qt{qt_major}_{compiler}_{arch}'

    def _include_folder_suffix(self):
        return self._build_folder_suffix(binary=False)

    def package(self):
        source_subfolder = self.source_folder

        self.copy("*.*", dst="Config", src=os.path.join(source_subfolder, "Config"))
        self.copy("*.*", dst="Partitura", src=os.path.join(source_subfolder, "Partitura"))
        self.copy("*.*", dst="Sdl", src=os.path.join(source_subfolder, "Sdl"))
        self.copy("*.h", dst="Include", src=os.path.join(source_subfolder, "Include"))
        self.copy("*.svg", dst="Include", src=os.path.join(source_subfolder, "Include"))
        self.copy("*.css", dst="Include", src=os.path.join(source_subfolder, "Include"))
        self.copy("*.theme", dst="Include", src=os.path.join(source_subfolder, "Include"))
        self.copy("*.cpp", dst="Include", src=os.path.join(source_subfolder, "Include"))
        self.copy("*.h", dst="Impl", src=os.path.join(source_subfolder, "Impl"))

        self.copy("*.h", dst="AuxInclude", src=os.path.join(source_subfolder, "AuxInclude"))
        self.copy("*.cpp", dst="AuxInclude", src=os.path.join(source_subfolder, "AuxInclude"))
        self.copy("*.qml", dst="AuxInclude", src=os.path.join(source_subfolder, "AuxInclude"))
        self.copy("*.qrc", dst="AuxInclude", src=os.path.join(source_subfolder, "AuxInclude"))
        self.copy("qmldir", dst="AuxInclude", src=os.path.join(source_subfolder, "AuxInclude"))
        self.copy("*.lib", dst="Lib", src=os.path.join(source_subfolder, "Lib"))
        self.copy(
            "*.pdb", dst="Lib/" + self._build_folder_suffix(),
            src=os.path.join(source_subfolder, "AuxInclude"), keep_path=False)
        self.copy("*.pdb", dst="Lib", src=os.path.join(source_subfolder, "Lib"))
        self.copy("*.a", dst="Lib", src=os.path.join(source_subfolder, "Lib"))
        self.copy("*.exe", dst="Bin", src=os.path.join(source_subfolder, "Bin"))
        self.copy("*.dll", dst="Bin", src=os.path.join(source_subfolder, "Bin"))
        self.copy("*.pdb", dst="Bin", src=os.path.join(source_subfolder, "Bin"))
        self.copy("*.arp", dst="Bin", src=os.path.join(source_subfolder, "Bin"))
        self.copy("*DesignTokenCreator", dst="Bin", src=os.path.join(source_subfolder, "Bin"))
        self.copy("*DdlCodeCreator", dst="Bin", src=os.path.join(source_subfolder, "Bin"))
        self.copy("*SdlCodeGenerator", dst="Bin", src=os.path.join(source_subfolder, "Bin"))

        # copy MAC OS bundles
        self.copy("*.app/*", dst="Bin", src=os.path.join(source_subfolder, "Bin"))

        if self.settings.os == 'Linux':
            from shlex import join
            bin_subdir = self.package_path / 'Bin' / self._build_folder_suffix()
            self.output.info(f"Processing: {bin_subdir}")
            assert os.path.isdir(bin_subdir)
            # installation is done not with cmake, remove rpath manually
            for file in bin_subdir.iterdir():
                self.output.info(f"Removing rpath from {file.name}")
                self.run(join(["patchelf", "--remove-rpath", str(file)]))

        self.copy("*", dst="Qml", src=os.path.join(source_subfolder, "Qml"))
        self.copy("*", dst="3rdParty", src=os.path.join(source_subfolder, "3rdParty"))
        self.copy("*", dst="Install", src=os.path.join(source_subfolder, "Install"))
        self.copy("*", dst="Include/imtstylecontrolsqml", src=os.path.join(source_subfolder, "Include/imtstylecontrolsqml"))

    def _collect_libs(self):
        if self.package_folder is not None:
            return collect_libs(self)

        # In editable mode with conan v2 layout() package_folder is None
        # and we need to implement our own search
        libs = set()
        for libdir in self.cpp_info.libdirs:
            for ext in ('*.a', '*.so', '*.lib', '*.dylib'):
                libPaths = Path(self.build_path / libdir).glob(ext)
                for lib in libPaths:
                    lib = lib.stem
                    if lib[:3] == 'lib' and ext != '*.lib':
                        lib = lib[3:]
                    libs.add(lib)
        return list(libs)

    def package_info(self):
        #self.cpp_info.srcdirs = ["."]

        self.cpp_info.includedirs = [os.path.join("AuxInclude", self._include_folder_suffix()), "Include", "Impl"]
        self.cpp_info.libdirs = [os.path.join("Lib", self._build_folder_suffix())]
        self.cpp_info.libs = self._collect_libs()

        # To support editable mode we need explicitly assign same values to conan v2 fields,
        # probably limitation of conan 1.x when using v2 features
        # HACK: we call it in package_info() instead of layout() because deps_cpp_info is needed to calculate the directory name
        self.cpp.source.includedirs = ["Include", "Impl"]
        self.cpp.build.includedirs = [os.path.join("AuxInclude", self._include_folder_suffix())]
        self.cpp.build.libdirs = self.cpp_info.libdirs

        cmakeModules = [
            "Config/CMake/ImtCoreDesign.cmake",
            "Config/CMake/ImtCoreDesignWeb.cmake",
            "Config/CMake/WebCompiler.cmake",
            "Config/CMake/ImtSdlConfig.cmake",
            "Config/CMake/ImtCoreSdlCustomConfig.cmake",
            "Config/CMake/DdlCreator.cmake"]
        # modern v2 approach
        self.cpp.package.set_property("cmake_build_modules", cmakeModules)
        self.cpp.source.set_property("cmake_build_modules", cmakeModules)
        # old v1 approach
        self.cpp_info.build_modules["cmake_find_package"] = cmakeModules
        self.cpp_info.build_modules["cmake_find_package_multi"] = cmakeModules

        self.cpp_info.requires = ['quazip::quazip', 'openssl::openssl', 'AcfPublic::AcfPublic']

        qt_components = self.deps_cpp_info["qt"].components.keys()
        self.output.info(f"Qt components: {qt_components}")
        if len(qt_components) > 0:
            # some imt components require more qt libraries, but for conan generated cmake configs keep it minimal
            self.cpp_info.requires += ['qt::qtCore']
