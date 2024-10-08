// Ramon Moreno
// Modified: 10/8/24
// Feature for game
#include "changes.h"
#include <GL/glut.h>
#include <GL/freeglut.h>

// title screen variables
bool show_title_screen = true;
std::string game_name = "Gorilla Dash";
std::string start_message = "Press Space to Start";

// Implement the new functions
void init_title_screen() {
    // To be added
}

void render_title_screen() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set text color to white
    glColor3f(1.0f, 1.0f, 1.0f);

    // Render game name
    glRasterPos2f(640 / 2 - 100, 480 / 2 + 50);  // Assuming 640x480 resolution
    for (char c : game_name) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    // Render start message
    glRasterPos2f(640 / 2 - 80, 480 / 2 - 50);
    for (char c : start_message) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

bool handle_title_screen_input(int key) {
    if (show_title_screen && key == ' ') {
        show_title_screen = false;
        return true;
    }
    return false;
}
