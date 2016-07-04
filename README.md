<img align="center" src="https://github.com/MineScroll/Cubby/blob/master/CubbyLogo.png">

# Cubby

Cubby is an open source Voxel Engine, and game, that is written in C++ and OpenGL and can be used to create easy and fast voxel based games and applications. The engine is written to be highly functional and very efficient at allowing voxel game creation and provides a simple framework for creating 3D voxel games.

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

* Player movement and controls.
* Inventory and item management.
* NPC AI and player interations.
* Quets and gameplay objectives.
* Enemies and monsters.
* Combat using melee and ranged weapons, magic and spellcasting.
* Farming and crop management.
* Character creation and player generation.
* World customization and creation elements.

## To-do List

- [ ] Create CubbyGame CubbyWindow and CubbyApplication classes.
- [ ] Refactor windows functionality into discrete files and using the CubbyWindow class.
- [ ] Add camera controls and movement to mouse and keyboard controls.
- [ ] Window and renderer resize functionality.
- [ ] Add proper license.
- [ ] Add Shader support to renderer.
- [ ] Add frame buffer support to renderer.
- [ ] Add selection of shaders to toggle different render modes for testing.
- [ ] Add ability to camera class to rotate around point (rotate position around view).
- [ ] Deferred lighting support.
- [ ] Lighting effects.
- [ ] Particle effects.
- [ ] Add weapon trails.
- [ ] Remove any GL calls from CubbyGame - Add InitZeroTexture() functionality to renderer.
- [ ] Use Cmake for build management.
- [ ] Add fullscreen support and toggle.
- [ ] Enable x64 build configurations.
- [ ] Phong lighting shader.
- [ ] Add basic lighting to scene - OpenGL lighting.
- [ ] Add pre-built release exe to root of git, so people can run exe without compiling - CubbyGame.exe
- [ ] Better SSAO shader.
  - [ ] Make deferred lighting and SSAO optional toggles separate.
  - [ ] Allow for phong shading and deferred lighting together.
- [ ] Shadow shader.
- [ ] Decouple shadow rendering from SSAO shader.
- [ ] Decouple dynamic lighting from SSAO shader.
- [ ] Better readme file.
- [ ] Split up CubbyGame file into CubbyRender, CubbyUpdate, CubbySetup(?), etc.
- [ ] App player class and put voxel character functionality in player class.
- [ ] Add ability to switch voxel character model.
- [ ] Fix up cmake for the different visual studio versions and also for the different lib linking.
- [ ] Weapon trails controlled by the animation file.
- [ ] Add blur shader.
- [ ] Implement MSAA (or other AA) in glsl shader.
- [ ] Convert shadow rendering calls to use renderer - remove GL from gamecode.
- [ ] Add LUA bindings.
- [ ] Make weapons trails time based, rather than "numpoints" based, better weapon trail alpha rendering.
- [ ] Switch over to using glm maths lib.
- [ ] Solution to allow for running the exe from Visual studio, because atm the working directory needs to be ../
- [ ] Textures on GUI.
- [ ] Frontend manager for textures and common GUI objects.
- [ ] Allow camera rotations around world coordinates, so 'upside down' doesnt happen.
- [ ] Add full GUI skin textures and control to change 'theme' on GUI.
- [ ] Add limits to rotation so we dont go over and under the max/min up-axis.
- [ ] Add different camera modes, option toggle on GUI.
- [ ] Add player camera controls.
- [ ] Enable cross platform compiling and support, linux, mac
- [ ] Convert all data, media, folders to lowercase to fix Linux issues.
- [ ] When in auto-camera mode, slowly orient to behind the player, after a time.
- [ ] Add gamepad support.
- [ ] Fix up auto-camera mode for gamepad and remove hacks for storing the up, right, forward target camera vectors.
  - [ ] Store a camera-to-player vector in the player class and use this for camera offsets in auto-mode.
  - [ ] Only use the x, z components when working out the offset camera position, plus the players up from the facing.
