#include <anthill/helpers.h>
#include <anthill/http_status.h>
#include <antithesis_sdk.h>
#include "handlers.h"
#include "control_server.h"
#include <map>

// Endpoint handlers
HttpResponse *
barky_func(HttpRequest& req, void *context) {
  return new HttpResponse(200, "text/plain", "Woof, woof\n");
}

HttpResponse *
root_func(HttpRequest& req, void *context) {
  return new HttpResponse(200, "text/plain", "Whatever can be provided!\n");
}

HttpResponse *
add_func(HttpRequest& req, void *context) {

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
exit_func(HttpRequest& req, void *context) {
  HttpResponse *rsp = new HttpResponse(200, "text/plain", "Exiting!\n");
  rsp->set_should_exit(true);
  return rsp;
}

// Get the current value of the counter.
// Poll all our backend servers and see if we have majority consensus.
// Sends a 200 and the value to the client if we have a consensus, 500 otherwise.
HttpResponse *
get_counter(HttpRequest& req, void *context) {
  ControlServer *ctl_server = static_cast<ControlServer *>(context);
	ALWAYS(true, "Control service: received a request to retrieve the counter's value", {});
	int result = ctl_server->get_value_from_vaults();
	int status_code = 0;
  std::string body;
  std::ostringstream ss_text;
	if (result >= 0) {
		ALWAYS_OR_UNREACHABLE(true, "Counter's value retrieved", {{"counter", body}, {"status", status_code}});
		status_code = HttpStatus::StatusOK;

		// TODO body = fmt.Sprintf("%d", result)
    ss_text << result;
    body = ss_text.str();

	} else {
		UNREACHABLE("Counter should never be unavailable", {{"result", result}});
		status_code = HttpStatus::StatusInternalServerError;
    ss_text << -1;
    body = ss_text.str();
	}

	int expected_status = (status_code == HttpStatus::StatusOK) || (status_code == HttpStatus::StatusInternalServerError);
	ALWAYS_OR_UNREACHABLE(expected_status, "HTTP return status is expected", {{"status", status_code}});
	ALWAYS(status_code != HttpStatus::StatusInternalServerError, "The server never return a 500 HTTP response code", {{"status", status_code}});
	
  return new HttpResponse(status_code, "text/plain", body.c_str());
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
