#ifndef FEATURE_H
#define FEATURE_H

#include <X11/Xlib.h>

// title screen
extern bool title_screen;

extern bool paused;

void render_title_screen();
void check_title_keys(XEvent *e);

void take_damage(int damage);
void heal(int amount);

extern int current_health;
extern int max_health;

#endif
