/*************************************************************************
> File Name: Bezier3.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Bezier Curve with 3 points - 1 control point.
> Created Time: 2016/06/19
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Bezier3.h"

// Constructors
Bezier3::Bezier3()
{
	// Do nothing
}

Bezier3::Bezier3(glm::vec3 start, glm::vec3 end, glm::vec3 control) :
	m_startPoint(start), m_endPoint(end), m_controlPoint(control)
{
	// Do nothing
}

Bezier3::Bezier3(float xStart, float yStart, float zStart, float xEnd, float yEnd, float zEnd, float xControl, float yControl, float zControl) :
	m_startPoint(glm::vec3(xStart, yStart, zStart)), m_endPoint(glm::vec3(xEnd, yEnd, zEnd)), m_controlPoint(glm::vec3(xControl, yControl, zControl))
{
	// Do nothing
}

// Operations
const glm::vec3 Bezier3::GetInterpolatedPoint(float t) const
{
	float iT = 1.0f - t;
	float b0 = iT * iT;
	float b1 = 2 * t * iT;
	float b2 = t * t;

	float x = m_startPoint.x * b0 + m_controlPoint.x * b1 + m_endPoint.x * b2;
	float y = m_startPoint.y * b0 + m_controlPoint.y * b1 + m_endPoint.y * b2;
	float z = m_startPoint.z * b0 + m_controlPoint.z * b1 + m_endPoint.z * b2;

	return glm::vec3(x, y, z);
}
