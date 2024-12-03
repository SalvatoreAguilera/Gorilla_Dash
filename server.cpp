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
/*
Great! Let’s start with **local hosting** for your C++ project using SFML sockets. Here’s a step-by-step guide to get your server running locally:

---

### **1. Set Up Your SFML Project**  
Make sure your multiplayer project works locally on your computer:  

- **Run the server and client locally:**  
  - Start the server program (listens for incoming connections).  
  - Run the client program (connects to the server).  
  - Test their communication (e.g., sending and receiving data).  

**Checklist:**  
- Your server should bind to a port (e.g., `5000`).  
- Your client should connect to your server using `127.0.0.1` (localhost).  

---

### **2. Port Forwarding (Optional for LAN)**  
If you want other devices on your local network (like your phone or another computer) to connect:  

- **Find your local IP address:**  
  - Run `ipconfig` (Windows) or `ifconfig` (Linux/macOS).  
  - Look for your IPv4 address (e.g., `192.168.1.xxx`).  

- **Modify the client:**  
  Change the IP from `127.0.0.1` to your local IP in the client code.  

- **Connect from another device:**  
  Run the client on another device, pointing it to your server's local IP.

---

### **3. Extend Beyond LAN (Public Access)**  
To let people outside your local network connect, you’ll need **port forwarding**:  

1. **Access your router’s settings:**  
   Open your browser and navigate to your router’s IP (e.g., `192.168.1.1`).  

2. **Set up port forwarding:**  
   Forward a port (e.g., `5000`) to your server's local IP address.  

3. **Find your public IP:**  
   Visit [whatismyip.com](https://www.whatismyip.com) or similar.  

4. **Modify the client:**  
   Replace the local IP (`192.168.x.x`) in the client with your public IP.  

**Testing:**  
Ask someone outside your network to connect using your public IP and the forwarded port.

---

### **4. Debugging and Monitoring**  
- Use tools like **Wireshark** or **tcpdump** to monitor traffic and debug issues.  
- Handle basic errors, like the server not responding when a port is already in use.  

---

Let me know where you're at with this, and I can guide you further, including code tweaks or troubleshooting!
*/