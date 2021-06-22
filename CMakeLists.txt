cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(dreamchess++ LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_COMPILER /bin/clang++)

#--------------
# MAIN SECTION
#--------------

add_executable(main_${PROJECT_NAME} main.cpp)