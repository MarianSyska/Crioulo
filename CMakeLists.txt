cmake_minimum_required(VERSION 4.2)
project(Crioulo)

include(CMakeDependentOption)
include("cmake/GenerateShaderHeaders.cmake")


# Define Options and Cache Variables
option(CRIOULO_INCLUDE_TRACY "Include tracy profiling." OFF)
cmake_dependent_option(CRIOULO_TRACY_ENABLE 
    "Enable the tracy profiler logic." ON 
    "CRIOULO_INCLUDE_TRACY" OFF)

if(NOT CRIOULO_INCLUDE_TRACY)
    set(CRIOULO_TRACY_ENABLE OFF CACHE BOOL "Enable the tracy profiler logic." FORCE)
endif()

set(CRIOULO_LOGGING_LEVEL "6" CACHE STRING "Set Logging Level.")

if(MSVC)
    add_compile_options(/utf-8)
endif()


# Find Packages
find_package(OpenGL REQUIRED)


# Configure GLM Libary
file(GLOB_RECURSE GLM_HEADER_FILES CONFIGURE_DEPENDS 
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


# Configure Tracy
add_library(tracy STATIC)
if(CRIOULO_TRACY_ENABLE)
    target_compile_definitions(tracy PUBLIC TRACY_ENABLE)
endif()
target_sources(tracy
    PRIVATE
        "lib/tracy/public/TracyClient.cpp"

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            "lib/tracy/public"
        FILES
            "lib/tracy/public/tracy/Tracy.hpp"
)

# Configure spdlog
file(GLOB_RECURSE SPDLOG_HEADERS CONFIGURE_DEPENDS 
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

generate_shaders("shaders" "${CMAKE_CURRENT_BINARY_DIR}/shaders/shaders")

# Set 
set(SOURCES 
    "src/core/renderer.cpp" 
    "src/core/init.cpp"
    "src/glad.c"
    "src/core/global_context_manager.cpp"
)

set(CRIOULO_SHADER_HEADER_FILES 
    "${CMAKE_CURRENT_BINARY_DIR}/shaders/shaders/internal/skybox_vert.hpp"
    "${CMAKE_CURRENT_BINARY_DIR}/shaders/shaders/internal/skybox_frag.hpp"
)

file(GLOB_RECURSE ALL_CRIOULO_HEADERS CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/*.hpp" 
)

set(PUBLIC_CRIOULO_HEADER_FILES ${ALL_CRIOULO_HEADERS})
list(FILTER PUBLIC_CRIOULO_HEADER_FILES EXCLUDE REGEX ".*/internal/.*")

set(PRIVATE_CRIOULO_HEADER_FILES ${ALL_CRIOULO_HEADERS})
list(FILTER PRIVATE_CRIOULO_HEADER_FILES INCLUDE REGEX ".*/internal/.*")

file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.glsl"
)

add_library(Crioulo STATIC)

target_compile_features(Crioulo PUBLIC cxx_std_20)
set_target_properties(Crioulo PROPERTIES LINKER_LANGUAGE CXX)
if (MSVC)
	target_compile_options(Crioulo PRIVATE /W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
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
        ${SOURCES}
        ${SHADER_FILES}

    PRIVATE
        FILE_SET privateHeaders
        TYPE HEADERS
        BASE_DIRS 
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES 
            ${PRIVATE_CRIOULO_HEADER_FILES}
            "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/khrplatform.h"

        FILE_SET shaderHeaders
        TYPE HEADERS
        BASE_DIRS 
            "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        FILES 
            ${CRIOULO_SHADER_HEADER_FILES}

    PUBLIC
        FILE_SET publicHeaders
        TYPE HEADERS
        BASE_DIRS 
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES 
            ${PUBLIC_CRIOULO_HEADER_FILES}
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