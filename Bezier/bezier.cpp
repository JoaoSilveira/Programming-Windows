#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("Bezier");
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

	auto hwnd = CreateWindow(appName, TEXT("Beier Splines"), WS_OVERLAPPEDWINDOW,
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

void DrawBezier(HDC hdc, POINT apt[])
{
	PolyBezier(hdc, apt, 4);

	MoveToEx(hdc, apt[0].x, apt[0].y, nullptr);
	LineTo(hdc, apt[1].x, apt[1].y);

	MoveToEx(hdc, apt[2].x, apt[2].y, nullptr);
	LineTo(hdc, apt[3].x, apt[3].y);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static POINT apt[4];

	switch (message)
	{
	case WM_SIZE:
	{
		auto cxClient = LOWORD(lParam);
		auto cyClient = HIWORD(lParam);

		apt[0].x = cxClient / 4;
		apt[0].y = cyClient / 2;

		apt[1].x = cxClient / 2;
		apt[1].y = cyClient / 4;

		apt[2].x = cxClient / 2;
		apt[2].y = 3 * cyClient / 4;

		apt[3].x = 3 * cxClient / 4;
		apt[3].y = cyClient / 2;
		return 0;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	{
		if (!(wParam & MK_LBUTTON || wParam & MK_RBUTTON))
			return 0;

		auto hdc = GetDC(hwnd);

		SelectObject(hdc, GetStockObject(WHITE_PEN));

		DrawBezier(hdc, apt);

		if (wParam & MK_LBUTTON)
		{
			apt[1].x = LOWORD(lParam);
			apt[1].y = HIWORD(lParam);
		}

		if (wParam & MK_RBUTTON)
		{
			apt[2].x = LOWORD(lParam);
			apt[2].y = HIWORD(lParam);
		}

		SelectObject(hdc, GetStockObject(BLACK_PEN));
		DrawBezier(hdc, apt);
		ReleaseDC(hwnd, hdc);

		return 0;
	}
	case WM_PAINT:
	{
		InvalidateRect(hwnd, nullptr, true);

		PAINTSTRUCT ps;
		auto hdc = BeginPaint(hwnd, &ps);

		DrawBezier(hdc, apt);

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