![Jewel3D Logo](Resources\JewelIcon.png)

# What is Jewel3D?
Jewel3D is a free C++ game development framework for Windows. It features a powerful Entity-Component-System
and fine-grain control of both 3D and 2D rendering.

# Why use Jewel3D?
The framework facilitates expressive and succinct code that is both easy to read, and to use.
With a thin API and modern C++14 concepts, Jewel3D provides powerful features without feeling clunky.

Highlight features include:
* [A Rich shader workflow](Resources\Docs\Shader.md)
* [Efficient entity queries](Resources\Docs\Entity.md)
* A modular and extendable particle system
* Audio loading and playback
* Texture and Mesh loading and rendering
* Freetype driven Text rendering
* Event dispatching and response
* Robust fixed-timestep updates
* A complete math library
* Keyboard/Mouse/Xbox-Gamepad support
* Local networking
* C++ Reflection

# Quick Start
1. Run "INSTALL.cmd" to save the checkout path to the environment and instal the project template.
	- You might have to restart your computer for this to take effect.
2. Run "BUILD.cmd" to compile the engine and tools.
3. Create a new game project by selecting ```Add New Project -> Visual C++ -> Jewel3D Project```. 
	- Due to a known bug in Visual Studio, source files are missing from the template project. 
	- Feel free to manually copy the project from ```\Jewel3D\Tools\TemplateProject\``` until this is fixed.
	- Includes and link dependencies are still set up correctly.
	
Code samples are available and maintained [here](https://github.com/EmilianC/Jewel3D-Samples).

# Upcoming Features
Jewel3D is always being updated and improved. See the [Trello board](https://trello.com/b/Oc2GFT2A/jewel3d) to follow development.

Future highlight features include:
* Custom GUI support
* A fully featured world/UI editor and asset manager
* Bullet physics integration
* Custom Memory management
* C++ Reflection driven features
	* Animations
	* Serialization and Deserialization
	* Networking
	* Seamless editor integration of user components

# Dependancies
All Dependencies used under their respective licenses. Copyright is held by their respective owners.
* [GLEW 2.0.0](http://glew.sourceforge.net/)
* [FreeType 2](https://www.freetype.org/)
* [Catch 1.6.0](https://github.com/philsquared/Catch)
* [OpenAL Soft 1.17.2](https://github.com/kcat/openal-soft)
* [SOIL ext](https://github.com/fenbf/SOIL_ext)
* [dirent 1.21](https://github.com/tronkko/dirent)
* [XInput 1.4](https://msdn.microsoft.com/en-us/library/windows/desktop/ee417001(v=vs.85).aspx)

# Notes
* Jewel3D is built with Visual Studio 2015 Community for Windows and is not tested on other compilers, IDEs, or platforms.
* Jewel3D requires at least OpenGL 3.3.
* Jewel3D is provided under the MIT License.
