#include <windows.h>
#include <cmath>

#define TWO_PI 6.283185307179586476925286766559

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("Clover");
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

	auto hwnd = CreateWindow(appName, "Draw a Clover", WS_OVERLAPPEDWINDOW,
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
	static int cxClient, cyClient;
	static HRGN rgnClip;

	switch (message)
	{
	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		auto hcursor = SetCursor(LoadCursor(nullptr, IDC_WAIT));
		ShowCursor(true);

		if (rgnClip)
			DeleteObject(rgnClip);

		HRGN temp[6];

		temp[0] = CreateEllipticRgn(0, cyClient / 3, cxClient / 2, 2 * cyClient / 3);
		temp[1] = CreateEllipticRgn(cxClient / 2, cyClient / 3, cxClient, 2 * cyClient / 3);
		temp[2] = CreateEllipticRgn(cxClient / 3, 0, 2 * cxClient / 3, cyClient / 2);
		temp[3] = CreateEllipticRgn(cxClient / 3, cyClient / 2, 2 * cxClient / 3, cyClient);
		temp[4] = CreateRectRgn(0, 0, 1, 1);
		temp[5] = CreateRectRgn(0, 0, 1, 1);
		rgnClip = CreateRectRgn(0, 0, 1, 1);

		CombineRgn(temp[4], temp[0], temp[1], RGN_OR);
		CombineRgn(temp[5], temp[2], temp[3], RGN_OR);
		CombineRgn(rgnClip, temp[4], temp[5], RGN_XOR);

		for (auto i = 0; i < 6; i++)
			DeleteObject(temp[i]);

		SetCursor(hcursor);
		ShowCursor(false);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		auto hdc = BeginPaint(hwnd, &ps);

		SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, nullptr);
		SelectClipRgn(hdc, rgnClip);

		auto radius = _hypot(cxClient / 2.0, cyClient / 2.0);

		for (auto angle = 0.0; angle < TWO_PI; angle += TWO_PI / 360.0)
		{
			MoveToEx(hdc, 0, 0, nullptr);
			LineTo(hdc, static_cast<int>(radius * cos(angle) + 0.5),
				static_cast<int>(-radius * sin(angle) + 0.5));
		}

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
	{
		DeleteObject(rgnClip);
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}