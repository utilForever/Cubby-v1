/*************************************************************************
> File Name: KeyEvent.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A key event class to handle events created by keyboard input.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_KEY_EVENT_H
#define CUBBY_KEY_EVENT_H

#include "Event.h"

enum class KeyEventType
{
	PRESSED = 0,
	RELEASED,
	TYPED,

	CHARACTER_ENTERED,
};

class KeyEvent : public Event
{
public:
	// Constructor, Destructor
	KeyEvent(Component* source, int id, int keyCode, int modifier);
	~KeyEvent();

	int GetKeyCode() const;
	int GetModifier() const;

private:
	int m_KeyCode;
	int m_Modifier;
};

#endif