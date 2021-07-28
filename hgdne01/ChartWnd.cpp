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
double ___minPrice = 0;

static
double ___chartHeight = 0;

static
double ___windowChartRatio = 0;

static
long ___windowHeight = 0;

static
long ___leftOffset = 0;

static
long ___dataWidth = 0;

//-----------------------------------------------------------------------------

static
void DrawCandle(HDC hdc, HBRUSH hbr, const long leftX, const double openPrice, const double highPrice, const double lowPrice, const double closePrice)
{
    const double candleHigh = (highPrice - ___minPrice) * ___windowChartRatio;
    const double candleLow  = ( lowPrice - ___minPrice) * ___windowChartRatio;

    const double candleHighR = std::round(candleHigh);
    const double candleLowR  = std::round(candleLow );

    const long candleHighL = (long)candleHighR;
    const long candleLowL  = (long)candleLowR ;

    const long candleHighL2 = ___windowHeight - candleHighL;
    const long candleLowL2  = ___windowHeight - candleLowL;

    //-------------------------------------------------------------------------

    const double candleOpen  = ( openPrice - ___minPrice) * ___windowChartRatio;
    const double candleClose = (closePrice - ___minPrice) * ___windowChartRatio;

    const double candleOpenR  = std::round(candleOpen);
    const double candleCloseR = std::round(candleClose);

    const long candleOpenL  = (long)candleOpenR;
    const long candleCloseL = (long)candleCloseR;

    const long candleOpenL2 = ___windowHeight - candleOpenL;
    const long candleCloseL2 = ___windowHeight - candleCloseL;

    //-------------------------------------------------------------------------

    if(openPrice > closePrice)
    {
        const long rcTop    = candleOpenL2;
        const long rcBottom = candleCloseL2;

        const long rcLeft = leftX;
        const long rcRight = leftX + 10 + 1;

        const RECT rc{ rcLeft, rcTop, rcRight, rcBottom };
        const int e = FrameRect(hdc, &rc, hbr);
        _ASSERT(0 != e);

        //---------------------------------------------------------------------

        BOOL b = MoveToEx(hdc, leftX + 5, candleOpenL2, nullptr);
        _ASSERT(b);

        b = LineTo(hdc, leftX + 5, candleHighL2);
        _ASSERT(b);

        //---------------------------------------------------------------------

        b = MoveToEx(hdc, leftX + 5, candleCloseL2, nullptr);
        _ASSERT(b);

        b = LineTo(hdc, leftX + 5, candleLowL2);
        _ASSERT(b);
    }
    else
    {
        const long rcTop = candleCloseL2;
        const long rcBottom = candleOpenL2 + 1;

        const long rcLeft = leftX;
        const long rcRight = leftX + 10 + 1;

        //---------------------------------------------------------------------

        const RECT rc{ rcLeft, rcTop, rcRight, rcBottom };
        const int e = FillRect(hdc, &rc, hbr);
        _ASSERT(0 != e);

        BOOL b = MoveToEx(hdc, leftX + 5, candleCloseL2, nullptr);
        _ASSERT(b);

        b = LineTo(hdc, leftX + 5, candleHighL2);
        _ASSERT(b);

        //---------------------------------------------------------------------

        b = MoveToEx(hdc, leftX + 5, candleOpenL2, nullptr);
        _ASSERT(b);

        b = LineTo(hdc, leftX + 5, candleLowL2);
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
        DrawCandle(hdc, hbr, i * 15 - ___leftOffset, ___data[i].open, ___data[i].high, ___data[i].low, ___data[i].close);
    }

    //-------------------------------------------------------------------------

    RECT rc{};
    rc.left = 15 - ___leftOffset;
    rc.top = 15;
    rc.right = ___dataWidth + 1 - ___leftOffset;
    rc.bottom = ___windowHeight - 0;

    int e = FrameRect(hdc, &rc, hbr);
    _ASSERT(0 != e);

    //-------------------------------------------------------------------------
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

#define CANDLES_TO_READ 200

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

            if(___data.size() == CANDLES_TO_READ)
                break;
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

            if(___data.size() == CANDLES_TO_READ)
                break;
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

    ___chartHeight = maxPrice - ___minPrice;

    RECT rcClient;
    GetClientRect(___hwndMe, &rcClient);

    ___windowHeight = rcClient.bottom - rcClient.top;

    ___windowChartRatio = ___windowHeight / ___chartHeight;

    size_t dataSize = ___data.size();
    ___dataWidth = dataSize * 15;
}

