#include <iostream>
#include "core/server.h"

int main() {
    
    std::vector<std::string> backend_addresses = {
        "127.0.0.1:8081",  // Backend 1
        "127.0.0.1:8082",  // Backend 2
        "127.0.0.1:8083"   // Backend 3
    };
    
    Server server(8080, backend_addresses);
    
    std::cout << "Starting CrabbyLB on port 8080..." << std::endl;
    server.start();

    return 0;
}