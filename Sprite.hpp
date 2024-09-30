//Link: https://stackoverflow.com/questions/1568559/rendering-sprites-from-spritesheet-with-opengl
#include <GL/gl.h>

class SpriteSheet {
    GLuint textureID; 
    int texWidth, texHeight;
    int spriteWidth, spriteHeight;

public:
    SpriteSheet(int texWidth, int texHeight, int spriteWidth, int spriteHeight) {};

    void drawSprite(float posX, float posY, int frameIndex);
};