#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>
#include <vector>

#include <anthill/status_reasons.h>
#include "http_request.h"
#include "http_response.h"
#include "request_handler.h"

class HttpServer {
    int server_socket;
    int port;
    struct sockaddr_in address;
    std::vector<RequestHandler> handlers;
    HttpStatusReasons& all_reasons;
    void * app_context;

  public:
    std::string error_text;
    
    HttpServer(int port);
    bool not_useable();
    bool is_useable();
    void close_listening_socket();
    int accept();
    void set_handlers(std::vector<RequestHandler> handlers);
    bool serve(int child_socket);
    void *get_app_context();
    void set_app_context(void *context);

  private:
    bool dispatch_request(int child_socket, HttpRequest& req);
};
