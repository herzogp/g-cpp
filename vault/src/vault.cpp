#include <anthill/http_server.h>
#include <anthill/helpers.h>
#include "vault.h"
#include "handlers.h"

#include <iostream>
#include <string>
#include <thread>

const int DEFAULT_PORT = 8001;
const int NUM_THREADS = 3;
enum ExpectingArgument { no_arg, port_arg };

Vault::Vault(int id): value(0), id(id) {}


int main(int argc, char const *argv[]) {

    int port = DEFAULT_PORT;
    ExpectingArgument expecting = no_arg;
    if (argc > 1) {
      for (int idx = 1; idx < argc; idx++) {
        std::string arg(argv[idx]);
        if (expecting == port_arg) {
          int maybe_port = to_int(arg);
          if (maybe_port > 1024) {
            port = maybe_port;
          }
          expecting = no_arg;
        }
        if (arg.compare("-port") == 0) {
          expecting = port_arg;
        }
      }
    }

    std::cout << "Starting server  thread: " << std::this_thread::get_id() <<  " (main)" << std::endl;
    HttpServer server(port, NUM_THREADS);
    Vault my_vault(port);
    my_vault.value = 29;

    if (server.not_useable()) {
      std::perror(server.error_text.c_str());
      exit(EXIT_FAILURE);
    }

    std::vector<RequestHandler> vault_handlers;
    setup_vault_handlers(vault_handlers);
    server.set_handlers(vault_handlers);
    server.set_app_context(&my_vault);

    server.start_services();
    std::cout << "\nAll services have been stopped now.\n";

    return 0;
}

