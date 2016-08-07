/*************************************************************************
> File Name: QuestManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Quest manager class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_QUEST_MANAGER_H
#define CUBBY_QUEST_MANAGER_H

#include "Quest.h"

// Forward declaration
class NPCManager;
class InventoryManager;
class QuestJournal;

using QuestList = std::vector<Quest*>;

class QuestManager
{
public:
	// Constructor, Destructor
	QuestManager();
	~QuestManager();

	void SetNPCManager(NPCManager* pNPCManager);
	void SetInventoryManager(InventoryManager* pInventoryManager);
	void SetQuestJournal(QuestJournal* pQuestJournal);

	void ClearRandomEnemyList();
	void AddRandomEnemyTypeToList(EnemyType enemy);

	void ClearQuests();

	Quest* CreateQuest(std::string name, std::string startText, std::string completedText, std::string denyText);
	Quest* CreateRandomQuest() const;

	Quest* GetQuest(std::string name);

private:
	QuestList m_vpQuestList;

	std::vector<EnemyType> m_vRandomEnemyList;

	NPCManager* m_pNPCManager;
	InventoryManager* m_pInventoryManager;
	QuestJournal* m_pQuestJournal;
};


#endif