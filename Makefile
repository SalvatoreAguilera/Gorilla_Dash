background: background.cpp sgutierrez2.cpp rmoreno2.cpp brodriguez.cpp
	g++ background.cpp sgutierrez2.cpp rmoreno2.cpp brodriguez.cpp libggfonts.a -Wall -lX11 -lGL -lGLU -lm -o background

clean:
	rm -f background 
