#include <anthill/helpers.h>
#include "vault.h"
#include "handlers.h"

#include <map>
#include <iostream>
#include <sstream>

// Endpoint handlers
HttpResponse *
root_get_func(HttpRequest& req, void *context) {
  Vault *p_vault = static_cast<Vault *>(context);
  std::ostringstream text;
  text << p_vault->value;
  return new HttpResponse(200, "text/plain", text.str().c_str());
}

HttpResponse *
root_post_func(HttpRequest& req, void *context) {
  char buffer[20] = {0};
  Vault *p_vault = static_cast<Vault *>(context);
  req.read_body(buffer, sizeof(buffer));
  std::string body(buffer);

  int some_val = to_int(body, -1);
  if (some_val >= 0) {
    if (some_val < p_vault->value) {
      std::cout << "THIS SHOULD NEVER HAPPEN: Counter value regressed from " << p_vault->value << " to " << some_val << "\n";
    }
    p_vault->value = some_val;
    std::cout << "Set vault :" << p_vault->id << " Counter " << p_vault->value << "\n";
    return new HttpResponse(200, "text/plain", body.c_str());
  } // some_val >= 0
  
  // body was not a valid integer, or it was negative
  return new HttpResponse(400, "text/plain", "Invalid or missing POST body"); 
}

HttpResponse *
exit_func(HttpRequest& req, void *context) {
  HttpResponse *rsp = new HttpResponse(200, "text/plain", "Exiting!\n");
  rsp->set_should_exit(true);
  return rsp;
}

void
setup_vault_handlers(std::vector<RequestHandler>& handlers) {
    RequestHandler h1("get", "/", root_get_func);
    RequestHandler h2("post", "/", root_post_func);
    RequestHandler h3("get", "/exit", exit_func);

    handlers.push_back(h1);
    handlers.push_back(h2);
    handlers.push_back(h3);
    return;
}
