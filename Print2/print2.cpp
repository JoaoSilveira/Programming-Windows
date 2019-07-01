#include <Windows.h>

HDC GetPrinterDC(void);
void PageGDICalls(HDC, int, int);

HINSTANCE hInst;
TCHAR appName[] = TEXT("Print2");
TCHAR caption[] = TEXT("Print Program 2 (Abort Procedure)");

BOOL CALLBACK AbortProc(HDC hdcPrn, int code)
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return TRUE;
}

bool PrintMyPage(HWND hwnd)
{
    static DOCINFO di = { sizeof(DOCINFO), TEXT("Print1: Printing") };

    HDC hdcPrn = GetPrinterDC();
    if (hdcPrn == nullptr)
        return false;

    int xPage = GetDeviceCaps(hdcPrn, HORZRES);
    int yPage = GetDeviceCaps(hdcPrn, VERTRES);

    EnableWindow(hwnd, false);
    SetAbortProc(hdcPrn, AbortProc);

    bool result = true;
    if (StartDoc(hdcPrn, &di) > 0)
    {
        if (StartPage(hdcPrn) > 0)
        {
            PageGDICalls(hdcPrn, xPage, yPage);

            if (EndPage(hdcPrn) > 0)
                EndDoc(hdcPrn);
            else
                result = false;
        }
        else
            result = false;
    }
    else
        result = false;

    EnableWindow(hwnd, true);
    DeleteDC(hdcPrn);
    return result;
}
