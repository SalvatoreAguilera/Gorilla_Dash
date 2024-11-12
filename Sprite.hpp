#ifndef SPRITE_HPP // Include guard
#define SPRITE_HPP

#include <GL/gl.h>
#include <vector> 
class Sprite {
public:
    GLuint textureID;
    int texWidth, texHeight;
    int spriteWidth, spriteHeight;

    // Constructor
    Sprite(int texWidth, int texHeight, int spriteWidth, int spriteHeight, unsigned char *data);

    // Method to draw the sprite
    void drawSprite(float posX, float posY, int frameIndex);
};

#endif // SPRITE_HPP
