cmake_minimum_required(VERSION 3.9)
project(lab2)

set(CMAKE_CXX_STANDARD 11)

find_package(MPI REQUIRED)
# add this line only when you are using openmpi which has a different c++ bindings
#add_definitions(-DOMPI_SKIP_MPICXX)

SET(CMAKE_C_COMPILER mpicc)
SET(CMAKE_CXX_COMPILER mpicxx)

set(SOURCE_FILES main.cpp)

add_executable(lab2 main.cpp common.h get_sd_index.h get_sd_index.cpp common.cpp types.h cluster_kohonen.cpp cluster_kohonen.h)

target_link_libraries(lab2 PRIVATE ${MPI_C_LIBRARIES})

target_include_directories(lab2 PRIVATE ${MPI_C_INCLUDE_PATH})
