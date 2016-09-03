/*************************************************************************
> File Name: ModMenu.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Mod menu.
> Created Time: 2016/09/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MOD_MENU_H
#define CUBBY_MOD_MENU_H

#include <GUI/FormattedLabel.h>
#include <GUI/GUIWindow.h>
#include <GUI/OptionController.h>

#include "../FrontendManager.h"

// Forward declaration
class ModMenu;

struct ModButtonData
{
	ModMenu* m_pModMenu;
	Button* m_pModButton;
	std::string m_modName;
	std::string m_modDescription;
	std::string m_modAuthor;
	bool m_toggled;
	bool m_allowToggleOff;
	bool m_allowMultipleSelection;
	bool m_gameplayButton;
	bool m_graphicsButton;
	bool m_soundButton;
	bool m_HUDButton;
	bool m_miscButton;
};

class ModMenu : public FrontendPage
{
public:
	// Constructor, Destructor
	ModMenu(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontPageManager, int windowWidth, int windowHeight);
	~ModMenu();

	void Reset();

	void ClearModButtonData();

	void SetWindowDimensions(int windowWidth, int windowHeight);

	// Pre-tick the already loaded mods and set them as toggled
	void SelectLoadedMods();

	// Are we returning to the main menu, or are we in the game?
	void SetReturnToMainMenu(bool mainMenu);

	// Skinning
	void SkinGUI();
	void UnSkinGUI();

	// Load unload
	void Load();
	void Unload();

	void CreateGameplayModButtons();
	void RemoveGameplayModButtons();

	void CreateGraphicsModButtons();
	void RemoveGraphicsModButtons();

	void CreateSoundModButtons();
	void RemoveSoundModButtons();

	void CreateHUDModButtons();
	void RemoveHUDModButtons();

	void CreateMiscModButtons();
	void RemoveMiscModButtons();

	// Tooltips
	void UpdateToolTipAppear(float dt);
	void ShowTooltip(ModButtonData* pModButtonData);
	void HideTooltip();

	// Update
	void Update(float dt);

	// Render
	void Render();
	void Render2D();

protected:
	static void _CloseExitPressed(void* pData);
	void CloseExitPressed() const;

	static void _GameplayTabPressed(void* pData);
	void GameplayTabPressed();

	static void _GraphicsTabPressed(void* pData);
	void GraphicsTabPressed();

	static void _SoundTabPressed(void* pData);
	void SoundTabPressed();

	static void _GUITabPressed(void* pData);
	void GUITabPressed();

	static void _MiscTabPressed(void* pData);
	void MiscTabPressed();

	static void _ModButtonPressed(void* pData);
	void ModButtonPressed(ModButtonData* pModButtonData);

	static void _ModButtonEntered(void* pData);
	void ModButtonEntered(ModButtonData* pModButtonData);

	static void _ModButtonExited(void* pData);
	void ModButtonExited(ModButtonData* pModButtonData);

private:
	bool m_loaded;

	int m_modWindowWidth;
	int m_modWindowHeight;
	int m_titlebarWidth;
	int m_titlebarHeight;

	// Are we in the front-end or in the game
	bool m_returnToMainMenu;

	int m_modButtonWidth;
	int m_modButtonheight;
	int m_modButtonSpace;

	// Mod window
	GUIWindow* m_pModWindow;
	Icon* m_pTitleBarIcon;
	Icon* m_pTitleBarBackgroundIcon;
	Icon* m_pModWindowBackgroundIcon;
	Button* m_pCloseExitButton;

	// Tabs
	OptionController* m_pModsModeController;
	OptionBox* m_pGameplayMode;
	OptionBox* m_pGraphicsMode;
	OptionBox* m_pSoundMode;
	OptionBox* m_pHUDMode;
	OptionBox* m_pMiscMode;

	ScrollBar* m_pModsScrollbar;
	std::vector<Button*> m_vpGameplayModButtons;
	std::vector<Button*> m_vpGraphicsModButtons;
	std::vector<Button*> m_vpSoundModButtons;
	std::vector<Button*> m_vpHUDModButtons;
	std::vector<Button*> m_vpMiscModButtons;

	std::vector<ModButtonData*> m_vpModButtonData;

	// Tooltip
	Icon* m_pTooltipBackgroundCommon;
	Icon* m_pTooltipBackgroundUncommon;
	Icon* m_pTooltipBackgroundMagical;
	Icon* m_pTooltipBackgroundRare;
	Icon* m_pTooltipBackgroundEpic;
	Label* m_pTooltipNameLabel;
	FormattedLabel* m_pTooltipDescriptionLabel;
	Label* m_pTooltipSlotLabel;
	Label* m_pTooltipQualityLabel;
	bool m_toolTipVisible;
	int m_tooltipWidth;
	int m_tooltipHeight;
	int m_tooltipDescBorder;
	ItemQuality m_tooltipQuality;
	float m_tooltipAppearDelayTimer;
	bool m_toolTipComponentsAdded;
};

#endif