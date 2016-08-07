/*************************************************************************
> File Name: MouseEvent.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A mouse event class to handle events created by mouse input.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MOUSE_EVENT_H
#define CUBBY_MOUSE_EVENT_H

#include "Event.h"

enum class MouseEventType
{
	PRESSED = 0,
	RELEASED,
	RELEASED_OUTSIDE,
	CLICKED,
	EXITED,
	DRAGGED,
	DRAGGED_OUTSIDE,  // Hack
	ENTERED,
	MOTION
};

enum class MouseButton
{
	BUTTON1 = 0,	// Left button
	BUTTON2,		// Right button
	BUTTON3,		// Middle button
	SCROLL_UP,
	SCROLL_DOWN
};

class MouseEvent : public Event
{
public:
	// Constructor, Destructor
	MouseEvent(Component* source, int id, int x, int y, int mouseButton);
	~MouseEvent();

	int GetButton() const;
	int GetX() const;
	int GetY() const;

private:
	/* Private members */
	int m_mouseX;
	int m_mouseY;
	int m_button;
};

#endif