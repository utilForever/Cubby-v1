<img src="https://github.com/MineScroll/Cubby/blob/master/CubbyLogo.png" align="right" width="200" height="200" />

# Cubby

Cubby is an open source Voxel Engine, and game, that is written in C++ and OpenGL and can be used to create easy and fast voxel based games and applications. The engine is written to be highly functional and very efficient at allowing voxel game creation and provides a simple framework for creating 3D voxel games.

The main code of this repository is based on [AlwaysGeeky's Vox](https://github.com/AlwaysGeeky/Vox).

[![License](https://img.shields.io/badge/Licence-GNU-blue.svg)](https://github.com/MineScroll/Cubby/blob/master/LICENSE.md)

## Features

* 3D voxel engine with cuboid and box-based rendering aesthetic. 
* GLSL shader support.
* Support for deferred or forward rendering.
* Fully dynamic and unlimited lighting system using deferred lighting model.
* Advanced graphics shaders included - SSAO, shadow mapping, dynamic lighting, FXAA.
* Full 3D math and geometry library (vector, matrix, quaternion, plane, bezier).
* Extensive camera functionality, either 3rd person or 1st person.
* In-built GUI that can be easily hooked up to gameplay functioanlity (re-skinable).
* Qubicle Binary (.qb) voxel model loading.
* Polygon model loading via ms3d and obj files.
* Keyframe and skeleton animation support.
* Basic 'player' functionality - movement, controls, animation.
* 'Voxel character' encapsulating character behaviours - idle looking, breathing, etc.
* Facial animation support for voxel characters.
* Voxel scenery and world items.
* Block/cube particle effect system.
* Modding and scripting support via LUA.
* Gamepad and controller support.
* Voxel world and terrain generation.
* Procedural content creation.
* Audio and music playback.

## Gameplay Features

Some parts of the Cubby gameplay logic are also incorporated into this source code repo, to better facilitate code sharing and the different sub-systems that make up the game. Some of the gameplay features of Cubby are as follows:

* Player movement and controls.
* Inventory and item management.
* NPC AI and player interations.
* Quets and gameplay objectives.
* Enemies and monsters.
* Combat using melee and ranged weapons, magic and spellcasting.
* Farming and crop management.
* Character creation and player generation.
* World customization and creation elements.

## Setup
Setting up and compiling the project is a fairly simple process. Cmake can be used to build the project files for each environment and for different platforms, depending on what target platform you are developing on.

## Building the Source Code

Visual Studio projects are included inside the ```\Projects``` sub-folder (Currently, Visual Studio 2015 only).

## Running

The Cubby executable (Cubby.exe) needs to be run from the root directory so that it has access from its running directory to the ```\Resources``` folder that contains all the game's content files. If you want to run the game's executable from within the Visual Studio projects (for debugging) then you will need to set the working directy in the 'debugging' options to ```'..\'``` since the Visual Studio projects files reside various in sub-directories above the root.

## Documentation

TBA

## Changelog

TBA

## License

This software is provided under the **GNU General Public License**, more information can be found in the [license file](https://github.com/MineScroll/Cubby/blob/master/LICENSE.md).

## How To Contribute

Contributions are always welcome, either reporting issues/bugs or forking the repository and then issuing pull requests when you have completed some additional coding that you feel will be beneficial to the main project. If you are interested in contributing in a more dedicated capacity, then please contact me.

## Contact

You can contact me via Facebook at http://www.facebook.com/utilForever. I am always happy to answer questions or help with any issues you might have, and please be sure to share any additional work or your creations with me, I love seeing what other people are making.

## Screenshots

<img src="https://github.com/MineScroll/Cubby/blob/master/Screenshots/Normal.PNG" width="300" height="300" />

<img src="https://github.com/MineScroll/Cubby/blob/master/Screenshots/LootingTreasure.PNG" width="300" height="300" />

## Dependencies

* fmod
* freetype
* glew
* glfw
* glm
* inih
* libnoise
* lua
* selene
* simplex
* tinythread