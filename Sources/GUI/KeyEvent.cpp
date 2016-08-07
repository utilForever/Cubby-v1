/*************************************************************************
> File Name: KeyEvent.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A key event class to handle events created by keyboard input.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "KeyEvent.h"

// Constructor, Destructor
KeyEvent::KeyEvent(Component* source, int id, int keyCode, int modifier) :
	Event(source, id), m_KeyCode(keyCode), m_Modifier(modifier)
{
	// Do nothing
}

KeyEvent::~KeyEvent()
{
	// Do nothing
}

int KeyEvent::GetKeyCode() const
{
	return m_KeyCode;
}

int KeyEvent::GetModifier() const
{
	return m_Modifier;
}