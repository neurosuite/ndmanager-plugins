# NDManager Plugins

NDManager Plugins include a number of scripts and programs to automatically pre-process files
recorded with data acquisition systems: file conversion (from vendor-specific, proprietary
formats to the open formats used by Klusters, NeuroScope and NDManager), channel resampling and
reordering, high-pass filtering and spike detection, waveform feature extraction (PCA) for
subsequent spike sorting, video transcoding and LED tracking, etc.

Developed by Lynn Hazan and Michaël Zugaro, with contributions by Ken Harris (LED tracking),
Florian Franzen and Théotime de Charrin (Qt6/PyQt6 porting), distributed under the GNU General
Public License v3 or later (`process_extractleds` is LGPL-2.1-or-later).

## Building

Requires CMake 3.16+, C and C++17 compilers, GSL, libxml2, libsamplerate and the FFmpeg
libraries (libavformat, libavcodec, libswscale, libavutil). The Python tools need Python 3 and
PyQt6 (`pyuic6`); manual pages need `xsltproc` and the DocBook XSL stylesheets.

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build
```

On Ubuntu 24.04:

```bash
sudo apt install cmake ninja-build g++ libgsl-dev libxml2-dev libsamplerate0-dev \
  libavformat-dev libavcodec-dev libswscale-dev libavutil-dev pyqt6-dev-tools \
  xsltproc docbook-xsl
```

At run time the scripts need `bash`, `gawk` and `ffmpeg`, and the Python tools `python3-pyqt6`.
Use `-DWITH_PYTHON_TOOLS=OFF` or `-DWITH_MANPAGES=OFF` to skip those parts.
With Nix: `nix build` or `nix develop`. See [CHANGELOG.md](CHANGELOG.md) for changes.
