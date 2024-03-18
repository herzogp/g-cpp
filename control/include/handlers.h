#pragma once
#include <anthill/request_handler.h>

// Endpoint handlers
HttpResponse * barky_func(HttpRequest& req, void *context);
HttpResponse * root_func(HttpRequest& req, void *context);
HttpResponse * add_func(HttpRequest& req, void *context);
HttpResponse * exit_func(HttpRequest& req, void *context);

void setup_control_handlers(std::vector<RequestHandler>& handlers);
