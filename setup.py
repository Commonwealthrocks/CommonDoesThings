## setup.py
## last updated: 25/11/2025 <d/m/y>
## common-does-things
from setuptools import setup, Extension
import sys
import os
extra_compile_args = []
extra_link_args = []
libraries = []
if sys.platform == "win32":
    libraries = ["advapi32"]
    if "MSC" in sys.version:
        extra_compile_args = ["/W3"]
    else:
        extra_compile_args = ["-Wall"]
else:
    extra_compile_args = ["-Wall", "-Wextra"]
c_module = Extension(
    "commondoesthings.c",
    sources=[
        "commondoesthings/src/module.c",
        "commondoesthings/src/c_run.c",
        "commondoesthings/src/c_bytes.c",
        "commondoesthings/src/c_random.c",
        "commondoesthings/src/c_file_io.c",
        "commondoesthings/src/_compiler_based.c",
        "commondoesthings/src/c_docs.c",
        "commondoesthings/src/c_rm.c",
        "commondoesthings/src/c_con_time.c",],
    include_dirs=["commondoesthings/src"],
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
    libraries=libraries,)

setup(
    ext_modules=[c_module],
    packages=["commondoesthings"],
    package_dir={"commondoesthings": "commondoesthings"},)

## end