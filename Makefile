background: background.cpp sgutierrez2.cpp rmoreno2.cpp 
	g++ background.cpp sgutierrez2.cpp rmoreno2.cpp -Wall -lX11 -lGL -lGLU -lm -o background

clean:
	rm -f background 