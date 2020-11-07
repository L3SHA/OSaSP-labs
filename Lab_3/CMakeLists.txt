cmake_minimum_required(VERSION 3.17)
project(Laba3)

set(CMAKE_CXX_STANDARD 14)

add_library(library SHARED library/library.cpp library/library.h)

add_library(remote_entry SHARED library/library.cpp library/library.h)

target_compile_definitions(remote_entry PUBLIC REMOTE_ENTRY)

add_executable(static-import static-import/main.cpp)
target_link_libraries(static-import library)

add_executable(dynamic-import dynamic-import/main.cpp)

add_executable(injector remote-thread-call/injector.cpp)
add_executable(injected remote-thread-call/injected.cpp)