/*************************************************************************
> File Name: EnemySpawner.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Enemy spawner class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <CubbyGame.h>

#include <Player/Player.h>
#include <Utils/Random.h>

#include "EnemyManager.h"
#include "EnemySpawner.h"

// Constructor, Destructor
EnemySpawner::EnemySpawner(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer, EnemyManager* pEnemyManager, NPCManager* pNPCManager) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager), m_pPlayer(pPlayer), m_pEnemyManager(pEnemyManager), m_pNPCManager(pNPCManager)
{
	m_spawning = false;
	m_canSpawn = false;
	m_biomeSpawn = Biome::None;
	m_spawnTime = 1.0f;
	m_followPlayerIntheWorld = false;
	m_spawnFullLoaderRange = false;
	m_minDistanceFromPlayer = 0;

	m_numSpawnedEnemies = 0;
	m_maxNumEnemiesToHaveActive = 1;
	m_shouldSpawnOnGround = true;

	m_spawnCountdownTimer = 0.0f;
}

EnemySpawner::~EnemySpawner()
{
	m_pEnemyManager->RemoveEnemySpawnerFromEnemies(this);
}

void EnemySpawner::SetPosition(glm::vec3 pos)
{
	m_position = pos;
}

glm::vec3 EnemySpawner::GetPosition() const
{
	return m_position;
}

void EnemySpawner::SetInitialPosition(glm::vec3 pos)
{
	m_initialiPosition = pos;
}

glm::vec3 EnemySpawner::GetInitialPosition() const
{
	return m_initialiPosition;
}

void EnemySpawner::SetFacingDirection(glm::vec3 dir)
{
	m_spawnFacingDirection = dir;
}

glm::vec3 EnemySpawner::GetFacingDirection() const
{
	return m_spawnFacingDirection;
}

bool EnemySpawner::ShouldFollowPlayer() const
{
	return m_followPlayerIntheWorld;
}

// Spawning params
void EnemySpawner::SetSpawningParams(float initialSpawnDelay, float spawnTimer, int maxNumEnemiesActive, glm::vec3 spawnRandomOffset, bool shouldSpawnOnGround, glm::vec3 groundSpawnOffset, bool followPlayerIntheWorld, bool spawnFullLoaderRange, float minDistanceFromPlayer, Biome biomeSpawn)
{
	m_spawnTime = spawnTimer;
	m_maxNumEnemiesToHaveActive = maxNumEnemiesActive;
	m_spawnRandomOffset = spawnRandomOffset;
	m_shouldSpawnOnGround = shouldSpawnOnGround;
	m_groundSpawnOffset = groundSpawnOffset;
	m_followPlayerIntheWorld = followPlayerIntheWorld;
	m_spawnFullLoaderRange = spawnFullLoaderRange;
	m_minDistanceFromPlayer = minDistanceFromPlayer;
	m_biomeSpawn = biomeSpawn;

	if (initialSpawnDelay >= 0.0f)
	{
		StartSpawning(initialSpawnDelay);
	}
}

void EnemySpawner::AddEnemyTypeToSpawn(EnemyType enemyType)
{
	m_vpEnemyTypeList.push_back(enemyType);
}

// Enemies killed
void EnemySpawner::RemoveEnemyFromThisSpawner()
{
	m_numSpawnedEnemies -= 1;
}

// Spawning
void EnemySpawner::StartSpawning(float initialDelay)
{
	m_spawning = true;
	m_spawnCountdownTimer = initialDelay;
}

void EnemySpawner::StopSpawning()
{
	m_spawning = false;
}

EnemyType EnemySpawner::GetEnemyTypeToSpawn()
{
	int numEnemyTypes = static_cast<int>(m_vpEnemyTypeList.size());

	int randomNum = GetRandomNumber(0, numEnemyTypes - 1);

	return m_vpEnemyTypeList[randomNum];
}

bool EnemySpawner::GetSpawnPosition(glm::vec3* pSpawnPosition) const
{
	bool locationGood = false;
	int numTries = 0;

	while (locationGood == false && numTries < 10)
	{
		glm::vec3 spawnPos = m_position;
		glm::vec3 randomOffset;

		if (m_spawnFullLoaderRange)
		{
			float loaderRadius = m_pChunkManager->GetLoaderRadius();
			randomOffset = glm::vec3(GetRandomNumber(-100, 100, 2) * 0.01f * loaderRadius, GetRandomNumber(-100, 100, 2) * 0.01f * loaderRadius, GetRandomNumber(-100, 100, 2) * 0.01f * loaderRadius);
		}
		else
		{
			randomOffset = glm::vec3(GetRandomNumber(-100, 100, 2) * 0.01f * (m_spawnRandomOffset.x - 8.0f), GetRandomNumber(-100, 100, 2) * 0.01f * (m_spawnRandomOffset.y - 8.0f), GetRandomNumber(-100, 100, 2) * 0.01f * (m_spawnRandomOffset.z - 8.0f));
		}

		spawnPos += randomOffset;

		int blockX, blockY, blockZ;
		glm::vec3 blockPos;
		Chunk* pChunk = nullptr;

		bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(spawnPos.x, spawnPos.y, spawnPos.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);

		if (pChunk != nullptr && pChunk->IsSetup() && active == false)
		{
			if (m_shouldSpawnOnGround)
			{
				glm::vec3 floorPosition;
				if (m_pChunkManager->FindClosestFloor(spawnPos, &floorPosition))
				{
					spawnPos = floorPosition + glm::vec3(0.0f, 0.01f, 0.0f);
					spawnPos += m_groundSpawnOffset;

					Biome biome = CubbyGame::GetInstance()->GetBiomeManager()->GetBiome(spawnPos);
					ZoneData* pTown = nullptr;
					ZoneData* pSafeZone = nullptr;
					bool isInTown = CubbyGame::GetInstance()->GetBiomeManager()->IsInTown(spawnPos, &pTown);
					bool isInSafeZone = CubbyGame::GetInstance()->GetBiomeManager()->IsInSafeZone(spawnPos, &pSafeZone);
					
					if (biome == m_biomeSpawn && isInTown == false && isInSafeZone == false)
					{
						*pSpawnPosition = spawnPos;
						locationGood = true;
					}
				}
			}
		}

		numTries++;
	}

	return locationGood;
}

// Updating
void EnemySpawner::Update(float dt)
{
	m_canSpawn = m_numSpawnedEnemies < m_maxNumEnemiesToHaveActive;

	// Update timers
	UpdateTimers(dt);

	if (m_spawning && m_canSpawn)
	{
		if (m_spawnCountdownTimer <= 0.0f)
		{
			glm::vec3 spawnPos;
			bool spawnGood = GetSpawnPosition(&spawnPos);
			int numEnemies = m_pEnemyManager->GetNumEnemies();

			if (spawnGood && numEnemies < EnemyManager::MAX_NUM_ENEMIES)
			{
				glm::vec3 toPlayer = spawnPos - m_pPlayer->GetCenter();
				if (length(toPlayer) > m_minDistanceFromPlayer)
				{
					EnemyType enemyType = GetEnemyTypeToSpawn();

					Enemy* pEnemy = m_pEnemyManager->CreateEnemy(spawnPos, enemyType, 0.08f);
					pEnemy->SetSpawningParams(spawnPos, spawnPos, 0.0f);
					pEnemy->SetTargetForwardToLookAtPoint(spawnPos + m_spawnFacingDirection);
					pEnemy->SetEnemySpawner(this);

					pEnemy->SetRotation(GetRandomNumber(0, 360, 2));
					pEnemy->SetTargetForwardToLookAtPoint(pEnemy->GetPosition() + pEnemy->GetTargetForward());

					m_numSpawnedEnemies += 1;

					m_spawnCountdownTimer = m_spawnTime;
				}
			}
			else
			{
				// Can't spawn an enemy at a good position, wait a while before we try again
				m_spawnCountdownTimer = 0.5f;
			}
		}
	}
}

void EnemySpawner::UpdateTimers(float dt)
{
	if (m_spawning && m_canSpawn)
	{
		if (m_spawnCountdownTimer > 0.0f)
		{
			m_spawnCountdownTimer -= dt;
		}
	}
}

void EnemySpawner::RenderDebug() const
{
	m_pRenderer->PushMatrix();

	float length = 0.5f;
	float height = 0.5f;
	float width = 0.5f;

	m_pRenderer->TranslateWorldMatrix(m_position.x, m_position.y, m_position.z);

	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->SetCullMode(CullMode::NOCULL);
	m_pRenderer->SetLineWidth(1.0f);

	// TODO: Loader culling
	//if(m_pChunkManager->IsInsideLoader(m_position) == false)
	//{
	//m_pRenderer->ImmediateColourAlpha(0.1f, 0.8f, 0.85f, 1.0f);
	//}

	if (m_spawning && m_canSpawn)
	{
		m_pRenderer->ImmediateColorAlpha(0.1f, 0.8f, 0.05f, 1.0f);
	}
	else if (m_spawning)
	{
		m_pRenderer->ImmediateColorAlpha(0.8f, 0.8f, 0.05f, 1.0f);
	}
	else
	{
		m_pRenderer->ImmediateColorAlpha(0.8f, 0.1f, 0.05f, 1.0f);
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

	// Spawn facing direction
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->SetLineWidth(3.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

	m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(m_spawnFacingDirection.x * 1.5f, m_spawnFacingDirection.y * 1.5f, m_spawnFacingDirection.z * 1.5f);
	
	m_pRenderer->DisableImmediateMode();

	m_pRenderer->SetCullMode(CullMode::BACK);
	
	m_pRenderer->PopMatrix();
}
