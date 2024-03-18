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
  p_netbuf = std::unique_ptr<Netbuf>(new Netbuf(socket));
  p_strm = std::unique_ptr<std::basic_iostream<char>>(new std::basic_iostream(p_netbuf.get()));
  read_headers();
}

void
HttpRequest::read_headers() {
  
  // Get all headers until receiving an empty line
  char buffer[3000] = {0};
  char last_char;
  size_t lx;
  bool is_first = true;
  std::string first_line;
  std::basic_iostream<char> *this_strm = this->p_strm.get();

  this_strm->getline(buffer, sizeof(buffer), '\n');
  while (!this_strm->fail()) {
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

    // If the stream is not healthy, stop reading from it
    if (!this_strm->good()) {
      break;
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

void
HttpRequest::read_body(char *buffer, size_t nbx) {
  std::basic_iostream<char> *this_strm = this->p_strm.get();

  this_strm->getline(buffer, nbx, '\0');
  return ;
}

void 
HttpRequest::read_formdata(std::map<std::string, std::string> & form_data) {
  char buffer[3000];
  char last_char;
  size_t lx;
  std::basic_iostream<char> *this_strm = this->p_strm.get();
 
  form_data.clear();

  this_strm->getline(buffer, sizeof(buffer), '&');
  while (!this_strm->fail()) {
    std::string line(buffer);
    lx = line.length();
    if (lx == 0) {
      break;
    }

    // line looks like name=value
    // parse into (name, value) and store in map<string, string>
    std::string::size_type idx = line.find_first_of("=", 0);
    if (idx != std::string::npos) {
      std::string data_name = line.substr(0, idx);
      std::string data_value = line.substr(idx + 1);
      form_data[data_name] = data_value;
    }
  
    // If the stream is not healthy, stop reading from it
    if (!this_strm->good()) {
      break;
    }

    // This might trigger an eof() condition, while
    // returning valid data.  So process the data (if any)
    // by staying in the while loop.
    this_strm->getline(buffer, sizeof(buffer), '&');
  }
 
}

void 
HttpRequest::show() {
  std::cout << "Method: " << this->method << "\n";
  std::cout << "URL: " << this->path << "\n";
  std::cout << "Protocol: " << this->protocol << std::endl;
  
  for (std::string hdr : this->headers) {
    std::cout << hdr << "\n";
  }
  std::cout.flush();
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

