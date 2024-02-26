#include "http_server.h"

#include <cstdio>
#include <cstring> //required_by: memset
#include <iostream>
#include <memory>
#include <ostream>
#include <sys/socket.h>
#include <unistd.h>

HttpServer::HttpServer(int port) : 
  port(port), 
  all_reasons(HttpStatusReasons::get_reasons()) {

    this->server_socket = socket(AF_INET, SOCK_STREAM |SOCK_CLOEXEC, 0);
    if (this->server_socket == (-1)) {
      this->error_text = "Creating a socket";
      return;
    }

    struct sockaddr *p_address =  (struct sockaddr *)&this->address;
    int len_address = sizeof(this->address);
   
    memset(&this->address, '\0', len_address);
    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(this->port);
  
    const int opt = 1;
    setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // bind socket to address:port and listen for connections
    if (bind(this->server_socket, p_address, len_address) < 0) {
      this->error_text = "Trying to bind()";
      ::close(this->server_socket);
      this->server_socket = -1;
      return;
    }
    if (listen(server_socket, 10) < 0) {
      this->error_text = "Trying to listen()";
      ::close(this->server_socket);
      this->server_socket = -1;
      return;
    }
}


bool
HttpServer::not_useable() {
  return (server_socket < 0);
}

bool
HttpServer::is_useable() {
  return (server_socket >= 0);
}

void
HttpServer::close_listening_socket() {
  if (this->not_useable()) {
    return;
  }
  int rc = ::close(this->server_socket);
  if (rc == -1) {
    this->error_text = "unable to close server socket";
  }
  this->server_socket = -1;
}

void 
HttpServer::set_handlers(std::vector<RequestHandler> handlers) {
  this->handlers = handlers;
}

int
HttpServer::accept() {
  if (this->not_useable()) {
    return -1;
  }

  struct sockaddr *p_address =  (struct sockaddr *)&this->address;
  int len_address = sizeof(this->address);
  int client_socket = ::accept(this->server_socket, p_address, (socklen_t *)&len_address);
  return client_socket;
}

void
HttpServer::dispatch_request(int child_socket, HttpRequest& req) {
    std::string& method = req.get_method();
    std::string& path = req.get_path();

    for (const auto &handler : this->handlers) { // Type inference by const reference.
      bool path_matches = (handler.path == path);
      bool method_matches = (handler.method == method);
      if (path_matches && method_matches) {
        std::unique_ptr<HttpResponse> rsp(handler.func(req));
        rsp->send_message(child_socket, this->all_reasons);
        return;
      }                                                
    } 
    std::unique_ptr<HttpResponse> rsp(RequestHandler::not_found_func(req));
    rsp->send_message(child_socket, this->all_reasons);

    return;
}

std::string&
HttpServer::serve(int child_socket) {
  std::cout << "\n" << ">> Child (" << getpid() << ") reading and responding to request" << std::endl;
  char buffer[4000] = {0};

  // TODO handle bigger requests 
  ssize_t valread = read(child_socket, buffer, sizeof(buffer)-1); // leave a trailing 0
  HttpRequest req(buffer);
  req.show();

  this->dispatch_request(child_socket, req);
  return req.get_path();
}