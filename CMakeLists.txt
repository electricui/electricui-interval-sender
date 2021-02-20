cmake_minimum_required(VERSION 3.6)

SET( PROJECT_VERSION 0.0.1 )

project(electricui-interval-sender)

add_library(electricui-interval-sender STATIC src/interval_send.c)
target_include_directories(electricui-interval-sender PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(electricui-interval-sender electricui)