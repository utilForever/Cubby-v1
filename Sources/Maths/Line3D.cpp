/*************************************************************************
> File Name: Line3D.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3d maths - Line 3D library.
> Created Time: 2016/06/18
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <../Libraries/glm/gtx/norm.hpp>
#include <../Libraries/glm/detail/func_geometric.hpp>

#include "Line3D.h"

// Constructors
Line3D::Line3D()
{
	/* Nothing */
}

Line3D::Line3D(glm::vec3 lStart, glm::vec3 lEnd)
{
	m_startPoint = lStart;
	m_endPoint = lEnd;
}

Line3D::Line3D(float x1, float y1, float z1, float x2, float y2, float z2)
{
	m_startPoint = glm::vec3(x1, y1, z1);
	m_endPoint = glm::vec3(x2, y2, z2);
}

// Properties
const glm::vec3 Line3D::GetMidPoint() const
{
	return GetInterpolatedPoint(0.5f);
}

const glm::vec3 Line3D::GetVector() const
{
	return m_endPoint - m_startPoint;
}

const float Line3D::GetLength() const
{
	return length(m_endPoint - m_startPoint);
}

const float Line3D::GetLengthSquared() const
{
	return length2(m_endPoint - m_startPoint);
}

// Operations
const glm::vec3 Line3D::GetInterpolatedPoint(float t) const
{
	glm::vec3 lScale = (m_endPoint - m_startPoint) * t;

	return m_startPoint + lScale;
}
