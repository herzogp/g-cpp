#include "client_queue.h"

void
ClientQueue::add_client(int child_socket) {
  {
    std::lock_guard<std::mutex> lk(this->mtx);
    this->que.push(child_socket);
  }
  
  // Notify that the queue has been changed
  this->c_var.notify_one();
}

bool
ClientQueue::is_stopped() {
  return this->stopped;
}
