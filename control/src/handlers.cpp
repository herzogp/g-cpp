#include "handlers.h"
#include <map>

// local functions
int to_int(std::string& s, int def_val=0);

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
add_func(HttpRequest& req) {

  std::map<std::string, std::string> form_data;
  req.read_formdata(form_data);
  int val_1 = to_int(form_data["arg1"]);
  int val_2 = to_int(form_data["arg2"]);
  int result = val_1 + val_2;

  std::ostringstream ss_text;
  ss_text << "Result: " << result << "\n";

  return new HttpResponse(201, "text/plain", ss_text.str().c_str());
}

HttpResponse *
exit_func(HttpRequest& req) {
  HttpResponse *rsp = new HttpResponse(200, "text/plain", "Exiting!\n");
  rsp->set_should_exit(true);
  return rsp;
}

void
setup_control_handlers(std::vector<RequestHandler>& handlers) {
    RequestHandler h1("get", "/barky", barky_func);
    RequestHandler h2("get", "/", root_func);
    RequestHandler h3("post", "/add", add_func);
    RequestHandler h4("get", "/exit", exit_func);

    handlers.push_back(h1);
    handlers.push_back(h2);
    handlers.push_back(h3);
    handlers.push_back(h4);
    return;
}

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
