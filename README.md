Keyboard remapper for Windows. Main use is to implement features needed for
[qwaf] keyboard layout:
- Vi-like editing keys (hold `;` and press `hjkl`)
- Compose key (press `,n` and then the combination, such as `..`)
- Latching modifiers (type `,` and then `j`; or hold `;`, type `s`, and then any letter)
- Work with multiple input languages/layouts

[qwaf]: https://github.com/forgottenswitch/qwaf

Limitations
-----------
Keyboard layouts need only be switched using the program's own combinations
(`,m` by default), so that it can notice this and switch the effective
remappings appropriately.

Would not work in privileged applications unless launched "as Administrator"
itself (but do this at your own risk).  Cannot work at all at password prompts.

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
