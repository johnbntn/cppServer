#include <sys/socket.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

class Server{
public:
    uint32_t port;
    Server() {
        port = 8080;
    };

    int run() {
        struct sockaddr_in server_addr, client_addr;
        uint32_t servSock, clientSock;

        //create server socket
        if ((servSock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            std::cerr << "Failed to create socket" << std::endl;
            return 1;
        };

        //config server socket.
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        //Bind server socket to port and listen on it
        int status = bind(servSock, (const struct sockaddr *) &server_addr, sizeof(server_addr));
        if (status == -1) {
            std::cerr << "Failed to bind" << std::endl;
            return 1;
        };

        if ((listen(servSock, 8)) == -1) {
            std::cerr << "Failed to listen" << std::endl;
            return 1;
        };

        std::cout << "Listening on port " << port << std::endl;

        //Accept clients and handle them
        while (true) {
            socklen_t clientlen = sizeof(client_addr);

            //accept connections
            clientSock = accept(servSock, (struct sockaddr *) &client_addr, &clientlen);
            if (clientSock == -1){
                continue;
            };

            handleClient(clientSock);
        }

        //close when no longer connecting
        close(servSock);
        return 0;
    }
private:
    int handleClient(uint32_t clientSock){
        char buffer[1024] = {0};
        ssize_t bytesRead = read(clientSock, buffer, 1024);
        
        //check if bytes were read
        if (bytesRead > 0) {
            std::cout << "Received request: " << buffer << std::endl;
            
            //Create + send simple HTTP response
            std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!\n";
            send(clientSock, response.c_str(), response.length(), 0);
        }
        else{return 1;}

        close(clientSock);
        return 0;
    }
};

int main(){
    Server s;
    s.run();
    return 0;
}