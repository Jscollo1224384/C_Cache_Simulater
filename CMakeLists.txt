cmake_minimum_required(VERSION 3.23)
project(cachelab-jscollo C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(cachelab-jscollo
        cachelab-jscollo.c
        cachelab.h)
target_link_libraries (cachelab-jscollo m )
