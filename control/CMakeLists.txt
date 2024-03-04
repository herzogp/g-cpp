cmake_minimum_required(VERSION 3.25)
project(glitch-grid-cpp VERSION $ENV{version} LANGUAGES CXX)

# ----------------------------------------
# glitch-grid control service
# ----------------------------------------
add_executable(control
  src/control.cpp
  src/client_queue.cpp
  src/handlers.cpp
)

target_link_libraries(control PRIVATE anthill)

target_include_directories(control PRIVATE include)

install(TARGETS control)
