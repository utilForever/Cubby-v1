/*************************************************************************
> File Name: PlayerStats.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Player stat class.
> Created Time: 2016/08/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_PLAYER_STATS_H
#define CUBBY_PLAYER_STATS_H

#include "PlayerClass.h"

// Forward declaration
class Player;

class PlayerStats
{
public:
	// Constructor, Destructor
	PlayerStats(Player* pPlayer);
	~PlayerStats();

	// Reset
	void ResetStats();

	// Supress export
	void SetSupressExport(bool supress);

	// Loading / Saving
	void ImportStats(std::string playerName);
	void ExportStats(std::string playerName) const;

	// Accessors
	PlayerClass GetClass() const;
	int GetLevel() const;
	int GetExperience() const;
	int GetMaxExperience() const;
	int GetNumPointsAvailable() const;
	int GetStrengthStat() const;
	int GetDexterityStat() const;
	int GetIntelligenceStat() const;
	int GetVitalityStat() const;
	int GetArmorStat() const;
	int GetLuckStat() const;

	// Setters
	void SetClass(PlayerClass playerClass);
	void GiveExperience(int amount);
	void SetNumPointsAvailable(int numPoints);
	void SetStrengthStat(int stat);
	void SetDexterityStat(int stat);
	void SetIntelligenceStat(int stat);
	void SetVitalityStat(int stat);
	void SetArmorStat(int stat);
	void SetLuckStat(int stat);

private:
	Player* m_pPlayer;

	// Player name
	std::string m_name;

	// Player class
	PlayerClass m_class;

	// Level
	int m_level;

	// Experience
	int m_experience;
	int m_maxExperience;

	// Num points to allocate to stats
	int m_numPointsAvailable;

	// Stats
	int m_strengthStat;
	int m_dexterityStat;
	int m_intelligenceStat;
	int m_vitalityStat;
	int m_armorStat;
	int m_luckStat;

	// Force to stop export player stats
	bool m_supressExport;
};

#endif