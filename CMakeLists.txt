cmake_minimum_required(VERSION 3.10)
project(Crioulo)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)

set(SOURCES ${PROJECT_SOURCE_DIR}/src/core/renderer.cpp ${PROJECT_SOURCE_DIR}/src/glad.c)

add_library(Crioulo ${SOURCES})
target_include_directories(Crioulo PRIVATE ${OPENGL_INCLUDE_DIR} include)
target_link_libraries(Crioulo ${OPENGL_gl_LIBRARY} ${PROJECT_SOURCE_DIR}/lib/glfw3.lib)