#include <windows.h>

#define ID_LIST 1
#define ID_TEXT 2

#define MAX_READ 8192
#define DIR_ATTR (DDL_READWRITE | DDL_READONLY | DDL_HIDDEN | DDL_SYSTEM | \
					DDL_DIRECTORY | DDL_ARCHIVE | DDL_DRIVES)
#define DT_FLAGS (DT_WORDBREAK | DT_EXPANDTABS | DT_NOCLIP | DT_NOPREFIX)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ListProc(HWND, UINT, WPARAM, LPARAM);

WNDPROC oldList;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("Head");
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

	auto hwnd = CreateWindow(appName, TEXT("Head"), WS_OVERLAPPEDWINDOW,
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool validFile;
	static byte buffer[MAX_READ];
	static HWND hwndList, hwndText;
	static RECT rect;
	static TCHAR szFile[MAX_PATH + 1];

	switch (message)
	{
	case WM_CREATE:
	{
		TCHAR szBuffer[MAX_PATH + 1];
		auto cxChar = LOWORD(GetDialogBaseUnits());
		auto cyChar = HIWORD(GetDialogBaseUnits());

		rect.left = 20 * cxChar;
		rect.top = 3 * cyChar;

		hwndList = CreateWindow(TEXT("listbox"), nullptr,
			WS_CHILD | WS_VISIBLE | LBS_STANDARD, cxChar, cyChar * 3,
			cxChar * 13 + GetSystemMetrics(SM_CXVSCROLL), cyChar * 10,
			hwnd, reinterpret_cast<HMENU>(ID_LIST),
			reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, nullptr);

		GetCurrentDirectory(MAX_PATH + 1, szBuffer);

		hwndText = CreateWindow(TEXT("static"), szBuffer,
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			cxChar, cyChar, cxChar * MAX_PATH, cyChar,
			hwnd, reinterpret_cast<HMENU>(ID_TEXT),
			reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, nullptr);

		// GetWindowLongPtr for x64 compatibility
		oldList = reinterpret_cast<WNDPROC>(SetWindowLong(hwndList, GWLP_WNDPROC,
			reinterpret_cast<LONG_PTR>(ListProc)));

		SendMessage(hwndList, LB_DIR, DIR_ATTR, reinterpret_cast<LPARAM>(TEXT("*.*")));
		return 0;
	}
	case WM_SIZE:
	{
		rect.right = LOWORD(lParam);
		rect.bottom = HIWORD(lParam);

		return 0;
	}
	case WM_SETFOCUS:
	{
		SetFocus(hwndList);
		return 0;
	}
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == ID_LIST && HIWORD(wParam) == LBN_DBLCLK)
		{
			TCHAR szBuffer[MAX_PATH + 1];
			auto i = SendMessage(hwndList, LB_GETCURSEL, 0, 0);

			if (LB_ERR == i)
				return DefWindowProc(hwnd, message, wParam, lParam);

			SendMessage(hwndList, LB_GETTEXT, i, reinterpret_cast<LPARAM>(szBuffer));
			auto hFile = CreateFile(szBuffer, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

			if (INVALID_HANDLE_VALUE != hFile)
			{
				CloseHandle(hFile);
				validFile = true;
				lstrcpy(szFile, szBuffer);

				GetCurrentDirectory(MAX_PATH + 1, szBuffer);

				if (szBuffer[lstrlen(szBuffer) - 1] != '\\')
					lstrcat(szBuffer, TEXT("\\"));

				SetWindowText(hwndText, lstrcat(szBuffer, szFile));
			}
			else
			{
				validFile = false;

				szBuffer[lstrlen(szBuffer) - 1] = '\0';

				if (!SetCurrentDirectory(szBuffer + 1))
				{
					szBuffer[3] = ':';
					szBuffer[4] = '\\';
					szBuffer[5] = '\0';
					SetCurrentDirectory(szBuffer + 2);
				}

				GetCurrentDirectory(MAX_PATH + 1, szBuffer);
				SetWindowText(hwndText, szBuffer);
				SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
				SendMessage(hwndList, LB_DIR, DIR_ATTR, reinterpret_cast<LPARAM>(TEXT("*.*")));
			}

			InvalidateRect(hwnd, nullptr, true);
		}

		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		DWORD i;

		if (!validFile)
			return DefWindowProc(hwnd, message, wParam, lParam);

		auto hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ,
			nullptr, OPEN_EXISTING, 0, nullptr);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			validFile = false;
			return DefWindowProc(hwnd, message, wParam, lParam);
		}

		ReadFile(hFile, buffer, MAX_READ, &i, nullptr);
		CloseHandle(hFile);

		auto hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
		SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));

		DrawTextA(hdc, reinterpret_cast<LPCSTR>(buffer), i, &rect, DT_FLAGS);

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

LRESULT CALLBACK ListProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_KEYDOWN && wParam == VK_RETURN)
		SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(1, LBN_DBLCLK),
			reinterpret_cast<LPARAM>(hwnd));

	return CallWindowProc(oldList, hwnd, message, wParam, lParam);
}