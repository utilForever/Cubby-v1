/*************************************************************************
> File Name: EnemyManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Enemy manager class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <CubbyGame.h>

#include <Items/ItemManager.h>
#include <Player/Player.h>

#include "EnemyManager.h"
#include <algorithm>

// Constructor, Destructor
EnemyManager::EnemyManager(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager), m_pLightingManager(nullptr),
	m_pPlayer(pPlayer), m_pBlockParticleManager(nullptr), m_pTextEffectsManager(nullptr),
	m_pItemManager(nullptr), m_pProjectileManager(nullptr), m_pHUD(nullptr),
	m_pQubicleBinaryManager(nullptr), m_pNPCManager(nullptr)
{
	m_numRenderEnemies = 0;
}

void EnemyManager::SetLightingManager(LightingManager* pLightingManager)
{
	m_pLightingManager = pLightingManager;
}

void EnemyManager::SetBlockParticleManager(BlockParticleManager* pBlockParticleManager)
{
	m_pBlockParticleManager = pBlockParticleManager;
}

void EnemyManager::SetTextEffectsManager(TextEffectsManager* pTextEffectsManager)
{
	m_pTextEffectsManager = pTextEffectsManager;
}

void EnemyManager::SetItemManager(ItemManager* pItemManager)
{
	m_pItemManager = pItemManager;
}

void EnemyManager::SetProjectileManager(ProjectileManager* pProjectileManager)
{
	m_pProjectileManager = pProjectileManager;
}

void EnemyManager::SetHUD(HUD* pHUD)
{
	m_pHUD = pHUD;
}

void EnemyManager::SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager)
{
	m_pQubicleBinaryManager = pQubicleBinaryManager;
}

void EnemyManager::SetNPCManager(NPCManager* pNPCManager)
{
	m_pNPCManager = pNPCManager;
}

EnemyManager::~EnemyManager()
{
	ClearEnemies();
	ClearEnemySpawners();
}

// Clearing
void EnemyManager::ClearEnemies()
{
	// Enemy list
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		delete m_vpEnemyList[i];
		m_vpEnemyList[i] = nullptr;
	}

	m_vpEnemyList.clear();

	m_enemyMutex.unlock();

	// Create list
	for (size_t i = 0; i < m_vpEnemyCreateList.size(); ++i)
	{
		delete m_vpEnemyCreateList[i];
		m_vpEnemyCreateList[i] = nullptr;
	}

	m_vpEnemyCreateList.clear();
}

void EnemyManager::ClearEnemyChunkCacheForChunk(Chunk* pChunk)
{
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		m_vpEnemyList[i]->ClearChunkCacheForChunk(pChunk);
	}

	m_enemyMutex.unlock();
}

void EnemyManager::ClearEnemySpawners()
{
	m_enemySpawnerMutex.lock();

	for (size_t i = 0; i < m_vpEnemySpawnerList.size(); ++i)
	{
		delete m_vpEnemySpawnerList[i];
		m_vpEnemySpawnerList[i] = nullptr;
	}

	m_vpEnemySpawnerList.clear();

	m_enemySpawnerMutex.unlock();
}

void EnemyManager::RemoveEnemySpawnerFromEnemies(EnemySpawner* pSpawner)
{
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		m_vpEnemyList[i]->RemoveEnemySpawner(pSpawner);
	}

	m_enemyMutex.unlock();

	for (size_t i = 0; i < m_vpEnemyCreateList.size(); ++i)
	{
		m_vpEnemyCreateList[i]->RemoveEnemySpawner(pSpawner);
	}
}

// Creation
Enemy* EnemyManager::CreateEnemy(glm::vec3 position, EnemyType enemyType, float scale)
{
	Enemy* pNewEnemy = new Enemy(m_pRenderer, m_pChunkManager, m_pPlayer, m_pLightingManager, m_pBlockParticleManager, m_pTextEffectsManager, m_pItemManager, m_pProjectileManager, m_pHUD, this, m_pNPCManager, m_pQubicleBinaryManager, enemyType);

	pNewEnemy->SetPosition(position);
	pNewEnemy->SetLeashParameters(position, 15.0f);
	pNewEnemy->ResetRandomTargetPosition();

	if (enemyType != EnemyType::Mimic)
	{
		pNewEnemy->SetRandomTargetMode(true);
	}

	pNewEnemy->SetScale(scale);

	m_vpEnemyCreateList.push_back(pNewEnemy);

	return pNewEnemy;
}

void EnemyManager::CreateEnemyCamp(glm::vec3 campPosition)
{
	Enemy* pEnemy1 = CreateEnemy(campPosition + glm::vec3(2.0f, 0.0f, 0.0f), EnemyType::NormalSkeleton, 0.08f);
	pEnemy1->SetRandomTargetMode(false);
	pEnemy1->SetTargetForwardToLookAtPoint(campPosition);
	
	Enemy* pEnemy2 = CreateEnemy(campPosition + glm::vec3(-1.0f, 0.0f, -3.0f), EnemyType::MeleeSkeleton, 0.08f);
	pEnemy2->SetRandomTargetMode(false);
	pEnemy2->SetTargetForwardToLookAtPoint(campPosition);
	
	Enemy* pEnemy3 = CreateEnemy(campPosition + glm::vec3(-1.3f, 0.0f, 2.5f), EnemyType::MageSkeleton, 0.08f);
	pEnemy3->SetRandomTargetMode(false);
	pEnemy3->SetTargetForwardToLookAtPoint(campPosition);
	
	Enemy* pEnemy4 = CreateEnemy(campPosition + glm::vec3(-3.3f, 0.0f, 0.5f), EnemyType::RangedSkeleton, 0.08f);
	pEnemy4->SetRandomTargetMode(false);
	pEnemy4->SetTargetForwardToLookAtPoint(campPosition);

	Item* pCampFire = m_pItemManager->CreateItem(campPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 45.0f, 0.0f), "Resources/gamedata/items/CampFire/CampFire.item", ItemType::CampFire, "Camp Fire", false, false, 0.08f);
	assert(pCampFire != nullptr);
}

EnemySpawner* EnemyManager::CreateEnemySpawner(glm::vec3 position, glm::vec3 direction)
{
	EnemySpawner* pEnemySpawner = new EnemySpawner(m_pRenderer, m_pChunkManager, m_pPlayer, this, m_pNPCManager);

	pEnemySpawner->SetPosition(position);
	pEnemySpawner->SetInitialPosition(position);
	pEnemySpawner->SetFacingDirection(normalize(direction));

	m_vpEnemySpawnerList.push_back(pEnemySpawner);

	return pEnemySpawner;
}

// Get number of enemies
int EnemyManager::GetNumEnemies()
{
	m_enemyMutex.lock();

	int numEnemies = static_cast<int>(m_vpEnemyList.size());

	m_enemyMutex.unlock();

	return numEnemies;
}

int EnemyManager::GetNumRenderEnemies() const
{
	return m_numRenderEnemies;
}

Enemy* EnemyManager::GetEnemy(int index)
{
	m_enemyMutex.lock();

	Enemy* pReturn = m_vpEnemyList[index];

	m_enemyMutex.unlock();

	return pReturn;
}

// Get enemy based on cursor position
Enemy* EnemyManager::GetCursorEnemy(Camera* pGameCamera, int cursorX, int cursorY)
{
	// Project the cursor into the world,
	m_pRenderer->PushMatrix();

	m_pRenderer->SetProjectionMode(ProjectionMode::PERSPECTIVE, CubbyGame::GetInstance()->GetDefaultViewport());
	pGameCamera->Look();

	glm::vec3 worldPos1 = m_pRenderer->GetWorldProjectionFromScreenCoordinates(cursorX, cursorY, 0.5f);
	glm::vec3 worldPos2 = m_pRenderer->GetWorldProjectionFromScreenCoordinates(cursorX, cursorY, 0.9999f);
	
	m_pRenderer->PopMatrix();

	// Check if our projected mouse line is colliding with any enemies
	bool collides = false;
	int interations = 0;
	float increments = Chunk::BLOCK_RENDER_SIZE * 0.1f;
	glm::vec3 worldNormal = normalize(worldPos2 - worldPos1);
	
	while (collides == false && interations < 100)
	{
		glm::vec3 newPos = worldPos1 + worldNormal * increments;

		m_enemyMutex.lock();

		for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
		{
			Enemy* pEnemy = m_vpEnemyList[i];

			glm::vec3 diff = pEnemy->GetCenter() - newPos;
			float diffLength = length(diff);
			
			if (diffLength < pEnemy->GetRadius() * 2.5f)
			{
				m_enemyMutex.unlock();
				return pEnemy;
			}
		}
		
		m_enemyMutex.unlock();

		interations++;
		increments += Chunk::BLOCK_RENDER_SIZE;
	}

	return nullptr;
}

// Rendering Helpers
void EnemyManager::SetWireFrameRender(bool wireframe)
{
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		pEnemy->SetWireFrameRender(wireframe);
	}

	m_enemyMutex.unlock();
}

// Gameplay
void EnemyManager::RemoveSappedFromEnemies(Enemy* pEnemyToBeSapped)
{
	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		if (pEnemy == pEnemyToBeSapped)
		{
			continue;
		}

		pEnemy->SetSapped(false);
	}
}

// Collision
void EnemyManager::PushCollisions(Enemy* pPushingEnemy, glm::vec3 position, float radius)
{
	m_enemyMutex.lock();

	for (auto iter = m_vpEnemyList.begin(); iter != m_vpEnemyList.end(); ++iter)
	{
		Enemy* pEnemy = *iter;

		if (pEnemy == pPushingEnemy)
		{
			continue;  // Can't push ourselves
		}

		glm::vec3 distance = pEnemy->GetCenter() - position;
		float lengthResult = length(distance) - radius - pEnemy->GetRadius();

		if (lengthResult < 0.0f)
		{
			glm::vec3 pushVector = distance;
			pushVector.y = 0.0f;  // Don't push in Y direction
			pushVector *= lengthResult;
			pEnemy->SetPosition(pEnemy->GetPosition() - pushVector);
		}
	}

	m_enemyMutex.unlock();
}

// Updating
void EnemyManager::UpdateWeaponLights()
{
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemy->GetCenter()) == false)
		//{
		//	continue;
		//}

		pEnemy->UpdateWeaponLights();
	}

	m_enemyMutex.unlock();
}

void EnemyManager::UpdateWeaponParticleEffects()
{
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemy->GetCenter()) == false)
		//{
		//	continue;
		//}

		pEnemy->UpdateWeaponParticleEffects();
	}

	m_enemyMutex.unlock();
}

bool NeedErase(Enemy* e)
{
	bool needsErase = e->GetErase();

	if (needsErase == true)
	{
		delete e;
	}

	return needsErase;
}

void EnemyManager::Update(float dt)
{
	// Update all enemy spawners
	m_enemySpawnerMutex.lock();

	for (size_t i = 0; i < m_vpEnemySpawnerList.size(); ++i)
	{
		EnemySpawner* pEnemySpawner = m_vpEnemySpawnerList[i];

		if (pEnemySpawner->ShouldFollowPlayer())
		{
			pEnemySpawner->SetPosition(m_pPlayer->GetCenter() + pEnemySpawner->GetInitialPosition());
		}

		// TODO : Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemySpawner->GetPosition()) == false)
		//{
		//	continue;
		//}

		pEnemySpawner->Update(dt);
	}

	m_enemySpawnerMutex.unlock();

	// Add any enemies on the create list to the main list and then clear the create list
	for (auto iter = m_vpEnemyCreateList.begin(); iter != m_vpEnemyCreateList.end(); ++iter)
	{
		Enemy* pEnemy = *iter;

		m_enemyMutex.lock();

		m_vpEnemyList.push_back(pEnemy);

		m_enemyMutex.unlock();
	}

	m_vpEnemyCreateList.clear();

	// Erase any dead enemies
	m_enemyMutex.lock();

	m_vpEnemyList.erase(remove_if(m_vpEnemyList.begin(), m_vpEnemyList.end(), NeedErase), m_vpEnemyList.end());

	// Update all enemies
	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemy->GetCenter()) == false)
		//{
		//	continue;
		//}

		pEnemy->Update(dt);

		m_enemyMutex.unlock();

		// Allow NPCs to push each other away (simple collision).
		PushCollisions(pEnemy, pEnemy->GetCenter(), pEnemy->GetRadius());

		m_enemyMutex.lock();
	}

	m_enemyMutex.unlock();

	// Update weapon lights
	UpdateWeaponLights();

	// Update weapon particle effects
	UpdateWeaponParticleEffects();

	// Update the players attack checking
	UpdateEnemyPlayerAttackCheck();

	// Check the projectile collisions
	UpdateEnemyProjectileCheck();
}

void EnemyManager::UpdateEnemyPlayerAttackCheck()
{
	if (m_pPlayer->IsDead() == true)
	{
		return;
	}

	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		if (pEnemy->GetErase() == true)
		{
			continue;
		}

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemy->GetCenter()) == false)
		//{
		//	continue;
		//}

		m_pPlayer->CheckEnemyDamageRadius(pEnemy);
	}

	m_enemyMutex.unlock();
}

void EnemyManager::UpdateEnemyProjectileCheck()
{
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		if (pEnemy->GetErase() == true)
		{
			continue;
		}

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemy->GetCenter()) == false)
		//{
		//	continue;
		//}

		for (int j = 0; j < m_pProjectileManager->GetNumProjectiles(); ++j)
		{
			Projectile* pProjectile = m_pProjectileManager->GetProjectile(j);

			if (pProjectile != nullptr && pProjectile->GetErase() == false)
			{
				pEnemy->CheckProjectileDamageRadius(pProjectile);
			}
		}
	}

	m_enemyMutex.unlock();
}

void EnemyManager::CalculateWorldTransformMatrix()
{
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		pEnemy->CalculateWorldTransformMatrix();
	}

	m_enemyMutex.unlock();
}

// Rendering
void EnemyManager::Render(bool outline, bool reflection, bool silhouette, bool shadow)
{
	m_numRenderEnemies = 0;

	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		if (silhouette && pEnemy->GetOutlineRender() == false)
		{
			continue; // Don't render silhouette unless we are rendering outline
		}

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemy->GetCenter()) == false)
		//{
		//	continue;
		//}

		// Fog
		float toCamera = length(CubbyGame::GetInstance()->GetGameCamera()->GetPosition() - pEnemy->GetCenter());
		if (toCamera > m_pChunkManager->GetLoaderRadius() + (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 5.0f))
		{
			continue;
		}
		if (toCamera > m_pChunkManager->GetLoaderRadius() - Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 3.0f)
		{
			m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		}

		if (shadow || m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pEnemy->GetCenter(), pEnemy->GetRadius()))
		{
			pEnemy->Render(outline, reflection, silhouette);

			m_numRenderEnemies++;
		}

		m_pRenderer->DisableTransparency();
	}

	m_enemyMutex.unlock();
}

void EnemyManager::RenderFaces()
{
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemy->GetCenter()) == false)
		//{
		//	continue;
		//}

		if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pEnemy->GetCenter(), pEnemy->GetRadius()))
		{
			pEnemy->RenderFace();
		}
	}

	m_enemyMutex.unlock();
}

void EnemyManager::RenderOutlineEnemies()
{
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		if (pEnemy->GetOutlineRender() == false)
		{
			continue;
		}

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemy->GetCenter()) == false)
		//{
		//	continue;
		//}

		if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pEnemy->GetCenter(), pEnemy->GetRadius()))
		{
			pEnemy->Render(true, false, false);
		}
	}

	m_enemyMutex.unlock();
}

void EnemyManager::RenderWeaponTrails()
{
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemy->GetCenter()) == false)
		//{
		//	continue;
		//}

		if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pEnemy->GetCenter(), pEnemy->GetRadius()))
		{
			pEnemy->RenderWeaponTrails();
		}
	}

	m_enemyMutex.unlock();
}

void EnemyManager::RenderDebug()
{
	// Enemies
	m_enemyMutex.lock();

	for (size_t i = 0; i < m_vpEnemyList.size(); ++i)
	{
		Enemy* pEnemy = m_vpEnemyList[i];

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pEnemy->GetCenter()) == false)
		//{
		//	continue;
		//}

		if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pEnemy->GetCenter(), pEnemy->GetRadius()))
		{
			pEnemy->RenderDebug();
		}
	}

	m_enemyMutex.unlock();

	// Enemy spawners
	m_enemySpawnerMutex.lock();
	for (size_t i = 0; i < m_vpEnemySpawnerList.size(); ++i)
	{
		EnemySpawner* pEnemySpawner = m_vpEnemySpawnerList[i];
		pEnemySpawner->RenderDebug();
	}

	m_enemySpawnerMutex.unlock();
}