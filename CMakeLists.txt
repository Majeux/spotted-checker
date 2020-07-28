cmake_minimum_required(VERSION 3.10.2)
project(spotted-checker)

include_directories(inc inc/model_abstract inc/model_empty inc/model_example)

file(GLOB SOURCES "src/*.cc")
file(GLOB ABSTRACT_SOURCES "src/model_abstract/*.cc")
file(GLOB EMPTY_SOURCES "src/model_empty/*.cc")
file(GLOB EXAMPLE_SOURCES "src/model_example/*.cc")

set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -O3")
set(CMAKE_CXX_STANDARD 14)

add_executable(checker main/main.cc ${SOURCES} ${ABSTRACT_SOURCES} ${EMPTY_SOURCES} ${EXAMPLE_SOURCES})

target_link_libraries(checker spot bddx)
