#include "Sprite.hpp"
#include "AlphaImage.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "handler_sprite.hpp"
#include <chrono>
#include <iostream>
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
    for (int i = 0; i < (int)block_coords.size(); i++) {
        int charLeft = c[0] + moveX;
        int charRight = c[4] + moveX;
        int charBottom = c[1] + moveY;
        int charTop = c[3] + moveY;

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
    static int jumpY = 60;
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
    for(int i = 0; i < 6; i++) {
        if(i < 3) {
            sprite_block.drawSprite(i*(w-1), (h-1), i);
            if(init) block_coords.push_back({i*(w-1), (h-1),   i*(w-1), (h-1)+100,    i*(w-1)+100, (h-1)+100,    i*(w-1)+100, (h-1)});
        }
        else {
            sprite_block.drawSprite((i-3)*(w-1), 0, i);
            if(init) block_coords.push_back({(i-3)*(w-1), 0,   (i-3)*(w-1), 100,    (i-3)*(w-1)+100, 100,    (i-3)*(w-1)+100, 0});
        }
    }
    
    h+=50;
    //bottom right corner block
    for(int i = 0; i < 6; i++) {
        int offset = 500;
        if(i < 3){
            sprite_block.drawSprite(offset + i*(w-1), (h-1), i);
            if(init) block_coords.push_back({offset + i*(w-1), (h-1),   offset + i*(w-1), (h-1)+100,    offset + i*(w-1)+100, (h-1)+100,    offset + i*(w-1)+100, (h-1)});
        }
        else {
            sprite_block.drawSprite(offset + (i-3)*(w-1), 0, i);
            if(init) block_coords.push_back({offset + (i-3)*(w-1), 0,   offset + (i-3)*(w-1), 100,    offset + (i-3)*(w-1)+100, 100,    offset + (i-3)*(w-1)+100, 0});
        }
    }

    
    init = false;
    
}

void init_character(std::vector<int>& character_coords, Sprite& sprite, Sprite& sprite_block) {
    //function intitialize character coordinates
    // uses the width and height of the character sprite and block sprite
    //int wc = sprite.spriteWidth, hc = sprite.spriteHeight;
    //int wb = sprite_block.spriteWidth, hb = sprite_block.spriteHeight;
    //int offset = 200;
    character_coords = { 0,200,  0,374,  174,374,  174, 200 };
}

void handle_gravity(std::vector<int>& character_coords, std::vector<std::vector<int>>& block_coords, bool& gravity, bool& jump) {
    int moveX = 0;
    int moveY = -2;
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
