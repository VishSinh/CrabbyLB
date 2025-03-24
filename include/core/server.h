#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <mutex>
#include "core/thread_pool.h"
#include "core/load_balancer.h"
#include "core/request.h"
#include "core/response.h"

// Define server modes
enum class ServerMode {
    BASIC,
    MULTI_THREAD,
    THREAD_POOL,
    LOAD_BALANCER
};

class Server {
public:
    Server(int port, ServerMode mode, const std::vector<std::string>& backends = {});
    ~Server();

    // Start server based on selected mode
    void start();

private:
    int port;
    ServerMode mode;
    ThreadPool thread_pool;
    LoadBalancer load_balancer;

    // Core server logic
    void start_basic();
    void start_multi_threaded();
    void start_thread_pool();
    void start_load_balancer();

    // Handle incoming requests
    void handle_request(int client_socket);

    // Process request and generate response
    void process_request(int client_socket, const Request& request);

    // Forward request to backend and send response
    void forward_request_to_backend(int client_socket, const Request& request);
};

#endif
