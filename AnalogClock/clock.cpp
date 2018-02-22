#include <windows.h>
#include <cmath>

#define ID_TIMER 1
#define TWO_PI 6.283185307179586476925286766559

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("Clock");
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

	auto hwnd = CreateWindow(appName, TEXT("Analog Clock"), WS_OVERLAPPEDWINDOW,
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

void SetIsotropic(HDC hdc, int cxClient, int cyClient)
{
	SetMapMode(hdc, MM_ISOTROPIC);
	SetWindowExtEx(hdc, 1000, 1000, nullptr);
	SetViewportExtEx(hdc, cxClient / 2, -cyClient / 2, nullptr);
	SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, nullptr);
}

void RotatePoint(POINT pt[], int num, int angle)
{
	POINT aux;

	for (auto i = 0; i < num; i++)
	{
		aux.x = static_cast<int>(pt[i].x * cos(TWO_PI * angle / 360) + pt[i].y * sin(TWO_PI * angle / 360));
		aux.y = static_cast<int>(pt[i].y * cos(TWO_PI * angle / 360) - pt[i].x * sin(TWO_PI * angle / 360));

		pt[i] = aux;
	}
}

void DrawClock(HDC hdc)
{
	POINT pt[3];

	for (auto angle = 0; angle < 360; angle += 6)
	{
		pt[0].x = 0;
		pt[0].y = 900;

		RotatePoint(pt, 1, angle);

		pt[2].x = pt[2].y = angle % 5 ? 33 : 100;

		pt[0].x -= pt[2].x / 2;
		pt[0].y -= pt[2].y / 2;

		pt[1].x = pt[0].x + pt[2].x;
		pt[1].y = pt[0].y + pt[2].y;

		SelectObject(hdc, GetStockObject(BLACK_BRUSH));

		Ellipse(hdc, pt[0].x, pt[0].y, pt[1].x, pt[1].y);
	}
}

void DrawHands(HDC hdc, SYSTEMTIME* pst, bool change)
{
	static POINT pt[3][5] = {
		0, -150, 100, 0, 0, 600, -100, 0, 0, -150,
		0, -200, 50, 0, 0, 800, -50, 0, 0, -200,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 800
	};

	POINT ptTemp[3][5];

	int angles[] = {
		pst->wHour * 30 % 360 + pst->wMinute / 2,
		pst->wMinute * 6,
		pst->wSecond * 6
	};

	memcpy(ptTemp, pt, sizeof(pt));

	for (auto i = change ? 0 : 2; i < 3; i++)
	{
		RotatePoint(ptTemp[i], 5, angles[i]);

		Polyline(hdc, ptTemp[i], 5);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static SYSTEMTIME stPrevious;
	static int cxClient, cyClient;

	switch (message)
	{
	case WM_CREATE:
	{
		SetTimer(hwnd, ID_TIMER, 1000, nullptr);
		GetLocalTime(&stPrevious);

		return 0;
	}
	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		return 0;
	}
	case WM_TIMER:
	{
		SYSTEMTIME st;

		GetLocalTime(&st);

		auto change = st.wHour != stPrevious.wHour ||
			st.wMinute != stPrevious.wMinute;

		auto hdc = GetDC(hwnd);

		SetIsotropic(hdc, cxClient, cyClient);

		SelectObject(hdc, GetStockObject(WHITE_PEN));
		DrawHands(hdc, &stPrevious, change);

		SelectObject(hdc, GetStockObject(BLACK_PEN));
		DrawHands(hdc, &st, true);

		ReleaseDC(hwnd, hdc);
		stPrevious = st;

		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		auto hdc = BeginPaint(hwnd, &ps);

		SetIsotropic(hdc, cxClient, cyClient);

		DrawClock(hdc);
		DrawHands(hdc, &stPrevious, true);

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