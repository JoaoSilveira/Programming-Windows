#include <windows.h>
#include "resource.h"

#define ID_TIMER 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR appName[] = TEXT("MenuDemo");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, PSTR cmdLine, int cmdShow)
{
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndclass.lpszMenuName = appName;
	wndclass.lpszClassName = appName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(nullptr, TEXT("This program requires Windows NT!"), appName, MB_ICONERROR);

		return 0;
	}

	auto hwnd = CreateWindow(appName, TEXT("Menu Demonstration"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr,
		hInstance, nullptr);

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
	static int idColor[5] = { WHITE_BRUSH, LTGRAY_BRUSH, GRAY_BRUSH,
		DKGRAY_BRUSH, BLACK_BRUSH };
	static int iSelection = IDM_BKGND_WHITE;

	switch (message)
	{
	case WM_COMMAND:
	{
		auto hMenu = GetMenu(hwnd);

		switch (LOWORD(wParam))
		{
		case IDM_FILE_NEW:
		case IDM_FILE_OPEN:
		case IDM_FILE_SAVE:
		case IDM_FILE_SAVE_AS:
		{
			MessageBeep(0);
			return 0;
		}
		case IDM_APP_EXIT:
		{
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;
		}
		case IDM_BKGND_WHITE:
		case IDM_BKGND_LTGRAY:
		case IDM_BKGND_GRAY:
		case IDM_BKGND_DKGRAY:
		case IDM_BKGND_BLACK:
		{
			CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);
			iSelection = LOWORD(wParam);
			CheckMenuItem(hMenu, iSelection, MF_CHECKED);

			// Use SetClassLongPtr for x64 compatibility
			SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND,
				reinterpret_cast<LONG>(GetStockObject(idColor[LOWORD(wParam) - IDM_BKGND_WHITE])));

			InvalidateRect(hwnd, nullptr, true);

			return 0;
		}
		case IDM_TIMER_START:
		{
			if (SetTimer(hwnd, ID_TIMER, 1000, nullptr))
			{
				EnableMenuItem(hMenu, IDM_TIMER_START, MF_GRAYED);
				EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_ENABLED);
			}

			return 0;
		}
		case IDM_TIMER_STOP:
		{
			KillTimer(hwnd, ID_TIMER);
			EnableMenuItem(hMenu, IDM_TIMER_START, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_TIMER_STOP, MF_GRAYED);

			return 0;
		}
		case IDM_APP_HELP:
		{
			MessageBox(hwnd, TEXT("Help not yet implemented!"), appName,
				MB_ICONEXCLAMATION | MB_OK);

			return 0;
		}
		case IDM_APP_ABOUT:
		{
			MessageBox(hwnd, TEXT("Menu Demonstration Program\n")
				TEXT("(c) Charles Petzold, 1998"), appName,
				MB_ICONINFORMATION | MB_OK);

			return 0;
		}
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}
	case WM_TIMER:
	{
		MessageBeep(0);

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