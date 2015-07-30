#pragma once
#include <windows.h>
typedef struct 
{ 
	TCHAR achName[MAX_PATH]; 
	TCHAR achPosition[12]; 
	int nGamesPlayed; 
	int nGoalsScored; 
} Player; 

extern Player Roster[];



typedef struct _session_from_table
{ 
	int id;
	TCHAR title[MAX_PATH]; 
	TCHAR screenshot_path[MAX_PATH];
	int x, y;

} Sesion; 

extern Sesion* sesion;
extern int elements_count;

extern BOOL sesion_is_changed;
extern int index_of_sesion;