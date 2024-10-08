// Ramon Moreno
// Add date modified/last time it was modified
// what it is for 

#include "fonts.h"

int show_x = 0;

void show_my_feature(int x, int y)
{
    // draw a rectangle
    // show some text
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 0;
    ggprint8b(&r, 16, 0x0000ff00, "hello");
}
