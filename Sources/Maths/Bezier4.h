/*************************************************************************
> File Name: Bezier4.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Bezier Curve with 4 points - 2 control points.
> Created Time: 2016/06/19
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BEZIER4_H
#define CUBBY_BEZIER4_H

#include <glm/vec3.hpp>

struct Bezier4
{
	// Constructors
	Bezier4();
	Bezier4(glm::vec3 start, glm::vec3 end, glm::vec3 control1, glm::vec3 control2);
	Bezier4(float xStart, float yStart, float zStart, float xEnd, float yEnd, float zEnd, float xControl1, float yControl1, float zControl1, float xControl2, float yControl2, float zControl2);

	// Operations
	const glm::vec3 GetInterpolatedPoint(float t) const;

	glm::vec3 m_startPoint;
	glm::vec3 m_endPoint;
	glm::vec3 m_controlPoint1;
	glm::vec3 m_controlPoint2;
};

#endif