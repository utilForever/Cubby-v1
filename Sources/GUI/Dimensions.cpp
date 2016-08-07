/*************************************************************************
> File Name: Dimensions.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Class to represent 2d rectangular space, useful for objects
>    such as windows, boxes, buttons, etc.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Dimensions.h"

// Constructor, Destructor
Dimensions::Dimensions() :
	x(0), y(0), width(0), height(0)
{
	
}

Dimensions::Dimensions(int _x, int _y, int _width, int _height) :
	x(_x), y(_y), width(_width), height(_height)
{
	
}

Dimensions::~Dimensions()
{
	// Nothing
}

bool Dimensions::Contains(int _x, int _y) const
{
	if ((_x >= x && _x <= (x + width)) && (_y >= y && _y <= (y + height)))
	{
		return true;
	}

	return false;
}

bool Dimensions::Contains(const Point& point) const
{
	return Contains(point.x, point.y);
}

bool Dimensions::operator==(const Dimensions& rhs) const
{
	return ((x == rhs.x) && (y == rhs.y) && (width == rhs.width) && (height == rhs.height));
}