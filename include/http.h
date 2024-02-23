#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <map>
#include <string>
#include <vector>

class HttpRequest;
class HttpResponse;

typedef HttpResponse * (*RequestHandlerFunc)(HttpRequest& req);

struct RequestHandler {
    std::string method;
    std::string path;
    RequestHandlerFunc func;
    static RequestHandlerFunc not_found_func;

    RequestHandler(std::string method, std::string path, RequestHandlerFunc func);
};

class HttpServer {
    int server_socket;
    int port;
    struct sockaddr_in address;
    std::vector<RequestHandler> handlers;

  public:
    std::string error_text;
    
    HttpServer(int port);
    bool not_useable();
    bool is_useable();
    void close_listening_socket();
    int accept();
    void set_handlers(std::vector<RequestHandler> handlers);
    std::string& serve(int child_socket);

  private:
    void dispatch_request(int child_socket, HttpRequest& req);
};

typedef std::pair<int, std::string> ReasonPair;
typedef std::map<int, std::string> ReasonMap;
typedef ReasonMap::iterator ReasonIterator;

class HttpStatusReasons {
    ReasonMap reasons;
  public:
    HttpStatusReasons();
    ~HttpStatusReasons();
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
    HttpResponse(int status, std::string content_type, std::string content);
    void send_message(int socket);
};
