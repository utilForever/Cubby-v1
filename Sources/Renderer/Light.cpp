/*************************************************************************
> File Name: Light.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An OpenGL hardware light.
> Created Time: 2016/06/21
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <glew/include/gl/glew.h>

#include "Light.h"

// Constructor
Light::Light() :
	m_exponent(0.0f), m_cutoff(0.0f),
	m_constantAttenuation(0.0f), m_linearAttenuation(0.0f), m_quadraticAttenuation(0.0f),
	m_point(false), m_spotlight(false)
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
void Light::Apply(int id) const
{
	unsigned int lightNum = GL_LIGHT0 + id;

	// Set ambient color
	glLightfv(lightNum, GL_AMBIENT, m_ambient.GetRGBA());

	// Set diffuse color
	glLightfv(lightNum, GL_DIFFUSE, m_diffuse.GetRGBA());

	// Set specular color
	glLightfv(lightNum, GL_SPECULAR, m_specular.GetRGBA());

	// Set light position
	GLfloat position[4];
	position[0] = m_position.x;
	position[1] = m_position.y;
	position[2] = m_position.z;

	if (m_point)
	{
		position[3] = 1.0;

		glLightfv(lightNum, GL_POSITION, position);

		if (m_spotlight)
		{
			GLfloat spotDirection[] = { m_direction.x, m_direction.y, m_direction.z };

			// Set spotlight direction
			glLightfv(lightNum, GL_SPOT_DIRECTION, spotDirection);

			// Set spotlight parameters
			glLightf(lightNum, GL_SPOT_CUTOFF, GetCutoff());
		}

		glLightf(lightNum, GL_SPOT_EXPONENT, GetExponent());
		glLightf(lightNum, GL_CONSTANT_ATTENUATION, GetConstantAttenuation());
		glLightf(lightNum, GL_LINEAR_ATTENUATION, GetLinearAttenuation());
		glLightf(lightNum, GL_QUADRATIC_ATTENUATION, GetQuadraticAttenuation());
	}
	else
	{
		position[3] = 0.0;

		glLightfv(lightNum, GL_POSITION, position);
	}

	glEnable(lightNum);
}

void Light::Render()
{
	GLfloat lightScale = 0.1f;

	glPushMatrix();
	
	glPushAttrib(GL_POLYGON_BIT);
	// Wireframe mode
	glPolygonMode(GL_FRONT, GL_LINE);
	glLineWidth(1.0f);

	glTranslatef(m_position.x, m_position.y, m_position.z);

	glDisable(GL_LIGHTING);

	glBegin(GL_QUADS);

	glColor3f(0.0f, 0.0f, 0.0f);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(lightScale, -lightScale, -lightScale);
	glVertex3f(-lightScale, -lightScale, -lightScale);
	glVertex3f(-lightScale, lightScale, -lightScale);
	glVertex3f(lightScale, lightScale, -lightScale);

	glNormal3f(0.0f, 0.0f, lightScale);
	glVertex3f(-lightScale, -lightScale, lightScale);
	glVertex3f(lightScale, -lightScale, lightScale);
	glVertex3f(lightScale, lightScale, lightScale);
	glVertex3f(-lightScale, lightScale, lightScale);

	glNormal3f(lightScale, 0.0f, 0.0f);
	glVertex3f(lightScale, -lightScale, lightScale);
	glVertex3f(lightScale, -lightScale, -lightScale);
	glVertex3f(lightScale, lightScale, -lightScale);
	glVertex3f(lightScale, lightScale, lightScale);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-lightScale, -lightScale, -lightScale);
	glVertex3f(-lightScale, -lightScale, lightScale);
	glVertex3f(-lightScale, lightScale, lightScale);
	glVertex3f(-lightScale, lightScale, -lightScale);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-lightScale, -lightScale, -lightScale);
	glVertex3f(lightScale, -lightScale, -lightScale);
	glVertex3f(lightScale, -lightScale, lightScale);
	glVertex3f(-lightScale, -lightScale, lightScale);

	glNormal3f(0.0f, lightScale, 0.0f);
	glVertex3f(lightScale, lightScale, -lightScale);
	glVertex3f(-lightScale, lightScale, -lightScale);
	glVertex3f(-lightScale, lightScale, lightScale);
	glVertex3f(lightScale, lightScale, lightScale);

	glEnd();

	glPopAttrib();

	glBegin(GL_QUADS);

	glColor3f(m_diffuse.GetRed(), m_diffuse.GetGreen(), m_diffuse.GetBlue());

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(lightScale, -lightScale, -lightScale);
	glVertex3f(-lightScale, -lightScale, -lightScale);
	glVertex3f(-lightScale, lightScale, -lightScale);
	glVertex3f(lightScale, lightScale, -lightScale);

	glNormal3f(0.0f, 0.0f, lightScale);
	glVertex3f(-lightScale, -lightScale, lightScale);
	glVertex3f(lightScale, -lightScale, lightScale);
	glVertex3f(lightScale, lightScale, lightScale);
	glVertex3f(-lightScale, lightScale, lightScale);

	glNormal3f(lightScale, 0.0f, 0.0f);
	glVertex3f(lightScale, -lightScale, lightScale);
	glVertex3f(lightScale, -lightScale, -lightScale);
	glVertex3f(lightScale, lightScale, -lightScale);
	glVertex3f(lightScale, lightScale, lightScale);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-lightScale, -lightScale, -lightScale);
	glVertex3f(-lightScale, -lightScale, lightScale);
	glVertex3f(-lightScale, lightScale, lightScale);
	glVertex3f(-lightScale, lightScale, -lightScale);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-lightScale, -lightScale, -lightScale);
	glVertex3f(lightScale, -lightScale, -lightScale);
	glVertex3f(lightScale, -lightScale, lightScale);
	glVertex3f(-lightScale, -lightScale, lightScale);

	glNormal3f(0.0f, lightScale, 0.0f);
	glVertex3f(lightScale, lightScale, -lightScale);
	glVertex3f(-lightScale, lightScale, -lightScale);
	glVertex3f(-lightScale, lightScale, lightScale);
	glVertex3f(lightScale, lightScale, lightScale);

	glEnd();

	// Render spotlight direction
	if (m_spotlight)
	{
		glm::vec3 directionUnit = normalize(m_direction);

		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor3f(m_diffuse.GetRed(), m_diffuse.GetGreen(), m_diffuse.GetBlue());

		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(directionUnit.x * 1.0f, directionUnit.y * 1.0f, directionUnit.z * 1.0f);

		glEnd();
	}

	glPopMatrix();
}