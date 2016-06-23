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

#include "Maths/Matrix4.h"

class Viewport
{
private:
	int m_bottom, m_left, m_width, m_height;
	float m_fov, m_aspect;
	Matrix4 m_perspective, m_orthographic, m_projection2D;
};

#endif