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

	void Create();

private:
	CubbyWindow* m_pCubbyWindow;

	// Singleton instance
	static CubbyGame* m_instance;
};