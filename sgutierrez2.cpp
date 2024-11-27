#include "Sprite.hpp"
#include "AlphaImage.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "handler_sprite.hpp"
#include <chrono>
#include <iostream>
#include <random>
#include <unordered_set>
//the coordinate system for all images
//8 coordinates in each array
//each pair is the represents w and h of the image for each corner


/*Added everythign regarding character animation, movement, and collision, created multiple classes
    like AlphaImage which reads in the character sprtie, handler_sprite which handles different movememnt events 
    with the character. Lastly loaded all spritesheets and set them up to be ready to read in.
    Also added flags in the main file for handling different events with different keys and created arrays to 
    save coordinates of all images*/

Sprite::Sprite(int texWidth, int texHeight, int spriteWidth, int spriteHeight, unsigned char *data) {
    this->texWidth = texWidth;
    this->texHeight = texHeight;
    this->spriteWidth = spriteWidth;
    this->spriteHeight = spriteHeight;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Sprite::drawSprite(float posX, float posY, int frameIndex) {
    const float verts[] = {
            posX, posY,
            posX + spriteWidth, posY,
            posX + spriteWidth, posY + spriteHeight,
            posX, posY + spriteHeight
        };
        const float tw = float(spriteWidth) / texWidth;
        const float th = float(spriteHeight) / texHeight;
        const int numPerRow = texWidth / spriteWidth;
        const float tx = (frameIndex % numPerRow) * tw;
        const float ty = (frameIndex / numPerRow+1) * th;
        const float texVerts[] = {
            tx, ty + th,        // top-left corner
            tx + tw, ty + th,   // top-right corner
            tx + tw, ty,        // bottom-right corner
            tx, ty              // bottom-left corner
        };

        // Enable the proper arrays
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        // Bind the texture and set pointers
        glBindTexture(GL_TEXTURE_2D, textureID);
        glVertexPointer(2, GL_FLOAT, 0, verts);
        glTexCoordPointer(2, GL_FLOAT, 0, texVerts); 
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Disable the arrays
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


AlphaImage::AlphaImage(const char* filename) {
    data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
}

void handle_running(bool& running, int& direction, bool& idle, Sprite& sprite_run, std::vector<int>& character_coords, std::vector<std::vector<int>>& block_coords) {
	static int i = 0;
    if(running == false) {
		i = 0;
		return;
	}

    //stop idle animation to start running
    idle = false;
    //direction == -1 then go left | direction == 1 then go right
    int moveX = 1 * direction;
    int moveY = 0;
	static auto start = std::chrono::steady_clock::now();
	auto now = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    if(collision_sprite(character_coords, moveX, moveY, block_coords)) {
        running = false;
        idle = true;
        return;
    } else {
        character_coords[0] += moveX;
        character_coords[2] += moveX;
        character_coords[4] += moveX;
        character_coords[6] += moveX;
	    sprite_run.drawSprite(character_coords[0], character_coords[1], i);
    }
	if (elapsed.count() >= 100) {
		i++;
		if (i == 8) {
			i = 0; 
            running = false;
            //start idle animation
            idle = true;
		}

		start = now;
	}
    
}

bool collision_sprite(std::vector<int>& c, int& moveX, int& moveY, std::vector<std::vector<int>>& block_coords) {
    int characterPaddingx = 45;
    int characterPaddingy = 12;
    for (int i = 0; i < (int)block_coords.size(); i++) {
        int charLeft = c[0] + moveX + characterPaddingx;
        int charRight = c[4] + moveX - characterPaddingx - 20;
        int charBottom = c[1] + moveY + characterPaddingy;
        int charTop = c[3] + moveY - characterPaddingy;

        int blockLeft = block_coords[i][0];
        int blockRight = block_coords[i][4];
        int blockBottom = block_coords[i][1];
        int blockTop = block_coords[i][3];

        if (charRight >= blockLeft && charLeft <= blockRight && 
            charTop >= blockBottom && charBottom <= blockTop) {
                
            return true;
        }
    }
    return false;
}

void handle_jumping(bool& jump, bool& idle, Sprite& sprite_jump, std::vector<int>& character_coords,
 std::vector<std::vector<int>>& block_coords, int& direction) {
    if(jump == false) {
		return;
	}

    //stop idle animation to start jumping
    idle = false;
    static int previ = 0;
	static int i = 0;
	static auto start = std::chrono::steady_clock::now();
	auto now = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    static int jumpY = 120;
    int jumpX = 60*direction;
    static bool isAscending = true;
    int temp = jumpY*-1;

    if(isAscending && collision_sprite(character_coords, jumpX, jumpY, block_coords))
        sprite_jump.drawSprite(character_coords[0], character_coords[1], i);
    
    else if(!isAscending && collision_sprite(character_coords, jumpX, temp, block_coords))
        sprite_jump.drawSprite(character_coords[0], character_coords[1], i);
     
    else {
        if(i < 3 && previ != i) {
            character_coords[0] += jumpX; character_coords[1] += jumpY;
            character_coords[2] += jumpX; character_coords[3] += jumpY;
            character_coords[4] += jumpX; character_coords[5] += jumpY;
            character_coords[6] += jumpX; character_coords[7] += jumpY;
        } else if(previ != i) {
            character_coords[0] += jumpX; character_coords[1] -= jumpY;
            character_coords[2] += jumpX; character_coords[3] -= jumpY;
            character_coords[4] += jumpX; character_coords[5] -= jumpY;
            character_coords[6] += jumpX; character_coords[7] -= jumpY;
        }
        
        sprite_jump.drawSprite(character_coords[0], character_coords[1], i);
    }
	previ = i;
	if (elapsed.count() >= 100) {
        previ = i;
		i++;

		if (i == 5) {
			i = 0;
            previ = 0;
            isAscending = true;
            jump = false;
            //start idle animation
            idle = true;
		}
        if(i == 3) {
            isAscending = false;
        }
		start = now;
	}
    
    
    
}

void handle_idle(bool& idle, Sprite& sprite_idle, std::vector<int>& character_coords) {
    static int i = 0;
    if(idle == false) {
        i = 0;
        return;
    }

    
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    sprite_idle.drawSprite(character_coords[0], character_coords[1], i);
    if (elapsed.count() >= 100) {
        i++;
        if (i == 10) {
            i = 0; 
        }

        start = now;
    }
}

void tile_block(Sprite& sprite_block, std::vector<std::vector<int>>& block_coords) {
    static bool init = true;

    //bottom left corner block
    int w = sprite_block.spriteWidth, h = sprite_block.spriteHeight;
    
    for(int i = 0;i < 10;i++) {
        sprite_block.drawSprite(i*(w), 0, 0);
        if(init) block_coords.push_back({i*(w), (h),   i*(w), (h),    i*(w)+100, (h)+50,    i*(w)+100, (h)});
    }
    

    init = false;
}

//lambda to add coords to  vector of coordinates
auto getCoords = [](std::vector<std::vector<int>>& coords, int& sprite_h, int& sprite_w, 
    int w, int h) {
    
    coords.push_back({(w), (h),   (w), (h+sprite_h),    
    (w+sprite_w), (h+sprite_h),    (w+sprite_w), (h)});
};

//lamda to gen a random number within a range
auto randNum = [](int low, int high) {
    std::random_device rd; 
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(low, high);
    return distrib(gen);
};

//lamda to move platforms right
// used auto because too lazy to write std::vec...
auto moveRight = [](auto& plat_coords, Sprite& sprite_plat) {
    for(int i = 10;i < (int)plat_coords.size();i++) {
        int w = plat_coords[i][0], h = plat_coords[i][1];
        
        int moveX = 1;
        sprite_plat.drawSprite(w-moveX, h, i);
        for(int j = 0;j <= 8;j++) {
            if(j % 2  == 0 && j != 8)
                plat_coords[i][j] -= moveX;
        }
    }
};

//lamda for out of bounds check of platforms
// used auto because too lazy to write std::vec...
auto outOfBounds = [](std::unordered_set<int>& plat_out_bounds, auto& plat_coords , auto& mp) {
    for (auto& entry : mp) {
        int last_block_idx = entry.first; 

        int& plat_right = plat_coords[last_block_idx][6]; 

        
        if (plat_right <= 0) {
            plat_out_bounds.insert(last_block_idx);
        }
    }
};

//delete from map given a vector 
auto mapDelete = [](auto& to_erase, auto& mp){
    for (int key : to_erase) {
        mp.erase(key);
    }
};

//inserts a vector of pairs to map
auto mapInsert = [](auto& to_insert, auto& mp){
    for (auto& pair : to_insert) {
        mp.insert(pair);
    }
};
//map is to keep track of each individual platform
// for example if we have a platform with 3 blocks 
// the pair is <endindex, size> 
// this is  needed to be ale to generate new platforms when they go out of bounds
// because we have to remove from block_coords the plat form that went out of bounds 
void handle_platform(Sprite& sprite_plat, std::vector<std::vector<int>>& plat_coords, std::unordered_map<int,int>& mp){
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
    std::unordered_set<int> plat_out_bounds;

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
        std::vector<std::pair<int, int>> to_insert; // For new keys and values
        std::vector<int> to_erase; 
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


void init_character(std::vector<int>& character_coords, Sprite& sprite, 
    Sprite& sprite_block) {
    //function intitialize character coordinates
    // uses the width and height of the character sprite and block sprite
    //int wc = sprite.spriteWidth, hc = sprite.spriteHeight;
    //int wb = sprite_block.spriteWidth, hb = sprite_block.spriteHeight;
    //int offset = 200;
    character_coords = { 0,200,  0,374,  174,374,  174, 200 };
}

void handle_gravity(std::vector<int>& character_coords, std::vector<std::vector<int>>& block_coords, bool& gravity, bool& jump) {
    int moveX = 0;
    int moveY = -3;
    if(jump || collision_sprite(character_coords, moveX, moveY, block_coords)) {
        gravity = false;
        return;
    } else {
        gravity = true;
        character_coords[0] += moveX;
        character_coords[2] += moveX;
        character_coords[4] += moveX;
        character_coords[6] += moveX;
        character_coords[1] += moveY;
        character_coords[3] += moveY;
        character_coords[5] += moveY;
        character_coords[7] += moveY;
    }
}
