#ifndef FEATURE_H
#define FEATURE_H

#include <X11/Xlib.h>

// title screen
extern bool title_screen;

void render_title_screen();
void check_title_keys(XEvent *e);
#endif
