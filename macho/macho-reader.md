# Mach-O Reader

A minimal header parser for Mach-O binaries on macOS, including support for FAT (multi-architecture) binaries.

## Overview

* Detect FAT vs Mach-O using the first 4 bytes (magic number).
* For FAT binaries, read `fat_header` and `fat_arch[]`, choose an architecture, seek to its offset.
* Read the Mach-O header at that offset and display basic fields (cputype, filetype, ncmds, flags).
* Handle byte-swapping when magic indicates different endianness.

This parser is educational and demonstrates proper header parsing without assumptions about 64-bit only or fixed offsets.