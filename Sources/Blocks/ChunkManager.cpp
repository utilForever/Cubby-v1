/*************************************************************************
> File Name: ChunkManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    ChunkManager handles all the chunk operations and management of the
>    individual chunks, deciding which chunks to load, unload, create, update
>    and also the rendering of the chunks.
> Created Time: 2016/07/09
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include <CubbyGame.h>
#include <CubbySettings.h>

#include <Maths/3DMaths.h>
#include <Models/QubicleBinaryManager.h>
#include <Renderer/Renderer.h>
#include <Utils/Random.h>

#include "BiomeManager.h"
#include "ChunkManager.h"

// Constructor, Destructor
ChunkManager::ChunkManager(Renderer* pRenderer, CubbySettings* pCubbySettings, QubicleBinaryManager* pQubicleBinaryManager) :
	m_pRenderer(pRenderer), m_pPlayer(nullptr), m_pCubbySettings(pCubbySettings), m_pQubicleBinaryManager(pQubicleBinaryManager)
{
	// Chunk material
	m_chunkMaterialID = -1;
	m_pRenderer->CreateMaterial(Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f), 64, &m_chunkMaterialID);

	// Create the block color to block type matching
	AddBlockColorBlockTypeMatching(59, 34, 4, BlockType::Wood);
	AddBlockColorBlockTypeMatching(82, 51, 4, BlockType::Wood);
	AddBlockColorBlockTypeMatching(87, 58, 0, BlockType::Wood);
	AddBlockColorBlockTypeMatching(25, 21, 14, BlockType::Wood);
	AddBlockColorBlockTypeMatching(30, 26, 18, BlockType::Wood);
	AddBlockColorBlockTypeMatching(132, 97, 36, BlockType::Wood);
	AddBlockColorBlockTypeMatching(55, 172, 3, BlockType::Leaf);
	AddBlockColorBlockTypeMatching(27, 82, 0, BlockType::Leaf);
	AddBlockColorBlockTypeMatching(61, 95, 24, BlockType::Leaf);
	AddBlockColorBlockTypeMatching(67, 104, 27, BlockType::Leaf);
	AddBlockColorBlockTypeMatching(121, 134, 0, BlockType::Leaf);
	AddBlockColorBlockTypeMatching(121, 134, 0, BlockType::Leaf);
	AddBlockColorBlockTypeMatching(113, 113, 1, BlockType::Leaf);
	AddBlockColorBlockTypeMatching(0, 182, 0, BlockType::Cactus);
	AddBlockColorBlockTypeMatching(34, 26, 48, BlockType::Wood); // TODO: Should be ash leaf, from ash trees
	AddBlockColorBlockTypeMatching(33, 26, 45, BlockType::Wood); // TODO: Should be ash leaf, from ash trees
	AddBlockColorBlockTypeMatching(255, 255, 255, BlockType::Snow);

	// Loader radius
	m_loaderRadius = m_pCubbySettings->m_loaderRadius;

	// Water
	m_waterHeight = 0.0f;

	// Update lock
	m_stepLockEnabled = false;
	m_updateStepLock = true;

	// Rendering modes
	m_wireframeRender = false;
	m_faceMerging = true;

	// Chunk counters
	m_numChunksLoaded = 0;
	m_numChunksRender = 0;

	// Threading
	m_updateThreadActive = true;
	m_updateThreadFinished = false;
	m_pUpdatingChunksThread = new tthread::thread(_UpdatingChunksThread, this);
}

ChunkManager::~ChunkManager()
{
	// Clear the block color to block type matching data
	for (unsigned int i = 0; i < m_vpBlockColorTypeMatchList.size(); ++i)
	{
		delete m_vpBlockColorTypeMatchList[i];
		m_vpBlockColorTypeMatchList[i] = nullptr;
	}
	m_vpBlockColorTypeMatchList.clear();

	m_stepLockEnabled = false;
	m_updateStepLock = true;
	m_updateThreadFlagLock.lock();
	m_updateThreadActive = false;
	m_updateThreadFlagLock.unlock();

	while (m_updateThreadFinished == false)
	{
#ifdef _WIN32
		Sleep(200);
#else
		usleep(200000);
#endif
	}
#ifdef _WIN32
	Sleep(200);
#else
	usleep(200000);
#endif
}

// Linkage
void ChunkManager::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

void ChunkManager::SetNPCManager(NPCManager* pNPCManager)
{
	m_pNPCManager = pNPCManager;
}

void ChunkManager::SetEnemyManager(EnemyManager* pEnemyManager)
{
	m_pEnemyManager = pEnemyManager;
}

void ChunkManager::SetBlockParticleManager(BlockParticleManager* pBlockParticleManager)
{
	m_pBlockParticleManager = pBlockParticleManager;
}

void ChunkManager::SetItemManager(ItemManager* pItemManager)
{
	m_pItemManager = pItemManager;
}

// Scenery manager pointer
void ChunkManager::SetSceneryManager(SceneryManager* pSceneryManager)
{
	m_pSceneryManager = pSceneryManager;
}

// Biome manager
void ChunkManager::SetBiomeManager(BiomeManager* pBiomeManager)
{
	m_pBiomeManager = pBiomeManager;
}

// Initial chunk creation
void ChunkManager::InitializeChunkCreation()
{
	// Create initial chunk
	CreateNewChunk(m_pPlayer->GetGridX(), m_pPlayer->GetGridY(), m_pPlayer->GetGridZ());
}

// Chunk rendering material
unsigned int ChunkManager::GetChunkMaterialID() const
{
	return m_chunkMaterialID;
}

int ChunkManager::GetNumChunksLoaded() const
{
	return m_numChunksLoaded;
}

int ChunkManager::GetNumChunksRender() const
{
	return m_numChunksRender;
}

// Loader radius
void ChunkManager::SetLoaderRadius(float radius)
{
	m_loaderRadius = radius;
}

float ChunkManager::GetLoaderRadius() const
{
	return m_loaderRadius;
}

// Step update
void ChunkManager::SetStepLockEnabled(bool enabled)
{
	m_stepLockEnabled = enabled;
}

void ChunkManager::StepNextUpdate()
{
	m_updateStepLock = false;
}

// Chunk Creation
void ChunkManager::CreateNewChunk(int x, int y, int z)
{
	ChunkCoordinateKeys coordKeys;
	coordKeys.x = x;
	coordKeys.y = y;
	coordKeys.z = z;

	// Create a new chunk at this grid position
	Chunk* pNewChunk = new Chunk(m_pRenderer, this, m_pCubbySettings);
	pNewChunk->SetPlayer(m_pPlayer);
	pNewChunk->SetSceneryManager(m_pSceneryManager);
	pNewChunk->SetBiomeManager(m_pBiomeManager);

	float xPos = x * (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f);
	float yPos = y * (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f);
	float zPos = z * (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f);

	pNewChunk->SetPosition(glm::vec3(xPos, yPos, zPos));
	pNewChunk->SetGrid(coordKeys.x, coordKeys.y, coordKeys.z);

	m_ChunkMapMutexLock.lock();
	m_chunksMap[coordKeys] = pNewChunk;
	m_ChunkMapMutexLock.unlock();

	pNewChunk->Setup();
	pNewChunk->SetNeedsRebuild(false, true);
	pNewChunk->RebuildMesh();
	pNewChunk->CompleteMesh();
	pNewChunk->SetCreated(true);

	UpdateChunkNeighbours(pNewChunk, x, y, z);
}

void ChunkManager::UpdateChunkNeighbours(Chunk* pChunk, int x, int y, int z)
{
	pChunk->SetNumNeighbors(0);

	Chunk* pChunkXMinus = GetChunk(x - 1, y, z);
	Chunk* pChunkXPlus = GetChunk(x + 1, y, z);
	Chunk* pChunkYMinus = GetChunk(x, y - 1, z);
	Chunk* pChunkYPlus = GetChunk(x, y + 1, z);
	Chunk* pChunkZMinus = GetChunk(x, y, z - 1);
	Chunk* pChunkZPlus = GetChunk(x, y, z + 1);

	if (pChunkXMinus)
	{
		pChunk->SetNumNeighbors(pChunk->GetNumNeighbors() + 1);
		pChunk->SetXMinus(pChunkXMinus);
		if (pChunkXMinus->GetXPlus() == nullptr)
		{
			pChunkXMinus->SetNumNeighbors(pChunkXMinus->GetNumNeighbors() + 1);
			pChunkXMinus->SetXPlus(pChunk);
		}
	}

	if (pChunkXPlus)
	{
		pChunk->SetNumNeighbors(pChunk->GetNumNeighbors() + 1);
		pChunk->SetXPlus(pChunkXPlus);
		if (pChunkXPlus->GetXMinus() == nullptr)
		{
			pChunkXPlus->SetNumNeighbors(pChunkXPlus->GetNumNeighbors() + 1);
			pChunkXPlus->SetXMinus(pChunk);
		}
	}

	if (pChunkYMinus)
	{
		pChunk->SetNumNeighbors(pChunk->GetNumNeighbors() + 1);
		pChunk->SetYMinus(pChunkYMinus);
		if (pChunkYMinus->GetYPlus() == nullptr)
		{
			pChunkYMinus->SetNumNeighbors(pChunkYMinus->GetNumNeighbors() + 1);
			pChunkYMinus->SetYPlus(pChunk);
		}
	}

	if (pChunkYPlus)
	{
		pChunk->SetNumNeighbors(pChunk->GetNumNeighbors() + 1);
		pChunk->SetYPlus(pChunkYPlus);
		if (pChunkYPlus->GetYMinus() == nullptr)
		{
			pChunkYPlus->SetNumNeighbors(pChunkYPlus->GetNumNeighbors() + 1);
			pChunkYPlus->SetYMinus(pChunk);
		}
	}

	if (pChunkZMinus)
	{
		pChunk->SetNumNeighbors(pChunk->GetNumNeighbors() + 1);
		pChunk->SetZMinus(pChunkZMinus);
		if (pChunkZMinus->GetZPlus() == nullptr)
		{
			pChunkZMinus->SetNumNeighbors(pChunkZMinus->GetNumNeighbors() + 1);
			pChunkZMinus->SetZPlus(pChunk);
		}
	}

	if (pChunkZPlus)
	{
		pChunk->SetNumNeighbors(pChunk->GetNumNeighbors() + 1);
		pChunk->SetZPlus(pChunkZPlus);
		if (pChunkZPlus->GetZMinus() == nullptr)
		{
			pChunkZPlus->SetNumNeighbors(pChunkZPlus->GetNumNeighbors() + 1);
			pChunkZPlus->SetZMinus(pChunk);
		}
	}
}

void ChunkManager::UnloadChunk(Chunk* pChunk)
{
	ChunkCoordinateKeys coordKeys;
	coordKeys.x = pChunk->GetGridX();
	coordKeys.y = pChunk->GetGridY();
	coordKeys.z = pChunk->GetGridZ();

	Chunk* pChunkXMinus = GetChunk(coordKeys.x - 1, coordKeys.y, coordKeys.z);
	Chunk* pChunkXPlus = GetChunk(coordKeys.x + 1, coordKeys.y, coordKeys.z);
	Chunk* pChunkYMinus = GetChunk(coordKeys.x, coordKeys.y - 1, coordKeys.z);
	Chunk* pChunkYPlus = GetChunk(coordKeys.x, coordKeys.y + 1, coordKeys.z);
	Chunk* pChunkZMinus = GetChunk(coordKeys.x, coordKeys.y, coordKeys.z - 1);
	Chunk* pChunkZPlus = GetChunk(coordKeys.x, coordKeys.y, coordKeys.z + 1);

	if (pChunkXMinus)
	{
		if (pChunkXMinus->GetXPlus())
		{
			pChunkXMinus->SetNumNeighbors(pChunkXMinus->GetNumNeighbors() - 1);
			pChunkXMinus->SetXPlus(nullptr);
		}
	}
	if (pChunkXPlus)
	{
		if (pChunkXPlus->GetXMinus())
		{
			pChunkXPlus->SetNumNeighbors(pChunkXPlus->GetNumNeighbors() - 1);
			pChunkXPlus->SetXMinus(nullptr);
		}
	}
	if (pChunkYMinus)
	{
		if (pChunkYMinus->GetYPlus())
		{
			pChunkYMinus->SetNumNeighbors(pChunkYMinus->GetNumNeighbors() - 1);
			pChunkYMinus->SetYPlus(nullptr);
		}
	}
	if (pChunkYPlus)
	{
		if (pChunkYPlus->GetYMinus())
		{
			pChunkYPlus->SetNumNeighbors(pChunkYPlus->GetNumNeighbors() - 1);
			pChunkYPlus->SetYMinus(nullptr);
		}
	}
	if (pChunkZMinus)
	{
		if (pChunkZMinus->GetZPlus())
		{
			pChunkZMinus->SetNumNeighbors(pChunkZMinus->GetNumNeighbors() - 1);
			pChunkZMinus->SetZPlus(nullptr);
		}
	}
	if (pChunkZPlus)
	{
		if (pChunkZPlus->GetZMinus())
		{
			pChunkZPlus->SetNumNeighbors(pChunkZPlus->GetNumNeighbors() - 1);
			pChunkZPlus->SetZMinus(nullptr);
		}
	}

	// Remove from map
	m_ChunkMapMutexLock.lock();

	auto it = m_chunksMap.find(coordKeys);
	
	if (it != m_chunksMap.end())
	{
		m_chunksMap.erase(coordKeys);
	}
	
	m_ChunkMapMutexLock.unlock();

	// Clear chunk linkage
	m_updateThreadFlagLock.lock();

	if (m_updateThreadActive)
	{
		if (m_pPlayer)
		{
			m_pPlayer->ClearChunkCacheForChunk(pChunk);
		}
		if (m_pNPCManager)
		{
			m_pNPCManager->ClearNPCChunkCacheForChunk(pChunk);
		}
		if (m_pEnemyManager)
		{
			m_pEnemyManager->ClearEnemyChunkCacheForChunk(pChunk);
		}
		if (m_pBlockParticleManager)
		{
			m_pBlockParticleManager->ClearParticleChunkCacheForChunk(pChunk);
		}
	}

	m_updateThreadFlagLock.unlock();

	// Unload and delete
	pChunk->Unload();
	delete pChunk;
}

// Getting chunk and positional information
void ChunkManager::GetGridFromPosition(glm::vec3 position, int* gridX, int* gridY, int* gridZ) const
{
	*gridX = static_cast<int>((position.x + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	*gridY = static_cast<int>((position.y + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	*gridZ = static_cast<int>((position.z + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);

	if (position.x <= -0.5f)
	{
		*gridX -= 1;
	}

	if (position.y <= -0.5f)
	{
		*gridY -= 1;
	}

	if (position.z <= -0.5f)
	{
		*gridZ -= 1;
	}
}

Chunk* ChunkManager::GetChunkFromPosition(float posX, float posY, float posZ)
{
	int gridX = static_cast<int>((posX + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridY = static_cast<int>((posY + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridZ = static_cast<int>((posZ + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);

	if (posX <= -0.5f)
	{
		gridX -= 1;
	}

	if (posY <= -0.5f)
	{
		gridY -= 1;
	}

	if (posZ <= -0.5f)
	{
		gridZ -= 1;
	}

	return GetChunk(gridX, gridY, gridZ);
}

Chunk* ChunkManager::GetChunk(int aX, int aY, int aZ)
{
	ChunkCoordinateKeys chunkKey;
	chunkKey.x = aX;
	chunkKey.y = aY;
	chunkKey.z = aZ;

	m_ChunkMapMutexLock.lock();

	auto it = m_chunksMap.find(chunkKey);
	
	if (it != m_chunksMap.end())
	{
		Chunk* pReturn = m_chunksMap[chunkKey];
		m_ChunkMapMutexLock.unlock();
		return pReturn;
	}

	m_ChunkMapMutexLock.unlock();

	return nullptr;
}

bool ChunkManager::FindClosestFloor(glm::vec3 position, glm::vec3* floorPosition)
{
	int blockX, blockY, blockZ;
	glm::vec3 blockPos;

	int iterations = 1;

	while (iterations < 100)
	{
		glm::vec3 testPos = position - glm::vec3(0.0f, Chunk::BLOCK_RENDER_SIZE, 0.0f) * static_cast<float>(iterations);

		Chunk* pChunk = nullptr;
		bool active = GetBlockActiveFrom3DPosition(testPos.x, testPos.y, testPos.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);

		if (pChunk != nullptr && pChunk->IsSetup() && pChunk->NeedsRebuild() == false && active == true)
		{
			*floorPosition = blockPos + glm::vec3(0.0f, Chunk::BLOCK_RENDER_SIZE, 0.0f);
			(*floorPosition).x = position.x;
			(*floorPosition).z = position.z;

			return true;
		}

		iterations++;
	}

	return false;
}

// Getting the active block state given a position and chunk information
bool ChunkManager::GetBlockActiveFrom3DPosition(float x, float y, float z, glm::vec3* blockPos, int* blockX, int* blockY, int* blockZ, Chunk** pChunk)
{
	if (*pChunk == nullptr)
	{
		*pChunk = GetChunkFromPosition(x, y, z);

		if (*pChunk == nullptr)
		{
			return false;
		}
	}

	*blockX = static_cast<int>((abs(x) + Chunk::BLOCK_RENDER_SIZE) / (Chunk::BLOCK_RENDER_SIZE * 2.0f));
	*blockY = static_cast<int>((abs(y) + Chunk::BLOCK_RENDER_SIZE) / (Chunk::BLOCK_RENDER_SIZE * 2.0f));
	*blockZ = static_cast<int>((abs(z) + Chunk::BLOCK_RENDER_SIZE) / (Chunk::BLOCK_RENDER_SIZE * 2.0f));

	*blockX = (*blockX) % Chunk::CHUNK_SIZE;
	*blockY = (*blockY) % Chunk::CHUNK_SIZE;
	*blockZ = (*blockZ) % Chunk::CHUNK_SIZE;

	(*blockPos).x = (*pChunk)->GetPosition().x + (*blockX) * (Chunk::BLOCK_RENDER_SIZE * 2.0f);
	(*blockPos).y = (*pChunk)->GetPosition().y + (*blockY) * (Chunk::BLOCK_RENDER_SIZE * 2.0f);
	(*blockPos).z = (*pChunk)->GetPosition().z + (*blockZ) * (Chunk::BLOCK_RENDER_SIZE * 2.0f);

	if (x < 0.0f)
	{
		if (*blockX == 0)
		{
			(*blockPos).x = (*pChunk)->GetPosition().x;
		}
		else
		{
			(*blockPos).x = (*pChunk)->GetPosition().x - (*blockX * (Chunk::BLOCK_RENDER_SIZE * 2.0f)) + (Chunk::CHUNK_SIZE * (Chunk::BLOCK_RENDER_SIZE * 2.0f));

			*blockX = Chunk::CHUNK_SIZE - *blockX;
		}
	}

	if (y < 0.0f)
	{
		if (*blockY == 0)
		{
			(*blockPos).y = (*pChunk)->GetPosition().y;
		}
		else
		{
			(*blockPos).y = (*pChunk)->GetPosition().y - (*blockY * (Chunk::BLOCK_RENDER_SIZE * 2.0f)) + (Chunk::CHUNK_SIZE * (Chunk::BLOCK_RENDER_SIZE * 2.0f));

			*blockY = Chunk::CHUNK_SIZE - *blockY;
		}
	}
	if (z < 0.0f)
	{
		if (*blockZ == 0)
		{
			(*blockPos).z = (*pChunk)->GetPosition().z;
		}
		else
		{
			(*blockPos).z = (*pChunk)->GetPosition().z - (*blockZ * (Chunk::BLOCK_RENDER_SIZE * 2.0f)) + (Chunk::CHUNK_SIZE * (Chunk::BLOCK_RENDER_SIZE * 2.0f));

			*blockZ = Chunk::CHUNK_SIZE - *blockZ;
		}
	}

	return (*pChunk)->GetActive(*blockX, *blockY, *blockZ);
}

void ChunkManager::GetBlockGridFrom3DPositionChunkStorage(float x, float y, float z, int* blockX, int* blockY, int* blockZ, ChunkStorageLoader* chunkStorage) const
{
	*blockX = static_cast<int>((abs(x) + Chunk::BLOCK_RENDER_SIZE) / (Chunk::BLOCK_RENDER_SIZE * 2.0f));
	*blockY = static_cast<int>((abs(y) + Chunk::BLOCK_RENDER_SIZE) / (Chunk::BLOCK_RENDER_SIZE * 2.0f));
	*blockZ = static_cast<int>((abs(z) + Chunk::BLOCK_RENDER_SIZE) / (Chunk::BLOCK_RENDER_SIZE * 2.0f));

	*blockX = *blockX % Chunk::CHUNK_SIZE;
	*blockY = *blockY % Chunk::CHUNK_SIZE;
	*blockZ = *blockZ % Chunk::CHUNK_SIZE;

	if (x < 0.0f)
	{
		if (*blockX != 0)
		{
			*blockX = Chunk::CHUNK_SIZE - *blockX;
		}
	}
	if (y < 0.0f)
	{
		if (*blockY != 0)
		{
			*blockY = Chunk::CHUNK_SIZE - *blockY;
		}
	}
	if (z < 0.0f)
	{
		if (*blockZ != 0)
		{
			*blockZ = Chunk::CHUNK_SIZE - *blockZ;
		}
	}
}

// Adding to chunk storage for parts of the world generation that are outside of loaded chunks
ChunkStorageLoader* ChunkManager::GetChunkStorage(int x, int y, int z, bool createIfNotExist)
{
	m_chunkStorageListLock.lock();

	for (size_t i = 0; i < m_vpChunkStorageList.size(); ++i)
	{
		if (m_vpChunkStorageList[i]->m_gridX == x && m_vpChunkStorageList[i]->m_gridY == y && m_vpChunkStorageList[i]->m_gridZ == z)
		{
			m_chunkStorageListLock.unlock();

			// Found and existing chunk storage, return it
			return m_vpChunkStorageList[i];
		}
	}

	m_chunkStorageListLock.unlock();

	// No storage found, create a new one
	if (createIfNotExist)
	{
		ChunkStorageLoader* pNewStorage = new ChunkStorageLoader(x, y, z);
		pNewStorage->m_gridX = x;
		pNewStorage->m_gridY = y;
		pNewStorage->m_gridZ = z;

		m_chunkStorageListLock.lock();

		m_vpChunkStorageList.push_back(pNewStorage);
		
		m_chunkStorageListLock.unlock();

		return pNewStorage;
	}

	return nullptr;
}

void ChunkManager::RemoveChunkStorageLoader(ChunkStorageLoader* pChunkStorage)
{
	m_chunkStorageListLock.lock();

	auto iter = find(m_vpChunkStorageList.begin(), m_vpChunkStorageList.end(), pChunkStorage);
	if (iter != m_vpChunkStorageList.end())
	{
		m_vpChunkStorageList.erase(iter);
	}

	m_chunkStorageListLock.unlock();

	delete pChunkStorage;
	pChunkStorage = nullptr;
}

// Block color to block type matching
void ChunkManager::AddBlockColorBlockTypeMatching(int r, int g, int b, BlockType blockType)
{
	BlockColorTypeMatch* pMatch = new BlockColorTypeMatch();
	pMatch->m_red = r;
	pMatch->m_green = g;
	pMatch->m_blue = b;
	pMatch->m_blockType = blockType;

	m_vpBlockColorTypeMatchList.push_back(pMatch);
}

bool ChunkManager::CheckBlockColor(int r, int g, int b, int rCheck, int gCheck, int bCheck) const
{
	if (r != rCheck)
	{
		return false;
	}

	if (g != gCheck)
	{
		return false;
	}

	if (b != bCheck)
	{
		return false;
	}

	return true;
}

BlockType ChunkManager::SetBlockTypeBasedOnColor(int r, int g, int b)
{
	for (int i = 0; i < m_vpBlockColorTypeMatchList.size(); ++i)
	{
		BlockColorTypeMatch* pMatch = m_vpBlockColorTypeMatchList[i];

		if (CheckBlockColor(pMatch->m_red, pMatch->m_green, pMatch->m_blue, r, g, b))
		{
			return pMatch->m_blockType;
		}
	}

	return BlockType::Default;
}

// Importing into the world chunks
void ChunkManager::ImportQubicleBinaryMatrix(QubicleMatrix* pMatrix, glm::vec3 position, QubicleImportDirection direction)
{
	bool mirrorX = false;
	bool mirrorY = false;
	bool mirrorZ = false;
	bool flipXZ = false;
	bool flipXY = false;
	bool flipYZ = false;

	switch (direction)
	{
	case QubicleImportDirection::Normal: {  } break;
	case QubicleImportDirection::MirrorX: { mirrorX = true; } break;
	case QubicleImportDirection::MirrorY: { mirrorY = true; } break;
	case QubicleImportDirection::MirrorZ: { mirrorZ = true; } break;
	case QubicleImportDirection::RotateY90: { mirrorX = true; flipXZ = true; } break;
	case QubicleImportDirection::RotateY180: { mirrorX = true; mirrorZ = true; } break;
	case QubicleImportDirection::RotateY270: { mirrorZ = true; flipXZ = true; } break;
	case QubicleImportDirection::RotateX90: { mirrorZ = true; flipYZ = true; } break;
	case QubicleImportDirection::RotateX180: { mirrorZ = true; mirrorY = true; } break;
	case QubicleImportDirection::RotateX270: { mirrorY = true; flipYZ = true; } break;
	case QubicleImportDirection::RotateZ90: { mirrorY = true; flipXY = true; } break;
	case QubicleImportDirection::RotateZ180: { mirrorX = true; mirrorY = true; } break;
	case QubicleImportDirection::RotateZ270: { mirrorX = true; flipXY = true; } break;
	}

	ChunkList vChunkBatchUpdateList;

	unsigned int xValueToUse = pMatrix->m_matrixSizeX;
	unsigned int yValueToUse = pMatrix->m_matrixSizeY;
	unsigned int zValueToUse = pMatrix->m_matrixSizeZ;

	if (flipXZ)
	{
		xValueToUse = pMatrix->m_matrixSizeZ;
		zValueToUse = pMatrix->m_matrixSizeX;
	}
	
	if (flipXY)
	{
		xValueToUse = pMatrix->m_matrixSizeY;
		yValueToUse = pMatrix->m_matrixSizeX;
	}
	
	if (flipYZ)
	{
		yValueToUse = pMatrix->m_matrixSizeZ;
		zValueToUse = pMatrix->m_matrixSizeY;
	}

	int xPosition = 0;
	
	if (mirrorX)
	{
		xPosition = xValueToUse - 1;
	}

	for (size_t x = 0; x < xValueToUse; ++x)
	{
		int yPosition = 0;

		if (mirrorY)
		{
			yPosition = yValueToUse - 1;
		}

		for (size_t y = 0; y < yValueToUse; ++y)
		{
			int zPosition = 0;

			if (mirrorZ)
			{
				zPosition = zValueToUse - 1;
			}

			for (size_t z = 0; z < zValueToUse; ++z)
			{
				int xPositionModified = xPosition;
				int yPositionModified = yPosition;
				int zPositionModified = zPosition;

				if (flipXZ)
				{
					xPositionModified = zPosition;
					zPositionModified = xPosition;
				}

				if (flipXY)
				{
					xPositionModified = yPosition;
					yPositionModified = xPosition;
				}

				if (flipYZ)
				{
					yPositionModified = zPosition;
					zPositionModified = yPosition;
				}

				if (pMatrix->GetActive(xPositionModified, yPositionModified, zPositionModified) == true)
				{
					unsigned int color = pMatrix->GetColorCompact(xPositionModified, yPositionModified, zPositionModified);

					glm::vec3 blockPos = position - glm::vec3((xValueToUse + 0.05f) * 0.5f, 0.0f, (zValueToUse + 0.05f) * 0.5f) + glm::vec3(x * Chunk::BLOCK_RENDER_SIZE * 2.0f, y * Chunk::BLOCK_RENDER_SIZE * 2.0f, z * Chunk::BLOCK_RENDER_SIZE * 2.0f);

					glm::vec3 blockPosition;
					int blockX, blockY, blockZ;
					Chunk* pChunk = nullptr;

					GetBlockActiveFrom3DPosition(blockPos.x, blockPos.y, blockPos.z, &blockPosition, &blockX, &blockY, &blockZ, &pChunk);

					if (pChunk != nullptr)
					{
						pChunk->SetColor(blockX, blockY, blockZ, color, true);

						// Add to batch update list (no duplicates)
						bool found = false;

						for (int i = 0; i < vChunkBatchUpdateList.size() && found == false; ++i)
						{
							if (vChunkBatchUpdateList[i] == pChunk)
							{
								found = true;
							}
						}

						if (found == false)
						{
							vChunkBatchUpdateList.push_back(pChunk);
							pChunk->StartBatchUpdate();
						}
					}
					else
					{
						// Add to the chunk storage
						int gridX;
						int gridY;
						int gridZ;

						GetGridFromPosition(blockPos, &gridX, &gridY, &gridZ);

						ChunkStorageLoader* pStorage = GetChunkStorage(gridX, gridY, gridZ, true);

						if (pStorage != nullptr)
						{
							GetBlockGridFrom3DPositionChunkStorage(blockPos.x, blockPos.y, blockPos.z, &blockX, &blockY, &blockZ, pStorage);

							pStorage->SetBlockColor(blockX, blockY, blockZ, color);
						}
					}
				}

				if (mirrorZ)
				{
					zPosition--;
				}
				else
				{
					zPosition++;
				}
			}

			if (mirrorY)
			{
				yPosition--;
			}
			else
			{
				yPosition++;
			}
		}

		if (mirrorX)
		{
			xPosition--;
		}
		else
		{
			xPosition++;
		}
	}

	for (int i = 0; i < vChunkBatchUpdateList.size(); ++i)
	{
		vChunkBatchUpdateList[i]->StopBatchUpdate();
	}

	vChunkBatchUpdateList.clear();
}

QubicleBinary* ChunkManager::ImportQubicleBinary(QubicleBinary* qubicleBinaryFile, glm::vec3 position, QubicleImportDirection direction)
{
	int numMatrices = qubicleBinaryFile->GetNumMatrices();

	for (int i = 0; i < numMatrices; ++i)
	{
		QubicleMatrix* pMatrix = qubicleBinaryFile->GetQubicleMatrix(i);

		ImportQubicleBinaryMatrix(pMatrix, position, direction);
	}

	return qubicleBinaryFile;
}

QubicleBinary* ChunkManager::ImportQubicleBinary(const char* fileName, glm::vec3 position, QubicleImportDirection direction)
{
	QubicleBinary* qubicleBinaryFile = m_pQubicleBinaryManager->GetQubicleBinaryFile(fileName, true);
	
	if (qubicleBinaryFile != nullptr)
	{
		return ImportQubicleBinary(qubicleBinaryFile, position, direction);
	}

	return nullptr;
}

// Explosions
void ChunkManager::CreateBlockDestroyParticleEffect(float r, float g, float b, float a, glm::vec3 blockPosition) const
{
	for (int i = 0; i < 8; ++i)
	{
		float size = Chunk::BLOCK_RENDER_SIZE * 0.5f;
		float scale = 0.3f + (GetRandomNumber(-1, 1, 4) * 0.2f);
		glm::vec3 addition;
		
		if (i == 0) addition = glm::vec3(-size, size, -size);
		if (i == 1) addition = glm::vec3(size, size, -size);
		if (i == 2) addition = glm::vec3(-size, size, size);
		if (i == 3) addition = glm::vec3(size, size, size);
		if (i == 4) addition = glm::vec3(-size, -size, -size);
		if (i == 5) addition = glm::vec3(size, -size, -size);
		if (i == 6) addition = glm::vec3(-size, -size, size);
		if (i == 7) addition = glm::vec3(size, -size, size);

		float lifeTime = 6.5f + GetRandomNumber(-1, 1, 1) * 0.75f;

		glm::vec3 gravityDir = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 pointOrigin = glm::vec3(0.0f, 0.0f, 0.0f);

		m_pBlockParticleManager->CreateBlockParticle(blockPosition + addition, blockPosition + addition, gravityDir, 2.5f, pointOrigin, scale, 0.0f, scale, 0.0f,
			r, g, b, a, 0.0f, 0.0f, 0.0f, 0.0f, r, g, b, a, 0.0f, 0.0f, 0.0f, 0.0f, lifeTime, 0.0f, 0.0f, 0.0f, glm::vec3(0.0f, 7.0f, 0.0f),
			glm::vec3(3.0f, 2.0f, 3.0f), glm::vec3(GetRandomNumber(-360, 360, 2), GetRandomNumber(-360, 360, 2), GetRandomNumber(-360, 360, 2)),
			glm::vec3(180.0f, 180.0f, 180.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true, glm::vec3(0.0f, 0.0f, 0.0f), true, false, false, false, nullptr);
	}
}

void ChunkManager::ExplodeSphere(glm::vec3 position, float radius)
{
	float startX = position.x - radius;
	float startY = position.y - radius;
	float startZ = position.z - radius;
	float endX = position.x + radius;
	float endY = position.y + radius;
	float endZ = position.z + radius;

	ChunkList vChunkBatchUpdateList;

	for (float x = startX; x < endX; x += Chunk::BLOCK_RENDER_SIZE)
	{
		for (float y = startY; y < endY; y += Chunk::BLOCK_RENDER_SIZE)
		{
			for (float z = startZ; z < endZ; z += Chunk::BLOCK_RENDER_SIZE)
			{
				glm::vec3 blockPosition;
				int blockX, blockY, blockZ;
				Chunk* pChunk = nullptr;

				bool active = GetBlockActiveFrom3DPosition(x, y, z, &blockPosition, &blockX, &blockY, &blockZ, &pChunk);

				float distance = length(blockPosition - position);

				if (pChunk != nullptr)
				{
					if (distance <= radius)
					{
						if (active)
						{
							float r;
							float g;
							float b;
							float a;

							// Store the color for particle effect later
							pChunk->GetColor(blockX, blockY, blockZ, &r, &g, &b, &a);

							// Remove the block from being active
							pChunk->SetColor(blockX, blockY, blockZ, 0);

							// Create particle effect
							if (GetRandomNumber(0, 100, 2) > 75.0f)
							{
								CreateBlockDestroyParticleEffect(r, g, b, a, blockPosition);
							}

							// Create the collectible block item
							if (GetRandomNumber(0, 100, 2) > 75.0f)
							{
								BlockType blockType = pChunk->GetBlockType(blockX, blockY, blockZ);
								CreateCollectibleBlock(blockType, blockPosition);
							}

							// Add to batch update list (no duplicates)
							bool isFound = false;
							for (int i = 0; i < vChunkBatchUpdateList.size() && isFound == false; ++i)
							{
								if (vChunkBatchUpdateList[i] == pChunk)
								{
									isFound = true;
								}
							}

							if (isFound == false)
							{
								vChunkBatchUpdateList.push_back(pChunk);
								pChunk->StartBatchUpdate();
							}
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < vChunkBatchUpdateList.size(); ++i)
	{
		vChunkBatchUpdateList[i]->StopBatchUpdate();
	}

	vChunkBatchUpdateList.clear();
}

// Collectible block objects
void ChunkManager::CreateCollectibleBlock(BlockType blockType, glm::vec3 blockPos)
{
	Item* pItem;
	ItemSubSpawnData* pItemSubSpawnData = m_pItemManager->GetItemSubSpawnData(blockType);
	
	if (pItemSubSpawnData != nullptr)
	{
		pItem = m_pItemManager->CreateItem(blockPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), pItemSubSpawnData->m_spawnedItemFileName.c_str(), pItemSubSpawnData->m_spawnedItem, pItemSubSpawnData->m_spawnedItemTitle.c_str(), pItemSubSpawnData->m_interactable, pItemSubSpawnData->m_collectible, pItemSubSpawnData->m_scale);

		if (pItem != nullptr)
		{
			float radius = 1.5f;
			float angle = DegreeToRadian(GetRandomNumber(0, 360, 2));
			glm::vec3 itemPosition = blockPos + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);

			pItem->SetGravityDirection(glm::vec3(0.0f, -1.0f, 0.0f));
			glm::vec3 velocity = itemPosition - blockPos;
			pItem->SetVelocity(normalize(velocity) * GetRandomNumber(0, 1, 2) + glm::vec3(GetRandomNumber(-1, 1, 2), 1.0f + GetRandomNumber(2, 5, 2), GetRandomNumber(-1, 1, 2)));
			pItem->SetRotation(glm::vec3(0.0f, GetRandomNumber(0, 360, 2), 0.0f));
			pItem->SetAngularVelocity(glm::vec3(0.0f, 90.0f, 0.0f));

			pItem->SetDroppedItem(pItemSubSpawnData->m_droppedItemFileName.c_str(), pItemSubSpawnData->m_droppedItemTextureFileName.c_str(), pItemSubSpawnData->m_droppedItemInventoryType, pItemSubSpawnData->m_droppedItemItem, pItemSubSpawnData->m_droppedItemStatus, pItemSubSpawnData->m_droppedItemEquipSlot, pItemSubSpawnData->m_droppedItemQuality, pItemSubSpawnData->m_droppedItemLeft, pItemSubSpawnData->m_droppedItemRight, pItemSubSpawnData->m_droppedItemTitle.c_str(), pItemSubSpawnData->m_droppedItemDescription.c_str(), pItemSubSpawnData->m_droppedItemPlacementR, pItemSubSpawnData->m_droppedItemPlacementG, pItemSubSpawnData->m_droppedItemPlacementB, pItemSubSpawnData->m_droppedItemQuantity);
			pItem->SetAutoDisappear(20.0f + (GetRandomNumber(-20, 20, 1) * 0.2f));
			pItem->SetCollisionEnabled(false);
		}
	}
}

// Water
void ChunkManager::SetWaterHeight(float height)
{
	m_waterHeight = height;
}

float ChunkManager::GetWaterHeight() const
{
	return m_waterHeight;
}

bool ChunkManager::IsUnderWater(glm::vec3 position)
{
	if (m_pCubbySettings->m_waterRendering == false)
	{
		return false;
	}

	if (CubbyGame::GetInstance()->GetGameMode() == GameMode::FrontEnd)
	{
		return false;
	}

	if (position.y <= m_waterHeight)
	{
		return true;
	}

	return false;
}

// Rendering modes
void ChunkManager::SetWireframeRender(bool wireframe)
{
	m_wireframeRender = wireframe;
}

void ChunkManager::SetFaceMerging(bool faceMerge)
{
	m_faceMerging = faceMerge;
}

bool ChunkManager::GetFaceMerging() const
{
	return m_faceMerging;
}

// Updating
void ChunkManager::Update(float dt)
{
	m_numChunksLoaded = m_chunksMap.size();
}

void ChunkManager::_UpdatingChunksThread(void* pData)
{
	ChunkManager* pChunkManager = static_cast<ChunkManager*>(pData);
	pChunkManager->UpdatingChunksThread();
}

void ChunkManager::UpdatingChunksThread()
{
	while (m_updateThreadActive)
	{
		while (m_pPlayer == nullptr)
		{
#ifdef _WIN32
			Sleep(100);
#else
			usleep(100000);
#endif
		}

		while (m_stepLockEnabled == true && m_updateStepLock == true)
		{
#ifdef _WIN32
			Sleep(100);
#else
			usleep(100000);
#endif
		}

		ChunkList updateChunkList;
		ChunkCoordinateKeysList addChunkList;
		ChunkList rebuildChunkList;
		ChunkList unloadChunkList;

		m_ChunkMapMutexLock.lock();

		for (auto iter = m_chunksMap.begin(); iter != m_chunksMap.end(); ++iter)
		{
			Chunk* pChunk = iter->second;

			updateChunkList.push_back(pChunk);
		}

		m_ChunkMapMutexLock.unlock();

		// Updating chunks
		int numAddedChunks = 0;
		const int MAX_NUM_CHUNKS_ADD = 10;

		sort(updateChunkList.begin(), updateChunkList.end(), Chunk::ClosestToCamera);
		for (unsigned int i = 0; i < updateChunkList.size(); ++i)
		{
			Chunk* pChunk = updateChunkList[i];

			if (pChunk != nullptr)
			{
				int gridX = pChunk->GetGridX();
				int gridY = pChunk->GetGridY();
				int gridZ = pChunk->GetGridZ();

				float xPos = gridX * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
				float yPos = gridY * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
				float zPos = gridZ * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;

				glm::vec3 chunkCenter = glm::vec3(xPos, yPos, zPos) + glm::vec3(Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE);
				glm::vec3 distanceVec = chunkCenter - m_pPlayer->GetCenter();
				float lengthValue = length(distanceVec);

				if (lengthValue > m_loaderRadius)
				{
					unloadChunkList.push_back(pChunk);
				}
				else
				{
					if (numAddedChunks < MAX_NUM_CHUNKS_ADD)
					{
						// Check neighbors
						if (pChunk->GetNumNeighbors() < 6 && pChunk->IsEmpty() == false || gridY == 0)
						{
							if (pChunk->GetXMinus() == nullptr)
							{
								ChunkCoordinateKeys coordKey;
								coordKey.x = gridX - 1;
								coordKey.y = gridY;
								coordKey.z = gridZ;

								float xCoordPos = coordKey.x * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float yCoordPos = coordKey.y * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float zCoordPos = coordKey.z * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;

								glm::vec3 coordChunkCenter = glm::vec3(xCoordPos, yCoordPos, zCoordPos) + glm::vec3(Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE);
								glm::vec3 coordDistanceVec = coordChunkCenter - m_pPlayer->GetCenter();
								float coordLengthValue = length(coordDistanceVec);

								if (coordLengthValue <= m_loaderRadius)
								{
									addChunkList.push_back(coordKey);
									numAddedChunks++;
								}
							}

							if (pChunk->GetXPlus() == nullptr)
							{
								ChunkCoordinateKeys coordKey;
								coordKey.x = gridX + 1;
								coordKey.y = gridY;
								coordKey.z = gridZ;

								float xCoordPos = coordKey.x * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float yCoordPos = coordKey.y * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float zCoordPos = coordKey.z * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;

								glm::vec3 coordChunkCenter = glm::vec3(xCoordPos, yCoordPos, zCoordPos) + glm::vec3(Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE);
								glm::vec3 coordDistanceVec = coordChunkCenter - m_pPlayer->GetCenter();
								float coordLengthValue = length(coordDistanceVec);

								if (coordLengthValue <= m_loaderRadius)
								{
									addChunkList.push_back(coordKey);
									numAddedChunks++;
								}
							}

							if (pChunk->GetYMinus() == nullptr)
							{
								ChunkCoordinateKeys coordKey;
								coordKey.x = gridX;
								coordKey.y = gridY - 1;
								coordKey.z = gridZ;

								float xCoordPos = coordKey.x * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float yCoordPos = coordKey.y * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float zCoordPos = coordKey.z * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;

								glm::vec3 coordChunkCenter = glm::vec3(xCoordPos, yCoordPos, zCoordPos) + glm::vec3(Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE);
								glm::vec3 coordDistanceVec = coordChunkCenter - m_pPlayer->GetCenter();
								float coordLengthValue = length(coordDistanceVec);

								if (coordLengthValue <= m_loaderRadius)
								{
									addChunkList.push_back(coordKey);
									numAddedChunks++;
								}
							}

							if (pChunk->GetYPlus() == nullptr)
							{
								ChunkCoordinateKeys coordKey;
								coordKey.x = gridX;
								coordKey.y = gridY + 1;
								coordKey.z = gridZ;

								float xCoordPos = coordKey.x * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float yCoordPos = coordKey.y * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float zCoordPos = coordKey.z * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;

								glm::vec3 coordChunkCenter = glm::vec3(xCoordPos, yCoordPos, zCoordPos) + glm::vec3(Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE);
								glm::vec3 coordDistanceVec = coordChunkCenter - m_pPlayer->GetCenter();
								float coordLengthValue = length(coordDistanceVec);

								if (coordLengthValue <= m_loaderRadius)
								{
									addChunkList.push_back(coordKey);
									numAddedChunks++;
								}
							}

							if (pChunk->GetZMinus() == nullptr)
							{
								ChunkCoordinateKeys coordKey;
								coordKey.x = gridX;
								coordKey.y = gridY;
								coordKey.z = gridZ - 1;

								float xCoordPos = coordKey.x * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float yCoordPos = coordKey.y * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float zCoordPos = coordKey.z * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;

								glm::vec3 coordChunkCenter = glm::vec3(xCoordPos, yCoordPos, zCoordPos) + glm::vec3(Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE);
								glm::vec3 coordDistanceVec = coordChunkCenter - m_pPlayer->GetCenter();
								float coordLengthValue = length(coordDistanceVec);

								if (coordLengthValue <= m_loaderRadius)
								{
									addChunkList.push_back(coordKey);
									numAddedChunks++;
								}
							}

							if (pChunk->GetZPlus() == nullptr)
							{
								ChunkCoordinateKeys coordKey;
								coordKey.x = gridX;
								coordKey.y = gridY;
								coordKey.z = gridZ + 1;

								float xCoordPos = coordKey.x * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float yCoordPos = coordKey.y * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
								float zCoordPos = coordKey.z * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;

								glm::vec3 coordChunkCenter = glm::vec3(xCoordPos, yCoordPos, zCoordPos) + glm::vec3(Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE, Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE);
								glm::vec3 coordDistanceVec = coordChunkCenter - m_pPlayer->GetCenter();
								float coordLengthValue = length(coordDistanceVec);

								if (coordLengthValue <= m_loaderRadius)
								{
									addChunkList.push_back(coordKey);
									numAddedChunks++;
								}
							}
						}
					}
				}
			}
		}

		updateChunkList.clear();

		// Adding chunks
		for (size_t i = 0; i < addChunkList.size(); ++i)
		{
			ChunkCoordinateKeys coordKey = addChunkList[i];
			Chunk* pChunk = GetChunk(coordKey.x, coordKey.y, coordKey.z);

			if (pChunk == nullptr)
			{
				CreateNewChunk(coordKey.x, coordKey.y, coordKey.z);
			}
			else
			{
				UpdateChunkNeighbours(pChunk, coordKey.x, coordKey.y, coordKey.z);
			}
		}

		addChunkList.clear();

		// Unloading chunks
		for (size_t i = 0; i < unloadChunkList.size(); ++i)
		{
			Chunk* pChunk = unloadChunkList[i];

			UnloadChunk(pChunk);
		}

		unloadChunkList.clear();

		// Check for rebuild chunks
		m_ChunkMapMutexLock.lock();

		for (auto iter = m_chunksMap.begin(); iter != m_chunksMap.end(); ++iter)
		{
			Chunk* pChunk = iter->second;

			if (pChunk != nullptr)
			{
				if (pChunk->NeedsRebuild())
				{
					rebuildChunkList.push_back(pChunk);
				}
			}
		}

		m_ChunkMapMutexLock.unlock();

		// Rebuilding chunks
		int numRebuildChunks = 0;
		const int MAX_NUM_CHUNKS_REBUILD = 30;

		for (size_t i = 0; i < rebuildChunkList.size() && numRebuildChunks < MAX_NUM_CHUNKS_REBUILD; ++i)
		{
			Chunk* pChunk = rebuildChunkList[i];

			pChunk->SwitchToCachedMesh();
			pChunk->RebuildMesh();
			pChunk->CompleteMesh();
			pChunk->UndoCachedMesh();

			numRebuildChunks++;
		}

		rebuildChunkList.clear();

		if (m_stepLockEnabled == true && m_updateStepLock == false)
		{
			m_updateStepLock = true;
		}

#ifdef _WIN32
		Sleep(10);
#else
		usleep(10000);
#endif
	}

	m_updateThreadFinished = true;
}

// Rendering
void ChunkManager::Render(bool shadowRender)
{
	if (shadowRender == false)
	{
		m_numChunksRender = 0;
	}

	m_pRenderer->StartMeshRender();

	// Store cull mode
	CullMode cullMode = m_pRenderer->GetCullMode();

	if (m_wireframeRender)
	{
		m_pRenderer->SetLineWidth(1.0f);
		m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	}
	else
	{
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
	}

	m_pRenderer->PushMatrix();

	m_ChunkMapMutexLock.lock();

	for (auto iter = m_chunksMap.begin(); iter != m_chunksMap.end(); ++iter)
	{
		Chunk* pChunk = iter->second;

		if (pChunk != nullptr && pChunk->IsCreated() && pChunk->IsSetup() && pChunk->IsUnloading() == false && pChunk->IsEmpty() == false && pChunk->IsSurrounded() == false)
		{
			glm::vec3 chunkCenter = pChunk->GetPosition() + glm::vec3((Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE) - Chunk::BLOCK_RENDER_SIZE, (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE) - Chunk::BLOCK_RENDER_SIZE, (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE) - Chunk::BLOCK_RENDER_SIZE);

			if (shadowRender == true || m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), chunkCenter, Chunk::CHUNK_RADIUS))
			{
				// Fog
				glm::vec3 fogChunkCenter = pChunk->GetPosition() + glm::vec3((Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE) - Chunk::BLOCK_RENDER_SIZE, (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE) - Chunk::BLOCK_RENDER_SIZE, (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE) - Chunk::BLOCK_RENDER_SIZE);
				float toCamera = length(CubbyGame::GetInstance()->GetGameCamera()->GetPosition() - fogChunkCenter);
			
				if (toCamera > GetLoaderRadius() + (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 5.0f))
				{
					continue;
				}
				if (toCamera > GetLoaderRadius() - Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 3.0f)
				{
					m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
				}

				pChunk->Render();

				if (shadowRender == false)
				{
					m_numChunksRender++;
				}

				m_pRenderer->DisableTransparency();
			}
		}
	}

	m_ChunkMapMutexLock.unlock();

	m_pRenderer->PopMatrix();

	// Restore cull mode
	m_pRenderer->SetCullMode(cullMode);

	m_pRenderer->EndMeshRender();
}

void ChunkManager::RenderWater() const
{
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);

	m_pRenderer->EnableMaterial(m_chunkMaterialID);

	float waterDistance = 500.0f;

	m_pRenderer->SetCullMode(CullMode::NOCULL);

	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

	m_pRenderer->ImmediateVertex(-waterDistance, m_waterHeight, -waterDistance);
	m_pRenderer->ImmediateVertex(-waterDistance, m_waterHeight, waterDistance);
	m_pRenderer->ImmediateVertex(waterDistance, m_waterHeight, waterDistance);
	m_pRenderer->ImmediateVertex(waterDistance, m_waterHeight, -waterDistance);

	m_pRenderer->DisableImmediateMode();

	m_pRenderer->SetCullMode(CullMode::BACK);

	m_pRenderer->DisableTransparency();
}

void ChunkManager::RenderDebug()
{
	m_pRenderer->SetRenderMode(RenderMode::SOLID);

	m_ChunkMapMutexLock.lock();

	for (auto iter = m_chunksMap.begin(); iter != m_chunksMap.end(); ++iter)
	{
		Chunk* pChunk = iter->second;

		if (pChunk != nullptr && pChunk->IsCreated())
		{
			pChunk->RenderDebug();
		}
	}

	m_ChunkMapMutexLock.unlock();
}

void ChunkManager::Render2D(Camera* pCamera, unsigned int viewport, unsigned int font)
{
	m_ChunkMapMutexLock.lock();
	
	for (auto iter = m_chunksMap.begin(); iter != m_chunksMap.end(); ++iter)
	{
		Chunk* pChunk = iter->second;

		if (pChunk != nullptr && pChunk->IsCreated())
		{
			pChunk->Render2D(pCamera, viewport, font);
		}
	}
	
	m_ChunkMapMutexLock.unlock();
}