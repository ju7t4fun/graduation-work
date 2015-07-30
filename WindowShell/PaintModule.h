

extern void init_my_graphics(HDC hdc);
extern void move_to_start(POINT *p);
extern void draw_lint_to(POINT *p);
extern void draw_rect(RECT * r, double activity_level = 0.5);
extern void draw_point(POINT *p);

extern void set_graphics_to_draw_line();
extern void set_graphics_to_draw_object();