#ifndef SERVER_H
#define SERVER_H

#include "core/request.h"
#include "core/response.h"

class Server {
public:
    Server(int port);
    void start();
private:
    int port;
    void handle_request(int client_socket);
};

#endif