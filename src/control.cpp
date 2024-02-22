#include "http.h"

#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

const int PORT = 8031;

int main(int argc, char const *argv[]) {
   
    signal(SIGCHLD, SIG_IGN);
    HttpServer server(PORT);

    if (server.not_useable()) {
      std::perror(server.error_text.c_str());
      exit(EXIT_FAILURE);
    }

    HttpStatusReasons *all_reasons = new HttpStatusReasons();

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
        server.close();
        std::cout << "\n" << ">> Child (" << getpid() << ") reading and responding to request" << std::endl;
        char buffer[1000] = {0};
        ssize_t valread = read(child_socket, buffer, sizeof(buffer)-1); // leave a trailing 0
        const char *cp1 = (const char *)buffer;
        HttpRequest req(buffer);
        req.show();
          if (req.get_method() == "GET") {
            if (req.get_path() == "/") {
              HttpResponse rsp(200, "text/plain", "Whatever can be provided!\n", all_reasons);
              rsp.send_message(child_socket);
            } else if (req.get_path() == "/exit") {
              HttpResponse rsp(200, "text/plain", "Exiting!\n", all_reasons);
              rsp.send_message(child_socket);
              shutdown(child_socket, SHUT_WR);
              close(child_socket);
              kill(getppid(), SIGINT);
              return 0;
            } else {
              HttpResponse rsp_fail(404, "text/plain", "", all_reasons);
              rsp_fail.send_message(child_socket);
              std::cout << "Path not found: '" << req.get_path() << "'" << std::endl;
            }
          }
          if ((req.get_method() == "POST") || (req.get_method() != "GET")) {
              HttpResponse rsp_fail(405, "text/plain", "", all_reasons);
              rsp_fail.send_message(child_socket);
              std::cout << "Method not supported: '" << req.get_path() << "'" << std::endl;
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
    server.close();
    return 0;
}
