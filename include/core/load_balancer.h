#ifndef LOAD_BALANCER_H
#define LOAD_BALANCER_H

#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>

struct Backend {
    std::string address; // IP:PORT of the backend server
    int active_connections; // Number of active connections to the backend
    bool is_alive; // Flag to indicate if the backend is alive
};

class LoadBalancer {
public:
    LoadBalancer(const std::vector<std::string>& backend_addresses);

    // Get the next available backend server
    Backend get_next_backend();

    // Mark a backend as unavailable
    void mark_backend_down(const std::string& address);

    // Check backend health periodically
    void start_health_check();
    void stop_health_check();

private:
    std::vector<Backend> backends;
    std::mutex backend_mutex;
    int current_backend_index; 

    // Health check thread
    std::thread health_check_thread;
    std::atomic<bool> stop_checking;

    // Health check function
    void perform_health_check();
};

#endif