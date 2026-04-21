# Crioulo OpenGL Rendering Library

Crioulo is a 3D rendering library written in C++ leveraging the OpenGL API. It serves as an educational project to explore the fundamentals of computer graphics and OpenGL. The repository CriouloDemo provides comprehensive implementation examples.

## Build Instructions

Crioulo uses CMake as its build system. By default, the project is configured to build as a static library.

Build:
```bash
    cd path/to/lib/Crioulo
    cmake -S . -B out/build -D CMAKE_BUILD_TYPE=Release
    cmake --build out/build
```

## Integration

You can either build the lib and integrate it manually. The necessary build artifacts can be found:

- Libary File: After the build, the libary can be usually found under "out/build/Release/Crioulo.lib" or "out/build/Crioulo.lib"
- Header Files: Headers can be found under "include/Crioulo"

The easiest way to use Crioulo in your own project is to include it as a subfolder and link to it directly. This handles all header paths and dependencies automatically. Place the whole repository either as a submodule or per copy.

Add this to your own CmakeLists.txt:
```cmake
    add_subdirectory(*Place the path to the repository*)
    target_link_libraries(*Place you target name* PRIVATE Crioulo)
```

## Roadmap

- Add PBR Support
- Add automatic #include handling for shaders
- Add Screen Space Ambient Occlusion
- Add Screen Space Reflections
- Add Static Shadow mapping
- Instanced Rendering
- HDR Support
- Gamma Correction
- Deferred Rendering option
- Improve Logging
- Create tests
- Implement Clang-Format and clang-tidy
- Add Bloom effect
