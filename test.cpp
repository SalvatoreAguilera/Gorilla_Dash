#include <stdio.h>
#include <stdlib.h>
#include <SFML/Network.hpp>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <iostream>
#include <vector>
#include "Sprite.hpp"
#include "AlphaImage.hpp"
#include "handler_sprite.hpp"
#include "feature.h"
#include "platforms.h"
#include <chrono>
#include <unordered_map>
#include <iostream>

int PLAYERS = 0;
#define SPRITES 8
int myID;
int prevState = 0, currState = 0;
//flags to handle different animations
struct character {
	bool running = false; 
	bool jump = false;
	bool idle = true;
	bool gravity = false;
	int direction = 1;
};
std::vector<character> characters;
std::vector<std::vector<int>> char_coords;

std::vector<std::vector<int>> block_coords;
std::unordered_map<int,int> coords_mp;

//moveType for sending to server
enum MoveType {
    IDLE = 0,
    JUMP = 1,
    RUN = 2,
    GRAVITY = 3,
};
void updateCharacter(character& enemy, int moveType, int direction = 0) {
    if (moveType == RUN) {
        enemy.running = true;
        enemy.jump = false;
        enemy.idle = false;
        enemy.gravity = false;
        enemy.direction = direction; 
    } 
    else if (moveType == JUMP) {
        enemy.jump = true;
        enemy.running = false;
        enemy.idle = false;
        enemy.gravity = false;
        enemy.direction = direction; 
    } 
    else if (moveType == IDLE) {
        enemy.idle = true;
        enemy.running = false;
        enemy.jump = false;
        enemy.gravity = false;
        enemy.direction = 0;
    } 
    else if (moveType == GRAVITY) {
        enemy.gravity = true;
        enemy.idle = false;
        enemy.running = false;
        enemy.jump = false;
    } 
    else {
        std::cerr << "Unknown moveType: " << moveType << "\n";
    }

    std::cout << "Updated character state: "
              << "Running=" << enemy.running
              << ", Jump=" << enemy.jump
              << ", Idle=" << enemy.idle
              << ", Gravity=" << enemy.gravity
              << ", Direction=" << enemy.direction
              << "\n";
}
auto type1 = [](int& id, int& players) {
    // type 1 has msg type, player
    myID = id;
    characters.resize(players);
	char_coords.resize(players);
	PLAYERS = players;
};
auto type2 = [](int& enemyID, int& moveType, int& direction) {
    if (enemyID < 0 || enemyID >= (int)characters.size()) {
        std::cerr << "Invalid enemyID: " << enemyID << "\n";
    }

    updateCharacter(characters[enemyID], moveType, direction);
    
};
void handle_receive_send(sf::TcpSocket& socket, int& prevState, int& currState) {
    sf::Packet packet;
    if (socket.receive(packet) == sf::Socket::Done) {
        int msgType = 0;
        int id = 0;
        int move = 0;
        int direction = 0;
        packet >> msgType >> id >> move >> direction;

        if(msgType == 1) {
            type1(id, move);
        }
        else if(msgType == 2) {
            type2(id, move, direction);
        }
    } 

    if(prevState != currState) {
        prevState = currState;
        sf::Uint32 msgType = 3;
        sf::Uint32 id = myID;
        sf::Uint32 move = currState;
        sf::Uint32 dir = characters[myID].direction;
        packet << msgType <<  id << move << dir;
        if(socket.send(packet) == sf::Socket::Done) {
            std::cout << "succes sending packet" << std::endl;
        }
    }
}


class Image
{
public:
	int width, height;
	unsigned char *data;
	~Image() { delete[] data; }
	Image(const char *fname)
	{
		if (fname[0] == '\0')
			return;
		char name[40];
		strcpy(name, fname);
		int slen = strlen(name);
		name[slen - 4] = '\0';
		char ppmname[80];
		sprintf(ppmname, "%s.ppm", name);
		char ts[100];
		sprintf(ts, "convert %s %s", fname, ppmname);
		system(ts);
		FILE *fpi = fopen(ppmname, "r");
		if (fpi)
		{
			char line[200];
			fgets(line, 200, fpi);
			fgets(line, 200, fpi);
			// skip comments and blank lines
			while (line[0] == '#' || strlen(line) < 2)
				fgets(line, 200, fpi);
			sscanf(line, "%i %i", &width, &height);
			fgets(line, 200, fpi);
			// get pixel data
			int n = width * height * 3;
			data = new unsigned char[n];
			for (int i = 0; i < n; i++)
				data[i] = fgetc(fpi);
			fclose(fpi);
		}
		else
		{
			printf("ERROR opening image: %s\n", ppmname);
			exit(0);
		}
		unlink(ppmname);
	}
};

