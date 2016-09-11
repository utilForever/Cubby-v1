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

#include <algorithm>

#include <Lighting/LightingManager.h>
#include <Maths/3DMaths.h>
#include <Models/VoxelObject.h>
#include <Utils/Random.h>

#include "Item.h"
#include <CubbyGame.h>

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
	m_droppedInventoryItem->m_iconFileName = iconFileName;
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
	SetDroppedItem(pItem->m_fileName.c_str(), pItem->m_iconFileName.c_str(), pItem->m_itemType, pItem->m_item, pItem->m_status, pItem->m_equipSlot, pItem->m_itemQuality,
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

bool Item::IsItemPickedUp() const
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
bool Item::IsStillAnimating() const
{
	for (int animatedSectionsIndex = 0; animatedSectionsIndex < m_pVoxelItem->GetNumAimatedSections(); ++animatedSectionsIndex)
	{
		if (m_pVoxelItem->HasSubSectionAnimationFinished(animatedSectionsIndex) == false)
		{
			return true;
		}
	}

	return false;
}

// Rendering helpers
void Item::SetOutlineRender(bool outline)
{
	m_outlineRender = outline;
}

bool Item::IsOutlineRender() const
{
	return m_outlineRender;
}

void Item::SetWireFrameRender(bool wireframe) const
{
	if (m_pVoxelItem != nullptr)
	{
		m_pVoxelItem->SetWireFrameRender(wireframe);
	}
}

void Item::CalculateWorldTransformMatrix()
{
	m_worldMatrix.LoadIdentity();
	m_worldMatrix.SetRotation(DegreeToRadian(m_rotation.x), DegreeToRadian(m_rotation.y), DegreeToRadian(m_rotation.z));
	m_worldMatrix.SetTranslation(m_position);

	for (size_t i = 0; i < m_vpBoundingRegionList.size(); ++i)
	{
		Matrix4 justParentRotation;
		justParentRotation.SetRotation(DegreeToRadian(m_rotation.x), DegreeToRadian(m_rotation.y), DegreeToRadian(m_rotation.z));

		m_vpBoundingRegionList[i]->UpdatePlanes(justParentRotation, m_renderScale);
	}
}

// Item type
ItemType Item::GetItemType() const
{
	return m_itemType;
}

// Item title
const char* Item::GetItemTitle() const
{
	return m_itemTitle.c_str();
}

// Should we create dying lights when we unload
void Item::SetCreateDyingLights(bool dyingLights)
{
	m_createDyingLights = dyingLights;
}

// Grid
void Item::UpdateGridPosition()
{
	int gridPositionX = static_cast<int>((m_position.x + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridPositionY = static_cast<int>((m_position.y + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridPositionZ = static_cast<int>((m_position.z + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);

	if (m_position.x <= -0.5f)
	{
		gridPositionX -= 1;
	}
	if (m_position.y <= -0.5f)
	{
		gridPositionY -= 1;
	}
	if (m_position.z <= -0.5f)
	{
		gridPositionZ -= 1;
	}

	if (gridPositionX != m_gridPositionX || gridPositionY != m_gridPositionY || gridPositionZ != m_gridPositionZ || m_pCachedGridChunk == nullptr)
	{
		m_gridPositionX = gridPositionX;
		m_gridPositionY = gridPositionY;
		m_gridPositionZ = gridPositionZ;

		m_pCachedGridChunk = m_pChunkManager->GetChunk(m_gridPositionX, m_gridPositionY, m_gridPositionZ);
	}
}

Chunk* Item::GetCachedGridChunkOrFromPosition(glm::vec3 pos) const
{
	// First check if the position is in the same grid as the cached chunk
	int gridPositionX = static_cast<int>((pos.x + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridPositionY = static_cast<int>((pos.y + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridPositionZ = static_cast<int>((pos.z + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);

	if (pos.x <= -0.5f)
	{
		gridPositionX -= 1;
	}
	if (pos.y <= -0.5f)
	{
		gridPositionY -= 1;
	}
	if (pos.z <= -0.5f)
	{
		gridPositionZ -= 1;
	}

	if (gridPositionX != m_gridPositionX || gridPositionY != m_gridPositionY || gridPositionZ != m_gridPositionZ)
	{
		return nullptr;
	}
	
	return m_pCachedGridChunk;
}

// Loot items
int Item::GetNumLootItems() const
{
	return static_cast<int>(m_vpInventoryItemList.size());
}

InventoryItem* Item::GetLootItem(int index)
{
	return m_vpInventoryItemList[index];
}

InventoryItem* Item::AddLootItem(InventoryItem* pItem, int slotX, int slotY)
{
	if(pItem != nullptr)
	{
		InventoryItem* pAddedLootItem = AddLootItem(pItem->m_fileName.c_str(), pItem->m_iconFileName.c_str(), pItem->m_itemType, pItem->m_item, pItem->m_status, pItem->m_equipSlot, pItem->m_itemQuality, pItem->m_title.c_str(), pItem->m_description.c_str(), pItem->m_left, pItem->m_right, pItem->m_placementR, pItem->m_placementG, pItem->m_placementB, pItem->m_quantity, slotX, slotY);

		for(size_t i = 0; i < pItem->m_vpStatAttributes.size(); ++i)
		{
			pAddedLootItem->AddStatAttribute(pItem->m_vpStatAttributes[i]->GetType(), pItem->m_vpStatAttributes[i]->GetModifyAmount());
		}

		return pAddedLootItem;
	}

	return nullptr;
}

InventoryItem* Item::AddLootItem(const char* fileName, const char* iconFileName, InventoryType itemType, ItemType item, ItemStatus status, EquipSlot equipSlot, ItemQuality itemQuality, const char* title, const char* description, bool left, bool right, float r, float g, float b, int quantity, int slotX, int slotY)
{
	bool addToExistingItem = false;

	if (quantity != -1)
	{
		addToExistingItem = true;
	}

	if (addToExistingItem)
	{
		for (size_t i = 0; i < m_vpInventoryItemList.size(); ++i)
		{
			if (strcmp(title, m_vpInventoryItemList[i]->m_title.c_str()) == 0)
			{
				m_vpInventoryItemList[i]->m_quantity += quantity;

				return m_vpInventoryItemList[i];
			}
		}
	}

	InventoryItem* pNewItem = new InventoryItem();

	pNewItem->m_fileName = fileName;
	pNewItem->m_iconFileName = iconFileName;
	pNewItem->m_title = title;
	pNewItem->m_description = description;

	pNewItem->m_itemType = itemType;
	pNewItem->m_item = item;
	pNewItem->m_status = status;
	pNewItem->m_equipSlot = equipSlot;
	pNewItem->m_itemQuality = itemQuality;

	pNewItem->m_left = left;
	pNewItem->m_right = right;

	pNewItem->m_placementR = r;
	pNewItem->m_placementG = g;
	pNewItem->m_placementB = b;

	pNewItem->m_lootSlotX = slotX;
	pNewItem->m_lootSlotY = slotY;

	pNewItem->m_equipped = false;

	pNewItem->m_quantity = quantity;

	pNewItem->m_remove = false;

	m_vpInventoryItemList.push_back(pNewItem);

	return pNewItem;
}

bool IsNeedErasing(InventoryItem* item)
{
	bool isNeedErase = item->m_remove;

	if (isNeedErase == true)
	{
		delete item;
	}

	return isNeedErase;
}

void Item::RemoveLootItem(InventoryItem* pInventoryItem)
{
	for(size_t i = 0; i < m_vpInventoryItemList.size(); ++i)
	{
		if(m_vpInventoryItemList[i] == pInventoryItem)
		{
			pInventoryItem->m_remove = true;
		}
	}

	m_vpInventoryItemList.erase(remove_if(m_vpInventoryItemList.begin(), m_vpInventoryItemList.end(), IsNeedErasing), m_vpInventoryItemList.end());
}

void Item::ClearLootItems()
{
	for(size_t i = 0; i < m_vpInventoryItemList.size(); ++i)
	{
		delete m_vpInventoryItemList[i];
		m_vpInventoryItemList[i] = nullptr;
	}
	m_vpInventoryItemList.clear();
}

InventoryItemList Item::GetLootItemList() const
{
	return m_vpInventoryItemList;
}

// Interaction position
void Item::SetInteractionPositionOffset(glm::vec3 offset)
{
	m_interactionPositionOffset = offset;
}

glm::vec3 Item::GetInteractionPosition() const
{
	return GetCenter() + m_interactionPositionOffset;
}

// World collision
void Item::SetWorldCollide(bool collide)
{
	m_worldCollide = collide;
}

bool Item::CheckCollisions(glm::vec3 positionCheck, glm::vec3 previousPosition, glm::vec3* pNormal, glm::vec3* pMovement) const
{
	float radius = GetRadius();

	glm::vec3 movementCache = *pMovement;

	// World collisions
	bool worldCollision = false;

	glm::vec3 floorPosition;

	if (m_pChunkManager->FindClosestFloor(positionCheck, &floorPosition) == false)
	{
		*pMovement = glm::vec3(0.0f, 0.0f, 0.0f);
		return true;
	}

	int blockX, blockY, blockZ;
	glm::vec3 blockPos;
	int numChecks = 1 + static_cast<int>(radius / (Chunk::BLOCK_RENDER_SIZE * 2.0f));

	for (int x = -numChecks; x <= numChecks; x++)
	{
		for (int y = -numChecks; y <= numChecks; y++)
		{
			for (int z = -numChecks; z <= numChecks; z++)
			{
				*pNormal = glm::vec3(0.0f, 0.0f, 0.0f);

				Chunk* pChunk = GetCachedGridChunkOrFromPosition(positionCheck + glm::vec3(Chunk::BLOCK_RENDER_SIZE * 2.0f * x, Chunk::BLOCK_RENDER_SIZE * 2.0f * y, Chunk::BLOCK_RENDER_SIZE * 2.0f * z));
				bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(positionCheck.x + (Chunk::BLOCK_RENDER_SIZE * 2.0f * x), positionCheck.y + (Chunk::BLOCK_RENDER_SIZE * 2.0f * y), positionCheck.z + (Chunk::BLOCK_RENDER_SIZE * 2.0f * z), &blockPos, &blockX, &blockY, &blockZ, &pChunk);

				if (active == false)
				{
					if (pChunk == nullptr || pChunk->IsSetup() == false)
					{
						*pMovement = glm::vec3(0.0f, 0.0f, 0.0f);
						worldCollision = false;
					}
				}
				else if (active == true)
				{
					Plane3D planes[6];
					planes[0] = Plane3D(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(Chunk::BLOCK_RENDER_SIZE, 0.0f, 0.0f));
					planes[1] = Plane3D(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-Chunk::BLOCK_RENDER_SIZE, 0.0f, 0.0f));
					planes[2] = Plane3D(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, Chunk::BLOCK_RENDER_SIZE, 0.0f));
					planes[3] = Plane3D(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -Chunk::BLOCK_RENDER_SIZE, 0.0f));
					planes[4] = Plane3D(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, Chunk::BLOCK_RENDER_SIZE));
					planes[5] = Plane3D(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -Chunk::BLOCK_RENDER_SIZE));

					float distance;
					int inside = 0;
					bool insideCache[6];

					for (int i = 0; i < 6; i++)
					{
						glm::vec3 pointToCheck = blockPos - previousPosition;
						distance = planes[i].GetPointDistance(pointToCheck);

						if (distance < -radius)
						{
							// Outside...
							insideCache[i] = false;
						}
						else if (distance < radius)
						{
							// Intersecting..
							insideCache[i] = true;
						}
						else
						{
							// Inside...
							insideCache[i] = true;
						}
					}

					for (int i = 0; i < 6; i++)
					{
						glm::vec3 pointToCheck = blockPos - positionCheck;
						distance = planes[i].GetPointDistance(pointToCheck);

						if (distance < -radius)
						{
							// Outside...
						}
						else if (distance < radius)
						{
							// Intersecting..
							inside++;
							if (insideCache[i] == false)
							{
								*pNormal += planes[i].m_normal;
							}
						}
						else
						{
							// Inside...
							inside++;
							if (insideCache[i] == false)
							{
								*pNormal += planes[i].m_normal;
							}
						}
					}

					if (inside == 6)
					{
						if (length(*pNormal) <= 1.0f)
						{
							if (length(*pNormal) > 0.0f)
							{
								*pNormal = normalize(*pNormal);
							}

							float dotResult = dot(*pNormal, *pMovement);
							*pNormal *= dotResult;

							*pMovement -= *pNormal;

							worldCollision = true;
						}
					}
				}
			}
		}
	}

	if (worldCollision)
	{
		return true;
	}

	*pMovement = movementCache;

	return false;
}

// Bounding collision region
void Item::CreateBoundingRegion(glm::vec3 origin, BoundingRegionType boundingType, float radius, float xWidth, float yWidth, float zWidth, float scale)
{
	if (m_vpBoundingRegionList.size() >= 1)
	{
		return;
	}

	SetCollisionEnabled(true);

	BoundingRegion* pNewRegion = new BoundingRegion();

	pNewRegion->m_origin = origin;
	pNewRegion->m_boundingType = boundingType;
	pNewRegion->m_radius = radius;
	pNewRegion->m_xLength = xWidth;
	pNewRegion->m_yLength = yWidth;
	pNewRegion->m_zLength = zWidth;
	pNewRegion->m_scale = scale;

	Matrix4 transformMatrix;
	pNewRegion->UpdatePlanes(transformMatrix, 1.0f);

	m_vpBoundingRegionList.push_back(pNewRegion);
}

void Item::UpdateBoundingRegion(int index, glm::vec3 origin, BoundingRegionType boundingType, float radius, float xWidth, float yWidth, float zWidth, float scale)
{
	if (m_vpBoundingRegionList.size() == 0)
	{
		CreateBoundingRegion(origin, boundingType, radius, xWidth, yWidth, zWidth, scale);
	}

	if (index >= static_cast<int>(m_vpBoundingRegionList.size()))
	{
		return;
	}

	BoundingRegion* pRegion = m_vpBoundingRegionList[index];

	pRegion->m_origin = origin;
	pRegion->m_boundingType = boundingType;
	pRegion->m_radius = radius;
	pRegion->m_xLength = xWidth;
	pRegion->m_yLength = yWidth;
	pRegion->m_zLength = zWidth;
	pRegion->m_scale = scale;

	Matrix4 transformMatrix;
	pRegion->UpdatePlanes(transformMatrix, 1.0f);
}

void Item::AddBoundingRegion(glm::vec3 origin, BoundingRegionType boundingType, float radius, float xWidth, float yWidth, float zWidth, float scale)
{
	BoundingRegion* pNewRegion = new BoundingRegion();

	pNewRegion->m_origin = origin;
	pNewRegion->m_boundingType = boundingType;
	pNewRegion->m_radius = radius;
	pNewRegion->m_xLength = xWidth;
	pNewRegion->m_yLength = yWidth;
	pNewRegion->m_zLength = zWidth;
	pNewRegion->m_scale = scale;

	Matrix4 transformMatrix;
	pNewRegion->UpdatePlanes(transformMatrix, 1.0f);

	m_vpBoundingRegionList.push_back(pNewRegion);
}

BoundingRegionList Item::GetBoundingRegionList() const
{
	return m_vpBoundingRegionList;
}

// Explode
void Item::Explode()
{
	CalculateWorldTransformMatrix();

	if (m_pVoxelItem != nullptr)
	{
		for (int animatedSectionsIndex = 0; animatedSectionsIndex < m_pVoxelItem->GetNumAimatedSections(); ++animatedSectionsIndex)
		{
			AnimatedSection* pAnimatedSection = m_pVoxelItem->GetAnimatedSection(animatedSectionsIndex);
			QubicleBinary* pQubicleModel = pAnimatedSection->pVoxelObject->GetQubicleModel();
			m_pBlockParticleManager->ExplodeQubicleBinary(pQubicleModel, m_renderScale, 100);
		}
	}
}

bool Item::IsColliding(glm::vec3 center, glm::vec3 previousCenter, float radius, glm::vec3* pNormal, glm::vec3* pMovement)
{
	bool colliding = false;

	center = GetCenter() - center;
	previousCenter = GetCenter() - previousCenter;

	*pNormal = glm::vec3(0.0f, 0.0f, 0.0f);

	for (size_t i = 0; i < m_vpBoundingRegionList.size(); ++i)
	{
		BoundingRegion* pRegion = m_vpBoundingRegionList[i];

		if (pRegion->m_boundingType == BoundingRegionType::Sphere)
		{
			glm::vec3 toRegion = center + (pRegion->m_origin * m_renderScale);
			float lengthSize = length(toRegion);

			if (lengthSize <= pRegion->m_radius * pRegion->m_scale + radius)
			{
				glm::vec3 toPrevious = previousCenter + pRegion->m_origin;
				*pNormal = -normalize(toPrevious);

				colliding = true;
			}
		}

		if (pRegion->m_boundingType == BoundingRegionType::Cube)
		{
			float distance;
			int inside = 0;
			bool insideCache[6];

			for (int j = 0; j < 6; j++)
			{
				distance = pRegion->m_planes[j].GetPointDistance(previousCenter + (pRegion->m_origin * m_renderScale));

				if (distance < -radius)
				{
					// Outside...
					insideCache[j] = false;
				}
				else if (distance < radius)
				{
					// Intersecting..
					insideCache[j] = true;
				}
				else
				{
					// Inside...
					insideCache[j] = true;
				}
			}

			for (int j = 0; j < 6; j++)
			{
				distance = pRegion->m_planes[j].GetPointDistance(center + (pRegion->m_origin * m_renderScale));

				if (distance < -radius)
				{
					// Outside...
				}
				else if (distance < radius)
				{
					// Intersecting..
					inside++;
					if (insideCache[j] == false)
					{
						*pNormal += pRegion->m_planes[j].m_normal;
					}
				}
				else
				{
					// Inside...
					inside++;
					if (insideCache[j] == false)
					{
						*pNormal += pRegion->m_planes[j].m_normal;
					}
				}
			}

			if (inside == 6)
			{
				if (length(*pNormal) <= 1.0f)
				{
					if (length(*pNormal) > 0.0f)
					{
						*pNormal = normalize(*pNormal);
					}

					colliding = true;
				}
			}
		}
	}

	if (colliding)
	{
		float dotResult = dot(*pNormal, *pMovement);
		*pNormal *= dotResult;

		*pMovement -= *pNormal;
	}

	return colliding;
}

void Item::SetCollisionEnabled(bool set)
{
	m_collisionEnabled = set;
}

bool Item::IsCollisionEnabled() const
{
	return m_collisionEnabled;
}

void Item::UpdateCollisionRadius()
{
	int numX = m_pVoxelItem->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix(0)->m_matrixSizeX;
	int numY = m_pVoxelItem->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix(0)->m_matrixSizeY;
	int numZ = m_pVoxelItem->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix(0)->m_matrixSizeZ;

	int max = numX;
	if (numY > max)
	{
		max = numY;
	}
	if (numZ > max)
	{
		max = numZ;
	}

	m_collisionRadius = max * (Chunk::BLOCK_RENDER_SIZE * 1.75f);
	m_collisionRadius *= m_renderScale;
	m_collisionRadius *= m_pVoxelItem->GetRenderScale();
}

float Item::GetCollisionRadius() const
{
	return m_collisionRadius;
}

// Interaction
void Item::SetInteractable(bool interactable)
{
	m_interactable = interactable;
}

bool Item::IsInteractable() const
{
	return m_interactable;
}

bool Item::IsInteracting() const
{
	return m_itemInteracting;
}

void Item::Interact()
{
	m_interactCount++;

	bool isNeedErase = false;
	bool spawnSubItems = false;
	bool createHitEffect = false;
	bool createCrumbleBlockParticles = false;

	if (m_interactCount < m_maxInteractCount)
	{
		bool changeItemModel = false;

		char itemFileName[64];
		if (m_itemType == ItemType::CopperVein)
		{
			changeItemModel = true;
			spawnSubItems = true;
			createHitEffect = true;
			createCrumbleBlockParticles = true;
			sprintf(itemFileName, "Resources/gamedata/items/CopperVein/CopperVein%i.item", m_interactCount);
		}
		if (m_itemType == ItemType::IronVein)
		{
			changeItemModel = true;
			spawnSubItems = true;
			createHitEffect = true;
			createCrumbleBlockParticles = true;
			sprintf(itemFileName, "Resources/gamedata/items/IronVein/IronVein%i.item", m_interactCount);
		}
		if (m_itemType == ItemType::SilverVein)
		{
			changeItemModel = true;
			spawnSubItems = true;
			createHitEffect = true;
			createCrumbleBlockParticles = true;
			sprintf(itemFileName, "Resources/gamedata/items/SilverVein/SilverVein%i.item", m_interactCount);
		}
		if (m_itemType == ItemType::GoldVein)
		{
			changeItemModel = true;
			spawnSubItems = true;
			createHitEffect = true;
			createCrumbleBlockParticles = true;
			sprintf(itemFileName, "Resources/gamedata/items/GoldVein/GoldVein%i.item", m_interactCount);
		}

		if (changeItemModel)
		{
			delete m_pVoxelItem;
			m_pVoxelItem = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
			m_pVoxelItem->SetVoxelCharacterParent(nullptr);
			m_pVoxelItem->LoadWeapon(itemFileName, false);
		}
	}
	else
	{
		if (m_itemType == ItemType::CopperVein || m_itemType == ItemType::IronVein || m_itemType == ItemType::SilverVein || m_itemType == ItemType::GoldVein)
		{
			isNeedErase = true;
			spawnSubItems = true;
			createHitEffect = true;
		}
	}

	// Chest open/close animation
	if (m_itemType == ItemType::Chest)
	{
		m_pVoxelItem->StartSubSectionAnimation();

		if (m_itemInteracting)
		{
			m_itemInteracting = false;
		}
		else
		{
			if (m_interactCount == m_maxInteractCount)
			{
				SpawnSubItems();
			}

			m_itemInteracting = true;
		}
	}

	// TODO: Sitting in chair
	//if(m_itemType == ItemType::Chair)
	//{
	//	m_pPlayer->SetPosition(m_position + vec3(0.0f, 0.01f, 0.0f));
	//	m_pPlayer->SetRotation(m_rotation.y);
	//	m_pPlayer->SetSitting(true);
	//}

	// Spawn sub items
	if (spawnSubItems)
	{
		SpawnSubItems();
	}

	// Crumble particle effects
	if (createCrumbleBlockParticles)
	{
		// Create some block particle effects for the crumbling ore veins
		int numCrubleBlocks = 10;
		for (int i = 0; i < numCrubleBlocks; i++)
		{
			float startScale = m_renderScale;
			float endScale = m_renderScale;
			startScale *= GetRandomNumber(90, 100, 2) * 0.01f;
			endScale *= GetRandomNumber(25, 75, 2) * 0.01f;
			float lifeTime = 4.0f + GetRandomNumber(-100, 200, 1) * 0.0075f;
			glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0f);
			glm::vec3 pointOrigin = glm::vec3(0.0f, 0.0f, 0.0f);
			float r = 0.49f; float g = 0.44f; float b = 0.44f; float a = 1.0f;
			glm::vec3 spawnPos = GetCenter() + glm::vec3(GetRandomNumber(-1, 1, 2) * 0.5f, GetRandomNumber(-1, 1, 2) * 0.5f, GetRandomNumber(-1, 1, 2) * 0.5f);
			
			BlockParticle* pParticle = m_pBlockParticleManager->CreateBlockParticle(spawnPos, spawnPos, gravity, 1.5f, pointOrigin, startScale, 0.0f, endScale, 0.0f, r, g, b, a, 0.0f, 0.0f, 0.0f, 0.0f, r, g, b, a, 0.0f, 0.0f, 0.0f, 0.0f, lifeTime, 0.0f, 0.0f, 0.0f, glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(1.85f, 3.0f, 1.85f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(180.0f, 180.0f, 180.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, true, glm::vec3(0.0f, 0.0f, 0.0f), true, false, false, false, nullptr);
			if (pParticle != nullptr)
			{
				pParticle->m_isAllowFloorSliding = true;
			}
		}
	}

	// Do a hit particle effect
	if (createHitEffect)
	{
		glm::vec3 direction = GetCenter() - m_pPlayer->GetCenter();
		glm::vec3 hitParticlePos = GetCenter() - (normalize(direction) * (m_radius*0.5f));
		unsigned int effectID = -1;
		
		BlockParticleEffect* pBlockParticleEffect = CubbyGame::GetInstance()->GetBlockParticleManager()->ImportParticleEffect("Resources/gamedata/particles/combat_hit.effect", hitParticlePos, &effectID);
		pBlockParticleEffect->PlayEffect();
	}

	if (isNeedErase)
	{
		// Explode the item
		Explode();

		// Erase
		SetErase(true);
	}
}

void Item::SpawnSubItems()
{
	int numItems = GetRandomNumber(2, 4);

	if (m_itemType == ItemType::Chest)
	{
		// Spawn more coins
		numItems += 3;
	}

	for (int i = 0; i < numItems; ++i)
	{
		float radius = 0.5f;
		float angle = DegreeToRadian(GetRandomNumber(0, 360, 1));
		glm::vec3 ItemPosition = GetCenter() + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);

		glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0f);
		gravity = normalize(gravity);
		
		Item* pItem;
	
		ItemSubSpawnData* pItemSubSpawnData = m_pItemManager->GetItemSubSpawnData(m_itemType);
		if (pItemSubSpawnData != nullptr)
		{
			pItem = m_pItemManager->CreateItem(GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), pItemSubSpawnData->m_spawnedItemFileName.c_str(), pItemSubSpawnData->m_spawnedItem, pItemSubSpawnData->m_spawnedItemTitle.c_str(), pItemSubSpawnData->m_interactable, pItemSubSpawnData->m_collectible, pItemSubSpawnData->m_scale);

			if (pItem != nullptr)
			{
				pItem->SetGravityDirection(gravity);
				glm::vec3 velocity = ItemPosition - GetCenter();
				pItem->SetVelocity(normalize(velocity) * static_cast<float>(GetRandomNumber(2, 4, 2)) + glm::vec3(0.0f, 9.5f + GetRandomNumber(-2, 4, 2), 0.0f));
				pItem->SetRotation(glm::vec3(0.0f, GetRandomNumber(0, 360, 2), 0.0f));
				pItem->SetAngularVelocity(glm::vec3(0.0f, 90.0f, 0.0f));

				if (pItemSubSpawnData->m_droppedItemItem != ItemType::Coin)
				{
					pItem->SetDroppedItem(pItemSubSpawnData->m_droppedItemFileName.c_str(), pItemSubSpawnData->m_droppedItemTextureFileName.c_str(), pItemSubSpawnData->m_droppedItemInventoryType, pItemSubSpawnData->m_droppedItemItem, pItemSubSpawnData->m_droppedItemStatus, pItemSubSpawnData->m_droppedItemEquipSlot, pItemSubSpawnData->m_droppedItemQuality, pItemSubSpawnData->m_droppedItemLeft, pItemSubSpawnData->m_droppedItemRight, pItemSubSpawnData->m_droppedItemTitle.c_str(), pItemSubSpawnData->m_droppedItemDescription.c_str(), pItemSubSpawnData->m_droppedItemPlacementR, pItemSubSpawnData->m_droppedItemPlacementG, pItemSubSpawnData->m_droppedItemPlacementB, pItemSubSpawnData->m_droppedItemQuantity);
				}

				pItem->SetAutoDisappear(20.0f + (GetRandomNumber(-20, 20, 1) * 0.2f));
			}
		}
	}
}

void Item::SetCurrentInteractCount(int currentInteract)
{
	m_interactCount = currentInteract;
}

int Item::GetCurrentInteractCount() const
{
	return m_interactCount;
}

void Item::SetMaxtInteractCount(int maxInteract)
{
	m_maxInteractCount = maxInteract;
}

int Item::GetMaxInteractCount() const
{
	return m_maxInteractCount;
}

// Update
void Item::Update(float dt)
{
	if (m_erase)
	{
		return;
	}

	if (m_pVoxelItem != nullptr)
	{
		m_pVoxelItem->Update(dt);
	}

	// Update grid position
	UpdateGridPosition();

	// Update timers
	UpdateTimers(dt);

	// Update player magnet
	UpdatePlayerMagnet();

	// If we don't belong to a chunk
	if (m_pCachedGridChunk == nullptr)
	{
		return;
	}

	// Make sure that an owning chunk knows about us
	if (m_pOwningChunk == nullptr || m_pOwningChunk->IsInsideChunk(m_position) == false)
	{
		if (m_pOwningChunk != nullptr)
		{
			m_pOwningChunk->RemoveItem(this);
		}

		m_pOwningChunk = m_pChunkManager->GetChunkFromPosition(m_position.x, m_position.y, m_position.z);

		if (m_pOwningChunk != nullptr)
		{
			m_pOwningChunk->AddItem(this);
		}

		return;
	}

	// Auto disappear
	if (m_autoDisappear)
	{
		if (m_autoDisappearTimer <= 0.0f)
		{
			SetErase(true);

			return;
		}
	}

	// Update physics
	UpdatePhysics(dt);
}

void Item::UpdatePhysics(float dt)
{
	glm::vec3 acceleration = (m_gravityDirection * 9.81f) * 4.0f;

	// Integrate velocity
	m_velocity += acceleration * dt;

	// Integrate angular velocity and rotation
	glm::vec3 angularAcceleration(0.0f, 0.0f, 0.0f);
	m_angularVelocity += angularAcceleration * dt;
	m_rotation += m_angularVelocity * dt;

	if (m_worldCollide)
	{
		int blockX, blockY, blockZ;
		glm::vec3 blockPos;

		// Check collision
		glm::vec3 velocityToUse = m_velocity;
		glm::vec3 velAmount = velocityToUse*dt;
		glm::vec3 pNormal;
		int numberDivision = 1;

		while (length(velAmount) >= 1.0f)
		{
			numberDivision++;
			velAmount = velocityToUse*(dt / numberDivision);
		}

		for (int i = 0; i < numberDivision; ++i)
		{
			float dtToUse = (dt / numberDivision) + ((dt / numberDivision) * i);
			glm::vec3 posToCheck = GetCenter() + velocityToUse*dtToUse;

			if (CheckCollisions(posToCheck, m_previousPosition, &pNormal, &velAmount))
			{
				// Reset velocity, we don't have any bounce
				m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				velocityToUse = glm::vec3(0.0f, 0.0f, 0.0f);
			}
		}

		// Integrate position
		m_position += velocityToUse * dt;

		// Owning chunks
		if (m_pOwningChunk != nullptr && m_pOwningChunk->IsSetup() && m_pOwningChunk->IsInsideChunk(m_position))
		{
			Chunk* pChunk = GetCachedGridChunkOrFromPosition(m_position);
			bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(m_position.x, m_position.y, m_position.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);

			if (active == true)
			{
				// Roll back the integration, since we will intersect the block otherwise
				m_position -= m_velocity * dt;

				m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			}
		}
		else
		{
			if (m_pOwningChunk != nullptr)
			{
				m_pOwningChunk->RemoveItem(this);
			}

			m_pOwningChunk = m_pChunkManager->GetChunkFromPosition(m_position.x, m_position.y, m_position.z);

			if (m_pOwningChunk != nullptr)
			{
				m_pOwningChunk->AddItem(this);
			}

			if (m_pOwningChunk == nullptr)
			{
				m_position -= m_velocity * dt;
				m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			}
		}
	}
	else
	{
		// Integrate position
		m_position += m_velocity * dt;
	}

	m_previousPosition = GetCenter();
}

void Item::UpdateTimers(float dt)
{
	if (m_isCollectible)
	{
		m_collectionDelay -= dt;
	}

	// Disappear for pickup
	if (m_disappear)
	{
		if (m_disappearTimer > 0.0f)
		{
			m_disappearTimer -= dt;
		}
	}

	// Auto disappear
	if (m_autoDisappear)
	{
		if (m_autoDisappearTimer > 0.0f)
		{
			m_autoDisappearTimer -= dt;
		}
	}
}

void Item::UpdatePlayerMagnet()
{
	if (IsItemPickedUp())
	{
		if (m_disappear)
		{
			m_renderScale = m_disappearScale;

			if (m_disappearTimer <= 0.0f)
			{
				if (m_disappearAnimationStarted == false)
				{
					Interpolator::GetInstance()->AddFloatInterpolation(&m_disappearScale, m_disappearScale, 0.0f, 0.5f, -100.0f, nullptr, _PickupAnimationFinished, this);

					m_disappearAnimationStarted = true;
				}
			}
		}
		else
		{
			glm::vec3 diff = m_pickupPos - m_position;
			float lengthSize = length(diff);

			if (lengthSize < 0.01f)
			{
				m_disappear = true;
				m_disappearTimer = m_disappearDelay;
				m_disappearScale = m_renderScale;
			}
			else
			{
				diff = normalize(diff);
				m_position += diff * lengthSize*0.25f;
			}
		}
	}
	else
	{
		if (IsCollectible())
		{
			if (m_droppedInventoryItem == nullptr || m_pInventoryManager->CanAddInventoryItem(GetItemTitle(), GetItemType(), 1))
			{
				float yAdditionalMaagnetOffset = 0.5f;

				// Magnet towards the player
				if (m_pPlayer->IsDead() == false && length(m_pPlayer->GetCenter() - GetCenter()) < m_pPlayer->GetRadius() + 4.0f)
				{
					glm::vec3 toPlayer = (m_pPlayer->GetCenter() + glm::vec3(0.0f, yAdditionalMaagnetOffset, 0.0f)) - GetCenter();
					SetGravityDirection(toPlayer);
					SetVelocity(normalize(toPlayer)*(20.0f / length(toPlayer)));
					SetWorldCollide(false);
				}
				else
				{
					SetGravityDirection(glm::vec3(0.0f, -1.0f, 0.0f));
					SetWorldCollide(true);
				}

				// Check if we have been picked up by the player
				if (m_pPlayer->IsDead() == false && length((m_pPlayer->GetCenter() + glm::vec3(0.0f, yAdditionalMaagnetOffset, 0.0f)) - GetCenter()) < m_pPlayer->GetRadius())
				{
					SetPickupGotoPosition(m_pPlayer->GetCenter() + glm::vec3(0.0f, 2.5f, 0.0f));
					SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
					SetGravityDirection(glm::vec3(0.0f, 0.0f, 0.0f));

					if (m_itemType == ItemType::Heart)
					{
						m_pPlayer->GiveHealth(10.0f);
					}

					if (m_itemType == ItemType::Coin)
					{
						m_pPlayer->GiveCoins(1);
					}

					if (m_droppedInventoryItem != nullptr)
					{
						m_pInventoryManager->AddInventoryItem(m_droppedInventoryItem, -1, -1);
					}
				}
			}
		}
	}
}

void Item::UpdateItemLights() const
{
	if (m_erase)
	{
		return;
	}

	if (m_pVoxelItem != nullptr)
	{
		for (int i = 0; i < m_pVoxelItem->GetNumLights(); ++i)
		{
			unsigned int lightID;
			glm::vec3 lightPos;
			float lightRadius;
			float lightDiffuseMultiplier;
			Color lightColor;
			bool connectedToSegment;

			m_pVoxelItem->GetLightParams(i, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

			if (lightID == -1)
			{
				m_pLightingManager->AddLight(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 1.0f, Color(1.0f, 1.0f, 1.0f, 1.0f), &lightID);
				m_pVoxelItem->SetLightingID(i, lightID);
			}

			if (connectedToSegment == false)
			{
				lightPos *= m_renderScale;

				// Rotate due to characters forward vector
				float rotationAngle = acos(dot(glm::vec3(0.0f, 0.0f, 1.0f), m_forward));
				if (m_forward.x < 0.0f)
				{
					rotationAngle = -rotationAngle;
				}
				Matrix4 rotationMatrix;
				rotationMatrix.SetRotation(0.0f, rotationAngle, 0.0f);
				lightPos = rotationMatrix * lightPos;

				// Translate to position
				lightPos += m_position;
			}

			m_pLightingManager->UpdateLightPosition(lightID, lightPos);
			m_pLightingManager->UpdateLightRadius(lightID, lightRadius * m_renderScale);
			m_pLightingManager->UpdateLightDiffuseMultiplier(lightID, lightDiffuseMultiplier);
			m_pLightingManager->UpdateLightColor(lightID, lightColor);
		}
	}
}

void Item::UpdateItemParticleEffects() const
{
	if (m_erase)
	{
		return;
	}

	if (m_pVoxelItem != nullptr)
	{
		for (int i = 0; i < m_pVoxelItem->GetNumParticleEffects(); ++i)
		{
			unsigned int particleEffectID;
			glm::vec3 particleEffectPos;
			std::string effectName;
			bool connectedToSegment;

			m_pVoxelItem->GetParticleEffectParams(i, &particleEffectID, &particleEffectPos, &effectName, &connectedToSegment);

			if (particleEffectID == -1)
			{
				m_pBlockParticleManager->ImportParticleEffect(effectName, particleEffectPos, &particleEffectID);
				m_pVoxelItem->SetParticleEffectID(i, particleEffectID);
			}

			if (connectedToSegment == false)
			{
				particleEffectPos *= m_renderScale;

				// Rotate due to characters forward vector
				float rotationAngle = acos(dot(glm::vec3(0.0f, 0.0f, 1.0f), m_forward));
				if (m_forward.x < 0.0f)
				{
					rotationAngle = -rotationAngle;
				}

				Matrix4 rotationMatrix;
				rotationMatrix.SetRotation(0.0f, rotationAngle, 0.0f);
				particleEffectPos = rotationMatrix * particleEffectPos;

				// Translate to position
				particleEffectPos += m_position;
			}

			m_pBlockParticleManager->UpdateParticleEffectPosition(particleEffectID, particleEffectPos, particleEffectPos);
		}
	}
}

// Rendering
void Item::Render(bool outline, bool reflection, bool silhouette) const
{
	if (m_erase == true)
	{
		return;
	}

	if (m_pOwningChunk == nullptr || m_pOwningChunk->IsUnloading())
	{
		return;
	}

	if (m_pVoxelItem != nullptr)
	{
		Color OutlineColor(1.0f, 1.0f, 0.0f, 1.0f);

		m_pRenderer->PushMatrix();
		m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);
		m_pRenderer->ScaleWorldMatrix(m_renderScale, m_renderScale, m_renderScale);
		m_pVoxelItem->Render(outline, reflection, silhouette, OutlineColor);
		m_pRenderer->PopMatrix();
	}
}

void Item::RenderDebug()
{
	if (m_erase == true)
	{
		return;
	}

	if (m_pOwningChunk == nullptr || m_pOwningChunk->IsUnloading())
	{
		return;
	}

	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);

	m_pRenderer->PushMatrix();

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->RotateWorldMatrix(GetRotation().x, GetRotation().y, GetRotation().z);

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->DrawSphere(m_radius, 20, 20);

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
	m_pRenderer->DrawSphere(m_collisionRadius, 20, 20);

	m_pRenderer->PopMatrix();

	m_pRenderer->PopMatrix();

	// Render interaction point
	m_pRenderer->PushMatrix();

	float length = 0.5f;
	m_pRenderer->TranslateWorldMatrix(GetInteractionPosition().x, GetInteractionPosition().y, GetInteractionPosition().z);

	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->SetLineWidth(2.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(-length, 0.0f, 0.0f);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(length, 0.0f, 0.0f);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, -length, 0.0f);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, length, 0.0f);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, -length);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, length);

	m_pRenderer->DisableImmediateMode();

	m_pRenderer->PopMatrix();

	// Render link to owning chunk
	if (m_pOwningChunk != nullptr)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);
		m_pRenderer->ImmediateVertex(m_position.x, m_position.y, m_position.z);
		m_pRenderer->ImmediateVertex(m_pOwningChunk->GetPosition().x + Chunk::BLOCK_RENDER_SIZE*Chunk::CHUNK_SIZE, m_pOwningChunk->GetPosition().y + Chunk::BLOCK_RENDER_SIZE*Chunk::CHUNK_SIZE, m_pOwningChunk->GetPosition().z + Chunk::BLOCK_RENDER_SIZE*Chunk::CHUNK_SIZE);
		m_pRenderer->DisableImmediateMode();

		m_pRenderer->PopMatrix();
	}

	// Render collision regions
	RenderCollisionRegions();
}

void Item::RenderCollisionRegions()
{
	for (size_t i = 0; i < m_vpBoundingRegionList.size(); ++i)
	{
		BoundingRegion* pRegion = m_vpBoundingRegionList[i];

		m_pRenderer->PushMatrix();
		m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);

		Matrix4 justParentRotation;
		justParentRotation.SetRotation(DegreeToRadian(m_rotation.x), DegreeToRadian(m_rotation.y), DegreeToRadian(m_rotation.z));
		m_pRenderer->MultiplyWorldMatrix(justParentRotation);

		m_pRenderer->ScaleWorldMatrix(m_renderScale, m_renderScale, m_renderScale);
		pRegion->Render(m_pRenderer);
		m_pRenderer->PopMatrix();
	}
}

void Item::_PickupAnimationFinished(void* pData)
{
	Item* pItem = static_cast<Item*>(pData);
	pItem->PickupAnimationFinished();
}

void Item::PickupAnimationFinished()
{
	SetErase(true);
}