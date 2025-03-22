#include "core/server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>


Server::Server(int port) : port(port) {}

void Server::start() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create a socket (IPv4, TCP)
    // This function is being used because it creates a socket that will be used to listen for incoming connections.
    // The socket is configured for IPv4 and TCP, which is suitable for HTTP communication.
    if ((server_fd  = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket Failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address settings
    // This sets up the server address structure with the necessary parameters.
    // The server will accept connections from any IP address and listen on the specified port.
    address.sin_family = AF_INET;         // IPV4
    address.sin_addr.s_addr = INADDR_ANY; // Accepts connects from any IP
    address.sin_port = htons(port);       // Convert port to network byte order

    // Bind socket to the specified port
    // This function binds the socket to the address and port specified in the address structure.
    // This is necessary to ensure that the server can listen for incoming connections on the correct port.
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind Failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    // This function puts the server socket into a passive mode where it waits for incoming connection requests.
    // The backlog parameter (3) specifies the maximum length for the queue of pending connections.
    if (listen(server_fd, 3) < 0) {
        perror("Listen Failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "CrabbyLB listening on port " << port << std::endl;

    // Main server loop to accept and handle incoming requests
    // This loop continuously accepts incoming connections and handles them.
    // It ensures that the server can handle multiple client requests sequentially.
    while(true){
        // Accept an incoming connection
        // This function accepts a connection from a client.
        // It creates a new socket for the connection and returns a file descriptor for the new socket.
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept Failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "New Connection accepted! Spawing thread to handle request." << std::endl;

        std::thread request_thread(&Server::handle_request, this, new_socket);
        request_thread.detach();
    }
}

// Handle incoming HTTP requests and send basic response 
// This function serves the purpose of processing incoming HTTP requests and sending back a basic HTTP response.
// It reads the request from the client socket, prints it to the console, and sends a simple HTML response.
void Server::handle_request(int client_socket){
    char buffer[1024] = {0};                         // Buffer to store incoming request
    int valread = read(client_socket, buffer, 1024); // Read request from client


    if(valread < 0){
        perror("Failed to read from socket");
        close(client_socket);
        return;
    }

    // Print the request to the console
    // std::cout << "Received Request:\n" << buffer << std::endl;

    Request request(buffer); // Parse the request

    // Log the incoming request
    std::cout << "\n\nIncoming Request: " << std::endl;
    std::cout << "Request Path: " << request.get_path() << std::endl; 
    std::cout << "Request Method: " << request.get_method() << std::endl;
    std::cout << "Query Parameters: " <<  request.get_query_params().size() << std::endl;
    for (auto const& [key, val] : request.get_query_params()) {
        std::cout << "PARAM" << key << ": " << val << std::endl;
    }

    Response response(200); // Create a response with status code 200 (OK)
    response.add_header("Content-Type", "text/html"); // Add a content type header

    if (request.get_path() == "/") {
        response.set_body("<h1>Hello, CrabbyLB!</h1>"); 
    } else if (request.get_path() == "/api/data") {
        // Get all query parameters
        std::map<std::string, std::string> query_params = request.get_query_params();

        // Construct a string with all query parameters
        std::string query_string = "<h1>Query Parameters found:</h1>";
        for (auto const& [key, val] : query_params) {
            query_string += "<p>" + key + ": " + val + "</p>";
        }

        if (query_params.size() == 0) {
            Response response(400); // Set response status code to 400 (Bad Request)
            response.set_body("<h1>400 Bad Request</h1><h2>No query parameters found.</h2>"); 
        } else{
            response.set_body(query_string); 
        }
    }
    
    
    else {
        response = Response(404); // Set response status code to 404 (Not Found)
        response.set_body("<h1>404 Not Found</h1>"); 
    }

    std::string final_response = response.build_response(); 
    send(client_socket, final_response.c_str(), final_response.length(), 0); 

    close(client_socket); 
}