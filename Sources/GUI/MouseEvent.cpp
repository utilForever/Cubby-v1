/*************************************************************************
> File Name: MouseEvent.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A mouse event class to handle events created by mouse input.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "MouseEvent.h"

// Constructor, Destructor
MouseEvent::MouseEvent(Component* source, int id, int x, int y, int mouseButton) :
	Event(source, id), m_mouseX(x), m_mouseY(y), m_button(mouseButton)
{
	// Do nothing
}

MouseEvent::~MouseEvent()
{
	// Do nothing
}

int MouseEvent::GetButton() const
{
	return m_button;
}

int MouseEvent::GetX() const
{
	return m_mouseX;
}

int MouseEvent::GetY() const
{
	return m_mouseY;
}