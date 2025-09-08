cmake_minimum_required(VERSION 3.14.0)
project(MyPing LANGUAGES C)

add_compile_options(-Werror -Wall)

file(GLOB MyPing_src "src/*.c")
add_executable(MyPing ${MyPing_src})
target_include_directories(MyPing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)