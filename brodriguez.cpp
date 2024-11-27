// Bryan Rodriguez
//
#include "platforms.h"
#include <iostream>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <cstring>
#include "Sprite.hpp"

using namespace std;
//extern class Image;
//const int MAX_PARTICLES = 1000;
/*
class Image {
public:
    int width, height;
    unsigned char *data;
    ~Image() { delete [] data; }
    Image(const char *fname) {
        if (fname[0] == '\0')
            return;
        char name[40];
        strcpy(name, fname);
        int slen = strlen(name);
        name[slen-4] = '\0';
        char ppmname[80];
        sprintf(ppmname, "%s.ppm", name);
        char ts[100];
        sprintf(ts, "convert %s %s", fname, ppmname);
        system(ts);
        FILE *fpi = fopen(ppmname, "r");
        if (fpi) {
            char line[200];
            fgets(line, 200, fpi);
            fgets(line, 200, fpi);
            while (line[0] == '#' || strlen(line) < 2)
                fgets(line, 200, fpi);
            sscanf(line, "%i %i", &width, &height);
            fgets(line, 200, fpi);
            int n = width * height * 3;
            data = new unsigned char[n];
            for (int i = 0; i < n; i++)
                data[i] = fgetc(fpi);
            fclose(fpi);

            printf("Image loaded with width: %d, height: %d\n", width, height);

        } else {
            cout << "ERROR opening image: " << ppmname << endl;
            exit(0);
        }
        unlink(ppmname);
    }
};
*/
// Texture class to hold texture details
/*class Platform_Texture {
public:
    Platform_Texture* platformTexture;
    Image *image;
    GLuint textureID;
    float xc[2] = {0.0f, 1.0f};  // Horizontal coordinate range
    float yc[2] = {0.0f, 1.0f};  // Vertical coordinate range

    Platform_Texture(const char *imagePath) {
        platformTexture = new Platform_Texture("grass_block.jpg");
        image = new Image(imagePath);
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, image->data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    ~Platform_Texture() { 
        delete platformTexture;
        delete image; 
    }
};*/

extern class Global {
public:
    int xres, yres;
} g;

class Box {
public:
    float pos[6]; // Adjusted to hold positions for multiple platforms
    float vel[2];
    int w, h;
    Box() {
        w = 175;
        h = 20;

        // Set positions for different platforms
        pos[0] = g.xres / 0.78; // high platform
        pos[1] = g.yres / 1.6;
        
        pos[2] = g.xres / 1.3; // low platform
        pos[3] = g.yres / 3;

        pos[4] = g.xres / 2.15; // ground platform
        pos[5] = g.yres / 30;
        
        vel[0] = -1.0f; // velocity for moving platforms
        vel[1] = 0.0f;
    }
} box;

