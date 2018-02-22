#include <windows.h>

#define ID_TIMER 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("DigClock");
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

	auto hwnd = CreateWindow(appName, TEXT("Digital Clock"), WS_OVERLAPPEDWINDOW,
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

void DisplayDigit(HDC hdc, int number)
{
	static bool fSevenSegment[10][7] = {
		true, true, true, false, true, true, true,
		false, false, true, false, false, true, false,
		true, false, true, true, true, false, true,
		true, false, true, true, false, true, true,
		false, true, true, true, false, true, false,
		true, true, false, true, false, true, true,
		true, true, false, true, true, true, true,
		true, false, true, false, false, true, false,
		true, true, true, true, true, true, true,
		true, true, true, true, false, true, true
	};

	static POINT ptSegment[7][6] = {
		7, 6, 11, 2, 31, 2, 35, 6, 31, 10, 11, 10,
		6, 7, 10, 11, 10, 31, 6, 35, 2, 31, 2, 11,
		36, 7, 40, 11, 40, 31, 36, 35, 32, 31, 32, 11,
		7, 36, 11, 32, 31, 32, 35, 36, 31, 40, 11, 40,
		6, 37, 10, 41, 10, 61, 6, 65, 2, 61, 2, 41,
		36, 37, 40, 41, 40, 61, 36, 65, 32, 61, 32, 41,
		7, 66, 11, 62, 31, 62, 35, 66, 31, 70, 11, 70
	};

	for (auto segment = 0; segment < 7; segment++)
	{
		if (fSevenSegment[number][segment])
			Polygon(hdc, ptSegment[segment], 6);
	}
}

void DisplayTwoDigits(HDC hdc, int number, bool suppress)
{
	if (!suppress || number / 10 != 0)
		DisplayDigit(hdc, number / 10);

	OffsetWindowOrgEx(hdc, -42, 0, nullptr);
	DisplayDigit(hdc, number % 10);
	OffsetWindowOrgEx(hdc, -42, 0, nullptr);
}

void DisplayColon(HDC hdc)
{
	static POINT ptColon[2][4] = {
		2, 21, 6, 17, 10, 21, 6, 25,
		2, 51, 6, 47, 10, 51, 6, 55
	};

	Polygon(hdc, ptColon[0], 4);
	Polygon(hdc, ptColon[1], 4);

	OffsetWindowOrgEx(hdc, -12, 0, nullptr);
}

void DisplayTime(HDC hdc, bool f24Hour, bool suppress)
{
	SYSTEMTIME st;

	GetLocalTime(&st);

	if (f24Hour)
		DisplayTwoDigits(hdc, st.wHour, suppress);
	else
		DisplayTwoDigits(hdc, (st.wHour %= 12) ? st.wHour : 12, suppress);

	DisplayColon(hdc);
	DisplayTwoDigits(hdc, st.wMinute, false);
	DisplayColon(hdc);
	DisplayTwoDigits(hdc, st.wSecond, false);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool f24Hour, suppress;
	static HBRUSH hBrushRed;
	static int cxClient, cyClient;

	switch (message)
	{
	case WM_CREATE:
	{
		hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
		SetTimer(hwnd, ID_TIMER, 1000, nullptr);
	}
	case WM_SETTINGCHANGE:
	{
		TCHAR szBuffer[2];

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuffer, 2);
		f24Hour = szBuffer[0] == '1';

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITLZERO, szBuffer, 2);
		suppress = szBuffer[0] == '0';

		InvalidateRect(hwnd, nullptr, true);
		return 0;
	}
	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		return 0;
	}
	case WM_TIMER:
	{
		InvalidateRect(hwnd, nullptr, true);

		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		auto hdc = BeginPaint(hwnd, &ps);

		SetMapMode(hdc, MM_ISOTROPIC);
		SetWindowExtEx(hdc, 276, 72, nullptr);
		SetViewportExtEx(hdc, cxClient, cyClient, nullptr);

		SetWindowOrgEx(hdc, 138, 36, nullptr);
		SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, nullptr);
		SelectObject(hdc, GetStockObject(NULL_PEN));
		SelectObject(hdc, hBrushRed);

		DisplayTime(hdc, f24Hour, suppress);

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
	{
		KillTimer(hwnd, ID_TIMER);
		DeleteObject(hBrushRed);
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}