/*************************************************************************
> File Name: Line3D.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3d maths - Line 3D library.
> Created Time: 2016/06/18
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <glm/gtx/norm.hpp>
#include <glm/detail/func_geometric.hpp>

#include "Line3D.h"

// Constructors
Line3D::Line3D()
{
	// Do nothing
}

Line3D::Line3D(glm::vec3 start, glm::vec3 end) :
	m_startPoint(start), m_endPoint(end)
{
	// Do nothing
}

Line3D::Line3D(float x1, float y1, float z1, float x2, float y2, float z2) :
	m_startPoint(glm::vec3(x1, y1, z1)), m_endPoint(glm::vec3(x2, y2, z2))
{
	// Do nothing
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
	glm::vec3 scale = (m_endPoint - m_startPoint) * t;

	return m_startPoint + scale;
}
