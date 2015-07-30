
#include "stdafx.h"

#include <windows.h>

#include "PaintModule.h"
#include "HSVvsRGB.h"

HDC hdc;
POINT sc;

BOOL start_print;


HPEN penToMouseLine;


void init_my_graphics(HDC my_hdc){
	//HWND hwnd;   
	//hwnd = GetDesktopWindow();
	hdc = my_hdc;
}
void set_graphics_to_draw_line(){
	penToMouseLine = CreatePen(PS_SOLID, 4, RGB(255, 0, 0));
	SelectObject(hdc,penToMouseLine);
}
void set_graphics_to_draw_object(){
	DeleteObject(penToMouseLine);
}

void move_to_start(POINT *p){

	sc.x = p->x;
	sc.y = p->y;

	start_print = TRUE;

}
void draw_lint_to(POINT *p){
	int x,y;
	x = p->x + sc.x;
	y = p->y + sc.y;
	if(start_print){
		MoveToEx(hdc, x, y, NULL); 
		start_print = FALSE;
	} else LineTo(hdc, x, y);
	//printf("LineTo: {%d, %d};\n", x, y);
}
void draw_point(POINT *p){
	int radius = 8;

	int x1,x2,y1,y2;
	x1 = p->x + sc.x - radius;
	y1 = p->y + sc.y - radius;
	x2 = p->x + sc.x + radius;
	y2 = p->y + sc.y + radius;
	Ellipse(hdc, x1, y1, x2, y2); 
}




void draw_rect(RECT * r, double activity_level){
	int left, top, right, bottom;
	/////
	double dt = 40.;
	hsv in_data;
	in_data.s = 1.;
	in_data.v = 1.;
	in_data.h = ((240. + dt) * (1. - activity_level)) - dt;
	rgb hr = hsv2rgb(in_data);

	HBRUSH hBrush;						//создаём объект-кисть
	hBrush = CreateSolidBrush(RGB(hr.r*255,hr.g*255,hr.b*255));		//задаём сплошную кисть, закрашенную цветом RGB
	SelectObject(hdc, hBrush);			//делаем кисть активной


	HPEN hPen = CreatePen(PS_NULL, 2, RGB(255, 255, 255));
	SelectObject(hdc,hPen);


	left	= r->left + sc.x;
	right	= r->right + sc.x;
	top		= r->top	+ sc.y;
	bottom	= r->bottom	+ sc.y;

	Rectangle(hdc, left, top, right, bottom);
}