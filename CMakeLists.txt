cmake_minimum_required(VERSION 3.8)
project(Binary_tree)

set(CMAKE_CXX_STANDARD 17)


    set(SOURCE_FILES main.cpp bintree.h)
    add_executable(Binary_tree ${SOURCE_FILES})

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ogdf/include)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(Binary_tree ${CMAKE_CURRENT_SOURCE_DIR}/ogdf/libOGDF.a)
    target_link_libraries(Binary_tree ${CMAKE_CURRENT_SOURCE_DIR}/ogdf/libCOIN.a)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")