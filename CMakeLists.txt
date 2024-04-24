cmake_minimum_required(VERSION 3.20)
project(TC CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    input_reader.cpp
    main.cpp
    stat_reader.cpp
    transport_catalogue.cpp
)

set(HEADERS
    geo.h
    input_reader.h
    stat_reader.h
    transport_catalogue.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

