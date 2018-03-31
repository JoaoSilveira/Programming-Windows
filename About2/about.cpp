#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

typedef struct
{
	int color;
	int figure;
}ABOUTBOX_DATA;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow) {
	static TCHAR appName[] = TEXT("About2");
	MSG msg;
	WNDCLASS wndClass;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, appName);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndClass.lpszMenuName = appName;
	wndClass.lpszClassName = appName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(nullptr, TEXT("This program requires Windows NT!"), appName, MB_ICONERROR);
		return 0;
	}

	auto hwnd = CreateWindow(appName, TEXT("About Box Demo Program"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void PaintWindow(HWND hwnd, int color, int figure)
{
	static COLORREF colors[] = {
		RGB(0, 0, 0), RGB(0, 0, 255), RGB(0, 255, 0), RGB(0, 255, 255),
		RGB(255, 0, 0), RGB(255, 0, 255), RGB(255, 255, 0), RGB(255, 255, 255)
	};
	RECT rect;

	auto hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	auto hBrush = CreateSolidBrush(colors[color - IDC_BLACK]);
	hBrush = static_cast<HBRUSH>(SelectObject(hdc, hBrush));

	if (figure == IDC_RECT)
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	else
		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);

	DeleteObject(SelectObject(hdc, hBrush));
	ReleaseDC(hwnd, hdc);
}

void PaintTheBlock(HWND hwnd, int color, int figure)
{
	InvalidateRect(hwnd, nullptr, true);
	UpdateWindow(hwnd);
	PaintWindow(hwnd, color, figure);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	static ABOUTBOX_DATA data = {IDC_BLACK, IDC_RECT};

	switch (message)
	{
	case WM_CREATE:
	{
		hInstance = reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance;
		return 0;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_APP_ABOUT:
		{
			if (DialogBoxParam(hInstance, TEXT("AboutBox"), hwnd, AboutDlgProc, reinterpret_cast<LPARAM>(&data)))
				InvalidateRect(hwnd, nullptr, true);
			return 0;
		}
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);

		PaintWindow(hwnd, data.color, data.figure);
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

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hCtrlBlock;
	static ABOUTBOX_DATA *param, data;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		param = reinterpret_cast<ABOUTBOX_DATA*>(lParam);
		data = *param;

		CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, data.color);
		CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, data.figure);

		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);

		SetFocus(GetDlgItem(hDlg, data.color));
		return false;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			*param = data;
			EndDialog(hDlg, true);
			return true;
		}
		case IDCANCEL:
		{
			EndDialog(hDlg, false);
			return true;
		}
		case IDC_BLACK:
		case IDC_RED:
		case IDC_GREEN:
		case IDC_YELLOW:
		case IDC_BLUE:
		case IDC_MAGENTA:
		case IDC_CYAN:
		case IDC_WHITE:
		{
			data.color = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, data.color);
			PaintTheBlock(hCtrlBlock, data.color, data.figure);
			return true;
		}
		case IDC_RECT:
		case IDC_ELLIPSE:
		{
			data.figure = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, data.figure);
			PaintTheBlock(hCtrlBlock, data.color, data.figure);
			return true;
		}
		default:
			return false;
		}
	}
	case WM_PAINT:
		PaintTheBlock(hCtrlBlock, data.color, data.figure);
	default:
		return false;
	}
}