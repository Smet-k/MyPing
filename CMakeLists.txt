cmake_minimum_required(VERSION 3.14.0)
project(myPing LANGUAGES C)

add_compile_options(-Werror)

file(GLOB myPing_src "src/*.c")
add_executable(myPing ${myPing_src})
target_include_directories(myPing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)