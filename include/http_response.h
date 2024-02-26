#pragma once

#include "status_reasons.h"
#include <string>

class HttpResponse {
  int status;
  std::string content_type;
  std::string content;

  public:
    HttpResponse(int status, std::string content_type, std::string content);
    void send_message(int socket, HttpStatusReasons& all_reasons);
};
