
#include "stdafx.h"

#include "my_global.h"
#include "mysql.h"

#include "..\WindowShell\PaintModule.h"

#pragma comment (lib,"C:\\Program Files \(x86\)\\MySQL\\MySQL Connector.C 6.1\\lib\\libmysql.lib")
int sesion_code = 25;

char table_select_mouse_action_pattern[] = "SELECT MouseCode, X, Y, ActionTime, ActionTimeMS, GUIObjectID FROM diploma.mouseaction LEFT JOIN diploma.actiontable ON mouseaction.ActionID=actiontable.ActionID WHERE actiontable.HookSesionID = %d;";


int render(){
	
	//init_my_graphics(NULL);
	//POINT p;
	//p.x = 17; 
	//p.y = 454;
	//move_to_start(&p);


	//////////////////////////////////////////////////////////////////////////
	// підключення до бази
	//////////////////////////////////////////////////////////////////////////

	MYSQL *con = mysql_init(NULL);

	if (con == NULL) 
	{
		//fprintf(stderr, "%s\n", mysql_error(con));
		exit(1);
	}

	if (mysql_real_connect(con, "localhost", "root", "DalayLayma55", 
		NULL, 13306, NULL, 0) == NULL) 
	{
		////fprintf(stderr, "%s\n", mysql_error(con));
		mysql_close(con);
		exit(1);
	}  
	



	//////////////////////////////////////////////////////////////////////////
	// створити тимчасову таблицю
	//////////////////////////////////////////////////////////////////////////
	unsigned int tmp_table_index = 22;
	{
		char tmp_table_drop_pattern[] = "DROP TABLE `diploma`.`gui_time_tmp_%d`";
		char tmp_table_drop_buffer[128];
		sprintf_s(tmp_table_drop_buffer, tmp_table_drop_pattern , tmp_table_index);
		mysql_query(con, tmp_table_drop_buffer);
		// зробити таблицю тимчасовою
		char tmp_table_create_pattern[] = "CREATE TABLE `diploma`.`gui_time_tmp_%d` (`GUIObjectID` INT(11) UNSIGNED NOT NULL, `TimeDifferenceMS` INT(11) NOT NULL, `ClickCount` INT(11) UNSIGNED NOT NULL DEFAULT 0, PRIMARY KEY (`GUIObjectID`), UNIQUE INDEX `GUIObjectID_UNIQUE` (`GUIObjectID` ASC));";
		char tmp_table_create_buf[256];
		sprintf_s(tmp_table_create_buf, tmp_table_create_pattern , tmp_table_index);
		if (mysql_query(con, tmp_table_create_buf)) 
		{
			////fprintf(stderr, "%s\n", mysql_error(con));
			mysql_close(con);
			exit(1);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// вираховуємо активність
	//////////////////////////////////////////////////////////////////////////
	{
		char time_format[] = "%d-%d-%d %d:%d:%d";
		char table_select_gui_activity_buffer[256];
		sprintf_s(table_select_gui_activity_buffer, table_select_mouse_action_pattern , sesion_code);
		//здійснення запиту
		if (mysql_query(con, table_select_gui_activity_buffer)) 
		{
			//fprintf(stderr, "%s\n", mysql_error(con));
			mysql_close(con);
			exit(1);
		}
		// витягаємо усі стрічки
		MYSQL_RES * result_activity = mysql_store_result(con);
		unsigned int count = mysql_num_rows(result_activity);
		//printf("COUNT_LINE=%d\n",count);
		//set_graphics_to_draw_line();


		SYSTEMTIME prev_time;
		BOOL isEmPrevTime = FALSE;

		for(unsigned int index_row=0;index_row<count;index_row++){
			int milisecond, second, minute, hour, day, month, year;
			int diff_ms;


			MYSQL_ROW res_row = mysql_fetch_row(result_activity);
			//POINT tmp;
			char *date_str;
			int gui_id	 = atoi(res_row[5]);
			int code_mouse = atoi(res_row[0]);
			//tmp.x = atoi(res_row[1]);
			//tmp.y = atoi(res_row[2]);
			date_str = res_row[3];
			SYSTEMTIME sm = SYSTEMTIME();
			sscanf_s(date_str,time_format,&sm.wYear,&sm.wMonth,&sm.wDay,&sm.wHour,&sm.wMinute,&sm.wSecond);
			sm.wMilliseconds = atoi(res_row[4]);
			if(!isEmPrevTime){
				isEmPrevTime = TRUE;
				diff_ms = 0;
				prev_time = sm;
			}else{
				milisecond	= sm.wMilliseconds	- prev_time.wMilliseconds;
				second		= sm.wSecond		- prev_time.wSecond;
				minute		= sm.wMinute		- prev_time.wMinute;
				hour			= sm.wHour			- prev_time.wHour;
				diff_ms		= milisecond + 1000 * second + 60 * 1000 * minute + 60 * 60 * 1000 * hour;
			}
			// вибір з таблиці
			char tmp_table_select_pattern[]	= "SELECT TimeDifferenceMS, ClickCount FROM diploma.gui_time_tmp_%d where GUIObjectID = %d";
			char tmp_table_select_buffer[128];
			sprintf_s(tmp_table_select_buffer,tmp_table_select_pattern, tmp_table_index, gui_id);



			if (mysql_query(con, tmp_table_select_buffer)) 
			{
				//fprintf(stderr, "%s\n", mysql_error(con));
				mysql_close(con);
				exit(1);
			}
			MYSQL_RES * tmp_result_line = mysql_store_result(con);
			unsigned int tmp_count = mysql_num_rows(tmp_result_line);

			char tmp_inject_new_ms_buffer[128];

			char tmp_table_insert_pattern[] = "INSERT INTO `diploma`.`gui_time_tmp_%d` (`TimeDifferenceMS`, `GUIObjectID` ) VALUES ('%d', '%d')";
			char tmp_table_update_pattern[] = "UPDATE `diploma`.`gui_time_tmp_%d` SET `TimeDifferenceMS`='%d' WHERE `GUIObjectID`='%d';";
			unsigned int click_count = 0;
			if(tmp_count>0){
				MYSQL_ROW tmp_res_row = mysql_fetch_row(tmp_result_line);

				int diff_ms_sumarry = atoi(tmp_res_row[0]) + diff_ms;
				click_count	= atoi(tmp_res_row[1]);
				sprintf(tmp_inject_new_ms_buffer,tmp_table_update_pattern,tmp_table_index,diff_ms_sumarry,gui_id);
			}else{
				sprintf(tmp_inject_new_ms_buffer,tmp_table_insert_pattern,tmp_table_index,diff_ms,gui_id);
			}
			// виконання запису
			mysql_free_result(tmp_result_line);
			if (mysql_query(con, tmp_inject_new_ms_buffer)) 
			{
				//fprintf(stderr, "%s\n", mysql_error(con));
				mysql_close(con);
				exit(1);
			}


			switch(code_mouse){
			case WM_MOUSEMOVE:		break;
			case WM_LBUTTONDOWN:		
			case WM_LBUTTONUP:			
				{
					click_count++;
					char tmp_table_insert_click_pattern[]	= "UPDATE `diploma`.`gui_time_tmp_%d` SET `ClickCount`='%d' WHERE `GUIObjectID`='%d'";
					char tmp_table_insert_click_buffer[128];
					sprintf_s(tmp_table_insert_click_buffer,tmp_table_insert_click_pattern, tmp_table_index ,click_count, gui_id);
					if (mysql_query(con, tmp_table_insert_click_buffer)) 
					{

						//fprintf(stderr, "%s\n", mysql_error(con));
						mysql_close(con);

						//scanf("%d",&i);
						exit(1);
					}



				}
				break;
			default:					break;
			}

		}

		mysql_free_result(result_activity);
	}


	//////////////////////////////////////////////////////////////////////////
	// відображення обєктів на екрані
	//////////////////////////////////////////////////////////////////////////
	{

		// отримання максимальних значень полів
		char tmp_table_max_count_pattern[]		= "SELECT MAX(ClickCount) AS ClickCount FROM diploma.gui_time_tmp_%d";
		char tmp_table_max_time_ms_pattern[]	= "SELECT MAX(TimeDifferenceMS) AS TimeDifferenceMS FROM diploma.gui_time_tmp_%d";

		char tmp_table_max_count_buffer[128];	
		char tmp_table_max_time_ms_buffer[128];
		
		int max_click_count, max_time_ms;

		sprintf_s(tmp_table_max_count_buffer,  tmp_table_max_count_pattern,  tmp_table_index);
		sprintf_s(tmp_table_max_time_ms_buffer,tmp_table_max_time_ms_pattern,tmp_table_index);

		//кількість кліків
		if (!mysql_query(con, tmp_table_max_count_buffer)) 
		{
			MYSQL_RES * result_local = mysql_store_result(con);
			unsigned int count_local = mysql_num_rows(result_local);
			if(count_local == 0)max_click_count = 0;
			else max_click_count = atoi(mysql_fetch_row(result_local)[0]);
			mysql_free_result(result_local);

		}else{
			max_click_count = 0;
			//fprintf(stderr, "%s\n", mysql_error(con));
		}
		//максимальний  час
		if (!mysql_query(con, tmp_table_max_time_ms_buffer)) 
		{
			MYSQL_RES * result_local = mysql_store_result(con);
			unsigned int count_local = mysql_num_rows(result_local);
			if(count_local == 0)max_time_ms = 0;
			else max_time_ms = atoi(mysql_fetch_row(result_local)[0]);
			mysql_free_result(result_local);

		}else{
			max_time_ms = 0;
			//fprintf(stderr, "%s\n", mysql_error(con));
		}


		char table_select_gui_pattern[512];
		{
			char pattern_obj[] =					"SELECT gui_object_info.left,gui_object_info.right, gui_object_info.top, gui_object_info.bottom ,gui_object_info.HWND,mouseaction.GUIObjectID  FROM diploma.gui_object_info left join diploma.mouseaction on gui_object_info.GUIObjectID=mouseaction.GUIObjectID left join diploma.actiontable on diploma.mouseaction.ActionID = diploma.actiontable.ActionID where actiontable.HookSesionID = '%d' GROUP BY HWND;";
			char table_select_gui_pattern_pre[] =	"SELECT gui_object_info.left,gui_object_info.right, gui_object_info.top, gui_object_info.bottom, HWND, gui_object_info.GUIObjectID, TimeDifferenceMS, ClickCount FROM diploma.gui_time_tmp_%d left join diploma.gui_object_info on gui_time_tmp_%d.GUIObjectID=gui_object_info.GUIObjectID";
			sprintf_s(table_select_gui_pattern,table_select_gui_pattern_pre,tmp_table_index,tmp_table_index);
		}

		char str2[512];
		sprintf(str2, table_select_gui_pattern , sesion_code);
		if (mysql_query(con, str2)) 
		{
			//fprintf(stderr, "%s\n", mysql_error(con));
			mysql_close(con);
			exit(1);
		}
		MYSQL_RES * result_object = mysql_store_result(con);
		unsigned int count_object = mysql_num_rows(result_object);
		//printf("COUNT_object=%d\n",count_object);

		set_graphics_to_draw_object();

		for(unsigned int index_row=0;index_row<count_object;index_row++){
			MYSQL_ROW res_row = mysql_fetch_row(result_object);
			RECT tmpRect;
			tmpRect.left = atoi(res_row[0]);
			tmpRect.right = atoi(res_row[1]);
			tmpRect.top = atoi(res_row[2]);
			tmpRect.bottom = atoi(res_row[3]);

			int click_count = atoi(res_row[7]);
			int time_ms = atoi(res_row[6]);

			double activity_level = (((double)click_count)/((double)max_click_count)) * 0.25 + (((double)time_ms)/((double)max_time_ms)) * 0.75;

			draw_rect(&tmpRect, activity_level);
		}
		mysql_free_result(result_object);
	}



	//////////////////////////////////////////////////////////////////////////
	// відображення миші на екрані
	//////////////////////////////////////////////////////////////////////////
	{

		char table_select_mouse_action_buffer[256];
		sprintf_s(table_select_mouse_action_buffer, table_select_mouse_action_pattern , sesion_code);
		if (mysql_query(con, table_select_mouse_action_buffer)) 
		{
			//fprintf(stderr, "%s\n", mysql_error(con));
			mysql_close(con);
			exit(1);
		}
		MYSQL_RES * result_line = mysql_store_result(con);
		unsigned int count = mysql_num_rows(result_line);
		//printf("COUNT_LINE=%d\n",count);

		set_graphics_to_draw_line();


		

		for(unsigned int index_row=0;index_row<count;index_row++){

			MYSQL_ROW res_row = mysql_fetch_row(result_line);
			POINT tmp;

			//int gui_id	 = atoi(res_row[5]);
			int code_mouse = atoi(res_row[0]);
			tmp.x = atoi(res_row[1]);
			tmp.y = atoi(res_row[2]);

			switch(code_mouse){
			case WM_MOUSEMOVE:		draw_lint_to(&tmp);break;
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:		draw_point(&tmp);break;
			default:				draw_lint_to(&tmp);
			}

		}

		mysql_free_result(result_line);
	}

	//////////////////////////////////////////////////////////////////////////
	// отримання індексу попередньо записаного елемента
	//////////////////////////////////////////////////////////////////////////



	//fprintf(stdout, "CurIndex: %d;\n",index);

	//////////////////////////////////////////////////////////////////////////
	// запис події миші в базу
	//////////////////////////////////////////////////////////////////////////
	/* int activation_code = 0, mouse_code = 160;
	int x = 240, y = 250;
	char insertInMouse[256];
	sprintf(insertInMouse, "INSERT INTO `diploma`.`mouseaction` (`ActionCode`, `MouseCode`, `X`, `Y`, `ActionID`) VALUES ('%d', '%d', '%d', '%d', '%d')", activation_code,  mouse_code, x, y, index);
	if (mysql_query(con, insertInMouse)) 
	{
	//fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);
	exit(1);
	}

	*/
	mysql_close(con);

	//exit(0);
	return 0;


}