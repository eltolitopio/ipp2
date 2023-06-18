#include "main.h"
 #include "stdafx.h



static LPCTSTR lpszWndTitle = _T("ipp_lb02_06");
static LPCTSTR lpszWndClass = _T("ipp_lb02_06");
static HWND hWndMain;
static HANDLE hEventH;
static HANDLE hEventV;

/*
 * NOTE: Compile as C code, not C++!
 * Properties -> C/C++ -> Advanced -> Compile as
 */
FIGURE hFigure = {
    .dir = 3,
    .h = 80,
    .w = 60,
    .x = 200,
    .y = 20,
};

FIGURE vFigure = {
    .dir = -1,
    .h = 20,
    .w = 40,
    .x = 40,
    .y = 200,
};

DWORD CALLBACK VFigureThread(LPVOID lpParam)
{
    FIGURE* f = (FIGURE*)lpParam;
    RECT rt;
    BOOL bCollision = FALSE;

    while (TRUE) {
        GetClientRect(hWndMain, &rt);
        bCollision = (f->y <= (INT32)(hFigure.y + hFigure.h)) && ((INT32)(f->y + f->h) >= hFigure.y);

        if (bCollision) {
            WaitForSingleObject(hEventH, INFINITE);
            ResetEvent(hEventV);
        }

        if (f->y <= rt.top)
            f->dir = abs(f->dir);

        if ((INT32)(f->y + f->h) >= rt.bottom)
            f->dir = -abs(f->dir);

        f->y += f->dir;

        if (!bCollision)
            SetEvent(hEventV);

        Sleep(10);
    }

    return TRUE;
}

DWORD CALLBACK HFigureThread(LPVOID lpParam)
{
  
FIGURE* f = (FIGURE*)lpParam;
RECT rt;
BOOL bCollision = FALSE;

while (TRUE) {
    GetClientRect(hWndMain, &rt);
    bCollision = (f->x <= (INT32)(vFigure.x + vFigure.w)) && ((INT32)(f->x + f->w) >= vFigure.x);

    if (bCollision) {
        WaitForSingleObject(hEventV, INFINITE);
        ResetEvent(hEventH);
    }

    if (f->x <= rt.left)
        f->dir = abs(f->dir);

    if ((INT32)(f->x + f->w) >= rt.right)
        f->dir = -abs(f->dir);

    f->x += f->dir;

    if (!bCollision)
        SetEvent(hEventH);

    Sleep(10);
}

return TRUE;
}

ATOM MyRegisterClass(HINSTANCE hInst) {
    WNDCLASSEX wcex;
    SecureZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_IPPLB0106));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_IPPLB0106);
    wcex.lpszClassName = lpszWndClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, INT nCmdShow) {
    hWndMain = CreateWindowEx(
        WS_EX_COMPOSITED,
        lpszWndClass,
        lpszWndTitle,
        

WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,
    NULL, NULL, hInstance, NULL
);

if (!hWndMain)
    return FALSE;

ShowWindow(hWndMain, nCmdShow);
UpdateWindow(hWndMain);
return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TIMER: {
            switch (wParam) {
                case 5000:
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                DEFAULT_UNREACHABLE;
            }
            break;
        }
        case WM_PAINT: {
            #define GREEN_COLOR RGB(30, 200, 10)
            #define YELLOW_COLOR RGB(220, 230, 10)

            HGDIOBJ hObj;
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint(hWnd, &ps);

            hObj = SelectObject(hDC, CreateSolidBrush(GREEN_COLOR));
            Rectangle(hDC, vFigure.x, vFigure.y, vFigure.x + vFigure.w, vFigure.y + vFigure.h);
            DeleteObject(hObj);

            hObj = SelectObject(hDC, CreateSolidBrush(YELLOW_COLOR));
            Ellipse(hDC, hFigure.x, hFigure.y, hFigure.x + hFigure.w, hFigure.y + hFigure.h);
            DeleteObject(hObj);

            EndPaint(hWnd, &ps);
            break;
        }
        case WM_CLOSE:
            SendMessage(hWnd, WM_DESTROY, 0, 0);
            break;
        case WM_DESTROY:
           


CloseHandle(hEventH);
CloseHandle(hEventV);
KillTimer(hWnd, 5000);
PostQuitMessage(0);
break;
case WM_CREATE: {
    hEventH = CreateEventEx(
        NULL, _T("CollisionH"),
        CREATE_EVENT_INITIAL_SET | CREATE_EVENT_MANUAL_RESET,
        EVENT_ALL_ACCESS
    );
    hEventV = CreateEventEx(
        NULL, _T("CollisionV"),
        CREATE_EVENT_INITIAL_SET | CREATE_EVENT_MANUAL_RESET,
        EVENT_ALL_ACCESS
    );
    SetTimer(hWnd, 5000, 20, NULL);
    CreateThreadEx(NULL, 0, HFigureThread, &hFigure, 0, NULL);
    CreateThreadEx(NULL, 0, VFigureThread, &vFigure, 0, NULL);
}
break;
default:
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
return S_OK;
}

INT APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpszCmd, INT nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInst);
    UNREFERENCED_PARAMETER(lpszCmd);
    MyRegisterClass(hInst);
    // Perform application initialization
    if (!InitInstance(hInst, nCmdShow))
        return FALSE;

    MSG msg;
    // Main message loop
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (INT)msg.wParam;
}



