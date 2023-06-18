#pragma once

#include "resource.h"
#include "stdafx.h"

/* Win32-like wrapper for _beginthreadex */
#define CreateThreadEx(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId) \
    (HANDLE)_beginthreadex(lpThreadAttributes, dwStackSize, (_beginthreadex_proc_type)lpStartAddress, lpParameter, dwCreationFlags, lpThreadId)

ATOM MyRegisterClass(HINSTANCE hInst);
BOOL InitInstance(HINSTANCE hInstance, INT nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct tagFIGURE {
    UINT32 w;
    UINT32 h;
    INT32 x;
    INT32 y;
    INT32 dir;
} FIGURE;

