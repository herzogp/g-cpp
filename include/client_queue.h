#include <condition_variable>
#include <mutex>
#include <queue>

struct ClientQueue {
  std::condition_variable c_var;
  std::mutex mtx;
  std::queue<int> que;
  bool stopped{false};
  void add_client(int child_socket);
  bool is_stopped();
};


