cmake_minimum_required(VERSION 3.19)
project(chatProject C)

set(CMAKE_C_STANDARD 11)

find_package(SQLite3)
link_libraries(ws2_32 wsock32)

find_package(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

file(GLOB src_client CONFIGURE_DEPENDS "client/*.h" "client/*.c" "shared/*.h" "shared/*.c")
file(GLOB src_server CONFIGURE_DEPENDS "server/*.h" "server/*.c" "shared/*.h" "shared/*.c")

add_executable(chatProject_server ${src_server})
add_executable(chatProject_client ${src_client})

include_directories(${SQLite3_INCLUDE_DIRS} ${GTK3_INCLUDE_DIRS})

target_link_libraries(chatProject_server ${SQLite3_LIBRARIES} ${GTK3_LIBRARIES})
target_link_libraries(chatProject_client ${GTK3_LIBRARIES})
