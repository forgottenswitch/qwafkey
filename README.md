Keyboard remapper.

To build, a mingw cross-compiler is needed.

Cygwin:
```sh
# Install make and mingw64-i686-gcc-g++
make clean
make
```

MSYS2:
```sh
pacman -S make mingw-w64-cross-{gcc,crt-git}
make clean
make
```

MSYS2, Mingw-w64 Win32 shell:
```sh
pacman -S make mingw-w64-i686-gcc
make clean
make CROSS=
```
