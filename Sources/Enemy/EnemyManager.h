/*************************************************************************
> File Name: EnemyManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Enemy manager class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ENEMY_MANAGER_H
#define CUBBY_ENEMY_MANAGER_H

#include "Enemy.h"
#include "EnemySpawner.h"

// Forward declaration
class LightingManager;
class BlockParticleManager;
class TextEffectsManager;
class ItemManager;
class ProjectileManager;
class HUD;
class NPCManager;

using EnemyList = std::vector<Enemy*>;
using EnemySpawnerList = std::vector<EnemySpawner*>;

class EnemyManager
{
public:
	// Constructor, Destructor
	EnemyManager(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer);
	~EnemyManager();

	void SetLightingManager(LightingManager* pLightingManager);
	void SetBlockParticleManager(BlockParticleManager* pBlockParticleManager);
	void SetTextEffectsManager(TextEffectsManager* pTextEffectsManager);
	void SetItemManager(ItemManager* pItemManager);
	void SetProjectileManager(ProjectileManager* pProjectileManager);
	void SetHUD(HUD* pHUD);
	void SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager);
	void SetNPCManager(NPCManager* pNPCManager);

	// Clearing
	void ClearEnemies();
	void ClearEnemyChunkCacheForChunk(Chunk* pChunk);
	void ClearEnemySpawners();

	void RemoveEnemySpawnerFromEnemies(EnemySpawner* pSpawner);

	// Creation
	Enemy* CreateEnemy(glm::vec3 position, EnemyType enemyType, float scale);
	void CreateEnemyCamp(glm::vec3 campPosition);
	EnemySpawner* CreateEnemySpawner(glm::vec3 position, glm::vec3 direction);

	// Get number of enemies
	int GetNumEnemies();
	int GetNumRenderEnemies() const;
	Enemy* GetEnemy(int index);

	// Get enemy based on cursor position
	Enemy* GetCursorEnemy(Camera* pGameCamera, int cursorX, int cursorY);

	// Rendering Helpers
	void SetWireFrameRender(bool wireframe);

	// Gameplay
	void RemoveSappedFromEnemies(Enemy* pEnemyToBeSapped);

	// Collision
	void PushCollisions(Enemy* pPushingEnemy, glm::vec3 position, float radius);

	// Updating
	void UpdateWeaponLights();
	void UpdateWeaponParticleEffects();
	void Update(float dt);
	void UpdateEnemyPlayerAttackCheck();
	void UpdateEnemyProjectileCheck();
	void CalculateWorldTransformMatrix();

	// Rendering
	void Render(bool outline, bool reflection, bool silhouette, bool shadow);
	void RenderFaces();
	void RenderOutlineEnemies();
	void RenderWeaponTrails();
	void RenderDebug();

	// Constants
	static const int MAX_NUM_ENEMIES = 15;

private:
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	LightingManager* m_pLightingManager;
	Player* m_pPlayer;
	BlockParticleManager* m_pBlockParticleManager;
	TextEffectsManager* m_pTextEffectsManager;
	ItemManager* m_pItemManager;
	ProjectileManager* m_pProjectileManager;
	HUD* m_pHUD;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	NPCManager* m_pNPCManager;

	int m_numRenderEnemies;

	// Enemy lists
	tthread::mutex m_enemyMutex;
	EnemyList m_vpEnemyList;
	EnemyList m_vpEnemyCreateList;

	// Enemy spawner
	tthread::mutex m_enemySpawnerMutex;
	EnemySpawnerList m_vpEnemySpawnerList;
};


#endif