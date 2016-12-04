vimkeys
=======

Implements [qwaf](https://github.com/forgottenswitch/qwaf) for Windows.
Also implements X11 Compose.

Requirements
------------
- [Msys2](https://msys2.github.io)
- Windows 2000 or higher.

Building
--------
In `Mingw-w64 Win32 Shell` window:
```sh
pacman -S make windres mingw-w64-i686-gcc
cd vimkeys
make fetch    # once to download the necessary Compose files
make
```

Running
-------
Run `vimkeys32`:
```sh
start vimkeys32
```
To autostart it at login, place a shortcut to it into `Start Menu -> All Programs -> Startup`.

`vimkeys32-cui` adds debugging output to console window;
`vimkeys32-con` also omits the tray icon.

Limitations
-----------
Does not work in Administrator applications unless ran as one.

Sometimes, needs to be restarted. Right-click the tray icon, and choose `Restart`.
