/*************************************************************************
> File Name: CubbyGame.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby game class.
> Created Time: 2016/01/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#pragma once

#include "CubbyWindow.h"

class CubbyGame
{
public:
	static CubbyGame* GetInstance();

	// Creation
	void Create();

	// Destruction
	void Destroy();

	// Events
	void PollEvents();
	bool ShouldClose();

	// Updating
	void Update();

	// Rendering
	void PreRender();
	void Render();

private:
	CubbyWindow* m_pCubbyWindow;

	// Singleton instance
	static CubbyGame* m_instance;
};