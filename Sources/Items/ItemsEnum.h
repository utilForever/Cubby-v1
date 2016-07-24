/*************************************************************************
> File Name: ItemsEnum.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    An enum list of all of the items in the game, used for item tracking
>    and item specific functionality.
> Created Time: 2016/07/21
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ITEMS_ENUM_H
#define CUBBY_ITEMS_ENUM_H

enum class ItemType
{
	None = 0,

	// Dropped item in the world
	DroppedItem,

	// Herts and coins dropped from killing enemies
	Coin,
	Heart,

	// Zombie gib
	Gib,

	// Interactable items
	Tombstone,
	Chest,
	Torch,
	Furnace,
	Anvil,
	QuestBoard,
	CampFire,
	Mannequin,

	// Ingredients
	SlimeJelly,
	BeeWing,
	Bone,

	// Ore and mining
	CopperVein,
	CopperOre,
	CopperBar,
	IronVein,
	IronOre,
	IronBar,
	SilverVein,
	SilverOre,
	SilverBar,
	GoldVein,
	GoldOre,
	GoldBar,

	// World blocks
	BlockGrass,
	BlockDirt,
	BlockStone,
	BlockWood,
	BlockLeaf,
	BlockSand,
	BlockCactus,
	BlockRock,
	BlockSnow,

	NUM_ITEMS,
};

#endif