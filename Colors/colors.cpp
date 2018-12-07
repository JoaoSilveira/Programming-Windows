#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ScrollProc(HWND, UINT, WPARAM, LPARAM);

int idFocus;
WNDPROC oldScroll[3];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("Colors");
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

	auto hwnd = CreateWindow(appName, TEXT("Color Scroll"), WS_OVERLAPPEDWINDOW,
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
	static COLORREF crPrim[3] = { RGB(255,0,0),RGB(0,255,0),RGB(0,0,255) };
	static HBRUSH hBrush[3], hBrushStatic;
	static HWND hwndScroll[3], hwndLabel[3], hwndValue[3], hwndRect;
	static int color[3], cyChar;
	static RECT rcColor;
	static PTCHAR szColorLabel[] = { TEXT("Red"), TEXT("Green"), TEXT("Blue") };

	switch (message)
	{
	case WM_CREATE:
	{
		// GetWindowLongPtr for x64 compatibility
		auto hInstance = reinterpret_cast<HINSTANCE>(GetWindowLong(hwnd, GWLP_HINSTANCE));

		hwndRect = CreateWindow(TEXT("static"), nullptr,
			WS_CHILD | WS_VISIBLE | SS_WHITERECT,
			0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(9), hInstance, nullptr);

		for (auto i = 0; i < 3; i++)
		{
			hwndScroll[i] = CreateWindow(TEXT("scrollbar"), nullptr,
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | SBS_VERT,
				0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(i), hInstance, nullptr);

			SetScrollRange(hwndScroll[i], SB_CTL, 0, 255, false);
			SetScrollPos(hwndScroll[i], SB_CTL, 0, false);

			hwndLabel[i] = CreateWindow(TEXT("static"), szColorLabel[i],
				WS_CHILD | WS_VISIBLE | SS_CENTER,
				0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(i + 3), hInstance, nullptr);

			hwndValue[i] = CreateWindow(TEXT("static"), TEXT("0"),
				WS_CHILD | WS_VISIBLE | SS_CENTER,
				0, 0, 0, 0, hwnd, reinterpret_cast<HMENU>(i + 6), hInstance, nullptr);

			// SetWindowLongPtr for x64 compatibility
			oldScroll[i] = reinterpret_cast<WNDPROC>(SetWindowLong(hwndScroll[i],
				GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(ScrollProc)));

			hBrush[i] = CreateSolidBrush(crPrim[i]);
		}

		hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));

		cyChar = HIWORD(GetDialogBaseUnits());
		return 0;
	}
	case WM_SIZE:
	{
		auto cxClient = LOWORD(lParam);
		auto cyClient = HIWORD(lParam);

		SetRect(&rcColor, cxClient / 2, 0, cxClient, cyClient);

		MoveWindow(hwndRect, 0, 0, cxClient / 2, cyClient, true);

		for (auto i = 0; i < 3; i++)
		{
			MoveWindow(hwndScroll[i], (2 * i + 1) * cxClient / 14, 2 * cyChar,
				cxClient / 14, cyClient - 4 * cyChar, true);

			MoveWindow(hwndLabel[i], (4 * i + 1) * cxClient / 28, cyChar / 2,
				cxClient / 7, cyChar, true);

			MoveWindow(hwndValue[i], (4 * i + 1) * cxClient / 28, cyClient - 3 * cyChar / 2,
				cyClient / 7, cyChar, true);
		}

		SetFocus(hwnd);

		return 0;
	}
	case WM_SETFOCUS:
	{
		SetFocus(hwndScroll[idFocus]);
		return 0;
	}
	case WM_VSCROLL:
	{
		// GetWindowLongPtr for x64 compatibility
		auto i = GetWindowLong(reinterpret_cast<HWND>(lParam), GWL_ID);

		switch (LOWORD(wParam))
		{
		case SB_PAGEDOWN:
			color[i] += 15;
		case SB_LINEDOWN:
		{
			color[i] = min(255, color[i] + 1);
			break;
		}
		case SB_PAGEUP:
			color[i] -= 15;
		case SB_LINEUP:
		{
			color[i] = max(0, color[i] - 1);
			break;
		}
		case SB_TOP:
		{
			color[i] = 0;
			break;
		}
		case SB_BOTTOM:
		{
			color[i] = 255;
			break;
		}
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
		{
			color[i] = HIWORD(wParam);
			break;
		}
		default:
			break;
		}

		TCHAR szBuffer[10];

		SetScrollPos(hwndScroll[i], SB_CTL, color[i], true);
		wsprintf(szBuffer, TEXT("%i"), color[i]);
		SetWindowText(hwndValue[i], szBuffer);

		// SetClassLongPtr for x64 compatibility
		DeleteObject(reinterpret_cast<HBRUSH>(SetClassLong(hwnd, GCLP_HBRBACKGROUND,
			reinterpret_cast<LONG_PTR>(CreateSolidBrush(RGB(color[0], color[1], color[2]))))));

		InvalidateRect(hwnd, &rcColor, true);
		return 0;
	}
	case WM_CTLCOLORSCROLLBAR:
	{
		// GetWindowLongPtr for x64 compatibility
		auto i = GetWindowLong(reinterpret_cast<HWND>(lParam), GWL_ID);

		return reinterpret_cast<LRESULT>(hBrush[i]);
	}
	case WM_CTLCOLORSTATIC:
	{
		// GetWindowLongPtr for x64 compatibility
		auto i = GetWindowLong(reinterpret_cast<HWND>(lParam), GWL_ID);

		if (i >= 3 && i <= 8)
		{
			SetTextColor(reinterpret_cast<HDC>(wParam), crPrim[i % 3]);
			SetBkColor(reinterpret_cast<HDC>(wParam), GetSysColor(COLOR_BTNHIGHLIGHT));

			return reinterpret_cast<LRESULT>(hBrushStatic);
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	case WM_SYSCOLORCHANGE:
	{
		DeleteObject(hBrushStatic);
		hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));
		return 0;
	}
	case WM_DESTROY:
	{
		// SetClassLongPtr for x64 compatibility
		DeleteObject(reinterpret_cast<HBRUSH>(SetClassLong(hwnd, GCLP_HBRBACKGROUND,
			reinterpret_cast<LONG_PTR>(GetStockObject(WHITE_BRUSH)))));

		for (auto i = 0; i < 3; i++)
			DeleteObject(hBrush[i]);

		DeleteObject(hBrushStatic);
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK ScrollProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// GetWindowLongPtr for x64 compatibility
	auto id = GetWindowLong(hwnd, GWL_ID);

	switch (message)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_TAB)
			SetFocus(GetDlgItem(GetParent(hwnd),
			(id + (GetKeyState(VK_SHIFT) < 0 ? 2 : 1)) % 3));

		break;
	}
	case WM_SETFOCUS:
	{
		idFocus = id;
		break;
	}
	default:
		break;
	}

	return oldScroll[id](hwnd, message, wParam, lParam);
}