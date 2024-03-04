#pragma once
#include <anthill/request_handler.h>

// Endpoint handlers
HttpResponse * barky_func(HttpRequest& req);
HttpResponse * root_func(HttpRequest& req);
HttpResponse * exit_func(HttpRequest& req);

void setup_control_handlers(std::vector<RequestHandler>& handlers);
