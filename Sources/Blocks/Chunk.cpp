/*************************************************************************
> File Name: Chunk.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A chunk is a collection of voxel blocks that are arranged together for
>    easier manipulation and management, when a single voxel in a chunk is
>    modified the whole chunk is refreshed. Chunks are rendered together
>    as a single vertex buffer and thus each chunk can be considered a single
>    draw call to render many voxels.
> Created Time: 2016/07/09
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <CubbyGame.h>
#include <CubbySettings.h>

#include <simplex/simplexnoise.h>

#include <Models/QubicleBinary.h>
#include <Utils/Random.h>

#include "BiomeManager.h"
#include "BlocksEnum.h"
#include "ChunkManager.h"
#include "Chunk.h"

// A chunk cube is double this render size, since we create from - to + for each axis.
const float Chunk::BLOCK_RENDER_SIZE = 0.5f;
// The chunk radius is an approximation of a sphere that will enclose totally our cuboid. (Used for culling)
const float Chunk::CHUNK_RADIUS = sqrt(((CHUNK_SIZE * BLOCK_RENDER_SIZE * 2.0f) * (CHUNK_SIZE * BLOCK_RENDER_SIZE * 2.0f)) * 2.0f) / 2.0f + ((BLOCK_RENDER_SIZE * 2.0f) * 2.0f);

// Constructor, Destructor
Chunk::Chunk(Renderer* pRenderer, ChunkManager* pChunkManager, CubbySettings* pVoxSettings) :
	m_pVoxSettings(pVoxSettings), m_pRenderer(pRenderer), m_pChunkManager(pChunkManager), m_pPlayer(nullptr)
{
	Initialize();
}

Chunk::~Chunk()
{
	Unload();

	delete m_color;
	delete m_blockType;
}

// Player pointer
void Chunk::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

// Scenery manager pointer
void Chunk::SetSceneryManager(SceneryManager* pSceneryManager)
{
	m_pSceneryManager = pSceneryManager;
}

// Biome manager
void Chunk::SetBiomeManager(BiomeManager* pBiomeManager)
{
	m_pBiomeManager = pBiomeManager;
}

// Initialize
void Chunk::Initialize()
{
	// Neighbors
	m_numNeighbors = 0;
	m_pXMinus = nullptr;
	m_pXPlus = nullptr;
	m_pYMinus = nullptr;
	m_pYPlus = nullptr;
	m_pZMinus = nullptr;
	m_pZPlus = nullptr;

	// Flag for change during a batch update
	m_chunkChangedDuringBatchUpdate = false;

	// Grid
	m_gridX = 0;
	m_gridY = 0;
	m_gridZ = 0;

	// Flags
	m_emptyChunk = false;
	m_surroundedChunk = false;
	m_xMinusFull = false;
	m_xPlusFull = false;
	m_yMinusFull = false;
	m_yPlusFull = false;
	m_zMinusFull = false;
	m_zPlusFull = false;

	// Setup and creation
	m_created = false;
	m_setup = false;
	m_isUnloading = false;
	m_rebuild = false;
	m_rebuildNeighbors = false;
	m_isRebuildingMesh = false;
	m_deleteCachedMesh = false;

	// Counters
	m_numRebuilds = 0;

	// Mesh
	m_pMesh = nullptr;
	m_pCachedMesh = nullptr;

	// Blocks data
	m_color = new unsigned int[CHUNK_SIZE_CUBED];
	m_blockType = new BlockType[CHUNK_SIZE_CUBED];
	for (int i = 0; i < CHUNK_SIZE_CUBED; ++i)
	{
		m_color[i] = 0;
		m_blockType[i] = BlockType::Default;
	}
}

// Creation and destruction
void Chunk::SetCreated(bool created)
{
	m_created = created;
}

bool Chunk::IsCreated() const
{
	return m_created;
}

void Chunk::Unload()
{
	m_isUnloading = true;

	if (m_pMesh != nullptr)
	{
		m_pRenderer->ClearMesh(m_pMesh);
		m_pMesh = nullptr;
	}

	if (m_setup == true)
	{
		// If we are already setup, when we unload, also tell our neighbors to update their flags
		Chunk* pChunkXMinus = m_pChunkManager->GetChunk(m_gridX - 1, m_gridY, m_gridZ);
		Chunk* pChunkXPlus = m_pChunkManager->GetChunk(m_gridX + 1, m_gridY, m_gridZ);
		Chunk* pChunkYMinus = m_pChunkManager->GetChunk(m_gridX, m_gridY - 1, m_gridZ);
		Chunk* pChunkYPlus = m_pChunkManager->GetChunk(m_gridX, m_gridY + 1, m_gridZ);
		Chunk* pChunkZMinus = m_pChunkManager->GetChunk(m_gridX, m_gridY, m_gridZ - 1);
		Chunk* pChunkZPlus = m_pChunkManager->GetChunk(m_gridX, m_gridY, m_gridZ + 1);

		if (pChunkXMinus != nullptr && pChunkXMinus->IsSetup() == true)
		{
			pChunkXMinus->UpdateSurroundedFlag();
		}
		if (pChunkXPlus != nullptr && pChunkXPlus->IsSetup() == true)
		{
			pChunkXPlus->UpdateSurroundedFlag();
		}
		if (pChunkYMinus != nullptr && pChunkYMinus->IsSetup() == true)
		{
			pChunkYMinus->UpdateSurroundedFlag();
		}
		if (pChunkYPlus != nullptr && pChunkYPlus->IsSetup() == true)
		{
			pChunkYPlus->UpdateSurroundedFlag();
		}
		if (pChunkZMinus != nullptr && pChunkZMinus->IsSetup() == true)
		{
			pChunkZMinus->UpdateSurroundedFlag();
		}
		if (pChunkZPlus != nullptr && pChunkZPlus->IsSetup() == true)
		{
			pChunkZPlus->UpdateSurroundedFlag();
		}
	}

	RemoveItems();
}

void Chunk::Setup()
{
	ChunkStorageLoader* pChunkStorage = m_pChunkManager->GetChunkStorage(m_gridX, m_gridY, m_gridZ, false);

	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int z = 0; z < CHUNK_SIZE; ++z)
		{
			float xPosition = m_position.x + x;
			float zPosition = m_position.z + z;

			Biome biome = CubbyGame::GetInstance()->GetBiomeManager()->GetBiome(glm::vec3(xPosition, 0.0f, zPosition));

			// Get the 
			float noise = octave_noise_2d(m_pVoxSettings->m_landscapeOctaves, m_pVoxSettings->m_landscapePersistence, m_pVoxSettings->m_landscapeScale, xPosition, zPosition);
			float noiseNormalized = ((noise + 1.0f) * 0.5f);
			float noiseHeight = noiseNormalized * CHUNK_SIZE;

			// Multiple by mountain ratio
			float mountainNoise = octave_noise_2d(m_pVoxSettings->m_mountainOctaves, m_pVoxSettings->m_mountainPersistence, m_pVoxSettings->m_mountainScale, xPosition, zPosition);
			float mountainNoiseNormalize = (mountainNoise + 1.0f) * 0.5f;
			float mountainMultiplier = m_pVoxSettings->m_mountainMultiplier * mountainNoiseNormalize;
			noiseHeight *= mountainMultiplier;

			// Smooth out for towns
			float townMultiplier = CubbyGame::GetInstance()->GetBiomeManager()->GetTownMultiplier(glm::vec3(xPosition, 0.0f, zPosition));
			noiseHeight *= townMultiplier;

			if (m_gridY < 0)
			{
				noiseHeight = CHUNK_SIZE;
			}

			for (int y = 0; y < CHUNK_SIZE; ++y)
			{
				float yPosition = m_position.y + y;

				if (pChunkStorage != nullptr && pChunkStorage->m_blockSet[x][y][z] == true)
				{
					SetColor(x, y, z, pChunkStorage->m_color[x][y][z]);
				}
				else
				{
					if (y + (m_gridY * CHUNK_SIZE) < noiseHeight)
					{
						float colorNoise = octave_noise_3d(4.0f, 0.3f, 0.005f, xPosition, yPosition, zPosition);
						float colorNoiseNormalized = ((colorNoise + 1.0f) * 0.5f);

						float red = 0.65f;
						float green = 0.80f;
						float blue = 0.00f;
						float alpha = 1.0f;
						BlockType blockType = BlockType::Default;

						m_pBiomeManager->GetChunkColorAndBlockType(xPosition, yPosition, zPosition, noise, colorNoiseNormalized, &red, &green, &blue, &blockType);

						SetColor(x, y, z, red, green, blue, alpha);
						SetBlockType(x, y, z, blockType);
					}
				}
			}

			// Tree generation
			if (m_gridY >= 0) // Only above ground
			{
				// Trees
				if (GetRandomNumber(0, 2000) >= 2000)
				{
					float minTreeHeight = 0.0f; 
					if (biome == Biome::GrassLand)
					{
						minTreeHeight = 0.5f;
					}
					else if (biome == Biome::Desert)
					{
						minTreeHeight = 0.0f;
					}
					else if (biome == Biome::AshLand)
					{
						minTreeHeight = 0.25f;
					}

					if (noiseNormalized >= minTreeHeight)
					{
						glm::vec3 treePos = glm::vec3(xPosition, noiseHeight, zPosition);

						if (biome == Biome::GrassLand)
						{
							m_pChunkManager->ImportQubicleBinary("Resources/gamedata/terrain/plains/smalltree.qb", treePos, QubicleImportDirection::Normal);
						}
						else if (biome == Biome::Desert)
						{
							m_pChunkManager->ImportQubicleBinary("Resources/gamedata/terrain/desert/cactus1.qb", treePos, QubicleImportDirection::Normal);
						}
						else if (biome == Biome::Tundra)
						{
							m_pChunkManager->ImportQubicleBinary("Resources/gamedata/terrain/tundra/tundra_tree1.qb", treePos, QubicleImportDirection::Normal);
						}
						else if (biome == Biome::AshLand)
						{
							m_pChunkManager->ImportQubicleBinary("Resources/gamedata/terrain/ashlands/ashtree1.qb", treePos, QubicleImportDirection::Normal);
						}
					}
				}

				// Scenery
				// TODO: Create scenery using poisson disk sampling and also using instance manager.
				//if ((GetRandomNumber(0, 1000) >= 995))
				//{
				//	if (noiseNormalized >= 0.5f)
				//	{
				//		vec3 pos = vec3(xPosition, noiseHeight, zPosition);
				//		m_pSceneryManager->AddSceneryObject("flower", "Resources/gamedata/terrain/plains/flower1.qb", pos, vec3(0.0f, 0.0f, 0.0f), QubicleImportDirection::Normal, QubicleImportDirection::Normal, 0.08f, GetRandomNumber(0, 360, 2));
				//	}
				//}
			}
		}
	}

	// Remove the chunk storage loader since we no longer need it
	if (pChunkStorage != nullptr)
	{
		m_pChunkManager->RemoveChunkStorageLoader(pChunkStorage);
	}

	m_setup = true;

	SetNeedsRebuild(true, true);
}

bool Chunk::IsSetup() const
{
	return m_setup;
}

bool Chunk::IsUnloading() const
{
	return m_isUnloading;
}

// Saving and loading
void Chunk::SaveChunk()
{
	// TODO: Write Chunk::SaveChunk()
}

void Chunk::LoadChunk()
{
	// TODO: Write Chunk::LoadChunk()
}

// Position
void Chunk::SetPosition(glm::vec3 pos)
{
	m_position = pos;
}

glm::vec3 Chunk::GetPosition() const
{
	return m_position;
}

// Neighbors
int Chunk::GetNumNeighbors() const
{
	return m_numNeighbors;
}

void Chunk::SetNumNeighbors(int neighbors)
{
	m_numNeighbors = neighbors;
}

Chunk* Chunk::GetXMinus() const
{
	return m_pXMinus;
}

Chunk* Chunk::GetXPlus() const
{
	return m_pXPlus;
}

Chunk* Chunk::GetYMinus() const
{
	return m_pYMinus;
}

Chunk* Chunk::GetYPlus() const
{
	return m_pYPlus;
}

Chunk* Chunk::GetZMinus() const
{
	return m_pZMinus;
}

Chunk* Chunk::GetZPlus() const
{
	return m_pZPlus;
}

void Chunk::SetXMinus(Chunk* pChunk)
{
	m_pXMinus = pChunk;
}

void Chunk::SetXPlus(Chunk* pChunk)
{
	m_pXPlus = pChunk;
}

void Chunk::SetYMinus(Chunk* pChunk)
{
	m_pYMinus = pChunk;
}

void Chunk::SetYPlus(Chunk* pChunk)
{
	m_pYPlus = pChunk;
}

void Chunk::SetZMinus(Chunk* pChunk)
{
	m_pZMinus = pChunk;
}

void Chunk::SetZPlus(Chunk* pChunk)
{
	m_pZPlus = pChunk;
}

// Grid
void Chunk::SetGrid(int x, int y, int z)
{
	m_gridX = x;
	m_gridY = y;
	m_gridZ = z;
}

int Chunk::GetGridX() const
{
	return m_gridX;
}

int Chunk::GetGridY() const
{
	return m_gridY;
}

int Chunk::GetGridZ() const
{
	return m_gridZ;
}

// Batch update
void Chunk::StartBatchUpdate()
{
	m_chunkChangedDuringBatchUpdate = false;
}

void Chunk::StopBatchUpdate()
{
	if (m_chunkChangedDuringBatchUpdate)
	{
		SetNeedsRebuild(true, true);
	}
}

// Active
bool Chunk::GetActive(int x, int y, int z) const
{
	unsigned color = m_color[x + y * CHUNK_SIZE + z * CHUNK_SIZE_SQUARED];
	unsigned int alpha = (color & 0xFF000000) >> 24;
	
	if (alpha == 0)
	{
		return false;
	}

	return true;
}

// Inside chunk
bool Chunk::IsInsideChunk(glm::vec3 pos) const
{
	if ((pos.x < m_position.x - BLOCK_RENDER_SIZE) || (pos.x - m_position.x > CHUNK_SIZE * (BLOCK_RENDER_SIZE*2.0f) - BLOCK_RENDER_SIZE))
		return false;

	if ((pos.y < m_position.y - BLOCK_RENDER_SIZE) || (pos.y - m_position.y > CHUNK_SIZE * (BLOCK_RENDER_SIZE*2.0f) - BLOCK_RENDER_SIZE))
		return false;

	if ((pos.z < m_position.z - BLOCK_RENDER_SIZE) || (pos.z - m_position.z > CHUNK_SIZE * (BLOCK_RENDER_SIZE*2.0f) - BLOCK_RENDER_SIZE))
		return false;

	return true;
}

// Items
void Chunk::AddItem(Item* pItem)
{
	m_itemMutexLock.lock();

	m_vpItemList.push_back(pItem);

	m_itemMutexLock.unlock();
}

void Chunk::RemoveItem(Item* pItem)
{
	m_itemMutexLock.lock();

	auto iter = find(m_vpItemList.begin(), m_vpItemList.end(), pItem);
	
	if (iter != m_vpItemList.end())
	{
		m_vpItemList.erase(iter);
	}
	
	m_itemMutexLock.unlock();
}

void Chunk::RemoveItems()
{
	// Delete the chunk items data
	m_itemMutexLock.lock();

	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		m_vpItemList[i]->SetChunk(nullptr);
		m_vpItemList[i]->SetErase(true);
	}
	
	m_vpItemList.clear();
	
	m_itemMutexLock.unlock();
}

// Block color
void Chunk::SetColor(int x, int y, int z, float r, float g, float b, float a, bool setBlockType)
{
	if (r > 1.0f)
	{
		r = 1.0f;
	}
	if (g > 1.0f)
	{
		g = 1.0f;
	}
	if (b > 1.0f)
	{
		b = 1.0f;
	}
	if (r < 0.0f)
	{
		r = 0.0f;
	}
	if (g < 0.0f)
	{
		g = 0.0f;
	}
	if (b < 0.0f)
	{
		b = 0.0f;
	}

	unsigned int alpha = static_cast<int>(a * 255) << 24;
	unsigned int blue = static_cast<int>(b * 255) << 16;
	unsigned int green = static_cast<int>(g * 255) << 8;
	unsigned int red = static_cast<int>(r * 255);

	unsigned int color = red + green + blue + alpha;
	SetColor(x, y, z, color, setBlockType);
}

void Chunk::GetColor(int x, int y, int z, float* r, float* g, float* b, float* a) const
{
	if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
	{
		return;
	}

	unsigned int color = m_color[x + y * CHUNK_SIZE + z * CHUNK_SIZE_SQUARED];
	unsigned int blue = (color & 0x00FF0000) >> 16;
	unsigned int green = (color & 0x0000FF00) >> 8;
	unsigned int red = (color & 0x000000FF);

	*r = red / 255.0f;
	*g = green / 255.0f;
	*b = blue / 255.0f;
	*a = 1.0f;
}

void Chunk::SetColor(int x, int y, int z, unsigned int color, bool setBlockType)
{
	if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
	{
		return;
	}

	bool isChanged = (m_color[x + y * CHUNK_SIZE + z * CHUNK_SIZE_SQUARED] == color) == false;

	if (isChanged)
	{
		m_chunkChangedDuringBatchUpdate = true;
	}

	m_color[x + y * CHUNK_SIZE + z * CHUNK_SIZE_SQUARED] = color;

	if (setBlockType)
	{
		unsigned int blockB = (color & 0x00FF0000) >> 16;
		unsigned int blockG = (color & 0x0000FF00) >> 8;
		unsigned int blockR = (color & 0x000000FF);
		m_blockType[x + y * CHUNK_SIZE + z * CHUNK_SIZE_SQUARED] = m_pChunkManager->SetBlockTypeBasedOnColor(blockR, blockG, blockB);
	}
}

unsigned int Chunk::GetColor(int x, int y, int z) const
{
	return m_color[x + y * CHUNK_SIZE + z * CHUNK_SIZE_SQUARED];
}

// Block type
BlockType Chunk::GetBlockType(int x, int y, int z) const
{
	return m_blockType[x + y * CHUNK_SIZE + z * CHUNK_SIZE_SQUARED];
}

void Chunk::SetBlockType(int x, int y, int z, BlockType blockType) const
{
	m_blockType[x + y * CHUNK_SIZE + z * CHUNK_SIZE_SQUARED] = blockType;
}

// Flags
bool Chunk::IsEmpty() const
{
	return m_emptyChunk;
}

bool Chunk::IsSurrounded() const
{
	return m_surroundedChunk;
}

void Chunk::UpdateWallFlags()
{
	// Figure out if we have any full walls(sides) and are a completely surrounded chunk
	int xMinus = 0;
	int xPlus = 0;
	int yMinus = 0;
	int yPlus = 0;
	int zMinus = 0;
	int zPlus = 0;

	for (int y = 0; y < CHUNK_SIZE; ++y)
	{
		for (int z = 0; z < CHUNK_SIZE; ++z)
		{
			if (GetActive(0, y, z) == true)
			{
				xMinus++;
			}

			if (GetActive(CHUNK_SIZE - 1, y, z) == true)
			{
				xPlus++;
			}
		}
	}

	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int z = 0; z < CHUNK_SIZE; ++z)
		{
			if (GetActive(x, 0, z) == true)
			{
				yMinus++;
			}

			if (GetActive(x, CHUNK_SIZE - 1, z) == true)
			{
				yPlus++;
			}
		}
	}

	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int y = 0; y < CHUNK_SIZE; ++y)
		{
			if (GetActive(x, y, 0) == true)
			{
				zMinus++;
			}

			if (GetActive(x, y, CHUNK_SIZE - 1) == true)
			{
				zPlus++;
			}
		}
	}

	// Reset the wall flags first
	m_xMinusFull = false;
	m_xPlusFull = false;
	m_yMinusFull = false;
	m_yPlusFull = false;
	m_zMinusFull = false;
	m_zPlusFull = false;

	// Set the flags to show if we have any sides completely full
	int wallsize = CHUNK_SIZE * CHUNK_SIZE;

	if (xMinus == wallsize)
	{
		m_xMinusFull = true;
	}
	if (xPlus == wallsize)
	{
		m_xPlusFull = true;
	}
	if (yMinus == wallsize)
	{
		m_yMinusFull = true;
	}
	if (yPlus == wallsize)
	{
		m_yPlusFull = true;
	}
	if (zMinus == wallsize)
	{
		m_zMinusFull = true;
	}
	if (zPlus == wallsize)
	{
		m_zPlusFull = true;
	}
}

bool Chunk::UpdateSurroundedFlag()
{
	if (m_pChunkManager == nullptr)
	{
		return false;
	}

	Chunk* pChunkXMinus = m_pChunkManager->GetChunk(m_gridX - 1, m_gridY, m_gridZ);
	Chunk* pChunkXPlus = m_pChunkManager->GetChunk(m_gridX + 1, m_gridY, m_gridZ);
	Chunk* pChunkYMinus = m_pChunkManager->GetChunk(m_gridX, m_gridY - 1, m_gridZ);
	Chunk* pChunkYPlus = m_pChunkManager->GetChunk(m_gridX, m_gridY + 1, m_gridZ);
	Chunk* pChunkZMinus = m_pChunkManager->GetChunk(m_gridX, m_gridY, m_gridZ - 1);
	Chunk* pChunkZPlus = m_pChunkManager->GetChunk(m_gridX, m_gridY, m_gridZ + 1);

	// Check our neighbor chunks
	if (pChunkXMinus != nullptr && pChunkXMinus->IsSetup() == true && pChunkXMinus->m_xPlusFull &&
		pChunkXPlus != nullptr && pChunkXPlus->IsSetup() == true && pChunkXPlus->m_xMinusFull &&
		pChunkYMinus != nullptr && pChunkYMinus->IsSetup() == true && pChunkYMinus->m_yPlusFull &&
		pChunkYPlus != nullptr && pChunkYPlus->IsSetup() == true && pChunkYPlus->m_yMinusFull &&
		pChunkZMinus != nullptr && pChunkZMinus->IsSetup() == true && pChunkZMinus->m_zPlusFull &&
		pChunkZPlus != nullptr && pChunkZPlus->IsSetup() == true && pChunkZPlus->m_zMinusFull)
	{
		m_surroundedChunk = true;
	}
	else
	{
		m_surroundedChunk = false;
	}

	return true;
}

void Chunk::UpdateEmptyFlag()
{
	// Figure out if we are a completely empty chunk
	int numVerts;
	int numTriangles;

	m_pRenderer->GetMeshInformation(&numVerts, &numTriangles, m_pMesh);

	if (numVerts == 0 && numTriangles == 0)
	{
		m_emptyChunk = true;
	}
	else
	{
		m_emptyChunk = false;
	}
}

// Create mesh
void Chunk::CreateMesh()
{
	if (m_pMesh == nullptr)
	{
		m_pMesh = m_pRenderer->CreateMesh(MeshType::Textured);
	}

	int* merged = new int[CHUNK_SIZE_CUBED];

	for (size_t j = 0; j < CHUNK_SIZE_CUBED; ++j)
	{
		merged[j] = static_cast<int>(MergedSide::None);
	}

	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	float a = 1.0f;

	for (int x = 0; x < CHUNK_SIZE; ++x)
	{
		for (int y = 0; y < CHUNK_SIZE; ++y)
		{
			for (int z = 0; z < CHUNK_SIZE; ++z)
			{
				if (GetActive(x, y, z) == true)
				{
					GetColor(x, y, z, &r, &g, &b, &a);

					a = 1.0f;

					glm::vec3 p1(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					glm::vec3 p2(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					glm::vec3 p3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					glm::vec3 p4(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					glm::vec3 p5(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					glm::vec3 p6(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					glm::vec3 p7(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					glm::vec3 p8(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

					glm::vec3 n1;
					unsigned int v1, v2, v3, v4;
		
					bool doXPositive = IsMergedXPositive(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE) == false;
					bool doXNegative = IsMergedXNegative(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE) == false;
					bool doYPositive = IsMergedYPositive(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE) == false;
					bool doYNegative = IsMergedYNegative(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE) == false;
					bool doZPositive = IsMergedZPositive(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE) == false;
					bool doZNegative = IsMergedZNegative(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE) == false;

					// Front
					if (doZPositive && (z == CHUNK_SIZE - 1 || z < CHUNK_SIZE - 1 && GetActive(x, y, z + 1) == false))
					{
						bool addSide = true;

						if (z == CHUNK_SIZE - 1)
						{
							Chunk* pChunk = m_pChunkManager->GetChunk(m_gridX, m_gridY, m_gridZ + 1);

							if (pChunk == nullptr || pChunk->IsSetup())
							{
								addSide = pChunk != nullptr && (pChunk->GetActive(x, y, 0) == false);
							}
						}

						if (addSide)
						{
							int endX = (x / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;
							int endY = (y / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;

							if (m_pChunkManager->GetFaceMerging())
							{
								UpdateMergedSide(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE, &p1, &p2, &p3, &p4, x, y, endX, endY, true, true, false, false);
							}

							n1 = glm::vec3(0.0f, 0.0f, 1.0f);
							v1 = m_pRenderer->AddVertexToMesh(p1, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, m_pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p2, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, m_pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p3, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, m_pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p4, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, m_pMesh);

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, m_pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, m_pMesh);
						}
					}

					p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

					// Back
					if (doZNegative && (z == 0 || z > 0 && GetActive(x, y, z - 1) == false))
					{
						bool addSide = true;

						if (z == 0)
						{
							Chunk* pChunk = m_pChunkManager->GetChunk(m_gridX, m_gridY, m_gridZ - 1);
							
							if (pChunk == nullptr || pChunk->IsSetup())
							{
								addSide = pChunk != nullptr && (pChunk->GetActive(x, y, CHUNK_SIZE - 1) == false);
							}
						}

						if (addSide)
						{
							int endX = (x / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;
							int endY = (y / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;

							if (m_pChunkManager->GetFaceMerging())
							{
								UpdateMergedSide(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE, &p6, &p5, &p8, &p7, x, y, endX, endY, false, true, false, false);
							}

							n1 = glm::vec3(0.0f, 0.0f, -1.0f);
							v1 = m_pRenderer->AddVertexToMesh(p5, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, m_pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p6, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, m_pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p7, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, m_pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p8, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, m_pMesh);

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, m_pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, m_pMesh);
						}
					}

					p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

					// Right
					if (doXPositive && (x == CHUNK_SIZE - 1 || x < CHUNK_SIZE - 1 && GetActive(x + 1, y, z) == false))
					{
						bool addSide = true;

						if (x == CHUNK_SIZE - 1)
						{
							Chunk* pChunk = m_pChunkManager->GetChunk(m_gridX + 1, m_gridY, m_gridZ);
							
							if (pChunk == nullptr || pChunk->IsSetup())
							{
								addSide = pChunk != nullptr && (pChunk->GetActive(0, y, z) == false);
							}
						}

						if (addSide)
						{
							int endZ = (z / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;
							int endY = (y / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;

							if (m_pChunkManager->GetFaceMerging())
							{
								UpdateMergedSide(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE, &p5, &p2, &p3, &p8, z, y, endZ, endY, true, false, true, false);
							}

							n1 = glm::vec3(1.0f, 0.0f, 0.0f);
							v1 = m_pRenderer->AddVertexToMesh(p2, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, m_pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p5, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, m_pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p8, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, m_pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p3, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, m_pMesh);

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, m_pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, m_pMesh);
						}
					}

					p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

					// Left
					if (doXNegative && (x == 0 || x > 0 && GetActive(x - 1, y, z) == false))
					{
						bool addSide = true;

						if (x == 0)
						{
							Chunk* pChunk = m_pChunkManager->GetChunk(m_gridX - 1, m_gridY, m_gridZ);
							
							if (pChunk == nullptr || pChunk->IsSetup())
							{
								addSide = pChunk != nullptr && (pChunk->GetActive(CHUNK_SIZE - 1, y, z) == false);
							}
						}

						if (addSide)
						{
							int endZ = (z / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;
							int endY = (y / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;

							if (m_pChunkManager->GetFaceMerging())
							{
								UpdateMergedSide(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE, &p6, &p1, &p4, &p7, z, y, endZ, endY, false, false, true, false);
							}

							n1 = glm::vec3(-1.0f, 0.0f, 0.0f);
							v1 = m_pRenderer->AddVertexToMesh(p6, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, m_pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p1, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, m_pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p4, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, m_pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p7, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, m_pMesh);

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, m_pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, m_pMesh);
						}
					}

					p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

					// Top
					if (doYPositive && (y == CHUNK_SIZE - 1 || y < CHUNK_SIZE - 1 && GetActive(x, y + 1, z) == false))
					{
						bool addSide = true;

						if (y == CHUNK_SIZE - 1)
						{
							Chunk* pChunk = m_pChunkManager->GetChunk(m_gridX, m_gridY + 1, m_gridZ);
							
							if (pChunk == nullptr || pChunk->IsSetup())
							{
								addSide = pChunk != nullptr && (pChunk->GetActive(x, 0, z) == false);
							}
						}

						if (addSide)
						{
							int endX = (x / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;
							int endZ = (z / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;

							if (m_pChunkManager->GetFaceMerging())
							{
								UpdateMergedSide(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE, &p7, &p8, &p3, &p4, x, z, endX, endZ, true, false, false, true);
							}

							n1 = glm::vec3(0.0f, 1.0f, 0.0f);
							v1 = m_pRenderer->AddVertexToMesh(p4, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, m_pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p3, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, m_pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p8, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, m_pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p7, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, m_pMesh);

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, m_pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, m_pMesh);
						}
					}

					p1 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p2 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p3 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p4 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z + BLOCK_RENDER_SIZE);
					p5 = glm::vec3(x + BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p6 = glm::vec3(x - BLOCK_RENDER_SIZE, y - BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p7 = glm::vec3(x - BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);
					p8 = glm::vec3(x + BLOCK_RENDER_SIZE, y + BLOCK_RENDER_SIZE, z - BLOCK_RENDER_SIZE);

					// Bottom
					if (doYNegative && (y == 0 || y > 0 && GetActive(x, y - 1, z) == false))
					{
						bool addSide = true;

						if (y == 0)
						{
							Chunk* pChunk = m_pChunkManager->GetChunk(m_gridX, m_gridY - 1, m_gridZ);
						
							if (pChunk == nullptr || pChunk->IsSetup())
							{
								addSide = pChunk != nullptr && (pChunk->GetActive(x, CHUNK_SIZE - 1, z) == false);
							}
						}

						if (addSide)
						{
							int endX = (x / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;
							int endZ = (z / CHUNK_SIZE) * CHUNK_SIZE + CHUNK_SIZE;

							if (m_pChunkManager->GetFaceMerging())
							{
								UpdateMergedSide(merged, x, y, z, CHUNK_SIZE, CHUNK_SIZE, &p6, &p5, &p2, &p1, x, z, endX, endZ, false, false, false, true);
							}

							n1 = glm::vec3(0.0f, -1.0f, 0.0f);
							v1 = m_pRenderer->AddVertexToMesh(p6, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 0.0f, m_pMesh);
							v2 = m_pRenderer->AddVertexToMesh(p5, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 0.0f, m_pMesh);
							v3 = m_pRenderer->AddVertexToMesh(p2, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(1.0f, 1.0f, m_pMesh);
							v4 = m_pRenderer->AddVertexToMesh(p1, n1, r, g, b, a, m_pMesh);
							m_pRenderer->AddTextureCoordinatesToMesh(0.0f, 1.0f, m_pMesh);

							m_pRenderer->AddTriangleToMesh(v1, v2, v3, m_pMesh);
							m_pRenderer->AddTriangleToMesh(v1, v3, v4, m_pMesh);
						}
					}
				}
			}
		}
	}

	// Delete the merged array
	delete[] merged;
}

void Chunk::CompleteMesh()
{
	m_pRenderer->FinishMesh(-1, m_pChunkManager->GetChunkMaterialID(), m_pMesh);

	UpdateEmptyFlag();

	m_isRebuildingMesh = false;
}

void Chunk::UpdateMergedSide(int* merged, int blockX, int blockY, int blockZ, int width, int height, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, glm::vec3* p4, int startX, int startY, int maxX, int maxY, bool positive, bool zFace, bool xFace, bool yFace) const
{
	bool doMore = true;

	unsigned int incrementX = 0;
	unsigned int incrementZ = 0;
	unsigned int incrementY = 0;

	int change = 1;

	if (zFace || yFace)
	{
		incrementX = 1;
		incrementY = 1;
	}
	if (xFace)
	{
		incrementZ = 1;
		incrementY = 1;
	}

	// 1st phase
	int incrementer = 1;

	while (doMore)
	{
		if (startX + incrementer >= maxX)
		{
			doMore = false;
		}
		else
		{
			bool doPhase1Merge = true;
			float r1, r2, g1, g2, b1, b2, a1, a2;
			GetColor(blockX, blockY, blockZ, &r1, &g1, &b1, &a1);
			GetColor(blockX + incrementX, blockY, blockZ + incrementZ, &r2, &g2, &b2, &a2);
		
			if (r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2)
			{
				// Don't do any phase 1 merging if we don't have the same color variation
				doMore = false;
			}
			else
			{
				if ((xFace && positive && blockX + incrementX + 1 == CHUNK_SIZE) ||
					(xFace && !positive && blockX + incrementX == 0) ||
					(yFace && positive && blockY + 1 == CHUNK_SIZE) ||
					(yFace && !positive && blockY == 0) ||
					(zFace && positive && blockZ + incrementZ + 1 == CHUNK_SIZE) ||
					(zFace && !positive && blockZ + incrementZ == 0))
				{
					doPhase1Merge = false;
					doMore = false;
				}
				// Don't do any phase 1 merging if we find an inactive block or already merged block in our path
				else if (xFace && positive && (blockX + incrementX + 1) < CHUNK_SIZE && GetActive(blockX + incrementX + 1, blockY, blockZ + incrementZ) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (xFace && !positive && (blockX + incrementX) > 0 && GetActive(blockX + incrementX - 1, blockY, blockZ + incrementZ) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (yFace && positive && (blockY + 1) < CHUNK_SIZE && GetActive(blockX + incrementX, blockY + 1, blockZ + incrementZ) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (yFace && !positive && blockY > 0 && GetActive(blockX + incrementX, blockY - 1, blockZ + incrementZ) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (zFace && positive && (blockZ + incrementZ + 1) < CHUNK_SIZE && GetActive(blockX + incrementX, blockY, blockZ + incrementZ + 1) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (zFace && !positive && (blockZ + incrementZ) > 0 && GetActive(blockX + incrementX, blockY, blockZ + incrementZ - 1) == true)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else if (GetActive(blockX + incrementX, blockY, blockZ + incrementZ) == false)
				{
					doPhase1Merge = false;
					doMore = false;
				}
				else
				{
					if (xFace)
					{
						doPhase1Merge = positive ? (IsMergedXPositive(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false) : (IsMergedXNegative(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false);
					}
					if (zFace)
					{
						doPhase1Merge = positive ? (IsMergedZPositive(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false) : (IsMergedZNegative(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false);
					}
					if (yFace)
					{
						doPhase1Merge = positive ? (IsMergedYPositive(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false) : (IsMergedYNegative(merged, blockX + incrementX, blockY, blockZ + incrementZ, width, height) == false);
					}
				}

				if (doPhase1Merge)
				{
					if (zFace || yFace)
					{
						(*p2).x += change * (BLOCK_RENDER_SIZE * 2.0f);
						(*p3).x += change * (BLOCK_RENDER_SIZE * 2.0f);
					}
					if (xFace)
					{
						(*p2).z += change * (BLOCK_RENDER_SIZE * 2.0f);
						(*p3).z += change * (BLOCK_RENDER_SIZE * 2.0f);
					}

					if (positive)
					{
						if (zFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= static_cast<int>(MergedSide::ZPositive);
						}
						if (xFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= static_cast<int>(MergedSide::XPositive);
						}
						if (yFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= static_cast<int>(MergedSide::YPositive);
						}
					}
					else
					{
						if (zFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= static_cast<int>(MergedSide::ZNegative);
						}
						if (xFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= static_cast<int>(MergedSide::XNegative);
						}
						if (yFace)
						{
							merged[(blockX + incrementX) + blockY * width + (blockZ + incrementZ) * width * height] |= static_cast<int>(MergedSide::YNegative);
						}
					}
				}
				else
				{
					doMore = false;
				}
			}
		}

		if (zFace || yFace)
		{
			incrementX += change;
		}

		if (xFace)
		{
			incrementZ += change;
		}

		incrementer += change;
	}

	// 2nd phase
	int loop = incrementer;
	incrementer = incrementY;

	doMore = true;

	while (doMore)
	{
		if (startY + incrementer >= maxY)
		{
			doMore = false;
		}
		else
		{
			for (int i = 0; i < loop - 1; ++i)
			{
				// Don't do any phase 2 merging is we have any inactive blocks or already merged blocks on the row
				if (zFace)
				{
					float r1, r2, g1, g2, b1, b2, a1, a2;
					GetColor(blockX, blockY, blockZ, &r1, &g1, &b1, &a1);
					GetColor(blockX + i, blockY + incrementY, blockZ, &r2, &g2, &b2, &a2);

					if (positive && (blockZ + 1) < CHUNK_SIZE && GetActive(blockX + i, blockY + incrementY, blockZ + 1) == true)
					{
						doMore = false;
					}
					else if (!positive && blockZ > 0 && GetActive(blockX + i, blockY + incrementY, blockZ - 1) == true)
					{
						doMore = false;
					}
					else if (GetActive(blockX + i, blockY + incrementY, blockZ) == false || (positive ? (IsMergedZPositive(merged, blockX + i, blockY + incrementY, blockZ, width, height) == true) : (IsMergedZNegative(merged, blockX + i, blockY + incrementY, blockZ, width, height) == true)))
					{
						// Failed active or already merged check
						doMore = false;
					}
					else if (r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2)
					{
						// Failed color check
						doMore = false;
					}
				}

				if (xFace)
				{
					float r1, r2, g1, g2, b1, b2, a1, a2;
					GetColor(blockX, blockY, blockZ, &r1, &g1, &b1, &a1);
					GetColor(blockX, blockY + incrementY, blockZ + i, &r2, &g2, &b2, &a2);

					if (positive && (blockX + 1) < CHUNK_SIZE && GetActive(blockX + 1, blockY + incrementY, blockZ + i) == true)
					{
						doMore = false;
					}
					else if (!positive && (blockX) > 0 && GetActive(blockX - 1, blockY + incrementY, blockZ + i) == true)
					{
						doMore = false;
					}
					else if (GetActive(blockX, blockY + incrementY, blockZ + i) == false || (positive ? (IsMergedXPositive(merged, blockX, blockY + incrementY, blockZ + i, width, height) == true) : (IsMergedXNegative(merged, blockX, blockY + incrementY, blockZ + i, width, height) == true)))
					{
						// Failed active or already merged check
						doMore = false;
					}
					else if (r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2)
					{
						// Failed color check
						doMore = false;
					}
				}

				if (yFace)
				{
					float r1, r2, g1, g2, b1, b2, a1, a2;
					GetColor(blockX, blockY, blockZ, &r1, &g1, &b1, &a1);
					GetColor(blockX + i, blockY, blockZ + incrementY, &r2, &g2, &b2, &a2);

					if (positive && (blockY + 1) < CHUNK_SIZE && GetActive(blockX + i, blockY + 1, blockZ + incrementY) == true)
					{
						doMore = false;
					}
					else if (!positive && blockY > 0 && GetActive(blockX + i, blockY - 1, blockZ + incrementY) == true)
					{
						doMore = false;
					}
					else if (GetActive(blockX + i, blockY, blockZ + incrementY) == false || (positive ? (IsMergedYPositive(merged, blockX + i, blockY, blockZ + incrementY, width, height) == true) : (IsMergedYNegative(merged, blockX + i, blockY, blockZ + incrementY, width, height) == true)))
					{
						// Failed active or already merged check
						doMore = false;
					}
					else if (r1 != r2 || g1 != g2 || b1 != b2 || a1 != a2)
					{
						// Failed color check
						doMore = false;
					}
				}
			}

			if (doMore == true)
			{
				if (zFace || xFace)
				{
					(*p3).y += change * (BLOCK_RENDER_SIZE * 2.0f);
					(*p4).y += change * (BLOCK_RENDER_SIZE * 2.0f);
				}
				if (yFace)
				{
					(*p3).z += change * (BLOCK_RENDER_SIZE * 2.0f);
					(*p4).z += change * (BLOCK_RENDER_SIZE * 2.0f);
				}

				for (int i = 0; i < loop - 1; ++i)
				{
					if (positive)
					{
						if (zFace)
						{
							merged[(blockX + i) + (blockY + incrementY) * width + blockZ * width * height] |= static_cast<int>(MergedSide::ZPositive);
						}
						if (xFace)
						{
							merged[blockX + (blockY + incrementY) * width + (blockZ + i) * width * height] |= static_cast<int>(MergedSide::XPositive);
						}
						if (yFace)
						{
							merged[(blockX + i) + blockY * width + (blockZ + incrementY) * width * height] |= static_cast<int>(MergedSide::YPositive);
						}
					}
					else
					{
						if (zFace)
						{
							merged[(blockX + i) + (blockY + incrementY) * width + blockZ * width * height] |= static_cast<int>(MergedSide::ZNegative);
						}
						if (xFace)
						{
							merged[blockX + (blockY + incrementY) * width + (blockZ + i) * width * height] |= static_cast<int>(MergedSide::XNegative);
						}
						if (yFace)
						{
							merged[(blockX + i) + blockY * width + (blockZ + incrementY) * width * height] |= static_cast<int>(MergedSide::YNegative);
						}
					}
				}
			}
		}

		incrementY += change;
		incrementer += change;
	}
}

// Rebuild
void Chunk::RebuildMesh()
{
	m_isRebuildingMesh = true;

	if (m_pMesh != nullptr)
	{
		m_pRenderer->ClearMesh(m_pMesh);
		m_pMesh = nullptr;
	}

	CreateMesh();

	// Update our wall flags, so that our neighbors can check if they are surrounded
	UpdateWallFlags();
	UpdateSurroundedFlag();

	Chunk* pChunkXMinus = m_pChunkManager->GetChunk(m_gridX - 1, m_gridY, m_gridZ);
	Chunk* pChunkXPlus = m_pChunkManager->GetChunk(m_gridX + 1, m_gridY, m_gridZ);
	Chunk* pChunkYMinus = m_pChunkManager->GetChunk(m_gridX, m_gridY - 1, m_gridZ);
	Chunk* pChunkYPlus = m_pChunkManager->GetChunk(m_gridX, m_gridY + 1, m_gridZ);
	Chunk* pChunkZMinus = m_pChunkManager->GetChunk(m_gridX, m_gridY, m_gridZ - 1);
	Chunk* pChunkZPlus = m_pChunkManager->GetChunk(m_gridX, m_gridY, m_gridZ + 1);

	if (pChunkXMinus != nullptr && pChunkXMinus->IsSetup() == true)
	{
		pChunkXMinus->UpdateSurroundedFlag();
	}
	if (pChunkXPlus != nullptr && pChunkXPlus->IsSetup() == true)
	{
		pChunkXPlus->UpdateSurroundedFlag();
	}
	if (pChunkYMinus != nullptr && pChunkYMinus->IsSetup() == true)
	{
		pChunkYMinus->UpdateSurroundedFlag();
	}
	if (pChunkYPlus != nullptr && pChunkYPlus->IsSetup() == true)
	{
		pChunkYPlus->UpdateSurroundedFlag();
	}
	if (pChunkZMinus != nullptr && pChunkZMinus->IsSetup() == true)
	{
		pChunkZMinus->UpdateSurroundedFlag();
	}
	if (pChunkZPlus != nullptr && pChunkZPlus->IsSetup() == true)
	{
		pChunkZPlus->UpdateSurroundedFlag();
	}

	// Rebuild neighbors
	if (m_rebuildNeighbors)
	{
		if (pChunkXMinus != nullptr && pChunkXMinus->IsSetup() == true)
		{
			pChunkXMinus->SetNeedsRebuild(true, false);
		}
		if (pChunkXPlus != nullptr && pChunkXPlus->IsSetup() == true)
		{
			pChunkXPlus->SetNeedsRebuild(true, false);
		}
		if (pChunkYMinus != nullptr && pChunkYMinus->IsSetup() == true)
		{
			pChunkYMinus->SetNeedsRebuild(true, false);
		}
		if (pChunkYPlus != nullptr && pChunkYPlus->IsSetup() == true)
		{
			pChunkYPlus->SetNeedsRebuild(true, false);
		}
		if (pChunkZMinus != nullptr && pChunkZMinus->IsSetup() == true)
		{
			pChunkZMinus->SetNeedsRebuild(true, false);
		}
		if (pChunkZPlus != nullptr && pChunkZPlus->IsSetup() == true)
		{
			pChunkZPlus->SetNeedsRebuild(true, false);
		}

		m_rebuildNeighbors = false;
	}

	m_numRebuilds++;
	m_rebuild = false;
}

void Chunk::SetNeedsRebuild(bool rebuild, bool rebuildNeighbors)
{
	m_rebuild = rebuild;
	m_rebuildNeighbors = rebuildNeighbors;
}

bool Chunk::NeedsRebuild() const
{
	return m_rebuild;
}

bool Chunk::IsRebuildingMesh() const
{
	return m_isRebuildingMesh;
}

void Chunk::SwitchToCachedMesh()
{
	m_pCachedMesh = m_pMesh;
	m_pMesh = nullptr;
}

void Chunk::UndoCachedMesh()
{
	m_deleteCachedMesh = true;
}

// Rendering
void Chunk::Render()
{
	TriangleMesh* pMeshToUse = m_pMesh;

	if (m_pCachedMesh != nullptr)
	{
		pMeshToUse = m_pCachedMesh;
	}

	if (pMeshToUse != nullptr)
	{
		m_pRenderer->PushMatrix();
	
		m_pRenderer->TranslateWorldMatrix(m_position.x, m_position.y, m_position.z);

		// Texture manipulation (for shadow rendering)
		Matrix4 worldMatrix;
		m_pRenderer->GetModelMatrix(&worldMatrix);

		m_pRenderer->PushTextureMatrix();
		m_pRenderer->MultiplyWorldMatrix(worldMatrix);
	
		m_pRenderer->MeshStaticBufferRender(pMeshToUse);

		m_pRenderer->PopTextureMatrix();

		m_pRenderer->PopMatrix();
	}

	if (m_deleteCachedMesh)
	{
		if (m_pCachedMesh != nullptr)
		{
			m_pRenderer->ClearMesh(m_pCachedMesh);
			m_pCachedMesh = nullptr;
		}

		m_deleteCachedMesh = false;
	}
}

void Chunk::RenderDebug() const
{
	float length = (CHUNK_SIZE * BLOCK_RENDER_SIZE) - 0.05f;
	float width = (CHUNK_SIZE * BLOCK_RENDER_SIZE) - 0.05f;
	float height = (CHUNK_SIZE * BLOCK_RENDER_SIZE) - 0.05f;

	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->SetCullMode(CullMode::NOCULL);
	m_pRenderer->SetLineWidth(1.0f);

	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(m_position.x, m_position.y, m_position.z);
	m_pRenderer->TranslateWorldMatrix(CHUNK_SIZE * BLOCK_RENDER_SIZE, CHUNK_SIZE * BLOCK_RENDER_SIZE, CHUNK_SIZE * BLOCK_RENDER_SIZE);
	m_pRenderer->TranslateWorldMatrix(-BLOCK_RENDER_SIZE, -BLOCK_RENDER_SIZE, -BLOCK_RENDER_SIZE);

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
	
	if (IsEmpty())
	{
		m_pRenderer->ImmediateColorAlpha(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (IsSurrounded())
	{
		m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 1.0f, 1.0f);
	}

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

	m_pRenderer->SetCullMode(CullMode::BACK);
}

void Chunk::Render2D(Camera* pCamera, unsigned int viewport, unsigned int font) const
{
	int winX, winY;
	glm::vec3 centerPos = m_position + glm::vec3(CHUNK_SIZE * BLOCK_RENDER_SIZE, CHUNK_SIZE * BLOCK_RENDER_SIZE, CHUNK_SIZE * BLOCK_RENDER_SIZE);
	centerPos += glm::vec3(-BLOCK_RENDER_SIZE, -BLOCK_RENDER_SIZE, -BLOCK_RENDER_SIZE);
	
	m_pRenderer->PushMatrix();
	
	m_pRenderer->SetProjectionMode(ProjectionMode::PERSPECTIVE, viewport);
	pCamera->Look();
	m_pRenderer->GetScreenCoordinatesFromWorldPosition(centerPos, &winX, &winY);
	
	m_pRenderer->PopMatrix();

	bool renderChunkInfo = true;
	if (renderChunkInfo)
	{
		char line1[64];
		sprintf(line1, "%i, %i, %i", m_gridX, m_gridY, m_gridZ);
		char line2[64];
		sprintf(line2, "Neighbors: %i", m_numNeighbors);
		char line3[64];
		sprintf(line3, "Empty: %s", m_emptyChunk ? "true" : "false");
		char line4[64];
		sprintf(line4, "Surrounded: %s", m_surroundedChunk ? "true" : "false");
		char line5[64];
		sprintf(line5, "Rebuilds: %i", m_numRebuilds);

		m_pRenderer->PushMatrix();

		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->SetProjectionMode(ProjectionMode::TWO_DIMENSION, viewport);
		m_pRenderer->SetLookAtCamera(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		m_pRenderer->RenderFreeTypeText(font, static_cast<float>(winX), static_cast<float>(winY), 1.0f, Color(1.0f, 1.0f, 1.0f), 1.0f, "%s", line1);
		m_pRenderer->RenderFreeTypeText(font, static_cast<float>(winX), static_cast<float>(winY) - 20.0f, 1.0f, Color(1.0f, 1.0f, 1.0f), 1.0f, "%s", line2);
		m_pRenderer->RenderFreeTypeText(font, static_cast<float>(winX), static_cast<float>(winY) - 40.0f, 1.0f, Color(1.0f, 1.0f, 1.0f), 1.0f, "%s", line3);
		m_pRenderer->RenderFreeTypeText(font, static_cast<float>(winX), static_cast<float>(winY) - 60.0f, 1.0f, Color(1.0f, 1.0f, 1.0f), 1.0f, "%s", line4);
		m_pRenderer->RenderFreeTypeText(font, static_cast<float>(winX), static_cast<float>(winY) - 80.0f, 1.0f, Color(1.0f, 1.0f, 1.0f), 1.0f, "%s", line5);
		
		m_pRenderer->PopMatrix();
	}
}

// < Operator (Used for chunk sorting, closest to camera)
bool Chunk::operator<(const Chunk& w) const
{
	int playerX = 0;
	int playerY = 0;
	int playerZ = 0;

	if (m_pPlayer != nullptr)
	{
		playerX = m_pPlayer->GetGridX();
		playerY = m_pPlayer->GetGridY();
		playerZ = m_pPlayer->GetGridZ();
	}

	int distance = abs(playerX - m_gridX) + abs(playerY - m_gridY) + abs(playerZ - m_gridZ);
	int wDistance = abs(playerX - w.GetGridX()) + abs(playerY - w.GetGridY()) + abs(playerZ - w.GetGridZ());

	return distance < wDistance;
}

bool Chunk::ClosestToCamera(const Chunk* lhs, const Chunk* rhs)
{
	if (*lhs < *rhs)
	{
		return true;
	}

	if (*rhs < *lhs)
	{
		return false;
	}

	return *lhs < *rhs;
}
