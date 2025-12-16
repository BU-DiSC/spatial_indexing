cmake_minimum_required(VERSION 3.14) 

project(RTreeBenchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  libspidx_fork
  GIT_REPOSITORY https://github.com/BU-DiSC/libspatialindex.git
  GIT_TAG main  
)
FetchContent_MakeAvailable(libspidx_fork)

# FetchContent_Declare(
#   tomlplusplus
#   GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
#   GIT_TAG v3.3.0
# )
# FetchContent_MakeAvailable(tomlplusplus)

add_executable(run_rtree 
    src/run_rtree.cpp
    src/workload_generator.cpp
    src/tree_setup.cpp
    src/benchmark_runner.cpp
    src/sware_benchmark.cpp 
)

target_include_directories(run_rtree PRIVATE
    ${libspidx_fork_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(run_rtree PRIVATE spatialindex)
