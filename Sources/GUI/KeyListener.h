/*************************************************************************
> File Name: KeyListener.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Key listener interface, inherit from this class if you need
>    to listen for and handle key events.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_KEY_LISTENER_H
#define CUBBY_KEY_LISTENER_H

#include "KeyEvent.h"
#include "EventListener.h"

class KeyListener : public EventListener
{
public:
	// Constructor, Destructor
	KeyListener();
	~KeyListener();

	virtual void KeyPressed(const KeyEvent& event) { }
	virtual void KeyReleased(const KeyEvent& event) { }
	virtual void KeyTyped(const KeyEvent& event) { } // Pressed AND Released
	virtual void CharacterEntered(const KeyEvent& event) { }
};

#endif