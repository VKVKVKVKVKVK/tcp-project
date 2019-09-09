cmake_minimum_required(VERSION 3.13)
project(tcp_server)

set(CMAKE_CXX_STANDARD 14)

add_executable(tcp_server server_tcp.cpp)
target_link_libraries(tcp_server ws2_32)
