function(capitalize_string INPUT_STR OUTPUT_VAR)
    # 1. Get first char and the rest
    string(SUBSTRING "${INPUT_STR}" 0 1 FIRST_CHAR)
    string(SUBSTRING "${INPUT_STR}" 1 -1 REMAINDER)
    
    # 2. Fix casing
    string(TOUPPER "${FIRST_CHAR}" FIRST_CHAR)
    string(TOLOWER "${REMAINDER}" REMAINDER)
    
    # 3. Combine and return to the caller
    set(${OUTPUT_VAR} "${FIRST_CHAR}${REMAINDER}" PARENT_SCOPE)
endfunction()

function(generate_shaders SOURCE_DIR TARGET_DIR)

    cmake_path(IS_RELATIVE SOURCE_DIR IS_RELATIVE_SOURCE_DIR)
    if(IS_RELATIVE_SOURCE_DIR)
        set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_DIR}")
    endif()

    cmake_path(IS_RELATIVE TARGET_DIR IS_RELATIVE_TARGET_DIR)
    if(IS_RELATIVE_TARGET_DIR)
        set(TARGET_DIR "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}")
    endif()

    file(MAKE_DIRECTORY "${TARGET_DIR}")
    file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS "${SOURCE_DIR}/*.vert" "${SOURCE_DIR}/*.frag" "${SOURCE_DIR}/*.glsl")

    foreach(SHADER_PATH ${SHADER_FILES})
        # Seperate filename from extension
        cmake_path(GET SHADER_PATH STEM FILENAME)
        cmake_path(GET SHADER_PATH EXTENSION FILE_EXTENSION)
        string(SUBSTRING "${FILE_EXTENSION}" 1 -1 FILE_EXTENSION)

        # Get relative path components
        cmake_path(RELATIVE_PATH SHADER_PATH BASE_DIRECTORY "${SOURCE_DIR}" OUTPUT_VARIABLE SHADER_RELATIVE_PATH)
        cmake_path(GET SHADER_RELATIVE_PATH PARENT_PATH SHADER_FOLDER_PATH)

        # Create list of path components
        string(REPLACE "/" ";" SHADER_FOLDER_PATH_COMPONENTS "${SHADER_FOLDER_PATH}")

        # Generate namespace name
        set(SHADER_FOLDER_PATH_COMPONENTS_CAPITALIZED "")
        foreach(PATH_COMPONENT IN LISTS SHADER_FOLDER_PATH_COMPONENTS)
            capitalize_string(${PATH_COMPONENT} PATH_COMPONENT_CAPITALIZED)
            list(APPEND SHADER_FOLDER_PATH_COMPONENTS_CAPITALIZED "${PATH_COMPONENT_CAPITALIZED}")
        endforeach()

        list(JOIN SHADER_FOLDER_PATH_COMPONENTS_CAPITALIZED "::" NAMESPACE_NAME)

        set(VAR_NAME "${FILENAME}_${FILE_EXTENSION}")
        set(HEADER_PATH "${TARGET_DIR}/${SHADER_FOLDER_PATH}/${VAR_NAME}.hpp")

        message("Shader Source Path: ${SHADER_PATH}")
        message("Header Target Path: ${HEADER_PATH}")
        message("Variable Name: ${VAR_NAME}")
        message("Namespace Name: ${NAMESPACE_NAME}")

        add_custom_command(
            OUTPUT ${HEADER_PATH}
            COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts/shader2header.py" 
                    ${SHADER_PATH} ${HEADER_PATH} ${VAR_NAME} ${NAMESPACE_NAME}
            DEPENDS ${SHADER_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/shader2header.py"
            COMMENT "Embedding shader: ${NAMESPACE_NAME}::${VAR_NAME}"
        )
    endforeach()

endfunction()