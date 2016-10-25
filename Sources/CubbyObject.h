/*************************************************************************
> File Name: CubbyObject.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: The CubbyObject class stores registered object to debug it easily.
> Created Time: 2016/10/26
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_OBJECT_H
#define CUBBY_OBJECT_H

#ifdef _DEBUG
size_t g_registeredObject = 0;

class CubbyObject
{
	CubbyObject()
	{
		++g_RegisteredObject;
	}
	~CubbyObject()
	{
		--g_RegisteredObject;
	}
};
#else
class CubbyObject
{
	CubbyObject() = delete;
	~CubbyObject() = delete;
};
#endif

#endif