// 2021-12-19 saukrs saved from: https://bugs.winehq.org/show_bug.cgi?id=4009#c27

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdio.h>

static int quit = 0;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
	switch (uMsg) { 
	case WM_DESTROY:
	case WM_KEYDOWN: 
		quit = 1; 
		return 1;
	default: 
		return DefWindowProc(hwnd, uMsg, wParam, lParam); 
	} 
	return 0; 
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LPWSTR cmd_line;
	LPWSTR *arg_list;
	int i, arg_count;
	WNDCLASS wc;
	const char *WIN_CLASS_NAME = "foo";
	const char *WIN_TITLE = "foo2";
	int rc;
	HWND main_hwnd;
	HWND hwnd2;
	DEVMODE devmode;
	DWORD screen_x;
	DWORD screen_y;
	DWORD screen_bpp;

	BOOL OK;
	LPDIRECTDRAW dd_obj;
	DDSURFACEDESC dd_sd1;
	LPDIRECTDRAWSURFACE dd_buf1;
	PALETTEENTRY palette[256];
	LPDIRECTDRAWPALETTE whatever;
	int x;
	int y;
        BYTE *lpscreen;
	MSG msg;

	cmd_line = GetCommandLineW();
	arg_list = CommandLineToArgvW(cmd_line, &arg_count);

	for(i=0; i<arg_count; i++)
		printf("arg_list[%d] = %ls\n", i, arg_list[i]);

	wc.style          = CS_DBLCLKS;
	wc.lpfnWndProc    = (WNDPROC) MainWndProc;
	wc.cbClsExtra     = 0;
	wc.cbWndExtra     = 0;
	wc.hInstance      = hInstance;
	wc.hIcon          = NULL;
	wc.hCursor        = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName   = NULL;
	wc.lpszClassName  = WIN_CLASS_NAME;

	/* 2021-12-18 sskras: inspired by the following GPLv2-licensed snippet:
	 * https://github.com/masterfeizz/EDuke3D/blob/e29108b78c777d5f7814ff4543c1643b93c77d24/build/src/misc/enumdisplay.c#L26
	 * (no copy-pasting though) */

	ZeroMemory(&devmode, sizeof(devmode));
	devmode.dmSize = sizeof(DEVMODE);
	SetLastError(0xfaceabee);

	OK = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	if (!OK) {
		DWORD GLE = GetLastError();
		printf("NO EnumDisplaySettings(): GLE = 0x%lx\n", GLE);
		printf("devmode says: %lu x %lu x %lu\n", devmode.dmPelsWidth, devmode.dmPelsHeight, devmode.dmBitsPerPel);
		return 1;
	}
	printf("OK EnumDisplaySettings()\n");

	printf("Current display mode: %lux%lu (%lu bpp)\n", devmode.dmPelsWidth, devmode.dmPelsHeight, devmode.dmBitsPerPel);
	screen_x   = (arg_count > 1 && arg_list[1]) ? (_wtoi(arg_list[1])) : (devmode.dmPelsWidth);
	screen_y   = (arg_count > 2 && arg_list[2]) ? (_wtoi(arg_list[2])) : (devmode.dmPelsHeight);
	screen_bpp = (arg_count > 3 && arg_list[3]) ? (_wtoi(arg_list[3])) : (devmode.dmBitsPerPel);
	printf("GetSystemMetrics(SM_CXSCREEN)=%d\n", GetSystemMetrics(SM_CXSCREEN));
	printf("GetSystemMetrics(SM_CYSCREEN)=%d\n", GetSystemMetrics(SM_CYSCREEN));

	rc = RegisterClass(&wc);
	if (!rc)
		return 1;
	printf("OK RegisterClass()\n");

	main_hwnd = CreateWindowEx(0, WIN_CLASS_NAME, WIN_TITLE, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
	if (!main_hwnd)
		return 1;
	printf("OK CreateWindowEx()\n");

	ShowWindow(main_hwnd, nCmdShow);
	UpdateWindow(main_hwnd);

        hwnd2 = CreateWindowEx(WS_EX_CONTROLPARENT, WIN_CLASS_NAME, WIN_TITLE, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), main_hwnd, NULL, hInstance, NULL);
        if (!hwnd2)
                return 1;
	printf("OK ShowWindow, UpdateWindow, CreateWindowEx()\n");

        ShowWindow(hwnd2, nCmdShow);
        UpdateWindow(hwnd2);

	rc = DirectDrawCreate(NULL, &dd_obj, NULL);
	if (rc != DD_OK) {
		return 1;
	}
	printf("OK ShowWindow, UpdateWindow, DirectDrawCreate()\n");

	rc = IDirectDraw_SetCooperativeLevel(dd_obj, main_hwnd, DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
	if (rc != DD_OK) {
		IDirectDraw_Release(dd_obj);
		return 1;
	}
	printf("OK IDirectDraw_SetCooperativeLevel()\n");
	printf("Switching to display mode: %lux%lu (%lu bpp)\n", screen_x, screen_y, screen_bpp);

	rc = IDirectDraw_SetDisplayMode(dd_obj, screen_x, screen_y, screen_bpp);
	if (rc != DD_OK) {
		printf("NO IDirectDraw_SetDisplayMode(): GLE = %ld\n", GetLastError());
		IDirectDraw_Release(dd_obj);
		return 1;
	}
	printf("OK IDirectDraw_SetDisplayMode()\n");

	printf("GetSystemMetrics(SM_CXSCREEN)=%d\n", GetSystemMetrics(SM_CXSCREEN));
	printf("GetSystemMetrics(SM_CYSCREEN)=%d\n", GetSystemMetrics(SM_CYSCREEN));

	OK = MoveWindow(hwnd2, 0, 0, GetSystemMetrics(SM_CXSCREEN)/2, GetSystemMetrics(SM_CYSCREEN), TRUE);
	printf("MoveWindow() returns %d\n", OK);

	ZeroMemory(&dd_sd1, sizeof(dd_sd1));
	dd_sd1.dwSize = sizeof(dd_sd1);
	dd_sd1.dwFlags = DDSD_CAPS;
	dd_sd1.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	rc = IDirectDraw_CreateSurface(dd_obj, &dd_sd1, &dd_buf1, NULL);
	if (rc != DD_OK) {
		IDirectDraw_Release(dd_obj);
		return 1;
	}
	printf("OK IDirectDraw_CreateSurface()\n");

	for (x = 0; x < 256; x++) {
		palette[x].peRed = x;
		palette[x].peGreen = x;
		palette[x].peBlue = x;
		palette[x].peFlags = 0;
	}
	rc = IDirectDraw_CreatePalette(dd_obj, DDPCAPS_8BIT, palette, &whatever, NULL);
	printf("OK IDirectDraw_CreatePalette()\n");

	DWORD ddcaps;

	rc = IDirectDrawPalette_GetCaps(whatever, &ddcaps);
	if (rc != DD_OK) {
		printf("NO IDirectDrawPalette_GetCaps(): GLE = %ld\n", GetLastError());
		IDirectDraw_Release(dd_obj);
		return 1;
	}

	printf("OK IDirectDrawSurface_GetCaps()\n");
	printf("DDSCAPS are: %lx\n", ddcaps);

	rc = IDirectDrawSurface_SetPalette(dd_buf1, whatever);
	printf("OK IDirectDrawSurface_SetPalette()\n");

	/*IDirectDraw_WaitForVerticalBlank(dd_obj, 1, NULL);*/
	rc = 1;
	while (rc != DD_OK) {
		rc = IDirectDrawSurface_Lock(dd_buf1, NULL, &dd_sd1, 1, NULL);
	}
	lpscreen = (BYTE*)dd_sd1.lpSurface;
	for (y = 0; y < screen_x * screen_y * (screen_bpp/8); y++) {
		lpscreen[y] = (BYTE)128; // Half-brite white; maybe gray
	}
	rc = 1;
	while (rc != DD_OK) {
		rc = IDirectDrawSurface_Unlock(dd_buf1, NULL);
	}
	UpdateWindow(main_hwnd);
	UpdateWindow(hwnd2);

	while (!quit) {
		while(PeekMessage(&msg, main_hwnd, 0, 0, PM_NOREMOVE)) {
			if(GetMessage(&msg, main_hwnd, 0, 0)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	IDirectDraw_Release(dd_obj);
	return 0;
}
