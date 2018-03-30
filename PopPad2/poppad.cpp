#include <windows.h>
#include "resource.h"

#define ID_EDIT 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR appName[] = TEXT("PopPad2");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
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

	auto hwnd = CreateWindow(appName, appName, WS_OVERLAPPEDWINDOW,
		GetSystemMetrics(SM_CXSCREEN) / 4,
		GetSystemMetrics(SM_CYSCREEN) / 4,
		GetSystemMetrics(SM_CXSCREEN) / 2,
		GetSystemMetrics(SM_CYSCREEN) / 2,
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	auto hAccel = LoadAccelerators(hInstance, appName);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

int AskConfirmation(HWND hwnd)
{
	return MessageBox(hwnd, TEXT("Really want to close PopPad2?"), appName,
		MB_YESNO | MB_ICONQUESTION);
}

bool isOutOfSpace(WPARAM wParam, LPARAM lParam)
{
	return LOWORD(lParam) == ID_EDIT &&
		(HIWORD(wParam) == EN_ERRSPACE ||
			HIWORD(wParam) == EN_MAXTEXT);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEdit;

	switch (message)
	{
	case WM_CREATE:
	{
		hwndEdit = CreateWindow(TEXT("edit"), nullptr,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
			WS_BORDER | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
			0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(ID_EDIT),
			reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, nullptr);

		return 0;
	}
	case WM_SETFOCUS:
	{
		SetFocus(hwndEdit);
		return 0;
	}
	case WM_SIZE:
	{
		MoveWindow(hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), true);
		return 0;
	}
	case WM_INITMENUPOPUP:
	{
		if (lParam == 1)
		{
			EnableMenuItem(reinterpret_cast<HMENU>(wParam), IDM_EDIT_UNDO,
				SendMessage(hwndEdit, EM_CANUNDO, 0, 0) ?
				MF_ENABLED : MF_GRAYED);

			EnableMenuItem(reinterpret_cast<HMENU>(wParam), IDM_EDIT_PASTE,
				IsClipboardFormatAvailable(CF_TEXT) ?
				MF_ENABLED : MF_GRAYED);

			auto select = SendMessage(hwndEdit, EM_GETSEL, 0, 0);

			auto enabled = HIWORD(select) == LOWORD(select) ?
				MF_GRAYED : MF_ENABLED;

			EnableMenuItem(reinterpret_cast<HMENU>(wParam), IDM_EDIT_CUT,
				enabled);
			EnableMenuItem(reinterpret_cast<HMENU>(wParam), IDM_EDIT_COPY,
				enabled);
			EnableMenuItem(reinterpret_cast<HMENU>(wParam), IDM_EDIT_CLEAR,
				enabled);
			return 0;
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	case WM_COMMAND:
	{
		if (lParam)
		{
			if (isOutOfSpace(wParam, lParam))
				MessageBox(hwnd, TEXT("Edit control out of space."), appName,
					MB_OK | MB_ICONSTOP);

			return 0;
		}

		switch (LOWORD(wParam))
		{
		case IDM_FILE_NEW:
		case IDM_FILE_OPEN:
		case IDM_FILE_SAVE:
		case IDM_FILE_SAVE_AS:
		case IDM_FILE_PRINT:
		{
			MessageBeep(0);
			return 0;
		}
		case IDM_APP_EXIT:
		{
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;
		}
		case IDM_EDIT_UNDO:
		{
			SendMessage(hwndEdit, WM_UNDO, 0, 0);
			return 0;
		}
		case IDM_EDIT_CUT:
		{
			SendMessage(hwndEdit, WM_CUT, 0, 0);
			return 0;
		}
		case IDM_EDIT_COPY:
		{
			SendMessage(hwndEdit, WM_COPY, 0, 0);
			return 0;
		}
		case IDM_EDIT_PASTE:
		{
			SendMessage(hwndEdit, WM_PASTE, 0, 0);
			return 0;
		}
		case IDM_EDIT_CLEAR:
		{
			SendMessage(hwndEdit, WM_CLEAR, 0, 0);
			return 0;
		}
		case IDM_EDIT_SELECT_ALL:
		{
			SendMessage(hwndEdit, EM_SETSEL, 0, -1);
			return 0;
		}
		case IDM_HELP_HELP:
		{
			MessageBox(hwnd, TEXT("Help not yet implemented!"),
				appName, MB_OK | MB_ICONEXCLAMATION);
			return 0;
		}
		case IDM_APP_ABOUT:
		{
			MessageBox(hwnd, TEXT("POPPAD2 (c) Charles Petzold, 1998"),
				appName, MB_OK | MB_ICONINFORMATION);
			return 0;
		}
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}
	case WM_CLOSE:
	{
		if (IDYES == AskConfirmation(hwnd))
			DestroyWindow(hwnd);
		return 0;
	}
	case WM_QUERYENDSESSION:
		return IDYES == AskConfirmation(hwnd) ? 1 : 0;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}