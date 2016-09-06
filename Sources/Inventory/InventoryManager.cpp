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
#include <algorithm>

#include <GameGUI/InventoryGUI.h>
#include <Items/Item.h>
#include <Player/Player.h>

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
	InventoryItem* pStone = AddInventoryItem("Resources/gamedata/items/Block_Stone/Block_Stone.item", "Resources/textures/items/block_stone.tga", InventoryType_Block, eItem_Block_Stone, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Stone Block", "Stone block for world building", 1.0f, 1.0f, 1.0f, 3, -1, -1, -1, -1);
	InventoryItem* pWood = AddInventoryItem("Resources/gamedata/items/Block_Wood/Block_Wood.item", "Resources/textures/items/block_wood.tga", InventoryType_Block, eItem_Block_Wood, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Wood Block", "Wood block for world building", 1.0f, 1.0f, 1.0f, 4, -1, -1, -1, -1);
	InventoryItem* pGrass = AddInventoryItem("Resources/gamedata/items/Block_Grass/Block_Grass.item", "Resources/textures/items/block_grass.tga", InventoryType_Block, eItem_Block_Grass, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Grass Block", "Grass block for world building", 1.0f, 1.0f, 1.0f, 2, -1, -1, -1, -1);
	InventoryItem* pDirt = AddInventoryItem("Resources/gamedata/items/Block_Dirt/Block_Dirt.item", "Resources/textures/items/block_dirt.tga", InventoryType_Block, eItem_Block_Dirt, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Dirt Block", "Dirt block for world building", 1.0f, 1.0f, 1.0f, 2, -1, -1, -1, -1);
	InventoryItem* pLeaf = AddInventoryItem("Resources/gamedata/items/Block_Leaf/Block_Leaf.item", "Resources/textures/items/block_leaf.tga", InventoryType_Block, eItem_Block_Leaf, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Leaf Block", "Leaf block for world building", 1.0f, 1.0f, 1.0f, 2, -1, -1, -1, -1);
	*/

	/*
	InventoryItem* pAnvil = AddInventoryItem("Resources/gamedata/items/Anvil/Anvil.item", "Resources/textures/items/anvil.tga", InventoryType_Item, eItem_Anvil, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Anvil", "Anvil for crafting", 1.0f, 1.0f, 1.0f, 1, -1, -1, -1, -1);
	InventoryItem* pChest = AddInventoryItem("Resources/gamedata/items/Chest/Chest.item", "Resources/textures/items/chest.tga", InventoryType_Item, eItem_Chest, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Chest", "Chest for storing items", 1.0f, 1.0f, 1.0f, 1, -1, -1, -1, -1);
	InventoryItem* pFurnace = AddInventoryItem("Resources/gamedata/items/Furnace/Furnace.item", "Resources/textures/items/furnace.tga", InventoryType_Item, eItem_Furnace, ItemStatus_None, EquipSlot_RightHand, ItemQuality_Common, false, false, "Furnace", "Used for smelting ore", 1.0f, 1.0f, 1.0f, 1, -1, -1, -1, -1);
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
	char exportFileName[128];
	sprintf(exportFileName, "Saves/characters/%s/%s.inv", playerName.c_str(), playerName.c_str());
	exportFile.open(exportFileName, std::ios_base::binary);

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
				exportFile << pItem->m_iconFileName << "|";
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
				exportFile << pItem->m_iconFileName << "|";
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

void InventoryManager::ImportInventory(std::string playerName)
{
	m_playerName = playerName;

	std::ifstream importFile;
	char importFileName[128];
	sprintf(importFileName, "Saves/characters/%s/%s.inv", playerName.c_str(), playerName.c_str());
	importFile.open(importFileName, std::ios_base::binary);

	if (importFile.is_open())
	{
		char* input = new char[128];
		char* pEnd;

		// Import the stored inventory items
		importFile.getline(input, 128, '|');
		int numInventory = strtol(input, &pEnd, 10);

		for (int i = 0; i < numInventory; ++i)
		{
			importFile.getline(input, 128, '|');
			int itemValid = strtol(input, &pEnd, 10);

			if (itemValid == 1)
			{
				char fileName[128];
				importFile.getline(fileName, 128, '|');

				char iconFileName[128];
				importFile.getline(iconFileName, 128, '|');

				importFile.getline(input, 128, '|');
				int itemType = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int item = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int status = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int equipSlot = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int itemQuality = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int left = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int right = strtol(input, &pEnd, 10);

				char title[128];
				importFile.getline(title, 128, '|');

				char description[128];
				importFile.getline(description, 128, '|');

				float r;
				float g;
				float b;
				importFile.getline(input, 128, '|');
				r = static_cast<float>(strtod(input, &pEnd));
				importFile.getline(input, 128, '|');
				g = static_cast<float>(strtod(input, &pEnd));
				importFile.getline(input, 128, '|');
				b = static_cast<float>(strtod(input, &pEnd));

				float scale;
				float offsetX;
				float offsetY;
				float offsetZ;

				importFile.getline(input, 128, '|');
				scale = static_cast<float>(strtod(input, &pEnd));
				importFile.getline(input, 128, '|');
				offsetX = static_cast<float>(strtod(input, &pEnd));
				importFile.getline(input, 128, '|');
				offsetY = static_cast<float>(strtod(input, &pEnd));
				importFile.getline(input, 128, '|');
				offsetZ = static_cast<float>(strtod(input, &pEnd));

				importFile.getline(input, 128, '|');
				int quantity = strtol(input, &pEnd, 10);

				// Create the new inventory item				
				InventoryItem* pNewItem = new InventoryItem();

				pNewItem->m_fileName = fileName;

				pNewItem->m_iconFileName = iconFileName;

				pNewItem->m_title = title;
				pNewItem->m_description = description;

				pNewItem->m_itemType = static_cast<InventoryType>(itemType);
				pNewItem->m_item = static_cast<ItemType>(item);
				pNewItem->m_status = static_cast<ItemStatus>(status);
				pNewItem->m_equipSlot = static_cast<EquipSlot>(equipSlot);
				pNewItem->m_itemQuality = static_cast<ItemQuality>(itemQuality);

				pNewItem->m_left = static_cast<bool>(left);
				pNewItem->m_right = static_cast<bool>(right);

				pNewItem->m_placementR = r;
				pNewItem->m_placementG = g;
				pNewItem->m_placementB = b;

				pNewItem->m_scale = scale;
				pNewItem->m_offsetX = offsetX;
				pNewItem->m_offsetY = offsetY;
				pNewItem->m_offsetZ = offsetZ;

				pNewItem->m_quantity = quantity;

				pNewItem->m_equipped = false;

				pNewItem->m_remove = false;

				// Also import the stats attributes
				int numStatAttributes;
				importFile.getline(input, 128, '|');
				numStatAttributes = strtol(input, &pEnd, 10);
				for (int k = 0; k < numStatAttributes; k++)
				{
					int type;
					int amount;
					importFile.getline(input, 128, '|');
					type = strtol(input, &pEnd, 10);

					importFile.getline(input, 128, '|');
					amount = strtol(input, &pEnd, 10);

					pNewItem->AddStatAttribute(static_cast<AttributeType>(type), amount);
				}

				// Add to the inventory container
				m_vpInventoryItemList.push_back(pNewItem);

				// Store the item mapping pointer, so we can easily get this when we need to find what items map to what slot positions
				m_ItemSlotMapping[i] = pNewItem;

				SetInventoryGUINeedsUpdate(true);
				SetCharacterGUINeedsUpdate(true);
			}
		}

		// Import the equipped items
		for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
		{
			importFile.getline(input, 128, '|');
			int itemValid = strtol(input, &pEnd, 10);

			if (itemValid == 1)
			{
				char fileName[128];
				importFile.getline(fileName, 128, '|');

				char iconFileName[128];
				importFile.getline(iconFileName, 128, '|');

				importFile.getline(input, 128, '|');
				int itemType = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int item = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int status = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int equipSlot = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int itemQuality = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int left = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int right = strtol(input, &pEnd, 10);

				char title[128];
				importFile.getline(title, 128, '|');

				char description[128];
				importFile.getline(description, 128, '|');

				float r;
				float g;
				float b;
				importFile.getline(input, 128, '|');
				r = static_cast<float>(strtod(input, &pEnd));
				importFile.getline(input, 128, '|');
				g = static_cast<float>(strtod(input, &pEnd));
				importFile.getline(input, 128, '|');
				b = static_cast<float>(strtod(input, &pEnd));

				float scale;
				float offsetX;
				float offsetY;
				float offsetZ;

				importFile.getline(input, 128, '|');
				scale = static_cast<float>(strtod(input, &pEnd));
				importFile.getline(input, 128, '|');
				offsetX = static_cast<float>(strtod(input, &pEnd));
				importFile.getline(input, 128, '|');
				offsetY = static_cast<float>(strtod(input, &pEnd));
				importFile.getline(input, 128, '|');
				offsetZ = static_cast<float>(strtod(input, &pEnd));

				importFile.getline(input, 128, '|');
				int quantity = strtol(input, &pEnd, 10);

				// Equip the item
				InventoryItem* pNewItem = new InventoryItem();

				pNewItem->m_fileName = fileName;
				
				pNewItem->m_iconFileName = iconFileName;

				pNewItem->m_title = title;
				pNewItem->m_description = description;

				pNewItem->m_itemType = static_cast<InventoryType>(itemType);
				pNewItem->m_item = static_cast<ItemType>(item);
				pNewItem->m_status = static_cast<ItemStatus>(status);
				pNewItem->m_equipSlot = static_cast<EquipSlot>(equipSlot);
				pNewItem->m_itemQuality = static_cast<ItemQuality>(itemQuality);

				pNewItem->m_left = static_cast<bool>(left);
				pNewItem->m_right = static_cast<bool>(right);

				pNewItem->m_placementR = r;
				pNewItem->m_placementG = g;
				pNewItem->m_placementB = b;

				pNewItem->m_scale = scale;
				pNewItem->m_offsetX = offsetX;
				pNewItem->m_offsetY = offsetY;
				pNewItem->m_offsetZ = offsetZ;

				pNewItem->m_quantity = quantity;

				pNewItem->m_equipped = false;

				pNewItem->m_remove = false;

				// Also import the stats attributes
				int numStatAttributes;
				importFile.getline(input, 128, '|');
				numStatAttributes = strtol(input, &pEnd, 10);
				for (int k = 0; k < numStatAttributes; k++)
				{
					int type;
					int amount;
					importFile.getline(input, 128, '|');
					type = strtol(input, &pEnd, 10);

					importFile.getline(input, 128, '|');
					amount = strtol(input, &pEnd, 10);

					pNewItem->AddStatAttribute(static_cast<AttributeType>(type), amount);
				}

				// Add to the inventory container
				m_vpInventoryItemList.push_back(pNewItem);

				m_equippedSlots[equipSlot] = pNewItem;
				m_equippedSlots[equipSlot]->m_equipped = true;

				m_pInventoryGUI->SetEquippedItem(static_cast<EquipSlot>(equipSlot), title);
				m_pPlayer->EquipItem(pNewItem);

				SetInventoryGUINeedsUpdate(true);
				SetCharacterGUINeedsUpdate(true);
			}
		}

		// Import the number of coins
		importFile.getline(input, 128, '|');
		m_numCoins = strtol(input, &pEnd, 10);
		m_coinsUpdated = true;

		// Import the file signature
		std::string tempString;
		std::string signature;
		importFile >> tempString >> signature;

		delete[] input;

		importFile.close();
	}
}

bool InventoryManager::IsInventoryFull() const
{
	for (int i = 0; i < MAX_NUM_INVENTORY_SLOTS; ++i)
	{
		if (m_ItemSlotMapping[i] == nullptr)
		{
			return false;
		}
	}

	return true;
}

InventoryItem* InventoryManager::CreateInventoryItem(InventoryItem* pItem)
{
	return CreateInventoryItem(pItem->m_fileName.c_str(), pItem->m_iconFileName.c_str(), pItem->m_itemType, pItem->m_item, pItem->m_status, pItem->m_equipSlot, pItem->m_itemQuality, pItem->m_left, pItem->m_right, pItem->m_title.c_str(), pItem->m_description.c_str(), pItem->m_placementR, pItem->m_placementG, pItem->m_placementB, pItem->m_quantity, pItem->m_lootSlotX, pItem->m_lootSlotY);
}

InventoryItem* InventoryManager::CreateInventoryItem(const char* fileName, const char* iconFileName, InventoryType itemType, ItemType item, ItemStatus status, EquipSlot equipSlot, ItemQuality itemQuality, bool left, bool right, const char* title, const char* description, float r, float g, float b, int quantity, int lootSlotX, int lootSlotY)
{
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

	pNewItem->m_scale = 0.5f;
	pNewItem->m_offsetX = 0.5f;
	pNewItem->m_offsetY = 0.5f;
	pNewItem->m_offsetZ = 0.5f;

	pNewItem->m_lootSlotX = lootSlotX;
	pNewItem->m_lootSlotY = lootSlotY;

	pNewItem->m_equipped = false;

	pNewItem->m_quantity = quantity;

	pNewItem->m_remove = false;

	m_vpOtherInventoryItemList.push_back(pNewItem);

	return pNewItem;
}

InventoryItem* InventoryManager::CreateInventoryItemForCrafting(ItemType item, int quantity, ItemQuality itemQuality)
{
	std::string fileName = GetItemFileNameForType(item);
	std::string textureFileName = GetItemTextureForType(item);
	std::string title = GetItemTitleForType(item);
	std::string desscription = GetItemDescriptionForType(item);

	return CreateInventoryItem(fileName.c_str(), textureFileName.c_str(), InventoryType::Item, item, ItemStatus::None, EquipSlot::NoSlot, itemQuality, false, false, title.c_str(), desscription.c_str(), 1.0f, 1.0f, 1.0f, quantity, -1, -1);
}

InventoryItem* InventoryManager::CreateEquipmentItemFromType(EquipmentType equipment)
{
	std::string fileName = GetEquipmentFileNameForType(equipment);
	std::string textureFileName = GetEquipmentTextureForType(equipment);
	std::string title = GetEquipmentTitleForType(equipment);
	std::string desscription = GetEquipmentDescriptionForType(equipment);
	InventoryType type = GetInventoryTypeForEquipment(equipment);
	EquipSlot slot = GetEquipSlotForEquipment(equipment);
	ItemQuality quality = GetItemQualityForEquipment(equipment);
	bool left;
	bool right;
	GetItemSidesForEquipment(equipment, &left, &right);

	InventoryItem* pInventoryItem = CreateInventoryItem(fileName.c_str(), textureFileName.c_str(), type, ItemType::None, ItemStatus::None, slot, quality, left, right, title.c_str(), desscription.c_str(), 1.0f, 1.0f, 1.0f, -1, -1, -1);

	// Add the stats modifiers for the equipment type
	AddStatsModifiersForType(equipment, pInventoryItem);

	return pInventoryItem;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
ItemTextData* InventoryManager::GetItemTextData(ItemType item) const
{
	for (int i = 0; i < (sizeof g_itemData / sizeof g_itemData[0]); ++i)
	{
		if (item == g_itemData[i].m_item)
		{
			return &g_itemData[i];
		}
	}

	return nullptr;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool InventoryManager::CanDestroyItemWithHammer(ItemType item) const
{
	for (int i = 0; i < (sizeof g_itemData / sizeof g_itemData[0]); ++i)
	{
		if (item == g_itemData[i].m_item)
		{
			return true;
		}
	}

	return false;
}

bool InventoryManager::CanAddInventoryItem(const char* title, ItemType item, int quantity)
{
	bool addToExistingItem = false;
	if (quantity != -1)
	{
		addToExistingItem = true;
	}

	if (addToExistingItem)
	{
		// First check if the item already exists in the inventory slots, to add to its quantity
		for (int i = 0; i < MAX_NUM_INVENTORY_SLOTS; ++i)
		{
			if (m_ItemSlotMapping[i] != nullptr && (strcmp(title, m_ItemSlotMapping[i]->m_title.c_str()) == 0) && (m_ItemSlotMapping[i]->m_item == item || item == ItemType::DroppedItem))
			{
				return true;
			}
		}

		// Next check the equipped slots
		for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
		{
			InventoryItem* pItem = m_equippedSlots[i];

			if (pItem != nullptr && (strcmp(title, pItem->m_title.c_str()) == 0) && (m_ItemSlotMapping[i]->m_item == item || item == ItemType::DroppedItem))
			{
				return true;
			}
		}
	}

	// Search for a free slot
	for (int i = 0; i < MAX_NUM_INVENTORY_SLOTS; ++i)
	{
		if (m_ItemSlotMapping[i] == nullptr)
		{
			return true;
		}
	}

	return false;
}

InventoryItem* InventoryManager::AddInventoryItem(const char* fileName, const char* iconFileName, InventoryType itemType, ItemType item, ItemStatus status, EquipSlot equipSlot, ItemQuality itemQuality, bool left, bool right, const char* title, const char* description, float r, float g, float b, int quantity, int lootSlotX, int lootSlotY, int setInventorySlotX, int setInventorySlotY)
{
	bool addToExistingItem = false;
	if (quantity != -1)
	{
		addToExistingItem = true;
	}

	// First check if the item already exists in the inventory slots, to add to its quantity
	if (addToExistingItem)
	{
		for (int i = 0; i < MAX_NUM_INVENTORY_SLOTS; ++i)
		{
			if (m_ItemSlotMapping[i] != nullptr && (strcmp(title, m_ItemSlotMapping[i]->m_title.c_str()) == 0) && m_ItemSlotMapping[i]->m_itemType == itemType)
			{
				m_ItemSlotMapping[i]->m_quantity += quantity;

				SetInventoryGUINeedsUpdate(true);
				SetCharacterGUINeedsUpdate(true);

				// Export the inventory data since we have changed an item quantity
				ExportInventory(m_playerName);

				return m_ItemSlotMapping[i];
			}
		}

		// Next check the equipped slots
		for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
		{
			InventoryItem* pItem = m_equippedSlots[i];

			if (pItem != nullptr && (strcmp(title, pItem->m_title.c_str()) == 0) && m_equippedSlots[i]->m_itemType == itemType)
			{
				pItem->m_quantity += quantity;

				SetInventoryGUINeedsUpdate(true);
				SetCharacterGUINeedsUpdate(true);

				// Export the inventory data since we have changed an item quantity
				ExportInventory(m_playerName);

				return pItem;
			}
		}
	}

	// First find any empty slot in the inventory
	int indexToUse = -1;

	if (setInventorySlotX != -1 && setInventorySlotY != -1)
	{
		// We have specified an index to use
		int convertedIndex = ConvertSlotsToIndex(setInventorySlotX, setInventorySlotY);
		indexToUse = convertedIndex;
	}
	else
	{
		// Search for a free slot
		for (int i = 0; i < MAX_NUM_INVENTORY_SLOTS; ++i)
		{
			if (m_ItemSlotMapping[i] == nullptr)
			{
				indexToUse = i;
				break;
			}
		}
	}

	// Create the new item
	if (indexToUse != -1)
	{
		if (m_ItemSlotMapping[indexToUse] == nullptr)
		{
			// Create the new inventory item
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

			pNewItem->m_scale = 0.5f;
			pNewItem->m_offsetX = 0.5f;
			pNewItem->m_offsetY = 0.5f;
			pNewItem->m_offsetZ = 0.5f;

			pNewItem->m_lootSlotX = lootSlotX;
			pNewItem->m_lootSlotY = lootSlotY;

			pNewItem->m_equipped = false;

			pNewItem->m_quantity = quantity;

			pNewItem->m_remove = false;

			// Add to the inventory container
			m_vpInventoryItemList.push_back(pNewItem);

			// Store the item mapping pointer, so we can easily get this when we need to find what items map to what slot positions
			m_ItemSlotMapping[indexToUse] = pNewItem;

			SetInventoryGUINeedsUpdate(true);
			SetCharacterGUINeedsUpdate(true);

			// Export the inventory data since we have added an item
			ExportInventory(m_playerName);

			return pNewItem;
		}
	}

	return nullptr;
}

InventoryItem* InventoryManager::AddInventoryItem(InventoryItem* pInventoryItem, int inventoryX, int inventoryY)
{
	InventoryItem* pItem = AddInventoryItem(pInventoryItem->m_fileName.c_str(), pInventoryItem->m_iconFileName.c_str(), pInventoryItem->m_itemType, pInventoryItem->m_item, pInventoryItem->m_status, pInventoryItem->m_equipSlot, pInventoryItem->m_itemQuality, pInventoryItem->m_left, pInventoryItem->m_right, pInventoryItem->m_title.c_str(), pInventoryItem->m_description.c_str(), pInventoryItem->m_placementR, pInventoryItem->m_placementG, pInventoryItem->m_placementB, pInventoryItem->m_quantity, pInventoryItem->m_lootSlotX, pInventoryItem->m_lootSlotY, inventoryX, inventoryY);

	for (size_t i = 0; i < pInventoryItem->m_vpStatAttributes.size(); ++i)
	{
		pItem->AddStatAttribute(pInventoryItem->m_vpStatAttributes[i]->GetType(), pInventoryItem->m_vpStatAttributes[i]->GetModifyAmount());
	}

	pItem->m_scale = pInventoryItem->m_scale;
	pItem->m_offsetX = pInventoryItem->m_offsetX;
	pItem->m_offsetY = pInventoryItem->m_offsetY;
	pItem->m_offsetZ = pInventoryItem->m_offsetZ;

	return pItem;
}

void InventoryManager::RemoveInventoryItem(const char* title, ItemType item, int quantity)
{
	for (int i = 0; i < MAX_NUM_INVENTORY_SLOTS; ++i)
	{
		if (m_ItemSlotMapping[i] != nullptr && (strcmp(title, m_ItemSlotMapping[i]->m_title.c_str()) == 0) && m_ItemSlotMapping[i]->m_item == item)
		{
			m_ItemSlotMapping[i]->m_quantity -= quantity;

			if (m_ItemSlotMapping[i]->m_quantity <= 0)
			{
				RemoveInventoryItem(i);
			}

			SetInventoryGUINeedsUpdate(true);
			SetCharacterGUINeedsUpdate(true);

			// Export the inventory data since we have changed an item quantity
			ExportInventory(m_playerName);

			return;
		}
	}

	// Next check the equipped slots
	for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
	{
		InventoryItem* pItem = m_equippedSlots[i];

		if (pItem != nullptr && (strcmp(title, pItem->m_title.c_str()) == 0) && m_equippedSlots[i]->m_item == item)
		{
			pItem->m_quantity -= quantity;

			if (pItem->m_quantity <= 0)
			{
				m_pPlayer->UnequipItem(static_cast<EquipSlot>(i), false, false);
				RemoveInventoryItem(static_cast<EquipSlot>(i));
			}

			SetInventoryGUINeedsUpdate(true);
			SetCharacterGUINeedsUpdate(true);

			// Export the inventory data since we have changed an item quantity
			ExportInventory(m_playerName);

			return;
		}
	}
}

void InventoryManager::RemoveInventoryItem(EquipSlot equipSlot)
{
	m_equippedSlots[static_cast<int>(equipSlot)] = nullptr;

	SetInventoryGUINeedsUpdate(true);
	SetCharacterGUINeedsUpdate(true);

	// Export the inventory data since we have removed an item
	ExportInventory(m_playerName);
}

void InventoryManager::RemoveInventoryItem(int slotIndex)
{
	InventoryItem* pNewItem = m_ItemSlotMapping[slotIndex];
	pNewItem->m_remove = true;
	m_ItemSlotMapping[slotIndex] = nullptr;

	SetInventoryGUINeedsUpdate(true);
	SetCharacterGUINeedsUpdate(true);

	// Export the inventory data since we have removed an item
	ExportInventory(m_playerName);
}

void InventoryManager::RemoveInventoryItem(int xPos, int yPos)
{
	int index = ConvertSlotsToIndex(xPos, yPos);
	RemoveInventoryItem(index);
}

InventoryItem* InventoryManager::GetInventoryItemForSlot(int slotIndex)
{
	return m_ItemSlotMapping[slotIndex];
}

InventoryItem* InventoryManager::GetInventoryItemForSlot(int xPos, int yPos)
{
	int index = ConvertSlotsToIndex(xPos, yPos);
	return GetInventoryItemForSlot(index);
}

InventoryItem* InventoryManager::GetInventoryItemWithTitle(std::string title)
{
	for (size_t i = 0; i < m_vpInventoryItemList.size(); ++i)
	{
		if (m_vpInventoryItemList[i] != nullptr)
		{
			if (strcmp(m_vpInventoryItemList[i]->m_title.c_str(), title.c_str()) == 0)
			{
				return m_vpInventoryItemList[i];
			}
		}
	}

	return nullptr;
}

void InventoryManager::SwitchInventoryItems(int slot1, int slot2)
{
	InventoryItem* pTempItem = m_ItemSlotMapping[slot1];
	m_ItemSlotMapping[slot1] = m_ItemSlotMapping[slot2];
	m_ItemSlotMapping[slot2] = pTempItem;

	// Export the inventory data since we have switched items
	ExportInventory(m_playerName);
}

void InventoryManager::SwitchInventoryItems(int x1, int y1, int x2, int y2)
{
	int index1 = ConvertSlotsToIndex(x1, y1);
	int index2 = ConvertSlotsToIndex(x2, y2);
	SwitchInventoryItems(index1, index2);
}

void InventoryManager::SwitchEquippedSlots(EquipSlot equipSlotSrc, EquipSlot equipSlotDst)
{
	InventoryItem* temp = m_equippedSlots[static_cast<int>(equipSlotDst)];
	m_equippedSlots[static_cast<int>(equipSlotDst)] = m_equippedSlots[static_cast<int>(equipSlotSrc)];
	m_equippedSlots[static_cast<int>(equipSlotSrc)] = temp;
}

void InventoryManager::EquipInventoryItem(int slotIndex, EquipSlot equipSlot)
{
	InventoryItem* pEquippedItem = m_equippedSlots[static_cast<int>(equipSlot)];

	// Equip the new item
	m_equippedSlots[static_cast<int>(equipSlot)] = m_ItemSlotMapping[slotIndex];
	m_equippedSlots[static_cast<int>(equipSlot)]->m_equipped = true;

	// If we already have an item equipped in this slot, unequip it
	m_ItemSlotMapping[slotIndex] = pEquippedItem;
	if (m_ItemSlotMapping[slotIndex] != nullptr)
	{
		m_ItemSlotMapping[slotIndex]->m_equipped = false;
	}

	// Refresh stat modifier cache since we have equipped an item
	m_pPlayer->RefreshStatModifierCacheValues();

	// Export the inventory data since we have equipped an item
	ExportInventory(m_playerName);
}

void InventoryManager::EquipInventoryItem(int xPos, int yPos, EquipSlot equipSlot)
{
	int index = ConvertSlotsToIndex(xPos, yPos);
	EquipInventoryItem(index, equipSlot);
}

void InventoryManager::EquipLootItem(int xPos, int yPos, InventoryItem* pInventoryItem, EquipSlot equipSlot)
{
	InventoryItem* newItem = CreateInventoryItem(pInventoryItem);
	newItem->m_scale = pInventoryItem->m_scale;
	newItem->m_offsetX = pInventoryItem->m_offsetX;
	newItem->m_offsetY = pInventoryItem->m_offsetY;
	newItem->m_offsetZ = pInventoryItem->m_offsetZ;

	for (size_t i = 0; i < pInventoryItem->m_vpStatAttributes.size(); ++i)
	{
		newItem->AddStatAttribute(pInventoryItem->m_vpStatAttributes[i]->GetType(), pInventoryItem->m_vpStatAttributes[i]->GetModifyAmount());
	}

	// Equip the new item
	m_equippedSlots[static_cast<int>(equipSlot)] = newItem;
	m_equippedSlots[static_cast<int>(equipSlot)]->m_equipped = true;

	// Refresh stat modifier cache since we have equipped an item
	m_pPlayer->RefreshStatModifierCacheValues();

	// Export the inventory data since we have equipped an item
	ExportInventory(m_playerName);
}

InventoryItem* InventoryManager::GetInventoryItemForEquipSlot(EquipSlot equipSlot)
{
	return m_equippedSlots[static_cast<int>(equipSlot)];
}

bool InventoryManager::IsEquippedStatus(ItemStatus status) const
{
	for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
	{
		if (m_equippedSlots[i] != nullptr)
		{
			if (m_equippedSlots[i]->m_status == status)
			{
				return true;
			}
		}
	}

	return false;
}

void InventoryManager::UnequipItem(int xPos, int yPos, EquipSlot equipSlot)
{
	int index = ConvertSlotsToIndex(xPos, yPos);
	InventoryItem* pTemp = m_ItemSlotMapping[index];

	InventoryItem* pEquippedItem = m_equippedSlots[static_cast<int>(equipSlot)];
	m_equippedSlots[static_cast<int>(equipSlot)]->m_equipped = false;

	m_ItemSlotMapping[index] = pEquippedItem;
	m_equippedSlots[static_cast<int>(equipSlot)] = pTemp;
	if (m_equippedSlots[static_cast<int>(equipSlot)] != nullptr)
	{
		m_equippedSlots[static_cast<int>(equipSlot)]->m_equipped = true;
	}

	// Refresh stat modifier cache since we have un-equipped an item
	m_pPlayer->RefreshStatModifierCacheValues();

	// Export the inventory data since we unequipped an item
	ExportInventory(m_playerName);
}

void InventoryManager::UnequipItemToLootGUI(EquipSlot equipSlot)
{
	m_equippedSlots[static_cast<int>(equipSlot)]->m_equipped = false;
	m_equippedSlots[static_cast<int>(equipSlot)] = nullptr;

	// Refresh stat modifier cache since we have un-equipped an item
	m_pPlayer->RefreshStatModifierCacheValues();

	// Export the inventory data since we unequipped an item
	ExportInventory(m_playerName);
}

bool InventoryManager::UnequipItemToFreeInventorySlot(EquipSlot equipSlot, int* newSlotX, int* newSlotY)
{
	for (int i = 0; i < MAX_NUM_INVENTORY_SLOTS; ++i)
	{
		if (m_ItemSlotMapping[i] == nullptr)
		{
			InventoryItem* pEquippedItem = m_equippedSlots[static_cast<int>(equipSlot)];
			if (pEquippedItem != nullptr)
			{
				m_equippedSlots[static_cast<int>(equipSlot)]->m_equipped = false;
				m_equippedSlots[static_cast<int>(equipSlot)] = nullptr;

				m_ItemSlotMapping[i] = pEquippedItem;

				*newSlotX = i % MAX_NUM_SLOTS_HORIZONTAL;
				*newSlotY = i / MAX_NUM_SLOTS_HORIZONTAL;

				// Refresh stat modifier cache since we have un-equipped an item
				m_pPlayer->RefreshStatModifierCacheValues();

				// Export the inventory data since we unequipped an item
				ExportInventory(m_playerName);
			}

			return true;
		}
	}

	*newSlotX = -1;
	*newSlotY = -1;

	return false;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
int InventoryManager::ConvertSlotsToIndex(int x, int y) const
{
	return (MAX_NUM_SLOTS_HORIZONTAL * y) + x;
}

// Coins
int InventoryManager::GetNumCoins() const
{
	return m_numCoins;
}

void InventoryManager::GiveCoins(int numCoins)
{
	m_numCoins += numCoins;

	m_coinsUpdated = true;

	m_InventoryGUINeedsUpdate = true;
	m_CharacterGUINeedsUpdate = true;
}

void InventoryManager::TakeCoins(int numCoins)
{
	m_numCoins -= numCoins;

	m_coinsUpdated = true;
}

bool InventoryManager::CoinsUpdated() const
{
	return m_coinsUpdated;
}

void InventoryManager::SetCoinsUpdated(bool set)
{
	m_coinsUpdated = set;
}

bool IsNeedRemove(InventoryItem* item)
{
	bool needErase = item->m_remove;

	if (needErase == true)
	{
		delete item;
	}

	return needErase;
}

void InventoryManager::Update()
{
	// Remove any items that need to be removed from the vector container
	m_vpInventoryItemList.erase(remove_if(m_vpInventoryItemList.begin(), m_vpInventoryItemList.end(), IsNeedRemove), m_vpInventoryItemList.end());
}