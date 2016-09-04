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

#include <glew/include/GL/glew.h>

#ifdef _WIN32
#include <windows.h>
#endif //_WIN32

#include <GL/gl.h>

#ifdef _MSC_VER
#define strdup(x) _strdup(x)
#endif

#pragma comment (lib, "opengl32")
#pragma comment (lib, "glu32")

#include "CubbyWindow.h"
#include "CubbyGame.h"
#include "CubbySettings.h"

// Callback functions
void WindowResizeCallback(GLFWwindow* window, int width, int height);
void WindowCloseCallback(GLFWwindow* window);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void CharacterCallback(GLFWwindow* window, unsigned int keyCode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void MouseScrollCallback(GLFWwindow* window, double x, double y);

// Constructor, Destructor
CubbyWindow::CubbyWindow(CubbyGame* pCubbyGame, CubbySettings* pCubbySettings) :
	m_pWindow(nullptr)
{
	m_pCubbyGame = pCubbyGame;
	m_pCubbySettings = pCubbySettings;

	// Minimized flag
	m_minimized = false;

	// Set default cursor positions
	m_cursorX = 0;
	m_cursorY = 0;
	m_cursorOldX = 0;
	m_cursorOldY = 0;

	// Default joystick params
	m_joystickCount = 0;
	m_joystickAnalogDeadZone = 0.20f;

	// Default windows dimensions
	m_windowWidth = m_pCubbySettings->m_windowWidth;
	m_windowHeight = m_pCubbySettings->m_windowHeight;
	m_oldWindowWidth = m_windowWidth;
	m_oldWindowHeight = m_windowHeight;
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

	// Initialize the joysticks object
	memset(m_joysticks, 0, sizeof(m_joysticks));

	// Create a windowed mode window and it's OpenGL context
	m_pWindow = glfwCreateWindow(m_windowWidth, m_windowHeight, "Cubby", nullptr, nullptr);
	if (!m_pWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Initialize this window object
	InitializeWindowContext(m_pWindow);

	if (m_pCubbySettings->m_fullscreen)
	{
		ToggleFullScreen(true);
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CubbyWindow::Destroy() const
{
	glfwTerminate();
}

void CubbyWindow::Update(float dt)
{
	// Update the cursor positions
	double x;
	double y;
	glfwGetCursorPos(m_pWindow, &x, &y);

	m_cursorX = static_cast<int>(floor(x));
	m_cursorY = static_cast<int>(floor(y));
}

void CubbyWindow::Render() const
{
	// Swap front and back buffers
	glfwSwapBuffers(m_pWindow);
}

void CubbyWindow::InitializeWindowContext(GLFWwindow* window)
{
	// Window resize callback
	glfwSetWindowSizeCallback(window, WindowResizeCallback);

	// Window close message callback
	glfwSetWindowCloseCallback(window, WindowCloseCallback);

	// Input callbacks
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCharCallback(window, CharacterCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);

	// Center on screen
	const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwGetWindowSize(window, &m_windowWidth, &m_windowHeight);
	glfwSetWindowPos(window, (vidmode->width - m_windowWidth) / 2, (vidmode->height - m_windowHeight) / 2);

	// Make the window's context current
	glfwMakeContextCurrent(window);
	glfwSwapInterval(m_pCubbySettings->m_vsync);

	// Force resize
	WindowResizeCallback(window, m_windowWidth, m_windowHeight);

	// Show the window
	glfwShowWindow(window);
}

// Windows dimensions
int CubbyWindow::GetWindowWidth() const
{
	return m_windowWidth;
}

int CubbyWindow::GetWindowHeight() const
{
	return m_windowHeight;
}

void CubbyWindow::ResizeWindow(int width, int height)
{
	m_minimized = (width == 0 || height == 0);

	m_windowWidth = width;
	m_windowHeight = height;
}

bool CubbyWindow::GetMinimized() const
{
	return m_minimized;
}

// Cursor position
int CubbyWindow::GetCursorX() const
{
	return m_cursorX;
}

int CubbyWindow::GetCursorY() const
{
	return m_cursorY;
}

void CubbyWindow::SetCursorPosition(int x, int y) const
{
	glfwSetCursorPos(m_pWindow, x, y);
}

void CubbyWindow::TurnCursorOff(bool forceOff)
{
	if (IsCursorOn() == true)
	{
		if (m_pCubbySettings->m_customCursors == false || forceOff)
		{
			glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	m_cursorOldX = m_cursorX;
	m_cursorOldY = m_cursorY;

	// Signal to the GUI that we have turned off the cursor, reset buttons states, cursor pos, etc
	m_pCubbyGame->GUITurnOffCursor();
}

void CubbyWindow::TurnCursorOn(bool resetCursorPosition, bool forceOn) const
{
	if (IsCursorOn() == false)
	{
		if (m_pCubbySettings->m_customCursors == false || forceOn)
		{
			glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	if (resetCursorPosition)
	{
		SetCursorPosition(m_cursorOldX, m_cursorOldY);
	}
}

bool CubbyWindow::IsCursorOn() const
{
	return glfwGetInputMode(m_pWindow, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
}

// Joysticks
void CubbyWindow::UpdateJoySticks()
{
	for (int i = 0; i < sizeof(m_joysticks) / sizeof(Joystick); i++)
	{
		Joystick* j = m_joysticks + i;

		if (glfwJoystickPresent(GLFW_JOYSTICK_1 + i))
		{
			const float* axes;
			const unsigned char* buttons;
			int axis_count, button_count;

			free(j->m_name);
			j->m_name = strdup(glfwGetJoystickName(GLFW_JOYSTICK_1 + i));

			axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + i, &axis_count);
			if (axis_count != j->m_axisCount)
			{
				j->m_axisCount = axis_count;
				j->m_axes = static_cast<float*>(realloc(j->m_axes, j->m_axisCount * sizeof(float)));
			}

			memcpy(j->m_axes, axes, axis_count * sizeof(float));

			buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + i, &button_count);
			if (button_count != j->m_buttonCount)
			{
				j->m_buttonCount = button_count;
				j->m_buttons = static_cast<unsigned char*>(realloc(j->m_buttons, j->m_buttonCount));
			}

			memcpy(j->m_buttons, buttons, button_count * sizeof(unsigned char));

			if (!j->m_present)
			{
				printf("\nFound joystick %i named \'%s\' with %i axes, %i buttons\n",
					i + 1, j->m_name, j->m_axisCount, j->m_buttonCount);

				m_joystickCount++;
			}

			j->m_present = true;
		}
		else
		{
			if (j->m_present)
			{
				printf("\nLost joystick %i named \'%s\'\n", i + 1, j->m_name);

				free(j->m_name);
				free(j->m_axes);
				free(j->m_buttons);
				memset(j, 0, sizeof(Joystick));

				m_joystickCount--;
			}
		}
	}
}

bool CubbyWindow::IsJoyStickConnected(int joyStickNum)
{
	Joystick* j = m_joysticks + joyStickNum;

	return j->m_present;
}

float CubbyWindow::GetJoystickAxisValue(int joyStickNum, int axisIndex)
{
	Joystick* j = m_joysticks + joyStickNum;

	if (j->m_present)
	{
		return j->m_axes[axisIndex];
	}

	return 0.0f;
}

bool CubbyWindow::GetJoystickButton(int joyStickNum, int axisIndex)
{
	Joystick* j = m_joysticks + joyStickNum;

	if (j->m_present)
	{
		return (j->m_buttons[axisIndex] != 0);
	}

	return false;
}

float CubbyWindow::GetJoystickAnalogDeadZone() const
{
	return m_joystickAnalogDeadZone;
}

// Fullscreen
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
	GLFWwindow* newWindow = glfwCreateWindow(m_windowWidth, m_windowHeight, "Cubby", fullscreen ? glfwGetPrimaryMonitor() : nullptr, m_pWindow);

	// Initialize this new window object
	InitializeWindowContext(newWindow);

	// Destroy the existing window pointer and assign new one, since we are context switching
	glfwDestroyWindow(m_pWindow);
	m_pWindow = newWindow;
}

// Events
// ReSharper disable once CppMemberFunctionMayBeStatic
void CubbyWindow::PollEvents() const
{
	// Poll for and process events
	glfwPollEvents();
}

// Callbacks
void WindowResizeCallback(GLFWwindow* window, int width, int height)
{
	CubbyGame::GetInstance()->ResizeWindow(width, height);
}

void WindowCloseCallback(GLFWwindow* window)
{
	CubbyGame::GetInstance()->CloseWindow();
}