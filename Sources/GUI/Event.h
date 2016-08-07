/*************************************************************************
> File Name: Event.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An abstract event base class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_EVENT_H
#define CUBBY_EVENT_H

// Forward declaration
class Component;

class Event
{
public:
	// Constructor, Destructor
	Event(Component* source, int id);
	~Event();

	Component* GetSource() const;
	int GetID() const;

private:
	int m_eventID;
	Component* m_pSource;
};

#endif