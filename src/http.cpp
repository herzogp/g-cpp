#include "http.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

RequestHandler::RequestHandler(std::string method, std::string path, RequestHandlerFunc func) : path(path), func(func) {
    this->method.resize(method.length());
    std::transform(method.begin(), method.end(), this->method.begin(), ::toupper); 
  }

HttpServer::HttpServer(int port) : port(port) {
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


HttpResponse *
not_found_impl(HttpRequest& req) {
  std::cout << "Path not found: '" << req.get_path() << "'" << std::endl;
  return new HttpResponse(404, "text/plain", "");
} 

RequestHandlerFunc RequestHandler::not_found_func = not_found_impl;


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
        rsp->send_message(child_socket);
        return;
      }                                                
    } 
    std::unique_ptr<HttpResponse> rsp(RequestHandler::not_found_func(req));
    rsp->send_message(child_socket);

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


HttpRequest::HttpRequest(std::string req): 
  method(""), 
  path(""),
  protocol(""), 
  body("") {

  std::stringstream ss_req(req);
  std::string line;
  
  // Get all headers until receiving an empty line
  char last_char;
  size_t lx;
  bool is_first = true;
  std::string first_line;
  while (std::getline(ss_req, line)) {
    lx = line.length();
    if (lx == 0) {
      break;
    }
    last_char = line.back();
    if (last_char == '\r') {
      line.pop_back();
      lx--;
    }
    if (lx == 0) {
      break;
    }
    if (is_first) {
      first_line = line;
      is_first = false;
    } else {
      this->headers.push_back(line);
    }
  }
  
  if (first_line.size() == 0) {
    return;
  }
  
  std::stringstream ss_line(first_line);
  
  // First line should be the request url and method
  if (ss_line.good()) {
    std::string maybe_method;
    ss_line >> maybe_method;
    this->method.resize(maybe_method.length()); 
    std::transform(maybe_method.begin(), maybe_method.end(), this->method.begin(), ::toupper); 
  }
  
  if (ss_line.good()) {
  this->path = "/";
    ss_line >> this->path;
  }
  
  if (ss_line.good()) {
    ss_line >> this->protocol;
  }
  
}

void HttpRequest::show() {
  std::cout << "Method: " << this->method << "\n";
  std::cout << "URL: " << this->path << "\n";
  std::cout << "Protocol: " << this->protocol << std::endl;
  
  for (std::string hdr : this->headers) {
    std::cout << "HDR: " << hdr << "\n";
  }
  std::cout << std::endl;
}

std::string&
HttpRequest::get_method() {
  return this->method;
}

std::string&
HttpRequest::get_path() {
  return this->path;
}

std::string&
HttpRequest::get_protocol() {
  return this->protocol;
}

std::string&
HttpRequest::get_body() {
  return this->body;
}

HttpStatusReasons::~HttpStatusReasons() {
  std::cout << "Clearing ReasonsMap" << std::endl;
  this->reasons.clear();
}

