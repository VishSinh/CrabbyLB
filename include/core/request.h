#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <map>


class Request {
public:
    Request(const std::string& raw_request);

    std::string get_method() const;
    std::string get_path() const;
    std::string get_header(const std::string& key) const;
    std::string get_query_param(const std::string& key) const;
    std::map<std::string, std::string> get_query_params() const;

private:
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> query_params;

    void parse_headers(const std::string& header_lines);
    void parse_query_params(const std::string& query_string);
};

#endif