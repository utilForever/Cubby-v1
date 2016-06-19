/*************************************************************************
> File Name: BoundingRegion.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3d maths - Bounding region library.
> Created Time: 2016/06/20
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BOUNDING_REGION_H
#define CUBBY_BOUNDING_REGION_H

#include <vector>

#include "Matrix4.h"
#include "Plane3D.h"

enum class BoundingRegionType
{
	BoundingRegionType_Sphere = 0,
	BoundingRegionType_Cube,
};

class BoundingRegion
{
public:
	// Constructor
	BoundingRegion();
	// Destructor
	~BoundingRegion();

	// Operations
	void UpdatePlanes(Matrix4 transformationMatrix, float scale);
	// TODO: Implement Render();
	// void Render(Renderer* pRenderer);

private:
	BoundingRegionType m_boundingType;
	glm::vec3 m_origin;
	float m_radius;
	float m_xLength;
	float m_yLength;
	float m_zLength;
	float m_scale;
	Plane3D m_planes[6];
};

using BoundingRegionList = std::vector<BoundingRegion*>;

#endif