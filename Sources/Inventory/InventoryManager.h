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

#include "ItemsEnum.h"
#include "StatAttribute.h"

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
	Weapon2HandedSword,
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
	std::string m_IconfileName;

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

	// Placement block colour
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

};

#endif