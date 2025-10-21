cmake_minimum_required(VERSION 3.14) 

project(RTreeBenchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  libspatialindex
  GIT_REPOSITORY https://github.com/libspatialindex/libspatialindex.git
  GIT_TAG        1.9.3 
)

FetchContent_MakeAvailable(libspatialindex)

add_executable(run_rtree src/run_rtree.cpp)
target_include_directories(run_rtree PRIVATE ${libspatialindex_SOURCE_DIR}/include)

target_link_libraries(run_rtree PRIVATE spatialindex)
