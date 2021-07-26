#pragma once

#define MAINWND_TITLE L"hgdne01"
#define MAINWND_CLASSNAME L"MAINWND_CLASSNAME"

ATOM MainWnd_RegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK MainWnd_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
