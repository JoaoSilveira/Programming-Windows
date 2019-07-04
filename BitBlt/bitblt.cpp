#include <Windows.h>

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmdLine, int cmdShow)
{
    static TCHAR appName[] = TEXT("BitBlt");
    WNDCLASS wndClass;

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WinProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(nullptr, IDI_INFORMATION);
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
        TEXT("BitBlt Demo"),
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

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int cxClient;
    static int cyClient;
    static int cxSource;
    static int cySource;

    switch (message)
    {
        case WM_CREATE:
        {
            cxSource = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXSIZE);
            cySource = GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);
            return 0;
        }
        case WM_SIZE:
        {
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdcClient = BeginPaint(hwnd, &ps);
            HDC hdcWindow = GetWindowDC(hwnd);

            for (int y = 0; y < cyClient; y += cySource)
                for (int x = 0; x < cxClient; x += cxSource)
                    BitBlt(hdcClient, x, y, cxSource, cySource, hdcWindow, 0, 0, SRCCOPY);

            ReleaseDC(hwnd, hdcWindow);
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