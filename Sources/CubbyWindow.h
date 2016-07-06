/*************************************************************************
> File Name: CubbyWindow.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The CubbyWindow class is an interface and wrapper around the glfw windows
>    library and adds common functionality to bind this to the Vox application.
>    The window class handles most 'window' functionality that isn't directly
>    part of the game, stuff like window resizing, fullscreen, cursors, etc.
> Created Time: 2016/07/06
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#pragma once

#include <GLFW/glfw3.h>

class CubbyWindow
{
public:
	CubbyWindow();
	~CubbyWindow();

	void Create();
	void InitializeWindowContext(GLFWwindow* window);

	void PollEvents();
	bool ShouldCloseWindow();
private:
	// The window object pointer
	GLFWwindow* m_pWindow;

	// Window dimensions
	int m_windowWidth;
	int m_windowHeight;
};