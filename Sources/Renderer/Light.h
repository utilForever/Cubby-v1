/*************************************************************************
> File Name: Light.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An OpenGL hardware light.
> Created Time: 2016/06/21
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_LIGHT_H
#define CUBBY_LIGHT_H

#include <glm/glm.hpp>

#include "Color.h"

class Light
{
public:
	// Constructor
	Light();

	// Getter, Setter
	const Color GetAmbient() const;
	const Color GetDiffuse() const;
	const Color GetSpecular() const;
	const glm::vec3 GetPosition() const;
	const glm::vec3 GetDirection() const;
	const float GetExponent() const;
	const float GetCutoff() const;
	const float GetConstantAttenuation() const;
	const float GetLinearAttenuation() const;
	const float GetQuadraticAttenuation() const;
	const bool GetPoint() const;
	const bool GetSpotlight() const;
	void SetAmbient(const Color& ambient);
	void SetDiffuse(const Color& diffuse);
	void SetSpecular(const Color& specular);
	void SetPosition(const glm::vec3& position);
	void SetDirection(const glm::vec3& direction);
	void SetExponent(const float exponent);
	void SetCutoff(const float cutoff);
	void SetConstantAttenuation(const float constantAttenuation);
	void SetLinearAttenuation(const float linearAttenuation);
	void SetQuadraticAttenuation(const float quadraticAttenuation);
	void SetPoint(const bool point);
	void SetSpotlight(const bool spotlight);

	// Apply light
	void Apply(int id) const;

	void Render();

private:
	Color m_ambient;
	Color m_diffuse;
	Color m_specular;

	glm::vec3 m_position;
	glm::vec3 m_direction;

	float m_exponent;
	float m_cutoff;
	float m_constantAttenuation;
	float m_linearAttenuation;
	float m_quadraticAttenuation;

	bool m_point;
	bool m_spotlight;
};

#endif