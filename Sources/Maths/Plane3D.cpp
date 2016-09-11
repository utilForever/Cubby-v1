/*************************************************************************
> File Name: Plane3D.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3d maths - Plane 3D library.
> Created Time: 2016/06/18
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <glm/gtx/norm.hpp>
#include <glm/detail/func_geometric.hpp>

#include "Plane3D.h"

// Constructors
Plane3D::Plane3D() :
	d(0.0f)
{
	// Do nothing
}

Plane3D::Plane3D(glm::vec3 normal, glm::vec3 point)
	: m_point(point), m_normal(normal)
{
	m_normal = normalize(m_normal);
	d = -dot(m_normal, m_point);
}

Plane3D::Plane3D(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
	glm::vec3 aux1;
	glm::vec3 aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	m_normal = cross(aux2, aux1);
	m_normal = normalize(m_normal);

	m_point = v2;

	d = -dot(m_normal, m_point);
}

Plane3D::Plane3D(float a, float b, float c, float d)
{
	// Set the normal vector
	m_normal = glm::vec3(a, b, c);

	// Compute the length of the vector
	float vLength = length(m_normal);

	// Normalize the vector
	m_normal = glm::vec3(a / vLength, b / vLength, c / vLength);

	// And divide d by the length as well
	this->d = d / vLength;
}

// Operations
float Plane3D::GetPointDistance(glm::vec3 point) const
{
	return d + dot(m_normal, point);
}