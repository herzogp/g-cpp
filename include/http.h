#pragma once

#include <string>
#include <map>
#include <vector>

typedef std::pair<int, std::string> ReasonPair;
typedef std::map<int, std::string> ReasonMap;
typedef ReasonMap::iterator ReasonIterator;

class HttpStatusReasons {
    ReasonMap reasons;
  public:
    HttpStatusReasons();
    std::string lookup(int status);
};

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

class HttpResponse {
  int status;
  std::string content_type;
  std::string content;

  HttpStatusReasons *p_reasons;

  public:
    HttpResponse(int status, std::string content_type, std::string content, HttpStatusReasons *p_reasons);
    void send_message(int socket);
};
