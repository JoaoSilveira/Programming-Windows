#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("StokFont");
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

	auto hwnd = CreateWindow(szAppName, TEXT("Stock Fonts"),
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
	static struct
	{
		int idStockFont;
		PTCHAR stockFont;
	} stockFont[] = {
		OEM_FIXED_FONT, "OEM_FIXED_FONT",
		ANSI_FIXED_FONT, "ANSI_FIXED_FONT",
		ANSI_VAR_FONT, "ANSI_VAR_FONT",
		SYSTEM_FONT, "SYSTEM_FONT",
		DEVICE_DEFAULT_FONT, "DEVICE_DEFAULT_FONT",
		SYSTEM_FIXED_FONT, "SYSTEM_FIXED_FONT",
		DEFAULT_GUI_FONT, "DEFAULT_GUI_FONT"
	};

	static int iFont, cFonts = sizeof(stockFont) / sizeof(stockFont[0]);

	switch (message)
	{
	case WM_CREATE:
	{
		SetScrollRange(hwnd, SB_VERT, 0, cFonts - 1, true);
		return 0;
	}
	case WM_DISPLAYCHANGE:
	{
		InvalidateRect(hwnd, nullptr, true);
		return 0;
	}
	case WM_VSCROLL:
	{
		switch (LOWORD(wParam))
		{
		case SB_TOP:
			iFont = 0;
			break;
		case SB_BOTTOM:
			iFont = cFonts - 1;
			break;
		case SB_PAGEUP:
		case SB_LINEUP:
			iFont--;
			break;
		case SB_PAGEDOWN:
		case SB_LINEDOWN:
			iFont++;
			break;
		case SB_THUMBPOSITION:
			iFont = HIWORD(wParam);
			break;
		default:
			break;
		}

		iFont = max(0, min(cFonts - 1, iFont));

		// Only update when changed
		//if (iFont != GetScrollPos(hwnd, SB_VERT))
		//{
		SetScrollPos(hwnd, SB_VERT, iFont, true);
		InvalidateRect(hwnd, nullptr, true);
		//}

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
		case VK_LEFT:
		case VK_UP:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0);
			break;
		case VK_NEXT:
		case VK_RIGHT:
		case VK_DOWN:
			SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0);
			break;
		default:
			break;
		}
		return 0;
	}
	case WM_PAINT:
	{
		TCHAR szFaceName[LF_FACESIZE], szBuffer[LF_FACESIZE + 64];
		PAINTSTRUCT ps;
		TEXTMETRIC tm;
		auto hdc = BeginPaint(hwnd, &ps);

		SelectObject(hdc, GetStockObject(stockFont[iFont].idStockFont));
		GetTextFace(hdc, LF_FACESIZE, szFaceName);
		GetTextMetrics(hdc, &tm);
		auto cxGrid = max(3 * tm.tmAveCharWidth, 2 * tm.tmMaxCharWidth);
		auto cyGrid = 3 + tm.tmHeight;

		TextOut(hdc, 0, 0, szBuffer,
			wsprintf(szBuffer, TEXT(" %s: Face name = %s, Charset = %i"),
				stockFont[iFont].stockFont, szFaceName, tm.tmCharSet));

		SetTextAlign(hdc, TA_TOP | TA_CENTER);

		// vertical and horizontal lines
		for (auto i = 0; i < 17; i++)
		{
			MoveToEx(hdc, (i + 2) * cxGrid, 2 * cyGrid, nullptr);
			LineTo(hdc, (i + 2) * cxGrid, 19 * cyGrid);

			MoveToEx(hdc, cxGrid, (i + 3) * cyGrid, nullptr);
			LineTo(hdc, 18 * cxGrid, (i + 3) * cyGrid);
		}

		// vertical and horizontal headings
		for (auto i = 0; i < 16; i++)
		{
			TextOut(hdc, (2 * i + 5) * cxGrid / 2, 2 * cyGrid + 2, szBuffer,
				wsprintf(szBuffer, TEXT("%X-"), i));

			TextOut(hdc, 3 * cxGrid / 2, (i + 3) * cyGrid + 2, szBuffer,
				wsprintf(szBuffer, TEXT("-%X"), i));
		}

		for (auto y = 0; y < 16; y++)
		{
			for (auto x = 0; x < 16; x++)
			{
				TextOut(hdc, (2 * x + 5) * cxGrid / 2, (y + 3) * cyGrid + 2, szBuffer,
					wsprintf(szBuffer, TEXT("%c"), 16 * x + y));
			}
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