/*************************************************************************
> File Name: NPCManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Manager class to control all of the NPC creation, updating, rendering and
>    general management of NPC characters.
> Created Time: 2016/08/06
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_NPC_MANAGER_H
#define CUBBY_NPC_MANAGER_H

#include "NPC.h"

using NPCList = std::vector<NPC*>;

// Forward declaration
class BlockParticleManager;
class TextEffectsManager;
class ItemManager;
class ProjectileManager;
class EnemyManager;

class NPCManager
{
public:
	// Constructor, Destructor
	NPCManager(Renderer* pRenderer, ChunkManager* pChunkManager);
	~NPCManager();

	void SetPlayer(Player* pPlayer);
	void SetLightingManager(LightingManager* pLightingManager);
	void SetBlockParticleManager(BlockParticleManager* pBlockParticleManager);
	void SetTextEffectsManager(TextEffectsManager* pTextEffectsManager);
	void SetItemManager(ItemManager* pItemManager);
	void SetProjectileManager(ProjectileManager* pProjectileManager);
	void SetEnemyManager(EnemyManager* pEnemyManager);
	void SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager);

	// Clearing
	void ClearNPCs();
	void ClearNPCChunkCacheForChunk(Chunk* pChunk);

	// Creation
	NPC* CreateNPC(const char* name, std::string typeName, std::string modelName, glm::vec3 position, float scale, bool characterSelectScreen, bool useQubicleManager);
	void DeleteNPC(std::string name);

	// Get NPCs
	NPC* GetNPC(int index);
	NPC* GetNPCByName(std::string name);
	int GetNumNPCs();
	int GetNumRenderNPCs() const;
	void ResetNumRenderNPCs();

	// Enemy Died
	void SetEnemyDied(Enemy* pEnemy);

	// Collision
	void PushCollisions(NPC* pPushingNPC, glm::vec3 position, float radius);

	// Interaction
	NPC* CheckNPCPlayerInteraction();

	// Rendering Helpers
	void SetWireFrameRender(bool wireframe);

	// Updating
	void UpdateNamePickingSelection(int pickingID);
	void UpdateHoverNamePickingSelection(int pickingID);
	void UpdateWeaponLights();
	void UpdateWeaponParticleEffects();
	void Update(float dt);
	void UpdateScreenCoordinates2d(Camera* pCamera);
	void UpdateHoverNPCs();
	void UpdateNPCProjectileCheck();
	void CalculateWorldTransformMatrix();

	// Rendering
	void Render(bool outline, bool reflection, bool silhouette, bool renderOnlyOutline, bool renderOnlyNormal, bool shadow);
	void RenderFaces();
	void RenderWeaponTrails();
	void RenderNamePicking();
	void RenderOutlineNPCs();
	void RenderSubSelectionNPCs();
	void RenderSubSelectionNormalNPCs();
	void RenderSubSelectionOverlayNPCs();
	void RenderDebug();

	// Constants
	static float NPC_INTERACTION_DISTANCE;
	static float NPC_INTERACTION_RADIUS_CHECK;

private:
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	LightingManager* m_pLightingManager;
	Player* m_pPlayer;
	BlockParticleManager* m_pBlockParticleManager;
	TextEffectsManager* m_pTextEffectsManager;
	ItemManager* m_pItemManager;
	ProjectileManager* m_pProjectileManager;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	EnemyManager* m_pEnemyManager;

	int m_numRenderNPCs;

	// NPC List
	tthread::mutex m_NPCMutex;
	NPCList m_vpNPCList;
};

#endif