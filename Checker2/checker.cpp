#include <windows.h>

#define DIVISIONS 5

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("Checker2");
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

	auto hwnd = CreateWindow(szAppName, TEXT("Checker Mouse Hit-Test Demo No. 2"),
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
	static bool states[DIVISIONS][DIVISIONS];
	static int cxBlock, cyBlock;

	switch (message)
	{
	case WM_SIZE:
	{
		cxBlock = LOWORD(lParam) / DIVISIONS;
		cyBlock = HIWORD(lParam) / DIVISIONS;

		return 0;
	}
	case WM_SETFOCUS:
	{
		ShowCursor(true);
		return 0;
	}
	case WM_KILLFOCUS:
	{
		ShowCursor(false);
		return 0;
	}
	case WM_KEYDOWN:
	{
		POINT point;

		GetCursorPos(&point);
		ScreenToClient(hwnd, &point);

		auto x = max(0, min(DIVISIONS - 1, point.x / cxBlock));
		auto y = max(0, min(DIVISIONS - 1, point.y / cyBlock));

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
		case VK_RETURN:
		case VK_SPACE:
			SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x * cxBlock, y * cyBlock));
			break;
		default:
			break;
		}

		x = (x + DIVISIONS) % DIVISIONS;
		y = (y + DIVISIONS) % DIVISIONS;

		point.x = x * cxBlock + cxBlock / 2;
		point.y = y * cyBlock + cyBlock / 2;

		ClientToScreen(hwnd, &point);
		SetCursorPos(point.x, point.y);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		auto x = LOWORD(lParam) / cxBlock;
		auto y = HIWORD(lParam) / cyBlock;

		if (x < DIVISIONS && y < DIVISIONS)
		{
			states[x][y] ^= 1;

			RECT rc;

			SetRect(&rc, cxBlock * x, cyBlock * y, cxBlock * (x + 1), cyBlock * (y + 1));
			InvalidateRect(hwnd, &rc, true);
		}
		else
		{
			MessageBeep(0);
		}

		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		auto hdc = BeginPaint(hwnd, &ps);

		for (auto y = 0; y < DIVISIONS; y++)
		{
			for (auto x = 0; x < DIVISIONS; x++)
			{
				Rectangle(hdc, cxBlock * x, cyBlock * y, cxBlock * (x + 1), cyBlock * (y + 1));

				if (states[x][y])
				{
					MoveToEx(hdc, cxBlock * x, cyBlock * y, nullptr);
					LineTo(hdc, cxBlock * (x + 1), cyBlock * (y + 1));

					MoveToEx(hdc, cxBlock * x, cyBlock * (y + 1), nullptr);
					LineTo(hdc, cxBlock * (x + 1), cyBlock * y);
				}
			}
		}

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