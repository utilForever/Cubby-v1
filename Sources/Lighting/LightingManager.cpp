/*************************************************************************
> File Name: LightingManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Lighting manager class.
> Created Time: 2016/07/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include "LightingManager.h"

// Constructor, Destructor
LightingManager::LightingManager(Renderer* pRenderer) :
	m_pRenderer(pRenderer), m_lightIndexCounter(0)
{
	
}

LightingManager::~LightingManager()
{
	ClearLights();
}

// Getter
int LightingManager::GetNumLights() const
{
	int numLights = m_vpDynamicLightList.size();
	return numLights;
}

DynamicLight* LightingManager::GetLight(int index)
{
	DynamicLight* pLight = m_vpDynamicLightList[index];
	return pLight;
}

// Clean-up
void LightingManager::ClearLights()
{
	for (size_t i = 0; i < m_vpDynamicLightList.size(); ++i)
	{
		delete m_vpDynamicLightList[i];
		m_vpDynamicLightList[i] = nullptr;
	}

	m_vpDynamicLightList.clear();
}

// Add & Remove light
void LightingManager::AddLight(glm::vec3 position, float radius, float diffuseModifier, Color color, unsigned int* pID)
{
	DynamicLight* pNewLight = new DynamicLight();

	pNewLight->SetPosition(position);
	pNewLight->SetRadius(radius);
	pNewLight->SetDiffuseScale(diffuseModifier);
	pNewLight->SetColor(color);
	pNewLight->SetLightID(m_lightIndexCounter);

	*pID = m_lightIndexCounter;

	m_lightIndexCounter++;

	m_vpDynamicLightList.push_back(pNewLight);
}

void LightingManager::AddDyingLight(glm::vec3 position, float radius, float diffuseModifier, Color color, float lifeTime, unsigned int* pID)
{
	DynamicLight* pNewLight = new DynamicLight();

	pNewLight->SetPosition(position);
	pNewLight->SetRadius(radius);
	pNewLight->SetDiffuseScale(diffuseModifier);
	pNewLight->SetColor(color);
	pNewLight->SetLightID(m_lightIndexCounter);

	pNewLight->SetLifeTime(lifeTime);
	pNewLight->SetMaxLifeTime(lifeTime);
	pNewLight->SetDyingLight(true);
	
	*pID = m_lightIndexCounter;

	m_lightIndexCounter++;

	m_vpDynamicLightList.push_back(pNewLight);
}

void LightingManager::RemoveLight(unsigned int lightID)
{
	for (size_t i = 0; i < m_vpDynamicLightList.size(); ++i)
	{
		if (m_vpDynamicLightList[i]->GetLightID() == lightID)
		{
			m_vpDynamicLightList[i]->SetErase(true);
		}
	}

	// Remove any lights that need to be erased
	m_vpDynamicLightList.erase(remove_if(m_vpDynamicLightList.begin(), m_vpDynamicLightList.end(),
		[](DynamicLight* light)
	{
		bool isNeedErase = light->IsNeedErasing();

		if (isNeedErase)
		{
			delete light;
		}

		return isNeedErase;
	}), m_vpDynamicLightList.end());
}

// Updates
void LightingManager::UpdateLight(unsigned int lightID, glm::vec3 position, float radius, float diffuseModifier, Color color)
{
	for (size_t i = 0; i < m_vpDynamicLightList.size(); ++i)
	{
		if (m_vpDynamicLightList[i]->GetLightID() == lightID)
		{
			m_vpDynamicLightList[i]->SetPosition(position);
			m_vpDynamicLightList[i]->SetRadius(radius);
			m_vpDynamicLightList[i]->SetDiffuseScale(diffuseModifier);
			m_vpDynamicLightList[i]->SetColor(color);

			return;
		}
	}
}

void LightingManager::UpdateLightRadius(unsigned int lightID, float radius)
{
	for (size_t i = 0; i < m_vpDynamicLightList.size(); ++i)
	{
		if (m_vpDynamicLightList[i]->GetLightID() == lightID)
		{
			m_vpDynamicLightList[i]->SetRadius(radius);

			return;
		}
	}
}

void LightingManager::UpdateLightDiffuseMultiplier(unsigned int lightID, float diffuseMultiplier)
{
	for (size_t i = 0; i < m_vpDynamicLightList.size(); ++i)
	{
		if (m_vpDynamicLightList[i]->GetLightID() == lightID)
		{
			m_vpDynamicLightList[i]->SetDiffuseScale(diffuseMultiplier);

			return;
		}
	}
}

void LightingManager::UpdateLightPosition(unsigned int lightID, glm::vec3 position)
{
	for (size_t i = 0; i < m_vpDynamicLightList.size(); ++i)
	{
		if (m_vpDynamicLightList[i]->GetLightID() == lightID)
		{
			m_vpDynamicLightList[i]->SetPosition(position);

			return;
		}
	}
}

void LightingManager::UpdateLightColor(unsigned int lightID, Color color)
{
	for (size_t i = 0; i < m_vpDynamicLightList.size(); ++i)
	{
		if (m_vpDynamicLightList[i]->GetLightID() == lightID)
		{
			m_vpDynamicLightList[i]->SetColor(color);

			return;
		}
	}
}

void LightingManager::Update(float dt)
{
	// Remove any lights that need to be erased
	m_vpDynamicLightList.erase(remove_if(m_vpDynamicLightList.begin(), m_vpDynamicLightList.end(),
		[](DynamicLight* light)
		{
			bool isNeedErase = light->IsNeedErasing();

			if (isNeedErase)
			{
				delete light;
			}

			return isNeedErase;
		}), m_vpDynamicLightList.end());

	for (size_t i = 0; i < m_vpDynamicLightList.size(); ++i)
	{
		m_vpDynamicLightList[i]->Update(dt);
	}
}

// Debug
void LightingManager::DebugRender()
{
	for (size_t i = 0; i < m_vpDynamicLightList.size(); ++i)
	{
		DynamicLight* pLight = m_vpDynamicLightList[i];

		float lightRadius = pLight->GetRadius();
		float r = pLight->GetColor().GetRed();
		float g = pLight->GetColor().GetGreen();
		float b = pLight->GetColor().GetBlue();
		float a = pLight->GetColor().GetAlpha();

		m_pRenderer->SetLineWidth(1.0f);

		m_pRenderer->PushMatrix();
		m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
		m_pRenderer->SetCullMode(CullMode::NOCULL);
		m_pRenderer->TranslateWorldMatrix(pLight->GetPosition().x, pLight->GetPosition().y, pLight->GetPosition().z);
		m_pRenderer->ImmediateColorAlpha(r, g, b, a);
		m_pRenderer->DrawSphere(lightRadius, 30, 30);
		m_pRenderer->PopMatrix();
	}
}