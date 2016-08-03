/*************************************************************************
> File Name: InventoryManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The inventory manager is linked to the player and holds the information
>    about what items the player has inside their inventory and also what
>    items the player has equipped. The internal state of the inventory and
>    equipped items gets changed when the player modifies the InventoryGUI
>    and characterGUI.
> Created Time: 2016/08/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>

#include <Items/Item.h>

#include "InventoryManager.h"

// Global variable
static ItemTextData g_itemData[] =
{
	// Item, FileName, Title, Description
	ItemType::None,	"none",	"None",	"None.",
};

// Constructor, Destructor
InventoryManager::InventoryManager() :
	m_pPlayer(nullptr), m_pInventoryGUI(nullptr), m_pLootGUI(nullptr), m_pActionBar(nullptr)
{
	// Reset the inventory mapping array
	for (int i = 0; i < MAX_NUM_INVENTORY_SLOTS; ++i)
	{
		m_ItemSlotMapping[i] = nullptr;
	}

	for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
	{
		m_equippedSlots[i] = nullptr;
	}

	m_playerName = "";

	m_numCoins = 0;
	m_coinsUpdated = true;

	m_supressExport = false;

	m_InventoryGUINeedsUpdate = false;
	m_CharacterGUINeedsUpdate = false;
}

InventoryManager::~InventoryManager()
{
	Reset();
	ClearOtherCreatedItems();
}

void InventoryManager::Reset()
{
	ClearInventory();
	ClearEquipped();
}

// Linkage
void InventoryManager::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

void InventoryManager::SetInventoryGUI(InventoryGUI* pInventoryGUI)
{
	m_pInventoryGUI = pInventoryGUI;
}

void InventoryManager::SetLootGUI(LootGUI* pLootGUI)
{
	m_pLootGUI = pLootGUI;
}

void InventoryManager::SetActionBar(ActionBar* pActionBar)
{
	m_pActionBar = pActionBar;
}

void InventoryManager::ClearInventory()
{
	for (size_t i = 0; i < m_vpInventoryItemList.size(); ++i)
	{
		delete m_vpInventoryItemList[i];
		m_vpInventoryItemList[i] = nullptr;
	}

	m_vpInventoryItemList.clear();

	for (int i = 0; i < MAX_NUM_INVENTORY_SLOTS; ++i)
	{
		m_ItemSlotMapping[i] = nullptr;
	}
}

void InventoryManager::ClearEquipped()
{
	for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
	{
		m_equippedSlots[i] = nullptr;
	}
}

void InventoryManager::ClearOtherCreatedItems()
{
	for (size_t i = 0; i < m_vpOtherInventoryItemList.size(); ++i)
	{
		delete m_vpOtherInventoryItemList[i];
		m_vpOtherInventoryItemList[i] = nullptr;
	}

	m_vpOtherInventoryItemList.clear();
}

void InventoryManager::LoadInventory(std::string playerName, PlayerClass playerClass, bool exportInventoryFile)
{
	ClearInventory();
	ClearEquipped();

	m_playerName = playerName;

	m_numCoins = 0;

	LoadInventoryForClass(playerClass);

	if (exportInventoryFile)
	{
		ExportInventory(m_playerName);
	}
}

void InventoryManager::LoadDefaultInventory()
{
	InventoryItem* pSword = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::IronSword), -1, -1);
	InventoryItem* pShield = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::IronShield), -1, -1);
	InventoryItem* pBow = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::WoodenBow), -1, -1);
	InventoryItem* pTwoHandedSword = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::TwoHandedSword), -1, -1);
	//InventoryItem* pBoomerang = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::Boomerang), -1, -1);
	//InventoryItem* pBomb = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::Bomb), -1, -1);
	//InventoryItem* pStaff1 = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::MageStaff), -1, -1); 
	//InventoryItem* pStaff2 = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::PriestStaff), -1, -1);
	//InventoryItem* pStaff3 = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::DruidStaff), -1, -1);
	//InventoryItem* pDagger1 = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::KnifeRight), -1, -1);
	//InventoryItem* pDagger2 = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::KnifeLeft), -1, -1);
	//InventoryItem* pFireball1 = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::FireballHandRight), -1, -1);
	//InventoryItem* pFireball2 = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::FireballHandLeft), -1, -1);

	/*
	InventoryItem* pStone = AddInventoryItem("media/gamedata/items/Block_Stone/Block_Stone.item", "media/textures/items/block_stone.tga", InventoryType_Block, eItem_Block_Stone, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Stone Block", "Stone block for world building", 1.0f, 1.0f, 1.0f, 3, -1, -1, -1, -1);
	InventoryItem* pWood = AddInventoryItem("media/gamedata/items/Block_Wood/Block_Wood.item", "media/textures/items/block_wood.tga", InventoryType_Block, eItem_Block_Wood, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Wood Block", "Wood block for world building", 1.0f, 1.0f, 1.0f, 4, -1, -1, -1, -1);
	InventoryItem* pGrass = AddInventoryItem("media/gamedata/items/Block_Grass/Block_Grass.item", "media/textures/items/block_grass.tga", InventoryType_Block, eItem_Block_Grass, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Grass Block", "Grass block for world building", 1.0f, 1.0f, 1.0f, 2, -1, -1, -1, -1);
	InventoryItem* pDirt = AddInventoryItem("media/gamedata/items/Block_Dirt/Block_Dirt.item", "media/textures/items/block_dirt.tga", InventoryType_Block, eItem_Block_Dirt, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Dirt Block", "Dirt block for world building", 1.0f, 1.0f, 1.0f, 2, -1, -1, -1, -1);
	InventoryItem* pLeaf = AddInventoryItem("media/gamedata/items/Block_Leaf/Block_Leaf.item", "media/textures/items/block_leaf.tga", InventoryType_Block, eItem_Block_Leaf, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Leaf Block", "Leaf block for world building", 1.0f, 1.0f, 1.0f, 2, -1, -1, -1, -1);
	*/

	/*
	InventoryItem* pAnvil = AddInventoryItem("media/gamedata/items/Anvil/Anvil.item", "media/textures/items/anvil.tga", InventoryType_Item, eItem_Anvil, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Anvil", "Anvil for crafting", 1.0f, 1.0f, 1.0f, 1, -1, -1, -1, -1);
	InventoryItem* pChest = AddInventoryItem("media/gamedata/items/Chest/Chest.item", "media/textures/items/chest.tga", InventoryType_Item, eItem_Chest, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Chest", "Chest for storing items", 1.0f, 1.0f, 1.0f, 1, -1, -1, -1, -1);
	InventoryItem* pFurnace = AddInventoryItem("media/gamedata/items/Furnace/Furnace.item", "media/textures/items/furnace.tga", InventoryType_Item, eItem_Furnace, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Furnace", "Used for smelting ore", 1.0f, 1.0f, 1.0f, 1, -1, -1, -1, -1);
	*/

	/*
	InventoryItem* pShoulders = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::SteelShoulders), -1, -1);
	InventoryItem* pHelm = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::SteelHelm), -1, -1);
	InventoryItem* pGloves = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::SteelGloves), -1, -1);
	InventoryItem* pBoots = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::SteelBoots), -1, -1);
	InventoryItem* pBody = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::SteelArmor), -1, -1);
	InventoryItem* pLegs = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::SteelPants), -1, -1);
	*/

	// Assertion
	assert(pSword != nullptr);
	assert(pShield != nullptr);
	assert(pBow != nullptr);
	assert(pTwoHandedSword != nullptr);
}