/*
class X11_wrapper {
private:
    Display *dpy;
    Window win;
    GLXContext glc;
public:
    ~X11_wrapper();
    X11_wrapper();
    void set_title();
    bool getXPending();
    XEvent getXNextEvent();
    void swapBuffers();
    void reshape_window(int width, int height);
    void check_resize(XEvent *e);
    int check_keys(XEvent *e);
} x11;
*/
// Function prototypes
//void init_opengl(void);
//void render(void);
/*
int main()
{
    init_opengl();

    int done = 0;
    // Main game loop
    while (!done) {
        // Look for external events such as keyboard
        while (x11.getXPending()) {
            XEvent e = x11.getXNextEvent();
            x11.check_resize(&e);
            done = x11.check_keys(&e);
        }
        render();
        x11.swapBuffers();
        usleep(20000); // Sleep for a while
    }
    return 0;
}
*/
/*
Global::Global()
{
    xres = 800;
    yres = 600;
}

X11_wrapper::~X11_wrapper()
{
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}
*/
/*X11_wrapper::X11_wrapper()
{
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w = g.xres, h = g.yres;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        cout << "\n\tcannot connect to X server\n" << endl;
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
        cout << "\n\tno appropriate visual found\n" << endl;
        exit(EXIT_FAILURE);
    } 
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask =
        ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPress | ButtonReleaseMask |
        PointerMotionMask |
        StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
                        InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void X11_wrapper::set_title()
{
    // Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "3350 Lab-6");
}

bool X11_wrapper::getXPending()
{
    // See if there are pending events.
    return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent()
{
    // Get a pending event.
    XEvent e;
    XNextEvent(dpy, &e);
    return e;
}

void X11_wrapper::swapBuffers()
{
    glXSwapBuffers(dpy, win);
}

void X11_wrapper::reshape_window(int width, int height)
{
    // Window has been resized.
    g.xres = width;
    g.yres = height;
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glOrtho(0, g.xres, 0, g.yres, -1, 1);
}

void X11_wrapper::check_resize(XEvent *e)
{
    // The ConfigureNotify is sent by the server if the window is resized.
    if (e->type != ConfigureNotify)
        return;
    XConfigureEvent xce = e->xconfigure;
    if (xce.width != g.xres || xce.height != g.yres) {
        // Window size did change.
        reshape_window(xce.width, xce.height);
    }
}

int X11_wrapper::check_keys(XEvent *e)
{
    if (e->type != KeyPress && e->type != KeyRelease)
        return 0;
    int key = XLookupKeysym(&e->xkey, 0);
    if (e->type == KeyPress) {
        switch (key) {
            case XK_a:
                break;
            case XK_Escape:
                // Escape key was pressed
                return 1;
        }
    }
    return 0;
}
*/
//Platform_Texture *platformTexture;
/*
void init_opengl(void)
{
    // OpenGL initialization
    glViewport(0, 0, g.xres, g.yres);
    // Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    // Set 2D mode (no perspective)
    glOrtho(0, g.xres, 0, g.yres, -1, 1);
    // Set the screen background color
    glClearColor(0.1, 0.1, 0.1, 1.0);
    
    // Enable textures
    glEnable(GL_TEXTURE_2D);
    // Load platform texture
    platformTexture = new Texture("grass_block.jpg");
}
*/
//Platform_Texture PlatformTexture;
void render_platforms()
{
    //glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, PlatformTexture.textureID);

    // draw 2 platforms
    for (int i = 0; i < 3; i++) {
        // move platforms to the left
        box.pos[i] += box.vel[0];

        // reappear platforms after moving offscreen
        if (box.pos[i] < -box.w)
            box.pos[i] = g.xres + box.w;
        
        glPushMatrix();
	    glColor3ub(0, 100, 0);
	    glTranslatef(box.pos[i]+1, box.pos[i+1], 0.0f);
	    glBegin(GL_QUADS);
		    //glTexCoord2f(PlatformTexture.xc[0], PlatformTexture.yc[0]); 
            glVertex2f(-box.w, -box.h);
		    //glTexCoord2f(PlatformTexture.xc[0], PlatformTexture.yc[1]); 
            glVertex2f(-box.w,  box.h);
		    //glTexCoord2f(PlatformTexture.xc[1], PlatformTexture.yc[1]); 
            glVertex2f( box.w,  box.h);
		    //glTexCoord2f(PlatformTexture.xc[1], PlatformTexture.yc[0]); 
            glVertex2f( box.w, -box.h);
	    glEnd();
	    
        glPopMatrix();
        i++;
    }

    //glBindTexture(GL_TEXTURE_2D, 0);
    //glDisable(GL_TEXTURE_2D);

    // Ground platform separately--don't want it moving
    glPushMatrix();
    glColor3ub(0, 100, 0);
    glTranslatef(box.pos[4], box.pos[5], 0.0f);
    glBegin(GL_QUADS);
        //glTexCoord2f(PlatformTexture.xc[0], PlatformTexture.yc[0]); 
        glVertex2f(-g.xres, -box.h);
        //glTexCoord2f(PlatformTexture.xc[0], PlatformTexture.yc[1]); 
        glVertex2f(-g.xres,  box.h);
        //glTexCoord2f(PlatformTexture.xc[1], PlatformTexture.yc[1]); 
        glVertex2f( g.xres,  box.h);
        //glTexCoord2f(PlatformTexture.xc[1], PlatformTexture.yc[0]); 
        glVertex2f( g.xres, -box.h);
    glEnd();
    glPopMatrix();
    // unbind texture
    //glBindTexture(GL_TEXTURE_2D, 0);
    //glDisable(GL_TEXTURE_2D);
}
