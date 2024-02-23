#include "http.h"
#include "handlers.h"

#include <csignal>
#include <iostream>

const int PORT = 8031;

int main(int argc, char const *argv[]) {
   
    signal(SIGCHLD, SIG_IGN);
    HttpServer server(PORT);

    if (server.not_useable()) {
      std::perror(server.error_text.c_str());
      exit(EXIT_FAILURE);
    }


    std::vector<RequestHandler> control_handlers;
    setup_control_handlers(control_handlers);
    server.set_handlers(control_handlers);

    while (server.is_useable()) {
      std::cout << "Accepting connections (" << getpid() << ")" << std::endl; 
      int child_socket = server.accept();
      if (child_socket < 0) {
        std::perror(server.error_text.c_str());
        std::perror("Failed to accept()");
        exit(EXIT_FAILURE);
      }

      //Create child process to handle request from different client
      pid_t pid = fork();
      if (pid < 0) {
        std::perror("Trying to fork()");
        exit(EXIT_FAILURE);
      }

      if (pid == 0) {
        server.close_listening_socket();
        std::string path = server.serve(child_socket);

        if (path == "/exit") {
          shutdown(child_socket, SHUT_WR);
          close(child_socket);
          kill(getppid(), SIGINT);
          return 0;
        }

        std::cout << ">> Child (" << getpid() << ") is done\n" << std::endl;
        shutdown(child_socket, SHUT_RDWR);
        close(child_socket);
        return 0;
      }
      else {
        close(child_socket); // because child owns a copy and will be responsible?
        std::cout << "Parent forked child with pid: " << pid << "\n" << std::endl;
      }
    }
    server.close_listening_socket();
    return 0;
}
