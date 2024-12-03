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

using namespace std;

extern bool end_screen;

extern class Global {
public:
    int xres, yres;
} g;

extern vector<int> block_coords;
class Score {
public:
    //float platform_start_x;
    //float last_increment_x;
    float increment_distance;
    float total_distance;
    //float low_platform_x;
    //float last_low_platform_x;
    //float pixels_to_feet;
    Score() {
        //platform_start_x = 0.0f;
        //last_increment_x = 0.0f;
        increment_distance = 0.001f;
        total_distance = 0.0f;
        //low_platform_x = 0.0f; //block_coords[0]; //box.pos[2];
        //last_low_platform_x = low_platform_x;
        //pixels_to_feet = 0.001f;
    }
} s;

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
    r2.left = g.xres / 2;
    r2.center = 0;

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
    snprintf(end_text, sizeof(end_text), "G A M E    O V E R");

    glColor3ub(255, 255, 255); // Set text color to white
    ggprint16(&r2, 16, 0x000000, end_text);
}












