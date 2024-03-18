#pragma once
#include <anthill/request_handler.h>

// Endpoint handlers
HttpResponse * root_get_func(HttpRequest& req, void *context);
HttpResponse * root_post_func(HttpRequest& req, void *context);
HttpResponse * exit_func(HttpRequest& req, void *context);

void setup_vault_handlers(std::vector<RequestHandler>& handlers);
