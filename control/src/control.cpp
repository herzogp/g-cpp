#include <anthill/http_server.h>
#include <anthill/helpers.h>
#include <antithesis_sdk.h>
#include "control_server.h"
#include "handlers.h"
#include <iostream>
#include <thread>

const int DEFAULT_PORT = 8000;
const int NUM_THREADS = 3;
enum ExpectingArgument { no_arg, port_arg, vaults_arg };

int main(int argc, char const *argv[]) {
  
  std::cout << "Starting server  thread: " << std::this_thread::get_id() <<  " (main)" << std::endl;

  std::cout << "Control Server booting...\n";
  ALWAYS(true, "Control service: service started", {});

  int port = DEFAULT_PORT;
  std::string vaults_list;
  //  std::vector<std::string> vaults;

  ExpectingArgument expecting = no_arg;
  int maybe_port = 0;

  if (argc > 1) {
    for (int idx = 1; idx < argc; idx++) {
      std::string arg(argv[idx]);
      switch (expecting) {
        case port_arg:
          maybe_port = to_int(arg);
          if (maybe_port > 1024) {
            port = maybe_port;
          }
          expecting = no_arg;
          break;

        case vaults_arg:
          vaults_list = arg;
          expecting = no_arg;
          break;

        case no_arg:
          if (arg.compare("-port") == 0) {
            expecting = port_arg;
          }
          if (arg.compare("-vaults") == 0) {
            expecting = vaults_arg;
          }
          break;

        default:
          break;
      }
    }
  }

  HttpServer server(port, NUM_THREADS);

  if (server.not_useable()) {
    std::perror(server.error_text.c_str());
    exit(EXIT_FAILURE);
  }
  
  ControlServer ctl_server(vaults_list);
  // ctl_server.show_vaults();

  antithesis::setup_complete({{"port",port}, {"vaults", vaults_list}});
  ALWAYS(true, "Control service: setup complete", {});

  // PH
  bool has_plenty = ctl_server.has_majority(2);
  std::cout << "has_majority: " << has_plenty << '\n';

  std::vector<RequestHandler> control_handlers;
  setup_control_handlers(control_handlers);
  server.set_handlers(control_handlers);

  server.start_services();

  // ------------------------------------------------------------------
  // TODO: Detect if the server did not start 
  // TODO Detect if the server stops unexpectedly
  // ------------------------------------------------------------------
	// if errors.Is(err, http.ErrServerClosed) {
	// 	assert.Unreachable("Control service: closed unexpectedly", Details{"error": err})
	// 	fmt.Printf("server closed\n")
	// } else if err != nil {
	// 	assert.Unreachable("Control service: did not start", Details{"error": err})
	// 	fmt.Printf("error starting server: %s\n", err)
	// 	os.Exit(1)
	// }
  // ------------------------------------------------------------------

  std::cout << "\nAll services have been stopped now.\n";

  return 0;
}

