// Ramon Moreno
// Modified: 10/8/24
// Feature for game
#include "feature.h"
#include <GL/glx.h>
#include <X11/keysym.h>
#include <cstring>

// Access the Global class 
extern class Global {
    public:
        int xres, yres;
} g;

bool title_screen = true;

void drawText(int x, int y, const char *text)
{
    static int initialized = 0;
    static GLuint fontOffset;

    if (!initialized) {
        initialized = 1;
        Display *dpy = glXGetCurrentDisplay();
        Font font = XLoadFont(dpy, "fixed");
        fontOffset = glGenLists(256);
        glXUseXFont(font, 0, 256, fontOffset);
    }

    glRasterPos2i(x, y);

    glPushAttrib(GL_LIST_BIT);
    glListBase(fontOffset);
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, (GLubyte *)text);
    glPopAttrib();
}

void render_title_screen()
{
    // Clear the screen with a black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set text color to white
    glColor3f(1.0, 1.0, 1.0);

    // Calculate positions for centered text
    int title_x = g.xres / 2 - 50;
    int title_y = g.yres / 2 + 20;
    int prompt_x = g.xres / 2 - 70;
    int prompt_y = g.yres / 2 - 20;

    // Render the title and prompt text
    drawText(title_x, title_y, "Gorilla Dash");
    drawText(prompt_x, prompt_y, "Press space to start");
}
void check_title_keys(XEvent *e) {
    if (title_screen && e->type == KeyPress) {
        int key = XLookupKeysym(&e->xkey, 0);
        if (key == XK_space) {
            title_screen = false;
        }
    }
}
