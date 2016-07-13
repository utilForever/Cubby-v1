/*************************************************************************
> File Name: ProjectileManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Projectile manager class.
> Created Time: 2016/07/13
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include "CubbyGame.h"

#include "ProjectileManager.h"

// Constructor, Destructor
ProjectileManager::ProjectileManager(Renderer* pRenderer, ChunkManager* pChunkManager) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager), m_pLightingManager(nullptr),
	m_pBlockParticleManager(nullptr), m_pQubicleBinaryManager(nullptr),
	m_pGameWindow(nullptr), m_pPlayer(nullptr),
	m_numRenderProjectiles(0)
{

}

ProjectileManager::~ProjectileManager()
{
	ClearProjectiles();
}

void ProjectileManager::SetLightingManager(LightingManager* pLightingManager)
{
	m_pLightingManager = pLightingManager;
}

void ProjectileManager::SetBlockParticleManager(BlockParticleManager* pBlockParticleManager)
{
	m_pBlockParticleManager = pBlockParticleManager;
}

void ProjectileManager::SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager)
{
	m_pQubicleBinaryManager = pQubicleBinaryManager;
}

void ProjectileManager::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

// Clearing
void ProjectileManager::ClearProjectiles()
{
	// Projectile list
	m_projectileMutex.lock();

	for (size_t i = 0; i < m_vpProjectileList.size(); ++i)
	{
		delete m_vpProjectileList[i];
		m_vpProjectileList[i] = nullptr;
	}

	m_vpProjectileList.clear();

	m_projectileMutex.unlock();

	// Create list
	m_projectileCreateMutex.lock();

	for (size_t i = 0; i < m_vpProjectileCreateList.size(); ++i)
	{
		delete m_vpProjectileCreateList[i];
		m_vpProjectileCreateList[i] = nullptr;
	}

	m_vpProjectileCreateList.clear();

	m_projectileCreateMutex.unlock();
}

void ProjectileManager::ClearProjectileOwnerForEnemy(Enemy* pEnemy)
{
	m_projectileMutex.lock();

	for (size_t i = 0; i < m_vpProjectileList.size(); ++i)
	{
		if (m_vpProjectileList[i]->GetEnemyOwner() == pEnemy)
		{
			m_vpProjectileList[i]->SetOwner(nullptr, nullptr, nullptr);
		}
	}

	m_projectileMutex.unlock();
}

void ProjectileManager::ClearProjectileOwnerForNPC(NPC* pNPC)
{
	m_projectileMutex.lock();

	for (size_t i = 0; i < m_vpProjectileList.size(); ++i)
	{
		if (m_vpProjectileList[i]->GetNPCOwner() == pNPC)
		{
			m_vpProjectileList[i]->SetOwner(nullptr, nullptr, nullptr);
		}
	}

	m_projectileMutex.unlock();
}

// Creating
Projectile* ProjectileManager::CreateProjectile(glm::vec3 position, glm::vec3 velocity, float rotation, const char* objectFileName, float scale)
{
	Projectile* pNewProjectile = new Projectile(m_pRenderer, m_pChunkManager, m_pQubicleBinaryManager, objectFileName, scale);

	pNewProjectile->SetLightingManager(m_pLightingManager);
	pNewProjectile->SetGameWindow(m_pGameWindow);
	pNewProjectile->SetBlockParticleManager(m_pBlockParticleManager);
	pNewProjectile->SetPlayer(m_pPlayer);

	pNewProjectile->SetPosition(position);
	pNewProjectile->SetVelocity(velocity);
	pNewProjectile->SetRotation(rotation);

	m_projectileCreateMutex.lock();
	m_vpProjectileCreateList.push_back(pNewProjectile);
	m_projectileCreateMutex.unlock();

	return pNewProjectile;
}

// Get projectile
Projectile* ProjectileManager::GetProjectile(int index)
{
	m_projectileMutex.lock();
	Projectile* pProjectile = m_vpProjectileList[index];
	m_projectileMutex.unlock();

	return pProjectile;
}

// Get number of projectiles
int ProjectileManager::GetNumProjectiles()
{
	m_projectileMutex.lock();
	int numProjectiles = m_vpProjectileList.size();
	m_projectileMutex.unlock();

	return numProjectiles;
}

int ProjectileManager::GetNumRenderProjectiles() const
{
	return m_numRenderProjectiles;
}

bool IsProjectileNeedsErasing(Projectile* projectile)
{
	bool IsNeedErase = projectile->GetErase();

	if (IsNeedErase == true)
	{
		delete projectile;
	}

	return IsNeedErase;
}

// Rendering helpers
void ProjectileManager::CalculateWorldTransformMatrix()
{
	m_projectileMutex.lock();

	for (size_t i = 0; i < m_vpProjectileList.size(); ++i)
	{
		Projectile* pProjectile = m_vpProjectileList[i];

		pProjectile->CalculateWorldTransformMatrix();
	}

	m_projectileMutex.unlock();
}

// Updating
void ProjectileManager::Update(float dt)
{
	// Add any projectiles on the create list to the main list and then clear the create list
	m_projectileCreateMutex.lock();

	for (size_t i = 0; i < m_vpProjectileCreateList.size(); ++i)
	{
		Projectile* pProjectile = m_vpProjectileCreateList[i];

		m_projectileMutex.lock();
		m_vpProjectileList.push_back(pProjectile);
		m_projectileMutex.unlock();
	}

	m_vpProjectileCreateList.clear();
	
	m_projectileCreateMutex.unlock();

	// Remove any projectiles that need to be erased
	m_projectileMutex.lock();
	m_vpProjectileList.erase(remove_if(m_vpProjectileList.begin(), m_vpProjectileList.end(), IsProjectileNeedsErasing), m_vpProjectileList.end());
	m_projectileMutex.unlock();

	// Update projectiles
	m_projectileMutex.lock();

	for (size_t i = 0; i < m_vpProjectileList.size(); ++i)
	{
		Projectile* pProjectile = m_vpProjectileList[i];

		pProjectile->Update(dt);
	}

	m_projectileMutex.unlock();
}

void ProjectileManager::UpdateProjectileLights()
{
	m_projectileMutex.lock();

	for (size_t i = 0; i < m_vpProjectileList.size(); ++i)
	{
		Projectile* pProjectile = m_vpProjectileList[i];

		pProjectile->UpdateProjectileLights();
	}

	m_projectileMutex.unlock();
}

void ProjectileManager::UpdateProjectileParticleEffects()
{
	m_projectileMutex.lock();

	for (size_t i = 0; i < m_vpProjectileList.size(); ++i)
	{
		Projectile* pProjectile = m_vpProjectileList[i];

		pProjectile->UpdateProjectileParticleEffects();
	}

	m_projectileMutex.unlock();
}

// Rendering
void ProjectileManager::Render()
{
	m_numRenderProjectiles = 0;

	m_projectileMutex.lock();

	for (size_t i = 0; i < m_vpProjectileList.size(); ++i)
	{
		Projectile* pProjectile = m_vpProjectileList[i];

		// TODO: Add me back in - render culling
	
		// NOTE: We should always render the projectiles, even when outisde of frustum, else the explosion particles dont appear in correct place.
		// This is due to the model matrix only being updated properly in the render call, for the qubicle binary, that the explode function uses.
		// if(m_pRenderer->SphereInFrustum(VoxGame::GetInstance()->GetDefaultViewport(), pProjectile->GetCenter(), pProjectile->GetRadius()))
		{
			pProjectile->Render();

			m_numRenderProjectiles++;
		}
	}

	m_projectileMutex.unlock();
}

void ProjectileManager::RenderWeaponTrails()
{
	m_projectileMutex.lock();

	for (size_t i = 0; i < m_vpProjectileList.size(); ++i)
	{
		Projectile* pProjectile = m_vpProjectileList[i];

		// TODO: Add me back in - render culling
		
		if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pProjectile->GetCenter(), pProjectile->GetRadius()))
		{
			pProjectile->RenderWeaponTrails();
		}
	}

	m_projectileMutex.unlock();
}

void ProjectileManager::RenderDebug()
{
	m_projectileMutex.lock();
	
	for (size_t i = 0; i < m_vpProjectileList.size(); ++i)
	{
		Projectile* pProjectile = m_vpProjectileList[i];

		// TODO: Add me back in - render culling
	
		if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pProjectile->GetCenter(), pProjectile->GetRadius()))
		{
			pProjectile->RenderDebug();
		}
	}

	m_projectileMutex.unlock();
}
