/*************************************************************************
> File Name: FocusListener.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Focus listener interface, inherit from this class if you need
>    to listen for and handle focus events.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_FOCUS_LISTENER_H
#define CUBBY_FOCUS_LISTENER_H

#include "FocusEvent.h"
#include "EventListener.h"

class FocusListener : public EventListener
{
public:
	// Constructor, Destructor
	FocusListener();
	~FocusListener();

	virtual void FocusLost(const FocusEvent& event) { }
	virtual void FocusGained(const FocusEvent& event) { }
};

#endif