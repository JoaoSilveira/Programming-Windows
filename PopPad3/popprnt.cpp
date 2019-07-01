#include <Windows.h>
#include <commdlg.h>
#include "resource.h"

bool userAbort;
HWND dlgPrint;

INT_PTR CALLBACK PrintDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
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

BOOL CALLBACK AbortProc(HDC printerDC, int code)
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

BOOL PopPrntPrintFile(HINSTANCE hInst, HWND hwnd, HWND hwndEdit, PTSTR titleName)
{
    static DOCINFO di = { sizeof(DOCINFO) };
    static PRINTDLG pd;

    pd.lStructSize = sizeof(PRINTDLG);
    pd.hwndOwner = hwnd;
    pd.hDevMode = nullptr;
    pd.hDevNames = nullptr;
    pd.hDC = nullptr;
    pd.Flags = PD_ALLPAGES | PD_COLLATE | PD_RETURNDC | PD_NOSELECTION;
    pd.nFromPage = 0;
    pd.nToPage = 0;
    pd.nMinPage = 0;
    pd.nMaxPage = 0;
    pd.nCopies = 1;
    pd.hInstance = hInst;
    pd.lCustData = 0L;
    pd.lpfnPrintHook = nullptr;
    pd.lpfnSetupHook = nullptr;
    pd.lpPrintTemplateName = nullptr;
    pd.lpSetupTemplateName = nullptr;
    pd.hPrintTemplate = nullptr;
    pd.hSetupTemplate = nullptr;

    if (!PrintDlg(&pd))
        return true;

    int totalLines = SendMessage(hwndEdit, EM_GETLINECOUNT, 0, 0);
    if (totalLines == 0)
        return true;

    TEXTMETRIC tm;
    GetTextMetrics(pd.hDC, &tm);
    int yChar = tm.tmHeight + tm.tmExternalLeading;

    int charsPerLine = GetDeviceCaps(pd.hDC, HORZRES) / tm.tmAveCharWidth;
    int linesPerPage = GetDeviceCaps(pd.hDC, VERTRES) / yChar;
    int totalPages = (totalLines + linesPerPage - 1) / linesPerPage;

    TCHAR* buffer = reinterpret_cast<TCHAR*>(malloc(sizeof(TCHAR) * charsPerLine + 1));

    EnableWindow(hwnd, false);

    bool success = true;
    userAbort = false;

    dlgPrint = CreateDialog(hInst, TEXT("PrintDlgBox"), hwnd, PrintDlgProc);
    SetDlgItemText(dlgPrint, IDC_FILENAME, titleName);
    SetAbortProc(pd.hDC, AbortProc);

    TCHAR jobName[64 + MAX_PATH];
    GetWindowText(hwnd, jobName, sizeof(jobName) / sizeof(TCHAR));
    di.lpszDocName = jobName;

    if (StartDoc(pd.hDC, &di) > 0)
    {
        for (int colCopy = 0; colCopy < ((WORD)pd.Flags & PD_COLLATE ? pd.nCopies : 1); colCopy++)
        {
            for (int page = 0; page < totalPages; page++)
            {
                for (int noColCopy = 0; noColCopy < ((WORD)pd.Flags & PD_COLLATE ? 1 : pd.nCopies); noColCopy++)
                {
                    if (StartPage(pd.hDC) < 0)
                    {
                        success = false;
                        break;
                    }

                    for (int line = 0; line < linesPerPage; line++)
                    {
                        int lineNum = linesPerPage * page + line;

                        if (lineNum > totalLines)
                            break;

                        *(int *)buffer = charsPerLine;

                        TextOut(pd.hDC, 0, yChar * line, buffer, (int)SendMessage(hwndEdit, EM_GETLINE, static_cast<WPARAM>(lineNum), reinterpret_cast<LPARAM>(buffer)));
                    }

                    if (EndPage(pd.hDC) < 0)
                    {
                        success = false;
                        break;
                    }

                    if (userAbort)
                        break;
                }

                if (userAbort || !success)
                    break;
            }

            if (userAbort || !success)
                break;
        }
    }
    else
        success = false;

    if (success)
        EndDoc(pd.hDC);

    if (!userAbort)
    {
        EnableWindow(hwnd, true);
        DestroyWindow(dlgPrint);
    }
    free(buffer);
    DeleteDC(pd.hDC);

    return success && !userAbort;
}