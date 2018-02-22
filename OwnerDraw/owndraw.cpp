#include <windows.h>

#define	ID_SMALLER 1
#define ID_LARGER 2
#define BTN_WIDTH (8 * cxChar)
#define BTN_HEIGHT (4 * cyChar)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("OwnDraw");
	MSG msg;
	WNDCLASS wndClass;

	hInst = hInstance;

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

	auto hwnd = CreateWindow(appName, TEXT("Owner-draw Button Demo"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

void Triangle(HDC hdc, POINT pt[])
{
	SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	Polygon(hdc, pt, 3);
	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndSmaller, hwndLarger;
	static int cxClient, cyClient, cxChar, cyChar;

	switch (message)
	{
	case WM_CREATE:
	{
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		hwndSmaller = CreateWindow(TEXT("button"), TEXT(""),
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, 0,
			BTN_WIDTH, BTN_HEIGHT, hwnd, reinterpret_cast<HMENU>(ID_SMALLER), hInst, nullptr);

		hwndLarger = CreateWindow(TEXT("button"), TEXT(""),
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0, 0,
			BTN_WIDTH, BTN_HEIGHT, hwnd, reinterpret_cast<HMENU>(ID_LARGER), hInst, nullptr);

		return 0;
	}
	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		MoveWindow(hwndSmaller, cxClient / 2 - 3 * BTN_WIDTH / 2,
			cyClient / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, true);

		MoveWindow(hwndLarger, cxClient / 2 + 3 * BTN_WIDTH / 2,
			cyClient / 2 - BTN_HEIGHT / 2, BTN_WIDTH, BTN_HEIGHT, true);

		return 0;
	}
	case WM_COMMAND:
	{
		RECT rc;
		GetWindowRect(hwnd, &rc);

		switch (wParam)
		{
		case ID_SMALLER:
		{
			rc.left += cxClient / 20;
			rc.right -= cxClient / 20;
			rc.top += cyClient / 20;
			rc.bottom -= cyClient / 20;

			break;
		}
		case ID_LARGER:
		{
			rc.left -= cxClient / 20;
			rc.right += cxClient / 20;
			rc.top -= cyClient / 20;
			rc.bottom += cyClient / 20;

			break;
		}
		default:
			break;
		}

		MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, true);

		return 0;
	}
	case WM_DRAWITEM:
	{
		POINT pt[3];
		auto pdis = reinterpret_cast<LPDRAWITEMSTRUCT>(lParam);

		FillRect(pdis->hDC, &pdis->rcItem, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

		FrameRect(pdis->hDC, &pdis->rcItem, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

		auto cx = pdis->rcItem.right - pdis->rcItem.left;
		auto cy = pdis->rcItem.bottom - pdis->rcItem.top;

		switch (pdis->CtlID)
		{
		case ID_SMALLER:
		{
			pt[0].x = 3 * cx / 8; pt[0].y = 1 * cy / 8;
			pt[1].x = 5 * cx / 8; pt[1].y = 1 * cy / 8;
			pt[2].x = 4 * cx / 8; pt[2].y = 3 * cy / 8;

			Triangle(pdis->hDC, pt);

			pt[0].x = 7 * cx / 8; pt[0].y = 3 * cy / 8;
			pt[1].x = 7 * cx / 8; pt[1].y = 5 * cy / 8;
			pt[2].x = 5 * cx / 8; pt[2].y = 4 * cy / 8;

			Triangle(pdis->hDC, pt);

			pt[0].x = 5 * cx / 8; pt[0].y = 7 * cy / 8;
			pt[1].x = 3 * cx / 8; pt[1].y = 7 * cy / 8;
			pt[2].x = 4 * cx / 8; pt[2].y = 5 * cy / 8;

			Triangle(pdis->hDC, pt);

			pt[0].x = 1 * cx / 8; pt[0].y = 5 * cy / 8;
			pt[1].x = 1 * cx / 8; pt[1].y = 3 * cy / 8;
			pt[2].x = 3 * cx / 8; pt[2].y = 4 * cy / 8;

			Triangle(pdis->hDC, pt);
			break;
		}
		case ID_LARGER:
		{
			pt[0].x = 5 * cx / 8; pt[0].y = 3 * cy / 8;
			pt[1].x = 3 * cx / 8; pt[1].y = 3 * cy / 8;
			pt[2].x = 4 * cx / 8; pt[2].y = 1 * cy / 8;

			Triangle(pdis->hDC, pt);

			pt[0].x = 5 * cx / 8; pt[0].y = 5 * cy / 8;
			pt[1].x = 5 * cx / 8; pt[1].y = 3 * cy / 8;
			pt[2].x = 7 * cx / 8; pt[2].y = 4 * cy / 8;

			Triangle(pdis->hDC, pt);

			pt[0].x = 3 * cx / 8; pt[0].y = 5 * cy / 8;
			pt[1].x = 5 * cx / 8; pt[1].y = 5 * cy / 8;
			pt[2].x = 4 * cx / 8; pt[2].y = 7 * cy / 8;

			Triangle(pdis->hDC, pt);

			pt[0].x = 3 * cx / 8; pt[0].y = 3 * cy / 8;
			pt[1].x = 3 * cx / 8; pt[1].y = 5 * cy / 8;
			pt[2].x = 1 * cx / 8; pt[2].y = 4 * cy / 8;

			Triangle(pdis->hDC, pt);
			break;
		}
		default:
			break;
		}

		if (pdis->itemState & ODS_SELECTED)
			InvertRect(pdis->hDC, &pdis->rcItem);

		if (pdis->itemState & ODS_FOCUS)
		{
			pdis->rcItem.left += cx / 16;
			pdis->rcItem.right -= cx / 16;
			pdis->rcItem.top += cy / 16;
			pdis->rcItem.bottom -= cy / 16;

			DrawFocusRect(pdis->hDC, &pdis->rcItem);
		}
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