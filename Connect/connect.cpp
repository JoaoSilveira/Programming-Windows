#include <windows.h>

#define MAX_POINTS 1000

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("Connect");
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

	auto hwnd = CreateWindow(szAppName, TEXT("Connect-the-Points Mouse Demo"),
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
	static POINT points[MAX_POINTS];
	static int count;

	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		count = 0;
		InvalidateRect(hwnd, nullptr, true);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam & MK_LBUTTON && count < MAX_POINTS)
		{
			points[count].x = LOWORD(lParam);
			points[count++].y = HIWORD(lParam);

			auto hdc = GetDC(hwnd);
			SetPixel(hdc, LOWORD(lParam), HIWORD(lParam), 0);
			ReleaseDC(hwnd, hdc);
		}
		return 0;
	}
	case WM_LBUTTONUP:
	{
		InvalidateRect(hwnd, nullptr, true);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		auto hdc = BeginPaint(hwnd, &ps);

		SetCursor(LoadCursor(nullptr, IDC_WAIT));
		ShowCursor(true);

		for (auto i = 0; i < count - 1; i++)
		{
			for (auto j = i + 1; j < count; j++)
			{
				MoveToEx(hdc, points[i].x, points[i].y, nullptr);
				LineTo(hdc, points[j].x, points[j].y);
			}
		}

		ShowCursor(false);
		SetCursor(LoadCursor(nullptr, IDC_ARROW));
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