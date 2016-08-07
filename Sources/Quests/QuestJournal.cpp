/*************************************************************************
> File Name: QuestJournal.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Quest journal class.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>

#include "QuestManager.h"
#include "QuestJournal.h"

// Constructor, Destructor
QuestJournal::QuestJournal(QuestManager* pQuestManager) :
	m_pQuestManager(pQuestManager), m_pPlayer(nullptr)
{

}

QuestJournal::~QuestJournal()
{
	ClearJournal();
}

void QuestJournal::ClearJournal()
{
	for (size_t i = 0; i < m_vpQuestJournalList.size(); ++i)
	{
		delete m_vpQuestJournalList[i];
		m_vpQuestJournalList[i] = nullptr;
	}

	m_vpQuestJournalList.clear();
}

void QuestJournal::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

void QuestJournal::AddQuestJournalEntry(Quest* pQuest)
{
	QuestJournalEntry* pNewEntry = new QuestJournalEntry();
	pNewEntry->m_pQuest = pQuest;
	pNewEntry->m_status = QuestEntryStatus::Uncompleted;

	m_vpQuestJournalList.push_back(pNewEntry);
}

void QuestJournal::UpdateQuestJournalEntry(Quest* pQuest)
{
	for (size_t i = 0; i < m_vpQuestJournalList.size(); ++i)
	{
		if (m_vpQuestJournalList[i]->m_pQuest == pQuest)
		{
			m_vpQuestJournalList[i]->m_status = QuestEntryStatus::Completed;
		}
	}
}

void QuestJournal::CompleteAllCurrentQuests()
{
	for (size_t i = 0; i < m_vpQuestJournalList.size(); ++i)
	{
		if (m_vpQuestJournalList[i]->m_status == QuestEntryStatus::Uncompleted)
		{
			m_vpQuestJournalList[i]->m_status = QuestEntryStatus::Completed;
			m_vpQuestJournalList[i]->m_pQuest->CompleteQuest();
		}
	}
}

int QuestJournal::GetNumCurrentQuests()
{
	int current = 0;

	for (size_t i = 0; i < m_vpQuestJournalList.size(); ++i)
	{
		if (m_vpQuestJournalList[i]->m_status == QuestEntryStatus::Uncompleted)
		{
			current++;
		}
	}

	return current;
}

int QuestJournal::GetNumCompletedQuests()
{
	int completed = 0;

	for (size_t i = 0; i < m_vpQuestJournalList.size(); ++i)
	{
		if (m_vpQuestJournalList[i]->m_status == QuestEntryStatus::Completed)
		{
			completed++;
		}
	}

	return completed;
}

Quest* QuestJournal::GetCurrentQuest(int index)
{
	int current = 0;

	for (size_t i = 0; i < m_vpQuestJournalList.size(); ++i)
	{
		if (m_vpQuestJournalList[i]->m_status == QuestEntryStatus::Uncompleted)
		{
			if (current == index)
			{
				return m_vpQuestJournalList[i]->m_pQuest;
			}

			current++;
		}
	}

	return nullptr;
}

Quest* QuestJournal::GetCompletedQuest(int index)
{
	int completed = 0;

	for (size_t i = 0; i < m_vpQuestJournalList.size(); ++i)
	{
		if (m_vpQuestJournalList[i]->m_status == QuestEntryStatus::Completed)
		{
			if (completed == index)
			{
				return m_vpQuestJournalList[i]->m_pQuest;
			}

			completed++;
		}
	}

	return nullptr;
}

void QuestJournal::ExportQuestJournal(int playerNum)
{
	std::ofstream exportFile;
	char exportFileName[128];
	sprintf(exportFileName, "Resources/characters/character%i/journal.quests", playerNum);
	exportFile.open(exportFileName);

	if (exportFile.is_open())
	{
		exportFile << static_cast<int>(m_vpQuestJournalList.size()) << "|";

		for (int i = 0; i < static_cast<int>(m_vpQuestJournalList.size()); ++i)
		{
			exportFile << static_cast<int>(m_vpQuestJournalList[i]->m_status) << "|" << m_vpQuestJournalList[i]->m_pQuest->GetName() << "|";

			for (int j = 0; j < m_vpQuestJournalList[i]->m_pQuest->GetNumObjectives(); ++j)
			{
				exportFile << m_vpQuestJournalList[i]->m_pQuest->GetObjective(j)->m_progressX << "|";
			}

			exportFile << "\n";
		}

		exportFile.close();
	}
}

void QuestJournal::ImportQuestJournal(int playerNum)
{
	ClearJournal();

	std::ifstream importFile;
	char importFileName[128];
	sprintf(importFileName, "Resources/characters/character%i/journal.quests", playerNum);
	importFile.open(importFileName);

	if (importFile.is_open())
	{
		char* input = new char[128];
		char* pEnd;

		int numQuests;
		importFile.getline(input, 128, '|');
		numQuests = strtol(input, &pEnd, 10);

		for (int i = 0; i < numQuests; ++i)
		{
			int questStatus;
			importFile.getline(input, 128, '|');
			questStatus = strtol(input, &pEnd, 10);

			char questName[128];
			importFile.getline(questName, 128, '|');

			QuestJournalEntry* pNewEntry = new QuestJournalEntry();
			pNewEntry->m_pQuest = m_pQuestManager->GetQuest(questName);
			if (pNewEntry->m_pQuest != nullptr)
			{
				pNewEntry->m_status = static_cast<QuestEntryStatus>(questStatus);

				for (int j = 0; j < pNewEntry->m_pQuest->GetNumObjectives(); ++j)
				{
					int numProgress;
					importFile.getline(input, 128, '|');
					numProgress = strtol(input, &pEnd, 10);

					if (static_cast<QuestEntryStatus>(questStatus) == QuestEntryStatus::Uncompleted)
					{
						pNewEntry->m_pQuest->GetObjective(j)->m_progressX = numProgress;
						pNewEntry->m_pQuest->GetObjective(j)->UpdateObjective();
					}
				}

				m_vpQuestJournalList.push_back(pNewEntry);

				if (static_cast<QuestEntryStatus>(questStatus) == QuestEntryStatus::Uncompleted)
				{
					// TODO: Implement it
					//m_pPlayer->SetQuest(pNewEntry->m_pQuest);
					//m_pGameWindow->GetHUD()->SetCurrentQuest(pNewEntry->m_pQuest);

					//pNewEntry->m_pQuest->GetNPC()->SetOverHeadMarkerStatus(OverHeadMarkerStatus_None);

					pNewEntry->m_pQuest->Update();
				}
			}
		}

		importFile.close();

		delete[] input;
	}
}