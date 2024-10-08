// Ramon Moreno
// Modified: 10/8/24
// Feature for game
#include <GL/freeglut.h>
#include <string>

class Global {
public:
    // make a title screen
    bool show_title_screen;
    std::string game_name;
    std::string start_message;
    Global() {
        show_title_screen = true;
        game_name = "Jetpack Adventure";
        start_message = "Press Space to Start";
    }
} g;

void render_title_screen();

if (g.show_title_screen) {
            render_title_screen();
        }
if (g.show_title_screen && key == XK_space) {
            g.show_title_screen = false;
        }
void render_title_screen()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set text color to white
    glColor3f(1.0f, 1.0f, 1.0f);

    // Render game name
    glRasterPos2f(g.xres / 2 - 100, g.yres / 2 + 50);
    for (char c : g.game_name) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }

    // Render start message
    glRasterPos2f(g.xres / 2 - 80, g.yres / 2 - 50);
    for (char c : g.start_message) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}
glutInit(&argc, argv);
