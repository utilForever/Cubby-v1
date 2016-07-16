/*************************************************************************
> File Name: ModsManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Mods manager class.
> Created Time: 2016/07/16
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MODS_MANAGER_H
#define CUBBY_MODS_MANAGER_H

#include <vector>

struct Mod
{
	std::string m_modName;
	bool m_erase;
	bool m_gameplayMod;
	bool m_graphicsMod;
	bool m_soundMod;
	bool m_HUDMod;
	bool m_miscMod;
};

class ModsManager
{
public:
	// Constructor, Destructor
	ModsManager();
	~ModsManager();

	// Clearing
	void ClearMods();

	// Loading
	void LoadMods();

	// Saving
	void SaveMods();

	// Adding and removing active mods
	void AddMod(std::string modName, bool gameplayMod, bool graphicsMod, bool soundMod, bool HUDMod, bool miscMod);
	void RemoveMod(std::string modName, bool gameplayMod, bool graphicsMod, bool soundMod, bool HUDMod, bool miscMod);

	// Accessors
	int GetNumMods() const;
	Mod* GetMod(int index);

	// Graphics pack
	std::string GetGraphicsPack();

	// Sound pack
	std::string GetSoundPack();

	// HUD Theme
	std::string GetHUDTextureTheme();

private:
	std::vector<Mod*> m_vpMods;
};

#endif