// Ramon Moreno
// Modified: 10/8/24
// Feature for game
#include "feature.h"
#include <GL/glx.h>
#include <X11/keysym.h>
#include <cstring>
#include <cmath>
#include <chrono>

extern class Global {
    public:
        int xres, yres;
} g;

bool title_screen = true;
static auto start_time = std::chrono::steady_clock::now();

// Animation state
float pulse_scale = 1.0f;
float title_y_offset = 0.0f;

void drawText(int x, int y, const char *text, bool large = false)
{
    static int initialized = 0;
    static GLuint fontOffset;
    static GLuint largeFontOffset;
    
    if (!initialized) {
        initialized = 1;
        Display *dpy = glXGetCurrentDisplay();
        // Load two different font sizes
        Font font = XLoadFont(dpy, "fixed");
        Font largeFont = XLoadFont(dpy, "fixed");
        fontOffset = glGenLists(256);
        largeFontOffset = glGenLists(256);
        glXUseXFont(font, 0, 256, fontOffset);
        glXUseXFont(largeFont, 0, 256, largeFontOffset);
    }
    
    glPushAttrib(GL_LIST_BIT);
    glListBase(large ? largeFontOffset : fontOffset);
    glRasterPos2i(x, y);
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, (GLubyte *)text);
    glPopAttrib();
}

void drawBox(int x, int y, int width, int height, bool filled = false)
{
    if (filled) {
        glBegin(GL_QUADS);
    } else {
        glBegin(GL_LINE_LOOP);
    }
    glVertex2i(x, y);
    glVertex2i(x + width, y);
    glVertex2i(x + width, y + height);
    glVertex2i(x, y + height);
    glEnd();
}

void render_title_screen()
{
    // Update animation timers
    auto current_time = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        current_time - start_time).count() / 1000.0f;
    
    // Calculate animations
    pulse_scale = 1.0f + 0.1f * sin(elapsed * 2.0f);
    title_y_offset = 5.0f * sin(elapsed * 1.5f);
    
    // Clear screen and disable textures temporarily
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_TEXTURE_2D);
    
    // Draw background gradient
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.1f, 0.3f);
    glVertex2i(0, 0);
    glVertex2i(g.xres, 0);
    glColor3f(0.1f, 0.1f, 0.2f);
    glVertex2i(g.xres, g.yres);
    glVertex2i(0, g.yres);
    glEnd();
    
    // Draw animated background lines
    glColor3f(0.3f, 0.3f, 0.4f);
    glLineWidth(2.0f);
    for (int i = 0; i < 10; i++) {
        float y = g.yres * (i / 10.0f);
        float offset = 50.0f * sin(elapsed + i * 0.5f);
        glBegin(GL_LINES);
        glVertex2f(0, y);
        glVertex2f(g.xres, y + offset);
        glEnd();
    }
    
    // Title text setup
    const char* title = "GORILLA DASH";
    int char_width = 12;  // Approximate width for large font
    int title_width = strlen(title) * char_width;
    int title_x = (g.xres - title_width) / 2;
    int title_y = (g.yres / 2) + 50 + title_y_offset;
    
    // Draw title shadow
    glColor3f(0.2f, 0.2f, 0.4f);
    drawText(title_x + 3, title_y - 3, title, true);
    
    // Draw main title
    glColor3f(1.0f, 0.9f, 0.2f);
    drawText(title_x, title_y, title, true);
    
    // Draw decorative box around title
    int box_padding = 20;
    glColor3f(0.5f, 0.5f, 0.8f);
    glLineWidth(2.0f);
    drawBox(title_x - box_padding, 
           title_y - box_padding, 
           title_width + box_padding * 2, 
           40 + box_padding * 2);
    
    // Draw pulsing prompt text
    const char* prompt = "Press SPACE to Start";
    int prompt_width = strlen(prompt) * 9;  // Approximate width for normal font
    int prompt_x = (g.xres - prompt_width) / 2;
    int base_prompt_y = (g.yres / 2) - 30;
    
    // Calculate pulsing offset
    float pulse_offset = 5.0f * sin(elapsed * 4.0f);
    glColor3f(0.8f, 0.8f, 1.0f);
    drawText(prompt_x, base_prompt_y + pulse_offset, prompt);
    
    // Draw version number
    glColor3f(0.5f, 0.5f, 0.5f);
    drawText(10, 10, "v1.0");
    
    // Draw decorative corners
    glColor3f(0.5f, 0.5f, 0.8f);
    int corner_size = 20;
    glLineWidth(2.0f);
    
    // Top-left corner
    glBegin(GL_LINES);
    glVertex2i(10, g.yres - 10);
    glVertex2i(10, g.yres - corner_size - 10);
    glVertex2i(10, g.yres - 10);
    glVertex2i(corner_size + 10, g.yres - 10);
    
    // Top-right corner
    glVertex2i(g.xres - 10, g.yres - 10);
    glVertex2i(g.xres - corner_size - 10, g.yres - 10);
    glVertex2i(g.xres - 10, g.yres - 10);
    glVertex2i(g.xres - 10, g.yres - corner_size - 10);
    
    // Bottom-left corner
    glVertex2i(10, 10);
    glVertex2i(10, corner_size + 10);
    glVertex2i(10, 10);
    glVertex2i(corner_size + 10, 10);
    
    // Bottom-right corner
    glVertex2i(g.xres - 10, 10);
    glVertex2i(g.xres - corner_size - 10, 10);
    glVertex2i(g.xres - 10, 10);
    glVertex2i(g.xres - 10, corner_size + 10);
    glEnd();
    
    // Reset line width
    glLineWidth(1.0f);
    
    // Re-enable textures for the rest of the game
    glEnable(GL_TEXTURE_2D);
}

void check_title_keys(XEvent *e) {
    if (title_screen && e->type == KeyPress) {
        int key = XLookupKeysym(&e->xkey, 0);
        if (key == XK_space) {
            title_screen = false;
        }
    }
}

