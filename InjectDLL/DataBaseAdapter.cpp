
#include "stdafx.h"

#include <stdio.h>
#include <my_global.h>
#include <mysql.h>
#include "DataBaseAdapter.h"

#include "CaptureScreen.h"

//#pragma comment (lib,"Z:\\mysql-connector-c\\lib\\libmysql.lib")

int sesion_code = 1;
char host[] = "localhost";
int port = 13306;
char user_name[] = "root";
char password[] = "DalayLayma55";

char time_format[] = "%d-%02d-%02d %02d:%02d:%02d";

HWND lastHWND = NULL;
int index_gui = 1;


void convert(HWND w, LONG &x, LONG &y)	{
	POINT tmp;
	tmp.x = x; tmp.y = y;
	ScreenToClient(w, &tmp);
	x = tmp.x; y = tmp.y;
}


//////////////////////////////////////////////////////////////////////////
// Запис початку сесії 
//////////////////////////////////////////////////////////////////////////
int sesion_start(){
	//////////////////////////////////////////////////////////////////////////
	// підключення до бази
	//////////////////////////////////////////////////////////////////////////

	MYSQL *con = mysql_init(NULL);

	if (con == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		return(1);
	}

	if (mysql_real_connect(con, host, user_name, password, NULL, port, NULL, 0) == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}  
	//////////////////////////////////////////////////////////////////////////
	// форумування локального часу
	//////////////////////////////////////////////////////////////////////////
	SYSTEMTIME sm;
	GetLocalTime(&sm);
	char date_str[64];
	unsigned int milisecont = sm.wMilliseconds;
	sprintf_s(date_str,time_format,sm.wYear,sm.wMonth,sm.wDay,sm.wHour,sm.wMinute,sm.wSecond);
	//////////////////////////////////////////////////////////////////////////
	// отрамання імені
	//////////////////////////////////////////////////////////////////////////
	char name[64];
	GetWindowTextA(GetParentHWND(),name,64);


	//////////////////////////////////////////////////////////////////////////
	// запис часу початку сесії в базу
	//////////////////////////////////////////////////////////////////////////

	char str[256];
	sprintf_s(str, "INSERT INTO `diploma`.`hook_sesion` (`SesionStart`,`ProgramName`) VALUES ('%s', '%s')", date_str,name);
	if (mysql_query(con, str)) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}
	//////////////////////////////////////////////////////////////////////////
	// отримання індексу сесії 
	//////////////////////////////////////////////////////////////////////////
	char lastIndex[] = "SELECT last_insert_id()";
	mysql_query(con, lastIndex);
	MYSQL_RES * result = mysql_store_result(con);
	int index =  atoi(mysql_fetch_row(result)[0]);
	mysql_free_result(result);

	sesion_code = index;
	//////////////////////////////////////////////////////////////////////////
	// запис імені файла та скріншот
	//////////////////////////////////////////////////////////////////////////

	char* file_name = GetCurentFileName(2);
	char update_path_pattern[] = "UPDATE `diploma`.`hook_sesion` SET `ScreenShotPath`='%s' WHERE `HookSesionID`='%d'";
	char update_path_buffer[256];
	wsprintfA(update_path_buffer,update_path_pattern,file_name,sesion_code);
	if (mysql_query(con, update_path_buffer)) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}

	//////////////////////////////////////////////////////////////////////////
	// заверешння роботи з базою
	//////////////////////////////////////////////////////////////////////////
	mysql_close(con);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Запис кінця сесії 
//////////////////////////////////////////////////////////////////////////
int sesion_finish(){
	if(sesion_code == 1) return (1);
	//////////////////////////////////////////////////////////////////////////
	// підключення до бази
	//////////////////////////////////////////////////////////////////////////

	MYSQL *con = mysql_init(NULL);

	if (con == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		return(1);
	}

	if (mysql_real_connect(con, host, user_name, password, NULL, port, NULL, 0) == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}  
	//////////////////////////////////////////////////////////////////////////
	// форумування локального часу
	//////////////////////////////////////////////////////////////////////////
	SYSTEMTIME sm;
	GetLocalTime(&sm);
	char date_str[64];
	unsigned int milisecont = sm.wMilliseconds;
	sprintf_s(date_str,time_format,sm.wYear,sm.wMonth,sm.wDay,sm.wHour,sm.wMinute,sm.wSecond);

	//////////////////////////////////////////////////////////////////////////
	// запис часу кінця сесії в базу
	//////////////////////////////////////////////////////////////////////////
	char str_end[256];
	sprintf_s(str_end, "UPDATE `diploma`.`hook_sesion` SET `SesionFinish`='%s' WHERE `HookSesionID`='%d'", date_str,sesion_code);
	if (mysql_query(con, str_end)) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}

	//////////////////////////////////////////////////////////////////////////
	// обнулення сесії
	//////////////////////////////////////////////////////////////////////////
	sesion_code = 1;

	//////////////////////////////////////////////////////////////////////////
	// заверешння роботи з базою
	//////////////////////////////////////////////////////////////////////////
	mysql_close(con);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Запис подій миші в базу
