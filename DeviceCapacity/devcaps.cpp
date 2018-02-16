#include <windows.h>

#define NUM_LINES (static_cast<int>(sizeof(devcaps) / sizeof(devcaps[0])))

struct
{
	int iIndex;
	TCHAR* szLabel;
	TCHAR* szDesc;
} devcaps[] = {
	HORZSIZE, TEXT("HORZSIZE"), TEXT("Width in millimeters:"),
	VERTSIZE, TEXT("VERTSIZE"), TEXT("Height in millimeters:"),
	HORZRES, TEXT("HORZRES"), TEXT("Width in pixels:"),
	VERTRES, TEXT("VERTRES"), TEXT("Height in raster lines:"),
	BITSPIXEL, TEXT("BITSPIXEL"), TEXT("Color bits per pixel:"),
	PLANES, TEXT("PLANES"), TEXT("Number of color planes:"),
	NUMBRUSHES, TEXT("NUMBRUSHES"), TEXT("Number of device brushes:"),
	NUMPENS, TEXT("NUMPENS"), TEXT("Number of device pens:"),
	NUMMARKERS, TEXT("NUMMARKERS"), TEXT("Number of device markers:"),
	NUMFONTS, TEXT("NUMFONTS"), TEXT("Number of device fonts:"),
	NUMCOLORS, TEXT("NUMCOLORS"), TEXT("Number of device colors:"),
	PDEVICESIZE, TEXT("PDEVICESIZE"), TEXT("Size of device structure:"),
	ASPECTX, TEXT("ASPECTX"), TEXT("Relative width of pixel:"),
	ASPECTY, TEXT("ASPECTY"), TEXT("Relative height of pixel:"),
	ASPECTXY, TEXT("ASPECTXY"), TEXT("Relative diagonal of pixel:"),
	LOGPIXELSX, TEXT("LOGPIXELSX"), TEXT("Horizontal dots per inch:"),
	LOGPIXELSY, TEXT("LOGPIXELSY"), TEXT("Vertical dots per inch:"),
	SIZEPALETTE, TEXT("SIZEPALETTE"), TEXT("Number of palette entries:"),
	NUMRESERVED, TEXT("NUMRESERVED"), TEXT("Reserved palette entries:"),
	COLORRES, TEXT("COLORRES"), TEXT("Actual color resolution:")
};

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("DevCaps1");
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WinProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndclass.lpszMenuName = nullptr;
	wndclass.lpszClassName = appName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(nullptr, TEXT("This program requires Windows NT!"), appName, MB_ICONERROR);
		return 0;
	}

	auto hwnd = CreateWindow(appName, TEXT("Device Capabilities"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxChar, cxCaps, cyChar;
	TCHAR buffer[10];
	HDC hdc;
	PAINTSTRUCT ps;
	TEXTMETRIC tm;

	switch (message)
	{
	case WM_CREATE:
	{
		hdc = GetDC(hwnd);

		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		ReleaseDC(hwnd, hdc);
		return 0;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);

		for (auto i = 0; i < NUM_LINES; i++)
		{
			TextOut(hdc, 0, cyChar * i, devcaps[i].szLabel, lstrlen(devcaps[i].szLabel));
			TextOut(hdc, 14 * cxCaps, cyChar * i, devcaps[i].szDesc, lstrlen(devcaps[i].szDesc));

			SetTextAlign(hdc, TA_RIGHT | TA_TOP);
			TextOut(hdc, 14 * cxCaps + 35 * cxChar, cyChar * i, buffer,
				wsprintf(buffer, TEXT("%5d"), GetDeviceCaps(hdc, devcaps[i].iIndex)));
			SetTextAlign(hdc, TA_LEFT | TA_TOP);
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