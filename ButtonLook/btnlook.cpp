#include <windows.h>

struct
{
	int iStyle;
	PTCHAR szText;
} button[] = {
	BS_PUSHBUTTON, TEXT("PUSHBUTTON"),
	BS_DEFPUSHBUTTON, TEXT("DEFPUSHBUTTON"),
	BS_CHECKBOX, TEXT("CHECKBOX"),
	BS_AUTOCHECKBOX, TEXT("AUTOCHECKBOX"),
	BS_RADIOBUTTON, TEXT("RADIOBUTTON"),
	BS_3STATE, TEXT("3STATE"),
	BS_AUTO3STATE, TEXT("AUTO3STATE"),
	BS_GROUPBOX, TEXT("GROUPBOX"),
	BS_AUTORADIOBUTTON, TEXT("AUTORADIOBUTTON"),
	BS_OWNERDRAW, TEXT("OWNERDRAW"),
};

#define NUM (sizeof(button) / sizeof(button[0]))

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("BtnLook");
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

	auto hwnd = CreateWindow(appName, TEXT("Button Look"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndButton[NUM];
	static RECT rect;
	static TCHAR szTop[] = TEXT("Message            wParam       lParam"),
		szUnd[] = TEXT("_______            ______       ______"),
		szFormat[] = TEXT("%-16s%04X-%04X    %04X-%04x"),
		szBuffer[50];
	static int cxChar, cyChar;

	switch (message)
	{
	case WM_CREATE:
	{
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		for (auto i = 0; i < NUM; i++)
		{
			hwndButton[i] = CreateWindow(TEXT("button"), button[i].szText,
				WS_CHILD | WS_VISIBLE | button[i].iStyle,
				cxChar, cyChar * (1 + 2 * i), 20 * cxChar, 7 * cyChar / 4,
				hwnd, reinterpret_cast<HMENU>(i),
				reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, nullptr);
		}
		return 0;
	}
	case WM_SIZE:
	{
		SetRect(&rect, 24 * cxChar, 2 * cyChar, LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_PAINT:
	{
		InvalidateRect(hwnd, &rect, true);

		PAINTSTRUCT ps;
		auto hdc = BeginPaint(hwnd, &ps);

		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
		SetBkMode(hdc, TRANSPARENT);

		TextOut(hdc, 24 * cxChar, cyChar, szTop, lstrlen(szTop));
		TextOut(hdc, 24 * cxChar, cyChar, szUnd, lstrlen(szUnd));

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DRAWITEM:
	case WM_COMMAND:
	{
		// cyChar for newer on top
		ScrollWindow(hwnd, 0, -cyChar, &rect, &rect);

		auto hdc = GetDC(hwnd);

		SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

		// 2 * cyChar for newer on top
		TextOut(hdc, 24 * cxChar, cyChar * (rect.bottom / cyChar - 1),
			szBuffer, wsprintf(szBuffer, szFormat,
				message == WM_DRAWITEM ? TEXT("WM_DRAWITEM") : TEXT("WM_COMMAND"),
				HIWORD(wParam), LOWORD(wParam), HIWORD(lParam), LOWORD(lParam)));

		ReleaseDC(hwnd, hdc);
		ValidateRect(hwnd, &rect);
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