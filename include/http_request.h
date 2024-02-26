#pragma once

#include <string>
#include <vector>

class HttpRequest {
    std::string method;
    std::string path;
    std::string protocol;
    std::string body;
    std::vector<std::string> headers;

  public:
    HttpRequest(std::string req);
    void show();
    std::string& get_method();
    std::string& get_path();
    std::string& get_protocol();
    std::string& get_body();
    std::vector<std::string>& get_headers();
};

