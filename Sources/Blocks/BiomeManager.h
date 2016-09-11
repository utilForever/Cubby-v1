/*************************************************************************
> File Name: BiomeManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Biome manager.
> Created Time: 2016/07/09
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BIOME_MANAGER_H
#define CUBBY_BIOME_MANAGER_H

#include <libnoise/noise/noise.h>

#include <Renderer/Renderer.h>

#include "BlocksEnum.h"

enum class Biome
{
	None = 0,
	GrassLand,
	Desert,
	Tundra,
	AshLand,
	NumBiomes
};

struct BiomeHeightBoundary
{
	float m_heightUpperBoundary;
	float m_red1;
	float m_green1;
	float m_blue1;
	float m_red2;
	float m_green2;
	float m_blue2;
	BlockType m_blockType;
};

using BiomeHeightBoundaryList = std::vector<BiomeHeightBoundary*>;

enum class ZoneRegionType
{
	Sphere = 0,
	Cube,
};

struct ZoneData
{
	glm::vec3 m_origin;
	ZoneRegionType m_regionType;
	float m_radius;
	float m_length;
	float m_height;
	float m_width;
	Plane3D m_planes[6];

	void UpdatePlanes(Matrix4 transformationMatrix);
};

using ZoneDataList = std::vector<ZoneData*>;

class BiomeManager
{
public:
	// Constructor, Destructor
	BiomeManager(Renderer* pRenderer);
	~BiomeManager();

	// Clear data
	void ClearBoundaryData();
	void ClearTownData();
	void ClearSafeZoneData();

	// Add data
	void AddBiomeBoundary(Biome biome, float heightUpperBoundary, float red1, float green1, float blue1, float red2, float green2, float blue2, BlockType blockType);
	void AddTown(glm::vec3 townCenter, float radius);
	void AddTown(glm::vec3 townCenter, float length, float height, float width);
	void AddSafeZone(glm::vec3 safeZoneCenter, float radius);
	void AddSafeZone(glm::vec3 safeZoneCenter, float length, float height, float width);

	// Get biome
	Biome GetBiome(glm::vec3 position) const;

	// Town
	bool IsInTown(glm::vec3 position, ZoneData** pReturnTown);
	float GetTownMultiplier(glm::vec3 position);

	// Safe zone
	bool IsInSafeZone(glm::vec3 position, ZoneData** pReturnSafeZone);

	// Check chunk and block type
	void GetChunkColorAndBlockType(float xPos, float yPos, float zPos, float noiseValue, float landscapeGradient, float* r, float* g, float* b, BlockType* blockType);

	// Render
	void RenderDebug();
	void RenderTownsDebug();
	void RenderSafeZoneDebug();

private:
	Renderer* m_pRenderer;

	// Biome voronoi regions
	noise::module::Voronoi m_biomeRegions;

	// Biome boundaries
	BiomeHeightBoundaryList m_vpBiomeHeightBoundaryList[static_cast<int>(Biome::NumBiomes)];

	// Towns
	ZoneDataList m_vpTownsList;

	// Safe zones
	ZoneDataList m_vpSafeZonesList;
};

#endif