void InventoryManager::LoadInventoryForClass(PlayerClass playerClass)
{
	InventoryItem* pPickaxe = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::NormalPickaxe), -1, -1);
	InventoryItem* pTorch = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::Torch), -1, -1);

	assert(pPickaxe != nullptr);
	assert(pTorch != nullptr);

	if (playerClass == PlayerClass::Debug)
	{
		LoadDefaultInventory();
	}
	else if (playerClass == PlayerClass::Warrior)
	{
		InventoryItem* pSword = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::IronSword), -1, -1);
		InventoryItem* pShield = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::IronShield), -1, -1);

		assert(pSword != nullptr);
		assert(pShield != nullptr);
	}
	else if (playerClass == PlayerClass::Ranger)
	{
		InventoryItem* pBow = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::WoodenBow), -1, -1);

		assert(pBow != nullptr);
	}
	else if (playerClass == PlayerClass::Mage)
	{
		InventoryItem* pMageStaff = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::MageStaff), -1, -1);

		assert(pMageStaff != nullptr);
	}
	else if (playerClass == PlayerClass::Priest)
	{
		InventoryItem* pPriestStaff = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::PriestStaff), -1, -1);
		InventoryItem* pFireball1 = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::FireballHandRight), -1, -1);
		InventoryItem* pFireball2 = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::FireballHandLeft), -1, -1);

		assert(pPriestStaff != nullptr);
		assert(pFireball1 != nullptr);
		assert(pFireball2 != nullptr);
	}
	else if (playerClass == PlayerClass::Necromaner)
	{
		InventoryItem* pNecroStaff = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::NecroStaff), -1, -1);

		assert(pNecroStaff != nullptr);
	}
	else if (playerClass == PlayerClass::Knight)
	{
		InventoryItem* pTwoHandedSword = AddInventoryItem(CreateEquipmentItemFromType(EquipmentType::TwoHandedSword), -1, -1);

		assert(pTwoHandedSword != nullptr);
	}
}

void InventoryManager::SetInventoryGUINeedsUpdate(bool update)
{
	m_InventoryGUINeedsUpdate = update;
}

