#include <Windows.h>
#include "resource.h"

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#if defined(UNICODE) || defined(_UNICODE)

#define CF_TCHAR CF_UNICODETEXT
TCHAR defaultText[] = TEXT("Default Text - Unicode Version");
TCHAR caption[] = TEXT("Clipboard Text Transfers - Unicode Version");

#else

#define CF_TCHAR CF_TEXT
TCHAR defaultText[] = TEXT("Default Text - ANSI Version");
TCHAR caption[] = TEXT("Clipboard Text Transfers - ANSI Version");

#endif // defined(UNICODE) || defined(_UNICODE)

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR cmdLine, int cmdShow)
{
    static TCHAR appName[] = TEXT("ClipText");
    WNDCLASS wndClass;

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WinProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wndClass.lpszMenuName = appName;
    wndClass.lpszClassName = appName;

    if (!RegisterClass(&wndClass))
    {
        MessageBox(nullptr, TEXT("This program requires Windows NT!"), appName, MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindow(
        appName,
        caption,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    ShowWindow(hwnd, cmdShow);
    UpdateWindow(hwnd);

    HACCEL hAccel = LoadAccelerators(hInstance, appName);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(hwnd, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static TCHAR *pText;
    
    switch (message)
    {
        case WM_CREATE:
            SendMessage(hwnd, WM_COMMAND, IDM_EDIT_RESET, 0);
            return 0;
        case WM_INITMENUPOPUP:
        {
            EnableMenuItem((HMENU)wParam, IDM_EDIT_PASTE, IsClipboardFormatAvailable(CF_TCHAR) ? MF_ENABLED : MF_GRAYED);

            auto enabled = pText ? MF_ENABLED : MF_GRAYED;
            EnableMenuItem((HMENU)wParam, IDM_EDIT_CUT, enabled);
            EnableMenuItem((HMENU)wParam, IDM_EDIT_COPY, enabled);
            EnableMenuItem((HMENU)wParam, IDM_EDIT_CLEAR, enabled);
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDM_EDIT_PASTE:
                {
                    OpenClipboard(hwnd);

                    if (auto hGlobal = GetClipboardData(CF_TCHAR))
                    {
                        auto pGlobal = reinterpret_cast<TCHAR*>(GlobalLock(hGlobal));
                        if (pText)
                        {
                            free(pText);
                            pText = nullptr;
                        }
                        pText = reinterpret_cast<TCHAR*>(malloc(GlobalSize(hGlobal)));
                        lstrcpy(pText, pGlobal);
                        InvalidateRect(hwnd, nullptr, true);
                        GlobalUnlock(hGlobal);
                    }

                    CloseClipboard();
                    return 0;
                }
                case IDM_EDIT_CUT:
                case IDM_EDIT_COPY:
                {
                    if (!pText)
                        return 0;

                    auto hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (lstrlen(pText) + 1) * sizeof(TCHAR));
                    auto pGlobal = reinterpret_cast<TCHAR*>(GlobalLock(hGlobal));
                    lstrcpy(pText, pGlobal);
                    GlobalUnlock(hGlobal);

                    OpenClipboard(hwnd);
                    EmptyClipboard();
                    SetClipboardData(CF_TCHAR, hGlobal);
                    CloseClipboard();

                    if (LOWORD(wParam) == IDM_EDIT_COPY)
                        return 0;
                }
                case IDM_EDIT_CLEAR:
                {
                    if (pText)
                    {
                        free(pText);
                        pText = nullptr;
                    }
                    InvalidateRect(hwnd, nullptr, true);
                    return 0;
                }
                case IDM_EDIT_RESET:
                {
                    if (pText)
                    {
                        free(pText);
                        pText = nullptr;
                    }
                    pText = reinterpret_cast<TCHAR*>(malloc((lstrlen(defaultText) + 1) * sizeof(TCHAR)));
                    lstrcpy(pText, defaultText);
                    InvalidateRect(hwnd, nullptr, true);
                    return 0;
                }
            }
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT rc;
            auto hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rc);

            if (pText)
                DrawText(hdc, pText, -1, &rc, DT_EXPANDTABS | DT_WORDBREAK);

            EndPaint(hwnd, &ps);

            return 0;
        }
        case WM_DESTROY:
        {
            if (pText)
                free(pText);

            PostQuitMessage(0);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
}