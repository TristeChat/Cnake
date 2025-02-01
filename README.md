# Cnake

## Overview
Classic 2d snake game written in C.

## Features 
- TODO: Enable color mode.
- Replay by clicking r or R.

## Dependencies
Requires libncurses to work.
To installed,
On arch linux:
```bash
pacman -S ncurses
```
On windows, using MSYS:
```bash
pacman -S mingw-w64-x86_64-ncurses
```

## Installation
Make sure you have gcc installed, if you have a different compiler edit the Makefile before running `make`.
```bash
mkdir Cnake && cd $_
git clone https://github.com/TristeChat/Cnake.git
cd Cnake
make
```

## Notes
Avoid manually deleting the binary file, use `make clean` to avoid mistakes. \
Debugging flags are available with `make debug`.
