/*************************************************************************
> File Name: ProjectileManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Projectile manager class.
> Created Time: 2016/07/13
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_PROJECTILE_MANAGER_H
#define CUBBY_PROJECTILE_MANAGER_H

#include <Blocks/ChunkManager.h>
#include <Particles/BlockParticleManager.h>

#include <../Libraries/tinythread/tinythread.h>

#include "Projectile.h"

// Forward declaration
class LightingManager;
class GameWindow;

using ProjectileList = std::vector<Projectile*>;

class ProjectileManager
{
public:
	// Constructor, Destructor
	ProjectileManager(Renderer* pRenderer, ChunkManager* pChunkManager);
	~ProjectileManager();

	void SetLightingManager(LightingManager* pLightingManager);
	void SetBlockParticleManager(BlockParticleManager* pBlockParticleManager);
	void SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager);
	void SetPlayer(Player* pPlayer);

	// Clearing
	void ClearProjectiles();
	void ClearProjectileOwnerForEnemy(Enemy* pEnemy);
	void ClearProjectileOwnerForNPC(NPC* pNPC);

	// Creating
	Projectile* CreateProjectile(glm::vec3 position, glm::vec3 velocity, float rotation, const char* objectFileName, float scale);

	// Get projectile
	Projectile* GetProjectile(int index);

	// Get number of projectiles
	int GetNumProjectiles();
	int GetNumRenderProjectiles() const;

	// Rendering helpers
	void CalculateWorldTransformMatrix();

	// Updating
	void Update(float dt);
	void UpdateProjectileLights();
	void UpdateProjectileParticleEffects();

	// Rendering
	void Render();
	void RenderWeaponTrails();
	void RenderDebug();

private:
	/* Private members */
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	LightingManager* m_pLightingManager;
	BlockParticleManager* m_pBlockParticleManager;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	GameWindow* m_pGameWindow;
	Player* m_pPlayer;

	int m_numRenderProjectiles;

	// Enemy lists
	tthread::mutex m_projectileMutex;
	ProjectileList m_vpProjectileList;
	tthread::mutex m_projectileCreateMutex;
	ProjectileList m_vpProjectileCreateList;
};

#endif