cmake_minimum_required(VERSION 3.25)
project(glitch-grid-cpp VERSION $ENV{version} LANGUAGES CXX)

# configure_file(
#   "${PROJECT_SOURCE_DIR}/config.h.in" 
#   "${PROJECT_BINARY_DIR}/include/config.h"
#   )

# ----------------------------------------
# glitch-grid services
# ----------------------------------------
add_executable(control
  src/hello.cpp
  src/http.cpp
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
      include/http.h
)

install(TARGETS control)

