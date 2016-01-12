/*************************************************************************
> File Name: CubbyGame.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby game class.
> Created Time: 2016/01/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

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

void CubbyGame::Create()
{
	m_pCubbyWindow = new CubbyWindow();

	// Create the window
	m_pCubbyWindow->Create();

	// Setup FPS and deltatime counters
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
}

// Events
void CubbyGame::PollEvents()
{
	m_pCubbyWindow->PollEvents();
}

bool CubbyGame::ShouldClose()
{
	return m_pCubbyWindow->ShouldCloseWindow();
}

// Updating
void CubbyGame::Update()
{
	// FPS
#ifdef _WIN32
	QueryPerformanceCounter(&m_fpsCurrentTicks);
	m_deltaTime = static_cast<float>(m_fpsCurrentTicks.QuadPart - m_fpsPreviousTicks.QuadPart) / static_cast<float>(m_fpsTicksPerSecond.QuadPart);
#else
	struct timeval tm;
	gettimeofday(&tm, NULL);
	m_fpsCurrentTicks = static_cast<double>(tm.tv_sec) + static_cast<double>(tm.tv_usec) / 1000000.0;
	m_deltaTime = (m_fpsCurrentTicks - m_fpsPreviousTicks);
#endif
	m_fps = 1.0f / m_deltaTime;
	m_fpsPreviousTicks = m_fpsCurrentTicks;
}