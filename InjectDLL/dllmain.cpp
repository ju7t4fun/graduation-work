// dllmain.cpp: определяет точку входа для приложения DLL.
#include "stdafx.h"
#include "DataBaseAdapter.h"
#include "CaptureScreen.h"
HHOOK mouseHLock,keyHLock,cbtHLock;
HINSTANCE hHookDll;
int AppActivated;
HWND hwnd;

EXPORT HWND GetParentHWND(){
	return hwnd;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			// вызывается каждый раз когда новый процесс грузит DLL в свою память
			hHookDll=hModule;
			AppActivated=0;
		}
		break;
	case DLL_THREAD_ATTACH:break;
	case DLL_THREAD_DETACH:break;
	case DLL_PROCESS_DETACH:
		// Снимать хуки
		// Система делает это автоматически
		SetHook(FALSE, NULL);
		break;
	default:	break;
	}
	return TRUE;
}

EXPORT int SetHook(BOOL bSet, HWND hwnd_loc) // Функция устанавливает/снимает хук в зависимости от bSet
{
	if(bSet) // Если устанавливаем
	{
		
		if(hHookDll) // Если загружена библиотека
		{

			DWORD dwProcessId;
			DWORD dwThreadId;

			if(hwnd_loc==NULL)	hwnd = FindWindow(NULL,L"Calculator");
			else hwnd = hwnd_loc;
			dwThreadId = GetWindowThreadProcessId(hwnd, &dwProcessId);

			RECT rect;
			GetWindowRect( hwnd, &rect );


			sesion_start();
			CaptureScreen(hwnd);
			logWindowRectToBase(&rect);


			// то установим хук
			mouseHLock = SetWindowsHookEx(	WH_MOUSE, // тип хука
				&MouseProc,    // хук-функция
				hHookDll,	 // хендл библиотеки с хук-функцией
				dwThreadId);
			cbtHLock = SetWindowsHookEx(WH_CBT,&CBTProc,hHookDll,dwThreadId);
			if(mouseHLock == NULL) // Если хук не установился, вернем ошибку
				return -2;
			if(cbtHLock == NULL)
				return -2;
			return sesion_code;

		}
		else return -1;
	}
	else // Если снимаем хук
	{
		UnhookWindowsHookEx(mouseHLock);
		UnhookWindowsHookEx(cbtHLock);
		sesion_finish();
	}
	return sesion_code;
}