// WindowShell.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "WindowShell.h"
#include "PaintModule.h"
#include "VisualCalculator.h"
#include "ListBoxData.h"
#include "WindowFinder.h"

#pragma comment (lib,"InjectDLL.lib")
#pragma comment (lib,"Msimg32.lib")


#ifdef __cplusplus
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT __declspec (dllexport)
#endif






#define MAX_LOADSTRING 100

BOOL key_info	= false;
BOOL key_bmp	= false;
//////////////////////////////////////////////////////////////////////////

//char		szMainWindowClassName[] = "WindowFinderMainWindow"; // Name of the Main Window class.
HINSTANCE	g_hInst = NULL;
HWND		g_hwndMainWnd = NULL;
//HANDLE		g_hApplicationMutex = NULL;
//DWORD		g_dwLastError = 0;
BOOL		g_bStartSearchWindow = FALSE;
HCURSOR		g_hCursorSearchWindow = NULL;
HCURSOR		g_hCursorPrevious = NULL;
HBITMAP		g_hBitmapFinderToolFilled;
HBITMAP		g_hBitmapFinderToolEmpty;
HWND		g_hwndFoundWindow = NULL;
HPEN		g_hRectanglePen = NULL;

//////////////////////////////////////////////////////////////////////////

// Глобальные переменные:
//HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ShowInfo(HWND, UINT, WPARAM, LPARAM);


BOOL InitialiseResources();
BOOL UninitialiseResources();
//////////////////////////////////////////////////////////////////////////
void my_alpha_blend(HWND hWnd, HDC hdc);

// Власний імпорт
bool bSetHook;
EXPORT int SetHook(BOOL bSet, HWND hWnd_local = NULL);


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINDOWSHELL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	// Выполнить инициализацию приложения:
	if (!InitialiseResources ())
	{
		UninitialiseResources();
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSHELL));

	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSHELL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINDOWSHELL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   //HWND hWnd;

   g_hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   g_hwndMainWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 228+16-8, 322+58, NULL, NULL, hInstance, NULL);

   if (!g_hwndMainWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hwndMainWnd, nCmdShow);
   UpdateWindow(g_hwndMainWnd);

   return TRUE;
}