Image img[1] = {"./images/BG.png"};
AlphaImage sprite_img[4] = {"./images/jump_dino_2.png", "./images/run_dino.png", "./images/flat_tile.png", "./images/idle_dino.png"};

class Texture
{
public:
	Image *backImage;
	GLuint backTexture;
	float xc[2];
	float yc[2];
};

class Global
{
public:
	int xres, yres;
	Texture tex;
	Global()
	{
		xres = 1000, yres = 750;
	}
} g;

class X11_wrapper
{
private:
	Display *dpy;
	Window win;
	GLXContext glc;

public:
	X11_wrapper()
	{
		GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
		// GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
		setup_screen_res(1000, 750);

		dpy = XOpenDisplay(NULL);
		if (dpy == NULL)
		{
			printf("\n\tcannot connect to X server\n\n");
			exit(EXIT_FAILURE);
		}
		Window root = DefaultRootWindow(dpy);
		XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
		if (vi == NULL)
		{
			printf("\n\tno appropriate visual found\n\n");
			exit(EXIT_FAILURE);
		}
		Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
		XSetWindowAttributes swa;
		swa.colormap = cmap;
		swa.event_mask =
			ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask |
			ButtonPressMask | ButtonReleaseMask |
			StructureNotifyMask | SubstructureNotifyMask;
		win = XCreateWindow(dpy, root, 0, 0, g.xres, g.yres, 0,
							vi->depth, InputOutput, vi->visual,
							CWColormap | CWEventMask, &swa);
		set_title();
		glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
		glXMakeCurrent(dpy, win, glc);
	}
	void cleanupXWindows()
	{
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}
	void setup_screen_res(const int w, const int h)
	{
		g.xres = w;
		g.yres = h;
	}
	void reshape_window(int width, int height)
	{
		// window has been resized.
		setup_screen_res(width, height);
		glViewport(0, 0, (GLint)width, (GLint)height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glOrtho(0, g.xres, 0, g.yres, -1, 1);
		set_title();
	}
	void set_title()
	{
		// Set the window title bar.
		XMapWindow(dpy, win);
		XStoreName(dpy, win, "scrolling background (seamless)");
	}
	bool getXPending()
	{
		return XPending(dpy);
	}
	XEvent getXNextEvent()
	{
		XEvent e;
		XNextEvent(dpy, &e);
		return e;
	}
	void swapBuffers()
	{
		glXSwapBuffers(dpy, win);
	}
	void check_resize(XEvent *e)
	{
		// The ConfigureNotify is sent by the
		// server if the window is resized.
		if (e->type != ConfigureNotify)
			return;
		XConfigureEvent xce = e->xconfigure;
		if (xce.width != g.xres || xce.height != g.yres)
		{
			// Window size did change.
			reshape_window(xce.width, xce.height);
		}
	}
} x11;


void init_opengl(void);
void check_mouse(XEvent *e);
int check_keys(XEvent *e);
void physics(void);
void render(void);
void get_sprite(void);

//===========================================================================
//===========================================================================
int main()
{
	//connect to server
	sf::TcpSocket socket;
	if (socket.connect("127.0.0.1", 8080) != sf::Socket::Done) {
        std::cerr << "Error: Unable to connect to server\n";
        return 1;
    }
    handle_receive_send(socket, prevState, currState);
	socket.setBlocking(false);
	

	init_opengl();
	int done = 0;

	while (!done)
	{
		while (x11.getXPending())
		{
			XEvent e = x11.getXNextEvent();
			x11.check_resize(&e);
			check_mouse(&e);
			done = check_keys(&e);
		}
		render();
		physics();
		handle_receive_send(socket, prevState, currState);
		x11.swapBuffers();

	}
	return 0;
}

void init_opengl(void)
{
	// OpenGL initialization
	glViewport(0, 0, g.xres, g.yres);
	// Initialize matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// This sets 2D mode (no perspective)
	glOrtho(0, g.xres, 0, g.yres, -1, 1);
	// Clear the screen
	glClearColor(1.0, 1.0, 1.0, 1.0);
	// glClear(GL_COLOR_BUFFER_BIT);
	// Do this to allow texture maps
	glEnable(GL_TEXTURE_2D);
	//
	// load the images file into a ppm structure.
	//
	g.tex.backImage = &img[0];
	// create opengl texture elements
	glGenTextures(1, &g.tex.backTexture);
	int w = g.tex.backImage->width;
	int h = g.tex.backImage->height;
	glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
				 GL_RGB, GL_UNSIGNED_BYTE, g.tex.backImage->data);
	g.tex.xc[0] = 0.0;
	g.tex.xc[1] = 1.0;
	g.tex.yc[0] = 0.0;
	g.tex.yc[1] = 1.0;
}

