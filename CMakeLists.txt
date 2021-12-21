cmake_minimum_required(VERSION 3.22)
project(concurrency C)

set(CMAKE_C_STANDARD 11)

add_executable(concurrency main.c occ.c operation.c bank_account.h operation.h occ.h)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})
add_definitions(${GLIB_CFLAGS_OTHER})
target_link_libraries(concurrency ${GLIB_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries (concurrency ${CMAKE_THREAD_LIBS_INIT})