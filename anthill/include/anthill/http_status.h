#pragma once

#include <map>
#include <string>

typedef std::pair<int, std::string> ReasonPair;
typedef std::map<int, std::string> ReasonMap;
typedef ReasonMap::iterator ReasonIterator;

class HttpStatus {
    ReasonMap reasons;
    HttpStatus();

  public:
    ~HttpStatus() = default;
    std::string lookup(int status);
    static HttpStatus& get_reasons();

    // The good
    static const int StatusOK = 200;
    static const int StatusCreated = 201;
    static const int StatusNoContent = 204;
    
    // The bad
    static const int StatusBadRequest = 400;
    static const int StatusNotFound = 404;
    
    // The ugly
    static const int StatusInternalServerError = 500;
};
