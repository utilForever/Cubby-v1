/*************************************************************************
> File Name: CubbyGame.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby game class.
> Created Time: 2016/07/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <glm/detail/func_geometric.hpp>

#include "CubbyGame.h"

// Initialize singleton instance
CubbyGame* CubbyGame::m_instance = nullptr;

CubbyGame* CubbyGame::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new CubbyGame;
	}

	return m_instance;
}

// Creation
void CubbyGame::Create(CubbySettings* pCubbySettings)
{
	m_pRenderer = nullptr;
	m_pGameCamera = nullptr;
	m_pQubicleBinaryManager = nullptr;
	m_pChunkManager = nullptr;
	
	m_pCubbySettings = pCubbySettings;
	m_pCubbyWindow = new CubbyWindow(this, m_pCubbySettings);

	// Create the window
	m_pCubbyWindow->Create();

	// Setup FPS and delta time counters
#ifdef _WIN32
	QueryPerformanceCounter(&m_fpsPreviousTicks);
	QueryPerformanceCounter(&m_fpsCurrentTicks);
	QueryPerformanceFrequency(&m_fpsTicksPerSecond);
#else
	struct timeval tm;
	gettimeofday(&tm, NULL);
	m_fpsCurrentTicks = static_cast<double>(tm.tv_sec) + static_cast<double>(tm.tv_usec) / 1000000.0;
	m_fpsPreviousTicks = static_cast<double>(tm.tv_sec) + static_cast<double>(tm.tv_usec) / 1000000.0;
#endif //_WIN32
	m_deltaTime = 0.0f;
	m_fps = 0.0f;

	// Create the renderer
	m_windowWidth = m_pCubbyWindow->GetWindowWidth();
	m_windowHeight = m_pCubbyWindow->GetWindowHeight();
	m_pRenderer = new Renderer(m_windowWidth, m_windowHeight, 32, 8);

	// Pause and quit
	m_isGameQuit = false;

	// Create cameras
	m_pGameCamera = new Camera(m_pRenderer);
	m_pGameCamera->SetPosition(glm::vec3(8.0f, 8.25f, 15.5f));
	m_pGameCamera->SetFakePosition(m_pGameCamera->GetPosition());
	m_pGameCamera->SetFacing(glm::vec3(0.0f, 0.0f, -1.0f));
	m_pGameCamera->SetUp(glm::vec3(0.0f, 1.0f, 0.0f));
	m_pGameCamera->SetRight(glm::vec3(1.0f, 0.0f, 0.0f));

	// Create viewports
	m_pRenderer->CreateViewport(0, 0, m_windowWidth, m_windowHeight, 60.0f, &m_defaultViewport);
	// TODO: Implement rest viewports

	// Create lights
	m_defaultLightPosition = glm::vec3(300.0f, 300.0f, 300.0f);
	m_defaultLightView = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lightDirection = m_defaultLightView - m_defaultLightPosition;
	m_pRenderer->CreateLight(Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f),
		m_defaultLightPosition, lightDirection, 0.0f, 0.0f, 2.0f, 0.001f, 0.0f, true, false, &m_defaultLight);
	// TODO: Implement rest lights

	// Create the qubicle binary file manager
	m_pQubicleBinaryManager = new QubicleBinaryManager(m_pRenderer);

	// Create the chunk manager
	m_pChunkManager = new ChunkManager(m_pRenderer, m_pCubbySettings, m_pQubicleBinaryManager);
	m_pChunkManager->SetStepLockEnabled(m_pCubbySettings->IsStepUpdating());

	// Create the biome manager
	m_pBiomeManager = new BiomeManager(m_pRenderer);
}

// Destruction
void CubbyGame::Destroy() const
{
	if (m_instance != nullptr)
	{
		delete m_pChunkManager;
		delete m_pBiomeManager;
		delete m_pQubicleBinaryManager;
		delete m_pGameCamera;

		m_pCubbyWindow->Destroy();

		delete m_pCubbyWindow;

		delete m_instance;
	}
}

// Events
void CubbyGame::PollEvents() const
{
	m_pCubbyWindow->PollEvents();
}

bool CubbyGame::ShouldClose() const
{
	return m_isGameQuit;
}

// Accessors
unsigned int CubbyGame::GetDefaultViewport()
{
	return m_defaultViewport;
}

Camera* CubbyGame::GetGameCamera()
{
	return m_pGameCamera;
}

ChunkManager* CubbyGame::GetChunkManager()
{
	return m_pChunkManager;
}

BiomeManager* CubbyGame::GetBiomeManager()
{
	return m_pBiomeManager;
}