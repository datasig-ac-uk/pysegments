import os
from glob import glob as _glob

from setuptools import setup, Extension


def glob(*parts):
    return _glob(os.path.join(*parts))


segments_ext = Extension(
    "pysegments._segments",
    language = "c++",
    sources = glob("src", "*.cpp"),
    depends = glob("src", "*.h"),
    include_dirs = ["Include"],
    extra_compile_args=["-std=c++11"]
)

with open("README.md", "rt", encoding="utf-8") as f:
    LONG_DESCRIPTION = f.read()

setup(
    name="pysegments",
    author="Sam Morley",
    author_email="Sam.Morley@maths.ox.ac.uk",
    version="0.1",
    description="Tools for performing dyadic segmentation of data.",
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    package_dir={"pysegments": "pysegments"},
    ext_modules=[
        segments_ext,
    ],
    python_requires=">=3.5",
    tests_require=["pytest"],
    test_suite="pysegments/tests"


)