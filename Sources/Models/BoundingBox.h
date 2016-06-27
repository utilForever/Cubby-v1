/*************************************************************************
> File Name: BoundingBox.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Bounding box.
> Created Time: 2016/06/26
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BOUNDING_BOX_H
#define CUBBY_BOUNDING_BOX_H

#include <glm/vec3.hpp>

class BoundingBox
{
public:
	// Constructor, Destructor
	BoundingBox();
	~BoundingBox();

	// Getter, Setter
	float GetMinX() const;
	float GetMinY() const;
	float GetMinZ() const;

	float GetMaxX() const;
	float GetMaxY() const;
	float GetMaxZ() const;

	float GetWidth() const;
	float GetHeight() const;
	float GetLength() const;

	glm::vec3 GetCenter() const;

	void SetMinX(float x);
	void SetMinY(float y);
	void SetMinZ(float z);

	void SetMaxX(float x);
	void SetMaxY(float y);
	void SetMaxZ(float z);

private:
	float m_minX;
	float m_minY;
	float m_minZ;

	float m_maxX;
	float m_maxY;
	float m_maxZ;
};

#endif