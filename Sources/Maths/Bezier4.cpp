/*************************************************************************
> File Name: Bezier4.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Bezier Curve with 4 points - 2 control points.
> Created Time: 2016/06/19
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Bezier4.h"

// Constructors
Bezier4::Bezier4()
{
	// Do nothing
}

Bezier4::Bezier4(glm::vec3 start, glm::vec3 end, glm::vec3 control1, glm::vec3 control2) :
	m_startPoint(start), m_endPoint(end), m_controlPoint1(control1), m_controlPoint2(control2)
{
	// Do nothing
}

Bezier4::Bezier4(float xStart, float yStart, float zStart, float xEnd, float yEnd, float zEnd, float xControl1, float yControl1, float zControl1, float xControl2, float yControl2, float zControl2) :
	m_startPoint(glm::vec3(xStart, yStart, zStart)), m_endPoint(glm::vec3(xEnd, yEnd, zEnd)), m_controlPoint1(glm::vec3(xControl1, yControl1, zControl1)), m_controlPoint2(glm::vec3(xControl2, yControl2, zControl2))
{
	// Do nothing
}

// Operations
const glm::vec3 Bezier4::GetInterpolatedPoint(float t) const
{
	float iT = 1.0f - t;
	float b0 = iT * iT * iT;
	float b1 = 3 * t * iT * iT;
	float b2 = 3 * t * t * iT;
	float b3 = t * t * t;

	float x = m_startPoint.x * b0 + m_controlPoint1.x * b1 + m_controlPoint2.x * b2 + m_endPoint.x * b3;
	float y = m_startPoint.y * b0 + m_controlPoint1.y * b1 + m_controlPoint2.y * b2 + m_endPoint.y * b3;
	float z = m_startPoint.z * b0 + m_controlPoint1.z * b1 + m_controlPoint2.z * b2 + m_endPoint.z * b3;

	return glm::vec3(x, y, z);
}
