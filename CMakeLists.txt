cmake_minimum_required(VERSION 3.7)

project(application)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# OpenGL
find_package(OpenGL REQUIRED)
link_libraries(${OPENGL_gl_LIBRARY})
include_directories(PUBLIC ${OPENGL_INCLUDE_DIR})

# GL Extension Wrangler (GLEW)
find_package(GLEW REQUIRED)
link_libraries(${GLEW_LIBRARIES})
include_directories(PUBLIC ${GLEW_INCLUDE_DIRS})

AUX_SOURCE_DIRECTORY(src SRCFILES)
FILE(GLOB_RECURSE SRCFILES  src/*.cpp)

add_executable(application ${SRCFILES})
target_link_libraries(application ${SDL2_LIBRARIES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders)
