from collections import defaultdict
from glob import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension
from setuptools.command.build_ext import build_ext

UNIX_BUILD_ARGS = ["-O3", "-g0", "-s", "-fvisibility=hidden", "-flto", "-fno-fat-lto-objects"]
BUILD_ARGS = defaultdict(lambda: UNIX_BUILD_ARGS)
BUILD_ARGS["msvc"] = ["/O3", "/DNDEBUG", "/arch:AVX"]
BUILD_ARGS["unix"] = UNIX_BUILD_ARGS

UNIX_LINK_ARGS = ["-Wl,--strip-all", "-flto", "-fno-fat-lto-objects"]
LINK_ARGS = defaultdict(lambda: UNIX_LINK_ARGS)
LINK_ARGS["msvc"] = []
LINK_ARGS["unix"] = UNIX_LINK_ARGS


class CustomBuildExt(build_ext):
    def build_extensions(self):
        compiler = self.compiler.compiler_type
        build_args = BUILD_ARGS[compiler]
        link_args = LINK_ARGS[compiler]
        for ext in self.extensions:
            ext.extra_link_args = link_args
            ext.extra_compile_args = build_args
        build_ext.build_extensions(self)


ext_modules = [
    Pybind11Extension(
        "pywuffs",
        glob("src/*.cpp"),
        include_dirs=["libs/wuffs-mirror-release-c/release"]
    ),
]

setup(name="pywuffs",
      version="1.0",
      description="Python bindings for Wuffs the Library",
      author="Georgiy Manuilov",
      url="https://github.com/dev0x13/pywuffs",
      cmdclass={"build_ext": CustomBuildExt},
      ext_modules=ext_modules)
