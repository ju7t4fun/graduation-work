// InjectDLL.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include <stdio.h>
#include "DataBaseAdapter.h"







EXPORT LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam)
{
	//////////////////////////////////////////////////////////////////////////
	// отримання координат миші
	//////////////////////////////////////////////////////////////////////////
	MOUSEHOOKSTRUCT *mhs = (MOUSEHOOKSTRUCT*) lParam;
	POINT*		 screenpoint = &mhs->pt;

	WM_MOUSEMOVE;
	//////////////////////////////////////////////////////////////////////////
	// запис даних у базу
	//////////////////////////////////////////////////////////////////////////
	logMouseActonToBase(code,wParam, screenpoint, mhs->hwnd);
	return CallNextHookEx(NULL, code, wParam, lParam);
}



/**************************************************************** 
  WH_CBT hook procedure 
 ****************************************************************/ 
 
EXPORT LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam) 
{ 
	//LogMessageInFile("Work!!!");
	/*
    CHAR szBuf[128]; 
    CHAR szCode[128]; 
    HDC hdc; 
    static int c = 0; 
    size_t cch; 
    HRESULT hResult;
 */
    if (nCode < 0)  // do not process message 
        return CallNextHookEx(NULL, nCode, wParam, 
            lParam); 
 

 
    switch (nCode) 
    { 
        case HCBT_ACTIVATE:     break; 
 
        case HCBT_CLICKSKIPPED: break; 
 
        case HCBT_CREATEWND:    break; 
 
        case HCBT_DESTROYWND:   break; 
 
        case HCBT_KEYSKIPPED:   break; 
 
        case HCBT_MINMAX:       break; 
 
        case HCBT_MOVESIZE:{
			//HWND hwnd = (HWND)wParam;
			RECT rect = *(LPRECT)lParam;
			//GetWindowRect( hwnd, &rect );
			logWindowRectToBase(&rect);
			//LogMessageInFile("HellYEAH!!!Resize!");
						   }
            break; 
 
        case HCBT_QS:
            break; 
 
        case HCBT_SETFOCUS:
			break; 
        case HCBT_SYSCOMMAND:
            break; 
 
        default:
           
            break; 
    } 

    
    return CallNextHookEx(NULL, nCode, wParam, lParam); 
} 







EXPORT void LogMessageInFile(char* msg){

	HANDLE file = CreateFile(
		TEXT("C:\\mouse.log"), 
		GENERIC_WRITE | GENERIC_READ, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
	DWORD realWrite;
	int sizeToWrite = strlen(msg);
	SetFilePointer( file, 
		NULL, 
		NULL, 
		FILE_END ); 
	BOOL isWrite =	WriteFile(file, msg, sizeToWrite, &realWrite, NULL);
	CloseHandle(file);

}