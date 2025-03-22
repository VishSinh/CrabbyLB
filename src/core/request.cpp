#include "core/request.h"
#include <sstream>
#include <algorithm>


Request::Request(const std::string& raw_request){
    std::istringstream request_stream(raw_request);
    std::string request_line;

    getline(request_stream, request_line);

    // Parse the request line (e.g. "GET / HTTP/1.1")
    std::istringstream request_line_stream(request_line); // Create an input string stream from the request_line string. This allows us to treat the string like an input stream, similar to std::cin or a file stream.
    request_line_stream >> method >> path; // Extract the method and path from the request line
    // The >> operator, when used with an istringstream, reads whitespace-separated values from the stream.
    // In this case, it first reads the "method" (e.g., "GET") and stores it in the 'method' variable.
    // Then, it reads the "path" (e.g., "/") and stores it in the 'path' variable.
    // Any subsequent parts of the request line (like "HTTP/1.1") are ignored by this code.

    // Check if the path contains query parameters and parse them
    size_t query_pos = path.find('?');
    if (query_pos != std::string::npos) { // If the query string is found
        std::string query_string = path.substr(query_pos + 1); // Extract the query string from the path
        path = path.substr(0, query_pos);                      // Update the path to remove the query string
        parse_query_params(query_string);
    }

    // Parse the headers
    std::string header_lines;
    while (getline(request_stream, header_lines) && header_lines != "\r") {
        parse_headers(header_lines);
    }
}

std::string Request::get_method() const {
    return method;
}

std::string Request::get_path() const {
    return path;
}

// Header retrieval (e.g., Host, User-Agent)
// Note:  Case-insensitive header retrieval
std::string Request::get_header(const std::string& key) const {
    std::string lower_ley = key;
    std::transform(lower_ley.begin(), lower_ley.end(), lower_ley.begin(), ::tolower);

    auto it = headers.find(lower_ley);
    if (it != headers.end()) {
        return it->second;
    }

        return "";
}

// Query parameter retrieval (e.g., /path?key=value)
// Note: This function is case-sensitive
std::string Request::get_query_param(const std::string& key) const {
    auto it = query_params.find(key);
    if (it != query_params.end()) {
        return it->second;
    }

    return "";
}

std::map<std::string, std::string> Request::get_query_params() const {
    return query_params;
}


// Parse headers from the header lines (e.g., "Host: localhost:8080")
void Request::parse_headers(const std::string& header_lines) {
    // Find the position of the colon which separates the header key and value
    size_t colon_pos = header_lines.find(':');
    if (colon_pos != std::string::npos) {
        // Extract the key and value from the header line
        std::string key = header_lines.substr(0, colon_pos);
        std::string value = header_lines.substr(colon_pos + 2); // Skip the colon and the space

        // Convert the key to lowercase to ensure case-insensitive header storage
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);

        // Store the header key-value pair in the map
        headers[key] = value;
    }
}

// Parse query parameters from the query string (e.g., key1=value1&key2=value2)
void Request::parse_query_params(const std::string& query_string) {
    std::istringstream query_stream(query_string); // Create an input string stream from the query string
    std::string query_param;                       // Declare a string to hold each query parameter

    // Split the query string into individual parameters using '&' as the delimiter
    while (getline(query_stream, query_param, '&')) {
        size_t equal_pos = query_param.find('=');  // Find the position of the '=' sign in the query parameter
        if (equal_pos != std::string::npos) {      // If the '=' sign is found
            std::string key = query_param.substr(0, equal_pos);      // Extract the key from the beginning of the parameter up to the '=' sign
            std::string value = query_param.substr(equal_pos + 1);   // Extract the value from after the '=' sign to the end of the parameter

            query_params[key] = value; // Store the key-value pair in the query_params map
        }
    }
}