/*************************************************************************
> File Name: Frustum.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A geometric implementation of the viewing frustum for a viewport.
> Created Time: 2016/06/23
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <cmath>

#include <../Libraries/glm/gtx/norm.hpp>

#include "Frustum.h"
#include "Maths/3DMaths.h"

// Constructor, Destructor
Frustum::Frustum() : 
	m_nearDistance(0.0f), m_farDistance(0.0f),
	m_nearWidth(0.0f), m_nearHeight(0.0f),
	m_farWidth(0.0f), m_farHeight(0.0f),
	m_ratio(0.0f), m_angle(0.0f), m_tang(0.0f)
{
	// Do nothing
}

Frustum::~Frustum()
{
	// Do nothing
}

// Setter
void Frustum::SetFrustum(float angle, float ratio, float nearDistance, float farDistance)
{
	m_ratio = ratio;
	m_angle = angle;
	m_nearDistance = nearDistance;
	m_farDistance = farDistance;

	m_tang = static_cast<float>(tan(DegreeToRadian(angle) * 0.5));
	m_nearHeight = m_nearDistance * m_tang;
	m_nearWidth = m_nearHeight * ratio;
	m_farHeight = m_farDistance * m_tang;
	m_farWidth = m_farHeight * ratio;
}

void Frustum::SetCamera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
{
	glm::vec3 nc, fc, x, y, z;

	z = pos - target;
	z = normalize(z);

	x = cross(up, z);
	x = normalize(x);

	y = cross(z, x);

	nc = pos - z * m_nearDistance;
	fc = pos - z * m_farDistance;

	m_nearTopLeft		= nc + y * m_nearHeight - x * m_nearWidth;
	m_nearTopRight		= nc + y * m_nearHeight + x * m_nearWidth;
	m_nearBottomLeft	= nc - y * m_nearHeight - x * m_nearWidth;
	m_nearBottomRight	= nc - y * m_nearHeight + x * m_nearWidth;

	m_farTopLeft		= fc + y * m_farHeight - x * m_farWidth;
	m_farTopRight		= fc + y * m_farHeight + x * m_farWidth;
	m_farBottomLeft		= fc - y * m_farHeight - x * m_farWidth;
	m_farBottomRight	= fc - y * m_farHeight + x * m_farWidth;

	m_planes[static_cast<int>(FrustumPlane::TOP)]		= Plane3D(m_nearTopRight, m_nearTopLeft, m_farTopLeft);
	m_planes[static_cast<int>(FrustumPlane::BOTTOM)]	= Plane3D(m_nearBottomLeft, m_nearBottomRight, m_farBottomRight);
	m_planes[static_cast<int>(FrustumPlane::LEFT)]		= Plane3D(m_nearTopLeft, m_nearBottomLeft, m_farBottomLeft);
	m_planes[static_cast<int>(FrustumPlane::RIGHT)]		= Plane3D(m_nearBottomRight, m_nearTopRight, m_farBottomRight);
	m_planes[static_cast<int>(FrustumPlane::NEAR)]		= Plane3D(m_nearTopLeft, m_nearTopRight, m_nearBottomRight);
	m_planes[static_cast<int>(FrustumPlane::FAR)]		= Plane3D(m_farTopRight, m_farTopLeft, m_farBottomLeft);
}

// Operations
FrustumBoundary Frustum::PointInFrustum(const glm::vec3& point) const
{
	FrustumBoundary result = FrustumBoundary::INSIDE;

	for (int i = 0; i < 6; ++i)
	{
		if (m_planes[i].GetPointDistance(point) < 0)
		{
			return FrustumBoundary::OUTSIDE;
		}
	}

	return result;
}

FrustumBoundary Frustum::SphereInFrustum(const glm::vec3& point, float radius) const
{
	FrustumBoundary result = FrustumBoundary::INSIDE;
	float distance;

	for (int i = 0; i < 6; ++i)
	{
		distance = m_planes[i].GetPointDistance(point);

		if (distance < -radius)
		{
			return FrustumBoundary::OUTSIDE;
		}
		
		if (distance < radius)
		{
			result = FrustumBoundary::INTERSECT;
		}
	}

	return result;
}

FrustumBoundary Frustum::CubeInFrustum(const glm::vec3& center, float x, float y, float z) const
{
	FrustumBoundary result = FrustumBoundary::INSIDE;

	for (int i = 0; i < 6; ++i)
	{
		// Reset counters for corners in and out
		int out = 0, in = 0;

		if (m_planes[i].GetPointDistance(center + glm::vec3(-x, -y, -z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (m_planes[i].GetPointDistance(center + glm::vec3(x, -y, -z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (m_planes[i].GetPointDistance(center + glm::vec3(-x, -y, z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (m_planes[i].GetPointDistance(center + glm::vec3(x, -y, z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (m_planes[i].GetPointDistance(center + glm::vec3(-x, y, -z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (m_planes[i].GetPointDistance(center + glm::vec3(x, y, -z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (m_planes[i].GetPointDistance(center + glm::vec3(-x, y, z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		if (m_planes[i].GetPointDistance(center + glm::vec3(x, y, z)) < 0)
		{
			out++;
		}
		else
		{
			in++;
		}

		// If all corners are out
		if (!in)
		{
			return FrustumBoundary::OUTSIDE;
		}
		
		// If some corners are out and others are in
		if (out)
		{
			result = FrustumBoundary::INTERSECT;
		}
	}

	return result;
}