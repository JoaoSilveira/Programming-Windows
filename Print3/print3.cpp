#include <Windows.h>

HDC GetPrinterDC(void);
void PageGDICalls(HDC, int, int);

HINSTANCE hInst;
TCHAR appName[] = TEXT("Print3");
TCHAR caption[] = TEXT("Print Program 3 (Dialog Box)");

bool userAbort;
HWND dlgPrint;

INT_PTR CALLBACK PrintDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            SetWindowText(hDlg, appName);
            EnableMenuItem(GetSystemMenu(hDlg, false), SC_CLOSE, MF_GRAYED);
            return true;
        }
        case WM_COMMAND:
        {
            userAbort = true;
            EnableWindow(GetParent(hDlg), true);
            DestroyWindow(hDlg);
            dlgPrint = nullptr;
            return true;
        }
        default:
            return false;
    }
}

BOOL CALLBACK AbortProc(HDC hdcPrn, int code)
{
    MSG msg;
    while (!userAbort && PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (dlgPrint != nullptr || !IsDialogMessage(dlgPrint, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return !userAbort;
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

    userAbort = false;
    dlgPrint = CreateDialog(hInst, TEXT("PrintDlgBox"), hwnd, PrintDlgProc);

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

    if (!userAbort)
    {
        EnableWindow(hwnd, true);
        DestroyWindow(dlgPrint);
    }

    DeleteDC(hdcPrn);
    return result && !userAbort;
}
