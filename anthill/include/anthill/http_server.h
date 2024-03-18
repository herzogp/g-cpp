#pragma once

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <string>
#include <thread>
#include <vector>

#include <anthill/status_reasons.h>
#include "http_request.h"
#include "http_response.h"
#include "request_handler.h"
#include "client_queue.h"

class HttpServer {
    int server_socket;
    int port;
    int num_threads;
    struct sockaddr_in address;
    std::vector<RequestHandler> handlers;
    HttpStatusReasons& all_reasons;
    void * app_context;

    ClientQueue client_que;
    std::vector<std::thread> service_threads; // (NUM_THREADS);
    // ClientQueue& cq_ref = client_que;

  public:
    std::string error_text;
    
    HttpServer(int port, int num_threads);
    bool not_useable();
    bool is_useable();
    void close_listening_socket();
    void start_services();
    int accept();
    void set_handlers(std::vector<RequestHandler> handlers);
    void *get_app_context();
    void set_app_context(void *context);
    void shutdown();

  private:
    void service();
    bool serve(int child_socket);
    bool dispatch_request(int child_socket, HttpRequest& req);
};
