QwafKey
=======

Implements [qwaf](https://github.com/forgottenswitch/qwaf) for Windows.
Also implements X11 Compose.

Requirements
------------
- [Msys2](https://msys2.github.io) with `make`, `windres`, and `mingw-w64-i686-gcc` installed.
- Windows 2000 or higher.

Running
-------
Run `make fetch` once to download the necessary X11 files.
In Mingw32 shell, do `make`, then run `qwafkey32.exe`.
`-cui` version adds debugging output to console window, `-con` also omits the tray icon.

Limitations
-----------
Does not work in Administrator applications unless ran as one.
