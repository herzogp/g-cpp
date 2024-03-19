cmake_minimum_required(VERSION 3.25)
project(control-cpp VERSION $ENV{version} LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-std=c++20 -fsanitize-coverage=trace-pc-guard")

# ----------------------------------------
# glitch-grid control service
# ----------------------------------------
add_executable(control
  src/control.cpp
  src/control_server.cpp
  src/handlers.cpp
)

target_link_libraries(control PRIVATE anthill)

target_include_directories(control PRIVATE include antithesis-sdk-cpp)

install(TARGETS control)
