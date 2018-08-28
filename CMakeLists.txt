cmake_minimum_required(VERSION 3.5)
project(bulk)

set(CMAKE_CXX_STANDARD 14)

add_executable(bulk main.cpp command_handler.cpp command_handler.h)