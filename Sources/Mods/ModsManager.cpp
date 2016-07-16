/*************************************************************************
> File Name: ModsManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Mods manager class.
> Created Time: 2016/07/16
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>
#include <algorithm>

#include <inih/INIReader.h>

#include "ModsManager.h"

// Constructor, Destructor
ModsManager::ModsManager()
{

}

ModsManager::~ModsManager()
{
	ClearMods();
}

// Clearing
void ModsManager::ClearMods()
{
	for (size_t i = 0; i < m_vpMods.size(); ++i)
	{
		delete m_vpMods[i];
		m_vpMods[i] = nullptr;
	}

	m_vpMods.clear();
}

// Loading
void ModsManager::LoadMods()
{
	std::string modsIniFile = "Resources/Config/mods.ini";
	INIReader reader(modsIniFile);

	// Gameplay mods
	std::string gameplayMods = reader.Get("Gameplay", "LoadedGameplayMods", "");
	char * pch = strtok(&gameplayMods[0], ";");
	while (pch != nullptr)
	{
		AddMod(pch, true, false, false, false, false);
		pch = strtok(nullptr, ";");
	}

	// Graphics pack
	std::string graphicsPack = reader.Get("Graphics", "GraphicsPack", "Default");
	AddMod(graphicsPack, false, true, false, false, false);

	// Sound pack
	std::string soundPack = reader.Get("Sound", "SoundPack", "Default");
	AddMod(soundPack, false, false, true, false, false);

	// Load the HUD textures mod
	std::string HUDTheme = reader.Get("HUD", "HUDTextures", "Default");
	AddMod(HUDTheme, false, false, false, true, false);

	// Misc mods
	std::string miscMods = reader.Get("Misc", "LoadedMiscMods", "");
	pch = strtok(&miscMods[0], ";");
	while (pch != nullptr)
	{
		AddMod(pch, false, false, false, false, true);
		pch = strtok(nullptr, ";");
	}
}

// Saving
void ModsManager::SaveMods()
{
	std::ofstream file;

	// Open the file
	std::string modsIniFile = "Resources/Config/mods.ini";
	file.open(modsIniFile.c_str(), std::ios::out);

	file << "[Gameplay]\n";
	file << "LoadedGameplayMods=";
	for (size_t i = 0; i < m_vpMods.size(); ++i)
	{
		Mod* pMod = m_vpMods[i];
		if (pMod->m_gameplayMod == true)
		{
			file << pMod->m_modName.c_str() << ";";
		}
	}
	file << "\n\n";

	file << "[Graphics]\n";
	file << "GraphicsPack=" << GetGraphicsPack().c_str() << "\n";
	file << "\n";

	file << "[Sound]\n";
	file << "SoundPack=" << GetSoundPack().c_str() << "\n";
	file << "\n";

	file << "[HUD]\n";
	file << "HUDTextures=" << GetHUDTextureTheme().c_str() << "\n";
	file << "\n";

	file << "[Misc]\n";
	file << "LoadedMiscMods=";
	for (size_t i = 0; i < m_vpMods.size(); ++i)
	{
		Mod* pMod = m_vpMods[i];
		if (pMod->m_miscMod == true)
		{
			file << pMod->m_modName.c_str() << ";";
		}
	}
	file << "\n\n";
}

// Adding and removing active mods
void ModsManager::AddMod(std::string modName, bool gameplayMod, bool graphicsMod, bool soundMod, bool HUDMod, bool miscMod)
{
	Mod* pNewMod = new Mod();

	pNewMod->m_modName = modName;
	pNewMod->m_erase = false;
	pNewMod->m_gameplayMod = gameplayMod;
	pNewMod->m_graphicsMod = graphicsMod;
	pNewMod->m_soundMod = soundMod;
	pNewMod->m_HUDMod = HUDMod;
	pNewMod->m_miscMod = miscMod;

	m_vpMods.push_back(pNewMod);
}

bool IsNeedErasing(Mod* mod)
{
	bool isNeedErase = mod->m_erase;

	if (isNeedErase == true)
	{
		delete mod;
	}

	return isNeedErase;
}

void ModsManager::RemoveMod(std::string modName, bool gameplayMod, bool graphicsMod, bool soundMod, bool HUDMod, bool miscMod)
{
	for (size_t i = 0; i < m_vpMods.size(); ++i)
	{
		if (m_vpMods[i]->m_gameplayMod && gameplayMod ||
			m_vpMods[i]->m_graphicsMod && graphicsMod ||
			m_vpMods[i]->m_soundMod && soundMod ||
			m_vpMods[i]->m_HUDMod && HUDMod ||
			m_vpMods[i]->m_miscMod && miscMod)
		{
			if (m_vpMods[i]->m_modName == modName)
			{
				m_vpMods[i]->m_erase = true;
			}
		}
	}

	m_vpMods.erase(remove_if(m_vpMods.begin(), m_vpMods.end(), IsNeedErasing), m_vpMods.end());
}

// Accessors
int ModsManager::GetNumMods() const
{
	return m_vpMods.size();
}

Mod* ModsManager::GetMod(int index)
{
	return m_vpMods[index];
}

// Graphics pack
std::string ModsManager::GetGraphicsPack()
{
	for (size_t i = 0; i < m_vpMods.size(); ++i)
	{
		if (m_vpMods[i]->m_graphicsMod == true)
		{
			return m_vpMods[i]->m_modName;
		}
	}

	return "";
}

// Sound pack
std::string ModsManager::GetSoundPack()
{
	for (size_t i = 0; i < m_vpMods.size(); ++i)
	{
		if (m_vpMods[i]->m_soundMod == true)
		{
			return m_vpMods[i]->m_modName;
		}
	}

	return "";
}

// HUD Theme
std::string ModsManager::GetHUDTextureTheme()
{
	for (size_t i = 0; i < m_vpMods.size(); ++i)
	{
		if (m_vpMods[i]->m_HUDMod == true)
		{
			return m_vpMods[i]->m_modName;
		}
	}

	return "";
}