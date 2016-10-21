QwafKey
=======

Implements [qwaf](https://github.com/forgottenswitch/qwaf) for Windows.

Requirements
------------
- [MSys2](https://msys2.github.io) with `make`, `windres`, and `mingw-w64-i686-gcc` installed.
- Windows 2000 or higher.

Running
-------
In Mingw32 shell, do `make`, then run `qwafkey32.exe`.
`-cui` version adds debugging output to console window, `-con` also omits the tray icon.

Bugs
----
Makefile does not handle `-j`.
