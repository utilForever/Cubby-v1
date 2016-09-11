/*************************************************************************
> File Name: Line3D.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3d maths - Line 3D library.
> Created Time: 2016/06/18
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_LINE_3D_H
#define CUBBY_LINE_3D_H

#include <glm/vec3.hpp>

class Line3D
{
	// Constructors
	Line3D();
	Line3D(glm::vec3 start, glm::vec3 end);
	Line3D(float x1, float y1, float z1, float x2, float y2, float z2);

	// Properties
	const glm::vec3 GetMidPoint() const;
	const glm::vec3 GetVector() const;
	const float GetLength() const;
	const float GetLengthSquared() const;

	// Operations
	const glm::vec3 GetInterpolatedPoint(float t) const;

	glm::vec3 m_startPoint;
	glm::vec3 m_endPoint;
};

#endif