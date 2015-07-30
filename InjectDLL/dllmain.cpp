// dllmain.cpp: ���������� ����� ����� ��� ���������� DLL.
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
			// ���������� ������ ��� ����� ����� ������� ������ DLL � ���� ������
			hHookDll=hModule;
			AppActivated=0;
		}
		break;
	case DLL_THREAD_ATTACH:break;
	case DLL_THREAD_DETACH:break;
	case DLL_PROCESS_DETACH:
		// ������� ����
		// ������� ������ ��� �������������
		SetHook(FALSE, NULL);
		break;
	default:	break;
	}
	return TRUE;
}

EXPORT int SetHook(BOOL bSet, HWND hwnd_loc) // ������� �������������/������� ��� � ����������� �� bSet
{
	if(bSet) // ���� �������������
	{
		
		if(hHookDll) // ���� ��������� ����������
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


			// �� ��������� ���
			mouseHLock = SetWindowsHookEx(	WH_MOUSE, // ��� ����
				&MouseProc,    // ���-�������
				hHookDll,	 // ����� ���������� � ���-��������
				dwThreadId);
			cbtHLock = SetWindowsHookEx(WH_CBT,&CBTProc,hHookDll,dwThreadId);
			if(mouseHLock == NULL) // ���� ��� �� �����������, ������ ������
				return -2;
			if(cbtHLock == NULL)
				return -2;
			return sesion_code;

		}
		else return -1;
	}
	else // ���� ������� ���
	{
		UnhookWindowsHookEx(mouseHLock);
		UnhookWindowsHookEx(cbtHLock);
		sesion_finish();
	}
	return sesion_code;
}