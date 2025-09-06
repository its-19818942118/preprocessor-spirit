cmake_minimum_required (
    VERSION 3.15
)

project ( Stress
    VERSION 0.1
    LANGUAGES CXX
)

set ( CMAKE_CXX_STANDARD 26 )
set ( CMAKE_CXX_STANDARD_REQUIRED true )

set ( CMAKE_C_STANDARD 26 )
set ( CMAKE_C_STANDARD_REQUIRED true )

add_compile_options (
    -O3
    -Oz
    -flto
    -Ofast
    -static
    -DNDEBUG
    -ffast-math
    -march=native
    -mtune=native
    -flto=jobserver
)

find_package ( fmt )

include_directories (
    include/
)

add_executable ( a.out
    ansi.cpp
)

target_link_libraries ( a.out PRIVATE fmt::fmt )


