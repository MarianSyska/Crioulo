# Crioulo

A modern C++20 3D rendering library built on OpenGL. Educational project exploring real-time graphics fundamentals with a focus on clean API design, Physically Based Rendering, and modern C++ practices.

## Features

- **Physically Based Rendering** — Cook-Torrance BRDF with GGX distribution, Smith geometry, and Fresnel-Schlick
- **C++20 Concepts** — Compile-time vertex attribute validation via `VertexAttributeObject` concept
- **RAII Resource Management** — OpenGL objects (VAO, VBO, EBO, textures, shaders) wrapped with RAII; shared ownership with context-aware custom deleters
- **Uniform Buffer Objects** — Three binding points for transform matrices, point lights, and scene data; efficient batched rendering
- **Point Lighting** — Up to 4 dynamic point lights per draw call via UBO
- **Skybox Rendering** — Cube map textures with depth-disabled rendering
- **Shader Include System** — Custom `#include` preprocessing for GLSL shader reuse
- **Multi-Context Support** — `GlobalContextManager` singleton manages multiple OpenGL contexts
- **Profiling** — Optional Tracy integration for frame-level performance analysis
- **Logging** — spdlog-based async logging
- **Primitive Shapes** — `constexpr` cube and plane generation utilities
- **X-Macro Enums** — Type-safe OpenGL state enums (depth test, culling) mapped to GL constants

## Requirements

- **CMake** 4.2+
- **C++20** compiler (MSVC 2022, Clang 16+, GCC 13+)
- **OpenGL** 4.3+ (for debug output and UBO support)
- Python 3.x (for shader code generation)

## Dependencies (vendored)

| Library | Purpose |
|---|---|
| [glad](https://glad.dav1d.de/) | OpenGL function loading |
| [glm](https://github.com/g-truc/glm) | Mathematics (vectors, matrices, transforms) |
| [spdlog](https://github.com/gabime/spdlog) | Asynchronous logging |
| [stb_include](https://github.com/nothings/stb) | Shader `#include` preprocessing |
| [Tracy](https://github.com/wolfpld/tracy) | Profiling (optional, off by default; git submodule) |

## Build

```bash
git clone --recursive https://github.com/MarianSyska/Crioulo.git
cd Crioulo
cmake -S . -B out/build -D CMAKE_BUILD_TYPE=Release
cmake --build out/build
```

### Options

| Option | Default | Description |
|---|---|---|
| `CRIOULO_INCLUDE_TRACY` | `OFF` | Include Tracy profiler |
| `CRIOULO_TRACY_ENABLE` | `ON` *(dep)* | Enable Tracy profiling |
| `CRIOULO_LOGGING_LEVEL` | `6` | spdlog active log level |

## Integration

### As a CMake subdirectory

```cmake
add_subdirectory(path/to/Crioulo)
target_link_libraries(your_target PRIVATE Crioulo)
```

Crioulo sets `cxx_std_20` as a public compile feature and exposes `glm` publicly. All include paths are handled automatically.

### Manual integration

Build artifacts are located in `out/build/`:
- **Library**: `out/build/Release/Crioulo.lib` (or `Debug/Crioulo.lib`)
- **Headers**: `include/Crioulo/`

## Quick Start

```cpp
#include <Crioulo/init.h>
#include <Crioulo/renderer.h>

// Your window/context must implement Crioulo::IContext
class WindowContext : public Crioulo::IContext {
    void makeCurrent() override { /* ... */ }
    void doneCurrent() override { /* ... */ }
};

int main() {
    WindowContext context;

    // Initialize OpenGL, logging, and context
    Crioulo::initialize(/* getProcAddress */, context);

    // Create renderer with default settings
    Crioulo::Renderer renderer(context, {});
    renderer.setCamera(/* your camera */);

    // Define vertex format using the VertexAttributeObject concept
    struct MyVertex {
        glm::vec3 position;
        glm::vec2 texCoord;
        static constexpr std::array attributes{
            VertexAttributeDetails{0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)offsetof(MyVertex, position)},
            VertexAttributeDetails{1, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)offsetof(MyVertex, texCoord)},
        };
    };
    static_assert(VertexAttributeObject<MyVertex>);

    // Load mesh, shader, texture
    auto mesh = renderer.loadMesh(MeshData<MyVertex>{...});
    auto shader = renderer.loadShader(vertexCode, fragmentCode);
    auto texture = renderer.loadTexture(textureData);

    // Create material and instance
    Material material(shader, {{texture, "uTexture"}});
    auto* instance = renderer.createMeshInstance(mesh, material);
    instance->setTransform(glm::mat4(1.0f));

    // Render loop
    while (running) {
        renderer.adjustViewport(0, 0, width, height);
        renderer.drawScene();
    }
}
```

## Architecture

```
include/Crioulo/            Public API headers
include/Crioulo/internal/   Private/internal headers
src/core/                   Core implementation
src/util/                   Shader include preprocessor
shaders/                    GLSL source files
cmake/                      CMake modules & code generation scripts
lib/tracy/                  Tracy profiler submodule
```

### Key Classes

| Class | Role |
|---|---|
| `Renderer` | Main facade — owns scene state, manages OpenGL, orchestrates drawing |
| `Mesh` | Geometry container (VAO/VBO/EBO); ref-counted via `shared_ptr` |
| `MeshInstance` | Per-instance transform + material binding |
| `Shader` | Compiled GLSL program; private constructor, created via `Renderer` |
| `Texture` | 2D or cube map texture; ref-counted via `shared_ptr` |
| `Material` | Shader + texture slot bindings |
| `Camera` | View/projection matrix generation |
| `PointLight` | Aligned for std140 UBO layout |
| `GlobalContextManager` | Meyer's singleton managing OpenGL context currency |

### UBO Binding Points

| Binding | Name | Contents |
|---|---|---|
| 0 | `uTransformMatrices` | View + projection matrices (2 × mat4) |
| 1 | `uPointLights` | Up to 4 point lights |
| 2 | `uSceneData` | Light count + camera position |

## Roadmap

- [x] PBR Support
- [x] Automatic `#include` handling for shaders
- [ ] Screen Space Ambient Occlusion
- [ ] Screen Space Reflections
- [ ] Static Shadow Mapping
- [ ] Instanced Rendering
- [ ] HDR Support
- [ ] Gamma Correction
- [ ] Deferred Rendering
- [ ] Improved Logging
- [ ] Tests
- [ ] Clang-Format & clang-tidy
- [ ] Bloom Effect

## License

MIT
