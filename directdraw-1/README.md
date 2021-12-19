This is a test case from [Jesse Allen](mailto:the3dfxdude@gmail.com), attached to the [Wine bug report 4009](https://bugs.winehq.org/show_bug.cgi?id=4009#c27) on 2006-08-19 23:39:44 CDT.

I patched it, so it uses current display mode instead of the old `640x480x8` one.

I build it using MSYS2 subsystem called "MinGW 32-bit":
```
hp@DESKTOP-O7JE7JE MINGW32 /c/Users/hp/Downloads/ddraw-test-2
$ /mingw32/bin/gcc --version
gcc.exe (Rev5, Built by MSYS2 project) 11.2.0
Copyright (C) 2021 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```
```
$ /mingw32/bin/gcc -Wall some.c -lgdi32 -lddraw -o some.exe
```
```
$ ls -Al
total 640
drwxr-xr-x 1 hp None      0 Dec 19 13:53 output
-rw-r--r-- 1 hp None   4707 Dec 19 17:57 some.c
-rwxr-xr-x 1 hp None 250261 Dec 19 18:01 some.exe
```
```
$ file some.exe
some.exe: PE32 executable (console) Intel 80386, for MS Windows
```
```
$ ldd some.exe
        ntdll.dll => /c/Windows/SYSTEM32/ntdll.dll (0x7ffc7cbb0000)
        ntdll.dll => /c/Windows/SysWOW64/ntdll.dll (0x77df0000)
        wow64.dll => /c/Windows/System32/wow64.dll (0x7ffc7c0a0000)
        wow64win.dll => /c/Windows/System32/wow64win.dll (0x7ffc7b1c0000)
```

The output of the app:
```
$ ./some.exe
OK EnumDisplaySettings()
devmode says: 1920x1080 (32 bpp)
OK RegisterClass()
OK CreateWindowEx()
OK ShowWindow, UpdateWindow, CreateWindowEx()
OK ShowWindow, UpdateWindow, DirectDrawCreate()
OK IDirectDraw_SetCooperativeLevel()
OK IDirectDraw_SetDisplayMode()
OK IDirectDraw_CreateSurface()
OK IDirectDraw_CreatePalette()
OK IDirectDrawSurface_SetPalette()
```
