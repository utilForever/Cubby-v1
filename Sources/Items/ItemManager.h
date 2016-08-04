/*************************************************************************
> File Name: ItemManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The item manager is in charge of managing and all the items in the game,
>    including creation, destruction, updating and rendering the items.
>    Different rendering modes are also supported for items depending on their
>    state.
> Created Time: 2016/08/02
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ITEM_MANAGER_H
#define CUBBY_ITEM_MANAGER_H

#include "ItemSpawner.h"

// Forward declaration
class LightingManager;

using ItemSpawnerList = std::vector<ItemSpawner*>;

struct ItemSubSpawnData
{
	ItemType m_sourceItem;
	BlockType m_sourceBlockType;

	ItemType m_spawnedItem;
	std::string m_spawnedItemFileName;
	std::string m_spawnedItemTitle;
	bool m_collectible;
	bool m_interactable;
	float m_scale;

	std::string m_droppedItemFileName;
	std::string m_droppedItemTextureFileName;
	InventoryType m_droppedItemInventoryType;
	ItemType m_droppedItemItem;
	ItemStatus m_droppedItemStatus;
	EquipSlot m_droppedItemEquipSlot;
	ItemQuality m_droppedItemQuality;
	bool m_droppedItemLeft;
	bool m_droppedItemRight;
	std::string m_droppedItemTitle;
	std::string m_droppedItemDescription;
	float m_droppedItemPlacementR;
	float m_droppedItemPlacementG;
	float m_droppedItemPlacementB;
	int m_droppedItemQuantity;
};

using ItemList = std::vector<Item*>;
using ItemSubSpawnDataList = std::vector<ItemSubSpawnData*>;

class ItemManager
{
public:
	// Constructor, Destructor
	ItemManager(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer);
	~ItemManager();

	void SetLightingManager(LightingManager* pLightingManager);
	void SetBlockParticleManager(BlockParticleManager* pBlockParticleManager);
	void SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager);
	void SetInventoryManager(InventoryManager* pInventoryManager);
	void SetNPCManager(NPCManager* pNPCManager);

	// Deletion
	void ClearItems();
	void ClearSubSpawnData();
	void ClearItemSpawners();

	void RemoveItemSpawnerFromItems(ItemSpawner* pSpawner);

	// Item sub spawn data
	ItemSubSpawnData* GetItemSubSpawnData(ItemType sourceItem);
	ItemSubSpawnData* GetItemSubSpawnData(BlockType sourceBlockType);

	// Creation
	Item* CreateItem(glm::vec3 position, glm::vec3 velocity, glm::vec3 rotation, const char* objectFileName, ItemType itemType, const char* itemTitle, bool interactable, bool collectible, float scale);
	void RemoveItem(const char* itemTitle);
	ItemSpawner* CreateItemSpawner(glm::vec3 position, glm::vec3 direction);

	// Get number of items
	int GetNumItems() const;
	int GetNumRenderItems() const;

	// Collision detection
	bool CheckCollisions(glm::vec3 center, glm::vec3 previousCenter, float radius, glm::vec3* pNormal, glm::vec3* pMovement);

	// Player item interaction
	Item* CheckItemPlayerInteraction();

	// Rendering Helpers
	void CalculateWorldTransformMatrix();
	void SetWireFrameRender(bool wireframe);

	// Update
	void Update(float dt);
	void UpdateItemLights();
	void UpdateItemParticleEffects();
	void UpdateHoverItems();

	// Rendering
	void Render(bool outline, bool reflection, bool silhouette, bool shadow);
	void RenderDebug();

	// Constants
	static float ITEM_INTERACTION_DISTANCE;
	static float ITEM_INTERACTION_RADIUS_CHECK;

private:
	void AddItemSubSpawnData(ItemType sourceItem, BlockType sourceBlockType, ItemType spawnedItem, std::string spawnedItemFilename, std::string spawnedItemTitle, bool interactable, bool collectible, float scale,
		std::string droppedItemFileName, std::string droppedItemTextureFilename, InventoryType droppedItemInventoryType, ItemType droppedItemItem, ItemStatus droppedItemStatus, EquipSlot droppedItemEquipSlot, ItemQuality droppedItemQuality,
		bool droppedItemLeft, bool droppedItemRight, std::string droppedItemTitle, std::string droppedItemDescription, float droppedItemPlacementR, float droppedItemPlacementG, float droppedItemPlacementB, int droppedItemQuantity);

	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	LightingManager* m_pLightingManager;
	BlockParticleManager* m_pBlockParticleManager;
	Player* m_pPlayer;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	InventoryManager* m_pInventoryManager;
	NPCManager* m_pNPCManager;

	// Counters
	int m_numRenderItems;

	// Item list
	ItemList m_vpItemList;

	// Item spawner
	ItemSpawnerList m_vpItemSpawnerList;

	// Subspawn data, used to control which items spawn sub items
	ItemSubSpawnDataList m_vpItemSubSpawnDataList;
};

#endif