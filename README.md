Keyboard remapper for Windows.

Prerequisites
-------------
- make
- mingw cross-compiler

On Cygwin:
- Launch setup.exe, and install `mingw64-i686-gcc-g++`

On MSYS2:
- Run `pacman -S mingw-w64-cross-{gcc,crt-git}`

On Linux:
- Install whatever the mingw cross-compiler package is.

On MSYS2 in Mingw-w64 Win32 shell:
- Run `pacman -S mingw-w64-i686-gcc`
- Build as `make CROSS32= 32`

On MSYS2 in Mingw-w64 Win64 shell:
- Run `pacman -S mingw-w64-x86_64-gcc`
- Build as `make CROSS64= 64`
