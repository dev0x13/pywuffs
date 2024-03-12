# pywuffs: Python bindings for Wuffs the Library

[![Linux CI](https://github.com/dev0x13/pywuffs/actions/workflows/linux.yml/badge.svg)](https://github.com/dev0x13/pywuffs/actions/workflows/linux.yml)
[![Windows CI](https://github.com/dev0x13/pywuffs/actions/workflows/windows.yml/badge.svg)](https://github.com/dev0x13/pywuffs/actions/workflows/windows.yml)
[![macOS CI](https://github.com/dev0x13/pywuffs/actions/workflows/macos.yml/badge.svg)](https://github.com/dev0x13/pywuffs/actions/workflows/macos.yml)
[![Downloads](https://static.pepy.tech/badge/pywuffs)](https://pepy.tech/project/pywuffs)

This project is intended to enable using [Wuffs the Library](https://github.com/google/wuffs) from Python code. For now,
it only provides bindings for image and JSON decoding parts of
the [Auxiliary C++ API](https://github.com/google/wuffs/blob/main/doc/note/auxiliary-code.md) as being of the most
interest since it provides for "ridiculously fast" decoding of images of some types.

Current version of Wuffs library used in this project is **unsupported snapshot** taken from
[this](https://github.com/google/wuffs/commit/13c72db3508d33b9416a22a0ab8a8d4d8d5cd7be) commit. The primary
rationale for using the snapshot version instead of a stable release is that it provides JPEG decoder.

## Installation

### Using pip

```bash
python3 -m pip install pywuffs
```

### Using CMake

CMake build support is mostly intended for development purposes, so the process might be
not so smooth.

Building the Python module using CMake requires `pybind11` library to be installed in the
system, for example in Ubuntu it can be installed like this:

```bash
sudo apt install pybind11-dev
```

#### Linux

```bash
mkdir _build && cd _build
cmake ..
make
```

#### Windows

```shell
mkdir _build && cd _build
cmake -A x64 ..
cmake --build .
```

## API reference

API documentation is available at https://pywuffs.readthedocs.io. 

## Implementation goals

1. Bindings are supposed to be as close as possible to the original C and C++ Wuffs API. The differences are only
   justified when it's hardly possible to transfer the API entries to Python as is.
2. Bindings are not supposed to add much overhead. Because of that some parts of the API are not as convenient as they
   expected to be.

## Roadmap

1. Bindings for other parts of `wuffs_aux` API (CBOR decoding).
2. Bindings for the C API of Wuffs the Library.
