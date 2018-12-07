#include <windows.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ChildWndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR szChildClass[] = TEXT("Checker4_Child");
int idFocus = 0;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("Checker4");
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

	wndclass.lpfnWndProc = ChildWndProc;
	//wndclass.cbWndExtra = sizeof(LONG_PTR); for compatibility with x64
	wndclass.cbWndExtra = sizeof(long);
	wndclass.hIcon = nullptr;
	wndclass.lpszClassName = szChildClass;

	RegisterClass(&wndclass);

	auto hwnd = CreateWindow(szAppName, TEXT("Checker Mouse Hit-Test Demo No. 4"),
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
	static HWND hwndChild[DIVISIONS][DIVISIONS];

	switch (message)
	{
	case WM_CREATE:
	{
		for (auto y = 0; y < DIVISIONS; y++)
		{
			for (auto x = 0; x < DIVISIONS; x++)
			{
				hwndChild[x][y] = CreateWindow(szChildClass, nullptr,
					WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd,
					reinterpret_cast<HMENU>(y << 8 | x),
					reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hwnd, GWLP_HINSTANCE)), nullptr);
			}
		}
		return 0;
	}
	case WM_SIZE:
	{
		auto cxBlock = LOWORD(lParam) / DIVISIONS;
		auto cyBlock = HIWORD(lParam) / DIVISIONS;

		for (auto y = 0; y < DIVISIONS; y++)
		{
			for (auto x = 0; x < DIVISIONS; x++)
			{
				MoveWindow(hwndChild[x][y], x * cxBlock, y * cyBlock, cxBlock, cyBlock, true);
			}
		}
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		MessageBeep(0);
		return 0;
	}
	case WM_SETFOCUS:
	{
		SetFocus(GetDlgItem(hwnd, idFocus));
		return 0;
	}
	case WM_KEYDOWN:
	{
		auto x = idFocus & 0xff;
		auto y = idFocus >> 8;

		switch (wParam)
		{
		case VK_UP:
			y--;
			break;
		case VK_DOWN:
			y++;
			break;
		case VK_LEFT:
			x--;
			break;
		case VK_RIGHT:
			x++;
			break;
		case VK_HOME:
			x = 0;
			y = 0;
			break;
		case VK_END:
			x = DIVISIONS - 1;
			y = DIVISIONS - 1;
			break;
		default:
			return 0;
		}

		x = (x + DIVISIONS) % DIVISIONS;
		y = (y + DIVISIONS) % DIVISIONS;

		idFocus = y << 8 | x;

		SetFocus(GetDlgItem(hwnd, idFocus));
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

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		// SetWindowLongPtr(hwnd, 0, 0); for compatibility with x64
		SetWindowLong(hwnd, 0, 0);
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam != VK_RETURN && wParam != VK_SPACE)
		{
			SendMessage(GetParent(hwnd), message, wParam, lParam);
			return 0;
		}
	}
	case WM_LBUTTONDOWN:
	{
		// SetWindowLongPtr(hwnd, 0, 1 ^ GetWindowLongPtr(hwnd, 0)); for compatibility with x64
		SetWindowLong(hwnd, 0, 1 ^ GetWindowLongPtr(hwnd, 0));
		SetFocus(hwnd);
		InvalidateRect(hwnd, nullptr, true);
		return 0;
	}
	case WM_SETFOCUS:
	{
		// GetWindowLongPtr(hwnd, GWL_ID); for compatibility with x64
		idFocus = GetWindowLongPtr(hwnd, GWL_ID);
	}
	case WM_KILLFOCUS:
	{
		InvalidateRect(hwnd, nullptr, true);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		RECT rect;

		auto hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		Rectangle(hdc, 0, 0, rect.right, rect.bottom);

		// GetWindowLongPtr(hwnd, 0) for compatibility with x64
		if (GetWindowLongPtr(hwnd, 0))
		{
			MoveToEx(hdc, 0, 0, nullptr);
			LineTo(hdc, rect.right, rect.bottom);
			MoveToEx(hdc, 0, rect.bottom, nullptr);
			LineTo(hdc, rect.right, 0);
		}

		if (hwnd == GetFocus())
		{
			rect.left += rect.right / 10;
			rect.right -= rect.left;
			rect.top += rect.bottom / 10;
			rect.bottom -= rect.top;

			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			SelectObject(hdc, CreatePen(PS_DASH, 0, 0));
			Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
			DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
		}

		EndPaint(hwnd, &ps);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}