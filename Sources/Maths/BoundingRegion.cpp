/*************************************************************************
> File Name: BoundingRegion.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3d maths - Bounding region library.
> Created Time: 2016/06/20
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "BoundingRegion.h"

BoundingRegion::BoundingRegion() {
	/* Nothing */
}

BoundingRegion::~BoundingRegion()
{
	/* Nothing */
}

void BoundingRegion::UpdatePlanes(Matrix4 transformationMatrix, float scale)
{
	m_planes[0] = Plane3D(transformationMatrix * (glm::vec3(-1.0f, 0.0f, 0.0f) * scale), transformationMatrix * (glm::vec3(m_xLength * m_scale, 0.0f, 0.0f) * scale));
	m_planes[1] = Plane3D(transformationMatrix * (glm::vec3(1.0f, 0.0f, 0.0f) * scale), transformationMatrix * (glm::vec3(-m_xLength * m_scale, 0.0f, 0.0f) * scale));
	m_planes[2] = Plane3D(transformationMatrix * (glm::vec3(0.0f, -1.0f, 0.0f) * scale), transformationMatrix * (glm::vec3(0.0f, m_yLength * m_scale, 0.0f) * scale));
	m_planes[3] = Plane3D(transformationMatrix * (glm::vec3(0.0f, 1.0f, 0.0f) * scale), transformationMatrix * (glm::vec3(0.0f, -m_yLength * m_scale, 0.0f) * scale));
	m_planes[4] = Plane3D(transformationMatrix * (glm::vec3(0.0f, 0.0f, -1.0f) * scale), transformationMatrix * (glm::vec3(0.0f, 0.0f, m_zLength * m_scale) * scale));
	m_planes[5] = Plane3D(transformationMatrix * (glm::vec3(0.0f, 0.0f, 1.0f) * scale), transformationMatrix * (glm::vec3(0.0f, 0.0f, -m_zLength * m_scale) * scale));
}

// TODO: Implement Render();
// void BoundingRegion::Render(Renderer* pRenderer);