BOOL InitialiseResources()
{
	BOOL bRet = FALSE;

	g_hCursorSearchWindow = LoadCursor (g_hInst, MAKEINTRESOURCE(IDC_CURSOR_SEARCH_WINDOW));
	if (g_hCursorSearchWindow == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	g_hRectanglePen = CreatePen (PS_SOLID, 3, RGB(256, 0, 0));
	if (g_hRectanglePen == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	g_hBitmapFinderToolFilled = LoadBitmap (g_hInst, MAKEINTRESOURCE(IDB_BITMAP_FINDER_FILLED));
	if (g_hBitmapFinderToolFilled == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	g_hBitmapFinderToolEmpty = LoadBitmap (g_hInst, MAKEINTRESOURCE(IDB_BITMAP_FINDER_EMPTY));
	if (g_hBitmapFinderToolEmpty == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	// All went well. Return TRUE.
	bRet = TRUE;

InitialiseResources_0:

	return bRet;
}





BOOL UninitialiseResources()
{
	BOOL bRet = TRUE;

	if (g_hCursorSearchWindow)
	{
		// No need to destroy g_hCursorSearchWindow. It was not created using 
		// CreateCursor().
	}

	if (g_hRectanglePen)
	{
		bRet = DeleteObject (g_hRectanglePen);
		g_hRectanglePen = NULL;
	}

	if (g_hBitmapFinderToolFilled)
	{
		DeleteObject (g_hBitmapFinderToolFilled);
		g_hBitmapFinderToolFilled = NULL;
	}

	if (g_hBitmapFinderToolEmpty)
	{
		DeleteObject (g_hBitmapFinderToolEmpty);
		g_hBitmapFinderToolEmpty = NULL;
	}

	return bRet;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	int res;
	PAINTSTRUCT ps;
	HDC hdc,hCompatibleDC;
	HANDLE hBitmap, hOldBitmap;
	RECT Rect;
	BITMAP Bitmap;



	switch (message)
	{
	
	case WM_SET_HOOK:
		{
			HWND hWnD = (HWND)lParam;
			res = SetHook(true,hWnD);
			bSetHook=true;

			WCHAR szText[256];
			// Display some information on the found window.
			wsprintf(szText, L"Номер Сесії:%d",	res	);


			HWND hwndDlg =  (HWND)wParam; 
			SetDlgItemText (hwndDlg, IDC_STATIC_SESION_NUMBER, szText);
		}
		break;
	case WM_UNSET_HOOK:
		{
			SetHook(false);
			bSetHook=false;
			HWND hwndDlg =  (HWND)wParam; 
			SetDlgItemText (hwndDlg, IDC_STATIC_SESION_NUMBER, L"Неактивно");
		}
		break;
	case WM_START_SEARCH_WINDOW:

		StartSearchWindowDialog (hWnd);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case IDM_HOOK_SET:
			/*
			res = SetHook(true);
			if(res==1)MessageBox(hWnd,L"Error1",0,NULL);
			if(res==2)MessageBox(hWnd,L"Error2",0,NULL);
			if(res==55)MessageBox(hWnd,L"Good",0,NULL);
			bSetHook=true;
			break;
			*/
		case IDM_HOOK_UNSET:
			/*
			SetHook(false);
			bSetHook=false;
			break;
			*/
		case IDM_FIND_WINDOW:

			PostMessage (hWnd, WM_START_SEARCH_WINDOW, 0, 0);
			break;

		case IDM_SHOW_ACT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_SHOWINFO), hWnd, ShowInfo);

			break;
		case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			PostMessage (hWnd, WM_DESTROY, 0, 0);
			//DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);


		if(key_bmp && sesion_is_changed){
			
			hBitmap = LoadImageW(NULL, sesion[index_of_sesion].screenshot_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			int res = GetObjectW(hBitmap, sizeof(BITMAP), &Bitmap);
			hCompatibleDC = CreateCompatibleDC(hdc);
			hOldBitmap = SelectObject(hCompatibleDC, hBitmap);
			GetWindowRect(hWnd, &Rect);
			
			StretchBlt(hdc, -8, -(50-20), Bitmap.bmWidth, Bitmap.bmHeight, hCompatibleDC, 0, 0, Bitmap.bmWidth, 
				Bitmap.bmHeight, SRCCOPY);

			
			SelectObject(hCompatibleDC, hOldBitmap);
			DeleteObject(hBitmap);
			DeleteDC(hCompatibleDC);
			
		}
		if(key_info && sesion_is_changed){
			my_alpha_blend(hWnd,hdc);
		}
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		SetHook(false);
		bSetHook=false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK ShowInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if(LOWORD(wParam) == IDC_BUTTON_SYNCHONYZE){

			get_sesion_info();
			

			
			// Add items to list. 
			HWND hwndList = GetDlgItem(hDlg, IDC_LIST_SESSION);  
			for (int i = 0; i < elements_count; i++) 
			{ 
				int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM) sesion[i].title); 
				// Set the array index of the player as item data.
				// This enables us to retrieve the item from the array
				// even after the items are sorted by the list box.
				SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM) i); 
			} 
			// Set input focus to the list box.
			SetFocus(hwndList); 



		}
		if(LOWORD(wParam) == IDC_LIST_SESSION){
			switch (HIWORD(wParam)) 
			{ 
				case LBN_SELCHANGE:
				{
					HWND hwndList = GetDlgItem(hDlg, IDC_LIST_SESSION); 

					// Get selected index.
					int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0); 

					// Get item data.
					int i = (int)SendMessageW(hwndList, LB_GETITEMDATA, lbItem, 0);

					// Do something with the data from Roster[i]
					TCHAR buff[MAX_PATH];
					wsprintf(buff,L"Вибрана сесія:%d;", sesion[i].id);
					index_of_sesion = i;
					sesion_is_changed = TRUE;
					SetDlgItemText(hDlg, IDC_EDIT_SELECTED_SESION, buff);

					RECT tsp;
					GetWindowRect(g_hwndMainWnd,&tsp);
					if(sesion_is_changed)
					MoveWindow(g_hwndMainWnd,tsp.left,tsp.top,sesion[index_of_sesion].x,sesion[index_of_sesion].y+20,FALSE);


					
					GetClientRect(g_hwndMainWnd,&tsp);
					InvalidateRect(g_hwndMainWnd,&tsp,TRUE);
					

					return TRUE; 
				} 
			}

		}
		if(LOWORD(wParam) == IDC_BUTTON_SHOW_INFO){
			
			key_info = ~key_info;
			key_bmp	= ~key_bmp;
			RECT tsp;
			GetWindowRect(g_hwndMainWnd,&tsp);
			if(sesion_is_changed)
			MoveWindow(g_hwndMainWnd,tsp.left,tsp.top,sesion[index_of_sesion].x,sesion[index_of_sesion].y+20,FALSE);
			GetClientRect(g_hwndMainWnd,&tsp);
			InvalidateRect(g_hwndMainWnd,&tsp,TRUE);
		}

		break;
	default:

		break;
	}
	return (INT_PTR)FALSE;
}

void my_alpha_blend(HWND hWnd, HDC hdc){

	RECT tsp;
	GetClientRect(hWnd,&tsp);
	HDC tempHdc         = CreateCompatibleDC(hdc);
	BLENDFUNCTION blend = {AC_SRC_OVER, 0, 192, 0};

	HBITMAP hbitmap;       // bitmap handle 
	BITMAPINFO bmi;        // bitmap header 
	// zero the memory for the bitmap info 
	ZeroMemory(&bmi, sizeof(BITMAPINFO));

	// setup bitmap info  
	// set the bitmap width and height to 60% of the width and height of each of the three horizontal areas. Later on, the blending will occur in the center of each of the three areas. 
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = tsp.right;
	bmi.bmiHeader.biHeight = tsp.bottom;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;         // four 8-bit components 
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = (tsp.right) * (tsp.bottom) * 4;

	// create our DIB section and select the bitmap into the dc 
	hbitmap = CreateDIBSection(tempHdc, &bmi, DIB_RGB_COLORS, NULL, NULL, 0x0);
	SelectObject(tempHdc, hbitmap);
	//////////////////////////////////////////////////////////////////////////
	init_my_graphics(tempHdc);
	POINT p;
	p.x = 0; 
	p.y = 50-30;
	move_to_start(&p);
	render();
	//////////////////////////////////////////////////////////////////////////
	AlphaBlend(hdc, 0, 0, tsp.right,tsp.bottom,tempHdc, 0, 0,tsp.right,tsp.bottom,blend);

}