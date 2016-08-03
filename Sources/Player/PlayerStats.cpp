/*************************************************************************
> File Name: PlayerStats.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Player stat class.
> Created Time: 2016/08/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>

#include <CubbyGame.h>

#include "PlayerStats.h"

// Constructor, Destructor
PlayerStats::PlayerStats(Player* pPlayer) :
	m_pPlayer(pPlayer), m_class(PlayerClass::Debug), m_supressExport(false)
{
	ResetStats();
}

PlayerStats::~PlayerStats()
{

}

// Reset
void PlayerStats::ResetStats()
{
	m_level = 1;

	m_experience = 0;
	m_maxExperience = 100;

	m_numPointsAvailable = 0;

	m_strengthStat = 1;
	m_dexterityStat = 1;
	m_intelligenceStat = 1;
	m_vitalityStat = 1;
	m_armorStat = 1;
	m_luckStat = 1;
}

// Supress export
void PlayerStats::SetSupressExport(bool supress)
{
	m_supressExport = supress;
}

// Loading / Saving
void PlayerStats::ImportStats(std::string playerName)
{
	m_name = playerName;

	std::ifstream importFile;
	char importFileName[128];
	sprintf(importFileName, "Saves/characters/%s/%s.stats", playerName.c_str(), playerName.c_str());
	importFile.open(importFileName, std::ios_base::binary);

	if (importFile.is_open())
	{
		std::string tempString;

		std::string name;
		importFile >> tempString >> name;

		int playerClass;
		importFile >> tempString >> playerClass;
		m_class = static_cast<PlayerClass>(playerClass);

		importFile >> tempString >> m_level;
		importFile >> tempString >> m_experience;
		importFile >> tempString >> m_maxExperience;
		importFile >> tempString >> m_numPointsAvailable;
		importFile >> tempString >> m_strengthStat;
		importFile >> tempString >> m_dexterityStat;
		importFile >> tempString >> m_intelligenceStat;
		importFile >> tempString >> m_vitalityStat;
		importFile >> tempString >> m_armorStat;
		importFile >> tempString >> m_luckStat;

		// Import the file signature
		std::string signature;
		importFile >> tempString >> signature;

		importFile.close();
	}
}

void PlayerStats::ExportStats(std::string playerName) const
{
	if (m_supressExport)
	{
		return;
	}

	if (playerName == "")
	{
		return;
	}

	std::ofstream exportFile;
	char exportFileName[128];
	sprintf(exportFileName, "Saves/characters/%s/%s.stats", playerName.c_str(), playerName.c_str());
	exportFile.open(exportFileName, std::ios_base::binary);

	if (exportFile.is_open())
	{
		exportFile << "name: " << playerName << "\n";
		exportFile << "class: " << static_cast<int>(m_class) << "\n";
		exportFile << "level: " << m_level << "\n";
		exportFile << "experience: " << m_experience << "\n";
		exportFile << "maxExperience: " << m_maxExperience << "\n";
		exportFile << "numStatsPoints: " << m_numPointsAvailable << "\n";
		exportFile << "strength: " << m_strengthStat << "\n";
		exportFile << "dexterity: " << m_dexterityStat << "\n";
		exportFile << "intelligence: " << m_intelligenceStat << "\n";
		exportFile << "vitality: " << m_vitalityStat << "\n";
		exportFile << "armor: " << m_armorStat << "\n";
		exportFile << "luck: " << m_luckStat << "\n\n";

		// Export the file signature
		exportFile << "signature: " << "\n";

		exportFile.close();
	}
}

// Accessors
PlayerClass PlayerStats::GetClass() const
{
	return m_class;
}

int PlayerStats::GetLevel() const
{
	return m_level;
}

int PlayerStats::GetExperience() const
{
	return m_experience;
}

int PlayerStats::GetMaxExperience() const
{
	return m_maxExperience;
}

int PlayerStats::GetNumPointsAvailable() const
{
	return m_numPointsAvailable;
}

int PlayerStats::GetStrengthStat() const
{
	return m_strengthStat;
}

int PlayerStats::GetDexterityStat() const
{
	return m_dexterityStat;
}

int PlayerStats::GetIntelligenceStat() const
{
	return m_intelligenceStat;
}

int PlayerStats::GetVitalityStat() const
{
	return m_vitalityStat;
}

int PlayerStats::GetArmorStat() const
{
	return m_armorStat;
}

int PlayerStats::GetLuckStat() const
{
	return m_luckStat;
}

// Setters
void PlayerStats::SetClass(PlayerClass playerClass)
{
	m_class = playerClass;
}

void PlayerStats::GiveExperience(int amount)
{
	m_experience += amount;

	if (m_experience >= m_maxExperience)
	{
		m_pPlayer->LevelUp();

		m_maxExperience = static_cast<int>(m_maxExperience * 1.5f);
		m_experience = 0;
		m_level++;

		m_numPointsAvailable += 5;

		CubbyGame::GetInstance()->GetCharacterGUI()->UpdatePlayerStats();
	}

	CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();

	ExportStats(m_name);
}

void PlayerStats::SetNumPointsAvailable(int numPoints)
{
	m_numPointsAvailable = numPoints;

	ExportStats(m_name);
}

void PlayerStats::SetStrengthStat(int stat)
{
	m_strengthStat = stat;

	ExportStats(m_name);
}

void PlayerStats::SetDexterityStat(int stat)
{
	m_dexterityStat = stat;

	ExportStats(m_name);
}

void PlayerStats::SetIntelligenceStat(int stat)
{
	m_intelligenceStat = stat;

	ExportStats(m_name);
}

void PlayerStats::SetVitalityStat(int stat)
{
	m_vitalityStat = stat;

	ExportStats(m_name);
}

void PlayerStats::SetArmorStat(int stat)
{
	m_armorStat = stat;

	ExportStats(m_name);
}

void PlayerStats::SetLuckStat(int stat)
{
	m_luckStat = stat;

	ExportStats(m_name);
}