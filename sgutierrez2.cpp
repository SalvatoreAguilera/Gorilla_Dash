#include "Sprite.hpp"
#include "AlphaImage.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "handler_sprite.hpp"
#include <chrono>

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

/* For collision detection the coordinates of the dinosaur will replace the y axis, currently the dinosaur jumps
 and returns back to zero, and it runs at zero*/

void handle_running(bool& running, Sprite& sprite_run) {
	
	static int i = 0;
	static auto start = std::chrono::steady_clock::now();
	auto now = std::chrono::steady_clock::now();
	if(running == false) {
		i = 0;
		return;
	}
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
	sprite_run.drawSprite(0, 0, i);
	if (elapsed.count() >= 100) {
		i++;
		if (i == 8) {
			i = 0; 
		}

		start = now;
	}
}

void handle_jumping(bool& running, Sprite& sprite_jump) {
	if(running == true) {
		return;
	}

	static int i = 0;
	static auto start = std::chrono::steady_clock::now();
	auto now = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
    static int jump_height = 0;

    
    if(i == 1) jump_height = 30;
    else if(i == 2) jump_height = 60;
    else if(i == 3) jump_height = 90;
    else if(i == 4) jump_height = 40;
    else if(i == 5) jump_height = 0;

	sprite_jump.drawSprite(0, jump_height, i);
	if (elapsed.count() >= 100) {
		i++;
		if (i == 5) {
			i = 0;
			running = true; 
		}

		start = now;
	}
    
}