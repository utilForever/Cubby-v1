/*************************************************************************
> File Name: Point.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Class to represent a 2d point in 2d screen space.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Point.h"

// Constructor, Destructor
Point::Point() :
	x(0), y(0)
{

}

Point::Point(int _x, int _y) :
	x(_x), y(_y)
{
	
}

Point::~Point()
{
	// Do nothing
}

bool Point::operator==(const Point& rhs) const
{
	return ((x == rhs.x) && (y == rhs.y));
}
