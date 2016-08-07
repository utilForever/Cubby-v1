/*************************************************************************
> File Name: QuestJournal.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Quest journal class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_QUEST_JOURNAL_H
#define CUBBY_QUEST_JOURNAL_H

#include "Quest.h"

// Forward declaration
class QuestManager;
class Player;

enum class QuestEntryStatus
{
	Uncompleted = 0,
	Completed,
};

struct QuestJournalEntry
{
	Quest* m_pQuest;
	QuestEntryStatus m_status;
};

using QuestJournalEntryList = std::vector<QuestJournalEntry*>;

class QuestJournal
{
public:
	// Constructor, Destructor
	QuestJournal(QuestManager* pQuestManager);
	~QuestJournal();

	void ClearJournal();

	void SetPlayer(Player* pPlayer);

	void AddQuestJournalEntry(Quest* pQuest);

	void UpdateQuestJournalEntry(Quest* pQuest);

	void CompleteAllCurrentQuests();

	int GetNumCurrentQuests();
	int GetNumCompletedQuests();
	Quest* GetCurrentQuest(int index);
	Quest* GetCompletedQuest(int index);

	void ExportQuestJournal(int playerNum);
	void ImportQuestJournal(int playerNum);

private:
	QuestManager* m_pQuestManager;
	Player* m_pPlayer;

	QuestJournalEntryList m_vpQuestJournalList;
};

#endif