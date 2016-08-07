/*************************************************************************
> File Name: Event.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An abstract event base class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Event.h"

// Constructor, Destructor
Event::Event(Component* source, int id) :
	m_eventID(id), m_pSource(source)
{
	// Do Nothing
}

Event::~Event()
{
	// Do Nothing
}

Component* Event::GetSource() const
{
	return m_pSource;
}

int Event::GetID() const
{
	return m_eventID;
}