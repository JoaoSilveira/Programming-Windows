#include <windows.h>
#include <cstdlib>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawRectangle(HWND);

int cxClient;
int cyClient;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("RandRect");
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

	auto hwnd = CreateWindow(appName, TEXT("Random Rectangles"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				return msg.wParam;

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			continue;
		}

		DrawRectangle(hwnd);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
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

void DrawRectangle(HWND hwnd)
{
	RECT rect;

	if (cxClient == 0 || cyClient == 0)
		return;

	SetRect(&rect, rand() % cxClient, rand() % cyClient, rand() % cxClient, rand() % cyClient);

	auto hbrush = CreateSolidBrush(RGB(rand() & 255, rand() & 255, rand() & 255));

	auto hdc = GetDC(hwnd);

	FillRect(hdc, &rect, hbrush);

	ReleaseDC(hwnd, hdc);
	DeleteObject(hbrush);
}