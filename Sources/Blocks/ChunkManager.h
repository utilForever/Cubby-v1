/*************************************************************************
> File Name: ChunkManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    ChunkManager handles all the chunk operations and management of the
>    individual chunks, deciding which chunks to load, unload, create, update
>    and also the rendering of the chunks.
> Created Time: 2016/07/09
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_CHUNK_MANAGER_H
#define CUBBY_CHUNK_MANAGER_H

#include <map>

#include <tinythread/tinythread.h>

#include <Renderer/Renderer.h>
#include <Models/QubicleBinary.h>

#include "Chunk.h"
#include "BlocksEnum.h"

// Forward declaration
class Player;
class NPCManager;
class EnemyManager;
class BlockParticleManager;
class ItemManager;
class SceneryManager;
class CubbySettings;
class QubicleBinaryManager;
class BiomeManager;

struct ChunkCoordinateKeys
{
	int x;
	int y;
	int z;
};

inline bool operator==(const ChunkCoordinateKeys& l, const ChunkCoordinateKeys& r)
{
	return l.x == r.x && l.y == r.y && l.z == r.z;
};

inline bool operator<(const ChunkCoordinateKeys& l, const ChunkCoordinateKeys& r)
{
	if (l.x < r.x)
	{
		return true;
	}
	if (l.x > r.x)
	{
		return false;
	}

	if (l.y < r.y)
	{
		return true;
	}
	if (l.y > r.y)
	{
		return false;
	}

	if (l.z < r.z)
	{
		return true;
	}
	if (l.z > r.z)
	{
		return false;
	}

	return false;
};

using ChunkList = std::vector<Chunk*>;
using ChunkCoordinateKeysList = std::vector<ChunkCoordinateKeys>;

enum class QubicleImportDirection
{
	Normal = 0,
	MirrorX,
	MirrorY,
	MirrorZ,
	RotateY90,
	RotateY180,
	RotateY270,
	RotateX90,
	RotateX180,
	RotateX270,
	RotateZ90,
	RotateZ180,
	RotateZ270,
};

struct ChunkStorageLoader
{
	ChunkStorageLoader(int gridX, int gridY, int gridZ) :
		m_gridX(gridX), m_gridY(gridY), m_gridZ(gridZ)
	{
		float xPos = gridX * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
		float yPos = gridY * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;
		float zPos = gridZ * Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 2.0f;

		m_position = glm::vec3(xPos, yPos, zPos);

		// Reset initial arrays
		for (int x = 0; x < Chunk::CHUNK_SIZE; ++x)
		{
			for (int y = 0; y < Chunk::CHUNK_SIZE; ++y)
			{
				for (int z = 0; z < Chunk::CHUNK_SIZE; ++z)
				{
					m_blockSet[x][y][z] = false;
					m_color[x][y][z] = 0;
				}
			}
		}
	}

	void SetBlockColor(int x, int y, int z, unsigned int color)
	{
		m_blockSet[x][y][z] = true;
		m_color[x][y][z] = color;
	}

	int m_gridX;
	int m_gridY;
	int m_gridZ;

	glm::vec3 m_position;

	bool m_blockSet[Chunk::CHUNK_SIZE][Chunk::CHUNK_SIZE][Chunk::CHUNK_SIZE];
	unsigned int m_color[Chunk::CHUNK_SIZE][Chunk::CHUNK_SIZE][Chunk::CHUNK_SIZE];
};

using ChunkStorageLoaderList = std::vector<ChunkStorageLoader*>;

struct BlockColorTypeMatch
{
	int m_red;
	int m_green;
	int m_blue;
	BlockType m_blockType;
};

using BlockColorTypeMatchList = std::vector<BlockColorTypeMatch*>;

class ChunkManager
{
public:
	/* Public methods */
	ChunkManager(Renderer* pRenderer, CubbySettings* pCubbySettings, QubicleBinaryManager* pQubicleBinaryManager);
	~ChunkManager();

	// Linkage
	void SetPlayer(Player* pPlayer);
	void SetNPCManager(NPCManager* pNPCManager);
	void SetEnemyManager(EnemyManager* pEnemyManager);
	void SetBlockParticleManager(BlockParticleManager* pBlockParticleManager);
	void SetItemManager(ItemManager* pItemManager);

	// Scenery manager pointer
	void SetSceneryManager(SceneryManager* pSceneryManager);

	// Biome manager
	void SetBiomeManager(BiomeManager* pBiomeManager);

	// Initial chunk creation
	void InitializeChunkCreation();

	// Chunk rendering material
	unsigned int GetChunkMaterialID() const;

	// Chunk counters
	int GetNumChunksLoaded() const;
	int GetNumChunksRender() const;

	// Loader radius
	void SetLoaderRadius(float radius);
	float GetLoaderRadius() const;

	// Step update
	void SetStepLockEnabled(bool enabled);
	void StepNextUpdate();

	// Chunk Creation
	void CreateNewChunk(int x, int y, int z);
	void UnloadChunk(Chunk* pChunk);
	void UpdateChunkNeighbours(Chunk* pChunk, int x, int y, int z);

	// Getting chunk and positional information
	void GetGridFromPosition(glm::vec3 position, int* gridX, int* gridY, int* gridZ) const;
	Chunk* GetChunkFromPosition(float posX, float posY, float posZ);
	Chunk* GetChunk(int x, int y, int z);
	bool FindClosestFloor(glm::vec3 position, glm::vec3* floorPosition);

	// Getting the active block state given a position and chunk information
	bool GetBlockActiveFrom3DPosition(float x, float y, float z, glm::vec3* blockPos, int* blockX, int* blockY, int* blockZ, Chunk** pChunk);
	void GetBlockGridFrom3DPositionChunkStorage(float x, float y, float z, int* blockX, int* blockY, int* blockZ, ChunkStorageLoader* chunkStorage) const;

	// Adding to chunk storage for parts of the world generation that are outside of loaded chunks
	ChunkStorageLoader* GetChunkStorage(int x, int y, int z, bool createIfNotExist);
	void RemoveChunkStorageLoader(ChunkStorageLoader* pChunkStorage);

	// Block color to block type matching
	void AddBlockColorBlockTypeMatching(int r, int g, int b, BlockType blockType);
	bool CheckBlockColor(int r, int g, int b, int rCheck, int gCheck, int bCheck) const;
	BlockType SetBlockTypeBasedOnColor(int r, int g, int b);

	// Importing into the world chunks
	void ImportQubicleBinaryMatrix(QubicleMatrix* pMatrix, glm::vec3 position, QubicleImportDirection direction);
	QubicleBinary* ImportQubicleBinary(QubicleBinary* qubicleBinaryFile, glm::vec3 position, QubicleImportDirection direction);
	QubicleBinary* ImportQubicleBinary(const char* fileName, glm::vec3 position, QubicleImportDirection direction);

	// Explosions
	void CreateBlockDestroyParticleEffect(float r, float g, float b, float a, glm::vec3 blockPosition) const;
	void ExplodeSphere(glm::vec3 position, float radius);

	// Collectible block objects
	void CreateCollectibleBlock(BlockType blockType, glm::vec3 blockPos);

	// Water
	void SetWaterHeight(float height);
	float GetWaterHeight() const;
	bool IsUnderWater(glm::vec3 position);

	// Rendering modes
	void SetWireframeRender(bool wireframe);
	void SetFaceMerging(bool faceMerge);
	bool GetFaceMerging() const;

	// Updating
	void Update(float dt);
	static void _UpdatingChunksThread(void* pData);
	void UpdatingChunksThread();

	// Rendering
	void Render(bool shadowRender);
	void RenderWater() const;
	void RenderDebug();
	void Render2D(Camera* pCamera, unsigned int viewport, unsigned int font);

