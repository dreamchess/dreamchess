cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(dreamchess++ LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_COMPILER_ID "CLANG")

#--------------
# MAIN SECTION
#--------------

set(SRC
	${PROJECT_SOURCE_DIR}/main.cpp
	${PROJECT_SOURCE_DIR}/src/Board.cpp
)

set(INC
	${PROJECT_SOURCE_DIR}/include/Board.hpp
	${PROJECT_SOURCE_DIR}/include/Piece.hpp
)

add_executable(main_${PROJECT_NAME} ${INC} ${SRC})

target_include_directories(main_${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
