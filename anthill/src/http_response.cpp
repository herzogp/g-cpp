#include "http_response.h"

#include <sys/socket.h>
#include <iostream>
#include <ostream>
#include <sstream>

HttpResponse::HttpResponse(int status, std::string content_type, std::string content) :
  status(status), 
  content_type(content_type), 
  content(content) {
 }

void 
HttpResponse::send_message(int socket, HttpStatus& all_reasons) {
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

  const char *divider = "--------------------------------------\n";
  std::cout << "\n" << divider << "Response (socket:" << socket << ")\n" << divider; 
  std::cout << buffer.str() << std::endl;

  std::string str_data = buffer.str();
  int bytes_sent = 0;
  int bytes_remaining = str_data.length();
  while (bytes_remaining > 0) {
    int nbx = ::send(socket, str_data.c_str() + bytes_sent, bytes_remaining, 0);
    bytes_sent += nbx;
    bytes_remaining -= nbx;
  }
}

void 
HttpResponse::set_should_exit(bool b) {
  this->want_exit = b;
}

bool 
HttpResponse::should_exit() {
  return this->want_exit;
}
