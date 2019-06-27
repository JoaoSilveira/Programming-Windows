#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmdLine, int cmdShow)
{
    static TCHAR appName[] = TEXT("ClipView");
    WNDCLASS wndClass;

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = appName;

    if (!RegisterClass(&wndClass))
    {
        MessageBox(nullptr, TEXT("This program requires Windows NT!"), appName, MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindow(
        appName,
        TEXT("Simple Clipboard Viewer (Text Only)"),
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

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndNextViewer;

    switch (message)
    {
        case WM_CREATE:
        {
            hwndNextViewer = SetClipboardViewer(hwnd);
            return 0;
        }
        case WM_CHANGECBCHAIN:
        {
            if (reinterpret_cast<HWND>(wParam) == hwndNextViewer)
                hwndNextViewer = reinterpret_cast<HWND>(lParam);
            else if (hwndNextViewer)
                SendMessage(hwndNextViewer, message, wParam, lParam);

            return 0;
        }
        case WM_DRAWCLIPBOARD:
        {
            if (hwndNextViewer)
                SendMessage(hwndNextViewer, message, wParam, lParam);

            InvalidateRect(hwnd, nullptr, true);
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT rc;
            auto hdc = BeginPaint(hwnd, &ps);
            
            GetClientRect(hwnd, &rc);
            OpenClipboard(hwnd);

        #if defined(UNICODE) || defined(_UNICODE)
            auto hGlobal = GetClipboardData(CF_UNICODETEXT);
        #else
            auto hGlobal = GetClipboardData(CF_TEXT);
        #endif

            if (hGlobal)
            {
                auto pGlobal = reinterpret_cast<TCHAR*>(GlobalLock(hGlobal));
                DrawText(hdc, pGlobal, -1, &rc, DT_EXPANDTABS);
                GlobalUnlock(hGlobal);
            }
            CloseClipboard();
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
        {
            ChangeClipboardChain(hwnd, hwndNextViewer);
            PostQuitMessage(0);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
}