//-----------------------------------------------------------------------------

static
BOOL ChartWnd_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    ___hwndMe = hwnd;

    ReadHstData();
    PrepareData();

    //-------------------------------------------------------------------------

    SCROLLINFO si{};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask |= SIF_RANGE;
    si.nMin = 0;
    si.nMax = ___dataWidth;
    si.fMask |= SIF_POS;
    si.nPos = 0;

    SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

    //-------------------------------------------------------------------------

    return TRUE;
}

//-----------------------------------------------------------------------------

#define RECTWIDTH(r) (r.right-r.left)
#define RECTHEIGT(r) (r.bottom-r.top)

static
void ChartWnd_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    RECT rc{};
    BOOL b = GetClientRect(hwnd, &rc);
    _ASSERT(b);

    const long rcWidth = RECTWIDTH(rc);
    const long rcHeight = RECTHEIGT(rc);

    ___windowHeight = rcHeight;
    ___windowChartRatio = ___windowHeight / ___chartHeight;

    //OutputDebugString(L"ChartWnd_OnSize\n");

    SCROLLINFO si{};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE;
    si.nPage = rcWidth;

    SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
}

//-----------------------------------------------------------------------------

static
void ChartWnd_OnHScroll_LineRight(HWND hwnd)
{
    RECT rc{};
    BOOL b = GetClientRect(hwnd, &rc);
    _ASSERT(b);

    const long rcWidth = RECTWIDTH(rc);
    const long leftOffset = ___leftOffset;
    const long dataWidth = ___dataWidth;
    const long x = leftOffset + rcWidth;

    if(x >= dataWidth)
    {
        return;
    }

    ___leftOffset += 15;

    _ASSERT(___leftOffset > 0);
    _ASSERT(___leftOffset % 15 == 0);

    //-----------------------------------------------------------------

    SCROLLINFO si{};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_POS;
    si.nPos = ___leftOffset;

    SetScrollInfo(hwnd, 0, &si, TRUE);

    //-----------------------------------------------------------------

    InvalidateRect(hwnd, nullptr, TRUE);
}

//-----------------------------------------------------------------------------

static
void ChartWnd_OnHScroll_LineLeft(HWND hwnd)
{
    if(___leftOffset > 0)
    {
        ___leftOffset -= 15;

        _ASSERT(___leftOffset >= 0);
        _ASSERT(___leftOffset % 15 == 0);

        //-----------------------------------------------------------------

        SCROLLINFO si{};
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_POS;
        si.nPos = ___leftOffset;

        SetScrollInfo(hwnd, 0, &si, TRUE);

        //-----------------------------------------------------------------

        InvalidateRect(hwnd, nullptr, TRUE);
    }
}

//-----------------------------------------------------------------------------

static
void ChartWnd_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
    switch(code)
    {
    case SB_LINERIGHT:
        ChartWnd_OnHScroll_LineRight(hwnd);
        break;

    case SB_LINELEFT:
        ChartWnd_OnHScroll_LineLeft(hwnd);
        break;

    case SB_ENDSCROLL:
        OutputDebugString(L"SB_ENDSCROLL\n");
        break;
    }
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK ChartWnd_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        HANDLE_MSG(hwnd, WM_HSCROLL, ChartWnd_OnHScroll);
        HANDLE_MSG(hwnd, WM_CREATE , ChartWnd_OnCreate );
        HANDLE_MSG(hwnd, WM_SIZE   , ChartWnd_OnSize   );

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
