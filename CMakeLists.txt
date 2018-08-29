cmake_minimum_required(VERSION 3.5)
project(bulk)

find_package(GTest REQUIRED)
find_package (Threads REQUIRED)

add_executable(bulk main.cpp command_handler.cpp command_handler.h)
add_executable(bulk_test command_handler.h command_handler.cpp test.cpp)

target_include_directories(bulk_test PRIVATE ${GTEST_INCLUDE_DIRS})

set_target_properties(bulk PROPERTIES   CXX_STANDARD 14
                                        CXX_STANDARD_REQUIRED ON
                                        COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;-Werror")

set_target_properties(bulk_test PROPERTIES  CXX_STANDARD 14
                                            CXX_STANDARD_REQUIRED ON)

target_link_libraries(bulk_test ${GTEST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_test(test bulk_test)

enable_testing()
