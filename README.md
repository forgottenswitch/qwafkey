QwafKey
=======

Implements [qwaf](https://github.com/forgottenswitch/qwaf) for Windows.

Requirements
------------
[MSys2](https://msys2.github.io) with `make`, `windres`, and `mingw32-i686-gcc` installed.

Running
-------
Do `make`, then run `qwafkey32.exe`.
`-cui` version adds debugging output to console window, `-con` also omits the tray icon.

Bugs
----
Makefile does not handle `-j`.
