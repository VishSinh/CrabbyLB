#include <iostream>
#include "core/server.h"

int main() {
    std::cout << "Starting CrabbyLB on port 8080..." << std::endl;

    Server server(8080, 12);

    server.start();

    return 0;
}