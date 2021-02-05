![Jewel3D Logo](docs/JewelLogo.png)

# What is Jewel3D?
Jewel3D is a free C++ game development framework for Windows. It features a powerful Entity-Component-System
and fine-grain control of both 2D and 3D rendering.

# Why use Jewel3D?
The framework facilitates expressive and succinct code that is both easy to read, and to use.
With a thin API and C++20 features, Jewel3D provides powerful features without feeling clunky.

Highlight features include:
* [A rich shader workflow](docs/Shader.md)
* [Efficient entity queries](docs/Entity.md)
* [Extendable asset management](docs/AssetManager.md)
* A modular and extendable particle system
* Audio powered by OpenAL
* Mesh and Texture loading
* [FreeType text rendering](docs/Text.md)
* Event dispatching and response
* Robust fixed-timestep updates
* A complete math library
* Keyboard/Mouse/Xbox-Controller support
* Local networking

# Quick Start
1. Run "INSTALL.cmd" to save the checkout path to the environment and install project templates.
	- You might have to restart your computer for this to take effect.
2. Run "BUILD.cmd" to compile the engine and tools.
3. Create a new game project by selecting `Add New Project -> Visual C++ -> Jewel3D Project`.

Code samples are available and maintained [here](https://github.com/EmilianC/Jewel3D-Samples).

# Upcoming Features
Jewel3D is always being updated and improved. See the [Trello board](https://trello.com/b/Oc2GFT2A/jewel3d) to follow development.

Future highlight features include:
* GUI support
* A fully featured world/UI editor
* Bullet physics integration
* Configurable memory management
* C++ reflection driven features:
	* Animation of arbitrary values
	* Serialization and deserialization
	* Replication over networks
	* Seamless editor integration of user data-types

# Dependencies
All Dependencies used under their respective licenses. Copyright is held by their respective owners.
* [GLEW 2.0.0](http://glew.sourceforge.net/)
* [FreeType 2](https://www.freetype.org/)
* [Catch 2.7.2](https://github.com/philsquared/Catch)
* [OpenAL Soft 1.21.1](https://github.com/kcat/openal-soft)
* [Scaffold](https://github.com/EmilianC/scaffold)
* [SOIL ext](https://github.com/fenbf/SOIL_ext)
* [dirent 1.23.1](https://github.com/tronkko/dirent)
* [XInput 1.4](https://msdn.microsoft.com/en-us/library/windows/desktop/ee417001(v=vs.85).aspx)

# Notes
* Jewel3D is built and tested on Windows with the latest version of Visual Studio 2019 Community.
* Jewel3D requires at least OpenGL 3.3.
* Jewel3D is provided under the MIT License.
