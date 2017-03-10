Keyboard remapper.

To build, a mingw cross-compiler is needed.

By default, a Console-with-GUI (`cui`) flavour is built.  "Console" here means
that real and generated key presses and releases would be printed to a console
window.  To build Console-without-GUI (`con`), or GUI-only (`gui`), pass the
flavours as arguments to the final `make` invocation below, as in:
```sh
make con cui gui
```

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
