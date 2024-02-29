#pragma once

#include "status_reasons.h"
#include <string>

class HttpResponse {
  int status;
  std::string content_type;
  std::string content;
  bool want_exit{false};

  public:
    HttpResponse(int status, std::string content_type, std::string content);
    void send_message(int socket, HttpStatusReasons& all_reasons);
    void set_should_exit(bool b);
    bool should_exit();
};
