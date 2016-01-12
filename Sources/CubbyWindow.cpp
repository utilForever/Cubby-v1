/*************************************************************************
> File Name: CubbyWindow.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby window class.
> Created Time: 2016/01/06
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Libraries/glew/include/GL/glew.h"

#include <GL/gl.h>

#include <stdlib.h>

#pragma comment (lib, "opengl32")
#pragma comment (lib, "glu32")

#include "CubbyWindow.h"

CubbyWindow::CubbyWindow()
	: m_pWindow(nullptr),
	m_windowWidth(500),
	m_windowHeight(500)
{
	
}

CubbyWindow::~CubbyWindow()
{
	
}

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

bool CubbyWindow::ShouldCloseWindow()
{
	return (glfwWindowShouldClose(m_pWindow) == 1);
}