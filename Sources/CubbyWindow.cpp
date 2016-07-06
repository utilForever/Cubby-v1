/*************************************************************************
> File Name: CubbyWindow.cpp
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

#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif //_WIN32

#include "glew/include/GL/glew.h"

#include <GL/gl.h>

#ifdef _MSC_VER
#define strdup(x) _strdup(x)
#endif

#pragma comment (lib, "opengl32")
#pragma comment (lib, "glu32")

#include "CubbyWindow.h"
#include "CubbyGame.h"
#include "CubbySettings.h"

// Constructor, Destructor
CubbyWindow::CubbyWindow(CubbyGame* pCubbyGame, CubbySettings* pCubbySettings) :
	m_pCubbyGame(pCubbyGame), m_pCubbySettings(pCubbySettings), m_pWindow(nullptr),
	m_windowWidth(m_pCubbySettings->GetWindowWidth()), m_windowHeight(m_pCubbySettings->GetWindowHeight()),
	m_oldWindowWidth(m_windowHeight), m_oldWindowHeight(m_windowHeight), 
	m_cursorX(0), m_cursorY(0), m_cursorOldX(0), m_cursorOldY(0), m_minimized(false)
{
	
}

CubbyWindow::~CubbyWindow()
{
	
}

// Creation
void CubbyWindow::Create()
{
	// Initialize the window library
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	// Initialize any rendering params
	int samples = 8;
	glfwWindowHint(GLFW_SAMPLES, samples);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glGetIntegerv(GL_SAMPLES_ARB, &samples);

	// Create a windowed mode window and it's OpenGL context
	m_pWindow = glfwCreateWindow(m_windowWidth, m_windowHeight, "Cubby", nullptr, nullptr);
	if (!m_pWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	/* Initialize this window object */
	InitializeWindowContext(m_pWindow);

	if (m_pCubbySettings->GetFullScreen())
	{
		ToggleFullScreen(true);
	}
}

// Destruction
void CubbyWindow::Destroy() const
{
	glfwTerminate();
}

void CubbyWindow::InitializeWindowContext(GLFWwindow* window)
{
	// Center on screen
	const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwGetWindowSize(window, &m_windowWidth, &m_windowHeight);
	glfwSetWindowPos(window, (vidmode->width - m_windowWidth) / 2, (vidmode->height - m_windowHeight) / 2);

	// Make the window's context current
	glfwMakeContextCurrent(window);
	glfwSwapInterval(true);

	// Show the window
	glfwShowWindow(window);
}

// Full screen
void CubbyWindow::ToggleFullScreen(bool fullscreen)
{
	if (fullscreen)
	{
		const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		m_oldWindowWidth = m_windowWidth;
		m_oldWindowHeight = m_windowHeight;

		m_windowWidth = vidMode->width;
		m_windowHeight = vidMode->height;
	}
	else
	{
		m_windowWidth = m_oldWindowWidth;
		m_windowHeight = m_oldWindowHeight;
	}

	// Create new window
	GLFWwindow* newWindow = glfwCreateWindow(m_windowWidth, m_windowHeight, "Cubby", fullscreen ? glfwGetPrimaryMonitor() : NULL, m_pWindow);

	/* Initialize this new window object */
	InitializeWindowContext(newWindow);

	// Destroy the existing window pointer and assign new one, since we are context switching
	glfwDestroyWindow(m_pWindow);
	m_pWindow = newWindow;
}

// Events
void CubbyWindow::PollEvents() const
{
	// Poll for and process events
	return glfwPollEvents();
}
