#include <SFML/Network.hpp>
#include <iostream>
#include <random>
#include <chrono>
#include <thread>

struct character {
	bool running = false; 
	bool jump = false;
	bool idle = true;
	bool gravity = false;
	int direction = 1;
};

enum MoveType {
    RUN = 0,
    JUMP = 1,
    IDLE = 2,
    GRAVITY = 3,
};

int myID;

void updateCharacter(character& enemy, int moveType, int direction = 0) {
    if (moveType == RUN) {
        enemy.running = true;
        enemy.jump = false;
        enemy.idle = false;
        enemy.gravity = false;
        enemy.direction = direction; 
    } 
    else if (moveType == JUMP) {
        enemy.jump = true;
        enemy.running = false;
        enemy.idle = false;
        enemy.gravity = false;
        enemy.direction = direction; 
    } 
    else if (moveType == IDLE) {
        enemy.idle = true;
        enemy.running = false;
        enemy.jump = false;
        enemy.gravity = false;
        enemy.direction = 0;
    } 
    else if (moveType == GRAVITY) {
        enemy.gravity = true;
        enemy.idle = false;
        enemy.running = false;
        enemy.jump = false;
    } 
    else {
        std::cerr << "Unknown moveType: " << moveType << "\n";
    }

    std::cout << "Updated character state: "
              << "Running=" << enemy.running
              << ", Jump=" << enemy.jump
              << ", Idle=" << enemy.idle
              << ", Gravity=" << enemy.gravity
              << ", Direction=" << enemy.direction
              << "\n";
}


std::vector<character> characters;


auto type1 = [](int& id, int& players) {
    // type 1 has msg type, player
    myID = id;
    characters.resize(players);
};

auto type2 = [](int& enemyID, int& moveType, int& direction) {
    if (enemyID < 0 || enemyID >= characters.size()) {
        std::cerr << "Invalid enemyID: " << enemyID << "\n";
        return;
    }

    updateCharacter(characters[enemyID], moveType, direction);
    
};


auto type3 = []() {

};


auto connect_client_server = [](sf::TcpSocket& socket) {
    if (socket.connect("34.56.232.94", 8080) != sf::Socket::Status::Done) {
        std::cerr << "Error: Unable to connect to server\n";
        return 1;
    }
};




int main() {
    sf::TcpSocket socket;
    int prevState = 0;
    int currState = 0;
    // Connect to the server
    connect_client_server(socket);
    std::cout << "Connected to server\n";

    //block until server send signal

    socket.setBlocking(false);
    while(true) {
        {
            sf::Packet packet;
            if (socket.receive(packet) == sf::Socket::Done) {
                int msgType = 0;
                int id = 0;
                int move = 0;
                int direction = 0;
                packet >> msgType >> id >> move >> direction;

                if(msgType == 1) {
                    type1(id, move);
                }
                else if(msgType == 2) {
                    type2(id, move, direction);
                    std::cout << "type 2" << std::endl;
                }
            } 

            if(prevState != currState) {
                prevState = currState;
                sf::Uint32 msgType = 3;
                sf::Uint32 id = myID;
                sf::Uint32 move = currState;
                sf::Uint32 dir = characters[myID].direction;
                packet << msgType <<  id << move << dir;
                if(socket.send(packet) == sf::Socket::Done) {
                    std::cout << "succes sending packet" << std::endl;
                }
            }
        }

    }
    

    
    
    return 0;
}