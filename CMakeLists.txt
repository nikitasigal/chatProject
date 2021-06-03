cmake_minimum_required(VERSION 3.19)
project(chatProject C)

set(CMAKE_C_STANDARD 11)

find_package(SQLite3)
link_libraries(ws2_32 wsock32)

add_executable(chatProject_server server/main.c)
add_executable(chatProject_client client/main.c)

include_directories(${SQLite3_INCLUDE_DIRS})
target_link_libraries(chatProject_server ${SQLite3_LIBRARIES})