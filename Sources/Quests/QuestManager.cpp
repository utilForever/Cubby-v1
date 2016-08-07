/*************************************************************************
> File Name: QuestManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Quest manager class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "QuestManager.h"

// Constructor, Destructor
QuestManager::QuestManager() :
	m_pNPCManager(nullptr), m_pInventoryManager(nullptr), m_pQuestJournal(nullptr)
{

}

QuestManager::~QuestManager()
{
	ClearQuests();
}

void QuestManager::SetNPCManager(NPCManager* pNPCManager)
{
	m_pNPCManager = pNPCManager;
}

void QuestManager::SetInventoryManager(InventoryManager* pInventoryManager)
{
	m_pInventoryManager = pInventoryManager;
}

void QuestManager::SetQuestJournal(QuestJournal* pQuestJournal)
{
	m_pQuestJournal = pQuestJournal;
}

void QuestManager::ClearRandomEnemyList()
{
	m_vRandomEnemyList.clear();
}

void QuestManager::AddRandomEnemyTypeToList(EnemyType enemy)
{
	m_vRandomEnemyList.push_back(enemy);
}

void QuestManager::ClearQuests()
{
	for (size_t i = 0; i < m_vpQuestList.size(); ++i)
	{
		delete m_vpQuestList[i];
		m_vpQuestList[i] = nullptr;
	}

	m_vpQuestList.clear();
}

Quest* QuestManager::CreateQuest(std::string name, std::string startText, std::string completedText, std::string denyText)
{
	Quest* pQuest = new Quest(name, startText, completedText, denyText);
	pQuest->SetNPCManager(m_pNPCManager);
	pQuest->SetInventoryManager(m_pInventoryManager);
	pQuest->SetQuestJournal(m_pQuestJournal);

	m_vpQuestList.push_back(pQuest);

	return pQuest;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
Quest* QuestManager::CreateRandomQuest() const
{
	return nullptr;
}

Quest* QuestManager::GetQuest(std::string name)
{
	for (size_t i = 0; i < m_vpQuestList.size(); ++i)
	{
		if (m_vpQuestList[i]->GetName() == name)
		{
			return m_vpQuestList[i];
		}
	}

	return nullptr;
}