/*************************************************************************
> File Name: Dimensions.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Class to represent 2d rectangular space, useful for objects
>    such as windows, boxes, buttons, etc.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_DIMENSIONS_H
#define CUBBY_DIMENSIONS_H

#include "Point.h"

struct Dimensions
{
	// Constructor, Destructor
	Dimensions();
	Dimensions(int _x, int _y, int _width, int _height);

	~Dimensions();

	bool Contains(int _x, int _y) const;
	bool Contains(const Point& point) const;

	bool operator==(const Dimensions& rhs) const;

	int x;
	int y;
	int width;
	int height;
};

#endif