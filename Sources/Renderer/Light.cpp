/*************************************************************************
> File Name: Light.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An OpenGL hardware light.
> Created Time: 2016/06/21
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Light.h"

// Constructor
Light::Light()
{
	
}

// Getter, Setter
const Color Light::GetAmbient() const
{
	return m_ambient;
}

const Color Light::GetDiffuse() const
{
	return m_diffuse;
}

const Color Light::GetSpecular() const
{
	return m_specular;
}

const glm::vec3 Light::GetPosition() const
{
	return m_position;
}

const glm::vec3 Light::GetDirection() const
{
	return m_direction;
}

const float Light::GetExponent() const
{
	return m_exponent;
}

const float Light::GetCutoff() const
{
	return m_cutoff;
}

const float Light::GetConstantAttenuation() const
{
	return m_constantAttenuation;
}

const float Light::GetLinearAttenuation() const
{
	return m_linearAttenuation;
}

const float Light::GetQuadraticAttenuation() const
{
	return m_quadraticAttenuation;
}

const bool Light::GetPoint() const
{
	return m_point;
}

const bool Light::GetSpotlight() const
{
	return m_spotlight;
}

void Light::SetAmbient(const Color& ambient)
{
	m_ambient = ambient;
}

void Light::SetDiffuse(const Color& diffuse)
{
	m_diffuse = diffuse;
}

void Light::SetSpecular(const Color& specular)
{
	m_specular = specular;
}

void Light::SetPosition(const glm::vec3& position)
{
	m_position = position;
}

void Light::SetDirection(const glm::vec3& direction)
{
	m_direction = direction;
}

void Light::SetExponent(const float exponent)
{
	m_exponent = exponent;
}

void Light::SetCutoff(const float cutoff)
{
	m_cutoff = cutoff;
}

void Light::SetConstantAttenuation(const float constantAttenuation)
{
	m_constantAttenuation = constantAttenuation;
}

void Light::SetLinearAttenuation(const float linearAttenuation)
{
	m_linearAttenuation = linearAttenuation;
}

void Light::SetQuadraticAttenuation(const float quadraticAttenuation)
{
	m_quadraticAttenuation = quadraticAttenuation;
}

void Light::SetPoint(const bool point)
{
	m_point = point;
}

void Light::SetSpotlight(const bool spotlight)
{
	m_spotlight = spotlight;
}

// Apply light
inline void Light::Apply(int id) const
{
	// TODO: Implement Apply();
}

inline void Light::Render()
{
	// TODO: Implement Render();
}
