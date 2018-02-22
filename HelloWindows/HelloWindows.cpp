#include <Windows.h>
#include <mutex>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR args, int cmdShow)
{
	static TCHAR appName[] = TEXT("Hello, Windows");
	WNDCLASS wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = instance;
	wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = appName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(nullptr, TEXT("The program requires Windows NT!"), appName, MB_OK | MB_ICONERROR);

		return 0;
	}

	auto hwnd = CreateWindow(appName,		// Window Class name
		TEXT("The Windows Hello Program"),	// Window caption
		WS_OVERLAPPEDWINDOW,				// Window style
		CW_USEDEFAULT,						// window initial x
		CW_USEDEFAULT,						// window initial y
		CW_USEDEFAULT,						// window initial width
		CW_USEDEFAULT,						// window initial height
		NULL,								// window parent
		NULL,								// window menu
		instance,							// program instance
		NULL);								// creation parameters

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	MSG message;
	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return message.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		PlaySound(TEXT("hellowin.wav"), nullptr, SND_FILENAME | SND_ASYNC);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		RECT rect;

		auto hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		DrawText(hdc, TEXT("Hello, Windows!!!"), -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
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