#include "pch.h"
#include "ChartWnd.h"
#include "PaintScope.h"
#include "mt4hst.h"
#include <vector>
#include "ohlcv.h"

static
std::vector<OHLCV> data;

//-----------------------------------------------------------------------------

static
void ChartWnd_OnPaint(HDC hdc)
{
    RECT rc{ 100, 100, 400, 400 };
    HBRUSH hbr = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FrameRect(hdc, &rc, hbr);
}

//-----------------------------------------------------------------------------

static
void ReadHstData()
{
#define HST_FILENAME "c:\\Program Files (x86)\\FXCM MetaTrader4\\history\\default\\EURUSD240.hst"

    //-------------------------------------------------------------------------

    FILE* f = fopen(HST_FILENAME, "rb");
    _ASSERT(f);

    //-------------------------------------------------------------------------

    HST_HEADER h{};

    size_t x = fread(&h, sizeof(h), 1, f);
    _ASSERT(1 == x);

    //-------------------------------------------------------------------------

    if(400 == h.version)
    {
        HST_RECORD_400 r;

        while(1 == fread(&r, sizeof(r), 1, f))
        {
            OHLCV ohlcv{
                r.open
            ,   r.high
            ,   r.low
            ,   r.close
            };

            data.push_back(ohlcv);
        }
    }
    else
    {
        HST_RECORD_401 r;

        while(1 == fread(&r, sizeof(r), 1, f))
        {
            OHLCV ohlcv{
                r.open
            ,   r.high
            ,   r.low
            ,   r.close
            };

            data.push_back(ohlcv);
        }
    }

    //-------------------------------------------------------------------------

    fclose(f);
}

//-----------------------------------------------------------------------------

static
BOOL ChartWnd_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    ReadHstData();

    //-------------------------------------------------------------------------

    return TRUE;
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK ChartWnd_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        HANDLE_MSG(hwnd, WM_CREATE, ChartWnd_OnCreate);

        case WM_PAINT:
        {
            PaintScope ps{ hwnd };
            ChartWnd_OnPaint(ps.hdc);
        }
        break;

        case WM_DESTROY:
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

//-----------------------------------------------------------------------------

ATOM ChartWnd_RegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wc{};

    wc.cbSize = sizeof(WNDCLASSEX);

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = ChartWnd_WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = CHARTWND_CLASSNAME;
    wc.hIconSm = nullptr;

    return RegisterClassExW(&wc);
}

//-----------------------------------------------------------------------------
