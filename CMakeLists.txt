cmake_minimum_required(VERSION 3.20)
project(TC CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC *.cpp)
file(GLOB HDR *.h)

#set(SOURCES
#    input_reader.cpp
#    main.cpp
#    stat_reader.cpp
#    transport_catalogue.cpp
#
#)
#
#set(HEADERS
#    geo.h
#    input_reader.h
#    stat_reader.h
#    transport_catalogue.h
#)

add_executable(${PROJECT_NAME} ${SRC} ${HDR})
target_compile_options(${PROJECT_NAME} PRIVATE /Wall /W4)
