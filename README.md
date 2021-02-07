![Gemcutter Logo](docs/Logo.png)

# What is Gemcutter?
Gemcutter is a free C++ game development framework for Windows. It features a powerful Entity-Component-System
and fine-grain control of both 2D and 3D rendering.

# Why use Gemcutter?
The framework facilitates expressive and succinct code that is both easy to read, and to use.
With a thin API and C++20 features, Gemcutter provides powerful features without feeling clunky.

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

Code samples are available and maintained [here](https://github.com/EmilianC/Gemcutter-Samples).

# Upcoming Features
Gemcutter is always being updated and improved. See the [Trello board](https://trello.com/b/Oc2GFT2A/gemcutter) to follow development.

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
* Gemcutter is built and tested on Windows with the latest version of Visual Studio.
* Gemcutter requires at least OpenGL 3.3.
* Gemcutter is provided under the MIT License.
