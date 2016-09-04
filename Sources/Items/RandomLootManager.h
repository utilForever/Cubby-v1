/*************************************************************************
> File Name: RandomLootManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Random root manager class.
> Created Time: 2016/08/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_RANDOM_LOOT_MANAGER_H
#define CUBBY_RANDOM_LOOT_MANAGER_H

#include <Enemy/Enemy.h>

#include "Item.h"

struct EnemyIngredientsSpawnData
{
	EnemyType m_sourceEnemy;
	ItemType m_spawnedItem;
	int m_minSpawn;
	int m_maxSpawn;
};

struct RandomLootItem
{
	InventoryItem* m_pLootItem;
	EquipmentType m_equipmentType;
	int m_repickValue;
};

using EnemyIngredientsSpawnDataList = std::vector<EnemyIngredientsSpawnData*>;
using RandomLootItemList = std::vector<RandomLootItem*>;

class RandomLootManager
{
public:
	// Constructor, Destructor
	RandomLootManager();
	~RandomLootManager();

	// Deletion
	void ClearEnemyIngredientsSpawnData();
	void ClearRandomLootList();

	// Enemy dropping ingredients
	void AddEnemyIngredientSpawnData(EnemyType sourceEnemy, ItemType spawnedItem, int minSpawn, int maxSpawn);
	void GetSpawnedIngredientItemForEnemy(EnemyType sourceEnemy, ItemType* item, int* quantity);

	// Random loot
	void AddRandomLoot(EquipmentType equipment);
	InventoryItem* GetRandomLootItem(EquipmentType* equipment);

private:
	// Enemy ingredients
	EnemyIngredientsSpawnDataList m_vpEnemyIngredientsSpawnDataList;

	// Random loot
	RandomLootItemList m_vpRandomLootItemList;
};

#endif