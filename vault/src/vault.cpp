#include <anthill/http_server.h>
#include <anthill/client_queue.h>
#include "vault.h"
#include "handlers.h"

#include <algorithm>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>

const int default_port = 8001;
enum ExpectingArgument { no_arg, port_arg };


const int NUM_THREADS = 3;
ClientQueue client_que;
ClientQueue& cq_ref = client_que;
std::vector<std::thread> service_threads(NUM_THREADS);

void server_shutdown(HttpServer *server, int child_socket) {
  std::lock_guard<std::mutex> lx(cq_ref.mtx);
  server->close_listening_socket();
  cq_ref.stopped = true;
  cq_ref.c_var.notify_all();
  return;
}

void service(HttpServer *server) {
  {
    std::lock_guard<std::mutex> lx(cq_ref.mtx);
    std::cout << "Starting thread: " << std::this_thread::get_id() << std::endl;
  }
  int child_socket;
  while (1) {
    {
      child_socket = 0;

      // Gain a lock on the client queue mutex and wait to be notified
      std::unique_lock<std::mutex> lck(cq_ref.mtx);
      cq_ref.c_var.wait(lck);

      if (cq_ref.is_stopped()) {
        std::cout << "Terminating thread: " << std::this_thread::get_id() << std::endl;
        return;
      }
      // we were notified that there is something in the cq_ref.que
      // pop this off of the cq_ref.que while we own the mutex
      child_socket = cq_ref.que.front();
      cq_ref.que.pop();
    }

    // Service the child socket
    bool should_exit = server->serve(child_socket);

    shutdown(child_socket, SHUT_WR);
    close(child_socket);

    // handle possible exit indication
    if (should_exit) {
      server_shutdown(server, child_socket);
      std::cout << "Terminating thread: " << std::this_thread::get_id() << std::endl;
      return;
    }
  } // while

  server->close_listening_socket();
  return;
}

// ---------------------------------------------------------------------
// portPtr := flag.Int("port", 8001, "Port on which to listen for requests")
// flag.Parse()
// s := NewVaultServer(*portPtr)
// err := http.ListenAndServe(fmt.Sprintf(":%d", s.port), s.mux)
// if errors.Is(err, http.ErrServerClosed) {
//   glog.Info("server closed")
// } else if err != nil {
//   glog.Errorf("error starting server: %s", err)
//   os.Exit(1)
// }
// ---------------------------------------------------------------------

// vault -port 9999
//
// default is 8001

Vault::Vault(int id): value(0), id(id) {}


int main(int argc, char const *argv[]) {

    int port = default_port;
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

    std::cout << "Starting thread: " << std::this_thread::get_id() <<  " (main)" << std::endl;
    HttpServer server(port);
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

    for (int i=0; i<NUM_THREADS; i++) {
      service_threads[i] = std::thread(service, &server);
      service_threads[i].detach();
    }

    while (server.is_useable()) {
      int child_socket = server.accept();
      if (child_socket < 0) {
        service_threads.clear();
        {
          std::lock_guard<std::mutex> lk(cq_ref.mtx);
          std::cout << "Terminating thread: " << std::this_thread::get_id() <<  " (main)" << std::endl;
        }
        return 0;
      }
      client_que.add_client(child_socket);

    }
    server.close_listening_socket();
    return 0;
}

