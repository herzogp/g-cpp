#include <anthill/netbuf.h>
#include "http_request.h"

#include <algorithm>
#include <iostream>
#include <unistd.h>


HttpRequest::HttpRequest(int socket):
  client_socket(socket),
  method(""), 
  path(""),
  protocol(""), 
  body("")
{
  std::unique_ptr<Netbuf> p_netbuf(new Netbuf(socket));
  p_strm = std::unique_ptr<std::basic_iostream<char>>(new std::basic_iostream(p_netbuf.get()));
  read_headers();
}

void
HttpRequest::read_headers() {

  char buffer[4000] = {0};
  
  // Get all headers until receiving an empty line
  char last_char;
  size_t lx;
  bool is_first = true;
  std::string first_line;
  std::basic_iostream<char> *this_strm = this->p_strm.get();

  this_strm->getline(buffer, sizeof(buffer), '\n');
  while (this_strm->good()) {
    std::string line(buffer);
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
    this_strm->getline(buffer, sizeof(buffer), '\n');
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

