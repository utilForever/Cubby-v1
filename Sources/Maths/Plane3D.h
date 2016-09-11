/*************************************************************************
> File Name: Plane3D.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3d maths - Plane 3D library.
> Created Time: 2016/06/18
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_PLANE_3D_H
#define CUBBY_PLANE_3D_H

#include <glm/vec3.hpp>

struct Plane3D
{
	// Constructors
	Plane3D();
	Plane3D(glm::vec3 normal, glm::vec3 point);
	Plane3D(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
	Plane3D(float a, float b, float c, float d);

	// Operations
	float GetPointDistance(glm::vec3 point) const;

	glm::vec3 m_point;
	glm::vec3 m_normal;
	float d;
};

#endif