/*************************************************************************
> File Name: ItemUtils.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Utility functionality for items, mostly a way to associate item enums with
>    data like the friendly names and display strings.
> Created Time: 2016/07/31
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Item.h"

std::string GetItemTitleForType(ItemType type)
{
	switch (type)
	{
	case ItemType::None: { return "None"; }
	// Dropped item in the world
	case ItemType::DroppedItem: { return "Dropped Item"; }
	// Hearts and coins dropped from killing enemies
	case ItemType::Coin: { return "Coin"; }
	case ItemType::Heart: { return "Heart"; }
	// Zombie gib
	case ItemType::Gib: { return "Gib"; }
	// Interact items
	case ItemType::Tombstone: { return "Tombstone"; }
	case ItemType::Chest: { return "Chest"; }
	case ItemType::Torch: { return "Torch"; }
	case ItemType::Furnace: { return "Furnace"; }
	case ItemType::Anvil: { return "Anvil"; }
	case ItemType::QuestBoard: { return "Quest Board"; }
	case ItemType::CampFire: { return "Camp Fire"; }
	case ItemType::Mannequin: { return "Mannequin"; }
	// Ingredients
	case ItemType::SlimeJelly: { return "Slime Jelly"; }
	case ItemType::BeeWing: { return "Bee Wing"; }
	case ItemType::Bone: { return "Bone"; }
	// Ore and mining
	case ItemType::CopperVein: { return "Copper Vein"; }
	case ItemType::CopperOre: { return "Copper Ore"; }
	case ItemType::CopperBar: { return "Copper Bar"; }
	case ItemType::IronVein: { return "Iron Vein"; }
	case ItemType::IronOre: { return "Iron Ore"; }
	case ItemType::IronBar: { return "Iron Bar"; }
	case ItemType::SilverVein: { return "Silver Vein"; }
	case ItemType::SilverOre: { return "Silver Ore"; }
	case ItemType::SilverBar: { return "Silver Bar"; }
	case ItemType::GoldVein: { return "Gold Vein"; }
	case ItemType::GoldOre: { return "Gold Ore"; }
	case ItemType::GoldBar: { return "Gold Bar"; }
		// World blocks
	case ItemType::BlockGrass: { return "Grass Block"; }
	case ItemType::BlockDirt: { return "Dirt Block"; }
	case ItemType::BlockStone: { return "Stone Block"; }
	case ItemType::BlockWood: { return "Wood Block"; }
	case ItemType::BlockLeaf: { return "Leaf Block"; }
	case ItemType::BlockSand: { return "Sand Block"; }
	case ItemType::BlockCactus: { return "Cactus Block"; }
	case ItemType::BlockRock: { return "Rock Block"; }
	case ItemType::BlockSnow: { return "Snow Block"; }
	case ItemType::NUM_ITEMS: throw std::logic_error("Invalid ItemType!");
	default: throw std::logic_error("Invalid ItemType!");
	}
}

std::string GetItemDescriptionForType(ItemType type)
{
	switch (type)
	{
	case ItemType::None: { return "None"; }
	// Dropped item in the world
	case ItemType::DroppedItem: { return "Dropped Item"; }
	// Hearts and coins dropped from killing enemies
	case ItemType::Coin: { return "Coin"; }
	case ItemType::Heart: { return "Heart"; }
	// Zombie gib
	case ItemType::Gib: { return "Gib"; }
	// Interact items
	case ItemType::Tombstone: { return "Tombstone"; }
	case ItemType::Chest: { return "Chest"; }
	case ItemType::Torch: { return "Torch"; }
	case ItemType::Furnace: { return "Furnace"; }
	case ItemType::Anvil: { return "Anvil"; }
	case ItemType::QuestBoard: { return "Quest Board"; }
	case ItemType::CampFire: { return "Camp Fire"; }
	case ItemType::Mannequin: { return "A wooden mannequin, used for storing armor sets."; }
	// Ingredients
	case ItemType::SlimeJelly: { return "Jelly from a slime monster."; }
	case ItemType::BeeWing: { return "A wing from a bee."; }
	case ItemType::Bone: { return "A raw bone from a skeleton."; }
	// Ore and mining
	case ItemType::CopperVein: { return "Copper Vein"; }
	case ItemType::CopperOre: { return "A nugget of copper ore. Can be smelted down at a furnace to create copper bars."; }
	case ItemType::CopperBar: { return "A solid bar of copper, used for crafting items made out of copper."; }
	case ItemType::IronVein: { return "Iron Vein"; }
	case ItemType::IronOre: { return "A nugget of iron ore. Can be smelted down at a furnace to create iron bars."; }
	case ItemType::IronBar: { return "A solid bar of iron, used for crafting items made out of iron."; }
	case ItemType::SilverVein: { return "Silver Vein"; }
	case ItemType::SilverOre: { return "A nugget of silver ore. Can be smelted down at a furnace to create silver bars."; }
	case ItemType::SilverBar: { return "A solid bar of silver, used for crafting items made out of silver."; }
	case ItemType::GoldVein: { return "Gold Vein"; }
	case ItemType::GoldOre: { return "A nugget of gold ore. Can be smelted down at a furnace to create gold bars."; }
	case ItemType::GoldBar: { return "A solid bar of gold, used for crafting items made out of gold."; }
	// World blocks
	case ItemType::BlockGrass: { return "Grass block for world building."; }
	case ItemType::BlockDirt: { return "Dirt block for world building."; }
	case ItemType::BlockStone: { return "Stone block for world building."; }
	case ItemType::BlockWood: { return "Wood block for world building."; }
	case ItemType::BlockLeaf: { return "Leaf block for world building."; }
	case ItemType::BlockSand: { return "Sand block for world building."; }
	case ItemType::BlockCactus: { return "Cactus block for world building."; }
	case ItemType::BlockRock: { return "Rock block for world building."; }
	case ItemType::BlockSnow: { return "Snow block for world building."; }
	case ItemType::NUM_ITEMS: throw std::logic_error("Invalid ItemType!");
	default: throw std::logic_error("Invalid ItemType!");
	}
}

std::string GetItemFileNameForType(ItemType type)
{
	switch (type)
	{
	case ItemType::None: { return ""; }
	// Dropped item in the world
	case ItemType::DroppedItem: { return ""; }
	// Hearts and coins dropped from killing enemies
	case ItemType::Coin: { return "Resources/gamedata/items/Coin/Coin.item"; }
	case ItemType::Heart: { return "Resources/gamedata/items/Heart/Heart.item"; }
	// Zombie gib
	case ItemType::Gib: { return "Resources/gamedata/items/Hand1Gib.item"; }
	// Interact items
	case ItemType::Tombstone: { return "Resources/gamedata/items/Tombstone/Tombstone1.item"; }
	case ItemType::Chest: { return "Resources/gamedata/items/Chest/Chest.item"; }
	case ItemType::Torch: { return "Resources/gamedata/items/Torch/Torch.item"; }
	case ItemType::Furnace: { return "Resources/gamedata/items/Furnace/Furnace.item"; }
	case ItemType::Anvil: { return "Resources/gamedata/items/Anvil/Anvil.item"; }
	case ItemType::QuestBoard: { return "Resources/gamedata/items/QuestBoard/QuestBoard.item"; }
	case ItemType::CampFire: { return "Resources/gamedata/items/CampFire/CampFire.item"; }
	case ItemType::Mannequin: { return "Resources/gamedata/items/Mannequin/Mannequin.item"; }
	// Ingredients
	case ItemType::SlimeJelly: { return "Resources/gamedata/items/SlimeJelly/SlimeJelly.item"; }
	case ItemType::BeeWing: { return "Resources/gamedata/items/BeeWing/BeeWing.item"; }
	case ItemType::Bone: { return "Resources/gamedata/items/Bone/Bone.item"; }
	// Ore and mining
	case ItemType::CopperVein: { return "Resources/gamedata/items/CopperVein/CopperVein0.item"; }
	case ItemType::CopperOre: { return "Resources/gamedata/items/CopperOre/CopperOre.item"; }
	case ItemType::CopperBar: { return "Resources/gamedata/items/CopperBar/CopperBar.item"; }
	case ItemType::IronVein: { return "Resources/gamedata/items/IronVein/IronVein0.item"; }
	case ItemType::IronOre: { return "Resources/gamedata/items/IronOre/IronOre.item"; }
	case ItemType::IronBar: { return "Resources/gamedata/items/IronBar/IronBar.item"; }
	case ItemType::SilverVein: { return "Resources/gamedata/items/SilverVein/SilverVein0.item"; }
	case ItemType::SilverOre: { return "Resources/gamedata/items/SilverOre/SilverOre.item"; }
	case ItemType::SilverBar: { return "Resources/gamedata/items/SilverBar/SilverBar.item"; }
	case ItemType::GoldVein: { return "Resources/gamedata/items/GoldVein/GoldVein0.item"; }
	case ItemType::GoldOre: { return "Resources/gamedata/items/GoldOre/GoldOre.item"; }
	case ItemType::GoldBar: { return "Resources/gamedata/items/GoldBar/GoldBar.item"; }
	// World blocks
	case ItemType::BlockGrass: { return "Resources/gamedata/items/Block_Grass/Block_Grass.item"; }
	case ItemType::BlockDirt: { return "Resources/gamedata/items/Block_Dirt/Block_Dirt.item"; }
	case ItemType::BlockStone: { return "Resources/gamedata/items/Block_Stone/Block_Stone.item"; }
	case ItemType::BlockWood: { return "Resources/gamedata/items/Block_Wood/Block_Wood.item"; }
	case ItemType::BlockLeaf: { return "Resources/gamedata/items/Block_Leaf/Block_Leaf.item"; }
	case ItemType::BlockSand: { return "Resources/gamedata/items/Block_Sand/Block_Sand.item"; }
	case ItemType::BlockCactus: { return "Resources/gamedata/items/Block_Cactus/Block_Cactus.item"; }
	case ItemType::BlockRock: { return "Resources/gamedata/items/Block_Rock/Block_Rock.item"; }
	case ItemType::BlockSnow: { return "Resources/gamedata/items/Block_Snow/Block_Snow.item"; }
	case ItemType::NUM_ITEMS: throw std::logic_error("Invalid ItemType!");
	default: throw std::logic_error("Invalid ItemType!");
	}
}

std::string GetItemTextureForType(ItemType type)
{
	switch (type)
	{
	case ItemType::None: { return ""; }
	// Dropped item in the world
	case ItemType::DroppedItem: { return ""; }
	// Hearts and coins dropped from killing enemies
	case ItemType::Coin: { return "Resources/textures/items/coin.tga"; }
	case ItemType::Heart: { return "Resources/textures/items/heart.tga"; }
	// Zombie gib
	case ItemType::Gib: { return "Resources/textures/items/question_mark.tga"; }
	// Interact items
	case ItemType::Tombstone: { return "Resources/textures/items/tombstone.tga"; }
	case ItemType::Chest: { return "Resources/textures/items/chest.tga"; }
	case ItemType::Torch: { return "Resources/textures/items/torch.tga"; }
	case ItemType::Furnace: { return "Resources/textures/items/furnace.tga"; }
	case ItemType::Anvil: { return "Resources/textures/items/anvil.tga"; }
	case ItemType::QuestBoard: { return "Resources/textures/items/quest_board.tga"; }
	case ItemType::CampFire: { return "Resources/textures/items/camp_fire.tga"; }
	case ItemType::Mannequin: { return "Resources/textures/items/mannequin.tga"; }
	// Ingredients
	case ItemType::SlimeJelly: { return "Resources/textures/items/slime_jelly.tga"; }
	case ItemType::BeeWing: { return "Resources/textures/items/bee_wing.tga"; }
	case ItemType::Bone: { return "Resources/textures/items/bone.tga"; }
	// Ore and mining
	case ItemType::CopperVein: { return "Resources/textures/items/question_mark.tga"; }
	case ItemType::CopperOre: { return "Resources/textures/items/copper_ore.tga"; }
	case ItemType::CopperBar: { return "Resources/textures/items/copper_bar.tga"; }
	case ItemType::IronVein: { return "Resources/textures/items/question_mark.tga"; }
	case ItemType::IronOre: { return "Resources/textures/items/iron_ore.tga"; }
	case ItemType::IronBar: { return "Resources/textures/items/iron_bar.tga"; }
	case ItemType::SilverVein: { return "Resources/textures/items/question_mark.tga"; }
	case ItemType::SilverOre: { return "Resources/textures/items/silver_ore.tga"; }
	case ItemType::SilverBar: { return "Resources/textures/items/silver_bar.tga"; }
	case ItemType::GoldVein: { return "Resources/textures/items/question_mark.tga"; }
	case ItemType::GoldOre: { return "Resources/textures/items/gold_ore.tga"; }
	case ItemType::GoldBar: { return "Resources/textures/items/gold_bar.tga"; }
	// World blocks
	case ItemType::BlockGrass: { return "Resources/textures/items/block_grass.tga"; }
	case ItemType::BlockDirt: { return "Resources/textures/items/block_dirt.tga"; }
	case ItemType::BlockStone: { return "Resources/textures/items/block_stone.tga"; }
	case ItemType::BlockWood: { return "Resources/textures/items/block_wood.tga"; }
	case ItemType::BlockLeaf: { return "Resources/textures/items/block_leaf.tga"; }
	case ItemType::BlockSand: { return "Resources/textures/items/block_sand.tga"; }
	case ItemType::BlockCactus: { return "Resources/textures/items/block_cactus.tga"; }
	case ItemType::BlockRock: { return "Resources/textures/items/block_rock.tga"; }
	case ItemType::BlockSnow: { return "Resources/textures/items/block_snow.tga"; }
	case ItemType::NUM_ITEMS: throw std::logic_error("Invalid ItemType!");
	default: throw std::logic_error("Invalid ItemType!");
	}
}

float GetItemRadiusForType(ItemType type)
{
	switch (type)
	{
	case ItemType::None: { return 1.00f; }
	// Dropped item in the world
	case ItemType::DroppedItem: { return 0.50f; }
	// Hearts and coins dropped from killing enemies
	case ItemType::Coin: { return 0.25f; }
	case ItemType::Heart: { return 0.25f; }
	// Zombie gib
	case ItemType::Gib: { return 0.125f; }
	// Interact items
	case ItemType::Tombstone: { return 0.50f; }
	case ItemType::Chest: { return 0.50f; }
	case ItemType::Torch: { return 1.00f; }
	case ItemType::Furnace: { return 0.50f; }
	case ItemType::Anvil: { return 0.50f; }
	case ItemType::QuestBoard: { return 1.00f; }
	case ItemType::CampFire: { return 0.50f; }
	case ItemType::Mannequin: { return 1.00f; }
	// Ingredients
	case ItemType::SlimeJelly: { return 0.25f; }
	case ItemType::BeeWing: { return 0.25f; }
	case ItemType::Bone: { return 0.25f; }
	// Ore and mining
	case ItemType::CopperVein: { return 0.50f; }
	case ItemType::CopperOre: { return 0.25f; }
	case ItemType::CopperBar: { return 0.25f; }
	case ItemType::IronVein: { return 0.50f; }
	case ItemType::IronOre: { return 0.25f; }
	case ItemType::IronBar: { return 0.25f; }
	case ItemType::SilverVein: { return 0.50f; }
	case ItemType::SilverOre: { return 0.25f; }
	case ItemType::SilverBar: { return 0.25f; }
	case ItemType::GoldVein: { return 0.5f; }
	case ItemType::GoldOre: { return 0.25f; }
	case ItemType::GoldBar: { return 0.25f; }
	// World blocks
	case ItemType::BlockGrass: { return 0.25f; }
	case ItemType::BlockDirt: { return 0.25f; }
	case ItemType::BlockStone: { return 0.25f; }
	case ItemType::BlockWood: { return 0.25f; }
	case ItemType::BlockLeaf: { return 0.25f; }
	case ItemType::BlockSand: { return 0.25f; }
	case ItemType::BlockCactus: { return 0.25f; }
	case ItemType::BlockRock: { return 0.25f; }
	case ItemType::BlockSnow: { return 0.25f; }
	case ItemType::NUM_ITEMS: throw std::logic_error("Invalid ItemType!");
	default: throw std::logic_error("Invalid ItemType!");
	}
}