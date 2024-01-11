cmake_minimum_required(VERSION 3.10)

project(jlibc_tests)

set(CMAKE_C_STANDARD 17)

if ( UNIX )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    else ()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native")
    endif ()
elseif( WIN32 )
endif()

enable_testing()
add_subdirectory(log)
add_subdirectory(util)
