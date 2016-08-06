/*************************************************************************
> File Name: EnemySpawner.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Enemy spawner class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ENEMY_SPAWNER_H
#define CUBBY_ENEMY_SPAWNER_H

#include <Blocks/BiomeManager.h>

#include "Enemy.h"

// Forward declaration
class LightingManager;
class GameWindow;
class EnemyManager;
class BlockParticleManager;
class TextEffectsManager;
class ItemManager;
class ProjectileManager;
class Projectile;
class NPCManager;
class NPC;

class EnemySpawner
{
public:
	// Constructor, Destructor
	EnemySpawner(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer, EnemyManager* pEnemyManager, NPCManager* pNPCManager);
	~EnemySpawner();

	// Accessors
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition() const;
	void SetInitialPosition(glm::vec3 pos);
	glm::vec3 GetInitialPosition() const;
	void SetFacingDirection(glm::vec3 dir);
	glm::vec3 GetFacingDirection() const;
	bool ShouldFollowPlayer() const;

	// Spawning params
	void SetSpawningParams(float initialSpawnDelay, float spawnTimer, int maxNumEnemiesActive, glm::vec3 spawnRandomOffset, bool shouldSpawnOnGround, glm::vec3 groundSpawnOffset, bool followPlayerIntheWorld, bool spawnFullLoaderRange, float minDistanceFromPlayer, Biome biomeSpawn);
	void AddEnemyTypeToSpawn(EnemyType enemyType);

	// Enemies removed
	void RemoveEnemyFromThisSpawner();

	// Spawning
	void StartSpawning(float initialDelay);
	void StopSpawning();
	EnemyType GetEnemyTypeToSpawn();
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
	EnemyManager* m_pEnemyManager;
	NPCManager* m_pNPCManager;

	// Are we spawning?
	bool m_spawning;

	// Can we spawn?
	bool m_canSpawn;

	// What biome do we spawn enemies in?
	Biome m_biomeSpawn;

	// Spawn time delay
	float m_spawnTime;

	// Number of spawned enemies from this spawner
	int m_numSpawnedEnemies;

	// Max number of enemies to have active at once
	int m_maxNumEnemiesToHaveActive;

	// Should we follow the player as he moves in the world? i.e So that we can constantly spawn enemies around the player
	bool m_followPlayerIntheWorld;

	// Can we spawn in the full loader range?
	bool m_spawnFullLoaderRange;

	// Random spawn offset
	glm::vec3 m_spawnRandomOffset;

	// What is the minimum distance that we have to be from the player when spawning.
	float m_minDistanceFromPlayer;

	// Should we spawn on the ground?
	bool m_shouldSpawnOnGround;

	// Should we spawn an offset from the ground? (Flying enemies)
	glm::vec3 m_groundSpawnOffset;

	// Enemy types to spawn
	std::vector<EnemyType> m_vpEnemyTypeList;

	// Spawner position
	glm::vec3 m_position;
	glm::vec3 m_initialiPosition;

	// Facing direction
	glm::vec3 m_spawnFacingDirection;

	// Spawning params
	float m_spawnCountdownTimer;
};

#endif