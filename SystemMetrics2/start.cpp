#include "SysMets.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("SysMets2");
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

	auto hwnd = CreateWindow(szAppName, TEXT("Get System Metrics No. 2"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL,
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
	static int cxChar, cyChar, cxCaps, cyClient, iVscrollPos = 0;

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

		SetScrollRange(hwnd, SB_VERT, 0, NumLines - 1, false);
		SetScrollPos(hwnd, SB_VERT, iVscrollPos, true);
		return 0;
	}
	case WM_SIZE:
	{
		cyClient = HIWORD(lParam);
		return 0;
	}
	case WM_VSCROLL:
	{
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			iVscrollPos--;
			break;
		case SB_LINEDOWN:
			iVscrollPos++;
			break;
		case SB_PAGEUP:
			iVscrollPos -= cyClient / cyChar;
			break;
		case SB_PAGEDOWN:
			iVscrollPos += cyClient / cyChar;
			break;
		case SB_THUMBPOSITION:
			iVscrollPos = HIWORD(wParam);
			break;
		case SB_THUMBTRACK:
			iVscrollPos = HIWORD(wParam);
			break;
		default:
			break;
		}

		iVscrollPos = max(0, min(iVscrollPos, NumLines - 1));

		if (iVscrollPos != GetScrollPos(hwnd, SB_VERT))
		{
			SetScrollPos(hwnd, SB_VERT, iVscrollPos, true);
			InvalidateRect(hwnd, nullptr, true);
		}
		return 0;
	}
	case WM_PAINT:
	{
		TCHAR szBuffer[10];
		PAINTSTRUCT ps;

		auto hdc = BeginPaint(hwnd, &ps);

		for (auto i = 0; i < NumLines; i++)
		{
			auto y = cyChar * (i - iVscrollPos);

			TextOut(hdc, 0, y, sysmetrics[i].szLabel, lstrlen(sysmetrics[i].szLabel));
			TextOut(hdc, 22 * cxCaps, y, sysmetrics[i].szDesc, lstrlen(sysmetrics[i].szDesc));
			SetTextAlign(hdc, TA_RIGHT | TA_TOP);
			TextOut(hdc, 22 * cxCaps + 40 * cxChar, y, szBuffer,
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