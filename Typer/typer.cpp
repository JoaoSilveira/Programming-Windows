#include <windows.h>

#define BUFFER(x, y) *(pBuffer + y * cxBuffer + x)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("Typer");
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

	auto hwnd = CreateWindow(szAppName, TEXT("Typing Program"),
		WS_OVERLAPPEDWINDOW,
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
	static DWORD dwCharSet = DEFAULT_CHARSET;
	static int cxChar, cyChar, cxClient, cyClient, cxBuffer, cyBuffer, xCaret, yCaret;
	static PTCHAR pBuffer = nullptr;

	switch (message)
	{
	case WM_INPUTLANGCHANGE:
	{
		dwCharSet = wParam;
	}
	case WM_CREATE:
	{
		TEXTMETRIC tm;

		auto hdc = GetDC(hwnd);

		SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, nullptr));
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cyChar = tm.tmHeight;

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
		ReleaseDC(hwnd, hdc);
	}
	case WM_SIZE:
	{
		if (message == WM_SIZE)
		{
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
		}

		cxBuffer = max(1, cxClient / cxChar);
		cyBuffer = max(1, cyClient / cyChar);

		if (pBuffer)
			delete[] pBuffer;

		pBuffer = new TCHAR[cxBuffer * cyBuffer];

		for (auto y = 0; y < cyBuffer; y++)
		{
			for (auto x = 0; x < cxBuffer; x++)
			{
				BUFFER(x, y) = ' ';
			}
		}

		xCaret = 0;
		yCaret = 0;

		if (hwnd == GetFocus())
			SetCaretPos(xCaret * cxChar, yCaret * cyChar);

		InvalidateRect(hwnd, nullptr, true);
		return 0;
	}
	case WM_SETFOCUS:
	{
		CreateCaret(hwnd, nullptr, cxChar, cyChar);
		SetCaretPos(xCaret * cxChar, yCaret * cyChar);
		ShowCaret(hwnd);
		return 0;
	}
	case WM_KILLFOCUS:
	{
		HideCaret(hwnd);
		DestroyCaret();
		return 0;
	}
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_HOME:
			xCaret = 0;
			break;
		case VK_END:
			xCaret = cxBuffer - 1;
			break;
		case VK_PRIOR:
			yCaret = 0;
			break;
		case VK_NEXT:
			yCaret = cyBuffer - 1;
			break;
		case VK_LEFT:
			xCaret = max(0, xCaret - 1);
			break;
		case VK_RIGHT:
			xCaret = min(cxBuffer - 1, xCaret + 1);
			break;
		case VK_UP:
			yCaret = max(0, yCaret - 1);
			break;
		case VK_DOWN:
			yCaret = min(cyBuffer - 1, yCaret + 1);
			break;
		case VK_DELETE:
		{
			for (auto x = xCaret; x < cxBuffer - 1; x++)
			{
				BUFFER(x, yCaret) = BUFFER(x + 1, yCaret);
			}
			BUFFER(cxBuffer - 1, yCaret) = ' ';

			HideCaret(hwnd);
			auto hdc = GetDC(hwnd);
			SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, nullptr));
			TextOut(hdc, xCaret*cxChar, yCaret*cyChar, &BUFFER(xCaret, yCaret), cxBuffer - xCaret);
			DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
			ReleaseDC(hwnd, hdc);
			ShowCaret(hwnd);
			break;
		}
		default:
			break;
		}
		SetCaretPos(xCaret*cxChar, yCaret*cyChar);
		return 0;
	}
	case WM_CHAR:
	{
		for (auto i = 0; i < LOWORD(lParam); i++)
		{
			switch (wParam)
			{
			case '\b':
			{
				if (xCaret > 0)
				{
					xCaret--;
					SendMessage(hwnd, WM_KEYDOWN, VK_DELETE, 1);
				}
				break;
			}
			case '\t':
			{
				do
				{
					SendMessage(hwnd, WM_CHAR, ' ', 1);
				} while (xCaret % 8);
				break;
			}
			case '\n':
			{
				if (++yCaret == cyBuffer)
					yCaret = 0;
				break;
			}
			case '\r':
			{
				xCaret = 0;
				if (++yCaret == cyBuffer)
					yCaret = 0;
				break;
			}
			case '\x1B':
			{
				for (auto y = 0; y < cyBuffer; y++)
				{
					for (auto x = 0; x < cxBuffer; x++)
					{
						BUFFER(x, y) = ' ';
					}
				}

				xCaret = 0;
				yCaret = 0;

				InvalidateRect(hwnd, nullptr, true);
				break;
			}
			default:
			{
				BUFFER(xCaret, yCaret) = static_cast<TCHAR>(wParam);

				HideCaret(hwnd);
				auto hdc = GetDC(hwnd);

				SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, nullptr));
				TextOut(hdc, xCaret * cxChar, yCaret * cyChar, &BUFFER(xCaret, yCaret), 1);
				DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));

				ReleaseDC(hwnd, hdc);
				ShowCaret(hwnd);

				if (++xCaret == cxBuffer)
				{
					xCaret = 0;

					if (++yCaret == cyBuffer)
						yCaret = 0;
				}
				break;
			}
			}
		}

		SetCaretPos(xCaret * cxChar, yCaret * cyChar);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		auto hdc = BeginPaint(hwnd, &ps);

		SelectObject(hdc, CreateFont(0, 0, 0, 0, 0, 0, 0, 0, dwCharSet, 0, 0, 0, FIXED_PITCH, nullptr));

		for (auto y = 0; y < cyBuffer; y++)
		{
			TextOut(hdc, 0, y * cyChar, &BUFFER(0, y), cxBuffer);
		}

		DeleteObject(SelectObject(hdc, GetStockObject(SYSTEM_FONT)));
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