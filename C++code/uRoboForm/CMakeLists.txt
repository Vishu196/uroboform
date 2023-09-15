cmake_minimum_required(VERSION 3.1)
project(uRoboForm)

find_package(xtl REQUIRED)
find_package(xtensor REQUIRED)
find_package(xsimd REQUIRED)
add_executable(first_example src/example.cpp)

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS /MANIFEST:NO)
endif()

target_link_libraries(first_example xtensor xtensor::optimize xtensor::use_xsimd)