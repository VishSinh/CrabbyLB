#include "core/load_balancer.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

LoadBalancer::LoadBalancer(const std::vector<std::string>& backend_addresses) 
    : current_backend_index(0) {
    for (const auto& address : backend_addresses) {
        backends.push_back({address, 0, true}); // Initialize all backends as alive with 0 active connections
    }

    // Start the health check thread
    start_health_check();
}

// Get the next available backend server
Backend LoadBalancer::get_next_backend() {
    // Lock the mutex to access the backend list
    std::lock_guard<std::mutex> lock(backend_mutex);

    // Find the next available backend server
    for (size_t i = 0; i < backends.size(); ++i) {
        Backend& backend = backends[current_backend_index];
        current_backend_index = (current_backend_index + 1) % backends.size(); // Move to the next backend for the next request

        if (backend.is_alive) {
            backend.active_connections++; // Increment active connections to the backend
            return backend;
        }

    }

    // If no backend is available, throw an exception
    throw std::runtime_error("No available backend servers");
}

// Mark a backend server as unavailable
void LoadBalancer::mark_backend_down(const std::string& address) {
    // Lock the mutex to access the backend list
    std::lock_guard<std::mutex> lock(backend_mutex);

    // Find the backend with the specified address
    for (Backend& backend : backends) {
        if (backend.address == address) {
            backend.is_alive = false; // Mark the backend as unavailable
            std::cout << "Backend " << address << " marked as DOWN" << std::endl;
            return;
        }
    }

    // If the backend is not found, throw an exception
    throw std::runtime_error("Backend server not found. Cannot mark as DOWN");
}

void LoadBalancer::start_health_check() {
    // Start the health check thread
    health_check_thread = std::thread([this]{
        while (!stop_checking.load()) {
            perform_health_check(); // Perform health check periodically
            std::this_thread::sleep_for(std::chrono::seconds(5)); // Check every 5 seconds
        }
    });
}

void LoadBalancer::stop_health_check() {
    // Stop the health check thread
    stop_checking.store(true);
    if (health_check_thread.joinable()) {
        health_check_thread.join();
    }
}

void LoadBalancer::perform_health_check() {
    // Lock the mutex to access the backend list
    std::lock_guard<std::mutex> lock(backend_mutex);

    for (Backend& backend : backends) {

        int health_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (health_socket < 0) {
            perror("Failed to create socket for health check");
            return;
        }

        struct sockaddr_in backend_address;
        backend_address.sin_family = AF_INET;

        // Extract IP and port from the backend address
        std::string backend_ip = backend.address.substr(0, backend.address.find(':'));
        int backend_port = std::stoi(backend.address.substr(backend.address.find(':') + 1));

        backend_address.sin_port = htons(backend_port);
        inet_pton(AF_INET, backend_ip.c_str(), &backend_address.sin_addr);

        std::cout << "Performing health check for backend: " << backend.address << std::endl;

        // Connect to the backend server for health check
        if (connect(health_socket, (struct sockaddr*)&backend_address, sizeof(backend_address)) < 0) {
            std::cout << "Backend " << backend.address << " is DOWN" << std::endl;
            backend.is_alive = false; // Mark the backend as unavailable
            close(health_socket);
            continue;
        }

        // Send GET /health HTTP request to check backend health
        std::string health_request = "GET /health HTTP/1.1\r\nHost: " + backend.address + "\r\n\r\n";
        if(send(health_socket, health_request.c_str(), health_request.size(), 0) < 0) {
            perror("Failed to send health check request");
            close(health_socket);
            continue;
        }

        char buffer[1024] = {0};
        ssize_t bytes_read = read(health_socket, buffer, sizeof(buffer));

        // Check if the response contains "200 OK" to mark the backend as alive
        if (strstr(buffer, "200 OK") != nullptr) {
            // If the Backend was previously marked as down, mark it as alive
            if(!backend.is_alive) {
                std::cout << "Backend " << backend.address << " is UP again!" << std::endl;
            }
            backend.is_alive = true;
        } else {
            // If the Backend was previously marked as alive, mark it as down
            if(backend.is_alive) {  
                std::cout << "Backend " << backend.address << " failed haelth check" << std::endl;
                backend.is_alive = false;
            }
        }

        close(health_socket);
    }
}