//////////////////////////////////////////////////////////////////////////
int logMouseActonToBase(int action_code, int mouse_code, POINT * mousePosition, HWND child )
{  
	//////////////////////////////////////////////////////////////////////////
	// підключення до бази
	//////////////////////////////////////////////////////////////////////////

	MYSQL *con = mysql_init(NULL);

	if (con == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		return(1);
	}

	if (mysql_real_connect(con, host, user_name, password, NULL, port, NULL, 0) == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}  

	//////////////////////////////////////////////////////////////////////////
	// форумування локального часу
	//////////////////////////////////////////////////////////////////////////
	SYSTEMTIME sm;
	GetLocalTime(&sm);
	char date_str[64];
	unsigned int milisecont = sm.wMilliseconds;
	sprintf_s(date_str,time_format,sm.wYear,sm.wMonth,sm.wDay,sm.wHour,sm.wMinute,sm.wSecond);

	//////////////////////////////////////////////////////////////////////////
	// запис часу події в базу
	//////////////////////////////////////////////////////////////////////////
	char type_action[4] = "MS";

	char str[256];
	sprintf_s(str, "INSERT INTO `diploma`.`actiontable` (`ActionTime`, `ActionTimeMS`, `ActionType`, `HookSesionID`) VALUES ('%s', '%03d', '%s','%d')", date_str, milisecont,type_action, sesion_code);
	if (mysql_query(con, str)) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}

	//////////////////////////////////////////////////////////////////////////
	// отримання індексу попередньо записаного елемента
	//////////////////////////////////////////////////////////////////////////
	char lastIndexAction[] = "SELECT last_insert_id()";
	mysql_query(con, lastIndexAction);
	MYSQL_RES * result_action = mysql_store_result(con);
	int index_action =  atoi(mysql_fetch_row(result_action)[0]);
	mysql_free_result(result_action);

	//fprintf(stdout, "CurIndex: %d;\n",index);

	//////////////////////////////////////////////////////////////////////////
	// перевірка, чи потрібне нове визначення
	//////////////////////////////////////////////////////////////////////////



	BOOL needNewRecord = FALSE;
	if(lastHWND==NULL || lastHWND!=child){
		needNewRecord = TRUE;
		lastHWND = child;
	}
	if(needNewRecord){
		//////////////////////////////////////////////////////////////////////////
		// перевірка, чи є обєкт в базі
		//////////////////////////////////////////////////////////////////////////
		char shablon[] = "SELECT gui_object_info.HWND,mouseaction.GUIObjectID  FROM diploma.gui_object_info left join diploma.mouseaction on gui_object_info.GUIObjectID=mouseaction.GUIObjectID left join diploma.actiontable on diploma.mouseaction.ActionID = diploma.actiontable.ActionID where actiontable.HookSesionID = '%d' AND HWND = '%d' GROUP BY HWND";
		char buffer[sizeof shablon + 24];
		sprintf_s(buffer,shablon,sesion_code,child);
		if (mysql_query(con, buffer)) 
		{
			fprintf(stderr, "%s\n", mysql_error(con));
			mysql_close(con);
			return(1);
		}
		MYSQL_RES * resultHWND = mysql_store_result(con);
		unsigned int count = (unsigned int)resultHWND->row_count;
		//char tmp_
		//sprintf_s()
		
		if (count!=0)index_gui =  atoi(mysql_fetch_row(resultHWND)[1]);
		mysql_free_result(resultHWND);
		if(count==0){
			//////////////////////////////////////////////////////////////////////////
			// визначення координат обєкту під мишею в батьківському вікні
			//////////////////////////////////////////////////////////////////////////
			HWND parent = GetParentHWND();
			RECT rc;
			GetWindowRect( child, &rc );
			convert(parent, rc.left, rc.top);
			convert(parent, rc.right, rc.bottom);

			//////////////////////////////////////////////////////////////////////////
			// запис GUI обєкта
			//////////////////////////////////////////////////////////////////////////
			RECT ts = rc;
			char guiInBase[256];
			sprintf_s(guiInBase, "INSERT INTO `diploma`.`gui_object_info` (`left`, `right`, `top`, `bottom`, `HWND`) VALUES ('%d', '%d', '%d', '%d', '%d')", ts.left, ts.right, ts.top, ts.bottom, child);
			if (mysql_query(con, guiInBase)) 
			{
				fprintf(stderr, "%s\n", mysql_error(con));
				mysql_close(con);
				return(1);
			}
			//////////////////////////////////////////////////////////////////////////
			// отримання індексу попередньо записаного GUI обєкта
			//////////////////////////////////////////////////////////////////////////
			char lastIndexGUI[] = "SELECT last_insert_id()";
			mysql_query(con, lastIndexGUI);
			MYSQL_RES * resultGUI = mysql_store_result(con);
			index_gui =  atoi(mysql_fetch_row(resultGUI)[0]);
			mysql_free_result(resultGUI);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// перевід координат миші до батьківського вікна
	//////////////////////////////////////////////////////////////////////////
	POINT  lm;
	lm.x = mousePosition->x;
	lm.y = mousePosition->y;
	ScreenToClient(GetParentHWND(), &lm);

	//////////////////////////////////////////////////////////////////////////
	// запис події миші в базу
	//////////////////////////////////////////////////////////////////////////
	char insertInMouse[256];
	sprintf_s(insertInMouse, "INSERT INTO `diploma`.`mouseaction` (`ActionCode`, `MouseCode`, `X`, `Y`, `ActionID`, `GUIObjectID`) VALUES ('%d', '%d', '%d', '%d', '%d', '%d')", action_code,  mouse_code, lm.x, lm.y, index_action, index_gui);
	if (mysql_query(con, insertInMouse)) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}

	//////////////////////////////////////////////////////////////////////////
	// заверешння роботи з базою
	//////////////////////////////////////////////////////////////////////////
	mysql_close(con);
	return(0);
}



