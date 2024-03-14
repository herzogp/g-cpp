#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

class HttpRequest {
    int client_socket;
    std::string method;
    std::string path;
    std::string protocol;
    std::string body;
    std::vector<std::string> headers;
    std::unique_ptr<std::basic_iostream<char>> p_strm;

  public:
    HttpRequest(int socket);
    void show();
    std::string& get_method();
    std::string& get_path();
    std::string& get_protocol();
    std::string& get_body();
    std::vector<std::string>& get_headers();

  private:
    void read_headers();
};
