/*************************************************************************
> File Name: FocusEvent.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A focus event class to handle events created by focus changes,
> 	 i.e Components gaining and losing focus from mouse/keyboard input.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_FOCUS_EVENT_H
#define CUBBY_FOCUS_EVENT_H

#include "Event.h"

enum class FocusEventType
{
	GAINED,
	LOST
};

class FocusEvent : public Event
{
public:
	// Constructor, Destructor
	FocusEvent(Component* source, int id);
	~FocusEvent();
};

#endif