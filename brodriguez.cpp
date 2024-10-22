//
//modified by: Bryan Rodriguez
//
//
//original author: Gordon Griesel
//date:            Fall 2024
//purpose:         OpenGL sample program
//
//This program needs some refactoring.
//We will do this in class together.
//
//
#include <iostream>
using namespace std;
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
const int MAX_PARTICLES = 1000;

class Global {
public:
	int xres, yres;
	Global();
} g;

class Box {
public:
    float pos[10];
    float last_pos[2];
    float vel[2];
    int w, h;
    Box() {
        w = 150;
        h = 25;
        //box 1
        pos[0] = g.xres / 2.15;
        pos[1] = g.yres / 2;
        
        /*//box 2
        pos[2] = g.xres / 1.6;
        pos[3] = g.yres / 2.5;*/
        
        //box 3
        pos[4] = g.xres / 1.3;
        pos[5] = g.yres / 3.35;
        
        /*//box 4
        pos[6] = pos[0] - w * 2 + (w / 1.5);
        pos[7] = pos[1] + h * 2.5;*/
        
        /*//box 5
        pos[8] = pos[6] - w * 2 + (w / 1.25);
        pos[9] = pos[7] + h * 2.5;*/
        
        vel[0] = 0.7f; 
        vel[1] = 0.0f;
    }
} box;
//Box particle[MAX_PARTICLES];
int n = 0;

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
	void check_mouse(XEvent *e);
	int check_keys(XEvent *e);
} x11;

//Function prototypes
void init_opengl(void);
void physics(void);
void render(void);


int main()
{
    int argc = 1;
    const char *argv[1] = { "app" };
    glutInit(&argc, (char**)argv);
	init_opengl();
	int done = 0;
	//main game loop
	while (!done) {
		//look for external events such as keyboard, mouse.
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			//x11.check_mouse(&e);
			done = x11.check_keys(&e);
		}
		//physics();
		render();
		x11.swapBuffers();
		usleep(200);
	}
	return 0;
}

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

X11_wrapper::X11_wrapper()
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
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "3350 Lab-6");
}

