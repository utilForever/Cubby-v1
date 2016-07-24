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

// Getter, Setter
glm::vec3 DynamicLight::GetPosition() const
{
	return m_position;
}

float DynamicLight::GetRadius() const
{
	return m_radius;
}

float DynamicLight::GetDiffuseScale() const
{
	return m_diffuseScale;
}

Color DynamicLight::GetColor() const
{
	return m_color;
}

unsigned int DynamicLight::GetLightID() const
{
	return m_lightID;
}

float DynamicLight::GetLifeTime() const
{
	return m_lifeTime;
}

float DynamicLight::GetMaxLifeTime() const
{
	return m_maxLifeTime;
}

bool DynamicLight::IsDyingLight() const
{
	return m_isDyingLight;
}

void DynamicLight::SetPosition(glm::vec3 position)
{
	m_position = position;
}

void DynamicLight::SetRadius(float radius)
{
	m_radius = radius;
}

void DynamicLight::SetDiffuseScale(float diffuseScale)
{
	m_diffuseScale = diffuseScale;
}

void DynamicLight::SetColor(Color color)
{
	m_color = color;
}

void DynamicLight::SetLightID(unsigned int lightID)
{
	m_lightID = lightID;
}

void DynamicLight::SetLifeTime(float lifeTime)
{
	m_lifeTime = lifeTime;
}

void DynamicLight::SetMaxLifeTime(float maxLifeTime)
{
	m_maxLifeTime = maxLifeTime;
}

void DynamicLight::SetDyingLight(bool isDyingLight)
{
	m_isDyingLight = isDyingLight;
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