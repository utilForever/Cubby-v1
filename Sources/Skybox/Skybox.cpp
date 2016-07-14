/*************************************************************************
> File Name: Skybox.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A cube mapped skybox render. Support for 2 concurrent skyboxes and blending
>    between the different skyboxes via the skybox cube shader.
> Created Time: 2016/07/14
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Skybox.h"

// Constructor, Destructor
Skybox::Skybox(Renderer* pRenderer) :
	m_pRenderer(pRenderer)
{
	SetSkybox1("glacier");
	SetSkybox2("delirious");

	m_skyBoxNames[static_cast<int>(Biome::None)]		= "glacier";
	m_skyBoxNames[static_cast<int>(Biome::GrassLand)]	= "glacier";
	m_skyBoxNames[static_cast<int>(Biome::Desert)]		= "desert";
	m_skyBoxNames[static_cast<int>(Biome::Tundra)]		= "blue_sky";
	m_skyBoxNames[static_cast<int>(Biome::AshLand)]		= "delirious";
}

Skybox::~Skybox()
{

}

void Skybox::SetSkybox1(std::string name)
{
	m_skyBox1Name = name;

	std::string front = "Resources/Textures/Skyboxes/" + m_skyBox1Name + "/front.tga";
	std::string back = "Resources/Textures/Skyboxes/" + m_skyBox1Name + "/back.tga";
	std::string top = "Resources/Textures/Skyboxes/" + m_skyBox1Name + "/top.tga";
	std::string bottom = "Resources/Textures/Skyboxes/" + m_skyBox1Name + "/bottom.tga";
	std::string left = "Resources/Textures/Skyboxes/" + m_skyBox1Name + "/left.tga";
	std::string right = "Resources/Textures/Skyboxes/" + m_skyBox1Name + "/right.tga";

	int textureWidth, textureHeight;
	m_pRenderer->LoadCubeTexture(&textureWidth, &textureHeight, front.c_str(), back.c_str(), top.c_str(), bottom.c_str(), left.c_str(), right.c_str(), &m_cubeTextureID1);
}

void Skybox::SetSkybox2(std::string name)
{
	m_skyBox2Name = name;

	std::string front = "Resources/Textures/Skyboxes/" + m_skyBox2Name + "/front.tga";
	std::string back = "Resources/Textures/Skyboxes/" + m_skyBox2Name + "/back.tga";
	std::string top = "Resources/Textures/Skyboxes/" + m_skyBox2Name + "/top.tga";
	std::string bottom = "Resources/Textures/Skyboxes/" + m_skyBox2Name + "/bottom.tga";
	std::string left = "Resources/Textures/Skyboxes/" + m_skyBox2Name + "/left.tga";
	std::string right = "Resources/Textures/Skyboxes/" + m_skyBox2Name + "/right.tga";

	int textureWidth, textureHeight;
	m_pRenderer->LoadCubeTexture(&textureWidth, &textureHeight, front.c_str(), back.c_str(), top.c_str(), bottom.c_str(), left.c_str(), right.c_str(), &m_cubeTextureID2);
}

unsigned int Skybox::GetCubeMapTexture1() const
{
	return m_cubeTextureID1;
}

unsigned int Skybox::GetCubeMapTexture2() const
{
	return m_cubeTextureID2;
}

void Skybox::SetCurrentBiome(Biome currentBiome)
{
	SetSkybox1(m_skyBoxNames[static_cast<int>(currentBiome)]);
}

void Skybox::Render() const
{
	float width = 4000.0f;
	float height = 4000.0f;
	float length = 4000.0f;

	// Center the Skybox around the given x,y,z position
	float x = -(width * 0.5f);
	float y = -(height * 0.5f);
	float z = -(length * 0.5f);


	m_pRenderer->SetRenderMode(RenderMode::TEXTURED);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);

	// Draw Front side
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y, z + length);
	glEnd();

	// Draw Back side
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, z);
	glEnd();

	// Draw Right side
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y + height, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y, z);
	glEnd();

	// Draw Left side
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y, z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z + length);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y + height, z);
	glEnd();

	// Draw Top side
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z);
	glEnd();

	// Draw Bottom side
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z);
	glEnd();
}
