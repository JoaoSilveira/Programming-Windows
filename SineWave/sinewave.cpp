#include <windows.h>
#include <cmath>

#define NUM 1000
#define TWO_PI (6.283185307179586476925286766559)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("SineWave");
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

	auto hwnd = CreateWindow(appName, TEXT("Sine Wave Using Polyline"), WS_OVERLAPPEDWINDOW,
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

	switch (message)
	{
	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;
	}
	case WM_PAINT:
	{
		POINT apt[NUM];
		PAINTSTRUCT ps;

		auto hdc = BeginPaint(hwnd, &ps);

		MoveToEx(hdc, 0, cyClient / 2, nullptr);
		LineTo(hdc, cxClient, cyClient / 2);

		for (auto i = 0; i < NUM; i++)
		{
			apt[i].x = i * cxClient / NUM;
			apt[i].y = static_cast<int>(cyClient / 2 * (1 - sin(TWO_PI * i / NUM)));
		}
		Polyline(hdc, apt, NUM);

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