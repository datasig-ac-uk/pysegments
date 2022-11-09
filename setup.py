import io
from skbuild import setup

with io.open("README.md", "rt", encoding="utf-8") as f:
    LONG_DESCRIPTION = f.read()

with io.open("CHANGELOG", "rt", encoding="utf-8") as f:
    CHANGELOG = f.read()

LONG_DESCRIPTION += "\n\n\n##Changelog\n" + CHANGELOG

CMAKE_SETTINGS = [
    "-DSEGMENTS_PYTHON_MODULE:BOOL=ON"
]

CLASSIFIERS = [
    "Development Status :: 4 - Beta",
    "Intented Audience :: Developers",
    "Programming Language :: Python :: 3",
    "Programming Language :: Python :: 3,5",
    "Programming Language :: Python :: 3.6",
    "Programming Language :: Python :: 3.7",
    "Programming Language :: Python :: 3.8",
    "Programming Language :: Python :: 3.9",
    "Programming Language :: Python :: 3.10",
    "Programming Language :: Python :: 3.11",
    "Topic :: Scientific/Engineering :: Mathematics",
]


setup(
    name="pysegments",
    author="Sam Morley",
    author_email="Sam.Morley@maths.ox.ac.uk",
    version="0.2,
    description="Tools for performing dyadic segmentation of data.",
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    packages=["pysegments"],
    package_dir={"pysegments": "pysegments"},
    cmake_args=CMAKE_SETTINGS,
    python_requires=">=3.5",
    tests_require=["pytest"],
    test_suite="pysegments/tests",
    classifiers=CLASSIFIERS
)
