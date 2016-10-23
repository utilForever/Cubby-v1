/*************************************************************************
> File Name: BiomeManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Biome manager.
> Created Time: 2016/07/09
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "BiomeManager.h"

// Constructor, Destructor
BiomeManager::BiomeManager(Renderer* pRenderer) :
	m_pRenderer(pRenderer)
{
	// Biome, Boundary, r1, g1, b1, r2, g2, b2, BlockType
	AddBiomeBoundary(Biome::GrassLand, -0.50f, 0.50f, 0.50f, 0.50f, 0.35f, 0.40f, 0.37f, BlockType::Stone);
	AddBiomeBoundary(Biome::GrassLand, 0.00f, 0.94f, 0.74f, 0.34f, 0.50f, 0.29f, 0.20f, BlockType::Dirt); // Shore
	AddBiomeBoundary(Biome::GrassLand, 0.50f, 0.00f, 0.56f, 0.26f, 0.75f, 0.90f, 0.00f, BlockType::Grass);
	AddBiomeBoundary(Biome::GrassLand, 1.00f, 0.85f, 0.85f, 0.85f, 0.77f, 0.65f, 0.80f, BlockType::Snow);

	AddBiomeBoundary(Biome::Desert, 1.00f, 0.94f, 0.90f, 0.12f, 0.98f, 1.0f, 0.60f, BlockType::Sand);

	AddBiomeBoundary(Biome::Tundra, -0.25f, 0.05f, 0.05f, 0.78f, 0.05f, 0.05f, 0.78f, BlockType::Stone);
	AddBiomeBoundary(Biome::Tundra, -0.00f, 0.37f, 0.61f, 0.85f, 0.30f, 0.56f, 0.95f, BlockType::Stone);
	AddBiomeBoundary(Biome::Tundra, 0.0625f, 0.75f, 0.83f, 0.60f, 0.78f, 0.88f, 0.65f, BlockType::Grass); // Shore
	AddBiomeBoundary(Biome::Tundra, 0.40f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, BlockType::Snow);
	AddBiomeBoundary(Biome::Tundra, 0.65f, 0.76f, 0.87f, 1.00f, 1.00f, 1.00f, 1.00f, BlockType::Snow);
	AddBiomeBoundary(Biome::Tundra, 0.80f, 0.68f, 0.80f, 1.00f, 0.76f, 0.87f, 1.00f, BlockType::Snow);
	AddBiomeBoundary(Biome::Tundra, 0.90f, 0.60f, 0.73f, 1.00f, 0.68f, 0.80f, 1.00f, BlockType::Snow);
	AddBiomeBoundary(Biome::Tundra, 1.00f, 0.51f, 0.63f, 1.00f, 0.60f, 0.73f, 1.00f, BlockType::Snow);

	AddBiomeBoundary(Biome::AshLand, -0.35f, 0.18f, 0.12f, 0.12f, 0.23f, 0.17f, 0.18f, BlockType::Stone);
	AddBiomeBoundary(Biome::AshLand, 0.40f, 0.24f, 0.25f, 0.26f, 0.24f, 0.25f, 0.26f, BlockType::Stone);
	AddBiomeBoundary(Biome::AshLand, 1.00f, 0.25f, 0.25f, 0.25f, 0.17f, 0.15f, 0.10f, BlockType::Stone);

	// Biome regions	
	m_biomeRegions.SetFrequency(0.005f);
	m_biomeRegions.SetSeed(2);
}

BiomeManager::~BiomeManager()
{
	ClearBoundaryData();
	ClearTownData();
	ClearSafeZoneData();
}

// Clear data
void BiomeManager::ClearBoundaryData()
{
	for (int i = 0; i < static_cast<int>(Biome::NumBiomes); ++i)
	{
		for (unsigned int j = 0; j < m_vpBiomeHeightBoundaryList[i].size(); ++j)
		{
			delete m_vpBiomeHeightBoundaryList[i][j];
			m_vpBiomeHeightBoundaryList[i][j] = nullptr;
		}

		m_vpBiomeHeightBoundaryList[i].clear();
	}
}

void BiomeManager::ClearTownData()
{
	for (unsigned int i = 0; i < m_vpTownsList.size(); ++i)
	{
		delete m_vpTownsList[i];
		m_vpTownsList[i] = nullptr;
	}

	m_vpTownsList.clear();
}

void BiomeManager::ClearSafeZoneData()
{
	for (unsigned int i = 0; i < m_vpSafeZonesList.size(); ++i)
	{
		delete m_vpSafeZonesList[i];
		m_vpSafeZonesList[i] = nullptr;
	}

	m_vpSafeZonesList.clear();
}

// Add data
void BiomeManager::AddBiomeBoundary(Biome biome, float heightUpperBoundary, float red1, float green1, float blue1, float red2, float green2, float blue2, BlockType blockType)
{
	BiomeHeightBoundary* pNewBiomeHeightBoundary = new BiomeHeightBoundary();

	pNewBiomeHeightBoundary->m_heightUpperBoundary = heightUpperBoundary;
	pNewBiomeHeightBoundary->m_red1 = red1;
	pNewBiomeHeightBoundary->m_green1 = green1;
	pNewBiomeHeightBoundary->m_blue1 = blue1;
	pNewBiomeHeightBoundary->m_red2 = red2;
	pNewBiomeHeightBoundary->m_green2 = green2;
	pNewBiomeHeightBoundary->m_blue2 = blue2;
	pNewBiomeHeightBoundary->m_blockType = blockType;

	m_vpBiomeHeightBoundaryList[static_cast<int>(biome)].push_back(pNewBiomeHeightBoundary);
}

void BiomeManager::AddTown(glm::vec3 townCenter, float radius)
{
	ZoneData* pNewTown = new ZoneData();
	pNewTown->m_origin = townCenter;
	pNewTown->m_regionType = ZoneRegionType::Sphere;
	pNewTown->m_radius = radius;

	m_vpTownsList.push_back(pNewTown);
}

void BiomeManager::AddTown(glm::vec3 townCenter, float length, float height, float width)
{
	ZoneData* pNewTown = new ZoneData();
	pNewTown->m_origin = townCenter;
	pNewTown->m_regionType = ZoneRegionType::Cube;
	pNewTown->m_length = length;
	pNewTown->m_height = height;
	pNewTown->m_width = width;
	pNewTown->m_radius = length;

	Matrix4 transformMatrix;
	pNewTown->UpdatePlanes(transformMatrix);

	m_vpTownsList.push_back(pNewTown);
}

void BiomeManager::AddSafeZone(glm::vec3 safeZoneCenter, float radius)
{
	ZoneData* pNewSafeZone = new ZoneData();
	pNewSafeZone->m_origin = safeZoneCenter;
	pNewSafeZone->m_regionType = ZoneRegionType::Sphere;
	pNewSafeZone->m_radius = radius;

	m_vpSafeZonesList.push_back(pNewSafeZone);
}

void BiomeManager::AddSafeZone(glm::vec3 safeZoneCenter, float length, float height, float width)
{
	ZoneData* pNewSafeZone = new ZoneData();
	pNewSafeZone->m_origin = safeZoneCenter;
	pNewSafeZone->m_regionType = ZoneRegionType::Cube;
	pNewSafeZone->m_length = length;
	pNewSafeZone->m_height = height;
	pNewSafeZone->m_width = width;
	pNewSafeZone->m_radius = length;

	Matrix4 transformMatrix;
	pNewSafeZone->UpdatePlanes(transformMatrix);

	m_vpSafeZonesList.push_back(pNewSafeZone);
}

// Get biome
Biome BiomeManager::GetBiome(glm::vec3 position) const
{
	float regionValue = static_cast<float>(m_biomeRegions.GetValue(position.x, position.y, position.z));

	float ratio = 1.0f / (static_cast<int>(Biome::NumBiomes) - 1.0f);
	for (int i = 1; i < static_cast<int>(Biome::NumBiomes); ++i)
	{
		float maxValue = i * ratio;

		if (regionValue <= maxValue)
		{
			return static_cast<Biome>(i);
		}
	}

	return Biome::None;
}

// Town
bool BiomeManager::IsInTown(glm::vec3 position, ZoneData** pReturnTown)
{
	for (unsigned int i = 0; i < m_vpTownsList.size(); ++i)
	{
		ZoneData* pTown = m_vpTownsList[i];

		if (pTown->m_regionType == ZoneRegionType::Sphere)
		{
			glm::vec3 difference = pTown->m_origin - position;
			float distance = length(difference);

			if (distance < pTown->m_radius)
			{
				*pReturnTown = pTown;
				return true;
			}
		}
		else if (pTown->m_regionType == ZoneRegionType::Cube)
		{
			float distance;
			int outside = 0;
			int inside = 0;

			for (int j = 0; j < 6; ++j)
			{
				distance = pTown->m_planes[j].GetPointDistance(position - pTown->m_origin);

				if (distance < 0.0f)
				{
					// Outside...
					outside++;
				}
				else
				{
					// Inside...
					inside++;
				}
			}

			if (outside == 0)
			{
				*pReturnTown = pTown;
				return true;
			}
		}
	}

	*pReturnTown = nullptr;
	return false;
}

float BiomeManager::GetTownMultiplier(glm::vec3 position)
{
	ZoneData* pTown = nullptr;

	if (IsInTown(position, &pTown))
	{
		glm::vec3 toCenter = position - pTown->m_origin;
		toCenter.y = 0.0f;
		float lengthToCenter = length(toCenter);

		float ratio = lengthToCenter / (pTown->m_radius);

		if (ratio > 1.0f)
		{
			ratio = 1.0f;
		}
		if (ratio < 0.01f)
		{
			ratio = 0.01f;
		}

		return ratio;
	}

	return 1.0f;
}

// Safe zone
bool BiomeManager::IsInSafeZone(glm::vec3 position, ZoneData** pReturnSafeZone)
{
	for (size_t i = 0; i < m_vpSafeZonesList.size(); ++i)
	{
		ZoneData* pSafeZone = m_vpSafeZonesList[i];

		if (pSafeZone->m_regionType == ZoneRegionType::Sphere)
		{
			glm::vec3 difference = pSafeZone->m_origin - position;
			float distance = length(difference);

			if (distance < pSafeZone->m_radius)
			{
				*pReturnSafeZone = pSafeZone;
				return true;
			}
		}
		else if (pSafeZone->m_regionType == ZoneRegionType::Cube)
		{
			float distance;
			int outside = 0;
			int inside = 0;

			for (int j = 0; j < 6; ++j)
			{
				distance = pSafeZone->m_planes[j].GetPointDistance(position - pSafeZone->m_origin);

				if (distance < 0.0f)
				{
					// Outside...
					outside++;
				}
				else
				{
					// Inside...
					inside++;
				}
			}

			if (outside == 0)
			{
				*pReturnSafeZone = pSafeZone;
				return true;
			}
		}
	}

	*pReturnSafeZone = nullptr;
	return false;
}

// Check chunk and block type
void BiomeManager::GetChunkColorAndBlockType(float xPos, float yPos, float zPos, float noiseValue, float landscapeGradient, float* r, float* g, float* b, BlockType* blockType)
{
	int biomeIndex = static_cast<int>(GetBiome(glm::vec3(xPos, yPos, zPos)));

	float red1 = 0.0f;
	float green1 = 0.0f;
	float blue1 = 0.0f;
	float red2 = 1.0f;
	float green2 = 1.0f;
	float blue2 = 1.0f;

	bool foundBoundary = false;
	int boundaryCount = m_vpBiomeHeightBoundaryList[biomeIndex].size();

	for (int i = 0; i < boundaryCount && foundBoundary == false; i++)
	{
		if (noiseValue <= m_vpBiomeHeightBoundaryList[biomeIndex][i]->m_heightUpperBoundary)
		{
			red1 = m_vpBiomeHeightBoundaryList[biomeIndex][i]->m_red1;
			green1 = m_vpBiomeHeightBoundaryList[biomeIndex][i]->m_green1;
			blue1 = m_vpBiomeHeightBoundaryList[biomeIndex][i]->m_blue1;

			red2 = m_vpBiomeHeightBoundaryList[biomeIndex][i]->m_red2;
			green2 = m_vpBiomeHeightBoundaryList[biomeIndex][i]->m_green2;
			blue2 = m_vpBiomeHeightBoundaryList[biomeIndex][i]->m_blue2;

			*blockType = m_vpBiomeHeightBoundaryList[biomeIndex][i]->m_blockType;

			foundBoundary = true;
		}
	}

	if (foundBoundary == false)
	{
		red1 = m_vpBiomeHeightBoundaryList[biomeIndex][boundaryCount - 1]->m_red1;
		green1 = m_vpBiomeHeightBoundaryList[biomeIndex][boundaryCount - 1]->m_green1;
		blue1 = m_vpBiomeHeightBoundaryList[biomeIndex][boundaryCount - 1]->m_blue1;

		red2 = m_vpBiomeHeightBoundaryList[biomeIndex][boundaryCount - 1]->m_red2;
		green2 = m_vpBiomeHeightBoundaryList[biomeIndex][boundaryCount - 1]->m_green2;
		blue2 = m_vpBiomeHeightBoundaryList[biomeIndex][boundaryCount - 1]->m_blue2;

		*blockType = m_vpBiomeHeightBoundaryList[biomeIndex][boundaryCount - 1]->m_blockType;
	}

	*r = red1 + ((red2 - red1) * landscapeGradient);
	*g = green1 + ((green2 - green1) * landscapeGradient);
	*b = blue1 + ((blue2 - blue1) * landscapeGradient);
}

// Render
void BiomeManager::RenderDebug()
{
	RenderTownsDebug();
	RenderSafeZoneDebug();
}

void BiomeManager::RenderTownsDebug()
{
	for (size_t i = 0; i < m_vpTownsList.size(); ++i)
	{
		ZoneData* pTown = m_vpTownsList[i];

		if (pTown->m_regionType == ZoneRegionType::Sphere)
		{
			m_pRenderer->PushMatrix();

			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);

			m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);

			m_pRenderer->TranslateWorldMatrix(pTown->m_origin.x, pTown->m_origin.y, pTown->m_origin.z);

			m_pRenderer->DrawSphere(pTown->m_radius, 20, 20);

			m_pRenderer->PopMatrix();
		}
		else if (pTown->m_regionType == ZoneRegionType::Cube)
		{
			m_pRenderer->PushMatrix();

			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 0.25f);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);

			m_pRenderer->TranslateWorldMatrix(pTown->m_origin.x, pTown->m_origin.y, pTown->m_origin.z);

			float length = pTown->m_length;
			float width = pTown->m_width;
			float height = pTown->m_height;

			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

			m_pRenderer->ImmediateNormal(0.0f, 0.0f, -1.0f);
			m_pRenderer->ImmediateVertex(length, -height, -width);
			m_pRenderer->ImmediateVertex(-length, -height, -width);
			m_pRenderer->ImmediateVertex(-length, height, -width);
			m_pRenderer->ImmediateVertex(length, height, -width);

			m_pRenderer->ImmediateNormal(0.0f, 0.0f, 1.0f);
			m_pRenderer->ImmediateVertex(-length, -height, width);
			m_pRenderer->ImmediateVertex(length, -height, width);
			m_pRenderer->ImmediateVertex(length, height, width);
			m_pRenderer->ImmediateVertex(-length, height, width);

			m_pRenderer->ImmediateNormal(1.0f, 0.0f, 0.0f);
			m_pRenderer->ImmediateVertex(length, -height, width);
			m_pRenderer->ImmediateVertex(length, -height, -width);
			m_pRenderer->ImmediateVertex(length, height, -width);
			m_pRenderer->ImmediateVertex(length, height, width);

			m_pRenderer->ImmediateNormal(-1.0f, 0.0f, 0.0f);
			m_pRenderer->ImmediateVertex(-length, -height, -width);
			m_pRenderer->ImmediateVertex(-length, -height, width);
			m_pRenderer->ImmediateVertex(-length, height, width);
			m_pRenderer->ImmediateVertex(-length, height, -width);

			m_pRenderer->ImmediateNormal(0.0f, -1.0f, 0.0f);
			m_pRenderer->ImmediateVertex(-length, -height, -width);
			m_pRenderer->ImmediateVertex(length, -height, -width);
			m_pRenderer->ImmediateVertex(length, -height, width);
			m_pRenderer->ImmediateVertex(-length, -height, width);

			m_pRenderer->ImmediateNormal(0.0f, 1.0f, 0.0f);
			m_pRenderer->ImmediateVertex(length, height, -width);
			m_pRenderer->ImmediateVertex(-length, height, -width);
			m_pRenderer->ImmediateVertex(-length, height, width);
			m_pRenderer->ImmediateVertex(length, height, width);

			m_pRenderer->DisableImmediateMode();
			
			m_pRenderer->PopMatrix();
		}
	}

	m_pRenderer->SetCullMode(CullMode::BACK);
}

void BiomeManager::RenderSafeZoneDebug()
{
	for (unsigned int i = 0; i < m_vpSafeZonesList.size(); ++i)
	{
		ZoneData* pSafeZone = m_vpSafeZonesList[i];

		if (pSafeZone->m_regionType == ZoneRegionType::Sphere)
		{
			m_pRenderer->PushMatrix();

			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);

			m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);

			m_pRenderer->TranslateWorldMatrix(pSafeZone->m_origin.x, pSafeZone->m_origin.y, pSafeZone->m_origin.z);

			m_pRenderer->DrawSphere(pSafeZone->m_radius, 20, 20);
			
			m_pRenderer->PopMatrix();
		}
		else if (pSafeZone->m_regionType == ZoneRegionType::Cube)
		{
			m_pRenderer->PushMatrix();

			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 0.25f);
			m_pRenderer->SetCullMode(CullMode::NOCULL);
			m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);

			m_pRenderer->TranslateWorldMatrix(pSafeZone->m_origin.x, pSafeZone->m_origin.y, pSafeZone->m_origin.z);

			float length = pSafeZone->m_length;
			float width = pSafeZone->m_width;
			float height = pSafeZone->m_height;

			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
			
			m_pRenderer->ImmediateNormal(0.0f, 0.0f, -1.0f);
			m_pRenderer->ImmediateVertex(length, -height, -width);
			m_pRenderer->ImmediateVertex(-length, -height, -width);
			m_pRenderer->ImmediateVertex(-length, height, -width);
			m_pRenderer->ImmediateVertex(length, height, -width);

			m_pRenderer->ImmediateNormal(0.0f, 0.0f, 1.0f);
			m_pRenderer->ImmediateVertex(-length, -height, width);
			m_pRenderer->ImmediateVertex(length, -height, width);
			m_pRenderer->ImmediateVertex(length, height, width);
			m_pRenderer->ImmediateVertex(-length, height, width);

			m_pRenderer->ImmediateNormal(1.0f, 0.0f, 0.0f);
			m_pRenderer->ImmediateVertex(length, -height, width);
			m_pRenderer->ImmediateVertex(length, -height, -width);
			m_pRenderer->ImmediateVertex(length, height, -width);
			m_pRenderer->ImmediateVertex(length, height, width);

			m_pRenderer->ImmediateNormal(-1.0f, 0.0f, 0.0f);
			m_pRenderer->ImmediateVertex(-length, -height, -width);
			m_pRenderer->ImmediateVertex(-length, -height, width);
			m_pRenderer->ImmediateVertex(-length, height, width);
			m_pRenderer->ImmediateVertex(-length, height, -width);

			m_pRenderer->ImmediateNormal(0.0f, -1.0f, 0.0f);
			m_pRenderer->ImmediateVertex(-length, -height, -width);
			m_pRenderer->ImmediateVertex(length, -height, -width);
			m_pRenderer->ImmediateVertex(length, -height, width);
			m_pRenderer->ImmediateVertex(-length, -height, width);

			m_pRenderer->ImmediateNormal(0.0f, 1.0f, 0.0f);
			m_pRenderer->ImmediateVertex(length, height, -width);
			m_pRenderer->ImmediateVertex(-length, height, -width);
			m_pRenderer->ImmediateVertex(-length, height, width);
			m_pRenderer->ImmediateVertex(length, height, width);
			
			m_pRenderer->DisableImmediateMode();
			
			m_pRenderer->PopMatrix();
		}
	}

	m_pRenderer->SetCullMode(CullMode::BACK);
}

void ZoneData::UpdatePlanes(Matrix4 transformationMatrix)
{
	m_planes[0] = Plane3D(transformationMatrix * glm::vec3(-1.0f, 0.0f, 0.0f), transformationMatrix * glm::vec3(m_length, 0.0f, 0.0f));
	m_planes[1] = Plane3D(transformationMatrix * glm::vec3(1.0f, 0.0f, 0.0f), transformationMatrix * glm::vec3(-m_length, 0.0f, 0.0f));
	m_planes[2] = Plane3D(transformationMatrix * glm::vec3(0.0f, -1.0f, 0.0f), transformationMatrix * glm::vec3(0.0f, m_height, 0.0f));
	m_planes[3] = Plane3D(transformationMatrix * glm::vec3(0.0f, 1.0f, 0.0f), transformationMatrix * glm::vec3(0.0f, -m_height, 0.0f));
	m_planes[4] = Plane3D(transformationMatrix * glm::vec3(0.0f, 0.0f, -1.0f), transformationMatrix * glm::vec3(0.0f, 0.0f, m_width));
	m_planes[5] = Plane3D(transformationMatrix * glm::vec3(0.0f, 0.0f, 1.0f), transformationMatrix * glm::vec3(0.0f, 0.0f, -m_width));
}