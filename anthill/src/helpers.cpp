#include "helpers.h"

int 
to_int(std::string& s, int def_val) {
  int result = 0;

  try {
    result = std::stoi(s);
  }
  catch (std::invalid_argument const& ex) {
    result = def_val;
  }
  catch (std::out_of_range const& ex) {
    result = def_val;
  }

  return result;
}

std::vector<std::string> 
split(const std::string &text, char delim) {
    std::vector<std::string> parts;
    std::stringstream ss(text);
    std::string item;

    while (getline(ss, item, delim)) {
        parts.push_back(item);
    }

    return parts;
}

