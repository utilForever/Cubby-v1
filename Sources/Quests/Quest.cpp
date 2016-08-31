/*************************************************************************
> File Name: Quest.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby game class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>
#include <algorithm>

#include <CubbyGame.h>

#include <NPC/NPC.h>
#include <NPC/NPCManager.h>

#include "Quest.h"

void QuestObjective::Reset()
{
	m_completed = false;

	m_progressX = 0;
}

void QuestObjective::UpdateObjective()
{
	if (m_progressX == m_numberOfX)
	{
		m_completed = true;
	}

	m_pParent->Update();
}

// Constructor, Destructor
Quest::Quest(std::string name, std::string startText, std::string completedText, std::string denyText) :
	m_erase(false), m_completed(false), m_questName(name),
	m_questStartText(startText), m_questCompletedText(completedText), m_questDenyText(denyText),
	m_pQuestReward(nullptr), m_pNPC(nullptr), m_pNPCManager(nullptr), m_pInventoryManager(nullptr), m_pQuestJournal(nullptr)
{
	
}

Quest::~Quest()
{
	ClearObjectives();

	if (m_pQuestReward != nullptr)
	{
		delete m_pQuestReward;
		m_pQuestReward = nullptr;
	}
}

void Quest::SetNPCManager(NPCManager* pNPCManager)
{
	m_pNPCManager = pNPCManager;
}

void Quest::SetInventoryManager(InventoryManager* pInventoryManager)
{
	m_pInventoryManager = pInventoryManager;
}

void Quest::SetQuestJournal(QuestJournal* pQuestJournal)
{
	m_pQuestJournal = pQuestJournal;
}

void Quest::SetNPCGiver(NPC* npcGiver)
{
	m_pNPC = npcGiver;
}

void Quest::SetQuestReward(InventoryItem* pReward)
{
	m_pQuestReward = new InventoryItem();
	m_pQuestReward->m_fileName = pReward->m_fileName;
	m_pQuestReward->m_iconFileName = pReward->m_iconFileName;
	m_pQuestReward->m_title = pReward->m_title;
	m_pQuestReward->m_description = pReward->m_description;
	m_pQuestReward->m_itemType = pReward->m_itemType;
	m_pQuestReward->m_item = pReward->m_item;
	m_pQuestReward->m_status = pReward->m_status;
	m_pQuestReward->m_equipSlot = pReward->m_equipSlot;
	m_pQuestReward->m_itemQuality = pReward->m_itemQuality;
	m_pQuestReward->m_left = pReward->m_left;
	m_pQuestReward->m_right = pReward->m_right;
	m_pQuestReward->m_placementR = pReward->m_placementR;
	m_pQuestReward->m_placementG = pReward->m_placementG;
	m_pQuestReward->m_placementB = pReward->m_placementB;
	m_pQuestReward->m_lootSlotX = pReward->m_lootSlotX;
	m_pQuestReward->m_lootSlotY = pReward->m_lootSlotY;
	m_pQuestReward->m_equipped = pReward->m_equipped;
	m_pQuestReward->m_scale = pReward->m_scale;
	m_pQuestReward->m_offsetX = pReward->m_offsetX;
	m_pQuestReward->m_offsetY = pReward->m_offsetY;
	m_pQuestReward->m_offsetZ = pReward->m_offsetZ;
	m_pQuestReward->m_quantity = pReward->m_quantity;
}

void Quest::ClearObjectives()
{
	for (size_t i = 0; i < m_vpObjectives.size(); ++i)
	{
		delete m_vpObjectives[i];
		m_vpObjectives[i] = nullptr;
	}

	m_vpObjectives.clear();
}

void Quest::AddQuestObjective(std::string objectiveText, QuestType type, int numX, EnemyType enemy, ItemType item, NPC* talkNPC, std::string talkingNPCDialog, InventoryItem* placementItem)
{
	QuestObjective* pObjective = new QuestObjective();

	pObjective->m_objectiveText = objectiveText;
	pObjective->m_questType = type;
	pObjective->m_numberOfX = numX;
	pObjective->m_progressX = 0;
	pObjective->m_enemy = enemy;
	pObjective->m_item = item;
	pObjective->m_talkNPC = talkNPC;
	pObjective->m_talkingNPCDialog = talkingNPCDialog;
	pObjective->m_placementItem = placementItem;

	pObjective->m_completed = false;

	pObjective->m_pParent = this;

	m_vpObjectives.push_back(pObjective);
}

void Quest::ExportQuest()
{
	std::ofstream exportFile;

	char fileName[128];

	// Replace all spaces in the quest title, with underscores for the quest filename
	std::string questName(m_questName);
	replace(questName.begin(), questName.end(), ' ', '_');
	sprintf(fileName, "Resources/gamedata/quests/%s.quest", questName.c_str());

	exportFile.open(fileName, std::ios_base::binary);

	if (exportFile.is_open())
	{
		// Quest name
		exportFile << m_questName << "|";

		// Quest started text
		exportFile << m_questStartText << "|";

		// Quest completed text
		exportFile << m_questCompletedText << "|";

		// Quest deny text
		exportFile << m_questDenyText << "|";

		// Num objectives
		exportFile << m_vpObjectives.size() << "|";

		// Objectives
		for (unsigned int i = 0; i < m_vpObjectives.size(); i++)
		{
			exportFile << m_vpObjectives[i]->m_objectiveText << "|";
			exportFile << static_cast<int>(m_vpObjectives[i]->m_questType) << "|";
			exportFile << m_vpObjectives[i]->m_numberOfX << "|";
			exportFile << static_cast<int>(m_vpObjectives[i]->m_enemy) << "|";
			exportFile << static_cast<int>(m_vpObjectives[i]->m_item) << "|";
			
			if (m_vpObjectives[i]->m_talkNPC)
			{
				exportFile << m_vpObjectives[i]->m_talkNPC->GetName() << "|";
				exportFile << m_vpObjectives[i]->m_talkingNPCDialog << "|";
			}
			else
			{
				exportFile << 0 << "|0|";
			}

			if (m_vpObjectives[i]->m_placementItem)
			{
				exportFile << m_vpObjectives[i]->m_placementItem->m_fileName << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_iconFileName << "|";
				exportFile << static_cast<int>(m_vpObjectives[i]->m_placementItem->m_itemType) << "|";
				exportFile << static_cast<int>(m_vpObjectives[i]->m_placementItem->m_item) << "|";
				exportFile << static_cast<int>(m_vpObjectives[i]->m_placementItem->m_equipSlot) << "|";
				exportFile << static_cast<int>(m_vpObjectives[i]->m_placementItem->m_itemQuality) << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_left << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_right << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_title << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_description << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_placementR << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_placementG << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_placementB << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_scale << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_offsetX << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_offsetY << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_offsetZ << "|";
				exportFile << m_vpObjectives[i]->m_placementItem->m_quantity << "|";
			}
			else
			{
				exportFile << "NULL" << "|";
				exportFile << "NULL" << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << "NULL" << "|";
				exportFile << "NULL" << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
				exportFile << 0 << "|";
			}

		}

		exportFile.close();
	}
}

void Quest::ImportQuest(const char* fileName)
{
	std::ifstream importFile;

	char importFileName[128];
	sprintf(importFileName, "Resources/gamedata/quests/%s.quest", fileName);

	importFile.open(importFileName, std::ios_base::binary);

	if (importFile.is_open())
	{
		char* input = new char[128];
		char* pEnd;

		// Name
		char questName[128];
		importFile.getline(questName, 128, '|');
		m_questName = questName;

		// Quest started text
		char questStartText[1024];
		importFile.getline(questStartText, 1024, '|');
		m_questStartText = questStartText;

		// Quest completed text
		char questCompletedText[1024];
		importFile.getline(questCompletedText, 1024, '|');
		m_questCompletedText = questCompletedText;

		// Quest deny text
		char questDenyText[1024];
		importFile.getline(questDenyText, 1024, '|');
		m_questDenyText = questDenyText;

		// Num Objectives
		int numObjectives;
		importFile.getline(input, 128, '|');
		numObjectives = strtol(input, &pEnd, 10);

		// OBjectives
		for (int i = 0; i < numObjectives; ++i)
		{
			QuestObjective* pNewObjective = new QuestObjective();

			// Objective text
			char objText[128];
			importFile.getline(objText, 128, '|');
			pNewObjective->m_objectiveText = objText;

			// Type
			int questType;
			importFile.getline(input, 128, '|');
			questType = strtol(input, &pEnd, 10);
			pNewObjective->m_questType = static_cast<QuestType>(questType);

			// Num X
			importFile.getline(input, 128, '|');
			pNewObjective->m_numberOfX = strtol(input, &pEnd, 10);

			// Enemie
			int enemie;
			importFile.getline(input, 128, '|');
			enemie = strtol(input, &pEnd, 10);
			pNewObjective->m_enemy = static_cast<EnemyType>(enemie);

			// Item
			int item;
			importFile.getline(input, 128, '|');
			item = strtol(input, &pEnd, 10);
			pNewObjective->m_item = static_cast<ItemType>(item);

			// Talk NPC
			char npcName[128];
			importFile.getline(npcName, 128, '|');
			NPC* talkNPC = m_pNPCManager->GetNPCByName(npcName);
			pNewObjective->m_talkNPC = talkNPC;

			// Talk NPC dialog
			char talkNPCDialog[256];
			importFile.getline(talkNPCDialog, 128, '|');
			pNewObjective->m_talkingNPCDialog = talkNPCDialog;

			// Scenery placement item
			InventoryItem* pNewItem = nullptr;
			char itemFileName[128];
			importFile.getline(itemFileName, 128, '|');

			char iconFileName[128];
			importFile.getline(iconFileName, 128, '|');

			importFile.getline(input, 128, '|');
			int itemType = strtol(input, &pEnd, 10);

			importFile.getline(input, 128, '|');
			int objectiveItem = strtol(input, &pEnd, 10);

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

			importFile.getline(input, 128, '|');
			float r = static_cast<float>(strtod(input, &pEnd));
			importFile.getline(input, 128, '|');
			float g = static_cast<float>(strtod(input, &pEnd));
			importFile.getline(input, 128, '|');
			float b = static_cast<float>(strtod(input, &pEnd));

			importFile.getline(input, 128, '|');
			float scale = static_cast<float>(strtod(input, &pEnd));
			importFile.getline(input, 128, '|');
			float offsetX = static_cast<float>(strtod(input, &pEnd));
			importFile.getline(input, 128, '|');
			float offsetY = static_cast<float>(strtod(input, &pEnd));
			importFile.getline(input, 128, '|');
			float offsetZ = static_cast<float>(strtod(input, &pEnd));

			importFile.getline(input, 128, '|');
			int quantity = strtol(input, &pEnd, 10);

			// Create the new inventory item		
			if (strcmp(itemFileName, "NULL") != 0)
			{
				pNewItem = new InventoryItem();

				pNewItem->m_fileName = itemFileName;

				pNewItem->m_iconFileName = iconFileName;

				pNewItem->m_title = title;
				pNewItem->m_description = description;

				pNewItem->m_itemType = static_cast<InventoryType>(itemType);
				pNewItem->m_item = static_cast<ItemType>(objectiveItem);
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
			}

			pNewObjective->m_placementItem = pNewItem;

			m_vpObjectives.push_back(pNewObjective);
		}

		importFile.close();

		delete[] input;
	}
}

void Quest::CompleteQuest()
{
	for (size_t i = 0; i < m_vpObjectives.size(); ++i)
	{
		m_vpObjectives[i]->m_progressX = m_vpObjectives[i]->m_numberOfX;
		m_vpObjectives[i]->m_completed = true;
	}

	Update();
}

bool Quest::CanAcceptQuest(std::string* acceptError)
{
	for (size_t i = 0; i < m_vpObjectives.size(); ++i)
	{
		if (m_vpObjectives[i]->m_questType == QuestType::PlaceXScenery)
		{
			if (m_vpObjectives[i]->m_placementItem != nullptr)
			{
				if (m_pInventoryManager->CanAddInventoryItem(m_vpObjectives[i]->m_placementItem->m_title.c_str(), m_vpObjectives[i]->m_placementItem->m_item, m_vpObjectives[i]->m_placementItem->m_quantity) == false)
				{
					*acceptError = "Inventory full";
					return false;
				}
			}
		}
	}

	*acceptError = "";
	return true;
}

void Quest::AcceptQuest()
{
	// Go through each quest objective and add a speech marker over any NPCs that need to be 'talked to'
	for (size_t i = 0; i < m_vpObjectives.size(); ++i)
	{
		if (m_vpObjectives[i]->m_questType == QuestType::TalkToPerson)
		{
			// TODO: Implement it
			// m_vpObjectives[i]->m_talkNPC->SetOverHeadMarkerStatus(OverHeadMarkerStatus_Speech);
			// m_vpObjectives[i]->m_talkNPC->SetTalkQuestDialog(m_vpObjectives[i]->m_talkingNPCDialog);
		}

		if (m_vpObjectives[i]->m_questType == QuestType::PlaceXScenery)
		{
			if (m_vpObjectives[i]->m_placementItem != nullptr)
			{
				m_pInventoryManager->AddInventoryItem(m_vpObjectives[i]->m_placementItem, -1, -1);
			}
		}
	}
}

void Quest::Reset()
{
	m_completed = false;

	for (size_t i = 0; i < m_vpObjectives.size(); ++i)
	{
		m_vpObjectives[i]->Reset();
	}
}

void Quest::Update()
{
	// If we have a collection quest, check objectives to see if we already have the items we are needing to collect
	for (size_t i = 0; i < m_vpObjectives.size(); ++i)
	{
		if (m_vpObjectives[i]->m_questType == QuestType::CollectX)
		{
			for (int itemIndex = 0; itemIndex < InventoryManager::MAX_NUM_INVENTORY_SLOTS; ++itemIndex)
			{
				InventoryItem* pItem = m_pInventoryManager->GetInventoryItemForSlot(itemIndex);

				if (pItem != nullptr)
				{
					if (m_vpObjectives[i]->m_item == pItem->m_item)
					{
						m_vpObjectives[i]->m_progressX = pItem->m_quantity;

						if (m_vpObjectives[i]->m_progressX > m_vpObjectives[i]->m_numberOfX)
						{
							m_vpObjectives[i]->m_progressX = m_vpObjectives[i]->m_numberOfX;
							m_vpObjectives[i]->m_completed = true;
						}
					}
				}
			}
		}
	}

	bool allCompleted = true;

	for (size_t i = 0; i < m_vpObjectives.size(); ++i)
	{
		if (m_vpObjectives[i]->m_completed == false)
		{
			allCompleted = false;
		}
	}

	if (allCompleted)
	{
		m_completed = true;
		
		// TODO: Implement it
		// m_pNPC->SetOverHeadMarkerStatus(OverHeadMarkerStatus_Question);
	}

	CubbyGame::GetInstance()->GetQuestGUI()->RefreshQuestButtons();
}

bool Quest::GetCompleted() const
{
	return m_completed;
}

std::string Quest::GetName() const
{
	return m_questName;
}

std::string Quest::GetStartText() const
{
	return m_questStartText;
}

std::string Quest::GetCompletedText() const
{
	return m_questCompletedText;
}

std::string Quest::GetDenyText() const
{
	return m_questDenyText;
}

int Quest::GetNumObjectives() const
{
	return static_cast<int>(m_vpObjectives.size());
}

QuestObjective* Quest::GetObjective(int index)
{
	return m_vpObjectives[index];
}

NPC* Quest::GetNPC() const
{
	return m_pNPC;
}

InventoryItem* Quest::GetReward() const
{
	return m_pQuestReward;
}
