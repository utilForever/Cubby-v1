/*************************************************************************
> File Name: Material.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An OpenGL material type.
> Created Time: 2016/06/22
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <glew/include/GL/glew.h>

#include "Material.h"

// Constructor
Material::Material() :
	m_shininess(0.0f)
{
	
}

// Getter, Setter
const Color Material::GetAmbient() const
{
	return m_ambient;
}

const Color Material::GetDiffuse() const
{
	return m_diffuse;
}

const Color Material::GetSpecular() const
{
	return m_specular;
}

const Color Material::GetEmission() const
{
	return m_emission;
}

const float Material::GetShininess() const
{
	return m_shininess;
}

void Material::SetAmbient(const Color& ambient)
{
	m_ambient = ambient;
}

void Material::SetDiffuse(const Color& diffuse)
{
	m_diffuse = diffuse;
}

void Material::SetSpecular(const Color& specular)
{
	m_specular = specular;
}

void Material::SetEmission(const Color& emission)
{
	m_emission = emission;
}

void Material::SetShininess(const float shininess)
{
	m_shininess = shininess;
}

// Operations
void Material::Apply() const
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient.GetRGBA());
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse.GetRGBA());
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular.GetRGBA());
	glMaterialfv(GL_FRONT, GL_EMISSION, m_emission.GetRGBA());
	glMaterialf(GL_FRONT, GL_SHININESS, m_shininess);
}