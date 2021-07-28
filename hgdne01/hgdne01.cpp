// hgdne01.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "hgdne01.h"
#include "MainWnd.h"
#include "ChartWnd.h"

#include "PaintScope.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE ___hInstance;                                // current instance
//WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
//WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
BOOL                InitInstance(HINSTANCE, int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    //LoadStringW(hInstance, IDC_HGDNE01, szWindowClass, MAX_LOADSTRING);
    
    ChartWnd_RegisterClass(hInstance);
    MainWnd_RegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HGDNE01));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    ___hInstance = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(MAINWND_CLASSNAME, MAINWND_TITLE, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if(!hWnd)
    {
        return FALSE;
    }

    //nCmdShow = SW_MAXIMIZE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}
