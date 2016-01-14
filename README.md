# Cubby
Cubby engine is an open source voxel engine that is written in C++ and OpenGL and can be used to create easy and fast voxel based games and applications. The engine is written to be highly functional and very efficient at allowing voxel game creation and provides a simple framework for creating 3d voxel games.

## Features (Coming soon!)
* GLSL shader support.
* Support for deferred or forward rendering.
* Fully dynamic and unlimited lighting system using deferred lighting model.
* Advanced graphics shaders included - SSAO, shadow mapping, dynamic lighting, FXAA.
* Full 3d math and geometry library (vector, matrix, quaternion, plane, bezier).
* Extensive camera functionality, either 3d person or 1st person.
* In-built GUI that can be easily hooked up to gameplay functioanlity. (re-skinable)
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
* In-built voxel editor for creation/editing of in-game assets. 
* Audio and music playback.

## To-do List
* Create CubbyGame CubbyWindow and CubbyApplication classes.
* Refactor windows functionality into discrete files and using the CubbyWindow class.
* Add camera controls and movement to mouse and keyboard controls.
* Window and renderer resize functionality.
* Add proper license.
* Add Shader support to renderer.
* Add frame buffer support to renderer.
* Add selection of shaders to toggle different render modes for testing.
* Add ability to camera class to rotate around point (rotate position around view).
* Deferred lighting support.
* Lighting effects.
* Particle effects.
* Add weapon trails.
* Remove any GL calls from CubbyGame - Add InitZeroTexture() functionality to renderer.
* Use Cmake for build management.
* Add fullscreen support and toggle.
* Enable x64 build configurations.
* Phong lighting shader.
* Add basic lighting to scene - OpenGL lighting.
* Add pre-built release exe to root of git, so people can run exe without compiling - CubbyGame.exe
* Better SSAO shader.
  * Make deferred lighting and SSAO optional toggles separate.
  * Allow for phong shading and deferred lighting together.
* Shadow shader.
* Decouple shadow rendering from SSAO shader.
* Decouple dynamic lighting from SSAO shader.
* Better readme file.
* Split up CubbyGame file into CubbyRender, CubbyUpdate, CubbySetup(?), etc.
* App player class and put voxel character functionality in player class.
* Add ability to switch voxel character model.
* Rebuild vs2013 libs using non-dll version of linking
* Fix up cmake for the different visual studio versions and also for the different lib linking.
* Weapon trails controlled by the animation file.
* Create vs2012 libs and VS2012 project files
* Add blur shader.
* Implement MSAA (or other AA) in glsl shader.
* Convert shadow rendering calls to use renderer - remove GL from gamecode.
* Add LUA bindings.
* Make weapons trails time based, rather than "numpoints" based, better weapon trail alpha rendering.
* Switch over to using glm maths lib.
* Solution to allow for running the exe from Visual studio, because atm the working directory needs to be ../
* Textures on GUI.
* Frontend manager for textures and common GUI objects.
* Allow camera rotations around world coordinates, so 'upside down' doesnt happen.
* Add full GUI skin textures and control to change 'theme' on GUI.
* Add limits to rotation so we dont go over and under the max/min up-axis.
* Add different camera modes, option toggle on GUI.
* Add player camera controls.
* Enable cross platform compiling and support, linux, mac
* Convert all data, media, folders to lowercase to fix Linux issues.
* When in auto-camera mode, slowly orient to behind the player, after a time.
* Add gamepad support.
* Fix up auto-camera mode for gamepad and remove hacks for storing the up, right, forward target camera vectors.
  * Store a camera-to-player vector in the player class and use this for camera offsets in auto-mode.
  * Only use the x, z components when working out the offset camera position, plus the players up from the facing.
* Proper first person mode camera.
* Implement mouse rotation camera mode for when in game mode.
* Turn cursor on/off and fix cursor in position when doing mouse rotations in game mode.
* Make camera auto-mode rotate to behind the player after a short delay.
* Settings file loading at startup, change options and rendering based on settings.
* Player alpha when camera zooms in.
* Add proper ini reader functionality - add other startup params to ini, resolution, fullscreen etc.
* Camera clipping mode.
* Add ability to create and render the Voxel objects/characters without merging, 2 vertex arrays, or rebuild?
* Add console and logger to the in-game gui for output and logging information.
  * Log when loading/saving model files.
  * Log on errors.
  * Allow input commands.
* Terrain generation via chunks.
  * Chunk loading and unloading based on player position
* Player movement on voxel world and terrain.
  * Walking up single block steps automatically.
* Create Cubby EXE launcher. (C# app)
* Convert shaders over to be more unified.
* Add VLD support to debug version, allow for memory leak detection.
* Add audio and music playback.
* Add scenery manager.
  * Add scenery to work, generation based on point distribution, poisson disc, etc.
* Add item manager.
* Create basic front-end.
* Create front end scene and character line up.
* Character creation and customization.
* Saving and loading chunks.
  * Group together chunks for saving, loading. Not indivisual
* Add character creator and customization from front-end.
* Add debug log information for model loading and rendering - num verts, tris, faces, etc.
* Add pause menu when ESC is pressed, and add popup in center of screen.
* Add options menu window to pause menu.
* Add exit/quit popup when the game is closed or quit using various exit methods.
* Add voxel editor.
* Add texture support for voxel blocks.

## License
This software is provided under the **GNU General Public License**, more information is provided in the [license file](https://github.com/MineScroll/Cubby/blob/master/LICENSE.md).