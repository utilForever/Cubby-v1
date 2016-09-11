/*************************************************************************
> File Name: Bezier3.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Bezier Curve with 3 points - 1 control point.
> Created Time: 2016/06/19
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BEZIER3_H
#define CUBBY_BEZIER3_H

#include <glm/vec3.hpp>

struct Bezier3
{
	// Constructors
	Bezier3();
	Bezier3(glm::vec3 start, glm::vec3 end, glm::vec3 control);
	Bezier3(float xStart, float yStart, float zStart, float xEnd, float yEnd, float zEnd, float xControl, float yControl, float zControl);

	// Operations
	const glm::vec3 GetInterpolatedPoint(float t) const;

	glm::vec3 m_startPoint;
	glm::vec3 m_endPoint;
	glm::vec3 m_controlPoint;
};

#endif