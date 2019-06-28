#include <Windows.h>

HDC GetPrinterDC(void);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmdLine, int cmdShow)
{
    static TCHAR text[] = TEXT("Suaviza contornos, quebra istmos estreitos e elimina proeminências delgadas");
    static DOCINFO di = { sizeof(DOCINFO), TEXT("FormFeed") };
    HDC hdcPrint = GetPrinterDC();

    if (hdcPrint != nullptr)
    {
        if (StartDoc(hdcPrint, &di) > 0)
        {
            if (StartPage(hdcPrint) > 0 && EndPage(hdcPrint) > 0)
                EndDoc(hdcPrint);
        }

        DeleteDC(hdcPrint);
    }

    return 0;
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
