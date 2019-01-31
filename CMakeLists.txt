cmake_minimum_required(VERSION 3.6)
project(YNet)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)


file(GLOB_RECURSE SRC_FILES  "src/*.cpp" )
include_directories("./src")

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-fPIC)
elseif(MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

set(ENABLE_PROGRAMS OFF)
set(ENABLE_TESTING OFF)
set(INSTALL_MBEDTLS_HEADERS OFF)
include_directories(${YNet_SOURCE_DIR}/mbedtls/include)
add_subdirectory(mbedtls)
add_library(YNet STATIC ${SRC_FILES})

option(HAS_TEST "is need test project"  OFF)
if(HAS_TEST)
    include_directories(${YNet_SOURCE_DIR}/googletest/googletest/include)
    set(BUILD_GMOCK OFF) #close gmock
    add_subdirectory(googletest)
    IF(MSVC)
        message("run here")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    endif()
    project(YURLTest)
    add_executable(YURLTest test/YURLTest.cpp)
    target_link_libraries(YURLTest
    gtest gtest_main YNet)
endif()


project(example)
add_executable(example test.cpp)
target_link_libraries(example
YNet mbedtls)