private:
	Renderer* m_pRenderer;
	Player* m_pPlayer;
	SceneryManager* m_pSceneryManager;
	BiomeManager* m_pBiomeManager;
	CubbySettings* m_pCubbySettings;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	ItemManager* m_pItemManager;
	BlockParticleManager* m_pBlockParticleManager;
	EnemyManager* m_pEnemyManager;
	NPCManager* m_pNPCManager;

	// Chunk Material
	unsigned int m_chunkMaterialID;

	// Loader radius
	float m_loaderRadius;

	// Water
	float m_waterHeight;

	// Update step lock
	bool m_stepLockEnabled;
	bool m_updateStepLock;

	// Render modes
	bool m_wireframeRender;
	bool m_faceMerging;

	// Chunks storage
	std::map<ChunkCoordinateKeys, Chunk*> m_chunksMap;

	// Storage for modifications to chunks that are not loaded yet
	ChunkStorageLoaderList m_vpChunkStorageList;
	tthread::mutex m_chunkStorageListLock;

	// Block colour to type matching boundaries
	BlockColorTypeMatchList m_vpBlockColorTypeMatchList;

	// Chunk counters
	int m_numChunksLoaded;
	int m_numChunksRender;

	// Threading
	tthread::thread* m_pUpdatingChunksThread;
	tthread::mutex m_ChunkMapMutexLock;
	bool m_updateThreadActive;
	tthread::mutex m_updateThreadFlagLock;
	bool m_updateThreadFinished;
};

#endif