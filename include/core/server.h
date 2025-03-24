#ifndef SERVER_H
#define SERVER_H

#include "core/request.h"
#include "core/response.h"
#include "core/thread_pool.h"
#include "core/load_balancer.h"

class Server {
public:
    // Server(int port, size_t num_threads = 4);
    Server(int port, const std::vector<std::string> &backend_addresses);


    void start();

private:
    int port;
    // ThreadPool thread_pool;
    LoadBalancer load_balancer;

    // Handle incoming HTTP requests and send basic response
    void handle_request(int client_socket);
};

#endif