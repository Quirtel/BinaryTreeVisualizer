cmake_minimum_required(VERSION 3.8)
project(Binary_tree)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp bintree.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ogdf/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ogdf/)

add_executable(Binary_tree ${SOURCE_FILES})
add_dependencies(Binary_tree OGDF)
add_dependencies(Binary_tree COIN)

find_library(OGDF libOGDF.a ${CMAKE_CURRENT_SOURCE_DIR}/ogdf/*)
find_library(COIN libCOIN.a ${CMAKE_CURRENT_SOURCE_DIR}/ogdf/*)
target_link_libraries(Binary_tree ${OGDF} ${COIN})
