/*************************************************************************
> File Name: Material.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An OpenGL material type.
> Created Time: 2016/06/22
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MATERIAL_H
#define CUBBY_MATERIAL_H

#include "Color.h"

class Material
{
public:
	// Constructor
	Material();

	// Getter, Setter
	const Color GetAmbient() const;
	const Color GetDiffuse() const;
	const Color GetSpecular() const;
	const Color GetEmission() const;
	const float GetShininess() const;
	void SetAmbient(const Color& ambient);
	void SetDiffuse(const Color& diffuse);
	void SetSpecular(const Color& specular);
	void SetEmission(const Color& emission);
	void SetShininess(const float shininess);

	// Operations
	void Apply() const;

private:
	Color m_ambient;
	Color m_diffuse;
	Color m_specular;
	Color m_emission;
	float m_shininess;
};

#endif