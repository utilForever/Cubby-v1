/*************************************************************************
> File Name: BoundingRegion.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3d maths - Bounding region library.
> Created Time: 2016/06/20
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "BoundingRegion.h"

BoundingRegion::BoundingRegion()
{
	// Do nothing
}

BoundingRegion::~BoundingRegion()
{
	// Do nothing
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

void BoundingRegion::Render(Renderer* pRenderer)
{
	if (m_boundingType == BoundingRegionType::Sphere)
	{
		pRenderer->PushMatrix();

		pRenderer->SetLineWidth(1.0f);
		pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 0.25f);
		pRenderer->SetRenderMode(RenderMode::WIREFRAME);

		pRenderer->TranslateWorldMatrix(m_origin.x, m_origin.y, m_origin.z);

		pRenderer->ScaleWorldMatrix(m_scale, m_scale, m_scale);

		pRenderer->DrawSphere(m_radius, 20, 20);

		pRenderer->PopMatrix();
	}

	if (m_boundingType == BoundingRegionType::Cube)
	{
		pRenderer->PushMatrix();

		pRenderer->SetLineWidth(1.0f);
		pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 0.25f);
		pRenderer->SetRenderMode(RenderMode::WIREFRAME);
		pRenderer->SetCullMode(CullMode::NOCULL);

		pRenderer->ScaleWorldMatrix(m_scale, m_scale, m_scale);

		pRenderer->TranslateWorldMatrix(m_origin.x, m_origin.y, m_origin.z);

		float length = m_xLength;
		float height = m_yLength;
		float width = m_zLength;

		pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

		pRenderer->ImmediateNormal(0.0f, 0.0f, -1.0f);
		pRenderer->ImmediateVertex(length, -height, -width);
		pRenderer->ImmediateVertex(-length, -height, -width);
		pRenderer->ImmediateVertex(-length, height, -width);
		pRenderer->ImmediateVertex(length, height, -width);

		pRenderer->ImmediateNormal(0.0f, 0.0f, 1.0f);
		pRenderer->ImmediateVertex(-length, -height, width);
		pRenderer->ImmediateVertex(length, -height, width);
		pRenderer->ImmediateVertex(length, height, width);
		pRenderer->ImmediateVertex(-length, height, width);

		pRenderer->ImmediateNormal(1.0f, 0.0f, 0.0f);
		pRenderer->ImmediateVertex(length, -height, width);
		pRenderer->ImmediateVertex(length, -height, -width);
		pRenderer->ImmediateVertex(length, height, -width);
		pRenderer->ImmediateVertex(length, height, width);

		pRenderer->ImmediateNormal(-1.0f, 0.0f, 0.0f);
		pRenderer->ImmediateVertex(-length, -height, -width);
		pRenderer->ImmediateVertex(-length, -height, width);
		pRenderer->ImmediateVertex(-length, height, width);
		pRenderer->ImmediateVertex(-length, height, -width);

		pRenderer->ImmediateNormal(0.0f, -1.0f, 0.0f);
		pRenderer->ImmediateVertex(-length, -height, -width);
		pRenderer->ImmediateVertex(length, -height, -width);
		pRenderer->ImmediateVertex(length, -height, width);
		pRenderer->ImmediateVertex(-length, -height, width);

		pRenderer->ImmediateNormal(0.0f, 1.0f, 0.0f);
		pRenderer->ImmediateVertex(length, height, -width);
		pRenderer->ImmediateVertex(-length, height, -width);
		pRenderer->ImmediateVertex(-length, height, width);
		pRenderer->ImmediateVertex(length, height, width);

		pRenderer->DisableImmediateMode();

		pRenderer->SetCullMode(CullMode::BACK);
		pRenderer->PopMatrix();
	}
}