void check_mouse(XEvent *e)
{
	// Did the mouse move?
	// Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	//
	if (e->type == ButtonRelease)
	{
		return;
	}
	if (e->type == ButtonPress)
	{
		if (e->xbutton.button == 1)
		{
			// Left button is down
		}
		if (e->xbutton.button == 3)
		{
			// Right button is down
		}
	}
	if (savex != e->xbutton.x || savey != e->xbutton.y)
	{
		// Mouse moved
		savex = e->xbutton.x;
		savey = e->xbutton.y;
	}
}


int check_keys(XEvent *e)
{
    
    // Was there input from the keyboard?
    if (e->type == KeyPress)
    {
        int key = XLookupKeysym(&e->xkey, 0);
        
        if (key == XK_Escape)
        {
            return 1;
        }
    
        // Handle pause functionality
        if (key == XK_p || key == XK_P) {
            paused = !paused;
            return 0;
        }
    
        //check if the up arrow keys were pressed
        if(key == XK_Up) {
          characters[myID].jump = true;
          characters[myID].running = false;
        }

        //check if right arrow key was pressed
        if(key == XK_Right && !characters[myID].gravity) {
          characters[myID].running = true;
          characters[myID].direction = 1;
        }

        if(key == XK_Left && !characters[myID].gravity) {
          characters[myID].running = true;
          characters[myID].direction = -1;
        }
	
    }
    else if(e->type == KeyRelease) {
      int key = XLookupKeysym(&e->xkey, 0);
      if(key == XK_Right || key == XK_Left) {
        characters[myID].running = false;
        characters[myID].idle = true;
        characters[myID].direction = 0;
      }
	  }
    
    check_title_keys(e);
    check_pause_keys(e); 
  
    return 0;
}
Sprite sprite_jump(sprite_img[0].width, sprite_img[0].height, 250, 174, sprite_img[0].data);
Sprite sprite_run(sprite_img[1].width, sprite_img[1].height, 250, 174, sprite_img[1].data);
Sprite sprite_block(sprite_img[2].width, sprite_img[2].height, 100, 49, sprite_img[2].data);
Sprite sprite_idle(sprite_img[3].width, sprite_img[3].height, 250, 174, sprite_img[3].data);
int size = 3, w = 1000, h = 750;
void physics()
{
	// move the background
	g.tex.xc[0] += 0.0001;
	g.tex.xc[1] += 0.0001;
	static bool b = true;
	if (b)
	{
		for(int i = 0;i < 1;i++)
			init_character(char_coords[i], sprite_run, sprite_block);
		b = false;
	}

	tile_block(sprite_block, block_coords);
	for(int i = 0;i < 1;i++) {
		handle_gravity(char_coords[i], block_coords, characters[i].gravity, characters[i].jump);
		handle_running(characters[i].running, characters[i].direction, characters[i].idle, sprite_run, char_coords[i], block_coords);
		handle_jumping(characters[i].jump, characters[i].idle, sprite_jump, char_coords[i], block_coords, characters[i].direction);
		handle_idle(characters[i].idle, sprite_idle, char_coords[i]);
	}

	//handle_platform(sprite_block, block_coords, coords_mp);

}



void render()
{
	
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, g.tex.backTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(g.tex.xc[0], g.tex.yc[1]);
	glVertex2i(0, 0);
	glTexCoord2f(g.tex.xc[0], g.tex.yc[0]);
	glVertex2i(0, g.yres);
	glTexCoord2f(g.tex.xc[1], g.tex.yc[0]);
	glVertex2i(g.xres, g.yres);
	glTexCoord2f(g.tex.xc[1], g.tex.yc[1]);
	glVertex2i(g.xres, 0);
	glEnd();


	render_health_bar();
	if (paused) {
       	render_pause_screen();
    }
	/*glPushMatrix();
	render_platforms();
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
	
	  */     

}