#include <windows.h>
#include "resource.h"
#include <winuser.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("NoPopups");
	WNDCLASS wndClass;
	MSG msg;

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

	auto hwnd = CreateWindow(appName, TEXT("No-Popup Nested Menu Demonstration"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

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
	static HMENU hMenuMain, hMenuEdit, hMenuFile;

	switch (message)
	{
	case WM_CREATE:
	{
		// Use GetWindowLongPtr for x64 compatibility
		auto hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hwnd, GWL_HINSTANCE));

		hMenuMain = LoadMenu(hInstance, MAKEINTRESOURCE(MENUMAIN));
		hMenuEdit = LoadMenu(hInstance, MAKEINTRESOURCE(MENUEDIT));
		hMenuFile = LoadMenu(hInstance, MAKEINTRESOURCE(MENUFILE));

		SetMenu(hwnd, hMenuMain);
		return 0;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_MAIN:
		{
			SetMenu(hwnd, hMenuMain);
			return 0;
		}
		case IDM_FILE:
		{
			SetMenu(hwnd, hMenuFile);
			return 0;
		}
		case IDM_EDIT:
		{
			SetMenu(hwnd, hMenuEdit);
			return 0;
		}
		case IDM_FILE_NEW:
		case IDM_FILE_OPEN:
		case IDM_FILE_SAVE:
		case IDM_FILE_SAVE_AS:
		case IDM_EDIT_UNDO:
		case IDM_EDIT_CUT:
		case IDM_EDIT_COPY:
		case IDM_EDIT_PASTE:
		case IDM_EDIT_CLEAR:
		{
			MessageBeep(0);
			return 0;
		}
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	case WM_DESTROY:
	{
		SetMenu(hwnd, hMenuMain);
		DestroyMenu(hMenuEdit);
		DestroyMenu(hMenuFile);
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}