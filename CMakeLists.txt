cmake_minimum_required(VERSION 3.8)
project(ws_test)

include_directories(
  lib
)

add_executable(ws_test main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(ws_test PRIVATE Threads::Threads)
