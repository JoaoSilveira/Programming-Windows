#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("KeyView2");
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndclass.lpszMenuName = nullptr;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(nullptr, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);

		return 0;
	}

	auto hwnd = CreateWindow(szAppName, TEXT("Keyboard Message View No. 2"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
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
	static DWORD dwCharSet = DEFAULT_CHARSET;
	static int cxClientMax, cyClientMax, cxClient, cyClient, cxChar, cyChar;
	static int linesMax, lines;
	static PMSG pmsg;
	static RECT rectScroll;
	static TCHAR szTop[] = TEXT("Message        Key       Char     Repeat Scan Ext ALT Prev Tran");
	static TCHAR szUnd[] = TEXT("_______        ___       ____     ______ ____ ___ ___ ____ ____");
	static PTCHAR fmt[] = {
		TEXT("%-13s %3d %-15s%c%6u %4d %3s %3s %4s %4s"),
		TEXT("%-13s          0x%04X%1s%c %6u %4d %3s %3s %4s %4s")
	};
	static PTCHAR szYes = TEXT("Yes");
	static PTCHAR szNo = TEXT("No");
	static PTCHAR szDown = TEXT("Down");
	static PTCHAR szUp = TEXT("Up");

	static PTCHAR szMessage[] = {
		TEXT("WM_KEYDOWN"), TEXT("WM_KEYUP"), TEXT("WM_CHAR"),
		TEXT("WM_DEADCHAR"), TEXT("WM_SYSKEYDOWN"), TEXT("WM_SYSKEYUP"),
		TEXT("WM_SYSCHAR"), TEXT("WM_SYSDEADCHAR")
	};

	switch (message)
	{
	case WM_INPUTLANGCHANGE:
	{
		dwCharSet = wParam;
	}
	case WM_CREATE:
	case WM_DISPLAYCHANGE:
	{
		TEXTMETRIC tm;

		cxClientMax = GetSystemMetrics(SM_CXMAXIMIZED);
		cyClientMax = GetSystemMetrics(SM_CYMAXIMIZED);

		auto hdc = GetDC(hwnd);

		SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, nullptr));
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight;

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		ReleaseDC(hwnd, hdc);

		if (pmsg)
			delete pmsg;

		linesMax = cyClientMax / cyChar;
		pmsg = new MSG[linesMax];
		lines = 0;
	}
	case WM_SIZE:
	{
		if (message == WM_SIZE)
		{
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
		}

		SetRect(&rectScroll, 0, cyChar, cxClient, cyChar * (cyClient / cyChar));

		InvalidateRect(hwnd, nullptr, true);

		if (message == WM_INPUTLANGCHANGE)
			return true;

		return 0;
	}
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
	{
		for (auto i = linesMax - 1; i > 0; i--)
		{
			pmsg[i] = pmsg[i - 1];
		}

		pmsg[0].hwnd = hwnd;
		pmsg[0].message = message;
		pmsg[0].wParam = wParam;
		pmsg[0].lParam = lParam;

		lines = min(lines + 1, linesMax);

		ScrollWindow(hwnd, 0, -cyChar, &rectScroll, &rectScroll); // newer on top change "-cyChar" to "cyChar"

		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	case WM_PAINT:
	{
		TCHAR szKeyName[32];
		TCHAR szBuffer[128];
		PAINTSTRUCT ps;

		auto hdc = BeginPaint(hwnd, &ps);

		SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, nullptr));
		SetBkMode(hdc, TRANSPARENT);
		TextOut(hdc, 0, 0, szTop, lstrlen(szTop));
		TextOut(hdc, 0, 0, szUnd, lstrlen(szUnd));

		for (auto i = 0; i < min(lines, cyClient / cyChar - 1); i++)
		{
			auto type = pmsg[i].message == WM_CHAR ||
				pmsg[i].message == WM_SYSCHAR ||
				pmsg[i].message == WM_DEADCHAR ||
				pmsg[i].message == WM_SYSDEADCHAR;

			GetKeyNameText(pmsg[i].lParam, szKeyName, sizeof(szKeyName) / sizeof(TCHAR));

			// newer on top change "(cyClient / cyChar - 1 - i)" to "(i + 1)"
			TextOut(hdc, 0, (cyClient / cyChar - 1 - i) * cyChar, szBuffer,
				wsprintf(szBuffer, fmt[type],
					szMessage[pmsg[i].message - WM_KEYFIRST],
					pmsg[i].wParam,
					reinterpret_cast<LPCSTR>(type ? TEXT(" ") : szKeyName),
					static_cast<TCHAR>(type ? pmsg[i].wParam : ' '),
					LOWORD(pmsg[i].lParam),
					HIWORD(pmsg[i].lParam) & 0xff,
					0x01000000 & pmsg[i].lParam ? szYes : szNo,
					0x20000000 & pmsg[i].lParam ? szYes : szNo,
					0x40000000 & pmsg[i].lParam ? szDown : szUp,
					0x80000000 & pmsg[i].lParam ? szUp : szDown
				));
		}

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
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