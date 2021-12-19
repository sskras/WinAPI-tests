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
	WNDCLASS wc;
	const char *WIN_CLASS_NAME = "foo";
	const char *WIN_TITLE = "foo2";
	int rc;
	HWND main_hwnd;
	HWND hwnd2;
	LPDIRECTDRAW dd_obj;
	DDSURFACEDESC dd_sd1;
	LPDIRECTDRAWSURFACE dd_buf1;
	PALETTEENTRY palette[256];
	LPDIRECTDRAWPALETTE whatever;
	int x;
	int y;
        BYTE *lpscreen;
	MSG msg;

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

	printf("Hello Moto from %s!\n", WIN_TITLE);

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

	rc = IDirectDraw_SetDisplayMode(dd_obj, 640, 480, 8);
	if (rc != DD_OK) {
		printf("NO IDirectDraw_SetDisplayMode(): GLE = %ld\n", GetLastError());
		IDirectDraw_Release(dd_obj);
		return 1;
	}
	printf("OK IDirectDraw_SetDisplayMode()\n");

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

	rc = IDirectDrawSurface_SetPalette(dd_buf1, whatever);
	printf("OK IDirectDrawSurface_SetPalette()\n");


	while (!quit) {
	for (x = 0; x < 255; x++) {
		/*IDirectDraw_WaitForVerticalBlank(dd_obj, 1, NULL);*/
		rc = 1;
		while (rc != DD_OK) {
			rc = IDirectDrawSurface_Lock(dd_buf1, NULL, &dd_sd1, 1, NULL);
		}
		lpscreen = (BYTE*)dd_sd1.lpSurface;
		for (y = 0; y < 640*480; y++) {
			lpscreen[y] = (BYTE)x;
		}
		rc = 1;
		while (rc != DD_OK) {
			rc = IDirectDrawSurface_Unlock(dd_buf1, NULL);
		}
		UpdateWindow(main_hwnd);
		UpdateWindow(hwnd2);
	    		while(PeekMessage(&msg, main_hwnd, 0, 0, PM_NOREMOVE)) {
	    			if(GetMessage(&msg, main_hwnd, 0, 0)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
	    			}
        		}
		}
	}

	IDirectDraw_Release(dd_obj);
	return 0;
}
