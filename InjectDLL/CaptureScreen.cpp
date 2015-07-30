#include "stdafx.h"

#pragma  once
#include <windows.h>
#include "DataBaseAdapter.h"


char * tmp = new char[64];


void CaptureScreen(HWND window);

char* GetCurentFileName(int duble = 1){
	delete tmp;

	char* ggg;
	if(duble == 1)ggg = "\\";
	else ggg = "\\\\";

	char filename_pattern[] = "D:%sdiploma_img%s%08d.bmp";
	tmp = new char[sizeof filename_pattern + 11];
	wsprintfA(tmp,filename_pattern,ggg,ggg,sesion_code);
	return tmp;
}


void CaptureScreen(char wnd_name[]){
	HWND hwnd;   
	hwnd = FindWindowA(NULL,wnd_name);

	//RECT wR,cR;
	//GetWindowRect(hwnd, &wR);
	//GetClientRect(hwnd, &cR);
	//printf("WINDOW RECT:{%04d, %04d, %04d, %04d};\nCLIENT RECT:{%04d, %04d, %04d, %04d};\n", wR.left,wR.top,wR.right, wR.bottom, cR.left,cR.top,cR.right, cR.bottom);

	CaptureScreen(hwnd);
}

void CaptureScreen(HWND window)
{
	//Получаем прямоугольную область экрана
	RECT windowRect;
	GetWindowRect(window, &windowRect);

	//Размеры битмэпа
	int bitmap_dx = windowRect.right-windowRect.left;
	int bitmap_dy = windowRect.bottom-windowRect.top;

	BITMAPINFOHEADER bmpInfoHeader;
	BITMAPFILEHEADER bmpFileHeader;
	BITMAP* pBitmap;

	bmpFileHeader.bfType = 0x4d42;
	bmpFileHeader.bfSize = 0;
	bmpFileHeader.bfReserved1 = 0;
	bmpFileHeader.bfReserved2 = 0;
	bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bmpInfoHeader.biSize = sizeof(bmpInfoHeader);
	bmpInfoHeader.biWidth = bitmap_dx;
	bmpInfoHeader.biHeight = bitmap_dy;
	bmpInfoHeader.biPlanes = 1;
	bmpInfoHeader.biBitCount = 24;
	bmpInfoHeader.biCompression = BI_RGB;
	bmpInfoHeader.biSizeImage = bitmap_dx*bitmap_dy*(24/8);
	bmpInfoHeader.biXPelsPerMeter = 0;
	bmpInfoHeader.biYPelsPerMeter = 0;
	bmpInfoHeader.biClrUsed = 0;
	bmpInfoHeader.biClrImportant = 0;

	BITMAPINFO info;
	info.bmiHeader = bmpInfoHeader;

	BYTE* memory;
	HDC winDC = GetWindowDC(window);
	HDC bmpDC = CreateCompatibleDC(winDC);
	//Создаем битмэп
	HBITMAP bitmap = CreateDIBSection(winDC, &info, DIB_RGB_COLORS, (void**)&memory, NULL, 0);
	SelectObject(bmpDC, bitmap);//Выбираем в контекст битмэп
	BitBlt(bmpDC, 0, 0, bitmap_dx, bitmap_dy, winDC, 0, 0, SRCCOPY);
	ReleaseDC(window, winDC);

	/*

	OPENFILENAME ofn;//Указатель на структуру с данными инициализации диалогового окна
	char strFilter[] = "Файлы данных *.bmp\0";
	char strFileName[MAX_PATH] = "";
	memset(&ofn, 0, sizeof(OPENFILENAME));//Обнуление ofn
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = window;
	ofn.lpstrFilter = strFilter;
	ofn.lpstrFile = strFileName;//Буфер для имени файла
	ofn.nMaxFile = MAX_PATH;//Размер файла
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
	strcpy_s(strFileName, ofn.lpstrFile);
	GetSaveFileName(&ofn); //MessageBox(hwnd,"Невозможно сохранить файл","О программе...",MB_ICONINFORMATION);

	*/

	//int index_file = 25;


	HANDLE hFile = CreateFileA(
		GetCurentFileName(), 
		GENERIC_WRITE, 
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
		return;
	DWORD dwWritten = 0;
	WriteFile(hFile, &bmpFileHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hFile, &bmpInfoHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	WriteFile(hFile, memory, bmpInfoHeader.biSizeImage, &dwWritten, NULL);
	CloseHandle(hFile);
}