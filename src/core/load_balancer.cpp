#include "core/load_balancer.h"
#include <iostream>


LoadBalancer::LoadBalancer(const std::vector<std::string>& backend_addresses) 
    : current_backend_index(0) {
    for (const auto& address : backend_addresses) {
        backends.push_back({address, 0, true}); // Initialize all backends as alive with 0 active connections
    }
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

// Health check function to periodically check the status of backend servers
void LoadBalancer::health_check() {
    // Lock the mutex to access the backend list
    std::lock_guard<std::mutex> lock(backend_mutex);

    // Perform health check on all backend servers
    for (Backend& backend : backends) {
        // Simulate health check by checking if the backend has active connections
        if (backend.active_connections > 0) {
            backend.is_alive = true; // Mark the backend as alive
            backend.active_connections = 0; // Reset active connections
            std::cout << "Backend " << backend.address << " is UP" << std::endl;
        } else {
            backend.is_alive = false; // Mark the backend as down
            std::cout << "Backend " << backend.address << " is DOWN" << std::endl;
        }
    }
}