bool X11_wrapper::getXPending()
{
	//See if there are pending events.
	return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent()
{
	//Get a pending event.
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
	//Window has been resized.
	g.xres = width;
	g.yres = height;
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
}

void X11_wrapper::check_resize(XEvent *e)
{
	//The ConfigureNotify is sent by the
	//server if the window is resized.
	if (e->type != ConfigureNotify)
		return;
	XConfigureEvent xce = e->xconfigure;
	if (xce.width != g.xres || xce.height != g.yres) {
		//Window size did change.
		reshape_window(xce.width, xce.height);
	}
}
//-----------------------------------------------------------------------------

#define rnd() (float)rand() / (float)RAND_MAX
/*void X11_wrapper::check_mouse(XEvent *e)
{
	static int savex = 0;
	static int savey = 0;

	//Weed out non-mouse events
	if (e->type != ButtonRelease &&
		e->type != ButtonPress &&
		e->type != MotionNotify) {
		//This is not a mouse event that we care about.
		return;
	}
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed.
			int y = g.yres - e->xbutton.y;
			//make particle
            if (n < MAX_PARTICLES) {
                particle[n].pos[0] = e->xbutton.x;
                particle[n].pos[1] = y;
                particle[n].w = 4;
                particle[n].h = 4;
                particle[n].vel[0] = 0.0f;
                particle[n].vel[1] = 0.0f;
                ++n;
            }
            return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed.
			return;
		}
	}
	if (e->type == MotionNotify) {
		//The mouse moved!
		if (savex != e->xbutton.x || savey != e->xbutton.y) {
			savex = e->xbutton.x;
			savey = e->xbutton.y;
			//Code placed here will execute whenever the mouse moves. 
			int y = g.yres - e->xbutton.y;
            for (int i=0; i<2; i++)
            if (n < MAX_PARTICLES) {
                particle[n].pos[0] = e->xbutton.x;
                particle[n].pos[1] = y;
                particle[n].w = 4;
                particle[n].h = 4;
                particle[n].vel[0] = rnd() * 1.0 - 0.5;
                particle[n].vel[1] = rnd() * 1.0 - 0.5;
                ++n;
            }

		}
	}
}*/

int X11_wrapper::check_keys(XEvent *e)
{
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
	int key = XLookupKeysym(&e->xkey, 0);
	if (e->type == KeyPress) {
		switch (key) {
			case XK_a:
				//the 'a' key was pressed
				break;
			case XK_Escape:
				//Escape key was pressed
				return 1;
		}
	}
	return 0;
}

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, g.xres, g.yres);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//Set 2D mode (no perspective)
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
	//Set the screen background color
	glClearColor(0.1, 0.1, 0.1, 1.0);
}
/*
const float GRAVITY = -0.1;
void physics()
{
    for (int i = 0; i < n; i++) {
        // Apply gravity
        particle[i].vel[1] += GRAVITY;
        particle[i].last_pos[0] = particle[i].pos[0];
        particle[i].last_pos[1] = particle[i].pos[1];
        particle[i].pos[0] += particle[i].vel[0];
        particle[i].pos[1] += particle[i].vel[1];
    }
    
    // Collision detection for all boxes
    for (int i = 0; i < n; i++) {
        // Iterate over the boxes (each box has an x and y position)
        for (int j = 0; j < 9; j += 2) {
            // Check if particle is within the bounds of the current box
            if (particle[i].pos[1] < box.pos[j+1] + box.h &&
                particle[i].pos[1] > box.pos[j+1] - box.h &&
                particle[i].pos[0] > box.pos[j] - box.w &&
                particle[i].pos[0] < box.pos[j] + box.w) {
                // In a collision state with box j/2 (box #1, #2, etc.)
                particle[i].pos[1] = particle[i].last_pos[1];
                particle[i].vel[1] = -particle[i].vel[1] * 0.5;
                particle[i].vel[0] += rnd() * 0.04;
            }
        }
        // Remove particle if it goes off-screen
        if (particle[i].pos[1] < 0) {
            particle[i] = particle[--n];
        }
    }
}
*/
/*void renderBitmapString(float x, float y, void *font, const char *string) {
    glRasterPos2f(x, y);
    for (const char *c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void renderText(const char* message) {
    glColor3f(1.0f, 1.0f, 1.0f);

    float textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, 
                                       (const unsigned char*)message);
    float textX = -(textWidth / 2.0f); // Center horizontally
    float textY = -4.0f; // Center vertically

    glRasterPos2f(textX, textY);

    renderBitmapString(textX, textY, GLUT_BITMAP_HELVETICA_18, message);
}*/

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//draw 5 boxes
    for (int i = 0; i < 9; i++) {
        glPushMatrix();
	    glColor3ub(50, 205, 50);
	    glTranslatef(box.pos[i]+1, box.pos[i+1], 0.0f);
	    glBegin(GL_QUADS);
		    glVertex2f(-box.w, -box.h);
		    glVertex2f(-box.w,  box.h);
		    glVertex2f( box.w,  box.h);
		    glVertex2f( box.w, -box.h);
	    glEnd();
	    
       /* //Text in boxes
        switch (i / 2) {
            case 0:
                renderText("Coding");
                break;
            case 1:
                renderText("Testing");
                break;
            case 2:
                renderText("Maintenance");
                break;
            case 3:
                renderText("Design");
                break;
            case 4:
                renderText("Requirements");
                break;
        }*/

        glPopMatrix();
        i++;
    }
    /*//draw a particle
	for (int i=0; i<n; i++) {
        glPushMatrix();
	    glColor3ub(100, 120, 220);
	    glTranslatef(particle[i].pos[0], particle[i].pos[1], 0.0f);
	    glBegin(GL_QUADS);
		    glVertex2f(-particle[i].w, -particle[i].h);
		    glVertex2f(-particle[i].w,  particle[i].h);
		    glVertex2f( particle[i].w,  particle[i].h);
		    glVertex2f( particle[i].w, -particle[i].h);
	    glEnd();
	    glPopMatrix();
    }*/
}