bool InventoryManager::InventoryGUINeedsUpdate() const
{
	return m_InventoryGUINeedsUpdate;
}

void InventoryManager::SetCharacterGUINeedsUpdate(bool update)
{
	m_CharacterGUINeedsUpdate = update;
}

bool InventoryManager::CharacterGUINeedsUpdate() const
{
	return m_CharacterGUINeedsUpdate;
}

// Supress export
void InventoryManager::SetSupressExport(bool supress)
{
	m_supressExport = supress;
}

// Import / Export inventory
void InventoryManager::ExportInventory(std::string playerName)
{
	if (m_supressExport)
	{
		return;
	}

	std::ofstream exportFile;
	char lfilename[128];
	sprintf(lfilename, "Saves/characters/%s/%s.inv", playerName.c_str(), playerName.c_str());
	exportFile.open(lfilename, std::ios_base::binary);

	if (exportFile.is_open())
	{
		// Export the stored inventory items
		exportFile << MAX_NUM_INVENTORY_SLOTS << "|";

		for (int i = 0; i < MAX_NUM_INVENTORY_SLOTS; i++)
		{
			InventoryItem* pItem = m_ItemSlotMapping[i];

			if (pItem == nullptr)
			{
				exportFile << "0" << "|";
			}
			else
			{
				exportFile << "1" << "|";
				exportFile << pItem->m_fileName << "|";
				exportFile << pItem->m_IconfileName << "|";
				exportFile << static_cast<int>(pItem->m_itemType) << "|";
				exportFile << static_cast<int>(pItem->m_item) << "|";
				exportFile << static_cast<int>(pItem->m_status) << "|";
				exportFile << static_cast<int>(pItem->m_equipSlot) << "|";
				exportFile << static_cast<int>(pItem->m_itemQuality) << "|";
				exportFile << pItem->m_left << "|";
				exportFile << pItem->m_right << "|";
				exportFile << pItem->m_title << "|";
				exportFile << pItem->m_description << "|";
				exportFile << pItem->m_placementR << "|";
				exportFile << pItem->m_placementG << "|";
				exportFile << pItem->m_placementB << "|";
				exportFile << pItem->m_scale << "|";
				exportFile << pItem->m_offsetX << "|";
				exportFile << pItem->m_offsetY << "|";
				exportFile << pItem->m_offsetZ << "|";
				exportFile << pItem->m_quantity << "|";

				// Also export the stats attributes
				exportFile << static_cast<int>(pItem->m_vpStatAttributes.size()) << "|";
				for (size_t j = 0; j < pItem->m_vpStatAttributes.size(); ++j)
				{
					exportFile << static_cast<int>(pItem->m_vpStatAttributes[j]->GetType()) << "|";
					exportFile << pItem->m_vpStatAttributes[j]->GetModifyAmount() << "|";
				}
			}
		}

		// Export the equipped items
		for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); i++)
		{
			InventoryItem* pItem = m_equippedSlots[i];

			if (pItem == nullptr)
			{
				exportFile << "0" << "|";
			}
			else
			{
				exportFile << "1" << "|";
				exportFile << pItem->m_fileName << "|";
				exportFile << pItem->m_IconfileName << "|";
				exportFile << static_cast<int>(pItem->m_itemType) << "|";
				exportFile << static_cast<int>(pItem->m_item) << "|";
				exportFile << static_cast<int>(pItem->m_status) << "|";
				exportFile << static_cast<int>(pItem->m_equipSlot) << "|";
				exportFile << static_cast<int>(pItem->m_itemQuality) << "|";
				exportFile << pItem->m_left << "|";
				exportFile << pItem->m_right << "|";
				exportFile << pItem->m_title << "|";
				exportFile << pItem->m_description << "|";
				exportFile << pItem->m_placementR << "|";
				exportFile << pItem->m_placementG << "|";
				exportFile << pItem->m_placementB << "|";
				exportFile << pItem->m_scale << "|";
				exportFile << pItem->m_offsetX << "|";
				exportFile << pItem->m_offsetY << "|";
				exportFile << pItem->m_offsetZ << "|";
				exportFile << pItem->m_quantity << "|";

				// Also export the stats attributes
				exportFile << static_cast<int>(pItem->m_vpStatAttributes.size()) << "|";
				for (size_t j = 0; j < pItem->m_vpStatAttributes.size(); ++j)
				{
					exportFile << static_cast<int>(pItem->m_vpStatAttributes[j]->GetType()) << "|";
					exportFile << pItem->m_vpStatAttributes[j]->GetModifyAmount() << "|";
				}
			}
		}

		// Export the number of coins
		exportFile << m_numCoins << "|";

		// Export the file signature
		exportFile << "signature: " << "\n";

		exportFile.close();
	}
}