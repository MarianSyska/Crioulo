cmake_minimum_required(VERSION 4.2)
project(Crioulo)

include(CMakeDependentOption)
include("cmake/GenerateShaderHeaders.cmake")


# --------------------------------------------------
# Define Options and Cache Variables
# -------------------------------------------------
option(CRIOULO_INCLUDE_TRACY "Include tracy profiling." OFF)
cmake_dependent_option(CRIOULO_TRACY_ENABLE 
    "Enable the tracy profiler logic." ON 
    "CRIOULO_INCLUDE_TRACY" OFF)

if(NOT CRIOULO_INCLUDE_TRACY)
    set(CRIOULO_TRACY_ENABLE OFF CACHE BOOL "Enable the tracy profiler logic." FORCE)
endif()

set(CRIOULO_LOGGING_LEVEL "6" CACHE STRING "Set Logging Level.")
# -------------------------------------------------


# --------------------------------------------------
# Find Packages (External Dependencies)
# --------------------------------------------------
find_package(OpenGL REQUIRED)
# --------------------------------------------------


# --------------------------------------------------
# Configure GLM Library (External Dependency)
# --------------------------------------------------
file(GLOB_RECURSE GLM_HEADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/glm/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/glm/*.h"  
    "${CMAKE_CURRENT_SOURCE_DIR}/include/glm/*.inl"
)

add_library(glm INTERFACE)
target_sources(glm 
    INTERFACE 
        FILE_SET HEADERS
        BASE_DIRS 
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES 
            ${GLM_HEADER_FILES}
)
# --------------------------------------------------


# --------------------------------------------------
# Configure Tracy (External Dependency)
# --------------------------------------------------
add_library(tracy STATIC)
if(CRIOULO_TRACY_ENABLE)
    target_compile_definitions(tracy PUBLIC TRACY_ENABLE)
endif()
target_sources(tracy
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy/public/TracyClient.cpp"

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy/public"
        FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/tracy/public/tracy/Tracy.hpp"
)
# --------------------------------------------------


# --------------------------------------------------
# Configure spdlog (External Dependency)
# --------------------------------------------------
file(GLOB_RECURSE SPDLOG_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/spdlog/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/spdlog/*.h"  
    "${CMAKE_CURRENT_SOURCE_DIR}/include/spdlog/*.inl"
)

add_library(spdlog INTERFACE)
target_sources(spdlog
    INTERFACE
        FILE_SET HEADERS
        BASE_DIRS
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES
            ${SPDLOG_HEADERS}
)
# --------------------------------------------------


# --------------------------------------------------
# Crioulo Library (Main Target): Define Headers and Implementations Files
# --------------------------------------------------
#
# Add all implementation (.cpp, .c), header (.h, .hpp, inl), 
# shader (.vert, .frag, glsl) or other files here
#
# Avoid to use file(GLOB_RECURSE ...); Use set() instead
#
# Sort all files alphabetically with subfolders first; 
# Use the Solution Explorer as example for simplicity
# --------------------------------------------------
set(CRIOULO_SOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/renderer.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/init.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/global_context_manager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/include_shaders.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stb_include/stb_include.cpp"
)

set(CRIOULO_HEADERS_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/internal/util/include_shaders.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/internal/debug_gl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/internal/primitive_shapes.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/camera.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/global_context_manager.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/init.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/light.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/material.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/mesh.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/mesh_instance.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/material.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/renderer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/shader.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/texture.h"
)

# Seperate private form public header files
set(CRIOULO_PUBLIC_HEADER_FILES ${CRIOULO_HEADERS_FILES})
list(FILTER CRIOULO_PUBLIC_HEADER_FILES EXCLUDE REGEX ".*/internal/.*")

set(CRIOULO_PRIVATE_HEADER_FILES ${CRIOULO_HEADERS_FILES})
list(FILTER CRIOULO_PRIVATE_HEADER_FILES INCLUDE REGEX ".*/internal/.*")

list(APPEND CRIOULO_PRIVATE_HEADER_FILES   
        "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/khrplatform.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/stb_include/stb_include.h"
)

set(SHADER_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/internal/skybox.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/internal/skybox.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/pbr.glsl"
)
# --------------------------------------------------


#---------------------------------------------------
# Generate Header Files with embed Shader source
#---------------------------------------------------
generate_shaders("${CMAKE_CURRENT_SOURCE_DIR}/shaders" "${SHADER_FILES}" "${CMAKE_CURRENT_BINARY_DIR}/shaders/shaders" CRIOULO_GENERATED_SHADER_HEADER_FILES)
#---------------------------------------------------


# --------------------------------------------------
# Configure Crioulo Library (Main Target)
# --------------------------------------------------
add_library(Crioulo STATIC)

target_compile_features(Crioulo PUBLIC cxx_std_20)
set_target_properties(Crioulo PROPERTIES LINKER_LANGUAGE CXX)

if (MSVC)
    target_compile_options(Crioulo PRIVATE /utf-8)
	target_compile_options(Crioulo PRIVATE /W4)
else()
    target_compile_options(Crioulo PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_definitions(Crioulo PRIVATE SPDLOG_ACTIVE_LEVEL=${CRIOULO_LOGGING_LEVEL})

source_group("shaders" 
	FILES 
		${SHADER_FILES}
)

set_source_files_properties(
	${SHADER_FILES}  
	PROPERTIES 
		HEADER_FILE_ONLY TRUE
)

target_sources(Crioulo
    PRIVATE
        ${CRIOULO_SOURCE_FILES}
        ${SHADER_FILES}

    PRIVATE
        FILE_SET privateHeaders
        TYPE HEADERS
        BASE_DIRS 
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES 
            ${CRIOULO_PRIVATE_HEADER_FILES}

        FILE_SET shaderHeaders
        TYPE HEADERS
        BASE_DIRS 
            "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        FILES 
            ${CRIOULO_GENERATED_SHADER_HEADER_FILES}

    PUBLIC
        FILE_SET publicHeaders
        TYPE HEADERS
        BASE_DIRS 
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES 
            ${CRIOULO_PUBLIC_HEADER_FILES}
)

target_link_libraries(Crioulo 
    PRIVATE 
        OpenGL::GL
        spdlog
    PUBLIC
        glm
)

if(CRIOULO_INCLUDE_TRACY)
    target_link_libraries(Crioulo
        PUBLIC
            tracy
    )
else()
    target_link_libraries(Crioulo
        PRIVATE
            tracy
    )
endif()
# --------------------------------------------------