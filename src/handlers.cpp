#include "http.h"

// Endpoint handlers
HttpResponse *
barky_func(HttpRequest& req) {
  return new HttpResponse(200, "text/plain", "Woof, woof\n");
}

HttpResponse *
root_func(HttpRequest& req) {
  return new HttpResponse(200, "text/plain", "Whatever can be provided!\n");
}

HttpResponse *
exit_func(HttpRequest& req) {
  return new HttpResponse(200, "text/plain", "Exiting!\n");
}

void
setup_control_handlers(std::vector<RequestHandler>& handlers) {
    RequestHandler h1("get", "/barky", barky_func);
    RequestHandler h2("get", "/", root_func);
    RequestHandler h3("get", "/exit", exit_func);

    handlers.push_back(h1);
    handlers.push_back(h2);
    handlers.push_back(h3);
    return;
}
