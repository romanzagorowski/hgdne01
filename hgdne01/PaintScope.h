#pragma once

#include <windef.h>
#include <winuser.h>

class PaintScope
{
public:
    PaintScope(HWND hwnd)
        : hwnd(hwnd)
    {
        this->hdc = BeginPaint(this->hwnd, &this->ps);
    }

    ~PaintScope()
    {
        EndPaint(this->hwnd, &this->ps);
    }

public:
    HDC hdc;
    HWND hwnd;
    PAINTSTRUCT ps;
};
