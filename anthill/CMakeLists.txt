cmake_minimum_required(VERSION 3.25)
project(anthill VERSION $ENV{version} LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-std=c++20 -fsanitize-coverage=trace-pc-guard")

# ----------------------------------------
# anthill http library
# ----------------------------------------
add_library(anthill
  src/client_queue.cpp
  src/helpers.cpp
  src/http_request.cpp
  src/http_response.cpp
  src/http_server.cpp
  src/netbuf.cpp
  src/request_handler.cpp
  src/http_status.cpp
)

target_include_directories(anthill PUBLIC include/anthill)

target_sources(anthill
  PUBLIC
    FILE_SET anthill
    TYPE HEADERS
    BASE_DIRS include
    FILES
      include/anthill/client_queue.h
      include/anthill/helpers.h
      include/anthill/http_request.h
      include/anthill/http_response.h
      include/anthill/http_server.h
      include/anthill/netbuf.h
      include/anthill/request_handler.h
      include/anthill/http_status.h
)

install(TARGETS anthill FILE_SET anthill)
