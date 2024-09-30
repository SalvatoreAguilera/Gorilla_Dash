#include "Sprite.hpp"
#include <GL/gl.h>

class SpriteSheet {
public:
    GLuint textureID;
    int texWidth, texHeight;
    int spriteWidth, spriteHeight;

    
    SpriteSheet(int texWidth, int texHeight, int spriteWidth, int spriteHeight) {
        this->texWidth = texWidth;
        this->texHeight = texHeight;
        this->spriteWidth = spriteWidth;
        this->spriteHeight = spriteHeight;
    }

    void drawSprite(float posX, float posY, int frameIndex) {
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
        const float ty = (frameIndex / numPerRow + 1) * th;
        const float texVerts[] = {
            tx, ty,
            tx + tw, ty,
            tx + tw, ty + th,
            tx, ty + th
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
};
