/*************************************************************************
> File Name: Skybox.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A cube mapped skybox render. Support for 2 concurrent skyboxes and blending
>    between the different skyboxes via the skybox cube shader.
> Created Time: 2016/07/14
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_SKYBOX_H
#define CUBBY_SKYBOX_H

#include <Blocks/BiomeManager.h>
#include <Renderer/Renderer.h>

class Skybox
{
public:
	// Constructor, Destructor
	Skybox(Renderer* pRenderer);
	~Skybox();

	void SetSkybox1(std::string name);
	void SetSkybox2(std::string name);

	unsigned int GetCubeMapTexture1() const;
	unsigned int GetCubeMapTexture2() const;

	void SetCurrentBiome(Biome currentBiome);

	void Render() const;

private:
	Renderer* m_pRenderer;

	unsigned int m_cubeTextureID1;
	unsigned int m_cubeTextureID2;

	std::string m_skyBox1Name;
	std::string m_skyBox2Name;

	std::string m_skyBoxNames[static_cast<int>(Biome::NumBiomes)];
};

#endif