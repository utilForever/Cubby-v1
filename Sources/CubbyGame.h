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

#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"

#include "CubbyWindow.h"
#include "CubbySettings.h"

class CubbyGame
{
public:
	static CubbyGame* GetInstance();

	// Creation
	void Create(CubbySettings* pCubbySettings);

	// Destruction
	void Destroy();

	// Events
	void PollEvents();
	bool ShouldClose();

	// Window functionality
	void UpdateJoySticks();

	// Updating
	void Update();

	// Rendering
	void PreRender();
	void Render();

private:
	CubbyWindow* m_pCubbyWindow;
	CubbySettings* m_pCubbySettings;

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

	// Singleton instance
	static CubbyGame* m_instance;
};

#endif