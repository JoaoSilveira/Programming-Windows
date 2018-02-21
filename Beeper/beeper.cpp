#include <windows.h>

#define ID_TIMER 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("Beeper");
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

	auto hwnd = CreateWindow(appName, "Beeper Timer Demo No. 1", WS_OVERLAPPEDWINDOW,
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
	static auto flipFlop = false;

	switch (message)
	{
	case WM_CREATE:
	{
		SetTimer(hwnd, ID_TIMER, 1000, nullptr);
		return 0;
	}
	case WM_TIMER:
	{
		MessageBeep(-1);
		flipFlop = !flipFlop;
		InvalidateRect(hwnd, nullptr, true);
		return 0;
	}
	case WM_PAINT:
	{
		RECT rect;
		PAINTSTRUCT ps;
		auto hdc = BeginPaint(hwnd, &ps);

		GetClientRect(hwnd, &rect);
		auto brush = CreateSolidBrush(flipFlop ? RGB(255, 0, 0) : RGB(0, 0, 255));

		FillRect(hdc, &rect, brush);

		DeleteObject(brush);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
	{
		KillTimer(hwnd, ID_TIMER);
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}