/*************************************************************************
> File Name: CubbyWindow.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby window class.
> Created Time: 2016/01/06
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