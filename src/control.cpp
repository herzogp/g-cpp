#include "http.h"

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
    int server_fd, new_socket, pid;
    long valread;
    struct sockaddr_in address;
    struct sockaddr *p_address =  (struct sockaddr *)&address;
    int len_address = sizeof(address);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
      std::perror("Creating a socket");
      exit(EXIT_FAILURE);
    }
   
    memset(&address, '\0', len_address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
  
    const int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    // bind socket to address:port and listen for connections
    if (bind(server_fd, p_address, len_address) < 0) {
      std::perror("Trying to bind()");
      close(server_fd);
      exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0) {
      perror("Trying to listen()");
      exit(EXIT_FAILURE);
    }
    
    HttpStatusReasons *all_reasons = new HttpStatusReasons();

    while(1)
    {
      std::cout << "Accepting connections" << std::endl; 
      new_socket = accept(server_fd, p_address, (socklen_t *)&len_address);
      if (new_socket < 0) {
        std::perror("Trying to accept()");
        exit(EXIT_FAILURE);
      }
      //Create child process to handle request from different client
      pid = fork();
      if (pid < 0) {
        std::perror("Trying to fork()");
        exit(EXIT_FAILURE);
      }

      if (pid == 0) {
        std::cout << "Newly forked client reading and responding to request" << std::endl;
        char buffer[1000] = {0};
        valread = read(new_socket, buffer, sizeof(buffer)-1); // leave a trailing 0
        const char *cp1 = (const char *)buffer;
        HttpRequest *p_request = new HttpRequest(buffer);
        p_request->show();
        if (p_request) {
          if (p_request->get_method() == "GET") {
            if (p_request->get_path() == "/") {
              HttpResponse *rsp = new HttpResponse(200, "text/plain", "Whatever can be provided!\n", all_reasons);
              rsp->send_message(new_socket);
              std::cout << "\nServer responded to HTTP GET" << std::endl;
              delete rsp;
              continue;
            }
            HttpResponse *rsp_fail = new HttpResponse(404, "text/plain", "", all_reasons);
            rsp_fail->send_message(new_socket);
            std::cout << "\nServer rejected path: '" << p_request->get_path() << "'" << std::endl;
            delete rsp_fail;
            continue;
          }
          if (p_request->get_method() == "POST") {
            std::cout << "\nServer responded to HTTP POST" << std::endl;
          }
        } // if p_request
        std::cout << "Client should be done now." << std::endl;
        close(new_socket);
        if (p_request) {
          delete p_request;  
        }
        close(server_fd);
        return 0;
      }
      else {
        std::cout << "Parent still alive.  Created child with pid: " << pid << std::endl;
        close(new_socket); // because child owns a copy and will be responsible?
      }
    }
    close(server_fd);
    return 0;
}
