#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	TCHAR appName[16], caption[64], errMsg[64];
	MSG msg;
	WNDCLASS wndClass;

	LoadString(hInstance, IDS_APPNAME, appName, sizeof(appName) / sizeof(TCHAR));
	LoadString(hInstance, IDS_CAPTION, caption, sizeof(caption) / sizeof(TCHAR));

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, appName);
	// LoadIcon is obsolete use LoadImage
	//wndClass.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON), IMAGE_ICON,
	//	0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_COPYFROMRESOURCE));
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = appName;

	if (!RegisterClass(&wndClass))
	{
		LoadStringA(hInstance, IDS_APPNAME, static_cast<char*>(appName), sizeof(appName));
		LoadStringA(hInstance, IDS_ERRMSG, static_cast<char*>(errMsg), sizeof(errMsg));

		MessageBoxA(nullptr, static_cast<const char*>(errMsg),
			static_cast<const char*>(appName), MB_ICONERROR);
		return 0;
	}

	auto hwnd = CreateWindow(appName, caption, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr, nullptr, hInstance, nullptr);

	hInst = hInstance;

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
	static char* pText;
	static HGLOBAL hResource;
	static HWND hScroll;
	static int iPosition, cxChar, cyChar, cyClient, iNumLines, xScroll;

	switch (message)
	{
	case WM_CREATE:
	{
		TEXTMETRIC tm;

		auto hdc = GetDC(hwnd);

		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		ReleaseDC(hwnd, hdc);

		xScroll = GetSystemMetrics(SM_CXVSCROLL);

		hScroll = CreateWindow(TEXT("scrollbar"), nullptr,
			WS_CHILD | WS_VISIBLE | SBS_VERT, 0, 0, 0, 0,
			hwnd, reinterpret_cast<HMENU>(1), hInst, nullptr);

		hResource = LoadResource(hInst,
			FindResource(hInst, TEXT("AnnabelLee"), TEXT("TEXT")));

		pText = static_cast<char*>(LockResource(hResource));
		iNumLines = 0;

		while (*pText != '\\' && *pText != '\0')
		{
			if (*pText == '\n')
				iNumLines++;

			pText = AnsiNext(pText);
		}

		SetScrollRange(hScroll, SB_CTL, 0, iNumLines, false);
		SetScrollPos(hScroll, SB_CTL, 0, false);

		return 0;
	}
	case WM_SIZE:
	{
		cyClient = HIWORD(lParam);
		MoveWindow(hScroll, LOWORD(lParam) - xScroll, 0, xScroll, cyClient, true);
		SetFocus(hwnd);

		return 0;
	}
	case WM_SETFOCUS:
	{
		SetFocus(hScroll);
		return 0;
	}
	case WM_VSCROLL:
	{
		switch (LOWORD(wParam))
		{
		case SB_TOP:
			iPosition = 0;
			break;
		case SB_BOTTOM:
			iPosition = iNumLines;
			break;
		case SB_LINEUP:
			iPosition--;
			break;
		case SB_LINEDOWN:
			iPosition++;
			break;
		case SB_PAGEUP:
			iPosition -= cyClient / cyChar;
			break;
		case SB_PAGEDOWN:
			iPosition += cyClient / cyChar;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			iPosition = HIWORD(wParam);
			break;
		default:
			break;
		}

		iPosition = max(0, min(iNumLines, iPosition));

		if (iPosition != GetScrollPos(hScroll, SB_CTL))
		{
			SetScrollPos(hScroll, SB_CTL, iPosition, true);
			InvalidateRect(hwnd, nullptr, true);
		}

		return 0;
	}
	case WM_PAINT:
	{
		RECT rc;
		PAINTSTRUCT ps;
		auto hdc = BeginPaint(hwnd, &ps);

		pText = static_cast<char*>(LockResource(hResource));

		GetClientRect(hwnd, &rc);
		rc.left += cxChar;
		rc.top += cyChar * (1 - iPosition);

		DrawTextA(hdc, pText, -1, &rc, DT_EXTERNALLEADING);

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
	{
		FreeResource(hResource);
		PostQuitMessage(0);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}