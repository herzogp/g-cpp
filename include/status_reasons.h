#pragma once

#include <map>
#include <string>

typedef std::pair<int, std::string> ReasonPair;
typedef std::map<int, std::string> ReasonMap;
typedef ReasonMap::iterator ReasonIterator;

class HttpStatusReasons {
    ReasonMap reasons;
    HttpStatusReasons();

  public:
    ~HttpStatusReasons() = default;
    std::string lookup(int status);
    static HttpStatusReasons& get_reasons();
};


