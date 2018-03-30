#include <windows.h>

#define IDM_SYS_ABOUT  1
#define IDM_SYS_HELP   2
#define IDM_SYS_REMOVE 3

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static TCHAR appName[] = TEXT("PoorMenu");

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	WNDCLASS wndClass;
	MSG msg;

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

	auto hwnd = CreateWindow(appName, TEXT("The Poor-Person's Menu"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

	auto hMenu = GetSystemMenu(hwnd, false);

	AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
	AppendMenu(hMenu, MF_STRING, IDM_SYS_ABOUT, TEXT("About..."));
	AppendMenu(hMenu, MF_STRING, IDM_SYS_HELP, TEXT("Help..."));
	AppendMenu(hMenu, MF_STRING, IDM_SYS_REMOVE, TEXT("Remove Additions"));

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
	switch (message)
	{
	case WM_SYSCOMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_SYS_ABOUT:
		{
			MessageBox(hwnd, TEXT("A Poor-Person's Menu Program\n")
				TEXT("(c) Charles Petzold, 1998"), appName,
				MB_OK | MB_ICONINFORMATION);
			return 0;
		}
		case IDM_SYS_HELP:
		{
			MessageBox(hwnd, TEXT("Help not yet implemented!"),
				appName, MB_OK | MB_ICONEXCLAMATION);
			return 0;
		}
		case IDM_SYS_REMOVE:
		{
			GetSystemMenu(hwnd, true);
			return 0;
		}
		default:
			break;
		}
		return DefWindowProc(hwnd, message, wParam, lParam);
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