#ifndef CHANGES_H
#define CHANGES_H

#include <string>

// title screen
extern bool show_title_screen;
extern std::string game_name;
extern std::string start_message;

void init_title_screen();
void render_title_screen();
bool handle_title_screen_input(int key);

#endif 
