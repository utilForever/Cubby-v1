/*************************************************************************
> File Name: Item.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    An item object contains all the data and functionality associated with
>    the items that are used in the game, this includes items that are stored
>    and equipped by the player, as well as items that are dropped in the world.
>    Items are interactable by the players and also have physics and collisions.
> Created Time: 2016/07/24
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ITEM_H
#define CUBBY_ITEM_H

#include <Blocks/ChunkManager.h>
#include <Inventory/InventoryManager.h>
#include <Maths/BoundingRegion.h>
#include <Models/VoxelWeapon.h>
#include <Particles/BlockParticleManager.h>

#include "ItemsEnum.h"
#include "EquipmentEnum.h"

// Forward declaration
class LightingManager;
class ItemManager;
class ItemSpawner;

// Item helper functionality
std::string GetItemTitleForType(ItemType type);
std::string GetItemDescriptionForType(ItemType type);
float GetItemRadiusForType(ItemType type);
std::string GetItemFileNameForType(ItemType type);
std::string GetItemTextureForType(ItemType type);

// Equipment helper functionality
std::string GetEquipmentTitleForType(EquipmentType type);
std::string GetEquipmentDescriptionForType(EquipmentType type);
std::string GetEquipmentFileNameForType(EquipmentType type);
std::string GetEquipmentTextureForType(EquipmentType type);
InventoryType GetInventoryTypeForEquipment(EquipmentType type);
EquipSlot GetEquipSlotForEquipment(EquipmentType type);
ItemQuality GetItemQualityForEquipment(EquipmentType type);
void GetItemSidesForEquipment(EquipmentType type, bool* left, bool* right);
void AddStatsModifiersForType(EquipmentType type, InventoryItem* pItem);

class Item
{
public:
	// Constructor, Destructor
	Item(Renderer* pRenderer, ChunkManager* pChunkManager, QubicleBinaryManager* pQubicleBinaryManager, std::string itemTitle, ItemType itemType, float scale);
	~Item();

	// Unloading
	void UnloadEffectsAndLights() const;

	void SetLightingManager(LightingManager* pLightingManager);
	void SetBlockParticleManager(BlockParticleManager* pBlockParticleManager);
	void SetPlayer(Player* pPlayer);
	void SetItemManager(ItemManager* pItemManager);
	void SetInventoryManager(InventoryManager* pInventoryManager);

	void SetErase(bool erase);
	bool IsNeedErase() const;

	std::string GetFileName() const;

	void SetChunk(Chunk* pChunk);
	Chunk* GetChunk() const;

	VoxelWeapon* GetVoxelItem() const;

	// Setup
	void LoadItem(const char* objectFileName);

	// Item spawner
	void SetItemSpawner(ItemSpawner* pSpawner);
	void RemoveItemSpawner(ItemSpawner* pSpawner);

	// Accessors / Setters
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition() const;
	void SetVelocity(glm::vec3 vel);
	glm::vec3 GetVelocity() const;
	void SetRotation(glm::vec3 rot);
	glm::vec3 GetRotation() const;
	void SetAngularVelocity(glm::vec3 angvel);
	glm::vec3 GetAngularVelocity() const;
	void SetGravityDirection(glm::vec3 dir);
	glm::vec3 GetGravityDirection() const;
	float GetScale() const;
	float GetRadius() const;
	glm::vec3 GetCenter() const;

	// Inventory item params
	void SetDroppedItem(const char* fileName, const char* iconFileName, InventoryType itemType, ItemType item, ItemStatus status, EquipSlot equipSlot, ItemQuality itemQuality, bool left, bool right, const char* title, const char* description, float r, float g, float b, int quantity);
	void SetDroppedItem(InventoryItem* pItem);
	InventoryItem* GetDroppedInventoryItem() const;

	// Collectible
	bool IsCollectible() const;
	void SetIsCollectible(bool collect);
	void SetCollectionDelay(float delay);
	bool IsItemPickedUp() const;
	void SetPickupGotoPosition(glm::vec3 pickupPos);

	// Auto disappear
	void SetAutoDisappear(float disappearTime);

	// Animation
	bool IsStillAnimating() const;

	// Rendering helpers
	void SetOutlineRender(bool outline);
	bool IsOutlineRender() const;
	void SetWireFrameRender(bool wireframe) const;
	void CalculateWorldTransformMatrix();

	// Loot items
	int GetNumLootItems() const;
	InventoryItem* GetLootItem(int index);
	InventoryItem* AddLootItem(InventoryItem* pItem, int slotX, int slotY);
	InventoryItem* AddLootItem(const char* fileName, const char* iconFileName, InventoryType itemType, ItemType item, ItemStatus status, EquipSlot equipSlot, ItemQuality itemQuality, const char* title, const char* description, bool left, bool right, float r, float g, float b, int quantity, int slotX, int slotY);
	void RemoveLootItem(InventoryItem* pInventoryItem);
	void ClearLootItems();
	InventoryItemList GetLootItemList() const;

	// Item type
	ItemType GetItemType() const;

	// Item title
	const char* GetItemTitle() const;

	// Should we create dying lights when we unload
	void SetCreateDyingLights(bool dyingLights);

	// Grid
	void UpdateGridPosition();
	Chunk* GetCachedGridChunkOrFromPosition(glm::vec3 pos) const;

	// Interaction position
	void SetInteractionPositionOffset(glm::vec3 offset);
	glm::vec3 GetInteractionPosition() const;

	// World collision
	void SetWorldCollide(bool collide);
	bool CheckCollisions(glm::vec3 positionCheck, glm::vec3 previousPosition, glm::vec3* pNormal, glm::vec3* pMovement) const;

	// Bounding collision region
	void CreateBoundingRegion(glm::vec3 origin, BoundingRegionType boundingType, float radius, float xWidth, float yWidth, float zWidth, float scale);
	void UpdateBoundingRegion(int index, glm::vec3 origin, BoundingRegionType boundingType, float radius, float xWidth, float yWidth, float zWidth, float scale);
	void AddBoundingRegion(glm::vec3 origin, BoundingRegionType boundingType, float radius, float xWidth, float yWidth, float zWidth, float scale);
	BoundingRegionList GetBoundingRegionList() const;
	bool IsColliding(glm::vec3 center, glm::vec3 previousCenter, float radius, glm::vec3* pNormal, glm::vec3* pMovement);
	void SetCollisionEnabled(bool set);
	bool IsCollisionEnabled() const;
	void UpdateCollisionRadius();
	float GetCollisionRadius() const;

	// Explode
	void Explode();

	// Interaction
	void SetInteractable(bool interactable);
	bool IsInteractable() const;
	bool IsInteracting() const;
	void Interact();
	void SpawnSubItems();
	void SetCurrentInteractCount(int currentInteract);
	int GetCurrentInteractCount() const;
	void SetMaxtInteractCount(int maxInteract);
	int GetMaxInteractCount() const;

	// Update
	void Update(float dt);
	void UpdatePhysics(float dt);
	void UpdateTimers(float dt);
	void UpdatePlayerMagnet();
	void UpdateItemLights() const;
	void UpdateItemParticleEffects() const;

	// Rendering
	void Render(bool outline, bool reflection, bool silhouette) const;
	void RenderDebug();
	void RenderCollisionRegions();

protected:
	static void _PickupAnimationFinished(void* pData);
	void PickupAnimationFinished();

private:
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	LightingManager* m_pLightingManager;
	BlockParticleManager* m_pBlockParticleManager;
	Player* m_pPlayer;
	ItemManager* m_pItemManager;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	InventoryManager* m_pInventoryManager;

	// Erase flag
	bool m_erase;

	// Item title
	std::string m_itemTitle;

	// Filename
	std::string m_fileName;

	// Forward vector
	glm::vec3 m_forward;

	// Position
	glm::vec3 m_position;

	// Previous position
	glm::vec3 m_previousPosition;

	// Velocity
	glm::vec3 m_velocity;

	// Rotation
	glm::vec3 m_rotation;

	// Angular velocity
	glm::vec3 m_angularVelocity;

	// Gravity
	glm::vec3 m_gravityDirection;

	// Item world matrix
	Matrix4 m_worldMatrix;

	// The item's qubicle class
	VoxelWeapon* m_pVoxelItem;

	// The items that are stored in this item if it is a chest
	InventoryItemList m_vpInventoryItemList;

	// For when an item is pickup up from the floor, this contains the params to make it into an inventory item
	InventoryItem* m_droppedInventoryItem;

	// Item type
	ItemType m_itemType;

	// 	Scale
	float m_renderScale;

	// Are we rendering outline for this item
	bool m_outlineRender;

	// Radius
	float m_radius;

	// Should we collide with the world
	bool m_worldCollide;

	// Interaction position
	glm::vec3 m_interactionPositionOffset;

	// Pickup animation variables
	bool m_itemPickup;
	glm::vec3 m_pickupPos;

	// Disappear animation, used when we pickup items
	bool m_disappear;
	float m_disappearDelay;
	float m_disappearTimer;
	bool m_disappearAnimationStarted;
	float m_disappearScale;

	// Auto dissapear;
	bool m_autoDisappear;
	float m_autoDisappearTimer;

	// Is this item collectible by the player
	bool m_isCollectible;

	// The time delay before this item is collectible by the player
	float m_collectionDelay;

	// Are we able to be interactable?
	bool m_interactable;

	// Are we currently interacting with this item
	// Are we currently interacting with this item
	bool m_itemInteracting;

	// Interact counting... i.e. mining takes more than one hit to destroy.
	int m_interactCount;
	int m_maxInteractCount;

	// Should we create dying lights when we unload the item?
	bool m_createDyingLights;

	// Were we created from an item spawner
	ItemSpawner* m_pParentItemSpawner;

	// Grid position
	int m_gridPositionX;
	int m_gridPositionY;
	int m_gridPositionZ;

	// Cached grid chunk
	Chunk* m_pCachedGridChunk;

	// Owning chunk
	Chunk* m_pOwningChunk;

	// Bounding region list
	BoundingRegionList m_vpBoundingRegionList;
	bool m_collisionEnabled;
	float m_collisionRadius;
};

#endif