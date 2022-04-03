cmake_minimum_required(VERSION 3.4.1)
project(socket)
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")
add_executable(listen listen.cpp) 