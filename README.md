NDManager Plugins
=================

NDManager Plugins include a number of scripts and programs to automatically pre-process files recorded with data acquisition systems: file conversion (from vendor-specific, proprietary formats to open formats used by Klusters, Neuroscope and NDManager), channel resampling and reordering, high-pass filtering and spike detection, waveform feature extraction (PCA) for subsequent spike sorting, video transcoding and LED tracking, etc.

Developed by Lynn Hazan and Michaël Zugaro, distributed under the GNU General Public License v3 or later.

The bundled third-party sources (libav 11.2 in `src/process_extractleds`, libsamplerate 0.1.8 in
`src/process_resample`) keep their own licences.
