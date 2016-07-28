/*************************************************************************
> File Name: Item.cpp
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

#include <Lighting/LightingManager.h>
#include <Models/VoxelObject.h>

#include "Item.h"

// Constructor, Destructor
Item::Item(Renderer* pRenderer, ChunkManager* pChunkManager, QubicleBinaryManager* pQubicleBinaryManager, std::string itemTitle, ItemType itemType, float scale) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager), m_pLightingManager(nullptr),
	m_pBlockParticleManager(nullptr), m_pPlayer(nullptr), m_pItemManager(nullptr),
	m_pQubicleBinaryManager(pQubicleBinaryManager), m_pInventoryManager(nullptr),
	m_erase(false), m_itemTitle(itemTitle), m_fileName(""), m_forward(glm::vec3(0.0f, 0.0f, 1.0f)),
	m_pVoxelItem(nullptr), m_droppedInventoryItem(nullptr), m_itemType(itemType),
	m_renderScale(scale), m_outlineRender(false), m_radius(GetItemRadiusForType(m_itemType)),
	m_worldCollide(true), m_itemPickup(false),
	m_disappear(false), m_disappearDelay(0.5f), m_disappearTimer(0.5f), m_disappearAnimationStarted(false), m_disappearScale(scale),
	m_autoDisappear(false), m_autoDisappearTimer(0.0f),
	m_isCollectible(false), m_collectionDelay(0.0f), m_interactable(true), m_itemInteracting(false),
	m_interactCount(0), m_maxInteractCount(1), m_createDyingLights(true), m_pParentItemSpawner(nullptr),
	m_gridPositionX(0), m_gridPositionY(0), m_gridPositionZ(0),
	m_pCachedGridChunk(nullptr), m_pOwningChunk(nullptr),
	m_collisionEnabled(false), m_collisionRadius(1.0f)
{

}

Item::~Item()
{
	// If we belong to a spawner, make sure we indicate that we were killed
	if (m_pParentItemSpawner != nullptr)
	{
		m_pParentItemSpawner->RemoveItemFromThisSpawner();
	}

	UnloadEffectsAndLights();

	// Delete the voxel item
	if (m_pVoxelItem != nullptr)
	{
		delete m_pVoxelItem;
	}

	// Remove us from an owning chunk
	if (m_pOwningChunk != nullptr)
	{
		m_pOwningChunk->RemoveItem(this);
	}

	// Clear the dropped item
	if (m_droppedInventoryItem != nullptr)
	{
		delete m_droppedInventoryItem;
	}

	// Clear the bounding region data
	for (size_t i = 0; i < m_vpBoundingRegionList.size(); ++i)
	{
		delete m_vpBoundingRegionList[i];
		m_vpBoundingRegionList[i] = nullptr;
	}
	m_vpBoundingRegionList.clear();
}

// Unloading
void Item::UnloadEffectsAndLights() const
{
	// Lights
	for (size_t i = 0; i < m_pVoxelItem->GetNumLights(); ++i)
	{
		unsigned int lightID;
		glm::vec3 lightPos;
		float lightRadius;
		float lightDiffuseMultiplier;
		Color lightColor;
		bool connectedToSegment;

		m_pVoxelItem->GetLightParams(i, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

		if (lightID != -1)
		{
			m_pLightingManager->RemoveLight(lightID);
			m_pVoxelItem->SetLightingID(i, -1);

			lightPos += m_position;

			if (m_itemType != ItemType::DroppedItem)
			{
				if (m_createDyingLights)
				{
					unsigned int id;
					m_pLightingManager->AddDyingLight(lightPos, lightRadius, lightDiffuseMultiplier, lightColor, 3.5f, &id);
				}
			}
		}
	}

	// Particle Effects
	for (size_t i = 0; i < m_pVoxelItem->GetNumParticleEffects(); ++i)
	{
		unsigned int particleEffectID;
		glm::vec3 particleEffectPos;
		std::string effectName;
		bool connectedToSegment;

		m_pVoxelItem->GetParticleEffectParams(i, &particleEffectID, &particleEffectPos, &effectName, &connectedToSegment);

		if (particleEffectID != -1)
		{
			m_pBlockParticleManager->DestroyParticleEffect(particleEffectID);
			m_pVoxelItem->SetParticleEffectID(i, -1);
		}
	}
}

void Item::SetLightingManager(LightingManager* pLightingManager)
{
	m_pLightingManager = pLightingManager;
}

void Item::SetBlockParticleManager(BlockParticleManager* pBlockParticleManager)
{
	m_pBlockParticleManager = pBlockParticleManager;
}

void Item::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

void Item::SetItemManager(ItemManager* pItemManager)
{
	m_pItemManager = pItemManager;
}

void Item::SetInventoryManager(InventoryManager* pInventoryManager)
{
	m_pInventoryManager = pInventoryManager;
}

void Item::SetErase(bool erase)
{
	m_erase = erase;
}

bool Item::IsNeedErase() const
{
	return m_erase;
}

std::string Item::GetFileName() const
{
	return m_fileName;
}

void Item::SetChunk(Chunk* pChunk)
{
	m_pOwningChunk = pChunk;
}

Chunk* Item::GetChunk() const
{
	return m_pOwningChunk;
}

VoxelWeapon* Item::GetVoxelItem() const
{
	return m_pVoxelItem;
}

// Setup
void Item::LoadItem(const char* objectFileName)
{
	if (m_pVoxelItem == nullptr)
	{
		m_pVoxelItem = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
	}

	m_pVoxelItem->SetVoxelCharacterParent(nullptr);
	m_pVoxelItem->LoadWeapon(objectFileName);

	m_fileName = objectFileName;

	if (m_pVoxelItem != nullptr)
	{
		// Collision bounding region		
		int numX = m_pVoxelItem->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix(0)->m_matrixSizeX;
		int numY = m_pVoxelItem->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix(0)->m_matrixSizeY;
		int numZ = m_pVoxelItem->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix(0)->m_matrixSizeZ;
		CreateBoundingRegion(glm::vec3(0.0f, 0.0f, 0.0f), BoundingRegionType::Cube, 0.0f, numX * 0.5f, numY * 0.5f, numZ * 0.5f, 1.0);

		UpdateCollisionRadius();

		// Lights
		for (size_t i = 0; i < m_pVoxelItem->GetNumLights(); ++i)
		{
			unsigned int lightID;
			glm::vec3 lightPos;
			float lightRadius;
			float lightDiffuseMultiplier;
			Color lightColor;
			bool connectedToSegment;

			m_pVoxelItem->GetLightParams(i, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

			if (lightID != -1)
			{
				m_pLightingManager->RemoveLight(lightID);
				m_pVoxelItem->SetLightingID(i, -1);
			}
		}

		// Particle effects
		for (size_t i = 0; i < m_pVoxelItem->GetNumParticleEffects(); ++i)
		{
			unsigned int particleEffectID;
			glm::vec3 particleEffectPos;
			std::string effectName;
			bool connectedToSegment;

			m_pVoxelItem->GetParticleEffectParams(i, &particleEffectID, &particleEffectPos, &effectName, &connectedToSegment);

			if (particleEffectID != -1)
			{
				m_pBlockParticleManager->DestroyParticleEffect(particleEffectID);
				m_pVoxelItem->SetParticleEffectID(i, -1);
			}
		}
	}
}

// Item spawner
void Item::SetItemSpawner(ItemSpawner* pSpawner)
{
	m_pParentItemSpawner = pSpawner;
}

void Item::RemoveItemSpawner(ItemSpawner* pSpawner)
{
	if (m_pParentItemSpawner == pSpawner)
	{
		m_pParentItemSpawner = nullptr;
	}
}

// Accessors / Setters
void Item::SetPosition(glm::vec3 pos)
{
	m_position = pos;
}

glm::vec3 Item::GetPosition() const
{
	return m_position;
}

void Item::SetVelocity(glm::vec3 vel)
{
	m_velocity = vel;
}

glm::vec3 Item::GetVelocity() const
{
	return m_velocity;
}

void Item::SetRotation(glm::vec3 rot)
{
	m_rotation = rot;
}

glm::vec3 Item::GetRotation() const
{
	return m_rotation;
}

void Item::SetAngularVelocity(glm::vec3 angvel)
{
	m_angularVelocity = angvel;
}

glm::vec3 Item::GetAngularVelocity() const
{
	return m_angularVelocity;
}

void Item::SetGravityDirection(glm::vec3 dir)
{
	m_gravityDirection = dir;

	if (length(m_gravityDirection) >= 0.0001f)
	{
		m_gravityDirection = normalize(m_gravityDirection);
	}
}

glm::vec3 Item::GetGravityDirection() const
{
	return m_gravityDirection;
}

float Item::GetScale() const
{
	return m_renderScale;
}

float Item::GetRadius() const
{
	return m_radius;
}

glm::vec3 Item::GetCenter() const
{
	glm::vec3 center = m_position + (m_pVoxelItem->GetCenter() * GetScale());
	return center;
}

// Inventory item params
void Item::SetDroppedItem(const char* fileName, const char* iconFileName, InventoryType itemType, ItemType item, ItemStatus status, EquipSlot equipSlot, ItemQuality itemQuality, bool left, bool right, const char* title, const char* description, float r, float g, float b, int quantity)
{
	if (m_droppedInventoryItem == nullptr)
	{
		m_droppedInventoryItem = new InventoryItem();
	}

	m_droppedInventoryItem->m_fileName = fileName;
	m_droppedInventoryItem->m_IconfileName = iconFileName;
	m_droppedInventoryItem->m_title = title;
	m_droppedInventoryItem->m_description = description;

	m_droppedInventoryItem->m_itemType = itemType;

	m_droppedInventoryItem->m_item = item;

	m_droppedInventoryItem->m_status = status;

	m_droppedInventoryItem->m_equipSlot = equipSlot;

	m_droppedInventoryItem->m_itemQuality = itemQuality;

	m_droppedInventoryItem->m_left = left;
	m_droppedInventoryItem->m_right = right;

	m_droppedInventoryItem->m_placementR = r;
	m_droppedInventoryItem->m_placementG = g;
	m_droppedInventoryItem->m_placementB = b;

	m_droppedInventoryItem->m_lootSlotX = -1;
	m_droppedInventoryItem->m_lootSlotY = -1;

	m_droppedInventoryItem->m_equipped = false;

	m_droppedInventoryItem->m_scale = 1.0f;
	m_droppedInventoryItem->m_offsetX = 0.0f;
	m_droppedInventoryItem->m_offsetY = 0.0f;
	m_droppedInventoryItem->m_offsetZ = 0.0f;

	m_droppedInventoryItem->m_quantity = quantity;

	m_droppedInventoryItem->m_remove = false;
}

void Item::SetDroppedItem(InventoryItem* pItem)
{
	SetDroppedItem(pItem->m_fileName.c_str(), pItem->m_IconfileName.c_str(), pItem->m_itemType, pItem->m_item, pItem->m_status, pItem->m_equipSlot, pItem->m_itemQuality,
		pItem->m_left, pItem->m_right, pItem->m_title.c_str(), pItem->m_description.c_str(), pItem->m_placementR, pItem->m_placementG, pItem->m_placementB, pItem->m_quantity);
}

InventoryItem* Item::GetDroppedInventoryItem() const
{
	return m_droppedInventoryItem;
}

// Collectible
bool Item::IsCollectible() const
{
	return m_isCollectible && m_collectionDelay <= 0.0f;
}

void Item::SetIsCollectible(bool collect)
{
	m_isCollectible = collect;
}

void Item::SetCollectionDelay(float delay)
{
	m_collectionDelay = delay;
}

bool Item::IsItemPickedUp()
{
	return m_itemPickup;
}

void Item::SetPickupGotoPosition(glm::vec3 pickupPos)
{
	m_autoDisappear = false;
	m_itemPickup = true;
	m_pickupPos = pickupPos;
}

// Auto disappear
void Item::SetAutoDisappear(float disappearTime)
{
	m_autoDisappear = true;
	m_autoDisappearTimer = disappearTime;
}

// Animation
bool Item::IsStillAnimating()
{
	for (int animatedSectionsIndex = 0; animatedSectionsIndex < m_pVoxelItem->GetNumAimatedSections(); animatedSectionsIndex++)
	{
		if (m_pVoxelItem->HasSubSectionAnimationFinished(animatedSectionsIndex) == false)
		{
			return true;
		}
	}

	return false;
}