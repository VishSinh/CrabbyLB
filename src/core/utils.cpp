#include "core/utils.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

// Create a listening socket
int create_listening_socket(int port) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket (IPv4, TCP)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Reuse address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Configure address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

// Send data over a socket
void send_data(int socket, const std::string& data) {
    send(socket, data.c_str(), data.length(), 0);
}

// Read data from a socket
std::string read_data(int socket) {
    char buffer[1024] = {0};
    int valread = read(socket, buffer, 1024);
    if (valread < 0) {
        perror("Failed to read from socket");
        return "";
    }
    return std::string(buffer, valread);
}
