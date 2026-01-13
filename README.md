![Gemcutter Logo](docs/Logo.png)

# What is Gemcutter?
Gemcutter is a free C++ game development framework for Windows. It features a powerful Entity-Component-System
and fine-grain control of both 2D and 3D rendering.

# Why use Gemcutter?
Gemcutter provides a programmer-centric modern C++ workflow that is expressive yet succinct.

The API is kept slim with a philosophy of minimizing boilerplate and unifying code paths.
For example: `Widgets`, `Sprites`, `Meshes`, and `Particles` all render the same way - a Renderable component with a Material and VertexArray.

Highlight features include:
* [A rich shader workflow](docs/Shader.md)
* [Queryable Entity-Component-System](docs/Entity.md)
* [Extendable asset management](docs/AssetManager.md)
* Run-time reflection
* A modular & extendable particle system
* Audio powered by [SoLoud](https://github.com/EmilianC/soloud)
* Mesh and Texture loading
* Dynamic GUI widgets
* [FreeType text rendering](docs/Text.md)
* Event handling & lifetime-safe delegates
* A robust fixed-timestep gameloop
* Keyboard/Mouse/Xbox-Controller support
* [ImGui](https://github.com/ocornut/imgui) integration
* A complete math library
* Local networking

# Quick Start
If you're looking to start a new project using Gemcutter, then you can use [this repository template](https://github.com/EmilianC/Gemcutter-Project-Template) to get started quickly.

If you just want to download the framework, then simply clone the repo and configure the project with CMake from the root folder:
```
> mkdir build
> cd build
> cmake ..                       # Default configuration, or
> cmake .. -DENABLE_SAMPLES=OFF  # Default configuration without downloading samples
```
And that's it! Any required git submodules are pulled automatically. You'll find your generated projects under the `build/` folder. Code samples can be found under `samples/` if they were enabled.

The following build configurations will be available:
| Config         | defines            | optimizations | asserts | tools |
|----------------|--------------------|---------------|---------|-------|
| Debug          | GEM_DEBUG, GEM_DEV | off           | on      | on    |
| RelWithDebInfo | GEM_DEV            | on            | off     | on    |
| Release        | GEM_PRODUCTION     | on            | off     | off   |

# Development Roadmap
See the [Trello board](https://trello.com/b/Oc2GFT2A/gemcutter) to follow development in more detail.

### v1.0
- [ ] A fully featured 3D world editor
- [ ] Input remapping utilities
- [ ] Serialization & deserialization
- [ ] Single draw-call text rendering
### v1.x
- CLI asset manager in Rust
- Native Linux support
- Memory management utilities
- Physics library integration
- UI screen designer

# Dependencies
All Dependencies used under their respective licenses. Copyright is held by their respective owners.
* [GLEW 2.2.0+](https://github.com/Perlmint/glew-cmake)
* [FreeType 2.13.3](https://github.com/EmilianC/freetype)
* [Catch 2.13.10](https://github.com/catchorg/Catch2/tree/v2.x)
* [SoLoud](https://github.com/EmilianC/soloud)
* [Scaffold](https://github.com/EmilianC/scaffold)
* [SOIL2 1.31](https://github.com/SpartanJ/SOIL2)
* [dirent 1.23.2](https://github.com/tronkko/dirent)
* [XInput 1.4](https://msdn.microsoft.com/en-us/library/windows/desktop/ee417001(v=vs.85).aspx)
* [Imgui 1.85](https://github.com/ocornut/imgui)
* [Loupe](https://github.com/EmilianC/Loupe)

# Notes
* Gemcutter is built and tested on Windows with the latest version of Visual Studio.
* Gemcutter requires at least OpenGL 3.3.
* Gemcutter is provided under the MIT License.
