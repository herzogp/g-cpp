#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include "netbuf.h"

class HttpRequest {

  public:
    HttpRequest(int socket);
    void show();
    std::string& get_method();
    std::string& get_path();
    std::string& get_protocol();
    std::string& get_body();
    std::vector<std::string>& get_headers();
    void read_formdata(std::map<std::string, std::string> & formdata_map);

  public:
    std::unique_ptr<std::basic_iostream<char>> p_strm;

  private:
    void read_headers();

  private:
    int client_socket;
    std::string method;
    std::string path;
    std::string protocol;
    std::string body;
    std::vector<std::string> headers;
    std::unique_ptr<Netbuf> p_netbuf;
};
