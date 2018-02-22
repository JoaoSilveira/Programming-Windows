#include <windows.h>

#define ID_TIMER 1

void FindWindowSize(int &, int &);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("WhatClr");
	MSG msg;
	WNDCLASS wndClass;
	int cxWindow, cyWindow;

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

	FindWindowSize(cxWindow, cyWindow);

	auto hwnd = CreateWindow(appName, TEXT("What Color"),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER,
		CW_USEDEFAULT, CW_USEDEFAULT,
		cxWindow, cyWindow,
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void FindWindowSize(int& cxWindow, int& cyWindow)
{
	TEXTMETRIC tm;

	auto hdcScreen = CreateIC(TEXT("DISPLAY"), nullptr, nullptr, nullptr);
	GetTextMetrics(hdcScreen, &tm);
	DeleteDC(hdcScreen);

	cxWindow = 2 * GetSystemMetrics(SM_CXBORDER) + 12 * tm.tmAveCharWidth;
	cyWindow = 2 * GetSystemMetrics(SM_CYBORDER) + GetSystemMetrics(SM_CYCAPTION) +
		2 * tm.tmHeight;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static COLORREF cr, crLast;
	static HDC hdcScreen;

	switch (message)
	{
	case WM_CREATE:
	{
		hdcScreen = CreateDC(TEXT("DISPLAY"), nullptr, nullptr, nullptr);

		SetTimer(hwnd, ID_TIMER, 100, nullptr);
		return 0;
	}
	case WM_TIMER:
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);

		cr = GetPixel(hdcScreen, cursorPos.x, cursorPos.y);

		SetPixel(hdcScreen, cursorPos.x, cursorPos.y, 0);

		if (cr != crLast)
		{
			crLast = cr;
			InvalidateRect(hwnd, nullptr, false);
		}
		return 0;
	}
	case WM_PAINT:
	{
		TCHAR szBuffer[16];
		RECT rc;
		PAINTSTRUCT ps;
		auto hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rc);

		wsprintf(szBuffer, TEXT("  %02X %02X %02X  "), GetRValue(cr), GetGValue(cr), GetBValue(cr));

		DrawText(hdc, szBuffer, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
	{
		DeleteDC(hdcScreen);
		KillTimer(hwnd, ID_TIMER);
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}