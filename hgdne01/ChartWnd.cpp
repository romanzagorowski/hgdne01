#include "pch.h"
#include "ChartWnd.h"
#include "PaintScope.h"
#include "mt4hst.h"
#include <vector>
#include <float.h>
#include "ohlcv.h"
#include <cmath>

//-----------------------------------------------------------------------------

static
HWND ___hwndMe = nullptr;

//-----------------------------------------------------------------------------

static
std::vector<OHLCV> ___data;

//-----------------------------------------------------------------------------

static
double ___windowChartRatio = 0;

static
double ___minPrice = 0;

static
long ___windowHeigh = 0;

//-----------------------------------------------------------------------------

static
void DrawCandle(HDC hdc, HBRUSH hbr, const long centerX, const double openPrice, const double highPrice, const double lowPrice, const double closePrice)
{
    const double candleHigh = (highPrice - ___minPrice) * ___windowChartRatio;
    const double candleLow  = ( lowPrice - ___minPrice) * ___windowChartRatio;

    const double candleHighR = std::round(candleHigh);
    const double candleLowR  = std::round(candleLow );

    const long candleHighL = (long)candleHighR;
    const long candleLowL  = (long)candleLowR ;

    const long candleHighL2 = ___windowHeigh - candleHighL;
    const long candleLowL2  = ___windowHeigh - candleLowL;

    //-------------------------------------------------------------------------

    const double candleOpen  = ( openPrice - ___minPrice) * ___windowChartRatio;
    const double candleClose = (closePrice - ___minPrice) * ___windowChartRatio;

    const double candleOpenR  = std::round(candleOpen);
    const double candleCloseR = std::round(candleClose);

    const long candleOpenL  = (long)candleOpenR;
    const long candleCloseL = (long)candleCloseR;

    const long candleOpenL2 = ___windowHeigh - candleOpenL;
    const long candleCloseL2 = ___windowHeigh - candleCloseL;

    //-------------------------------------------------------------------------

    if(openPrice > closePrice)
    {
        const long rcTop    = candleOpenL2;
        const long rcBottom = candleCloseL2;

        const long rcLeft = centerX - 5;
        const long rcRight = centerX + 5 + 1;

        const RECT rc{ rcLeft, rcTop, rcRight, rcBottom };
        const int e = FrameRect(hdc, &rc, hbr);
        _ASSERT(0 != e);

        //---------------------------------------------------------------------

        BOOL b = MoveToEx(hdc, centerX, candleOpenL2, nullptr);
        _ASSERT(b);

        b = LineTo(hdc, centerX, candleHighL2);
        _ASSERT(b);

        //---------------------------------------------------------------------

        b = MoveToEx(hdc, centerX, candleCloseL2, nullptr);
        _ASSERT(b);

        b = LineTo(hdc, centerX, candleLowL2);
        _ASSERT(b);
    }
    else
    {
        const long rcTop = candleCloseL2;
        const long rcBottom = candleOpenL2 + 1;

        const long rcLeft = centerX - 5;
        const long rcRight = centerX + 5 + 1;

        //---------------------------------------------------------------------

        const RECT rc{ rcLeft, rcTop, rcRight, rcBottom };
        const int e = FillRect(hdc, &rc, hbr);
        _ASSERT(0 != e);

        BOOL b = MoveToEx(hdc, centerX, candleCloseL2, nullptr);
        _ASSERT(b);

        b = LineTo(hdc, centerX, candleHighL2);
        _ASSERT(b);

        //---------------------------------------------------------------------

        b = MoveToEx(hdc, centerX, candleOpenL2, nullptr);
        _ASSERT(b);

        b = LineTo(hdc, centerX, candleLowL2);
        _ASSERT(b);
    }
}

//-----------------------------------------------------------------------------

static
void ChartWnd_OnPaint(HDC hdc)
{
    HBRUSH hbr = (HBRUSH)GetStockObject(BLACK_BRUSH);

    for(size_t i = 0; i < ___data.size(); i++)
    {
        DrawCandle(hdc, hbr, (i + 1) * 15, ___data[i].open, ___data[i].high, ___data[i].low, ___data[i].close);
    }
}

//-----------------------------------------------------------------------------

static
void ReadHstData()
{
    //#define HST_FILENAME "c:\\Program Files (x86)\\FXCM MetaTrader4\\history\\default\\EURUSD240.hst"
#define HST_FILENAME "c:\\Temp\\USDJPY10080.hst"

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

            ___data.push_back(ohlcv);
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

            ___data.push_back(ohlcv);
        }
    }

    //-------------------------------------------------------------------------

    fclose(f);
}

//-----------------------------------------------------------------------------

static
void GetDataMinMaxPrice(double& minPrice, double& maxPrice)
{
    minPrice = DBL_MAX;
    maxPrice = -DBL_MAX;

    for(auto i = ___data.cbegin(); i < ___data.cend(); ++i)
    {
        if(i->high > maxPrice)
        {
            maxPrice = i->high;
        }

        if(i->low < minPrice)
        {
            minPrice = i->low;
        }
    }
}

//-----------------------------------------------------------------------------

static
void PrepareData()
{
    double maxPrice;

    GetDataMinMaxPrice(___minPrice, maxPrice);

    double chartWidth = maxPrice - ___minPrice;

    RECT rcClient;
    GetClientRect(___hwndMe, &rcClient);

    ___windowHeigh = rcClient.bottom - rcClient.top;

    ___windowChartRatio = ___windowHeigh / chartWidth;
}

//-----------------------------------------------------------------------------

static
BOOL ChartWnd_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    ___hwndMe = hwnd;

    ReadHstData();
    PrepareData();

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
