#include <Windows.h>

HDC GetPrinterDC(void);
void PageGDICalls(HDC, int, int);

HINSTANCE hInst;
TCHAR appName[] = TEXT("Print1");
TCHAR caption[] = TEXT("Print Program 1");

bool PrintMyPage(HWND hwnd)
{
    static DOCINFO di = { sizeof(DOCINFO), TEXT("Print1: Printing") };
    
    HDC hdcPrn = GetPrinterDC();
    if (hdcPrn == nullptr)
        return false;

    int xPage = GetDeviceCaps(hdcPrn, HORZRES);
    int yPage = GetDeviceCaps(hdcPrn, VERTRES);

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

    DeleteDC(hdcPrn);
    return result;
}
