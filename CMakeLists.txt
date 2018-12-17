cmake_minimum_required(VERSION 3.6)
project(YNet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

file(GLOB SRC_FILES  "src/*.cpp" )

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-fPIC)
elseif(MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()



add_library(YNet STATIC ${SRC_FILES})