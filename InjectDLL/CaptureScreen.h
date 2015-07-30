#pragma  once
#include <windows.h>

void CaptureScreen(char wnd_name[]);
void CaptureScreen(HWND window);
char* GetCurentFileName(int duble = 1);