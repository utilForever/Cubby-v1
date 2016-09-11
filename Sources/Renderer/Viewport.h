/*************************************************************************
> File Name: Viewport.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A viewport, used by the opengl renderer to hold viewport information.
> Created Time: 2016/06/23
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_VIEWPORT_H
#define CUBBY_VIEWPORT_H

#include <Maths/Matrix4.h>

struct Viewport
{
	int bottom, left, width, height;
	float fov, aspect;
	Matrix4 perspective, orthographic, projection2D;
};

#endif