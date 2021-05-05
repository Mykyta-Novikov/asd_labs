#include "ads.lab.2.5.h"
#include <string.h>
#include "graph.c"
#include "util.c"
#include "drawing.c"


ATOM registerWndClass (HINSTANCE hInstance);
boolean initWindow (HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK wndProc (HWND, UINT, WPARAM, LPARAM);
void parse_arguments (char* cmd_line);


struct graph* graph;

enum ui_state screen = MAIN;
enum ui_state previous_screen = MAIN;
int sequence_step = 0;
int max_sequence_step;

/**********************************************************************************************************************/

int APIENTRY WinMain (_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
    parse_arguments(lpCmdLine);

    graph = initialise_graph();
    max_sequence_step = graph->sequence->count - 1;

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
 * Parses command line arguments and writes global flags.
 */

void parse_arguments (char* cmd_line) {
    char* next_argument = strtok(cmd_line, " ");
    while (next_argument != NULL) {

        if (!strcasecmp(next_argument, "-u"))
            directed_graph = 0;
        else if (!strcasecmp(next_argument, "-b"))
            use_dfs = 0;

        next_argument = strtok(NULL, " ");
    }
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

#define redraw(window) RedrawWindow(window, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW)

LRESULT CALLBACK wndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rect;
            GetClientRect(hWnd, &rect);
            render_graph(hdc, graph, rect.bottom, rect.right, sequence_step, screen);

            EndPaint(hWnd, &ps);
        }
        break;
        case WM_KEYDOWN:
            switch (screen) {
                case INITIAL:            // screen showing the whole graph
                    switch (wParam) {
                        case VK_ESCAPE:         // return to previous screen
                            screen = previous_screen;
                            redraw(hWnd);
                            break;
                        default:
                            DefWindowProc(hWnd, message, wParam, lParam);
                    }
                    break;
                case MAIN:           // screen showing graph search
                    switch (wParam) {
                        case VK_ESCAPE:         // go tp INITIAL screen
                            previous_screen = screen;
                            screen = INITIAL;
                            redraw(hWnd);
                            break;
                        case VK_SPACE:          // got to SPANNING_TREE screen
                            screen = SPANNING_TREE;
                            redraw(hWnd);
                            break;
                        case VK_RIGHT:          // go to next step, if step is already last, go to SPANNING_TREE screen
                            if (sequence_step == max_sequence_step)
                                screen = SPANNING_TREE;
                            else
                                sequence_step++;
                            redraw(hWnd);
                            break;
                        case VK_LEFT:           // go to previous step
                            sequence_step = max(0, sequence_step - 1);
                            redraw(hWnd);
                            break;
                        default:
                            DefWindowProc(hWnd, message, wParam, lParam);
                    }
                    break;
                case SPANNING_TREE:           // screen spanning tree of the graph after search
                    switch (wParam) {
                        case VK_ESCAPE:         // go tp INITIAL screen
                            previous_screen = screen;
                            screen = INITIAL;
                            redraw(hWnd);
                            break;
                        case VK_RIGHT:           // set step to first and go to STANDART screen
                            sequence_step = 0;
                            screen = MAIN;
                            redraw(hWnd);
                            break;
                        case VK_LEFT:           // set step to last and go to STANDART screen
                            sequence_step = max_sequence_step;
                            screen = MAIN;
                            redraw(hWnd);
                            break;
                        case VK_SPACE:          // go to STANDART screen
                            screen = MAIN;
                            redraw(hWnd);
                            break;
                        default:
                            DefWindowProc(hWnd, message, wParam, lParam);
                    }
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