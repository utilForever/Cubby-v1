/*************************************************************************
> File Name: MouseListener.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Mouse listener interface, inherit from this class if you need
>    to listen for and handle mouse events.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MOUSE_LISTENER_H
#define CUBBY_MOUSE_LISTENER_H

#include "MouseEvent.h"
#include "EventListener.h"

class MouseListener : public EventListener
{
public:
	// Constructor, Destructor
	MouseListener();
	~MouseListener();

	virtual void MouseClicked(const MouseEvent& event) { }  // Pressed AND released
	virtual void MousePressed(const MouseEvent& event) { }
	virtual void MouseReleased(const MouseEvent& event) { }
	virtual void MouseReleasedOutside(const MouseEvent& event) { } // Pressed an object, then released outside of the objects bounds
	virtual void MouseEntered(const MouseEvent& event) { }
	virtual void MouseExited(const MouseEvent& event) { }
	virtual void MouseDragged(const MouseEvent& event) { }
	virtual void MouseDraggedOutside(const MouseEvent& event) { } // HACK
	virtual void MouseMotion(const MouseEvent& event) { }
};

#endif