#include "core/response.h"

Response::Response(int status_code) : status_code(status_code) {}

void Response::set_body(const std::string& body_content) {
    body = body_content;
}

void Response::add_header(const std::string& key, const std::string& value) {
    headers[key] = value;
}

std::string Response::build_response() const {
    std::string response;

    response += "HTTP/1.1 " + std::to_string(status_code) + " " + get_status_message() + "\r\n";

    for (const auto& header : headers) {
        response += header.first + ": " + header.second + "\r\n";
    }

    response += "\r\n";

    response += body;

    return response;
}

std::string Response::get_status_message() const {
    switch (status_code) {
        case 200: return "OK";
        case 404: return "Not Found";
        case 500: return "Internal Server Error";
        default: return "Not Implemented";
    }
}