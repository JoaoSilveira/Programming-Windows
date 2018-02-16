#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("WhatSize");
	MSG msg;
	WNDCLASS wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = appName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(nullptr, TEXT("This program requires Windows NT!"), appName, MB_ICONERROR);
		return 0;
	}

	auto hwnd = CreateWindow(appName, "What Size is the Window?", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void Show(HWND hwnd, HDC hdc, int xText, int yText, int iMapMode, PTCHAR mapMode)
{
	TCHAR buffer[60];
	RECT rect;

	SaveDC(hdc);

	SetMapMode(hdc, iMapMode);
	GetClientRect(hwnd, &rect);
	DPtoLP(hdc, reinterpret_cast<PPOINT>(&rect), 2);

	RestoreDC(hdc, -1);

	TextOut(hdc, xText, yText, buffer, wsprintf(buffer, TEXT("%-20s %7d %7d %7d %7d"), mapMode,
		rect.left, rect.right, rect.top, rect.bottom));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TCHAR heading[] = TEXT("Mapping Mode            Left   Right     Top  Bottom");
	static TCHAR udrLine[] = TEXT("------------            ----   -----     ---  ------");
	static int cxChar;
	static int cyChar;

	switch (message)
	{
	case WM_CREATE:
	{
		TEXTMETRIC tm;
		auto hdc = GetDC(hwnd);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		ReleaseDC(hwnd, hdc);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		auto hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

		SetMapMode(hdc, MM_ANISOTROPIC);
		SetWindowExtEx(hdc, 1, 1, nullptr);
		SetViewportExtEx(hdc, cxChar, cyChar, nullptr);

		TextOut(hdc, 1, 1, heading, lstrlen(heading));
		TextOut(hdc, 1, 2, udrLine, lstrlen(udrLine));

		Show(hwnd, hdc, 1, 3, MM_TEXT, TEXT("TEXT (pixels)"));
		Show(hwnd, hdc, 1, 4, MM_LOMETRIC, TEXT("LOMETRIC (.1 mm)"));
		Show(hwnd, hdc, 1, 5, MM_HIMETRIC, TEXT("HIMETRIC (.01 mm)"));
		Show(hwnd, hdc, 1, 6, MM_LOENGLISH, TEXT("LOENGLISH (.01 in)"));
		Show(hwnd, hdc, 1, 7, MM_HIENGLISH, TEXT("HIENGLISH (.001 in)"));
		Show(hwnd, hdc, 1, 8, MM_TWIPS, TEXT("TWIPS (1/1440 in)"));

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}