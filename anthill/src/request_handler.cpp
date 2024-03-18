#include "request_handler.h"

#include <algorithm>
#include <iostream>
#include <ostream>

RequestHandler::RequestHandler(std::string method, std::string path, RequestHandlerFunc func) : path(path), func(func) {
    this->method.resize(method.length());
    std::transform(method.begin(), method.end(), this->method.begin(), ::toupper); 
  }

HttpResponse *
not_found_impl(HttpRequest& req, void *context) {
  std::cout << "Path not found: '" << req.get_path() << "'" << std::endl;
  return new HttpResponse(404, "text/plain", "");
} 

RequestHandlerFunc RequestHandler::not_found_func = not_found_impl;

int 
to_int(std::string& s, int def_val) {
  int result = 0;

  try {
    result = std::stoi(s);
  }
  catch (std::invalid_argument const& ex) {
    result = def_val;
  }
  catch (std::out_of_range const& ex) {
    result = def_val;
  }

  return result;
}

