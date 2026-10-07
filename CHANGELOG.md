# Changelog

All notable changes to this project are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/).
Changes up to 1.4.14 are listed in `changelog-ndmanager-plugins.txt`.

## [3.0.0] - unreleased

Version numbers now follow the rest of Neurosuite.

### Added
- The 1.4.13 and 1.4.14 releases by Michaël Zugaro, previously only on SourceForge:
  `ndm_tsp2sts` for Amplipex video timestamps, a `picture` option in `spots2pos`, and higher
  time precision for events in `process_smrconvert`.

### Changed
- `process_extractleds` decodes video with the system FFmpeg libraries instead of a bundled,
  modified copy of libav 11.2's player. Output is unchanged; it no longer waits about 100 s
  after the last frame, and the live video display is gone (`-hide` is accepted and ignored).
  For full-range (MJPEG) video the colour averages can differ by less than one unit.
- `process_resample` uses the system libsamplerate instead of a bundled copy.
- `ndm_prepare` and `ndm_checkconsistency` run on Python 3 and PyQt6 (port started by
  Théotime de Charrin).
- `ndm_transcodevideo` uses `ffmpeg`/`ffprobe` instead of libav's `avconv`/`avprobe`.
- Scripts start with `#!/usr/bin/env bash`.
- New CMake build (3.16, C++17, GNUInstallDirs); manual pages and the Python tools are optional.
  MATLAB files are installed to `share/ndmanager-plugins/matlab`.
- Licence file corrected to GPL-3.0-or-later, matching the source headers.

### Fixed
- `process_extractleds` read past the frame while thresholding landscape videos.
- `process_smrconvert` printed file information through functions without a return value,
  which can crash with optimisation.
- `process_nlxconvert` and `process_smrconvert` build with C++17 (exception specifications).

### Removed
- KDE3/KDE4 service menus, bundled libav and libsamplerate, generated API documentation and the
  old packaging helpers.
