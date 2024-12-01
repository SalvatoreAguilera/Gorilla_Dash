all: background server client

background: background.cpp sgutierrez2.cpp rmoreno2.cpp brodriguez.cpp
	g++ background.cpp sgutierrez2.cpp rmoreno2.cpp brodriguez.cpp -Wall -lsfml-network -lsfml-system -lX11 -lGL -lGLU -lm -o background

server: server.cpp
	g++ server.cpp -Wall -lsfml-network -lsfml-system -o server

client: client.cpp
	g++ client.cpp -Wall -lsfml-network -lsfml-system -o client

clean:
	rm -f background client server
