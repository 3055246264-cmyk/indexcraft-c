cmake_minimum_required(VERSION 3.16)

project(indexcraft_c VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(indexcraft
    src/main.c
    src/index.c
)

target_include_directories(indexcraft PRIVATE src)
target_compile_options(indexcraft PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(indexcraft PRIVATE __USE_MINGW_ANSI_STDIO=1)

if (NOT WIN32)
    target_link_libraries(indexcraft PRIVATE m)
endif()

