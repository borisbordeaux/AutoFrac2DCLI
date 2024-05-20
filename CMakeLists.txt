cmake_minimum_required(VERSION 3.22)
project(AutoFrac2DCLI)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE FILES "src/*.cpp")

add_executable(AutoFrac2DCLI ${FILES})

target_include_directories(AutoFrac2DCLI PUBLIC include)

target_compile_options(AutoFrac2DCLI PRIVATE -Wall -Wextra -Werror)
