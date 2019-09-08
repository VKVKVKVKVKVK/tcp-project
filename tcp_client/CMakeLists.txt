cmake_minimum_required(VERSION 3.13)
project(tcp_client)

set(CMAKE_CXX_STANDARD 14)

add_executable(tcp_client client.cpp)
target_link_libraries(tcp_client ws2_32)