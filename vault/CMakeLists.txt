cmake_minimum_required(VERSION 3.25)
project(vault-cpp VERSION $ENV{version} LANGUAGES CXX)

# ----------------------------------------
# glitch-grid vault service
# ----------------------------------------
add_executable(vault
  src/vault.cpp
  src/handlers.cpp
)

target_link_libraries(vault PRIVATE anthill)

target_include_directories(vault PRIVATE include)

install(TARGETS vault)

