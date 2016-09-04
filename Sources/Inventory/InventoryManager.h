/*************************************************************************
> File Name: InventoryManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The inventory manager is linked to the player and holds the information
>    about what items the player has inside their inventory and also what
>    items the player has equipped. The internal state of the inventory and
>    equipped items gets changed when the player modifies the InventoryGUI
>    and characterGUI.
> Created Time: 2016/07/24
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_INVENTORY_MANAGER_H
#define CUBBY_INVENTORY_MANAGER_H

#include <vector>

#include <Items/EquipmentEnum.h>
#include <Items/ItemsEnum.h>
#include <Items/StatAttribute.h>
#include <Player/PlayerClass.h>

// Forward declaration
class Player;
class InventoryGUI;
class LootGUI;
class ActionBar;

using StatAttributeList = std::vector<StatAttribute*>;

// Item types
enum class InventoryType
{
	Nothing = 0,

	Item,
	Scenery,
	Block,

	Clothing,

	WeaponSword,
	WeaponAxe,
	WeaponHammer,
	WeaponMace,
	WeaponDagger,
	WeaponSickle,
	WeaponTwoHandedSword,
	WeaponShield,
	WeaponBoomerang,
	WeaponBomb,
	WeaponBow,
	WeaponStaff,
	WeaponWand,
	WeaponPickaxe,
	WeaponTorch,
	WeaponSpellHands,

	Consumable,
};

// Equipment slots
enum class EquipSlot
{
	NoSlot = 0,

	LeftHand,
	RightHand,

	Head,
	Shoulders,
	Body,
	Legs,
	Hand,
	Feet,
	Accessory1,
	Accessory2,

	NumSlots,
};

// Quality
enum class ItemQuality
{
	Common = 0,
	Uncommon,
	Magical,
	Rare,
	Epic,
};

// Status effects
enum class ItemStatus
{
	None = 0,
	Fire,
	Ice,
	Poison,
	Exploding,
};

struct ItemTextData
{
	ItemType m_item;
	std::string m_fileName;
	std::string m_title;
	std::string m_description;
};

struct InventoryItem
{
	// Destructor
	~InventoryItem()
	{
		ClearAttributes();
	}

	void ClearAttributes()
	{
		for (size_t i = 0; i < m_vpStatAttributes.size(); ++i)
		{
			delete m_vpStatAttributes[i];
			m_vpStatAttributes[i] = nullptr;
		}

		m_vpStatAttributes.clear();
	}

	void AddStatAttribute(AttributeType type, int amount)
	{
		StatAttribute* pAttribute = new StatAttribute(type, amount);

		m_vpStatAttributes.push_back(pAttribute);
	}

	std::string GetStatsAttributeString()
	{
		std::string returnString = "";

		if (m_vpStatAttributes.size() > 0)
		{
			returnString += "\n";
		}

		for (size_t i = 0; i < m_vpStatAttributes.size(); ++i)
		{
			std::string statAttribute = m_vpStatAttributes[i]->GetString();

			returnString += statAttribute;

			if (i < m_vpStatAttributes.size() - 1)
			{
				returnString += "\n";
			}
		}

		return returnString;
	}

	// Filename
	std::string m_fileName;

	// Icon filename for GUI
	std::string m_iconFileName;

	// Title
	std::string m_title;

	// Description
	std::string m_description;

	// Item type
	InventoryType m_itemType;

	// Item
	ItemType m_item;

	// Status effects
	ItemStatus m_status;

	// Equip slot
	EquipSlot m_equipSlot;

	// Quality
	ItemQuality m_itemQuality;

	// Display left or right sprites
	bool m_left;
	bool m_right;

	// Placement block color
	float m_placementR;
	float m_placementG;
	float m_placementB;

	// Loot GUI placement
	int m_lootSlotX;
	int m_lootSlotY;

	// Is the item equipped or not
	bool m_equipped;

	// Scale and offset
	float m_scale;
	float m_offsetX;
	float m_offsetY;
	float m_offsetZ;

	// Quantity
	int m_quantity;

	// Stat modifiers
	StatAttributeList m_vpStatAttributes;

	// Erase flag
	bool m_remove;
};

using InventoryItemList = std::vector<InventoryItem*>;

class InventoryManager
{
public:
	// Constructor, Destructor
	InventoryManager();
	~InventoryManager();

	// Linkage
	void SetPlayer(Player* pPlayer);
	void SetInventoryGUI(InventoryGUI* pInventoryGUI);
	void SetLootGUI(LootGUI* pLootGUI);
	void SetActionBar(ActionBar* pActionBar);

	void Reset();

	void ClearInventory();
	void ClearEquipped();
	void ClearOtherCreatedItems();
	void LoadInventory(std::string playerName, PlayerClass playerClass, bool exportInventoryFile);
	void LoadDefaultInventory();
	void LoadInventoryForClass(PlayerClass playerClass);

	void SetInventoryGUINeedsUpdate(bool update);
	bool InventoryGUINeedsUpdate() const;

	void SetCharacterGUINeedsUpdate(bool update);
	bool CharacterGUINeedsUpdate() const;

	// Supress export
	void SetSupressExport(bool supress);

	// Import / Export inventory
	void ExportInventory(std::string playerName);
	void ImportInventory(std::string playerName);

	bool IsInventoryFull() const;

	ItemTextData* GetItemTextData(ItemType item) const;
	bool CanDestroyItemWithHammer(ItemType item) const;

	bool CanAddInventoryItem(const char* title, ItemType item, int quantity);

	InventoryItem* CreateInventoryItem(InventoryItem* pItem);
	InventoryItem* CreateInventoryItem(const char* fileName, const char* iconFileName, InventoryType itemType, ItemType item, ItemStatus status, EquipSlot equipSlot, ItemQuality itemQuality, bool left, bool right, const char* title, const char* description, float r, float g, float b, int quantity, int lootSlotX, int lootSlotY);
	InventoryItem* CreateInventoryItemForCrafting(ItemType item, int quantity, ItemQuality itemQuality);
	InventoryItem* CreateEquipmentItemFromType(EquipmentType equipment);

	InventoryItem* AddInventoryItem(const char* fileName, const char* iconFileName, InventoryType itemType, ItemType item, ItemStatus status, EquipSlot equipSlot, ItemQuality itemQuality, bool left, bool right, const char* title, const char* description, float r, float g, float b, int quantity, int lootSlotX, int lootSlotY, int setInventorySlotX, int setInventorySlotY);
	InventoryItem* AddInventoryItem(InventoryItem* pInventoryItem, int inventoryX, int inventoryY);

	void RemoveInventoryItem(const char* title, ItemType item, int quantity);
	void RemoveInventoryItem(EquipSlot equipSlot);
	void RemoveInventoryItem(int slotIndex);
	void RemoveInventoryItem(int xPos, int yPos);

	InventoryItem* GetInventoryItemForSlot(int slotIndex);
	InventoryItem* GetInventoryItemForSlot(int xPos, int yPos);
	InventoryItem* GetInventoryItemWithTitle(std::string title);

	void SwitchInventoryItems(int slot1, int slot2);
	void SwitchInventoryItems(int x1, int y1, int x2, int y2);

	void SwitchEquippedSlots(EquipSlot equipSlotSrc, EquipSlot equipSlotDst);

	void EquipInventoryItem(int slotIndex, EquipSlot equipSlot);
	void EquipInventoryItem(int xPos, int yPos, EquipSlot equipSlot);

	void EquipLootItem(int xPos, int yPos, InventoryItem* pInventoryItem, EquipSlot equipSlot);

	void UnequipItem(int xPos, int yPos, EquipSlot equipSlot);
	void UnequipItemToLootGUI(EquipSlot equipSlot);
	bool UnequipItemToFreeInventorySlot(EquipSlot equipSlot, int* newSlotX, int* newSlotY);

	InventoryItem* GetInventoryItemForEquipSlot(EquipSlot equipSlot);

	bool IsEquippedStatus(ItemStatus status) const;

	// Coins
	int GetNumCoins() const;
	void GiveCoins(int numCoins);
	void TakeCoins(int numCoins);
	bool CoinsUpdated() const;
	void SetCoinsUpdated(bool set);

	void Update();

	// Constants
	static const int MAX_NUM_SLOTS_HORIZONTAL = 6;
	static const int MAX_NUM_SLOTS_VERTICAL = 3;
	static const int MAX_NUM_INVENTORY_SLOTS = MAX_NUM_SLOTS_HORIZONTAL * MAX_NUM_SLOTS_VERTICAL;

protected:
	int ConvertSlotsToIndex(int x, int y) const;

private:
	bool m_InventoryGUINeedsUpdate;
	bool m_CharacterGUINeedsUpdate;

	// Player name
	std::string m_playerName;

	// List of inventory items in the GUI and equipped
	InventoryItemList m_vpInventoryItemList;

	// Slot mapping for inventory
	InventoryItem* m_ItemSlotMapping[MAX_NUM_INVENTORY_SLOTS];

	// SLot mapping for equipped items
	InventoryItem* m_equippedSlots[static_cast<int>(EquipSlot::NumSlots)];

	// List for deletion purposes, since inventory items get created for random loot and crafting and are stored in this manager.
	InventoryItemList m_vpOtherInventoryItemList;

	// Coins
	int m_numCoins;
	bool m_coinsUpdated;

	// Force to stop export inventory file
	bool m_supressExport;

	Player* m_pPlayer;
	InventoryGUI* m_pInventoryGUI;
	LootGUI* m_pLootGUI;
	ActionBar* m_pActionBar;
};

#endif