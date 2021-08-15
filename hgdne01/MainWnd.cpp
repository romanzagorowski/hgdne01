#include "pch.h"
#include "framework.h"
#include "MainWnd.h"
#include "resource.h"
#include "ChartWnd.h"

//-----------------------------------------------------------------------------

static
HWND ___hwndChart = nullptr;

//-----------------------------------------------------------------------------

INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

//-----------------------------------------------------------------------------

ATOM MainWnd_RegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wc{};

    wc.cbSize = sizeof(WNDCLASSEX);

    //wcex.style = CS_HREDRAW | CS_VREDRAW;
    wc.style = 0;
    wc.lpfnWndProc = MainWnd_WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HGDNE01));
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCEW(IDC_HGDNE01);
    wc.lpszClassName = MAINWND_CLASSNAME;
    wc.hIconSm = LoadIcon(wc.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wc);
}

//-----------------------------------------------------------------------------

static
BOOL MainWnd_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    extern HINSTANCE ___hInstance;

    RECT rc{};
    BOOL bGot = GetClientRect(hwnd, &rc);
    _ASSERT(bGot);

    ___hwndChart = CreateWindowEx(
        0
    ,   CHARTWND_CLASSNAME
    ,   nullptr
//  ,   WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL
    ,   WS_CHILD | WS_VISIBLE
        ,   0, 0, rc.right, rc.bottom
    ,   hwnd
    ,   nullptr
    ,   lpCreateStruct->hInstance
    ,   nullptr
    );

    return nullptr != ___hwndChart ? TRUE : FALSE;
}

//-----------------------------------------------------------------------------

static
void MainWnd_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    MoveWindow(___hwndChart, 0, 0, cx, cy, TRUE);
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK MainWnd_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        HANDLE_MSG(hwnd, WM_SIZE, MainWnd_OnSize);
        HANDLE_MSG(hwnd, WM_CREATE, MainWnd_OnCreate);
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch(wmId)
            {
            case IDM_ABOUT:
                extern HINSTANCE ___hInstance;
                DialogBox(___hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hwnd);
                break;
            case ID_FILE_OPEN:
            {
                WCHAR szFile[260] = L"";
                OPENFILENAME ofn = {};
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                ofn.nMaxFile = 260;
                ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
                ofn.nFilterIndex = 1;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                BOOL bGot = GetOpenFileName(&ofn);
            }
            break;
            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
            }
        }
        break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hwnd, &ps);
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch(message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
