// Bryan Rodriguez
//
#include <iostream>
#include <unistd.h>
#include <cstring>
#include "platforms.h"
#include "fonts.h"
#include <cstring>
#include <cmath>
#include <string>
#include <sstream>
#include "handler_sprite.hpp"
#include <random>
#include <cstdlib>
#include <ctime>

using namespace std;

extern bool end_screen;
extern vector<int> block_coords;
extern vector<int> char_coords;
extern int current_health;
extern void take_damage();

extern class Global {
public:
    int xres, yres;
} g;

class Damage {
public:
    float pos[4];
    float vel[2];
    int w, h;
    Damage() {
        w = 6;
        h = 6;

        pos[0] = g.xres / 0.78; // high
        pos[1] = g.yres / 1.3;
        
        pos[2] = g.xres / 1.3; // low
        pos[3] = g.yres / 5.2;

        vel[0] = -0.3f; // velocity for moving platforms
        vel[1] = 0.0f;
    }

    void randomize_size() {
        // Randomly set width and height (6x6 or 10x10)
        w = (rand() % 2 == 0) ? 6 : 10;
        h = w;
    }
} d;

class Score {
public:
    float increment_distance;
    float total_distance;
    Score() {
        increment_distance = 0.001f;
        total_distance = 0.0f;
    }
} s;

// this func was worked on by Bryan and Salvatore
//map is to keep track of each individual platform
// for example if we have a platform with 3 blocks 
// the pair is <endindex, size> 
// this is  needed to be ale to generate new platforms when they go out of bounds
// because we have to remove from block_coords the plat form that went out of bounds
void handle_platform(Sprite& sprite_plat, vector<vector<int>>& plat_coords, unordered_map<int,int>& mp){
    static bool init = true;
    
    if(init) {
        int platform1 = randNum(5,8), pw1 = 100, ph1 = 225;
        int platform2 = 4, pw2 = 100, ph2 = 450;
        int platform3 = 4, pw3 = 100, ph3 = 450;
        
        int offset1 = randNum(200,300);
        int offset2 = 100;
        int offset3 = 800;
        //middle plat
        for(int i = 0;i < platform1;i++) {
            if(i == 0) mp.insert({plat_coords.size()-1+platform1, platform1});
            sprite_plat.drawSprite(i*(pw1)+offset1, ph1, 0);
            getCoords(plat_coords, sprite_plat.spriteHeight, sprite_plat.spriteWidth, i*pw1+offset1, ph1);
            
        }


        //left plat
        for(int i = 0;i < platform2;i++) {
            if(i == 0) mp.insert({plat_coords.size()-1+platform2, platform2});
            sprite_plat.drawSprite(i*(pw2)+offset2, ph2, 0);
            getCoords(plat_coords, sprite_plat.spriteHeight, sprite_plat.spriteWidth, i*pw2+offset2, ph2);
            
        }


        //right plat
        for(int i = 0;i < platform3;i++) {
            if(i == 0) mp.insert({plat_coords.size()-1+platform3, platform3});
            sprite_plat.drawSprite(i*(pw3)+offset3, ph3, 0);
            getCoords(plat_coords, sprite_plat.spriteHeight, sprite_plat.spriteWidth, i*pw3+offset3, ph3);
            
        }


    }

    //move plats Right
    moveRight(plat_coords, sprite_plat);
    unordered_set<int> plat_out_bounds;

    //check out of bounds plats
    outOfBounds(plat_out_bounds,plat_coords, mp);
    
    // If platform out of bounds, then make a new one
    for (auto& idx : plat_out_bounds) {
        int size = mp[idx];
        mp.erase(idx);
        int currH = plat_coords[idx][1]; // Get current height for redraw
        int eraseStart = idx - size + 1;
        int eraseEnd = idx;
        //deletes old vectors coords from 2d list
        plat_coords.erase(plat_coords.begin() + eraseStart, plat_coords.begin() + eraseEnd); 
        
        // Adjust indices in the map after erasing rows
        vector<pair<int, int>> to_insert; // For new keys and values
        vector<int> to_erase; 
        for (auto& m : mp) {
            if (m.first >= eraseEnd) {
                int new_key = m.first - (eraseEnd - eraseStart); 
                int value = m.second;
                
                to_insert.push_back({new_key, value});
                to_erase.push_back(m.first);

            } 
        }

        // delete old keys from map
        mapDelete(to_erase, mp);
        // Insert new keys
        mapInsert(to_insert, mp);
        
        
        
        // Add new platforms
        //big platform
        if (size >= 5) {
            int platform = randNum(5, 8);
            for (int i = 0; i < platform; i++) {
                if(i == 0) mp.insert({plat_coords.size()-1+platform, platform});
                sprite_plat.drawSprite((i * 100) + 1000, currH, 0);
                getCoords(plat_coords, sprite_plat.spriteHeight, sprite_plat.spriteWidth, (i * 100) + 1000, currH);
            }
        }
        //small platform 
        else if(size <= 4) {
            int platform = randNum(1, 4);
            //we need to calculate offset to keep the same distance as the init dits with the platforms
            // blocks = 4
            // block - platform
            int calc = 4 - platform;
            int offset = calc*100;
            for (int i = 0; i < platform; i++) {
                if(i == 0) mp.insert({plat_coords.size()-1+platform, platform});
                sprite_plat.drawSprite((i * 100) + 1000 + offset, currH, 0);
                getCoords(plat_coords, sprite_plat.spriteHeight, sprite_plat.spriteWidth, (i * 100) + 1000 + offset, currH);
            }
        }
        
    }
    
    init = false;
}

