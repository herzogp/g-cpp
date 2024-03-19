#pragma once

#include <mutex>
#include <string>
#include <vector>

class ControlServer {
  std::vector<std::string> vaults;
  int min_value;

  //sync.RWMutex lock;
  std::mutex mutex;
  // to lock for reading (Go => RLock/RUnlock)
  // std::shared_lock<std::mutex> lock(this->mutex)
  //
  // to lock for writing (Go => Lock/Unlock)
  // std::lock_guard<std::mutex> lock(this->mutex)

public:
  ControlServer(std::string& vaults_list);
  void show_vaults();
  bool has_majority(int count);
  void heal_failing_vault(std::string vault);
  int get_value_from_vaults();

};


