#include "core/server.h"
#include "core/utils.h"
#include <thread>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Constructor to initialize port and mode with optional backend addresses
Server::Server(int port, ServerMode mode, const std::vector<std::string>& backend_addresses)
    : port(port), mode(mode), thread_pool(10), load_balancer(backend_addresses) {}


// Destructor
Server::~Server() {
    std::cout << "Shutting down CrabbyLB..." << std::endl;
}

// Start server based on selected mode
void Server::start() {
    switch (mode) {
        case ServerMode::BASIC:
            start_basic();
            break;
        case ServerMode::MULTI_THREAD:
            start_multi_threaded();
            break;
        case ServerMode::THREAD_POOL:
            start_thread_pool();
            break;
        case ServerMode::LOAD_BALANCER:
            start_load_balancer();
            break;
        default:
            std::cerr << "Invalid server mode!" << std::endl;
            exit(1);
    }
}

// Basic HTTP server (single-threaded)
void Server::start_basic() {
    int server_fd = create_listening_socket(port);
    std::cout << "🦾 Starting Basic HTTP Server on port " << port << "..." << std::endl;

    while (true) {
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        handle_request(client_socket);
    }
}

// Multi-threaded server (one thread per request)
void Server::start_multi_threaded() {
    int server_fd = create_listening_socket(port);
    std::cout << "🧵 Starting Multi-Threaded Server on port " << port << "..." << std::endl;

    while (true) {
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        std::thread request_thread(&Server::handle_request, this, client_socket);
        request_thread.detach();
    }
}

// ThreadPool-based server
void Server::start_thread_pool() {
    int server_fd = create_listening_socket(port);
    std::cout << "⚡️ Starting ThreadPool-Based Server on port " << port << "..." << std::endl;

    while (true) {
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        thread_pool.enqueue_task([this, client_socket] {
            handle_request(client_socket);
        });
    }
}

// Load Balancer with Health Checks and Auto-Restart
void Server::start_load_balancer() {
    int server_fd = create_listening_socket(port);
    std::cout << "🌐 Starting Load Balancer with Health Checks on port " << port << "..." << std::endl;

    while (true) {
        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        std::thread request_thread(&Server::handle_request, this, client_socket);
        request_thread.detach();
    }
}

// Handle incoming HTTP requests
// Handle incoming HTTP requests
void Server::handle_request(int client_socket) {
    std::string request_data = read_data(client_socket);
    if (request_data.empty()) {
        close(client_socket);
        return;
    }

    Request request(request_data);

    if (mode == ServerMode::LOAD_BALANCER) {
        forward_request_to_backend(client_socket, request);
    } else {
        process_request(client_socket, request);
    }
}

// Process request and generate appropriate response
void Server::process_request(int client_socket, const Request& request) {
    std::string response_body;
    int status_code = 200;

    if (request.get_path() == "/") {
        response_body = "<h1>Welcome to CrabbyLB!</h1>";
    } else if (request.get_path() == "/health") {
        response_body = "OK";
    } else {
        status_code = 404;
        response_body = "<h1>404 Not Found</h1>";
    }

    Response response(status_code);
    response.add_header("Content-Type", "text/html");
    response.set_body(response_body);

    std::string final_response = response.build_response();
    send_data(client_socket, final_response);
    close(client_socket);
}


// Forward request to backend and send response to client
void Server::forward_request_to_backend(int client_socket, const Request& request) {
    try {
        Backend backend = load_balancer.get_next_backend();  // Get next backend
        std::cout << "🔄 Routing request to backend: " << backend.address << "\n";

        std::string backend_ip = backend.address.substr(0, backend.address.find(":"));
        int backend_port = std::stoi(backend.address.substr(backend.address.find(":") + 1));

        int backend_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (backend_socket < 0) {
            perror("Backend socket creation failed");
            load_balancer.mark_backend_down(backend.address);
            close(client_socket);
            return;
        }

        struct sockaddr_in backend_address;
        backend_address.sin_family = AF_INET;
        backend_address.sin_port = htons(backend_port);
        inet_pton(AF_INET, backend_ip.c_str(), &backend_address.sin_addr);

        // Connect to the backend
        if (connect(backend_socket, (struct sockaddr*)&backend_address, sizeof(backend_address)) < 0) {
            perror("Connection to backend server failed");
            load_balancer.mark_backend_down(backend.address);
            close(backend_socket);
            close(client_socket);
            return;
        }

        // Forward request to backend
        std::string raw_request = request.get_raw_request();
        send(backend_socket, raw_request.c_str(), raw_request.length(), 0);

        // Receive response from backend
        char response_buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(backend_socket, response_buffer, sizeof(response_buffer))) > 0) {
            send(client_socket, response_buffer, bytes_read, 0);
        }

        close(backend_socket);
    } catch (const std::runtime_error& e) {
        std::cerr << "⚠️ Error forwarding request: " << e.what() << "\n";
        send_data(client_socket, "HTTP/1.1 503 Service Unavailable\r\nContent-Length: 0\r\n\r\n");
    }

    close(client_socket);
}