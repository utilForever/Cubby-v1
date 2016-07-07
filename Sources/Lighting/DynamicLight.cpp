/*************************************************************************
> File Name: DynamicLight.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Dynamic light class.
> Created Time: 2016/07/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DynamicLight.h"

// Constructor, Destructor
DynamicLight::DynamicLight() :
	m_lightID(0), m_radius(0.0f), m_diffuseScale(0.0f),
	m_lifeTime(0.0f), m_maxLifeTime(0.0f), m_isDyingLight(false), m_isNeedErase(false)
{

}

DynamicLight::~DynamicLight()
{

}

// Erase operations
void DynamicLight::SetErase(bool erase)
{
	m_isNeedErase = erase;
}

bool DynamicLight::IsNeedErasing() const
{
	return m_isNeedErase;
}

// Update
void DynamicLight::Update(float dt)
{
	if (m_isDyingLight)
	{
		if (m_lifeTime >= 0.0f)
		{
			m_lifeTime -= dt;

			m_diffuseScale = m_lifeTime / m_maxLifeTime;
		}
		else
		{
			SetErase(true);
		}
	}
}