//////////////////////////////////////////////////////////////////////////
// Запис положення вікна
//////////////////////////////////////////////////////////////////////////
int logWindowRectToBase( RECT * rect ){  
	//////////////////////////////////////////////////////////////////////////
	// підключення до бази
	//////////////////////////////////////////////////////////////////////////

	MYSQL *con = mysql_init(NULL);

	if (con == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		return(1);
	}

	if (mysql_real_connect(con, host, user_name, password, NULL, port, NULL, 0) == NULL) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}  

	//////////////////////////////////////////////////////////////////////////
	// форумування локального часу
	//////////////////////////////////////////////////////////////////////////
	SYSTEMTIME sm;
	GetLocalTime(&sm);
	char date_str[64];
	unsigned int milisecont = sm.wMilliseconds;
	sprintf_s(date_str,time_format,sm.wYear,sm.wMonth,sm.wDay,sm.wHour,sm.wMinute,sm.wSecond);

	//////////////////////////////////////////////////////////////////////////
	// запис часу події в базу
	//////////////////////////////////////////////////////////////////////////
	char type_action[4] = "MS";

	char str[256];
	sprintf_s(str, "INSERT INTO `diploma`.`actiontable` (`ActionTime`, `ActionTimeMS`, `ActionType`, `HookSesionID`) VALUES ('%s', '%03d', '%s','%d')", date_str, milisecont,type_action, sesion_code);
	if (mysql_query(con, str)) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}

	//////////////////////////////////////////////////////////////////////////
	// отримання індексу попередньо записаного елемента
	//////////////////////////////////////////////////////////////////////////
	char lastIndex[] = "SELECT last_insert_id()";
	mysql_query(con, lastIndex);
	MYSQL_RES * result = mysql_store_result(con);
	int index =  atoi(mysql_fetch_row(result)[0]);
	mysql_free_result(result);

	//fprintf(stdout, "CurIndex: %d;\n",index);

	//////////////////////////////////////////////////////////////////////////
	// запис події миші в базу
	//////////////////////////////////////////////////////////////////////////

	char insertInWindow[256];
	sprintf_s(insertInWindow, "INSERT INTO `diploma`.`window_position` (`left`, `right`, `top`, `bottom`, `ActionID`) VALUES ('%d', '%d', '%d', '%d', '%d')", rect->left,  rect->right, rect->top, rect->bottom, index);
	if (mysql_query(con, insertInWindow)) 
	{
		fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		return(1);
	}

	//////////////////////////////////////////////////////////////////////////
	// заверешння роботи з базою
	//////////////////////////////////////////////////////////////////////////
	mysql_close(con);
	return(0);
}