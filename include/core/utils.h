#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <netinet/in.h>

// Create a listening socket
int create_listening_socket(int port);

// Send data over a socket
void send_data(int socket, const std::string& data);

// Read data from a socket
std::string read_data(int socket);

#endif
