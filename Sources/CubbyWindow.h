/*************************************************************************
> File Name: CubbyWindow.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The CubbyWindow class is an interface and wrapper around the glfw windows
>    library and adds common functionality to bind this to the Vox application.
>    The window class handles most 'window' functionality that isn't directly
>    part of the game, stuff like window resizing, fullscreen, cursors, etc.
> Created Time: 2016/07/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#pragma once

#include <GLFW/glfw3.h>

class CubbyGame;
class CubbySettings;

class CubbyWindow
{
public:
	// Constructor, Destructor
	CubbyWindow(CubbyGame* pCubbyGame, CubbySettings* pCubbySettings);
	~CubbyWindow();

	// Creation
	void Create();

	// Destruction
	void Destroy() const;

	// Rendering
	void Render() const;

	void InitializeWindowContext(GLFWwindow* window);

	// Windows dimensions
	int GetWindowWidth() const;
	int GetWindowHeight() const;

	// Minimized
	bool GetMinimized() const;

	// Full screen
	void ToggleFullScreen(bool fullscreen);

	// Events
	void PollEvents() const;
private:
	CubbyGame* m_pCubbyGame;
	CubbySettings* m_pCubbySettings;

	// The window object pointer
	GLFWwindow* m_pWindow;

	// Window dimensions
	int m_windowWidth;
	int m_windowHeight;
	int m_oldWindowWidth;
	int m_oldWindowHeight;

	// Cursor position
	int m_cursorX;
	int m_cursorY;
	int m_cursorOldX;
	int m_cursorOldY;

	// Minimized flag
	bool m_minimized;
};