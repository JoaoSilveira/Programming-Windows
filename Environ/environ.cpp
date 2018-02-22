#include <windows.h>

#define ID_LIST 1
#define ID_TEXT 2

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	static TCHAR appName[] = TEXT("Environ");
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

	auto hwnd = CreateWindow(appName, TEXT("Environment List Box"), WS_OVERLAPPEDWINDOW,
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

void FillListBox(HWND hwndList)
{
	auto pVarBlock = GetEnvironmentStrings();
	auto cpy = pVarBlock;

	while (*pVarBlock)
	{
		if (*pVarBlock != '=')
		{
			auto pVarBeg = pVarBlock;

			while (*pVarBlock != '=')
				++pVarBlock;

			auto length = pVarBlock - pVarBeg;

			auto pVarName = new TCHAR[length + 1];
			CopyMemory(pVarName, pVarBeg, length * sizeof(TCHAR));
			pVarName[length] = '\0';

			SendMessage(hwndList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(pVarName));

			delete[] pVarName;
		}

		while (*pVarBlock != '\0')
			pVarBlock++;

		pVarBlock++;
	}

	FreeEnvironmentStrings(cpy);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndList, hwndText;

	switch (message)
	{
	case WM_CREATE:
	{
		auto cxChar = LOWORD(GetDialogBaseUnits());
		auto cyChar = HIWORD(GetDialogBaseUnits());

		hwndList = CreateWindow(TEXT("listbox"), nullptr,
			WS_CHILD | WS_VISIBLE | LBS_STANDARD, cxChar, cyChar * 3,
			cxChar * 16 + GetSystemMetrics(SM_CXVSCROLL), cyChar * 5,
			hwnd, reinterpret_cast<HMENU>(ID_LIST),
			reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, nullptr);

		hwndText = CreateWindow(TEXT("static"), nullptr,
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			cxChar, cyChar, GetSystemMetrics(SM_CXSCREEN), cyChar,
			hwnd, reinterpret_cast<HMENU>(ID_TEXT),
			reinterpret_cast<LPCREATESTRUCT>(lParam)->hInstance, nullptr);

		FillListBox(hwndList);
		return 0;
	}
	case WM_SETFOCUS:
	{
		SetFocus(hwndList);
		return 0;
	}
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == ID_LIST && HIWORD(wParam) == LBN_SELCHANGE)
		{
			auto index = SendMessage(hwndList, LB_GETCURSEL, 0, 0);
			auto length = SendMessage(hwndList, LB_GETTEXTLEN, index, 0) + 1;

			auto pVarName = new TCHAR[length];
			SendMessage(hwndList, LB_GETTEXT, index, reinterpret_cast<LPARAM>(pVarName));

			length = GetEnvironmentVariable(pVarName, nullptr, 0);
			auto pVarValue = new TCHAR[length];
			GetEnvironmentVariable(pVarName, pVarValue, length);

			SetWindowText(hwndText, pVarValue);

			delete[] pVarValue, pVarName;
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