background: background.cpp Sprite.cpp 
	g++ background.cpp Sprite.cpp -Wall -lX11 -lGL -lGLU -lm

clean:
	rm -f background 