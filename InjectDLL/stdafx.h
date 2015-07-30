// stdafx.h: ���������� ���� ��� ����������� ��������� ���������� ������
// ��� ���������� ������ ��� ����������� �������, ������� ����� ������������, ��
// �� ����� ����������
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ��������� ����� ������������ ���������� �� ���������� Windows
// ����� ���������� Windows:
#include <windows.h>



// TODO: ���������� ����� ������ �� �������������� ���������, ����������� ��� ���������
#include "dll.h"

#ifdef __cplusplus
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT __declspec (dllexport)
#endif

EXPORT LRESULT CALLBACK MouseProc(int code, WPARAM wParam, LPARAM lParam);
EXPORT LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);

EXPORT int SetHook(BOOL bSet, HWND hwnd_loc);
EXPORT HWND GetParentHWND();

EXPORT void LogMessageInFile(char* msg);

