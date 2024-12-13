#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <omp.h>

#define CLIENTS 2

struct PlayerState {
    sf::Uint32 msg = 0;
    sf::Uint32 id = 0;
    sf::Uint32 move = 0;
    sf::Uint32 direction = 0;
};

enum MoveType {
    IDLE = 0,
    JUMP = 1,
    RUN = 2,
    GRAVITY = 3,
    DEAD = 4
};



void sendStates(sf::Packet& packet, std::vector<sf::TcpSocket>& clients);
void sendStartSignal(std::vector<sf::TcpSocket>& clients);

int main() {
    sf::TcpListener listener;
    sf::SocketSelector selector;
    std::vector<sf::TcpSocket> clients(CLIENTS);
    std::vector<int> playerIDs(CLIENTS); 

    if (listener.listen(8080) != sf::Socket::Done) {
        std::cerr << "Error: Unable to bind to port 8080\n";
        return 1;
    }

    std::cout << "Server is listening on port 8080...\n";

    int currClients = 0;
    while (currClients < CLIENTS) {
        if (listener.accept(clients[currClients]) != sf::Socket::Done) {
            std::cerr << "Error: Unable to accept connection\n";
            return 1;
        }

        playerIDs[currClients] = currClients;
        selector.add(clients[currClients]);

        sf::Packet idPacket;
        sf::Uint32 msgType = 1;
        sf::Uint32 id = currClients;
        sf::Uint32 players = CLIENTS;
        sf::Uint32 direction = 1;
        idPacket << msgType << id << players << direction;
        if (clients[currClients].send(idPacket) != sf::Socket::Done) {
            std::cerr << "Error: Unable to send player ID to client " << currClients + 1 << "\n";
            return 1;
        }

        std::cout << "Client " << currClients + 1 << " connected with ID " << playerIDs[currClients] << std::endl;
        currClients++;
    }
    
    sendStartSignal(clients);

    while (true) {
        if (selector.wait(sf::milliseconds(1))) {
            //#pragma omp parallel for
            for (int i = 0; i < CLIENTS; i++) {
                if (selector.isReady(clients[i])) {
                    sf::Packet packet;
                    if (clients[i].receive(packet) == sf::Socket::Done) {
                        sendStates(packet, clients);
                    }
                }
            }
        }
    }

    return 0;
}

void sendStates(sf::Packet& packet, std::vector<sf::TcpSocket>& clients) {
    sf::Uint32 msgType;
    sf::Uint32 id;
    sf::Uint32 move;
    sf::Uint32 direction;
    packet >> msgType >> id >> move >> direction;
    sf::Packet outPacket;
    msgType = 2;
    outPacket << msgType << id << move << direction;
    #pragma omp parallel for
    for (int i = 0; i < CLIENTS; i++) {
        if (i != (int)id) {
            if (clients[i].send(outPacket) != sf::Socket::Done) {
                #pragma omp critical
                std::cerr << "Error sending data to client " << i + 1 << "\n";
            }
        }
    }
}


void sendStartSignal(std::vector<sf::TcpSocket>& clients) {
    #pragma omp parallel for
    for (int i = 0; i < CLIENTS; i++) {
        sf::Packet startPacket;
        sf::Uint32 msgType = 4; 
        startPacket << msgType;
        if (clients[i].send(startPacket) != sf::Socket::Done) {
            std::cerr << "Error sending start signal to client " << i + 1 << "\n";
        }
    }
}
