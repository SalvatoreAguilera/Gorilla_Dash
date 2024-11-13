// Ramon Moreno
#include "feature.h"
#include <GL/glx.h>
#include <X11/keysym.h>
#include <cstring>
#include <cmath>
#include <chrono>
#include <cstdio>

extern class Global {
    public:
        int xres, yres;
} g;

bool title_screen = true;
static auto start_time = std::chrono::steady_clock::now();

float pulse_scale = 1.0f;
float title_y_offset = 0.0f;

int current_health = 75;
int max_health = 100;

void drawText(int x, int y, const char *text, bool large = false)
{
    static int initialized = 0;
    static GLuint fontOffset;
    static GLuint largeFontOffset;

    if (!initialized) {
        initialized = 1;
        Display *dpy = glXGetCurrentDisplay();
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

    
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_TEXTURE_2D);

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
    int char_width = 12;
    int title_width = strlen(title) * char_width;
    int title_x = (g.xres - title_width) / 2;
    int title_y = (g.yres / 2) + 50 + title_y_offset;

   
    glColor3f(0.2f, 0.2f, 0.4f);
    drawText(title_x + 3, title_y - 3, title, true);

   
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
    int prompt_width = strlen(prompt) * 9;
    int prompt_x = (g.xres - prompt_width) / 2;
    int base_prompt_y = (g.yres / 2) - 30;

    
    float pulse_offset = 5.0f * sin(elapsed * 4.0f);
    glColor3f(0.8f, 0.8f, 1.0f);
    drawText(prompt_x, base_prompt_y + pulse_offset, prompt);

    // Version 1 (just for fun)
    glColor3f(0.5f, 0.5f, 0.5f);
    drawText(10, 10, "v1.0");

    
    glColor3f(0.5f, 0.5f, 0.8f);
    int corner_size = 20;
    glLineWidth(2.0f);

    
    glBegin(GL_LINES);
    glVertex2i(10, g.yres - 10);
    glVertex2i(10, g.yres - corner_size - 10);
    glVertex2i(10, g.yres - 10);
    glVertex2i(corner_size + 10, g.yres - 10);

   
    glVertex2i(g.xres - 10, g.yres - 10);
    glVertex2i(g.xres - corner_size - 10, g.yres - 10);
    glVertex2i(g.xres - 10, g.yres - 10);
    glVertex2i(g.xres - 10, g.yres - corner_size - 10);

    
    glVertex2i(10, 10);
    glVertex2i(10, corner_size + 10);
    glVertex2i(10, 10);
    glVertex2i(corner_size + 10, 10);

    
    glVertex2i(g.xres - 10, 10);
    glVertex2i(g.xres - corner_size - 10, 10);
    glVertex2i(g.xres - 10, 10);
    glVertex2i(g.xres - 10, corner_size + 10);
    glEnd();

    
    glLineWidth(1.0f);


    glEnable(GL_TEXTURE_2D);
}

void render_health_bar()
{
    int x = 10;
    int y = g.yres - 40; 

    // Health bar settings
    const int bar_width = 300;
    const int bar_height = 25;
    const int num_segments = 50; 

    int filled_segments = (current_health * num_segments) / max_health;
    int empty_segments = num_segments - filled_segments;

    char health_bar_text[400];
    strcpy(health_bar_text, "Health Bar: [");
    for(int i = 0; i < filled_segments; i++) strcat(health_bar_text, "|");
    for(int i = 0; i < empty_segments; i++) strcat(health_bar_text, " ");
    strcat(health_bar_text, "] ");
    sprintf(health_bar_text + strlen(health_bar_text), "%d/%d", current_health, max_health);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(x, y + bar_height + 10, health_bar_text);

    glColor3f(0.5f, 0.0f, 0.0f); 
    drawBox(x, y, bar_width, bar_height, true);

    glColor3f(0.8f, 0.0f, 0.0f); 
    drawBox(x, y, (bar_width * filled_segments) / num_segments, bar_height, true);

    glColor3f(1.0f, 1.0f, 1.0f); 
    drawBox(x, y, bar_width, bar_height, false);
}

void take_damage(int damage) {
    current_health -= damage;
    if (current_health < 0)
        current_health = 0;
}

void heal(int amount) {
    current_health += amount;
    if (current_health > max_health)
        current_health = max_health;
}


void check_title_keys(XEvent *e) {
    if (title_screen && e->type == KeyPress) {
        int key = XLookupKeysym(&e->xkey, 0);
        if (key == XK_space) {
            title_screen = false;
        }
    }
}
