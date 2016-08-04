/*************************************************************************
> File Name: ItemManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The item manager is in charge of managing and all the items in the game,
>    including creation, destruction, updating and rendering the items.
>    Different rendering modes are also supported for items depending on their
>    state.
> Created Time: 2016/08/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include <Utils/Random.h>

#include "ItemManager.h"
#include <CubbyGame.h>

// Constants
float ItemManager::ITEM_INTERACTION_DISTANCE = 2.5f;
float ItemManager::ITEM_INTERACTION_RADIUS_CHECK = 0.75f;

// Constructor, Destructor
ItemManager::ItemManager(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager), m_pLightingManager(nullptr),
	m_pBlockParticleManager(nullptr), m_pPlayer(pPlayer),
	m_pQubicleBinaryManager(nullptr), m_pInventoryManager(nullptr),
	m_numRenderItems(0)
{
	// Chest
	AddItemSubSpawnData(ItemType::Chest, BlockType::Default, ItemType::Coin, GetItemFileNameForType(ItemType::Coin), GetItemTitleForType(ItemType::Coin), false, true, 0.0225f,
		GetItemFileNameForType(ItemType::Coin), GetItemTextureForType(ItemType::Coin), InventoryType::Item, ItemType::Coin, ItemStatus::None, EquipSlot::NoSlot, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::Coin), GetItemDescriptionForType(ItemType::Coin), 1.0f, 1.0f, 1.0f, 1);

	// Ores
	AddItemSubSpawnData(ItemType::CopperVein, BlockType::Default, ItemType::CopperOre, GetItemFileNameForType(ItemType::CopperOre), GetItemTitleForType(ItemType::CopperOre), false, true, 0.08f,
		GetItemFileNameForType(ItemType::CopperOre), GetItemTextureForType(ItemType::CopperOre), InventoryType::Item, ItemType::CopperOre, ItemStatus::None, EquipSlot::NoSlot, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::CopperOre), GetItemDescriptionForType(ItemType::CopperOre), 1.0f, 1.0f, 1.0f, 1);
	AddItemSubSpawnData(ItemType::IronVein, BlockType::Default, ItemType::IronOre, GetItemFileNameForType(ItemType::IronOre), GetItemTitleForType(ItemType::IronOre), false, true, 0.08f,
		GetItemFileNameForType(ItemType::IronOre), GetItemTextureForType(ItemType::IronOre), InventoryType::Item, ItemType::IronOre, ItemStatus::None, EquipSlot::NoSlot, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::IronOre), GetItemDescriptionForType(ItemType::IronOre), 1.0f, 1.0f, 1.0f, 1);
	AddItemSubSpawnData(ItemType::SilverVein, BlockType::Default, ItemType::SilverOre, GetItemFileNameForType(ItemType::SilverOre), GetItemTitleForType(ItemType::SilverOre), false, true, 0.08f,
		GetItemFileNameForType(ItemType::SilverOre), GetItemTextureForType(ItemType::SilverOre), InventoryType::Item, ItemType::SilverOre, ItemStatus::None, EquipSlot::NoSlot, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::SilverOre), GetItemDescriptionForType(ItemType::SilverOre), 1.0f, 1.0f, 1.0f, 1);
	AddItemSubSpawnData(ItemType::GoldVein, BlockType::Default, ItemType::GoldOre, GetItemFileNameForType(ItemType::GoldOre), GetItemTitleForType(ItemType::GoldOre), false, true, 0.08f,
		GetItemFileNameForType(ItemType::GoldOre), GetItemTextureForType(ItemType::GoldOre), InventoryType::Item, ItemType::GoldOre, ItemStatus::None, EquipSlot::NoSlot, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::GoldOre), GetItemDescriptionForType(ItemType::GoldOre), 1.0f, 1.0f, 1.0f, 1);

	// Blocks
	AddItemSubSpawnData(ItemType::None, BlockType::Grass, ItemType::BlockGrass, GetItemFileNameForType(ItemType::BlockGrass), GetItemTitleForType(ItemType::BlockGrass), false, true, 0.06f,
		GetItemFileNameForType(ItemType::BlockGrass), GetItemTextureForType(ItemType::BlockGrass), InventoryType::Block, ItemType::BlockGrass, ItemStatus::None, EquipSlot::RightHand, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::BlockGrass), GetItemDescriptionForType(ItemType::BlockGrass), 0.23f, 0.13f, 0.02f, 1);
	AddItemSubSpawnData(ItemType::None, BlockType::Dirt, ItemType::BlockDirt, GetItemFileNameForType(ItemType::BlockDirt), GetItemTitleForType(ItemType::BlockDirt), false, true, 0.06f,
		GetItemFileNameForType(ItemType::BlockDirt), GetItemTextureForType(ItemType::BlockDirt), InventoryType::Block, ItemType::BlockDirt, ItemStatus::None, EquipSlot::RightHand, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::BlockDirt), GetItemDescriptionForType(ItemType::BlockDirt), 0.23f, 0.13f, 0.02f, 1);
	AddItemSubSpawnData(ItemType::None, BlockType::Stone, ItemType::BlockStone, GetItemFileNameForType(ItemType::BlockStone), GetItemTitleForType(ItemType::BlockStone), false, true, 0.06f,
		GetItemFileNameForType(ItemType::BlockStone), GetItemTextureForType(ItemType::BlockStone), InventoryType::Block, ItemType::BlockStone, ItemStatus::None, EquipSlot::RightHand, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::BlockStone), GetItemDescriptionForType(ItemType::BlockStone), 0.23f, 0.13f, 0.02f, 1);
	AddItemSubSpawnData(ItemType::None, BlockType::Wood, ItemType::BlockWood, GetItemFileNameForType(ItemType::BlockWood), GetItemTitleForType(ItemType::BlockWood), false, true, 0.06f,
		GetItemFileNameForType(ItemType::BlockWood), GetItemTextureForType(ItemType::BlockWood), InventoryType::Block, ItemType::BlockWood, ItemStatus::None, EquipSlot::RightHand, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::BlockWood), GetItemDescriptionForType(ItemType::BlockWood), 0.23f, 0.13f, 0.02f, 1);
	AddItemSubSpawnData(ItemType::None, BlockType::Leaf, ItemType::BlockLeaf, GetItemFileNameForType(ItemType::BlockLeaf), GetItemTitleForType(ItemType::BlockLeaf), false, true, 0.06f,
		GetItemFileNameForType(ItemType::BlockLeaf), GetItemTextureForType(ItemType::BlockLeaf), InventoryType::Block, ItemType::BlockLeaf, ItemStatus::None, EquipSlot::RightHand, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::BlockLeaf), GetItemDescriptionForType(ItemType::BlockLeaf), 0.23f, 0.13f, 0.02f, 1);
	AddItemSubSpawnData(ItemType::None, BlockType::Sand, ItemType::BlockSand, GetItemFileNameForType(ItemType::BlockSand), GetItemTitleForType(ItemType::BlockSand), false, true, 0.06f,
		GetItemFileNameForType(ItemType::BlockSand), GetItemTextureForType(ItemType::BlockSand), InventoryType::Block, ItemType::BlockSand, ItemStatus::None, EquipSlot::RightHand, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::BlockSand), GetItemDescriptionForType(ItemType::BlockSand), 0.23f, 0.13f, 0.02f, 1);
	AddItemSubSpawnData(ItemType::None, BlockType::Cactus, ItemType::BlockCactus, GetItemFileNameForType(ItemType::BlockCactus), GetItemTitleForType(ItemType::BlockCactus), false, true, 0.06f,
		GetItemFileNameForType(ItemType::BlockCactus), GetItemTextureForType(ItemType::BlockCactus), InventoryType::Block, ItemType::BlockCactus, ItemStatus::None, EquipSlot::RightHand, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::BlockCactus), GetItemDescriptionForType(ItemType::BlockCactus), 0.23f, 0.13f, 0.02f, 1);
	AddItemSubSpawnData(ItemType::None, BlockType::Rock, ItemType::BlockRock, GetItemFileNameForType(ItemType::BlockRock), GetItemTitleForType(ItemType::BlockRock), false, true, 0.06f,
		GetItemFileNameForType(ItemType::BlockRock), GetItemTextureForType(ItemType::BlockRock), InventoryType::Block, ItemType::BlockRock, ItemStatus::None, EquipSlot::RightHand, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::BlockRock), GetItemDescriptionForType(ItemType::BlockRock), 0.23f, 0.13f, 0.02f, 1);
	AddItemSubSpawnData(ItemType::None, BlockType::Snow, ItemType::BlockSnow, GetItemFileNameForType(ItemType::BlockSnow), GetItemTitleForType(ItemType::BlockSnow), false, true, 0.06f,
		GetItemFileNameForType(ItemType::BlockSnow), GetItemTextureForType(ItemType::BlockSnow), InventoryType::Block, ItemType::BlockSnow, ItemStatus::None, EquipSlot::RightHand, ItemQuality::Common, false, false, GetItemTitleForType(ItemType::BlockSnow), GetItemDescriptionForType(ItemType::BlockSnow), 1.0f, 1.0f, 1.0f, 1);
}

ItemManager::~ItemManager()
{
	ClearItems();
	ClearSubSpawnData();
	ClearItemSpawners();
}

void ItemManager::SetLightingManager(LightingManager* pLightingManager)
{
	m_pLightingManager = pLightingManager;
}

void ItemManager::SetBlockParticleManager(BlockParticleManager* pBlockParticleManager)
{
	m_pBlockParticleManager = pBlockParticleManager;
}

void ItemManager::SetQubicleBinaryManager(QubicleBinaryManager* pQubicleBinaryManager)
{
	m_pQubicleBinaryManager = pQubicleBinaryManager;
}

void ItemManager::SetInventoryManager(InventoryManager* pInventoryManager)
{
	m_pInventoryManager = pInventoryManager;
}

void ItemManager::SetNPCManager(NPCManager* pNPCManager)
{
	m_pNPCManager = pNPCManager;
}

// Deletion
void ItemManager::ClearItems()
{
	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		delete m_vpItemList[i];
		m_vpItemList[i] = nullptr;
	}

	m_vpItemList.clear();
}

void ItemManager::ClearSubSpawnData()
{
	for (size_t i = 0; i < m_vpItemSubSpawnDataList.size(); ++i)
	{
		delete m_vpItemSubSpawnDataList[i];
		m_vpItemSubSpawnDataList[i] = nullptr;
	}

	m_vpItemSubSpawnDataList.clear();
}

void ItemManager::ClearItemSpawners()
{
	for (size_t i = 0; i < m_vpItemSpawnerList.size(); ++i)
	{
		delete m_vpItemSpawnerList[i];
		m_vpItemSpawnerList[i] = nullptr;
	}

	m_vpItemSpawnerList.clear();
}

void ItemManager::RemoveItemSpawnerFromItems(ItemSpawner* pSpawner)
{
	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		m_vpItemList[i]->RemoveItemSpawner(pSpawner);
	}
}

// Item sub spawn data
void ItemManager::AddItemSubSpawnData(ItemType sourceItem, BlockType sourceBlockType, ItemType spawnedItem, std::string spawnedItemFileName, std::string spawnedItemTitle, bool interactable, bool collectible, float scale,
	std::string droppedItemFileName, std::string droppedItemTextureFileName, InventoryType droppedItemInventoryType, ItemType droppedItemItem, ItemStatus droppedItemStatus, EquipSlot droppedItemEquipSlot, ItemQuality droppedItemQuality,
	bool droppedItemLeft, bool droppedItemRight, std::string droppedItemTitle, std::string droppedItemDescription, float droppedItemPlacementR, float droppedItemPlacementG, float droppedItemPlacementB, int droppedItemQuantity)
{
	ItemSubSpawnData* pItemSubSpawnData = new ItemSubSpawnData();

	pItemSubSpawnData->m_sourceItem = sourceItem;
	pItemSubSpawnData->m_sourceBlockType = sourceBlockType;

	pItemSubSpawnData->m_spawnedItem = spawnedItem;
	pItemSubSpawnData->m_spawnedItemFileName = spawnedItemFileName;
	pItemSubSpawnData->m_spawnedItemTitle = spawnedItemTitle;
	pItemSubSpawnData->m_collectible = collectible;
	pItemSubSpawnData->m_interactable = interactable;
	pItemSubSpawnData->m_scale = scale;

	pItemSubSpawnData->m_droppedItemFileName = droppedItemFileName;
	pItemSubSpawnData->m_droppedItemTextureFileName = droppedItemTextureFileName;
	pItemSubSpawnData->m_droppedItemInventoryType = droppedItemInventoryType;
	pItemSubSpawnData->m_droppedItemItem = droppedItemItem;
	pItemSubSpawnData->m_droppedItemStatus = droppedItemStatus;
	pItemSubSpawnData->m_droppedItemEquipSlot = droppedItemEquipSlot;
	pItemSubSpawnData->m_droppedItemQuality = droppedItemQuality;
	pItemSubSpawnData->m_droppedItemLeft = droppedItemLeft;
	pItemSubSpawnData->m_droppedItemRight = droppedItemRight;
	pItemSubSpawnData->m_droppedItemTitle = droppedItemTitle;
	pItemSubSpawnData->m_droppedItemDescription = droppedItemDescription;
	pItemSubSpawnData->m_droppedItemPlacementR = droppedItemPlacementR;
	pItemSubSpawnData->m_droppedItemPlacementG = droppedItemPlacementG;
	pItemSubSpawnData->m_droppedItemPlacementB = droppedItemPlacementB;
	pItemSubSpawnData->m_droppedItemQuantity = droppedItemQuantity;

	m_vpItemSubSpawnDataList.push_back(pItemSubSpawnData);
}

ItemSubSpawnData* ItemManager::GetItemSubSpawnData(ItemType sourceItem)
{
	for (size_t i = 0; i < m_vpItemSubSpawnDataList.size(); ++i)
	{
		if (m_vpItemSubSpawnDataList[i]->m_sourceItem == sourceItem)
		{
			return m_vpItemSubSpawnDataList[i];
		}
	}

	return nullptr;
}

ItemSubSpawnData* ItemManager::GetItemSubSpawnData(BlockType sourceBlockType)
{
	for (size_t i = 0; i < m_vpItemSubSpawnDataList.size(); ++i)
	{
		if (m_vpItemSubSpawnDataList[i]->m_sourceBlockType == sourceBlockType)
		{
			return m_vpItemSubSpawnDataList[i];
		}
	}

	return nullptr;
}

// Creation
Item* ItemManager::CreateItem(glm::vec3 position, glm::vec3 velocity, glm::vec3 rotation, const char* objectFileName, ItemType itemType, const char* itemTitle, bool interactable, bool collectible, float scale)
{
	Item* pNewItem = new Item(m_pRenderer, m_pChunkManager, m_pQubicleBinaryManager, itemTitle, itemType, scale);
	pNewItem->LoadItem(objectFileName);

	pNewItem->SetLightingManager(m_pLightingManager);
	pNewItem->SetBlockParticleManager(m_pBlockParticleManager);
	pNewItem->SetPlayer(m_pPlayer);
	pNewItem->SetItemManager(this);
	pNewItem->SetInventoryManager(m_pInventoryManager);

	glm::vec3 gravityDir = glm::vec3(0.0f, -1.0f, 0.0f);
	pNewItem->SetPosition(position);
	pNewItem->SetRotation(rotation);
	pNewItem->SetGravityDirection(gravityDir);
	pNewItem->SetVelocity(velocity);

	pNewItem->SetIsCollectible(collectible);
	pNewItem->SetCollectionDelay(GetRandomNumber(1, 2, 2));

	pNewItem->SetInteractable(interactable);

	m_vpItemList.push_back(pNewItem);

	return pNewItem;
}

void ItemManager::RemoveItem(const char* itemTitle)
{
	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		if (m_vpItemList[i]->IsNeedErase())
		{
			continue;
		}

		if (strcmp(m_vpItemList[i]->GetItemTitle(), itemTitle) == 0)
		{
			m_vpItemList[i]->SetErase(true);
		}
	}
}

ItemSpawner* ItemManager::CreateItemSpawner(glm::vec3 position, glm::vec3 direction)
{
	ItemSpawner* pItemSpawner = new ItemSpawner(m_pRenderer, m_pChunkManager, m_pPlayer, this, m_pNPCManager);

	pItemSpawner->SetPosition(position);
	pItemSpawner->SetInitialPosition(position);
	pItemSpawner->SetFacingDirection(glm::normalize(direction));

	m_vpItemSpawnerList.push_back(pItemSpawner);

	return pItemSpawner;
}

// Get number of items
int ItemManager::GetNumItems() const
{
	int numItems = static_cast<int>(m_vpItemList.size());

	return numItems;
}

int ItemManager::GetNumRenderItems() const
{
	return m_numRenderItems;
}

// Collision detection
bool ItemManager::CheckCollisions(glm::vec3 center, glm::vec3 previousCenter, float radius, glm::vec3* pNormal, glm::vec3* pMovement)
{
	bool colliding = false;
	for (size_t i = 0; i < m_vpItemList.size() && colliding == false; ++i)
	{
		//if(m_pChunkManager->IsInsideLoader(m_vpItemList[i]->GetCenter()) == false)
		//{
		//	continue;
		//}

		if (m_vpItemList[i]->IsCollisionEnabled())
		{
			glm::vec3 toPlayer = m_vpItemList[i]->GetCenter() - m_pPlayer->GetCenter();

			if (length(toPlayer) < radius + m_vpItemList[i]->GetCollisionRadius())
			{
				m_vpItemList[i]->CalculateWorldTransformMatrix();

				if (m_vpItemList[i]->IsColliding(center, previousCenter, radius, pNormal, pMovement))
				{
					colliding = true;
				}
			}
		}
	}

	return colliding;
}

// Player item interaction
Item* ItemManager::CheckItemPlayerInteraction()
{
	Item* pInteractingItem = nullptr;
	float maxDotProduct = 0.0f;

	// Check if any item are within interaction range
	for (auto iter = m_vpItemList.begin(); iter != m_vpItemList.end(); ++iter)
	{
		Item* pItem = *iter;

		if (pItem->IsNeedErase())
		{
			continue;
		}

		if (pItem->IsInteractable() == false)
		{
			// Don't test non-interactable items
			continue;
		}

		glm::vec3 distance = pItem->GetCenter() - m_pPlayer->GetCenter();
		glm::vec3 direction = normalize(distance);
		glm::vec3 facing = m_pPlayer->GetForwardVector();

		distance.y = 0.0f;
		float lengthSize = length(distance);

		float dotProduct = dot(direction, facing);

		if (lengthSize > pItem->GetRadius() && lengthSize < ITEM_INTERACTION_DISTANCE + pItem->GetRadius())
		{
			// Make sure we are facing
			if (dotProduct > ITEM_INTERACTION_RADIUS_CHECK)
			{
				if (dotProduct > maxDotProduct)
				{
					maxDotProduct = dotProduct;
					pInteractingItem = pItem;
				}
			}
		}
	}

	return pInteractingItem;
}

bool IsNeedErasing(Item* item)
{
	bool needsErase = item->IsNeedErase();

	if (needsErase == true)
	{
		delete item;
	}

	return needsErase;
}

// Rendering Helpers
void ItemManager::CalculateWorldTransformMatrix()
{
	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		Item* pItem = m_vpItemList[i];

		pItem->CalculateWorldTransformMatrix();
	}
}

void ItemManager::SetWireFrameRender(bool wireframe)
{
	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		Item* pItem = m_vpItemList[i];

		pItem->SetWireFrameRender(wireframe);
	}
}

// Update
void ItemManager::Update(float dt)
{
	// Update all item spawners
	for (size_t i = 0; i < m_vpItemSpawnerList.size(); ++i)
	{
		ItemSpawner* pItemSpawner = m_vpItemSpawnerList[i];

		if (pItemSpawner->ShouldFollowPlayer())
		{
			pItemSpawner->SetPosition(m_pPlayer->GetCenter() + pItemSpawner->GetInitialPosition());
		}

		// TODO: Loader radius culling
		//if(m_pChunkManager->IsInsideLoader(pItemSpawner->GetPosition()) == false)
		//{
		//	continue;
		//}

		pItemSpawner->Update(dt);
	}

	// Remove any items that need to be erased
	m_vpItemList.erase(remove_if(m_vpItemList.begin(), m_vpItemList.end(), IsNeedErasing), m_vpItemList.end());

	UpdateHoverItems();

	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		Item* pItem = m_vpItemList[i];

		if (pItem->IsNeedErase())
		{
			continue;
		}

		pItem->Update(dt);
	}
}

void ItemManager::UpdateItemLights()
{
	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		Item* pItem = m_vpItemList[i];

		if (pItem->IsNeedErase())
		{
			continue;
		}

		pItem->UpdateItemLights();
	}
}

void ItemManager::UpdateItemParticleEffects()
{
	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		Item* pItem = m_vpItemList[i];

		if (pItem->IsNeedErase())
		{
			continue;
		}

		pItem->UpdateItemParticleEffects();
	}
}

void ItemManager::UpdateHoverItems()
{
	Item* pHoverItem = nullptr;
	float maxDotProduct = 0.0f;

	// Check if any item are within interaction range
	for (auto iter = m_vpItemList.begin(); iter != m_vpItemList.end(); ++iter)
	{
		Item* pItem = *iter;

		if (pItem->IsNeedErase())
		{
			continue;
		}

		pItem->SetOutlineRender(false);

		if (pItem->IsInteractable() == false)
		{
			// Don't hover highlight non-interactable items
			continue;
		}
		if (pItem->IsCollectible() == true)
		{
			// Don't hover highlight collectible items
			continue;
		}

		glm::vec3 distance = pItem->GetCenter() - m_pPlayer->GetCenter();
		glm::vec3 direction = normalize(distance);
		glm::vec3 facing = m_pPlayer->GetForwardVector();

		distance.y = 0.0f;
		float lengthSize = length(distance);

		float dotProduct = dot(direction, facing);

		if (lengthSize > pItem->GetRadius() && lengthSize < ITEM_INTERACTION_DISTANCE + pItem->GetRadius())
		{
			// Make sure we are facing
			if (dotProduct > ITEM_INTERACTION_RADIUS_CHECK)
			{
				if (dotProduct > maxDotProduct)
				{
					maxDotProduct = dotProduct;
					pHoverItem = pItem;
				}
			}
		}
	}

	if (pHoverItem != nullptr)
	{
		pHoverItem->SetOutlineRender(true);
	}
}

// Rendering
void ItemManager::Render(bool outline, bool reflection, bool silhouette, bool shadow)
{
	m_numRenderItems = 0;

	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		Item* pItem = m_vpItemList[i];

		if (pItem->IsNeedErase())
		{
			continue;
		}

		if ((outline || silhouette) && pItem->IsOutlineRender() == false)
		{
			continue;
		}

		// Fog
		float toCamera = length(CubbyGame::GetInstance()->GetGameCamera()->GetPosition() - pItem->GetCenter());
		if (toCamera > m_pChunkManager->GetLoaderRadius() + (Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 5.0f))
		{
			continue;
		}
		if (toCamera > m_pChunkManager->GetLoaderRadius() - Chunk::CHUNK_SIZE * Chunk::BLOCK_RENDER_SIZE * 3.0f)
		{
			m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		}

		if (shadow || m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pItem->GetCenter(), pItem->GetRadius()))
		{
			pItem->Render(outline, reflection, silhouette);

			m_numRenderItems++;
		}

		m_pRenderer->DisableTransparency();
	}
}

void ItemManager::RenderDebug()
{
	// Items
	for (size_t i = 0; i < m_vpItemList.size(); ++i)
	{
		Item* pItem = m_vpItemList[i];

		if (pItem->IsNeedErase())
		{
			continue;
		}

		if (m_pRenderer->SphereInFrustum(CubbyGame::GetInstance()->GetDefaultViewport(), pItem->GetCenter(), pItem->GetRadius()))
		{
			pItem->RenderDebug();
		}
	}

	// Item spawners
	for (size_t i = 0; i < m_vpItemSpawnerList.size(); ++i)
	{
		ItemSpawner* pItemSpawner = m_vpItemSpawnerList[i];

		pItemSpawner->RenderDebug();
	}
}
