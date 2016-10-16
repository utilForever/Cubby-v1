/*************************************************************************
> File Name: main.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The main entry point for the application. Creates the initial instances of
>    the container classes, iterates the game loop, polls events/input and
>    detects for game closure to cleanup the with the destruction.
> Created Time: 2016/07/06
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "CubbyGame.h"

#if defined(WIN32) || defined(_WIN32)
extern "C" { __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }
extern "C" { __declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001; }
#else 
extern "C" { int NvOptimusEnablement = 1; }
extern "C" { int AmdPowerXpressRequestHighPerformance = 1; }
#endif

int main()
{
	// Load the settings
	CubbySettings* pCubbySettings = new CubbySettings();
	pCubbySettings->LoadSettings();
	pCubbySettings->LoadOptions();

	// Initialize and create the CubbyGame object
	CubbyGame* pCubbyGame = CubbyGame::GetInstance();
	pCubbyGame->Create(pCubbySettings);

	// Loop until the user closes the window or application
	while (!pCubbyGame->ShouldClose())
	{
		// Poll input events
		pCubbyGame->PollEvents();

		// Update joysticks
		pCubbyGame->UpdateJoySticks();
		
		// Update
		pCubbyGame->Update();

		// PreRender
		pCubbyGame->PreRender();

		// Render
		pCubbyGame->Render();
	}

	// Cleanup
	pCubbyGame->Destroy();

	// Exit
	exit(EXIT_SUCCESS);
}