HttpStatusReasons::HttpStatusReasons() {
  this->reasons.insert(ReasonPair(100, "Continue"));
  this->reasons.insert(ReasonPair(101, "Switching Protocols"));
  this->reasons.insert(ReasonPair(102, "Processing (WebDAV)"));
  this->reasons.insert(ReasonPair(103, "Early Hints"));
  this->reasons.insert(ReasonPair(200, "OK"));
  this->reasons.insert(ReasonPair(201, "Created"));
  this->reasons.insert(ReasonPair(202, "Accepted"));
  this->reasons.insert(ReasonPair(203, "Non-Authoritative Information"));
  this->reasons.insert(ReasonPair(204, "No Content"));
  this->reasons.insert(ReasonPair(205, "Reset Content"));
  this->reasons.insert(ReasonPair(206, "Partial Content"));
  this->reasons.insert(ReasonPair(207, "Multi-Status (WebDAV)"));
  this->reasons.insert(ReasonPair(208, "Already Reported (WebDAV)"));
  this->reasons.insert(ReasonPair(226, "IM Used (HTTP Delta encoding)"));
  this->reasons.insert(ReasonPair(300, "Multiple Choices"));
  this->reasons.insert(ReasonPair(301, "Moved Permanently"));
  this->reasons.insert(ReasonPair(302, "Found"));
  this->reasons.insert(ReasonPair(303, "See Other"));
  this->reasons.insert(ReasonPair(304, "Not Modified"));
  this->reasons.insert(ReasonPair(305, "Use Proxy Deprecated"));
  this->reasons.insert(ReasonPair(306, "unused"));
  this->reasons.insert(ReasonPair(307, "Temporary Redirect"));
  this->reasons.insert(ReasonPair(308, "Permanent Redirect"));
  this->reasons.insert(ReasonPair(400, "Bad Request"));
  this->reasons.insert(ReasonPair(401, "Unauthorized"));
  this->reasons.insert(ReasonPair(402, "Payment Required Experimental"));
  this->reasons.insert(ReasonPair(403, "Forbidden"));
  this->reasons.insert(ReasonPair(404, "Not Found"));
  this->reasons.insert(ReasonPair(405, "Method Not Allowed"));
  this->reasons.insert(ReasonPair(406, "Not Acceptable"));
  this->reasons.insert(ReasonPair(407, "Proxy Authentication Required"));
  this->reasons.insert(ReasonPair(408, "Request Timeout"));
  this->reasons.insert(ReasonPair(409, "Conflict"));
  this->reasons.insert(ReasonPair(410, "Gone"));
  this->reasons.insert(ReasonPair(411, "Length Required"));
  this->reasons.insert(ReasonPair(412, "Precondition Failed"));
  this->reasons.insert(ReasonPair(413, "Payload Too Large"));
  this->reasons.insert(ReasonPair(414, "URI Too Long"));
  this->reasons.insert(ReasonPair(415, "Unsupported Media Type"));
  this->reasons.insert(ReasonPair(416, "Range Not Satisfiable"));
  this->reasons.insert(ReasonPair(417, "Expectation Failed"));
  this->reasons.insert(ReasonPair(418, "I'm a teapot"));
  this->reasons.insert(ReasonPair(421, "Misdirected Request"));
  this->reasons.insert(ReasonPair(422, "Unprocessable Content (WebDAV)"));
  this->reasons.insert(ReasonPair(423, "Locked (WebDAV)"));
  this->reasons.insert(ReasonPair(424, "Failed Dependency (WebDAV)"));
  this->reasons.insert(ReasonPair(425, "Too Early Experimental"));
  this->reasons.insert(ReasonPair(426, "Upgrade Required"));
  this->reasons.insert(ReasonPair(428, "Precondition Required"));
  this->reasons.insert(ReasonPair(429, "Too Many Requests"));
  this->reasons.insert(ReasonPair(431, "Request Header Fields Too Large"));
  this->reasons.insert(ReasonPair(451, "Unavailable For Legal Reasons"));
  this->reasons.insert(ReasonPair(500, "Internal Server Error"));
  this->reasons.insert(ReasonPair(501, "Not Implemented"));
  this->reasons.insert(ReasonPair(502, "Bad Gateway"));
  this->reasons.insert(ReasonPair(503, "Service Unavailable"));
  this->reasons.insert(ReasonPair(504, "Gateway Timeout"));
  this->reasons.insert(ReasonPair(505, "HTTP Version Not Supported"));
  this->reasons.insert(ReasonPair(506, "Variant Also Negotiates"));
  this->reasons.insert(ReasonPair(507, "Insufficient Storage (WebDAV)"));
  this->reasons.insert(ReasonPair(508, "Loop Detected (WebDAV)"));
  this->reasons.insert(ReasonPair(510, "Not Extended"));
  this->reasons.insert(ReasonPair(511, "Network Authentication Required"));
}

std::string
HttpStatusReasons::lookup(int status) {
  ReasonIterator it = this->reasons.find(status);
  if (it == reasons.end()) {
    return "Unknown";
  }
  return it->second;
}


HttpResponse::HttpResponse(int status, std::string content_type, std::string content) :
  status(status), 
  content_type(content_type), 
  content(content) {
 }

void HttpResponse::send_message(int socket) {

  HttpStatusReasons all_reasons;

  std::stringstream buffer;
  std::string status_text = all_reasons.lookup(this->status);
  buffer << "HTTP/1.1 " << this->status << " " << status_text << "\r\n";
  buffer << "Content-Type: " << this->content_type << "; charset=utf-8" << "\r\n";
  buffer << "Content-Length: " << this->content.length() << "\r\n";
  buffer << "Connection: close\r\n";
  buffer << "\r\n";
  if (content.length()) {
    buffer << content;
  }

  std::cout << buffer.str() << std::endl;

  std::string str_data = buffer.str();
  int bytes_sent = 0;
  int bytes_remaining = str_data.length();
  while (bytes_remaining > 0) {
    int nbx = send(socket, str_data.c_str() + bytes_sent, bytes_remaining, 0);
    bytes_sent += nbx;
    bytes_remaining -= nbx;
  }
}