void calculate_distance() {
    s.total_distance += s.increment_distance;
}


void render_score() {
    Rect r;
    r.bot = g.yres - 40; // Position from the bottom
    r.left = g.xres - 170; // Position from the left
    r.center = 0; // Left-aligned text

    calculate_distance();
    char text[128];
    snprintf(text, sizeof(text), "Distance: %.2f m", s.total_distance);

    glColor3ub(255, 255, 255); // Set text color to white
    ggprint16(&r, 16, 0x000000, text);
    glColor3f(1.0, 1.0, 1.0);
}

void render_end_screen() {
    // need ramon's current_health variable in main
    // to set end_screen = true

    Rect r2;
    r2.bot = g.yres / 2;
    r2.left = g.xres / 2.5;
    r2.center = 0;
 
    Rect r3;
    r3.bot = g.yres / 2.15;
    r3.left = g.xres / 2.25;
    r3.center = 0;

    Rect r4;
    r4.bot = g.yres / 2.3;
    r4.left = g.xres / 2.12;
    r4.center = 0;

    // transparent screen
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(g.xres, 0);
        glVertex2i(g.xres, g.yres);
        glVertex2i(0, g.yres);
    glEnd();
    glDisable(GL_BLEND);

    char end_text[128];
    char final_score_text[128];
    char end_distance[128];
    snprintf(end_text, sizeof(end_text), "G A M E    O V E R");
    snprintf(final_score_text, sizeof(final_score_text), "Final score:");
    snprintf(end_distance, sizeof(end_distance), "%.2f m", s.total_distance);

    glColor3ub(255, 255, 255); // Set color to white
    ggprint16(&r2, 16, 0x000000, end_text);
    
    glColor3ub(255, 255, 255);
    ggprint16(&r3, 13, 0x000000, final_score_text);
    
    glColor3ub(255, 255, 255);
    ggprint16(&r4, 13, 0x000000, end_distance);
    
    glColor3ub(255, 255, 255); // Set color back to white
}

bool check_collision(float obj1_x, float obj1_y, int obj1_w, int obj1_h,
                     const std::vector<int>& char_coords) {
    // Calculate the Dino's bounding box from char_coords
    int dino_min_x = char_coords[0]; // Bottom-left x
    int dino_min_y = char_coords[1]; // Bottom-left y
    int dino_max_x = char_coords[6]; // Top-right x
    int dino_max_y = char_coords[5]; // Top-right y

    return !(obj1_x + obj1_w < dino_min_x || 
             obj1_x > dino_max_x || 
             obj1_y + obj1_h < dino_min_y || 
             obj1_y > dino_max_y);
}

void render_damaging_objects()
{
    int damage1 = 5;
    int damage2 = 10;
    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < 3; i++) {
        d.pos[i] += d.vel[0];

        // reappear objects after moving offscreen
        if (d.pos[i] < -d.w) {
            d.pos[i] = g.xres + d.w;
            
            // Randomize size when object reappears
            d.randomize_size();
        }

        // Check for collision with the Dino
        if (check_collision(d.pos[i], d.pos[i+1], d.w, d.h, char_coords)) {
            if (d.w == 6)
                take_damage(damage1);
            else if (d.w == 10)
                take_damage(damage2);
            d.pos[i] = g.xres + d.w; // Reset the pos of the damaging object
            d.randomize_size();
        }

        glPushMatrix();
	    glColor3ub(139, 69, 19);
	    glTranslatef(d.pos[i]+1, d.pos[i+1], 0.0f);
	    glBegin(GL_QUADS);
            glVertex2f(-d.w, -d.h);
            glVertex2f(-d.w,  d.h);
            glVertex2f( d.w,  d.h);
            glVertex2f( d.w, -d.h);
	    glEnd();
	    
        glPopMatrix();
        i++;
    }
    glColor3f(1.0, 1.0, 1.0);
}











