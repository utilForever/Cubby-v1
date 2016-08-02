/*************************************************************************
> File Name: ItemSpawner.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Item spawner class.
> Created Time: 2016/07/31
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ITEM_SPAWNER_H
#define CUBBY_ITEM_SPAWNER_H

#include <Blocks/ChunkManager.h>
#include <Blocks/BiomeManager.h>

#include "Item.h"

// Forward declaration
class LightingManager;
class GameWindow;
class BlockParticleManager;
class TextEffectsManager;
class ItemManager;
class ProjectileManager;
class Projectile;
class NPCManager;
class NPC;

class ItemSpawner
{
public:
	// Constructor, Destructor
	ItemSpawner(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer, ItemManager* pItemManager, NPCManager* pNPCManager);
	~ItemSpawner();

	// Accessors
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition() const;
	void SetInitialPosition(glm::vec3 pos);
	glm::vec3 GetInitialPosition() const;
	void SetFacingDirection(glm::vec3 dir);
	glm::vec3 GetFacingDirection() const;
	bool ShouldFollowPlayer() const;

	// Spawning params
	void SetSpawningParams(float initialSpawnDelay, float spawnTimer, int maxNumItemsActive, glm::vec3 spawnRandomOffset, bool shouldSpawnOnGround, glm::vec3 groundSpawnOffset, bool followPlayerIntheWorld, bool spawnFullLoaderRange, float minDistanceFromPlayer, Biome biomeSpawn, float spawnScale);
	void AddItemTypeToSpawn(ItemType item);

	// Items removed
	void RemoveItemFromThisSpawner();

	// Spawning
	void StartSpawning(float initialDelay);
	void StopSpawning();
	ItemType GetItemTypeToSpawn();
	bool GetSpawnPosition(glm::vec3* pSpawnPosition) const;

	// Updating
	void Update(float dt);
	void UpdateTimers(float dt);

	// Rendering
	void RenderDebug() const;

private:
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	Player* m_pPlayer;
	ItemManager* m_pItemManager;
	NPCManager* m_pNPCManager;

	// Are we spawning?
	bool m_spawning;

	// Can we spawn?
	bool m_canSpawn;

	// What biome do we spawn items in?
	Biome m_biomeSpawn;

	// Spawn time delay
	float m_spawnTime;

	// Number of spawned items from this spawner
	int m_numSpawnedItems;

	// Max number of items to have active at once
	int m_maxNumItemsToHaveActive;

	// Should we follow the player as he moves in the world? i.e So that we can constantly spawn items around the player
	bool m_followPlayerIntheWorld;

	// Can we spawn in the full loader range?
	bool m_spawnFullLoaderRange;

	// Spawn scale for the item
	float m_spawnScale;

	// Random spawn offset
	glm::vec3 m_spawnRandomOffset;

	// What is the minimum distance that we have to be from the player when spawning.
	float m_minDistanceFromPlayer;

	// Should we spawn on the ground?
	bool m_shouldSpawnOnGround;

	// Should we spawn an offset from the ground?
	glm::vec3 m_groundSpawnOffset;

	// Item types to spawn
	std::vector<ItemType> m_vpItemTypeList;

	// Spawner position
	glm::vec3 m_position;
	glm::vec3 m_initialiPosition;

	// Facing direction
	glm::vec3 m_spawnFacingDirection;

	// Spawning params
	float m_spawnCountdownTimer;
};


#endif