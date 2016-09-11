/*************************************************************************
> File Name: Frustum.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A geometric implementation of the viewing frustum for a viewport.
> Created Time: 2016/06/23
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_FRUSTUM_H
#define CUBBY_FRUSTUM_H

#include <Maths/Plane3D.h>

// Avoid conflict with #define FAR far and #define NEAR near
#undef FAR
#undef NEAR

enum class FrustumPlane
{
	TOP = 0,
	BOTTOM,
	LEFT,
	RIGHT,
	NEAR,
	FAR,
};

enum class FrustumBoundary
{
	OUTSIDE = 0,
	INTERSECT,
	INSIDE,
};

class Frustum
{
public:
	// Constructor, Destructor
	Frustum();
	~Frustum();

	// Setter
	void SetFrustum(float angle, float ratio, float nearDistance, float farDistance);
	void SetCamera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);

	// Operations
	FrustumBoundary PointInFrustum(const glm::vec3& point) const;
	FrustumBoundary SphereInFrustum(const glm::vec3& point, float radius) const;
	FrustumBoundary CubeInFrustum(const glm::vec3& center, float x, float y, float z) const;

private:
	Plane3D m_planes[6];

	glm::vec3 m_nearTopLeft, m_nearTopRight, m_nearBottomLeft, m_nearBottomRight;
	glm::vec3 m_farTopLeft, m_farTopRight, m_farBottomLeft, m_farBottomRight;

	float m_nearDistance, m_farDistance;
	float m_nearWidth, m_nearHeight;
	float m_farWidth, m_farHeight;
	float m_ratio, m_angle, m_tang;
};

#endif