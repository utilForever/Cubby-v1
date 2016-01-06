/*************************************************************************
> File Name: main.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: The main entry point for the application.
> Created Time: 2016/01/05
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "CubbyGame.h"

int main(int argc, char* argv[])
{
	// Initialize and create the CubbyGame object
	CubbyGame* pCubbyGame = CubbyGame::GetInstance();
	pCubbyGame->Create();
}