/*************************************************************************
> File Name: CubbyGame.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby game class.
> Created Time: 2016/07/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_GAME_H
#define CUBBY_GAME_H

#include <Blocks/ChunkManager.h>
#include <Blocks/BiomeManager.h>
#include <Models/QubicleBinaryManager.h>
#include <Renderer/Renderer.h>
#include <Renderer/Camera.h>

#include "CubbyWindow.h"
#include "CubbySettings.h"

// Game modes
enum class GameMode
{
	Debug = 0,
	Loading,
	FrontEnd,
	Game,
};

// Camera modes
enum class CameraMode
{
	Debug = 0,
	Frontend,
	MouseRotate,
	AutoCamera,
	FirstPerson,
	NPCDialog,
	EnemyTarget,
};

class CubbyGame
{
public:
	static CubbyGame* GetInstance();

	// Creation
	void Create(CubbySettings* pCubbySettings);

	// Destruction
	void Destroy() const;

	// Events
	void PollEvents() const;
	bool ShouldClose() const;

	// Updating
	void Update();

	// Rendering
	void PreRender();
	void Render();

	// Accessors
	unsigned int GetDefaultViewport();
	Camera* GetGameCamera();
	ChunkManager* GetChunkManager();
	BiomeManager* GetBiomeManager();

private:
	CubbyWindow* m_pCubbyWindow;
	CubbySettings* m_pCubbySettings;

	// Renderer
	Renderer* m_pRenderer;

	// Game camera
	Camera* m_pGameCamera;

	// Qubicle binary manager
	QubicleBinaryManager* m_pQubicleBinaryManager;

	// Chunk manager
	ChunkManager* m_pChunkManager;

	// Biome manager
	BiomeManager* m_pBiomeManager;

	// Window width and height
	int m_windowWidth;
	int m_windowHeight;

	// Quit message
	bool m_isGameQuit;

	// Viewports
	unsigned int m_defaultViewport;

	// Lights
	unsigned int m_defaultLight;
	glm::vec3 m_defaultLightPosition;
	glm::vec3 m_defaultLightView;

	// Frame buffers
	unsigned int m_SSAOFrameBuffer;

	// FPS and delta time
#ifdef _WIN32
	LARGE_INTEGER m_fpsPreviousTicks;
	LARGE_INTEGER m_fpsCurrentTicks;
	LARGE_INTEGER m_fpsTicksPerSecond;
#else
	double m_fpsPreviousTicks;
	double m_fpsCurrentTicks;
#endif
	float m_deltaTime;
	float m_fps;

	// Toggle flags
	bool m_isMultiSampling;
	bool m_isDeferredRendering;

	// Singleton instance
	static CubbyGame* m_instance;
};

#endif