#include "SysMets.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("SysMets4");
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclass.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wndclass.lpszMenuName = nullptr;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(nullptr, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);

		return 0;
	}

	auto hwnd = CreateWindow(szAppName, TEXT("Get System Metrics No. 4"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
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
	static int cxChar, cyChar, cxCaps, cxClient, cyClient, iMaxWidth;

	switch (message)
	{
	case WM_CREATE:
	{
		TEXTMETRIC tm;
		auto hdc = GetDC(hwnd);

		GetTextMetrics(hdc, &tm);

		cxChar = tm.tmAveCharWidth;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		ReleaseDC(hwnd, hdc);

		iMaxWidth = 40 * cxChar + 22 * cxCaps;
		return 0;
	}
	case WM_SIZE:
	{
		SCROLLINFO si;
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = NumLines - 1;
		si.nPage = cyClient / cyChar;
		SetScrollInfo(hwnd, SB_VERT, &si, true);

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = 2 + iMaxWidth / cxChar;
		si.nPage = cxClient / cxChar;
		SetScrollInfo(hwnd, SB_HORZ, &si, true);

		return 0;
	}
	case WM_VSCROLL:
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT, &si);

		auto iVertPos = si.nPos;

		switch (LOWORD(wParam))
		{
		case SB_TOP:
			si.nPos = si.nMin;
			break;
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;
		case SB_LINEUP:
			si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			si.nPos += 1;
			break;
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;
		case SB_THUMBPOSITION:
			si.nPos = si.nTrackPos;
			break;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		default:
			break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_VERT, &si, true);
		GetScrollInfo(hwnd, SB_VERT, &si);

		if (iVertPos != si.nPos)
		{
			ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos), nullptr, nullptr);
		}
		return 0;
	}
	case WM_HSCROLL:
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_HORZ, &si);

		auto iHorzPos = si.nPos;

		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
			si.nPos--;
			break;
		case SB_LINERIGHT:
			si.nPos++;
			break;
		case SB_PAGELEFT:
			si.nPos -= si.nPage;
			break;
		case SB_PAGERIGHT:
			si.nPos += si.nPage;
			break;
		case SB_THUMBPOSITION:
			si.nPos = si.nTrackPos;
			break;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		default:
			break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_HORZ, &si, true);
		GetScrollInfo(hwnd, SB_HORZ, &si);

		if (iHorzPos != si.nPos)
		{
			ScrollWindow(hwnd, cxChar * (iHorzPos - si.nPos), 0, nullptr, nullptr);
		}
		return 0;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_HOME:
			SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0);
			break;
		case VK_END:
			SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0);
			break;
		case VK_PRIOR:
			SendMessage(hwnd, WM_VSCROLL, SB_PAGEUP, 0);
			break;
		case VK_NEXT:
			SendMessage(hwnd, WM_VSCROLL, SB_PAGEDOWN, 0);
			break;
		case VK_UP:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
			break;
		case VK_DOWN:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
			break;
		case VK_LEFT:
			SendMessage(hwnd, WM_HSCROLL, SB_PAGEUP, 0);
			break;
		case VK_RIGHT:
			SendMessage(hwnd, WM_HSCROLL, SB_PAGEDOWN, 0);
			break;
		default:
			return 0;
		}
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		SCROLLINFO si;
		TCHAR szBuffer[10];

		auto hdc = BeginPaint(hwnd, &ps);
		RECT rc;

		GetWindowRect(hwnd, &rc);

		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS;
		GetScrollInfo(hwnd, SB_VERT, &si);
		auto iVertPos = si.nPos;

		GetScrollInfo(hwnd, SB_HORZ, &si);
		auto iHorzPos = si.nPos;

		int iPaintBeg = max(0, iVertPos + ps.rcPaint.top / cyChar);
		int iPaintEnd = min(NumLines - 1, iVertPos + ps.rcPaint.bottom / cyChar);

		if ((rc.bottom - rc.top) % cyChar)
			iPaintEnd = min(NumLines - 1, iPaintEnd + 1);

		for (auto i = iPaintBeg; i < iPaintEnd; i++)
		{
			auto x = cxChar * (1 - iHorzPos);
			auto y = cyChar * (i - iVertPos);

			TextOut(hdc, x, y, sysmetrics[i].szLabel, lstrlen(sysmetrics[i].szLabel));
			TextOut(hdc, x + 22 * cxCaps, y, sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));
			SetTextAlign(hdc, TA_RIGHT | TA_TOP);
			TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer,
				wsprintf(szBuffer, TEXT("%5d"), GetSystemMetrics(sysmetrics[i].iIndex)));
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