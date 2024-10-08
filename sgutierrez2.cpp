
//this class is used to get every sprite from a spritesheet and return the coords of the sprite
/*
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

    std::pair<float*,float*> drawSprite(float posX, float posY, int frameIndex) {
        float verts[] = {
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
        float texVerts[] = {
            tx, ty,
            tx + tw, ty,
            tx + tw, ty + th,
            tx, ty + th
        };
		std::pair<float*, float*> arrays;
		arrays.first = verts;
		arrays.second = texVerts;
		return arrays;
        /*
		Renders sprite
		enable the proper arrays
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
		*/
    //}
//};

/*
function to create the spritesheet
uses image class to read in the sprtiesheet and calls the drawSprite function to get the coordinates of the sprite
more work needs to be done to render the sprite when right arrow key is pressed

void createSheet() {
	Image sprite[1] = { "sprite.png" };
	SpriteSheet sheet(sprite->width, sprite->height, 256, 256);
	glGenTextures(1, &sheet.textureID);
	int w = sprite->width;
	int h = sprite->height;
	glBindTexture(GL_TEXTURE_2D, sheet.textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0,
				GL_RGB, GL_UNSIGNED_BYTE, sprite->data);
	float verts[8][8];
	float texVerts[8][8];

	//get the coordinates from the spritesheet
	for(int i = 0;i < 8;i++) {
		std::pair<const float*, const float*> arrays = sheet.drawSprite(256*i, 0, i);
		for(int j = 0;j < 8;j++) {
			verts[i][j] = arrays.first[j];
			texVerts[i][j] = arrays.second[j];
		}
	}

}

*/
