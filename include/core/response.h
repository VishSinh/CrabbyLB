#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <map>


class Response {
public:
    Response(int status_code);

    void set_body(const std::string& body);

    void add_header(const std::string& key, const std::string& value);

    std::string build_response() const;

private:
    int status_code;
    std::string body;
    std::map<std::string, std::string> headers;

    std::string get_status_message() const;
};

#endif