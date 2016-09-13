/*************************************************************************
> File Name: BoundingBox.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Bounding box.
> Created Time: 2016/06/26
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "BoundingBox.h"

// Constructor, Destructor
BoundingBox::BoundingBox() :
	m_minX(0.0f), m_minY(0.0f), m_minZ(0.0f),
	m_maxX(0.0f), m_maxY(0.0f), m_maxZ(0.0f)
{
	
}

BoundingBox::~BoundingBox()
{

}

// Getter, Setter
float BoundingBox::GetMinX() const
{
	return m_minX;
}

float BoundingBox::GetMinY() const
{
	return m_minY;
}

float BoundingBox::GetMinZ() const
{
	return m_minZ;
}

float BoundingBox::GetMaxX() const
{
	return m_maxX;
}

float BoundingBox::GetMaxY() const
{
	return m_maxY;
}

float BoundingBox::GetMaxZ() const
{
	return m_maxZ;
}

float BoundingBox::GetWidth() const
{
	return m_maxX - m_minX;
}

float BoundingBox::GetHeight() const
{
	return m_maxY - m_minY;
}

float BoundingBox::GetLength() const
{
	return m_maxZ - m_minZ;
}

void BoundingBox::SetMinX(float x)
{
	m_minX = x;
}

void BoundingBox::SetMinY(float y)
{
	m_minY = y;
}

void BoundingBox::SetMinZ(float z)
{
	m_minZ = z;
}

void BoundingBox::SetMaxX(float x)
{
	m_maxX = x;
}

void BoundingBox::SetMaxY(float y)
{
	m_maxY = y;
}

void BoundingBox::SetMaxZ(float z)
{
	m_maxZ = z;
}

glm::vec3 BoundingBox::GetCenter() const
{
	return glm::vec3(m_maxX - m_minX * 0.5f, m_maxY - m_minY * 0.5f, m_maxZ - m_minZ * 0.5f);
}