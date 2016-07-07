/*************************************************************************
> File Name: LightingManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Lighting manager class.
> Created Time: 2016/07/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_LIGHTING_MANAGER_H
#define CUBBY_LIGHTING_MANAGER_H

#include <vector>

#include "DynamicLight.h"
#include "Renderer/Renderer.h"

using DynamicLightList = std::vector<DynamicLight*>;

class LightingManager
{
public:
	// Constructor, Destructor
	LightingManager(Renderer* pRenderer);
	~LightingManager();

	// Getter
	int GetNumLights() const;
	DynamicLight* GetLight(int index);

	// Clean-up
	void ClearLights();

	// Add & Remove light
	void AddLight(glm::vec3 position, float radius, float diffuseModifier, Color color, unsigned int* pID);
	void AddDyingLight(glm::vec3 position, float radius, float diffuseModifier, Color color, float lifeTime, unsigned int* pID);
	void RemoveLight(unsigned int lightID);

	// Updates
	void UpdateLight(unsigned int lightId, glm::vec3 position, float radius, float diffuseModifier, Color color);
	void UpdateLightRadius(unsigned int lightID, float radius);
	void UpdateLightDiffuseMultiplier(unsigned int lightID, float diffuseMultiplier);
	void UpdateLightPosition(unsigned int lightID, glm::vec3 position);
	void UpdateLightColor(unsigned int lightID, Color color);

	void Update(float dt);

	// Debug
	void DebugRender();

private:
	Renderer* m_pRenderer;

	DynamicLightList m_vpDynamicLightList;

	unsigned int m_lightIndexCounter;
};

#endif