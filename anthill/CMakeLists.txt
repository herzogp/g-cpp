cmake_minimum_required(VERSION 3.25)
project(anthill VERSION $ENV{version} LANGUAGES CXX)

# ----------------------------------------
# anthill http library
# ----------------------------------------
add_library(anthill
  src/status_reasons.cpp
)

target_include_directories(anthill
  PUBLIC
    include/anthill
)

target_sources(anthill
  PUBLIC
    FILE_SET anthill
    TYPE HEADERS
    BASE_DIRS include
    FILES
      include/anthill/status_reasons.h
)

install(TARGETS anthill FILE_SET anthill)

