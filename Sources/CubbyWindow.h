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

#ifndef CUBBY_WINDOW_H
#define CUBBY_WINDOW_H

#include <GLFW/glfw3.h>

// Forward declaration
class CubbyGame;
class CubbySettings;

struct Joystick
{
	bool m_present;
	char* m_name;
	float* m_axes;
	unsigned char* m_buttons;
	int m_axisCount;
	int m_buttonCount;
};

class CubbyWindow
{
public:
	// Constructor, Destructor
	CubbyWindow(CubbyGame* pCubbyGame, CubbySettings* pCubbySettings);
	~CubbyWindow() = default;

	void Create();
	void Destroy() const;
	void Update(float dt);
	void Render() const;

	void InitializeWindowContext(GLFWwindow* window);

	// Windows dimensions
	int GetWindowWidth() const;
	int GetWindowHeight() const;
	void ResizeWindow(int width, int height);

	// Minimized
	bool GetMinimized() const;

	// Cursor
	int GetCursorX() const;
	int GetCursorY() const;
	void SetCursorPosition(int x, int y) const;
	void TurnCursorOff(bool forceOff);
	void TurnCursorOn(bool resetCursorPosition, bool forceOn) const;
	bool IsCursorOn() const;

	// Joysticks
	void UpdateJoySticks();
	bool IsJoyStickConnected(int joyStickNum);
	float GetJoystickAxisValue(int joyStickNum, int axisIndex);
	bool GetJoystickButton(int joyStickNum, int axisIndex);
	float GetJoystickAnalogDeadZone() const;

	// Fullscreen
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

	// Minimized flag
	bool m_minimized;

	// Cursor position
	int m_cursorX;
	int m_cursorY;
	int m_cursorOldX;
	int m_cursorOldY;

	// Joysticks
	Joystick m_joysticks[GLFW_JOYSTICK_LAST - GLFW_JOYSTICK_1 + 1];
	int m_joystickCount;
	float m_joystickAnalogDeadZone;
};

#endif