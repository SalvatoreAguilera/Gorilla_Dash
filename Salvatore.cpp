/*

Added a sprite which will be the main character of the game
contains 8 frames of the sprite
File: sprite.png

*/

// Added a class to read in the sprite and get every frame of the sprite
//File : Sprite.hpp
//File : Sprite.cpp
//NOTE - it is not include in makefile because I need to fix some things in the code

/*
class SpriteSheet
{
public:
	Texture text;
	int texWidth, texHeight;
	int spriteWidth, spriteHeight;

	// Constructor to load texture
	SpriteSheet(int texWidth, int texHeight, int spriteWidth, int spriteHeight)
	{
		this->texWidth = texWidth;
		this->texHeight = texHeight;
		this->spriteWidth = spriteWidth;
		this->spriteHeight = spriteHeight;

		text.backImage = &img[1];
		glGenTextures(1, &text.backTexture);
		glBindTexture(GL_TEXTURE_2D, text.backTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, text.backImage->width, text.backImage->height, 0,
					 GL_RGB, GL_UNSIGNED_BYTE, text.backImage->data);
	}

	void drawSprite(float posX, float posY, int frameIndex)
	{
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
		glBindTexture(GL_TEXTURE_2D, text.backTexture);
		glVertexPointer(2, GL_FLOAT, 0, verts);
		glTexCoordPointer(2, GL_FLOAT, 0, texVerts);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		// Disable the arrays
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
};
*/