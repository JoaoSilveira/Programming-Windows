#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("BlacOut");
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

	auto hwnd = CreateWindow(appName, "Mouse Button Demo", WS_OVERLAPPEDWINDOW,
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

void DrawBoxOutline(HWND hwnd, POINT ptBegin, POINT ptEnd)
{
	auto hdc = GetDC(hwnd);

	SetROP2(hdc, R2_NOT);
	SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Rectangle(hdc, ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y);

	ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool blocking, validBox;
	static POINT ptBegin, ptEnd, ptBoxBegin, ptBoxEnd;

	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		ptBegin.x = ptEnd.x = LOWORD(lParam);
		ptBegin.y = ptEnd.y = HIWORD(lParam);

		DrawBoxOutline(hwnd, ptBegin, ptEnd);

		SetCursor(LoadCursor(nullptr, IDC_CROSS));

		blocking = true;

		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (blocking)
		{
			SetCursor(LoadCursor(nullptr, IDC_CROSS));

			DrawBoxOutline(hwnd, ptBegin, ptEnd);

			ptEnd.x = LOWORD(lParam);
			ptEnd.y = HIWORD(lParam);

			DrawBoxOutline(hwnd, ptBegin, ptEnd);
		}

		return 0;
	}
	case WM_LBUTTONUP:
	{
		if (blocking)
		{
			DrawBoxOutline(hwnd, ptBegin, ptEnd);

			ptBoxBegin = ptBegin;
			ptBoxEnd.x = LOWORD(lParam);
			ptBoxEnd.y = HIWORD(lParam);

			SetCursor(LoadCursor(nullptr, IDC_ARROW));

			blocking = false;
			validBox = true;

			InvalidateRect(hwnd, nullptr, true);
		}
		return 0;
	}
	case WM_CHAR:
	{
		if (blocking && wParam == '\x1b') // escape
		{
			DrawBoxOutline(hwnd, ptBegin, ptEnd);

			SetCursor(LoadCursor(nullptr, IDC_ARROW));

			blocking = false;
		}
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		auto hdc = BeginPaint(hwnd, &ps);

		if (validBox)
		{
			SelectObject(hdc, GetStockObject(BLACK_BRUSH));
			Rectangle(hdc, ptBoxBegin.x, ptBoxBegin.y, ptBoxEnd.x, ptBoxEnd.y);
		}

		if (blocking)
		{
			SetROP2(hdc, R2_NOT);
			SelectObject(hdc, GetStockObject(NULL_BRUSH));
			Rectangle(hdc, ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y);
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