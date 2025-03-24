#ifndef SERVER_H
#define SERVER_H

#include "core/request.h"
#include "core/response.h"
#include "core/thread_pool.h"

class Server {
public:
    Server(int port, size_t num_threads = 4);
    void start();
private:
    int port;
    ThreadPool thread_pool;

    // Handle incoming HTTP requests and send basic response
    void handle_request(int client_socket);
};

#endif