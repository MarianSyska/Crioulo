# Crioulo OpenGL Rendering Library

Crioulo is a 3D rendering library written in C++ leveraging the OpenGL API. It serves as an educational project to explore the fundamentals of computer graphics and OpenGL.The repository CriouloDemo provides comprehensive implementation examples.

## Build Instructions

Crioulo uses CMake as its build system. By default, the project is configured to build as a static library.

Build:
    cd path/to/lib/Crioulo
    cmake -S . -B out/build -D CMAKE_BUILD_TYPE=Release
    cmake --build out/build

## Integration

You can either build the lib and integrate it manually. The necessary build artifacts can be found:

- Libary File: After the build, the libary can be usually found under "out/build/Release/Crioulo.lib" or "out/build/Crioulo.lib"
- Header Files: Headers can be found under "include/Crioulo"

The easiest way to use Crioulo in your own project is to include it as a subfolder and link to it directly. This handles all header paths and dependencies automatically. Place the whole repository either as a submodule or per copy.

Add this to your own CmakeLists.txt:
    add_subdirectory(*Place the path to the repository*)
    target_link_libraries(*Place you target name* PRIVATE Crioulo)

## Roadmap


