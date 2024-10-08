//
//modified by: Bryan Rodriguez
//date: 10/01/2024
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
const int MAX_PARTICLES = 500;
const int MAX_BOXES = 5;

//some structures
class Global {
public:
	int xres, yres;
	Global();
} g;

class Box {
public:
    float pos[2];
    float last_pos[2];
    float vel[2];
    int w, h;
    Box() {
        pos[0] = g.xres / 2;
        pos[1] = g.yres / 2;
        vel[0] = vel[1] = 0.0f;
        w = 100;
        h = 50;
    }
} box;
Box particle[MAX_PARTICLES];
int n = 0;
Box boxes[MAX_BOXES];

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
	init_opengl();
	int done = 0;
	//main game loop
	while (!done) {
		//look for external events such as keyboard, mouse.
		while (x11.getXPending()) {
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			x11.check_mouse(&e);
			done = x11.check_keys(&e);
		}
		physics();
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
    for (int i=0; i<MAX_BOXES;i++) {
        boxes[i].pos[0] = (g.xres / (MAX_BOXES + 1)) * (i + 1);
        boxes[i].pos[1] = g.yres / 2;
        boxes[i].w = 20.0f;
        boxes[i].h = 20.0f;
    }
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
	XStoreName(dpy, win, "3350 Lab-1");
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
void X11_wrapper::check_mouse(XEvent *e)
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
}

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

const float GRAVITY = -0.1;
//#define rnd() (float)rand() / (float)RAND_MAX
void physics()
{
    for (int i=0; i<n; i++) {
	    particle[i].vel[1] += GRAVITY;
        particle[i].last_pos[0] = particle[i].pos[0];
        particle[i].last_pos[1] = particle[i].pos[1];
	    particle[i].pos[0] += particle[i].vel[0];
	    particle[i].pos[1] += particle[i].vel[1];
    }
    //collision detection
    for (int i=0; i<n; i++) {
	    if (particle[i].pos[1] < box.pos[1]+box.h && 
	        particle[i].pos[1] > box.pos[1]-box.h &&
	        particle[i].pos[0] > box.pos[0]-box.w &&
	        particle[i].pos[0] < box.pos[0]+box.w) {
            //in a collision state
	        //particle[i].pos[0] = particle[i].last_pos[0];
	        particle[i].pos[1] = particle[i].last_pos[1];
	        particle[i].vel[1] = -particle[i].vel[1] * 0.5;
	        particle[i].vel[0] += rnd() * 0.04;
        }
        if (particle[i].pos[1] < 0) {
            //delete this particle
            particle[i] = particle[--n];
        }
    }
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//draw a box
	for (int i=0; i<MAX_BOXES; i++) {
        glPushMatrix();
	    glColor3ub(100, 120, 220);
	    glTranslatef(boxes[i].pos[0], boxes[i].pos[1], 0.0f);
	    glBegin(GL_QUADS);
		    glVertex2f(-boxes[i].w, -boxes[i].h);
		    glVertex2f(-boxes[i].w,  boxes[i].h);
		    glVertex2f( boxes[i].w,  boxes[i].h);
            glColor3ub(200, 0, 0);
		    glVertex2f( boxes[i].w, -boxes[i].h);
	    glEnd();
	    glPopMatrix();
    }

	//draw a particle
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
    }
}





