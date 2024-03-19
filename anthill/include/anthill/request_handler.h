#pragma once

#include "http_request.h"
#include "http_response.h"
#include <string>

typedef HttpResponse * (*RequestHandlerFunc)(HttpRequest& req, void *context);

struct RequestHandler {
    std::string method;
    std::string path;
    RequestHandlerFunc func;
    static RequestHandlerFunc not_found_func;

    RequestHandler(std::string method, std::string path, RequestHandlerFunc func);
};
