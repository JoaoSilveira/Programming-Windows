#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK EllipPushWndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow) {
	static TCHAR appName[] = TEXT("About3");
	MSG msg;
	WNDCLASS wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, appName);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndClass.lpszMenuName = appName;
	wndClass.lpszClassName = appName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(nullptr, TEXT("This program requires Windows NT!"), appName, MB_ICONERROR);
		return 0;
	}

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = EllipPushWndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = nullptr;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = TEXT("EllipPush");

	RegisterClass(&wndClass);

	auto hwnd = CreateWindow(appName, TEXT("About Box Demo Program"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;

	switch (message)
	{
	case WM_CREATE:
	{
		hInstance = reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance;
		return 0;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_APP_ABOUT:
		{
			DialogBox(hInstance, TEXT("AboutBox"), hwnd, AboutDlgProc);
			return 0;
		}
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
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

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return true;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, 0);
			return true;
		default:
			return false;
		}
	}
	default:
		return false;
	}
}

LRESULT CALLBACK EllipPushWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		RECT rc;
		TCHAR text[40];

		GetClientRect(hwnd, &rc);
		GetWindowText(hwnd, text, sizeof(text));

		auto hdc = BeginPaint(hwnd, &ps);

		auto hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		hBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));
		SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
		SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));

		Ellipse(hdc, rc.left, rc.top, rc.right, rc.bottom);
		DrawText(hdc, text, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		DeleteObject(SelectObject(hdc, hBrush));

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_KEYUP:
	{
		if (wParam != VK_SPACE)
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	case WM_LBUTTONUP:
	{
		// use GetWindowLongPtr for x64 compatibility
		SendMessage(GetParent(hwnd), WM_COMMAND, GetWindowLong(hwnd, GWL_ID), reinterpret_cast<LPARAM>(hwnd));
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}