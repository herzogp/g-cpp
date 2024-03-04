cmake_minimum_required(VERSION 3.25)
project(anthill VERSION $ENV{version} LANGUAGES CXX)

# ----------------------------------------
# anthill http library
# ----------------------------------------
add_library(anthill
  src/http_request.cpp
  src/http_response.cpp
  src/http_server.cpp
  src/request_handler.cpp
  src/status_reasons.cpp
)

target_include_directories(anthill PUBLIC include/anthill)

target_sources(anthill
  PUBLIC
    FILE_SET anthill
    TYPE HEADERS
    BASE_DIRS include
    FILES
      include/anthill/http_request.h
      include/anthill/http_response.h
      include/anthill/http_server.h
      include/anthill/request_handler.h
      include/anthill/status_reasons.h
)

install(TARGETS anthill FILE_SET anthill)
