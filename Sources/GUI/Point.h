/*************************************************************************
> File Name: Point.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Class to represent a 2d point in 2d screen space.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_POINT_H
#define CUBBY_POINT_H

struct Point
{
	// Constructor, Destructor
	Point();
	Point(int _x, int _y);

	~Point();

	bool operator==(const Point& rhs) const;

	int x;
	int y;
};

#endif