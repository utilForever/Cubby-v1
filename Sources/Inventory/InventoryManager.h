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

class InventoryItem
{

};

using InventoryItemList = std::vector<InventoryItem*>;

class InventoryManager
{

};

#endif