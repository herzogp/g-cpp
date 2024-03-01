cmake_minimum_required(VERSION 3.25)
project(glitch-grid-cpp VERSION $ENV{version} LANGUAGES CXX)

# ----------------------------------------
# glitch-grid services
# ----------------------------------------
add_executable(control
  src/control.cpp
  src/status_reasons.cpp
  src/http_request.cpp
  src/http_response.cpp
  src/request_handler.cpp
  src/http_server.cpp
  src/client_queue.cpp
  src/handlers.cpp
)

target_include_directories(control
  PUBLIC
    include
  PRIVATE
    "${PROJECT_BINARY_DIR}/include"
)

target_sources(control
  PUBLIC
    FILE_SET control
    TYPE HEADERS
    BASE_DIRS include
    FILES
      include/http_server.h
      include/http_request.h
      include/http_response.h
      include/status_reasons.h
      include/request_handler.h
      include/client_queue.h
      include/handlers.h
)

install(TARGETS control)
