#include "ads.lab.2.3.h"
#include "graph.c"
#include "util.c"
#include "drawing.c"


ATOM registerWndClass (HINSTANCE hInstance);
boolean initWindow (HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK wndProc (HWND, UINT, WPARAM, LPARAM);

struct graph* graph;

/**********************************************************************************************************************/

int APIENTRY WinMain (_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
    graph = initialise_graph();

    if (!registerWndClass(hInstance))
        return 0;

    if (!initWindow(hInstance, nCmdShow))
        return 0;

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

/**********************************************************************************************************************/
/*
 * Function that creates, sets up, and registers a WNDCLASS.
 */

ATOM registerWndClass (HINSTANCE hInstance) {
    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255));
    WNDCLASS wndClass = {
            .lpszClassName = PROGRAM_NAME,
            .lpszMenuName = NULL,

            .hInstance=hInstance,
            .lpfnWndProc =  wndProc,

            .hIcon = NULL,
            .hCursor = LoadCursor(hInstance, IDC_ARROW),
            .hbrBackground = brush,

            .style = CS_HREDRAW | CS_VREDRAW,

            .cbClsExtra = 0,
            .cbWndExtra = 0,
    };
    
    return RegisterClass(&wndClass);
}

/**********************************************************************************************************************/
/*
 * Initialises and shows the window.
 */

boolean initWindow (HINSTANCE hInstance, int nCmdShow) {
   HWND hWnd = CreateWindow(PROGRAM_NAME, WINDOW_NAME, WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, WINDOW_HEIGHT, CW_USEDEFAULT, WINDOW_WIDTH,
                            NULL, NULL, hInstance, NULL);

   if (!hWnd) {
       printf("%d", GetLastError());
       return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

/**********************************************************************************************************************/

LRESULT CALLBACK wndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rect;
            GetClientRect(hWnd, &rect);
            render_graph(hdc, graph, rect.bottom, rect.right);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}