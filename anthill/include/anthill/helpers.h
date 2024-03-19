#pragma once

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// string helpers
int to_int(std::string& s, int def_val = 0);
std::vector<std::string> split(const std::string &s, char delim);
