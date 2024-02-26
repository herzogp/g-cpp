#include "request_handler.h"

#include <algorithm>
#include <iostream>
#include <ostream>

RequestHandler::RequestHandler(std::string method, std::string path, RequestHandlerFunc func) : path(path), func(func) {
    this->method.resize(method.length());
    std::transform(method.begin(), method.end(), this->method.begin(), ::toupper); 
  }

HttpResponse *
not_found_impl(HttpRequest& req) {
  std::cout << "Path not found: '" << req.get_path() << "'" << std::endl;
  return new HttpResponse(404, "text/plain", "");
} 

RequestHandlerFunc RequestHandler::not_found_func = not_found_impl;


