cmake_minimum_required(VERSION 3.20)
project(TC CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp)
file(GLOB HDR *.h)

add_executable(${PROJECT_NAME} ${SRC} ${HDR})
target_compile_options(${PROJECT_NAME} PRIVATE /Wall /W4)
