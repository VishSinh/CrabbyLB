#include "core/server.h"
#include <iostream>
#include <string>
#include <vector>

// Helper function to parse backend addresses
std::vector<std::string> parse_backend_addresses(int argc, char* argv[]) {
    std::vector<std::string> backend_addresses;
    for (int i = 2; i < argc; ++i) {
        backend_addresses.push_back(argv[i]);
    }
    return backend_addresses;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./crabbyLB <mode> [backend_addresses]\n";
        std::cerr << "Modes: basic, multi_thread, thread_pool, load_balancer\n";
        return 1;
    }

    std::string mode_str = argv[1];
    ServerMode mode;

    std::cout << "Mode: " << mode_str << std::endl;

    if (mode_str == "basic") {
        mode = ServerMode::BASIC;
    } else if (mode_str == "multi_thread") {
        mode = ServerMode::MULTI_THREAD;
    } else if (mode_str == "thread_pool") {
        mode = ServerMode::THREAD_POOL;
    } else if (mode_str == "load_balancer") {
        mode = ServerMode::LOAD_BALANCER;
    } else {
        std::cerr << "Invalid mode specified! Use: basic, multi_thread, thread_pool, load_balancer\n";
        return 1;
    }

    // ✅ Parse backend addresses if mode is load_balancer
    std::vector<std::string> backend_addresses;
    if (mode == ServerMode::LOAD_BALANCER) {
        if (argc < 3) {
            std::cerr << "❗️ For load_balancer mode, specify at least one backend address.\n";
            return 1;
        }
        backend_addresses = parse_backend_addresses(argc, argv);
        std::cout << "Backend Addresses: ";
        for (const auto& address : backend_addresses) {
            std::cout << address << " ";
        }
        std::cout << std::endl;
    }

    // ✅ Create server instance with selected mode and backend addresses
    Server server(8080, mode, backend_addresses);
    server.start();

    return 0;
}
