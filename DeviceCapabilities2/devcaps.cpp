#include <Windows.h>
#include "resource.h"

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void DoBasicInfo(HDC, HDC, int, int);
void DoOtherInfo(HDC, HDC, int, int);
void DoBitCodedCaps(HDC, HDC, int, int, int);

typedef struct
{
    int mask;
    const TCHAR* desc;
}BITS;

#define IDM_DEVMODE 1000

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR cmdLine, int cmdShow)
{
    static TCHAR appName[] = TEXT("DevCaps2");
    WNDCLASS wndClass;

    wndClass.style = CS_VREDRAW | CS_HREDRAW;
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
        nullptr,
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
    static TCHAR device[32];
    static TCHAR windowText[64];
    static int cxChar;
    static int cyChar;
    static int currentDevice = IDM_SCREEN;
    static int currentInfo = IDM_BASIC;
    static DWORD needed;
    static DWORD returned;
    static PRINTER_INFO_4 *pInfo4;
    static PRINTER_INFO_5 *pInfo5;

    switch (message)
    {
        case WM_CREATE:
        {
            TEXTMETRIC tm;
            HDC hdc = GetDC(hwnd);
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
            GetTextMetrics(hdc, &tm);
            cxChar = tm.tmAveCharWidth;
            cyChar = tm.tmHeight + tm.tmExternalLeading;
            ReleaseDC(hwnd, hdc);
        }
        case WM_SETTINGCHANGE:
        {
            HMENU hMenu = GetSubMenu(GetMenu(hwnd), 0);

            while (GetMenuItemCount(hMenu) > 1)
                DeleteMenu(hMenu, 1, MF_BYPOSITION);

            // GetVersion is deprecated, use VersionHelpers.h instead
            //if (GetVersion() & 0x80000000)
            //{
            //    EnumPrinters(PRINTER_ENUM_LOCAL, nullptr, 5, nullptr, 0, &needed, &returned);

            //    pInfo5 = reinterpret_cast<PRINTER_INFO_5*>(malloc(needed));

            //    EnumPrinters(PRINTER_ENUM_LOCAL, nullptr, 5, (PBYTE)pInfo5, needed, &needed, &returned);

            //    for (DWORD i = 0; i < returned; i++)
            //        AppendMenu(hMenu, (i + 1) % 16 ? 0 : MF_MENUBARBREAK, i + 1, pInfo5[i].pPrinterName);

            //    free(pInfo5);
            //}
            //else
            //{
            EnumPrinters(PRINTER_ENUM_LOCAL, nullptr, 4, nullptr, 0, &needed, &returned);

            pInfo4 = reinterpret_cast<PRINTER_INFO_4*>(malloc(needed));

            EnumPrinters(PRINTER_ENUM_LOCAL, nullptr, 4, (PBYTE)pInfo4, needed, &needed, &returned);

            for (DWORD i = 0; i < returned; i++)
                AppendMenu(hMenu, (i + 1) % 16 ? 0 : MF_MENUBARBREAK, i + 1, pInfo4[i].pPrinterName);

            free(pInfo4);
            //}

            AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenu(hMenu, 0, IDM_DEVMODE, TEXT("Properties"));

            wParam = IDM_SCREEN;
        }
        case WM_COMMAND:
        {
            HMENU hMenu = GetMenu(hwnd);

            if (LOWORD(wParam) == IDM_SCREEN || LOWORD(wParam) < IDM_DEVMODE)
            {
                CheckMenuItem(hMenu, currentDevice, MF_UNCHECKED);
                currentDevice = LOWORD(wParam);
                CheckMenuItem(hMenu, currentDevice, MF_CHECKED);
            }
            else if (LOWORD(wParam) == IDM_DEVMODE)
            {
                GetMenuString(hMenu, currentDevice, device, sizeof(device) / sizeof(TCHAR), MF_BYCOMMAND);

                HANDLE hPrint;
                if (OpenPrinter(device, &hPrint, nullptr))
                {
                    PrinterProperties(hwnd, hPrint);
                    ClosePrinter(hPrint);
                }
            }
            else
            {
                CheckMenuItem(hMenu, currentInfo, MF_UNCHECKED);
                currentInfo = LOWORD(wParam);
                CheckMenuItem(hMenu, currentInfo, MF_CHECKED);
            }
            InvalidateRect(hwnd, nullptr, true);
            return 0;
        }
        case WM_INITMENUPOPUP:
        {
            if (lParam == 0)
                EnableMenuItem(GetMenu(hwnd), IDM_DEVMODE, currentDevice == IDM_SCREEN ? MF_GRAYED : MF_ENABLED);
            
            return 0;
        }
        case WM_PAINT:
        {
            HDC hdcInfo;
            lstrcpy(windowText, TEXT("Device Capabilities: "));

            if (currentDevice == IDM_SCREEN)
            {
                lstrcpy(device, TEXT("DISPLAY"));
                hdcInfo = CreateIC(device, nullptr, nullptr, nullptr);
            }
            else
            {
                HMENU hMenu = GetMenu(hwnd);
                GetMenuString(hMenu, currentDevice, device, sizeof(device) / sizeof(TCHAR), MF_BYCOMMAND);
                hdcInfo = CreateIC(nullptr, device, nullptr, nullptr);
            }

            lstrcat(windowText, device);
            SetWindowText(hwnd, windowText);

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));

            if (hdcInfo)
            {
                switch (currentInfo)
                {
                    case IDM_BASIC:
                    {
                        DoBasicInfo(hdc, hdcInfo, cxChar, cyChar);
                        break;
                    }
                    case IDM_OTHER:
                    {
                        DoOtherInfo(hdc, hdcInfo, cxChar, cyChar);
                        break;
                    }
                    case IDM_CURVE:
                    case IDM_LINE:
                    case IDM_POLY:
                    case IDM_TEXT:
                    {
                        DoBitCodedCaps(hdc, hdcInfo, cxChar, cyChar, currentInfo - IDM_CURVE);
                        break;
                    }
                }
                DeleteDC(hdcInfo);
            }

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

void DoBasicInfo(HDC hdc, HDC hdcInfo, int cxChar, int cyChar)
{
    static struct
    {
        int index;
        const TCHAR *desc;
    }
    info[] =
    {
        HORZSIZE,           TEXT("HORZSIZE        Width in millimeters:"),
        VERTSIZE,           TEXT("VERTSIZE        Height in millimeters:"),
        HORZRES,            TEXT("HORZRES         Width in pixels:"),
        VERTRES,            TEXT("VERTRES         Height in raster lines:"),
        BITSPIXEL,          TEXT("BITSPIXEL       Color bits per pixel:"),
        PLANES,             TEXT("PLANES          Number of color planes:"),
        NUMBRUSHES,         TEXT("NUMBRUSHES      Number of device brushes:"),
        NUMPENS,            TEXT("NUMPENS         Number of device pens:"),
        NUMMARKERS,         TEXT("NUMMARKERS      Number of device markers:"),
        NUMFONTS,           TEXT("NUMFONTS        Number of device fonts:"),
        NUMCOLORS,          TEXT("NUMCOLORS       Number of device colors:"),
        PDEVICESIZE,        TEXT("PDEVICESIZE     Size of device structure:"),
        ASPECTX,            TEXT("ASPECTX         Relative width of pixel:"),
        ASPECTY,            TEXT("ASPECTY         Relative height of pixel:"),
        ASPECTXY,           TEXT("ASPECTXY        Relative diagonal of pixel:"),
        LOGPIXELSX,         TEXT("LOGPIXELSX      Horizontal dots per inch:"),
        LOGPIXELSY,         TEXT("LOGPIXELSY      Vertical dots per inch:"),
        SIZEPALETTE,        TEXT("SIZEPALETTE     Number of palette entries:"),
        NUMRESERVED,        TEXT("NUMRESERVED     Reserved palette entries:"),
        COLORRES,           TEXT("COLORRES        Actual color resolution:"),
        PHYSICALWIDTH,      TEXT("PHYSICALWIDTH   Printer page pixel width:"),
        PHYSICALHEIGHT,     TEXT("PHYSICALHEIGHT  Printer page pixel height:"),
        PHYSICALOFFSETX,    TEXT("PHYSICALOFFSETX Printer page x offset:"),
        PHYSICALOFFSETY,    TEXT("PHYSICALOFFSETY Printer page y offset:")
    };

    TCHAR buffer[80];
    for (int i = 0; i < sizeof(info) / sizeof(info[0]); i++)
        TextOut(hdc, cxChar, (i + 1) * cyChar, buffer, wsprintf(buffer, TEXT("%-45s%8d"), info[i].desc, GetDeviceCaps(hdcInfo, info[i].index)));
}

void DoOtherInfo(HDC hdc, HDC hdcInfo, int cxChar, int cyChar)
{
    static BITS clip[] =
    {
        CP_RECTANGLE, TEXT("CP_RECTANGLE    Can Clip To Rectangle:")
    };
    static BITS raster[] =
    {
        RC_BITBLT,          TEXT("RC_BITBLT         Capable of simple BitBlt:"),
        RC_BANDING,         TEXT("RC_BANDING        Requires banding support:"),
        RC_SCALING,         TEXT("RC_SCALING        Requires scaling support:"),
        RC_BITMAP64,        TEXT("RC_BITMAP64       Supports bitmaps >64K:"),
        RC_GDI20_OUTPUT,    TEXT("RC_GDI20_OUTPUT   Has 2.0 output calls:"),
        RC_DI_BITMAP,       TEXT("RC_DI_BITMAP      Supports DIB to memory:"),
        RC_PALETTE,         TEXT("RC_PALETTE        Supports a palette:"),
        RC_DIBTODEV,        TEXT("RC_DIBTODEV       Supports bitmap conversion:"),
        RC_BIGFONT,         TEXT("RC_BIGFONT        Supports fonts >64K:"),
        RC_STRETCHBLT,      TEXT("RC_STRETCHBLT     Supports StretchBlt:"),
        RC_FLOODFILL,       TEXT("RC_FLOODFILL      Supports FloodFill:"),
        RC_STRETCHDIB,      TEXT("RC_STRETCHDIB     Supports StretchDIBits:")
    };
    static const TCHAR* tech[] =
    {
        TEXT("DT_PLOTTER (Vector plotter)"),
        TEXT("DT_RASDISPLAY (Raster display)"),
        TEXT("DT_RASPRINTER (Raster printer)"),
        TEXT("DT_RASCAMERA (Raster camera)"),
        TEXT("DT_CHARSTREAM (Character stream)"),
        TEXT("DT_METAFILE (Metafile)"),
        TEXT("DT_DISPFILE (Display file)")
    };

    TCHAR buffer[80];

    TextOut(hdc, cxChar, cyChar, buffer, wsprintf(buffer, TEXT("%-24s%01XH"), TEXT("DRIVERVERSION:"), GetDeviceCaps(hdcInfo, DRIVERVERSION)));
    TextOut(hdc, cxChar, cyChar << 1, buffer, wsprintf(buffer, TEXT("%-24s%-40s"), TEXT("TECHNOLOGY:"), tech[GetDeviceCaps(hdcInfo, TECHNOLOGY)]));
    TextOut(hdc, cxChar, cyChar << 2, buffer, wsprintf(buffer, TEXT("CLIPCAPS (Clipping capabilities)")));

    for (int i = 0; i < sizeof(clip) / sizeof(clip[0]); i++)
        TextOut(hdc, 9 * cxChar, (i + 6) * cyChar, buffer, wsprintf(buffer, TEXT("%-45s %3s"), clip[i].desc, GetDeviceCaps(hdcInfo, CLIPCAPS) & clip[i].mask ? TEXT("Yes") : TEXT("No")));

    TextOut(hdc, cxChar, 8 * cyChar, buffer, wsprintf(buffer, TEXT("RASTERCAPS (Raster capabilities)")));

    for (int i = 0; i < sizeof(clip) / sizeof(clip[0]); i++)
        TextOut(hdc, 9 * cxChar, (i + 10) * cyChar, buffer, wsprintf(buffer, TEXT("%-45s %3s"), raster[i].desc, GetDeviceCaps(hdcInfo, RASTERCAPS) & raster[i].mask ? TEXT("Yes") : TEXT("No")));
}

void DoBitCodedCaps(HDC hdc, HDC hdcInfo, int cxChar, int cyChar, int type)
{
    static BITS curves[] =
    {
        CC_CIRCLES,     TEXT("CC_CIRCLES    Can do circles:"),
        CC_PIE,         TEXT("CC_PIE        Can do pie wedges:"),
        CC_CHORD,       TEXT("CC_CHORD      Can do chord arcs:"),
        CC_ELLIPSES,    TEXT("CC_ELLIPSES   Can do ellipses:"),
        CC_WIDE,        TEXT("CC_WIDE       Can do wide borders:"),
        CC_STYLED,      TEXT("CC_STYLED     Can do styled borders:"),
        CC_WIDESTYLED,  TEXT("CC_WIDESTYLED Can do wide and styled borders:"),
        CC_INTERIORS,   TEXT("CC_INTERIORS  Can do interiors:")
    };
    static BITS lines[] =
    {
        LC_POLYLINE,    TEXT("LC_POLYLINE   Can do polyline:"),
        LC_MARKER,      TEXT("LC_MARKER     Can do markers:"),
        LC_POLYMARKER,  TEXT("LC_POLYMARKER Can do polymarkers"),
        LC_WIDE,        TEXT("LC_WIDE       Can do wide lines:"),
        LC_STYLED,      TEXT("LC_STYLED     Can do styled lines:"),
        LC_WIDESTYLED,  TEXT("LC_WIDESTYLED Can do wide and styled lines:"),
        LC_INTERIORS,   TEXT("LC_INTERIORS  Can do interiors:")
    };
    static BITS poly[] =
    {
        PC_POLYGON,     TEXT("PC_POLYGON     Can do alternate fill polygon:"),
        PC_RECTANGLE,   TEXT("PC_RECTANGLE   Can do rectangle:"),
        PC_WINDPOLYGON, TEXT("PC_WINDPOLYGON Can do winding number fill polygon:"),
        PC_SCANLINE,    TEXT("PC_SCANLINE    Can do scanlines:"),
        PC_WIDE,        TEXT("PC_WIDE        Can do wide borders:"),
        PC_STYLED,      TEXT("PC_STYLED      Can do styled borders:"),
        PC_WIDESTYLED,  TEXT("PC_WIDESTYLED  Can do wide and styled borders:"),
        PC_INTERIORS,   TEXT("PC_INTERIORS   Can do interiors:")
    };
    static BITS text[] =
    {
        TC_OP_CHARACTER,    TEXT("TC_OP_CHARACTER Can do character output precision:"),
        TC_OP_STROKE,       TEXT("TC_OP_STROKE    Can do stroke output precision:"),
        TC_CP_STROKE,       TEXT("TC_CP_STROKE    Can do stroke clip precision:"),
        TC_CR_90,           TEXT("TC_CP_90        Can do 90 degree character rotation:"),
        TC_CR_ANY,          TEXT("TC_CR_ANY       Can do any character rotation:"),
        TC_SF_X_YINDEP,     TEXT("TC_SF_X_YINDEP  Can do scaling independent of X and Y:"),
        TC_SA_DOUBLE,       TEXT("TC_SA_DOUBLE    Can do doubled character for scaling:"),
        TC_SA_INTEGER,      TEXT("TC_SA_INTEGER   Can do integer multiples for scaling:"),
        TC_SA_CONTIN,       TEXT("TC_SA_CONTIN    Can do any multiples for exact scaling:"),
        TC_EA_DOUBLE,       TEXT("TC_EA_DOUBLE    Can do double weight characters:"),
        TC_IA_ABLE,         TEXT("TC_IA_ABLE      Can do italicizing:"),
        TC_UA_ABLE,         TEXT("TC_UA_ABLE      Can do underlining:"),
        TC_SO_ABLE,         TEXT("TC_SO_ABLE      Can do strikeouts:"),
        TC_RA_ABLE,         TEXT("TC_RA_ABLE      Can do raster fonts:"),
        TC_VA_ABLE,         TEXT("TC_VA_ABLE      Can do vector fonts:")
    };
    static struct
    {
        int index;
        const TCHAR * title;
        BITS(*pbits)[];
        int size;
    }
    bitinfo[] =
    {
        CURVECAPS,      TEXT("CURVCAPS      (Curve Capabilities)"), (BITS(*)[]) curves, sizeof(curves) / sizeof(curves[0]),
        LINECAPS,       TEXT("LINECAPS      (Line Capabilities)"), (BITS(*)[]) lines, sizeof(lines) / sizeof(lines[0]),
        POLYGONALCAPS,  TEXT("POLYGONALCAPS (Polygonal Capabilities)"), (BITS(*)[]) poly, sizeof(poly) / sizeof(poly[0]),
        TEXTCAPS,       TEXT("TEXTCAPS      (Text Capabilities)"), (BITS(*)[]) text, sizeof(text) / sizeof(text[0])
    };

    static TCHAR buffer[80];
    TextOut(hdc, cxChar, cyChar, bitinfo[type].title, lstrlen(bitinfo[type].title));
    BITS(*pBits)[] = bitinfo[type].pbits;
    int devCaps = GetDeviceCaps(hdcInfo, bitinfo[type].index);

    for (int i = 0; i < bitinfo[type].size; i++)
        TextOut(hdc, cxChar, (i + 3) * cyChar, buffer, wsprintf(buffer, TEXT("%-55s %3s"), (*pBits)[i].desc, devCaps & (*pBits)[i].mask ? TEXT("Yes") : TEXT("No")));
}
