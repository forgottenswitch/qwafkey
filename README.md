Keyboard remapper

To build, a mingw cross-comiler is needed.
Then pass its name as $CC, and its windres as $WINDRES:
```sh
CROSS=i686-w64-mingw32-
make CC=${CROSS}gcc WINDRES=${CROSS}windres
```

Or just `make` in a MSYS2's "Mingw-w64 Win32 shell" (or Win64 one).
