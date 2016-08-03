/*************************************************************************
> File Name: RandomLootManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Random root manager class.
> Created Time: 2016/08/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "RandomLootManager.h"
#include <Utils/Random.h>
#include <CubbyGame.h>

// Constructor, Destructor
RandomLootManager::RandomLootManager()
{
	// Create the enemy ingredient spawn data lists
	AddEnemyIngredientSpawnData(EnemyType::GreenSlime, ItemType::SlimeJelly, 1, 3);
	AddEnemyIngredientSpawnData(EnemyType::RedSlime, ItemType::SlimeJelly, 1, 3);
	AddEnemyIngredientSpawnData(EnemyType::BlueSlime, ItemType::SlimeJelly, 1, 3);
	AddEnemyIngredientSpawnData(EnemyType::YellowSlime, ItemType::SlimeJelly, 1, 3);
	AddEnemyIngredientSpawnData(EnemyType::Bee, ItemType::BeeWing, 1, 1);
	AddEnemyIngredientSpawnData(EnemyType::NormalSkeleton, ItemType::Bone, 1, 2);
	AddEnemyIngredientSpawnData(EnemyType::RangedSkeleton, ItemType::Bone, 1, 2);
	AddEnemyIngredientSpawnData(EnemyType::MeleeSkeleton, ItemType::Bone, 1, 2);
	AddEnemyIngredientSpawnData(EnemyType::MageSkeleton, ItemType::Bone, 1, 2);

	// Create the random loot
	bool createRandomLootWeapons = true;
	if (createRandomLootWeapons)
	{
		AddRandomLoot(EquipmentType::MageStaff);
		AddRandomLoot(EquipmentType::NecroStaff);
		AddRandomLoot(EquipmentType::PriestStaff);
		AddRandomLoot(EquipmentType::DruidStaff);
		AddRandomLoot(EquipmentType::Boomerang);
		AddRandomLoot(EquipmentType::DragonBow);
		AddRandomLoot(EquipmentType::BoneSword);
		AddRandomLoot(EquipmentType::AshSword);
		AddRandomLoot(EquipmentType::FireballHandLeft);
		AddRandomLoot(EquipmentType::FireballHandRight);
	}

	bool createRandomLootArmor = true;
	if (createRandomLootArmor)
	{
		AddRandomLoot(EquipmentType::BlacksmithGloves);
		AddRandomLoot(EquipmentType::NormalBoots);
		AddRandomLoot(EquipmentType::NormalGloves);
		AddRandomLoot(EquipmentType::NormalShoulders);
		AddRandomLoot(EquipmentType::RocketBoots);
		AddRandomLoot(EquipmentType::RegrowthBoots);
		AddRandomLoot(EquipmentType::WoodenHelm);
		AddRandomLoot(EquipmentType::WoodenArmor);
		AddRandomLoot(EquipmentType::WoodenPants);
		AddRandomLoot(EquipmentType::WoodenGloves);
		AddRandomLoot(EquipmentType::WoodenBoots);
		AddRandomLoot(EquipmentType::WoodenShoulders);
		AddRandomLoot(EquipmentType::IronHelm);
		AddRandomLoot(EquipmentType::IronArmor);
		AddRandomLoot(EquipmentType::IronPants);
		AddRandomLoot(EquipmentType::IronGloves);
		AddRandomLoot(EquipmentType::IronBoots);
		AddRandomLoot(EquipmentType::IronShoulders);
		AddRandomLoot(EquipmentType::SteelHelm);
		AddRandomLoot(EquipmentType::SteelArmor);
		AddRandomLoot(EquipmentType::SteelPants);
		AddRandomLoot(EquipmentType::SteelGloves);
		AddRandomLoot(EquipmentType::SteelBoots);
		AddRandomLoot(EquipmentType::SteelShoulders);
		AddRandomLoot(EquipmentType::IceHelm);
		AddRandomLoot(EquipmentType::IceArmor);
		AddRandomLoot(EquipmentType::IcePants);
		AddRandomLoot(EquipmentType::IceGloves);
		AddRandomLoot(EquipmentType::IceBoots);
		AddRandomLoot(EquipmentType::IceShoulders);
		AddRandomLoot(EquipmentType::AshHelm);
		AddRandomLoot(EquipmentType::AshArmor);
		AddRandomLoot(EquipmentType::AshPants);
		AddRandomLoot(EquipmentType::AshGloves);
		AddRandomLoot(EquipmentType::AshBoots);
		AddRandomLoot(EquipmentType::AshShoulders);
		AddRandomLoot(EquipmentType::BoneHelm);
		AddRandomLoot(EquipmentType::BoneArmor);
		AddRandomLoot(EquipmentType::BonePants);
		AddRandomLoot(EquipmentType::BoneGloves);
		AddRandomLoot(EquipmentType::BoneBoots);
		AddRandomLoot(EquipmentType::BoneShoulders);
		AddRandomLoot(EquipmentType::SpikeHelm);
		AddRandomLoot(EquipmentType::SpikeArmor);
		AddRandomLoot(EquipmentType::SpikePants);
		AddRandomLoot(EquipmentType::SpikeGloves);
		AddRandomLoot(EquipmentType::SpikeBoots);
		AddRandomLoot(EquipmentType::SpikeShoulders);
	}
}

RandomLootManager::~RandomLootManager()
{
	ClearEnemyIngredientsSpawnData();
	ClearRandomLootList();
}

// Deletion
void RandomLootManager::ClearEnemyIngredientsSpawnData()
{
	for (size_t i = 0; i < m_vpEnemyIngredientsSpawnDataList.size(); ++i)
	{
		delete m_vpEnemyIngredientsSpawnDataList[i];
		m_vpEnemyIngredientsSpawnDataList[i] = nullptr;
	}

	m_vpEnemyIngredientsSpawnDataList.clear();
}

void RandomLootManager::ClearRandomLootList()
{
	for (size_t i = 0; i < m_vpRandomLootItemList.size(); ++i)
	{
		delete m_vpRandomLootItemList[i];
		m_vpRandomLootItemList[i] = nullptr;
	}

	m_vpRandomLootItemList.clear();
}

// Enemy dropping ingredients
void RandomLootManager::AddEnemyIngredientSpawnData(EnemyType sourceEnemy, ItemType spawnedItem, int minSpawn, int maxSpawn)
{
	EnemyIngredientsSpawnData* pEnemyIngredientsSpawnData = new EnemyIngredientsSpawnData();

	pEnemyIngredientsSpawnData->m_sourceEnemy = sourceEnemy;
	pEnemyIngredientsSpawnData->m_spawnedItem = spawnedItem;
	pEnemyIngredientsSpawnData->m_minSpawn = minSpawn;
	pEnemyIngredientsSpawnData->m_maxSpawn = maxSpawn;

	m_vpEnemyIngredientsSpawnDataList.push_back(pEnemyIngredientsSpawnData);
}

void RandomLootManager::GetSpawnedIngredientItemForEnemy(EnemyType sourceEnemy, ItemType* item, int* quantity)
{
	for (size_t i = 0; i < m_vpEnemyIngredientsSpawnDataList.size(); ++i)
	{
		if (m_vpEnemyIngredientsSpawnDataList[i]->m_sourceEnemy == sourceEnemy)
		{
			*item = m_vpEnemyIngredientsSpawnDataList[i]->m_spawnedItem;

			if (m_vpEnemyIngredientsSpawnDataList[i]->m_minSpawn == m_vpEnemyIngredientsSpawnDataList[i]->m_maxSpawn)
			{
				*quantity = m_vpEnemyIngredientsSpawnDataList[i]->m_minSpawn;
			}
			else
			{
				*quantity = GetRandomNumber(m_vpEnemyIngredientsSpawnDataList[i]->m_minSpawn, m_vpEnemyIngredientsSpawnDataList[i]->m_maxSpawn);
			}

			return;
		}
	}

	*item = ItemType::None;
	*quantity = 0;
}

// Random loot
void RandomLootManager::AddRandomLoot(EquipmentType equipment)
{
	RandomLootItem* pNewRandomLoot = new RandomLootItem();

	pNewRandomLoot->m_repickValue = 500;
	pNewRandomLoot->m_equipmentType = equipment;
	pNewRandomLoot->m_pLootItem = CubbyGame::GetInstance()->GetInventoryManager()->CreateEquipmentItemFromType(equipment);

	m_vpRandomLootItemList.push_back(pNewRandomLoot);
}

InventoryItem* RandomLootManager::GetRandomLootItem(EquipmentType* equipment)
{
	if (m_vpRandomLootItemList.size() > 0)
	{
		int numRetries = 0;

		while (numRetries < 10)
		{
			int randomSelection = GetRandomNumber(0, static_cast<int>(m_vpRandomLootItemList.size()) - 1);
			int repickValue = GetRandomNumber(0, 1000);

			if (numRetries < 9)
			{
				if (m_vpRandomLootItemList[randomSelection]->m_repickValue > repickValue)
				{
					m_vpRandomLootItemList[randomSelection]->m_repickValue = static_cast<int>(m_vpRandomLootItemList[randomSelection]->m_repickValue * 0.5f);

					*equipment = m_vpRandomLootItemList[randomSelection]->m_equipmentType;
					return m_vpRandomLootItemList[randomSelection]->m_pLootItem;
				}
				
				m_vpRandomLootItemList[randomSelection]->m_repickValue += static_cast<int>((1000 - m_vpRandomLootItemList[randomSelection]->m_repickValue) * 0.5f);

				numRetries++;
			}
			else
			{
				*equipment = m_vpRandomLootItemList[randomSelection]->m_equipmentType;
				return m_vpRandomLootItemList[randomSelection]->m_pLootItem;
			}
		}
	}

	*equipment = EquipmentType::None;

	return nullptr;
}