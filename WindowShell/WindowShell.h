//#pragma once

#include "resource.h"
#include <windows.h>

extern HINSTANCE	g_hInst;
extern HWND			g_hwndMainWnd;
//extern HANDLE		g_hApplicationMutex;
//extern DWORD		g_dwLastError;
extern BOOL			g_bStartSearchWindow;
extern HCURSOR		g_hCursorSearchWindow;
extern HCURSOR		g_hCursorPrevious;
extern HBITMAP		g_hBitmapFinderToolFilled;
extern HBITMAP		g_hBitmapFinderToolEmpty;
extern HWND			g_hwndFoundWindow;
extern HPEN			g_hRectanglePen;


#define WM_START_SEARCH_WINDOW			WM_USER + 100
#define WM_SET_HOOK						WM_USER + 101
#define WM_UNSET_HOOK					WM_USER + 102