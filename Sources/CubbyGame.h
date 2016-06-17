/*************************************************************************
> File Name: CubbyGame.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby game class.
> Created Time: 2016/01/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#pragma once

// Platform dependency
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <sys/time.h>
#endif

#include "CubbyWindow.h"

class CubbyGame
{
public:
	static CubbyGame* GetInstance();

	// Creation
	void Create();

	// Destruction
	// void Destroy();

	// Events
	void PollEvents();
	bool ShouldClose();

	// Updating
	void Update();

	// Rendering
	// void PreRender();
	// void Render();

private:
	CubbyWindow* m_pCubbyWindow;

	// FPS and deltatime
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

	// Singleton instance
	static CubbyGame* m_instance;
};