- [ ] Proper first person mode camera.
- [ ] Implement mouse rotation camera mode for when in game mode.
- [ ] Turn cursor on/off and fix cursor in position when doing mouse rotations in game mode.
- [ ] Make camera auto-mode rotate to behind the player after a short delay.
- [ ] Settings file loading at startup, change options and rendering based on settings.
- [ ] Player alpha when camera zooms in.
- [ ] Add proper ini reader functionality - add other startup params to ini, resolution, fullscreen etc.
- [ ] Camera clipping mode.
- [ ] Add ability to create and render the Voxel objects/characters without merging, 2 vertex arrays, or rebuild?
- [ ] Terrain generation via chunks.
  - [ ] Chunk loading and unloading based on player position
- [ ] Create Cubby EXE launcher. (C# app)
- [ ] Debug settings option to toggle the Debug GUI from the settings ini file.
- [ ] Settings in ini to run straight into game, with player and camera controls - or run into front-end etc.
- [ ] Ordering of the chunk loading based on the camera/player position, so that loading filters outwards from player.
- [ ] Add back in skyboxes and support tweeningblending from one skybox to the other.
- [ ] Wait before spawning player in world, until world is generated.
- [ ] Better physics response and detection when jumping and from above
- [ ] Add version string to config ini file and display on GUI.
- [ ] Add DEBUG/RELEASE switch depending on what build was made, display on GUI.
- [ ] Player movement on voxel world and terrain.
  - [ ] Walking up single block steps automatically.
  - [ ] Smooth out camera when doing single 1-size block step up animation. (Was because of different interpolator values) 
- [ ] Add exit/quit popup when the game is closed or quit using various exit methods.
- [ ] Add pause menu when ESC is pressed, and add popup in center of screen.
- [ ] Add back in frustum culling for chunk rendering.
- [ ] Player collision with items.
- [ ] Player interaction with items.
- [ ] Cinematic letterbox mode.
- [ ] Add console and logger to the in-game gui for output and logging information.
  - [ ] Log when loading/saving model files.
  - [ ] Log to output file and log files as well as console GUI.
  - [ ] Remove old log messages, only allow a certain nunber of log messages to be shown.
  - [ ] Add debug log information for model loading and rendering - num verts, tris, faces, etc.
  - [ ] Log on errors.
  - [ ] Allow input commands.
  - [ ] Clear log text functionality
  - [ ] Rendering of many log messages is slow, output to file?
- [ ] Create basic front-end.
- [ ] Fix camera glitching when going in and out of pause menu (also quit menu).
- [ ] Animated text effects in the HUD, for pickups, combat, etc..
- [ ] Add biome manager
  - [ ] Add colour variation management to biome manager.
- [ ] Add item manager.
- [ ] Add inventory GUI.
- [ ] Add character GUI.
- [ ] Add options menu window to pause menu.
- [ ] Add mods menu window to pause menu.
  - [ ] Select mods that are already loaded and 'tick' their buttons. SelectLoadedMods() function called on each tab.
- [ ] Tidy up inventory and item managers!
  - [ ] Add back in TODO code (TODO : Add me back in).
  - [ ] Test ALL functionality with player equipping items and weapons.
  - [ ] Add hooks to GUI.
  - [ ] Player stats.
  - [ ] Loot GUI chest interactions.
  - [ ] Crafting GUI functionality.
  - [ ] Character GUI paperdoll render.
  - [ ] CharacterGUI player poses and animation stances.
  - [ ] CharacterGUI particle effects.
  - [ ] CharacterGUI dynamic lighting.
- [ ] Investigate continuous automatic builds on github when files are submitted to depot, using cmake and make.
- [ ] NPCs.
- [ ] Enemies.
  - [ ] Enemy spawners.
- [ ] Add scenery manager.
  - [ ] Flower generation.
  - [ ] Poisson-disc separation and generation for random placement of scenery.
  - [ ] Convert scenery manager to use instance rendering and the instance shader.
- [ ] Add Quest GUI.
- [ ] Create front end scene and character line up.
- [ ] Character creation and customization.
- [ ] Add character creator and customization from front-end.
- [ ] Player particle trail on walking.
- [ ] Use proper animation pose for target camera mode, with different weapons.
- [ ] Add common front-end slider graphics and accessors in front-end manager.
- [ ] Custom cursors.
- [ ] Particle effects and lights in character GUI.
  - [ ] Have particles with characterGUI viewport flag, also another world matrix that isnt offset by position.
  - [ ] Add flag to block particles so that only weapon/item particles are rendered in the viewport (non world offset).
- [ ] Bug with HUD text when resizing (HP, experience, etc)
- [ ] Make sure to fully clear the quest journal, manager and QuestGUI when round tripping to the front-end.
- [ ] Debug GUI should be on top of HUD and action bar.
- [ ] Add a left/right pose to the character GUI animation pose so that hands can be animated seperately.
- [ ] Enemy and NPC particle effect and light update code should be simplfied, (like player) no duplication for left/right.
- [ ] Fix for getting hit / knockback when crafting or interating with items / chest / npc
- [ ] Physics & collision goes wrong with lag spikes when MoveAbsolute() is called, during a lag spike.
- [ ] Add common front-end label / button colours, hover, pressed, etc.
- [ ] Add spell hands attacks.
- [ ] Add dagger / knife attacks.
- [ ] Respawn button properly skinned and textured.
- [ ] Dont create dying lights for the weapons in the front-end create character screen, since this bleeds into the customization section.
- [ ] Add magic back into player logic.
- [ ] Random direction on mimic spawning.
- [ ] Mimics turning around when going back to non-aggro.
- [ ] Different default inventory when we load a different class type from the create character screen.
- [ ] Use mouse and gamepad sensativity in the looking and camera rotation functions.
- [ ] Fog rendering.
- [ ] Add easier crafting functionality, just a single function call.
- [ ] Water, creating and manipulation, rendering, updating.
- [ ] Create random loot manager and random loot dropping.
- [ ] NPC loading falls through the world when we road trip to the front-end and start the game, Because physics happens during loading.
- [ ] Better front-end scenery and character screens.
- [ ] Add audio and music playback.
- [ ] Add VLD support to debug version, allow for memory leak detection.
- [ ] Saving and loading chunks.
  - [ ] Group together chunks for saving, loading. Not individual
- [ ] Add voxel editor.
- [ ] Add texture support for voxel blocks.
- [ ] Add mode to play in reduced and low FPS.
  - [ ] Low FPS causes bad physics and also bad step-up block functionality, investigate and fix.
- [ ] Dont allow jumping multiple times by holding down the space (or joy button).
- [ ] Allow for font changing on the fly, allow different font styles to go with the themes.
- [ ] Add shadow receiving on instanced rendered objects. (Add texture matrix to shader??)
- [ ] Beter player walking - animation speeds, lean(?), speed up, slow down, etc
- [ ] Test out Baked AO models using MagikaVoxel.
- [ ] Companion pets.
- [ ] Add debug render metrics for all counters (i.e num chunks, particles, items, triangles, verts, etc).
- [ ] Particle effects proper position when in first person mode!
- [ ] Add more presets for other body parts in character creator screen.
- [ ] Side stepping animation when in target camera mode.
- [ ] Bug with custom cursors when going outside of windows bounds.
- [ ] Don't allow 'other' buttons in select character screen when we have delet popup - i.e select, create, etc.
- [ ] Remove debug options and debug controls in RELEASE mode.

## Dependencies

* freetype
* tinythread
* glew
* glm

## License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The class is licensed under the [MIT License](http://opensource.org/licenses/MIT):

Copyright &copy; 2016 [Chris Ohk](http://www.github.com/utilForever)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.