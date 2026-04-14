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


function(generate_shaders BASE_DIR SHADER_FILES TARGET_DIR OUT_HEADER_FILES)

    cmake_path(ABSOLUTE_PATH BASE_DIR BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

    foreach(SHADER_FILE ${SHADER_FILES})
        cmake_path(ABSOLUTE_PATH SHADER_FILE BASE_DIRECTORY BASE_DIR)
    endforeach()

    cmake_path(ABSOLUTE_PATH TARGET_DIR BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    file(MAKE_DIRECTORY "${TARGET_DIR}")

    set(GENERATED_HEADER_FILES "")

    foreach(SHADER_FILE ${SHADER_FILES})
        # Seperate filename from extension
        cmake_path(GET SHADER_FILE STEM FILENAME)
        cmake_path(GET SHADER_FILE EXTENSION FILE_EXTENSION)
        string(SUBSTRING "${FILE_EXTENSION}" 1 -1 FILE_EXTENSION)

        # Get relative path components
        cmake_path(RELATIVE_PATH SHADER_FILE BASE_DIRECTORY "${BASE_DIR}" OUTPUT_VARIABLE SHADER_RELATIVE_PATH)
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

        add_custom_command(
            OUTPUT ${HEADER_PATH}
            COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts/shader2header.py" 
                    ${SHADER_FILE} ${HEADER_PATH} ${VAR_NAME} "${NAMESPACE_NAME}"
            DEPENDS ${SHADER_FILE} "${CMAKE_CURRENT_SOURCE_DIR}/shader2header.py"
            COMMENT "Embedding shader: ${NAMESPACE_NAME}::${VAR_NAME}"
        )

        list(APPEND GENERATED_HEADER_FILES ${HEADER_PATH})
    endforeach()

    set(${OUT_HEADER_FILES} ${GENERATED_HEADER_FILES} PARENT_SCOPE)
endfunction()