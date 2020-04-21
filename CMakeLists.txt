cmake_minimum_required(VERSION 3.7)

project(application)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_link_options("-lepoxy")

AUX_SOURCE_DIRECTORY(src SRCFILES)
FILE(GLOB_RECURSE SRCFILES  src/*.cpp)

add_executable(application ${SRCFILES})
target_link_libraries(application ${SDL2_LIBRARIES})
