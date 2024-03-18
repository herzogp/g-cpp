#include <anthill/http_server.h>
#include "handlers.h"
#include <iostream>
#include <thread>

const int DEFAULT_PORT = 8031;
const int NUM_THREADS = 3;

int main(int argc, char const *argv[]) {
  
    std::cout << "Starting server  thread: " << std::this_thread::get_id() <<  " (main)" << std::endl;
    HttpServer server(DEFAULT_PORT, NUM_THREADS);

    if (server.not_useable()) {
      std::perror(server.error_text.c_str());
      exit(EXIT_FAILURE);
    }

    std::vector<RequestHandler> control_handlers;
    setup_control_handlers(control_handlers);
    server.set_handlers(control_handlers);

    server.start_services();
    std::cout << "\nAll services have been stopped now.\n";

    return 0;
}
