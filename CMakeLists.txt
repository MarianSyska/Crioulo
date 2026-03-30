cmake_minimum_required(VERSION 4.2)
project(Crioulo)
include(CMakeDependentOption)

option(CRIOULO_INCLUDE_TRACY "Include tracy profiling" OFF)
cmake_dependent_option(CRIOULO_TRACY_ENABLE 
    "Enable the tracy profiler logic" ON 
    "CRIOULO_INCLUDE_TRACY" OFF)

if(NOT CRIOULO_INCLUDE_TRACY)
    set(CRIOULO_TRACY_ENABLE OFF CACHE BOOL "Enable the tracy profiler logic" FORCE)
endif()

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
            "include"
        FILES 
            ${GLM_HEADER_FILES}
)


# Configure Tracy
add_library(tracy STATIC)
if(CRIOULO_TRACY_ENABLE)
    target_compile_definitions(tracy PUBLIC TRACY_ENABLE)
endif()
target_sources(tracy
    PUBLIC
        "lib/tracy/public/TracyClient.cpp"

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            "lib/tracy/public"
        FILES
            "lib/tracy/public/tracy/Tracy.hpp"
)


set(SOURCES 
    "src/core/renderer.cpp" 
    "src/glad.c"
    "src/core/global_context_manager.cpp"
)

file(GLOB_RECURSE ALL_CRIOULO_HEADERS CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Crioulo/*.hpp" 
)

set(PUBLIC_CRIOULO_HEADER_FILES ${ALL_CRIOULO_HEADERS})
list(FILTER PUBLIC_CRIOULO_HEADER_FILES EXCLUDE REGEX ".*/internal/.*")

set(PRIVATE_CRIOULO_HEADER_FILES ${ALL_CRIOULO_HEADERS})
list(FILTER PRIVATE_CRIOULO_HEADER_FILES INCLUDE REGEX ".*/internal/.*")


add_library(Crioulo STATIC)

target_compile_features(Crioulo PUBLIC cxx_std_20)
set_target_properties(Crioulo PROPERTIES LINKER_LANGUAGE CXX)

target_sources(Crioulo
    PRIVATE
        ${SOURCES}

    PRIVATE
        FILE_SET privateHeaders
        TYPE HEADERS
        BASE_DIRS 
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES 
            ${PRIVATE_CRIOULO_HEADER_FILES}
            "${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/include/KHR/khrplatform.h"

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