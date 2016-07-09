/*************************************************************************
> File Name: Chunk.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby update functions.
> Created Time: 2016/07/09
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "CubbyGame.h"

#ifdef __linux__
#include <sys/time.h>
#endif //__linux__

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