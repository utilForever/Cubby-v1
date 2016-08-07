/*************************************************************************
> File Name: FocusEvent.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A focus event class to handle events created by focus changes,
> 	 i.e Components gaining and losing focus from mouse/keyboard input.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "FocusEvent.h"

// Constructor, Destructor
FocusEvent::FocusEvent(Component* source, int id) :
	Event(source, id)
{
	// Do nothing
}

FocusEvent::~FocusEvent()
{
	// Do nothing
}