/*************************************************************************
> File Name: Quest.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby game class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_QUEST_H
#define CUBBY_QUEST_H

#include <Enemy/Enemy.h>
#include <Items/Item.h>

enum class QuestType
{
	CollectX = 0,
	KillX,
	TalkToPerson,
	FindPerson, // TODO: Implement - FindPerson
	ExplorePlace, // TODO: Implement - ExplorePlace
	PlaceXScenery,

	NUM,
};

// Forward declaration
class NPC;
class Quest;
class NPCManager;
struct InventoryItem;
class InventoryManager;
class QuestJournal;

class QuestObjective
{
public:
	void Reset();

	void UpdateObjective();

	std::string m_objectiveText;

	QuestType m_questType;

	int m_numberOfX;
	int m_progressX;

	bool m_completed;

	// The enemy to kill
	EnemyType m_enemy;

	// The item to collect
	ItemType m_item;

	// Person to talk to
	NPC* m_talkNPC;

	// The dialog that this talking NPC should say
	std::string m_talkingNPCDialog;

	// The scenery placement item
	InventoryItem* m_placementItem;

	Quest* m_pParent;
};

typedef std::vector<QuestObjective*> QuestObjectiveList;


class Quest
{
public:
	// Constructor, Destructor
	Quest(std::string name, std::string startText, std::string completedText, std::string denyText);
	~Quest();

	void SetNPCManager(NPCManager* pNPCManager);
	void SetInventoryManager(InventoryManager* pInventoryManager);
	void SetQuestJournal(QuestJournal* pQuestJournal);

	void SetNPCGiver(NPC* npcGiver);

	void SetQuestReward(InventoryItem* pReward);

	void ClearObjectives();

	void AddQuestObjective(std::string objectiveText, QuestType type, int numX, EnemyType enemie, ItemType item, NPC* talkNPC, std::string talkingNPCDialog, InventoryItem* placementItem);

	void ExportQuest();
	void ImportQuest(const char* fileName);

	void CompleteQuest();

	bool CanAcceptQuest(std::string *acceptError);
	void AcceptQuest();

	void Reset();

	void Update();

	bool GetCompleted() const;

	std::string GetName() const;

	std::string GetStartText() const;

	std::string GetCompletedText() const;

	std::string GetDenyText() const;

	int GetNumObjectives() const;
	QuestObjective* GetObjective(int index);

	NPC* GetNPC() const;

	InventoryItem* GetReward() const;

private:
	bool m_erase;

	bool m_completed;

	std::string m_questName;

	std::string m_questStartText;
	std::string m_questCompletedText;
	std::string m_questDenyText;

	QuestObjectiveList m_vpObjectives;

	InventoryItem* m_pQuestReward;

	NPC* m_pNPC;

	NPCManager* m_pNPCManager;
	InventoryManager* m_pInventoryManager;
	QuestJournal* m_pQuestJournal;
};

#endif