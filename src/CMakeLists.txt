cmake_minimum_required(VERSION 3.8)
project(OnePassSynthesisDMFB)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_path(Z3_INCLUDE_DIR z3++.h)
find_library(Z3_LIBRARY z3)

set(SOURCE_FILES main.cpp Graph.cpp Node.cpp Solver.cpp)
add_executable(OPSDMFB ${SOURCE_FILES})
target_link_libraries(OPSDMFB PRIVATE ${Z3_LIBRARY}) 
target_include_directories(OPSDMFB PRIVATE ${Z3_INCLUDE_DIR}) 
