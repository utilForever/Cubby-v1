/*************************************************************************
> File Name: Chunk.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A chunk is a collection of voxel blocks that are arranged together for
>    easier manipulation and management, when a single voxel in a chunk is
>    modified the whole chunk is refreshed. Chunks are rendered together
>    as a single vertex buffer and thus each chunk can be considered a single
>    draw call to render many voxels.
> Created Time: 2016/07/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_CHUNK_H
#define CUBBY_CHUNK_H

#include <vector>

#include "BlocksEnum.h"
#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"

#include "tinythread/tinythread.h"

// Forward declaration
class ChunkManager;
class Player;
class SceneryManager;
class VoxSettings;
class Item;
class BiomeManager;

using ItemList = std::vector<Item*>;

class Chunk
{
public:
	// Constructor, Destructor
	Chunk(Renderer* pRenderer, ChunkManager* pChunkManager, VoxSettings* pVoxSettings);
	~Chunk();

	// Player pointer
	void SetPlayer(Player* pPlayer);

	// Scenery manager pointer
	void SetSceneryManager(SceneryManager* pSceneryManager);

	// Biome manager
	void SetBiomeManager(BiomeManager* pBiomeManager);

	// Initialize
	void Initialize();

	// Creation and destruction
	void SetCreated(bool created);
	bool IsCreated();
	void Unload();
	void Setup();
	bool IsSetup();
	bool IsUnloading();

	// Saving and loading
	void SaveChunk();
	void LoadChunk();

	// Position
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition();

	// Neighbours
	int GetNumNeighbours();
	void SetNumNeighbours(int neighbours);
	Chunk* GetXMinus();
	Chunk* GetXPlus();
	Chunk* GetYMinus();
	Chunk* GetYPlus();
	Chunk* GetZMinus();
	Chunk* GetZPlus();
	void SetXMinus(Chunk* pChunk);
	void SetXPlus(Chunk* pChunk);
	void SetYMinus(Chunk* pChunk);
	void SetYPlus(Chunk* pChunk);
	void SetZMinus(Chunk* pChunk);
	void SetZPlus(Chunk* pChunk);

	// Grid
	void SetGrid(int x, int y, int z);
	int GetGridX() const;
	int GetGridY() const;
	int GetGridZ() const;

	// Batch update
	void StartBatchUpdate();
	void StopBatchUpdate();

	// Active
	bool GetActive(int x, int y, int z);

	// Inside chunk
	bool IsInsideChunk(glm::vec3 pos);

	// Items
	void AddItem(Item* pItem);
	void RemoveItem(Item* pItem);
	void RemoveItems();

	// Block colour
	void SetColor(int x, int y, int z, float r, float g, float b, float a);
	void GetColor(int x, int y, int z, float* r, float* g, float* b, float* a);
	void SetColor(int x, int y, int z, unsigned int color);
	unsigned int GetColor(int x, int y, int z);

	// Block type
	BlockType GetBlockType(int x, int y, int z);
	void SetBlockType(int x, int y, int z, BlockType blockType);

	// Flags
	bool IsEmpty();
	bool IsSurrounded();
	void UpdateWallFlags();
	bool UpdateSurroundedFlag();
	void UpdateEmptyFlag();

	// Create mesh
	void CreateMesh();
	void CompleteMesh();
	void UpdateMergedSide(int* merged, int blockX, int blockY, int blockZ, int width, int height, glm::vec3* p1, glm::vec3* p2, glm::vec3* p3, glm::vec3* p4, int startX, int startY, int maxX, int maxY, bool positive, bool xFace, bool yFace, bool zFace);

	// Rebuild
	void RebuildMesh();
	void SetNeedsRebuild(bool rebuild, bool rebuildNeighours);
	bool NeedsRebuild();
	bool IsRebuildingMesh();
	void SwitchToCachedMesh();
	void UndoCachedMesh();

	// Updating
	void Update(float dt);

	// Rendering
	void Render();
	void RenderDebug();
	void Render2D(Camera* pCamera, unsigned int viewport, unsigned int font);

	// < Operator (Used for chunk sorting, closest to camera)
	bool operator<(const Chunk& w) const;
	static bool ClosestToCamera(const Chunk* lhs, const Chunk* rhs);

	// Constants
	static const int CHUNK_SIZE = 16;
	static const int CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
	static const int CHUNK_SIZE_CUBED = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
	static const float BLOCK_RENDER_SIZE;
	static const float CHUNK_RADIUS;

private:
	VoxSettings* m_pVoxSettings;
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	Player* m_pPlayer;
	SceneryManager* m_pSceneryManager;
	BiomeManager* m_pBiomeManager;

	// Chunk neighbours
	int m_numNeighbours;
	Chunk* m_pXMinus;
	Chunk* m_pXPlus;
	Chunk* m_pYMinus;
	Chunk* m_pYPlus;
	Chunk* m_pZMinus;
	Chunk* m_pZPlus;

	// Flag for change during a batch update
	bool m_chunkChangedDuringBatchUpdate;

	// Grid co-ordinates
	int m_gridX;
	int m_gridY;
	int m_gridZ;

	// Chunk position
	glm::vec3 m_position;

	// Setup and creation flags
	bool m_created;
	bool m_setup;
	bool m_isUnloading;
	bool m_rebuild;
	bool m_rebuildNeighours;
	bool m_isRebuildingMesh;
	bool m_deleteCachedMesh;

	// Counters
	int m_numRebuilds;

	// Flags for empty chunk and completely surrounded
	bool m_emptyChunk;
	bool m_surroundedChunk;

	// Used for testing if chunk completely covers neighbour chunks
	bool m_xMinusFull;
	bool m_xPlusFull;
	bool m_yMinusFull;
	bool m_yPlusFull;
	bool m_zMinusFull;
	bool m_zPlusFull;

	// The blocks color data
	unsigned int* m_color;

	// Block type
	BlockType *m_blockType;

	// Item list
	tthread::mutex m_itemMutexLock;
	ItemList m_vpItemList;

	// Render mesh
	TriangleMesh* m_pMesh;
	TriangleMesh* m_pCachedMesh;
};

#endif