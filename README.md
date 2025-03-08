# PONG!

This is just my experiment make game using LOVE2D but the logic of the game entirely written in C. This approach deppend in ffi from LuaJIT, so if your LOVE2D Doesn't have LuaJIT enabled you can't run this.

#### use LOVE2D 11.5 is recomended.

## Runing The Game

### Windows
You need mingw from msys2, you can download [msys2](https://www.msys2.org/), and [update pacman](https://www.msys2.org/docs/updating/):
```bash
pacman -Suy
```
install [mingw toolchain](https://packages.msys2.org/groups/mingw-w64-x86_64-toolchain):
```bash
pacman -S mingw-w64-x86_64-toolchain
```
add to environment path your "C:\msys2\mingw64\bin" and "C:\msys2\usr\bin".
Clone or Download this directory.

compile the C library:
```bash
gcc -shared -fPIC -o libs\libtest.so test.c -lwinmm
```
FINALLY just run LOVE2D at current folder (the repo).

### Linux

install love and gcc.
compile C library:
```bash
gcc -shared -fPIC -o libs/libtest.so test.c -lm
```

and just run it:
```bash
love .
```

HAVE FUN!
