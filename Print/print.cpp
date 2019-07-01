#include <Windows.h>

HDC GetPrinterDC(void);
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
bool PrintMyPage(HWND);

extern HINSTANCE hInst;
extern TCHAR appName[];
extern TCHAR caption[];

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmdLine, int cmdShow)
{
    WNDCLASS wndClass;

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WinProc;
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

    hInst = hInstance;
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

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

void PageGDICalls(HDC hdcPrn, int cxPage, int cyPage)
{
    static TCHAR textStr[] = TEXT("Hello, Printer");

    Rectangle(hdcPrn, 0, 0, cxPage, cyPage);

    MoveToEx(hdcPrn, 0, 0, nullptr);
    LineTo(hdcPrn, cxPage, cyPage);
    MoveToEx(hdcPrn, cxPage, 0, nullptr);
    LineTo(hdcPrn, 0, cyPage);

    SaveDC(hdcPrn);

    SetMapMode(hdcPrn, MM_ISOTROPIC);
    SetWindowExtEx(hdcPrn, 1000, 1000, nullptr);
    SetViewportExtEx(hdcPrn, cxPage / 2, -cyPage / 2, nullptr);
    SetViewportOrgEx(hdcPrn, cxPage / 2, cyPage / 2, nullptr);

    Ellipse(hdcPrn, -500, 500, 500, -500);

    SetTextAlign(hdcPrn, TA_BASELINE | TA_CENTER);
    TextOut(hdcPrn, 0, 0, textStr, lstrlen(textStr));
    RestoreDC(hdcPrn, -1);
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int cxClient;
    static int cyClient;

    switch (message)
    {
        case WM_CREATE:
        {
            HMENU hMenu = GetSystemMenu(hwnd, false);
            AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenu(hMenu, 0, 1, TEXT("&Print"));
            return 0;
        }
        case WM_SIZE:
        {
            cxClient = LOWORD(lParam);
            cyClient = HIWORD(lParam);
            return 0;
        }
        case WM_SYSCOMMAND:
        {
            if (wParam != 1)
                return DefWindowProc(hwnd, message, wParam, lParam);

            if (!PrintMyPage(hwnd))
                MessageBox(hwnd, TEXT("Could not print page!"), appName, MB_OK | MB_ICONEXCLAMATION);

            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            PageGDICalls(hdc, cxClient, cyClient);

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

HDC GetPrinterDC(void)
{
    DWORD needed;
    DWORD returned;
    HDC hdc;
    PRINTER_INFO_4* pInfo;

    EnumPrinters(PRINTER_ENUM_LOCAL, nullptr, 4, nullptr, 0, &needed, &returned);
    pInfo = reinterpret_cast<PRINTER_INFO_4*>(malloc(needed));
    EnumPrinters(PRINTER_ENUM_LOCAL, nullptr, 4, reinterpret_cast<PBYTE>(pInfo), needed, &needed, &returned);

    hdc = CreateDC(nullptr, pInfo[2].pPrinterName, nullptr, nullptr);

    free(pInfo);

    return hdc;
}