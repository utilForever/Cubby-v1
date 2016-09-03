/*************************************************************************
> File Name: ModMenu.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Mod menu.
> Created Time: 2016/09/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "ModMenu.h"
#include "Mods/ModsManager.h"
#include "CubbyGame.h"
#include "Utils/FileUtils.h"
#include "../../../Libraries/inih/INIReader.h"
#include "Maths/3DMaths.h"

// Constructor, Destructor
ModMenu::ModMenu(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontPageManager, int windowWidth, int windowHeight) :
	FrontendPage(pRenderer, pGUI, pFrontPageManager, FrontendScreen::ModMenu, windowWidth, windowHeight)
{
	// Mod Window
	m_pModWindow = new GUIWindow(m_pRenderer, m_pFrontendManager->GetFrontendFontMedium(), "Mods");
	m_pModWindow->AllowMoving(true);
	m_pModWindow->AllowClosing(false);
	m_pModWindow->AllowMinimizing(false);
	m_pModWindow->AllowScrolling(false);
	m_pModWindow->SetRenderTitleBar(true);
	m_pModWindow->SetRenderWindowBackground(true);
	m_pModWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pModWindow->Hide();

	m_pTitleBarIcon = new Icon(m_pRenderer, "", 44, 44);
	m_pTitleBarIcon->SetDepth(4.0f);

	m_pModWindowBackgroundIcon = new Icon(m_pRenderer, "", 600, 475);
	m_pModWindowBackgroundIcon->SetDepth(1.0f);

	m_pTitleBarBackgroundIcon = new Icon(m_pRenderer, "", 86, 35);
	m_pTitleBarBackgroundIcon->SetDepth(1.0f);

	m_pCloseExitButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCloseExitButton->SetLabelOffset(0, 5);
	m_pCloseExitButton->SetCallBackFunction(_CloseExitPressed);
	m_pCloseExitButton->SetCallBackData(this);

	m_pModWindow->AddComponent(m_pTitleBarIcon);
	m_pModWindow->AddComponent(m_pCloseExitButton);

	// Tabs
	m_pModsModeController = new OptionController(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), "Mode");
	m_pModsModeController->SetDisplayLabel(false);
	m_pModsModeController->SetDisplayBorder(false);

	m_pGameplayMode = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), "Gameplay", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pGameplayMode->SetCallBackFunction(_GameplayTabPressed);
	m_pGameplayMode->SetCallBackData(this);
	m_pGameplayMode->SetDepth(2.0f);
	m_pGameplayMode->SetPressedOffset(0, -2);

	m_pGraphicsMode = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), "Graphics", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pGraphicsMode->SetCallBackFunction(_GraphicsTabPressed);
	m_pGraphicsMode->SetCallBackData(this);
	m_pGraphicsMode->SetDepth(2.1f);
	m_pGraphicsMode->SetPressedOffset(0, -2);

	m_pSoundMode = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), "Sound", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pSoundMode->SetCallBackFunction(_SoundTabPressed);
	m_pSoundMode->SetCallBackData(this);
	m_pSoundMode->SetDepth(2.2f);
	m_pSoundMode->SetPressedOffset(0, -2);

	m_pHUDMode = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), "HUD", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pHUDMode->SetCallBackFunction(_GUITabPressed);
	m_pHUDMode->SetCallBackData(this);
	m_pHUDMode->SetDepth(2.3f);
	m_pHUDMode->SetPressedOffset(0, -2);

	m_pMiscMode = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), "Misc", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pMiscMode->SetCallBackFunction(_MiscTabPressed);
	m_pMiscMode->SetCallBackData(this);
	m_pMiscMode->SetDepth(2.4f);
	m_pMiscMode->SetPressedOffset(0, -2);

	// Scrollbar
	m_pModsScrollbar = new ScrollBar(m_pRenderer);
	m_pModsScrollbar->SetScrollDirection(ScrollBarDirection::Vertical);
	m_pModsScrollbar->SetScrollSize(0.25f);
	m_pModsScrollbar->SetScrollPosition(1.0f);
	m_pModsScrollbar->SetDepth(4.0f);
	m_pModsScrollbar->SetScissorEnabled(true);

	m_pModsModeController->Add(m_pGameplayMode);
	m_pModsModeController->Add(m_pGraphicsMode);
	m_pModsModeController->Add(m_pSoundMode);
	m_pModsModeController->Add(m_pHUDMode);
	m_pModsModeController->Add(m_pMiscMode);
	m_pGameplayMode->SetToggled(true);

	m_pModWindow->AddComponent(m_pModsModeController);
	m_pModWindow->AddComponent(m_pModsScrollbar);

	// Tooltip
	m_pTooltipBackgroundCommon = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundCommon->SetDepth(8.5f);

	m_pTooltipBackgroundUncommon = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundUncommon->SetDepth(8.5f);

	m_pTooltipBackgroundMagical = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundMagical->SetDepth(8.5f);

	m_pTooltipBackgroundRare = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundRare->SetDepth(8.5f);

	m_pTooltipBackgroundEpic = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundEpic->SetDepth(8.5f);

	char nameText[] = "[ITEM]";
	m_pTooltipNameLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), nameText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pTooltipNameLabel->SetOutline(true);
	m_pTooltipNameLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipNameLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont30Outline());
	m_pTooltipNameLabel->SetDepth(8.5f);

	char descText[] = "[REPLACE ME]";
	m_pTooltipDescriptionLabel = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), descText);
	m_pTooltipDescriptionLabel->SetOutline(true);
	m_pTooltipDescriptionLabel->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pTooltipDescriptionLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipDescriptionLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pTooltipDescriptionLabel->SetDepth(8.5f);
	m_pTooltipDescriptionLabel->SetWordWrap(true);

	char slotText[] = "[SLOT]";
	m_pTooltipSlotLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), slotText, Color(0.5f, 0.5f, 0.5f, 1.0f));
	m_pTooltipSlotLabel->SetOutline(true);
	m_pTooltipSlotLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipSlotLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont20Outline());
	m_pTooltipSlotLabel->SetDepth(8.5f);

	char qualityText[] = "[QUALITY]";
	m_pTooltipQualityLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), qualityText, Color(0.5f, 0.5f, 0.5f, 1.0f));
	m_pTooltipQualityLabel->SetOutline(true);
	m_pTooltipQualityLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipQualityLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont20Outline());
	m_pTooltipQualityLabel->SetDepth(8.5f);

	ModMenu::SetWindowDimensions(m_windowWidth, m_windowHeight);

	m_returnToMainMenu = false;
}

ModMenu::~ModMenu()
{
	RemoveGameplayModButtons();
	RemoveGraphicsModButtons();
	RemoveSoundModButtons();
	RemoveHUDModButtons();
	RemoveMiscModButtons();

	delete m_pModWindow;
	delete m_pTitleBarIcon;
	delete m_pTitleBarBackgroundIcon;
	delete m_pModWindowBackgroundIcon;
	delete m_pCloseExitButton;

	// Tabs
	delete m_pModsModeController;
	delete m_pGameplayMode;
	delete m_pGraphicsMode;
	delete m_pSoundMode;
	delete m_pHUDMode;
	delete m_pMiscMode;

	// Scrollbar
	delete m_pModsScrollbar;

	// Tooltip
	delete m_pTooltipBackgroundCommon;
	delete m_pTooltipBackgroundUncommon;
	delete m_pTooltipBackgroundMagical;
	delete m_pTooltipBackgroundRare;
	delete m_pTooltipBackgroundEpic;
	delete m_pTooltipNameLabel;
	delete m_pTooltipDescriptionLabel;
	delete m_pTooltipSlotLabel;
	delete m_pTooltipQualityLabel;
}

void ModMenu::Reset()
{
}

void ModMenu::ClearModButtonData()
{
	for (size_t i = 0; i < m_vpModButtonData.size(); ++i)
	{
		delete m_vpModButtonData[i];
		m_vpModButtonData[i] = nullptr;
	}
	m_vpModButtonData.clear();
}

void ModMenu::SetWindowDimensions(int windowWidth, int windowHeight)
{
	FrontendPage::SetWindowDimensions(windowWidth, windowHeight);

	m_modButtonWidth = 170;
	m_modButtonheight = 150;
	m_modButtonSpace = 15;

	m_modWindowWidth = 600;
	m_modWindowHeight = 510;
	m_titlebarWidth = 118;
	m_titlebarHeight = 35;

	m_pModWindow->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (m_modWindowWidth * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) - (m_modWindowHeight * 0.5f)), m_modWindowWidth, m_modWindowHeight);
	m_pModWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);
	m_pModWindow->SetTitleOffset(50, 5);
	m_pModWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pModWindow->SetApplicationBorder(25, 15, 10, 10);

	m_pTitleBarIcon->SetDimensions(0, m_modWindowHeight, 44, 44);

	m_pCloseExitButton->SetDimensions(m_modWindowWidth - 32, m_modWindowHeight, 32, 32);

	// Tabs
	m_pModsModeController->SetDimensions(0, m_modWindowHeight, m_modWindowWidth, 32);
	m_pGameplayMode->SetDisplayLabel(false);
	m_pGameplayMode->SetDimensions(m_modWindowWidth - 32 - 375, 0, 75, 32);
	m_pGameplayMode->SetDisplayLabel(true);
	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pGameplayMode->GetLabel().GetText().c_str());
	m_pGameplayMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pGraphicsMode->SetDisplayLabel(false);
	m_pGraphicsMode->SetDimensions(m_modWindowWidth - 32 - 300, 0, 75, 32);
	m_pGraphicsMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pGraphicsMode->GetLabel().GetText().c_str());
	m_pGraphicsMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pSoundMode->SetDisplayLabel(false);
	m_pSoundMode->SetDimensions(m_modWindowWidth - 32 - 225, 0, 75, 32);
	m_pSoundMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pSoundMode->GetLabel().GetText().c_str());
	m_pSoundMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pHUDMode->SetDisplayLabel(false);
	m_pHUDMode->SetDimensions(m_modWindowWidth - 32 - 150, 0, 75, 32);
	m_pHUDMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pHUDMode->GetLabel().GetText().c_str());
	m_pHUDMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pMiscMode->SetDisplayLabel(false);
	m_pMiscMode->SetDimensions(m_modWindowWidth - 32 - 75, 0, 75, 32);
	m_pMiscMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pMiscMode->GetLabel().GetText().c_str());
	m_pMiscMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	// Scrollbar
	int scrollbarHeight = m_modWindowHeight - 6;
	m_pModsScrollbar->SetDimensions(m_modWindowWidth - 28, 4, 24, scrollbarHeight);
	m_pModsScrollbar->SetScrollArea(-(m_modWindowWidth - 40), 0, m_modWindowWidth - 40, scrollbarHeight);
}

// Pre-tick the already loaded mods and set them as toggled
void ModMenu::SelectLoadedMods()
{
	int buttonWidth = m_modButtonWidth;

	ModsManager* pModsManager = CubbyGame::GetInstance()->GetModsManager();

	for (int i = 0; i < pModsManager->GetNumMods(); ++i)
	{
		Mod* pMod = pModsManager->GetMod(i);

		for (int j = 0; j < m_vpModButtonData.size(); ++j)
		{
			if (pMod->m_gameplayMod && m_vpModButtonData[j]->m_gameplayButton ||
				pMod->m_graphicsMod && m_vpModButtonData[j]->m_graphicsButton ||
				pMod->m_soundMod && m_vpModButtonData[j]->m_soundButton ||
				pMod->m_HUDMod && m_vpModButtonData[j]->m_HUDButton ||
				pMod->m_miscMod && m_vpModButtonData[j]->m_miscButton)
			{
				if (pMod->m_modName == m_vpModButtonData[j]->m_modName)
				{
					m_vpModButtonData[j]->m_toggled = true;
					std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
					std::string tickIcon = "Resources/textures/gui/" + themeName + "/common/tick.tga";
					m_vpModButtonData[j]->m_pModButton->AddIcon(m_pRenderer, tickIcon.c_str(), 32, 32, 32, 32, buttonWidth - 38, 4, 3.25f);
				}
			}
		}
	}
}

// Are we returning to the main menu, or are we in the game?
void ModMenu::SetReturnToMainMenu(bool mainMenu)
{
	m_returnToMainMenu = mainMenu;
}

// Skinning
void ModMenu::SkinGUI()
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/ModMenu/mod_title_icon.tga";
	m_pTitleBarIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/ModMenu/mod_window_background.tga";
	m_pModWindowBackgroundIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/ModMenu/titlebar_background.tga";
	m_pTitleBarBackgroundIcon->SetIcon(iconName);

	m_pModWindow->SetBackgroundIcon(m_pModWindowBackgroundIcon);
	m_pModWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	Point location = m_pModWindow->GetLocation();
	m_pModWindow->SetDimensions(location.x, location.y, m_modWindowWidth, m_modWindowHeight);
	m_pModWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);

	m_pFrontendManager->SetScrollbarIcons(m_pModsScrollbar);

	m_pCloseExitButton->SetDefaultIcon(m_pFrontendManager->GetCloseExitButtonIcon());
	m_pCloseExitButton->SetHoverIcon(m_pFrontendManager->GetCloseExitButtonIconHover());
	m_pCloseExitButton->SetSelectedIcon(m_pFrontendManager->GetCloseExitButtonIconPressed());
	m_pCloseExitButton->SetDisabledIcon(m_pFrontendManager->GetCloseExitButtonIcon());

	m_pFrontendManager->SetTabIcons(m_pGameplayMode);
	m_pFrontendManager->SetTabIcons(m_pGraphicsMode);
	m_pFrontendManager->SetTabIcons(m_pSoundMode);
	m_pFrontendManager->SetTabIcons(m_pHUDMode);
	m_pFrontendManager->SetTabIcons(m_pMiscMode);

	m_pGameplayMode->SetDisplayLabel(false);
	m_pGameplayMode->SetDimensions(m_modWindowWidth - 32 - 375, 0, 75, 32);
	m_pGameplayMode->SetDisplayLabel(true);
	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pGameplayMode->GetLabel().GetText().c_str());
	m_pGameplayMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pGraphicsMode->SetDisplayLabel(false);
	m_pGraphicsMode->SetDimensions(m_modWindowWidth - 32 - 300, 0, 75, 32);
	m_pGraphicsMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pGraphicsMode->GetLabel().GetText().c_str());
	m_pGraphicsMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pSoundMode->SetDisplayLabel(false);
	m_pSoundMode->SetDimensions(m_modWindowWidth - 32 - 225, 0, 75, 32);
	m_pSoundMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pSoundMode->GetLabel().GetText().c_str());
	m_pSoundMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pHUDMode->SetDisplayLabel(false);
	m_pHUDMode->SetDimensions(m_modWindowWidth - 32 - 150, 0, 75, 32);
	m_pHUDMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pHUDMode->GetLabel().GetText().c_str());
	m_pHUDMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pMiscMode->SetDisplayLabel(false);
	m_pMiscMode->SetDimensions(m_modWindowWidth - 32 - 75, 0, 75, 32);
	m_pMiscMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pMiscMode->GetLabel().GetText().c_str());
	m_pMiscMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	iconName = "Resources/textures/gui/" + themeName + "/common/Tooltips/tooltip_background_common.tga";
	m_pTooltipBackgroundCommon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/Tooltips/tooltip_background_uncommon.tga";
	m_pTooltipBackgroundUncommon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/Tooltips/tooltip_background_magical.tga";
	m_pTooltipBackgroundMagical->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/Tooltips/tooltip_background_rare.tga";
	m_pTooltipBackgroundRare->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/Tooltips/tooltip_background_epic.tga";
	m_pTooltipBackgroundEpic->SetIcon(iconName);

	m_pGameplayMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pGameplayMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pGameplayMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pGameplayMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pGraphicsMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pGraphicsMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pGraphicsMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pGraphicsMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pSoundMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pSoundMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pSoundMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pSoundMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pHUDMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pHUDMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pHUDMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pHUDMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pMiscMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pMiscMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pMiscMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pMiscMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
}

void ModMenu::UnSkinGUI()
{
	m_pModsScrollbar->SetDefaultIcons(m_pRenderer);
}

// Load unload
void ModMenu::Load()
{
	m_pGameplayMode->SetToggled(true);
	GameplayTabPressed();

	m_toolTipVisible = false;
	m_tooltipAppearDelayTimer = 0.0f;
	m_toolTipComponentsAdded = false;
	m_tooltipQuality = ItemQuality::Common;

	m_loaded = true;
}

void ModMenu::Unload()
{
	CubbyGame::GetInstance()->GetModsManager()->SaveMods();

	HideTooltip();

	RemoveGameplayModButtons();
	RemoveGraphicsModButtons();
	RemoveSoundModButtons();
	RemoveHUDModButtons();
	RemoveMiscModButtons();

	m_pGUI->RemoveWindow(m_pModWindow);

	ClearModButtonData();

	m_loaded = false;
}

void ModMenu::CreateGameplayModButtons()
{
	int buttonWidth = m_modButtonWidth;
	int buttonHeight = m_modButtonheight;
	int buttonSpacer = m_modButtonSpace;
	int buttonAndSpacerWidth = buttonWidth + buttonSpacer;
	int buttonAndSpacerHeight = buttonHeight + buttonSpacer;

	int buttonX;
	int buttonY = m_modWindowHeight - buttonHeight - 17;

	char importDirectory[128];
	sprintf(importDirectory, "Resources/mods/gameplay/*.*");

	std::vector<std::string> listFiles;
	listFiles = ListFilesInDirectory(importDirectory);
	int modButtonCounter = 0;
	int yCounter = 0;
	while (modButtonCounter < listFiles.size())
	{
		if (strcmp(listFiles[modButtonCounter].c_str(), ".") == 0 || strcmp(listFiles[modButtonCounter].c_str(), "..") == 0)
		{
			modButtonCounter++;
			continue;
		}

		buttonX = -(m_modWindowWidth - 42);

		for (int x = 0; x < 3 && modButtonCounter < listFiles.size(); x++)
		{
			Button* pNewButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont35(), "");
			m_pFrontendManager->SetButtonIcons(pNewButton, ButtonSize::Size225x75);
			pNewButton->SetDimensions(buttonX, buttonY, buttonWidth, buttonHeight);
			pNewButton->SetPressedOffset(0, -1);

			// Add thumbnail icon
			char thumbnailIcon[128];
			sprintf(thumbnailIcon, "Resources/mods/gameplay/%s/thumbnail.tga", listFiles[modButtonCounter].c_str());
			pNewButton->AddIcon(m_pRenderer, thumbnailIcon, buttonWidth, buttonHeight, buttonWidth - 16, buttonHeight - 16, 8, 8, 2.5f);

			// Add header text
			pNewButton->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), listFiles[modButtonCounter].c_str(), Color(1.0f, 1.0f, 1.0f, 1.0f), 7, buttonHeight - 20, true, Color(0.0f, 0.0f, 0.0f, 1.0f));

			ModButtonData* pModButtonData = new ModButtonData();
			pModButtonData->m_pModMenu = this;
			pModButtonData->m_pModButton = pNewButton;
			pModButtonData->m_modName = listFiles[modButtonCounter];
			pModButtonData->m_toggled = false;
			pModButtonData->m_allowToggleOff = true;
			pModButtonData->m_allowMultipleSelection = true;
			pModButtonData->m_gameplayButton = true;
			pModButtonData->m_graphicsButton = false;
			pModButtonData->m_soundButton = false;
			pModButtonData->m_HUDButton = false;
			pModButtonData->m_miscButton = false;

			// Load the meta data file for the mod description and author
			std::string settingsIniFile = "Resources/mods/gameplay/" + listFiles[modButtonCounter] + "/description.ini";
			INIReader reader(settingsIniFile);

			if (reader.ParseError() >= 0)
			{
				pModButtonData->m_modDescription = reader.Get("MetaData", "Description", "[DECRIPTION]");
				pModButtonData->m_modAuthor = reader.Get("MetaData", "Author", "[AUTHOR]");
			}

			pNewButton->SetCallBackFunction(_ModButtonPressed);
			pNewButton->SetCallBackData(pModButtonData);

			pNewButton->SetEnterCallBackFunction(_ModButtonEntered);
			pNewButton->SetEnterCallBackData(pModButtonData);

			pNewButton->SetExitCallBackFunction(_ModButtonExited);
			pNewButton->SetExitCallBackData(pModButtonData);

			m_vpModButtonData.push_back(pModButtonData);

			m_pModsScrollbar->AddScrollAreaItem(pNewButton);

			m_vpGameplayModButtons.push_back(pNewButton);

			buttonX += buttonAndSpacerWidth;

			modButtonCounter++;
		}

		yCounter++;

		buttonY -= buttonAndSpacerHeight;
	}
}

void ModMenu::RemoveGameplayModButtons()
{
	m_pModsScrollbar->ClearScrollAreaItems();

	for (size_t i = 0; i < m_vpGameplayModButtons.size(); ++i)
	{
		delete m_vpGameplayModButtons[i];
		m_vpGameplayModButtons[i] = nullptr;
	}

	m_vpGameplayModButtons.clear();
}

void ModMenu::CreateGraphicsModButtons()
{
	int buttonWidth = m_modButtonWidth;
	int buttonHeight = m_modButtonheight;
	int buttonSpacer = m_modButtonSpace;
	int buttonAndSpacerWidth = buttonWidth + buttonSpacer;
	int buttonAndSpacerHeight = buttonHeight + buttonSpacer;

	int buttonX;
	int buttonY = m_modWindowHeight - buttonHeight - 17;

	char importDirectory[128];
	sprintf(importDirectory, "Resources/graphics/*.*");

	std::vector<std::string> listFiles;
	listFiles = ListFilesInDirectory(importDirectory);
	int modButtonCounter = 0;
	int yCounter = 0;
	while (modButtonCounter < listFiles.size())
	{
		if (strcmp(listFiles[modButtonCounter].c_str(), ".") == 0 || strcmp(listFiles[modButtonCounter].c_str(), "..") == 0)
		{
			modButtonCounter++;
			continue;
		}

		buttonX = -(m_modWindowWidth - 42);

		for (int x = 0; x < 3 && modButtonCounter < listFiles.size(); ++x)
		{
			Button* pNewButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont35(), "");
			m_pFrontendManager->SetButtonIcons(pNewButton, ButtonSize::Size225x75);
			pNewButton->SetDimensions(buttonX, buttonY, buttonWidth, buttonHeight);
			pNewButton->SetPressedOffset(0, -1);

			// Add thumbnail icon
			char thumbnailIcon[128];
			sprintf(thumbnailIcon, "Resources/graphics/%s/thumbnail.tga", listFiles[modButtonCounter].c_str());
			pNewButton->AddIcon(m_pRenderer, thumbnailIcon, buttonWidth, buttonHeight, buttonWidth - 16, buttonHeight - 16, 8, 8, 2.5f);

			// Add header text
			pNewButton->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), listFiles[modButtonCounter].c_str(), Color(1.0f, 1.0f, 1.0f, 1.0f), 7, buttonHeight - 20, true, Color(0.0f, 0.0f, 0.0f, 1.0f));

			ModButtonData* pModButtonData = new ModButtonData();
			pModButtonData->m_pModMenu = this;
			pModButtonData->m_pModButton = pNewButton;
			pModButtonData->m_modName = listFiles[modButtonCounter];
			pModButtonData->m_toggled = false;
			pModButtonData->m_allowToggleOff = false;
			pModButtonData->m_allowMultipleSelection = false;
			pModButtonData->m_gameplayButton = false;
			pModButtonData->m_graphicsButton = true;
			pModButtonData->m_soundButton = false;
			pModButtonData->m_HUDButton = false;
			pModButtonData->m_miscButton = false;

			// Load the meta data file for the mod description and author
			std::string settingsIniFile = "Resources/graphics/" + listFiles[modButtonCounter] + "/description.ini";
			INIReader reader(settingsIniFile);

			if (reader.ParseError() >= 0)
			{
				pModButtonData->m_modDescription = reader.Get("MetaData", "Description", "[DECRIPTION]");
				pModButtonData->m_modAuthor = reader.Get("MetaData", "Author", "[AUTHOR]");
			}

			pNewButton->SetCallBackFunction(_ModButtonPressed);
			pNewButton->SetCallBackData(pModButtonData);

			pNewButton->SetEnterCallBackFunction(_ModButtonEntered);
			pNewButton->SetEnterCallBackData(pModButtonData);

			pNewButton->SetExitCallBackFunction(_ModButtonExited);
			pNewButton->SetExitCallBackData(pModButtonData);

			m_vpModButtonData.push_back(pModButtonData);

			m_pModsScrollbar->AddScrollAreaItem(pNewButton);

			m_vpGraphicsModButtons.push_back(pNewButton);

			buttonX += buttonAndSpacerWidth;

			modButtonCounter++;
		}

		yCounter++;

		buttonY -= buttonAndSpacerHeight;
	}
}

void ModMenu::RemoveGraphicsModButtons()
{
	m_pModsScrollbar->ClearScrollAreaItems();

	for (size_t i = 0; i < m_vpGraphicsModButtons.size(); ++i)
	{
		delete m_vpGraphicsModButtons[i];
		m_vpGraphicsModButtons[i] = nullptr;
	}

	m_vpGraphicsModButtons.clear();
}

void ModMenu::CreateSoundModButtons()
{
	int buttonWidth = m_modButtonWidth;
	int buttonHeight = m_modButtonheight;
	int buttonSpacer = m_modButtonSpace;
	int buttonAndSpacerWidth = buttonWidth + buttonSpacer;
	int buttonAndSpacerHeight = buttonHeight + buttonSpacer;

	int buttonX;
	int buttonY = m_modWindowHeight - buttonHeight - 17;

	char importDirectory[128];
	sprintf(importDirectory, "Resources/audio/*.*");

	std::vector<std::string> listFiles;
	listFiles = ListFilesInDirectory(importDirectory);
	int modButtonCounter = 0;
	int yCounter = 0;
	while (modButtonCounter < listFiles.size())
	{
		if (strcmp(listFiles[modButtonCounter].c_str(), ".") == 0 || strcmp(listFiles[modButtonCounter].c_str(), "..") == 0)
		{
			modButtonCounter++;
			continue;
		}

		buttonX = -(m_modWindowWidth - 42);

		for (int x = 0; x < 3 && modButtonCounter < listFiles.size(); ++x)
		{
			Button* pNewButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont35(), "");
			m_pFrontendManager->SetButtonIcons(pNewButton, ButtonSize::Size225x75);
			pNewButton->SetDimensions(buttonX, buttonY, buttonWidth, buttonHeight);
			pNewButton->SetPressedOffset(0, -1);

			// Add thumbnail icon
			char thumbnailIcon[128];
			sprintf(thumbnailIcon, "Resources/audio/%s/thumbnail.tga", listFiles[modButtonCounter].c_str());
			pNewButton->AddIcon(m_pRenderer, thumbnailIcon, buttonWidth, buttonHeight, buttonWidth - 16, buttonHeight - 16, 8, 8, 2.5f);

			// Add header text
			pNewButton->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), listFiles[modButtonCounter].c_str(), Color(1.0f, 1.0f, 1.0f, 1.0f), 7, buttonHeight - 20, true, Color(0.0f, 0.0f, 0.0f, 1.0f));

			ModButtonData* pModButtonData = new ModButtonData();
			pModButtonData->m_pModMenu = this;
			pModButtonData->m_pModButton = pNewButton;
			pModButtonData->m_modName = listFiles[modButtonCounter];
			pModButtonData->m_toggled = false;
			pModButtonData->m_allowToggleOff = false;
			pModButtonData->m_allowMultipleSelection = false;
			pModButtonData->m_gameplayButton = false;
			pModButtonData->m_graphicsButton = false;
			pModButtonData->m_soundButton = true;
			pModButtonData->m_HUDButton = false;
			pModButtonData->m_miscButton = false;

			// Load the meta data file for the mod description and author
			std::string settingsIniFile = "Resources/audio/" + listFiles[modButtonCounter] + "/description.ini";
			INIReader reader(settingsIniFile);

			if (reader.ParseError() >= 0)
			{
				pModButtonData->m_modDescription = reader.Get("MetaData", "Description", "[DECRIPTION]");
				pModButtonData->m_modAuthor = reader.Get("MetaData", "Author", "[AUTHOR]");
			}

			pNewButton->SetCallBackFunction(_ModButtonPressed);
			pNewButton->SetCallBackData(pModButtonData);

			pNewButton->SetEnterCallBackFunction(_ModButtonEntered);
			pNewButton->SetEnterCallBackData(pModButtonData);

			pNewButton->SetExitCallBackFunction(_ModButtonExited);
			pNewButton->SetExitCallBackData(pModButtonData);

			m_vpModButtonData.push_back(pModButtonData);

			m_pModsScrollbar->AddScrollAreaItem(pNewButton);

			m_vpSoundModButtons.push_back(pNewButton);

			buttonX += buttonAndSpacerWidth;

			modButtonCounter++;
		}

		yCounter++;

		buttonY -= buttonAndSpacerHeight;
	}
}

void ModMenu::RemoveSoundModButtons()
{
	m_pModsScrollbar->ClearScrollAreaItems();

	for (size_t i = 0; i < m_vpSoundModButtons.size(); ++i)
	{
		delete m_vpSoundModButtons[i];
		m_vpSoundModButtons[i] = nullptr;
	}

	m_vpSoundModButtons.clear();
}

void ModMenu::CreateHUDModButtons()
{
	int buttonWidth = m_modButtonWidth;
	int buttonHeight = m_modButtonheight;
	int buttonSpacer = m_modButtonSpace;
	int buttonAndSpacerWidth = buttonWidth + buttonSpacer;
	int buttonAndSpacerHeight = buttonHeight + buttonSpacer;

	int buttonX;
	int buttonY = m_modWindowHeight - buttonHeight - 17;

	char importDirectory[128];
	sprintf(importDirectory, "Resources/textures/gui/*.*");

	std::vector<std::string> listFiles;
	listFiles = ListFilesInDirectory(importDirectory);
	int modButtonCounter = 0;
	int yCounter = 0;
	while (modButtonCounter < listFiles.size())
	{
		if (strcmp(listFiles[modButtonCounter].c_str(), ".") == 0 || strcmp(listFiles[modButtonCounter].c_str(), "..") == 0)
		{
			modButtonCounter++;
			continue;
		}

		buttonX = -(m_modWindowWidth - 42);

		for (int x = 0; x < 3 && modButtonCounter < listFiles.size(); ++x)
		{
			Button* pNewButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont35(), "");
			m_pFrontendManager->SetButtonIcons(pNewButton, ButtonSize::Size225x75);
			pNewButton->SetDimensions(buttonX, buttonY, buttonWidth, buttonHeight);
			pNewButton->SetPressedOffset(0, -1);

			// Add thumbnail icon
			char thumbnailIcon[128];
			sprintf(thumbnailIcon, "Resources/textures/gui/%s/thumbnail.tga", listFiles[modButtonCounter].c_str());
			pNewButton->AddIcon(m_pRenderer, thumbnailIcon, buttonWidth, buttonHeight, buttonWidth - 16, buttonHeight - 16, 8, 8, 2.5f);

			// Add header text
			pNewButton->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), listFiles[modButtonCounter].c_str(), Color(1.0f, 1.0f, 1.0f, 1.0f), 7, buttonHeight - 20, true, Color(0.0f, 0.0f, 0.0f, 1.0f));

			ModButtonData* pModButtonData = new ModButtonData();
			pModButtonData->m_pModMenu = this;
			pModButtonData->m_pModButton = pNewButton;
			pModButtonData->m_modName = listFiles[modButtonCounter];
			pModButtonData->m_toggled = false;
			pModButtonData->m_allowToggleOff = false;
			pModButtonData->m_allowMultipleSelection = false;
			pModButtonData->m_gameplayButton = false;
			pModButtonData->m_graphicsButton = false;
			pModButtonData->m_soundButton = false;
			pModButtonData->m_HUDButton = true;
			pModButtonData->m_miscButton = false;

			// Load the meta data file for the mod description and author
			std::string settingsIniFile = "Resources/textures/gui/" + listFiles[modButtonCounter] + "/description.ini";
			INIReader reader(settingsIniFile);

			if (reader.ParseError() >= 0)
			{
				pModButtonData->m_modDescription = reader.Get("MetaData", "Description", "[DECRIPTION]");
				pModButtonData->m_modAuthor = reader.Get("MetaData", "Author", "[AUTHOR]");
			}

			pNewButton->SetCallBackFunction(_ModButtonPressed);
			pNewButton->SetCallBackData(pModButtonData);

			pNewButton->SetEnterCallBackFunction(_ModButtonEntered);
			pNewButton->SetEnterCallBackData(pModButtonData);

			pNewButton->SetExitCallBackFunction(_ModButtonExited);
			pNewButton->SetExitCallBackData(pModButtonData);

			m_vpModButtonData.push_back(pModButtonData);

			m_pModsScrollbar->AddScrollAreaItem(pNewButton);

			m_vpHUDModButtons.push_back(pNewButton);

			buttonX += buttonAndSpacerWidth;

			modButtonCounter++;
		}

		yCounter++;

		buttonY -= buttonAndSpacerHeight;
	}
}

void ModMenu::RemoveHUDModButtons()
{
	m_pModsScrollbar->ClearScrollAreaItems();

	for (size_t i = 0; i < m_vpHUDModButtons.size(); ++i)
	{
		delete m_vpHUDModButtons[i];
		m_vpHUDModButtons[i] = nullptr;
	}

	m_vpHUDModButtons.clear();
}

void ModMenu::CreateMiscModButtons()
{
	int buttonWidth = m_modButtonWidth;
	int buttonHeight = m_modButtonheight;
	int buttonSpacer = m_modButtonSpace;
	int buttonAndSpacerWidth = buttonWidth + buttonSpacer;
	int buttonAndSpacerHeight = buttonHeight + buttonSpacer;

	int buttonX;
	int buttonY = m_modWindowHeight - buttonHeight - 17;

	char importDirectory[128];
	sprintf(importDirectory, "Resources/mods/misc/*.*");

	std::vector<std::string> listFiles;
	listFiles = ListFilesInDirectory(importDirectory);
	int modButtonCounter = 0;
	int yCounter = 0;
	while (modButtonCounter < listFiles.size())
	{
		if (strcmp(listFiles[modButtonCounter].c_str(), ".") == 0 || strcmp(listFiles[modButtonCounter].c_str(), "..") == 0)
		{
			modButtonCounter++;
			continue;
		}

		buttonX = -(m_modWindowWidth - 42);

		for (int x = 0; x < 3 && modButtonCounter < listFiles.size(); ++x)
		{
			Button* pNewButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont35(), "");
			m_pFrontendManager->SetButtonIcons(pNewButton, ButtonSize::Size225x75);
			pNewButton->SetDimensions(buttonX, buttonY, buttonWidth, buttonHeight);
			pNewButton->SetPressedOffset(0, -1);

			// Add thumbnail icon
			char thumbnailIcon[128];
			sprintf(thumbnailIcon, "Resources/mods/misc/%s/thumbnail.tga", listFiles[modButtonCounter].c_str());
			pNewButton->AddIcon(m_pRenderer, thumbnailIcon, buttonWidth, buttonHeight, buttonWidth - 16, buttonHeight - 16, 8, 8, 2.5f);

			// Add header text
			pNewButton->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), listFiles[modButtonCounter].c_str(), Color(1.0f, 1.0f, 1.0f, 1.0f), 7, buttonHeight - 20, true, Color(0.0f, 0.0f, 0.0f, 1.0f));

			ModButtonData* pModButtonData = new ModButtonData();
			pModButtonData->m_pModMenu = this;
			pModButtonData->m_pModButton = pNewButton;
			pModButtonData->m_modName = listFiles[modButtonCounter];
			pModButtonData->m_toggled = false;
			pModButtonData->m_allowToggleOff = true;
			pModButtonData->m_allowMultipleSelection = true;
			pModButtonData->m_gameplayButton = false;
			pModButtonData->m_graphicsButton = false;
			pModButtonData->m_soundButton = false;
			pModButtonData->m_HUDButton = false;
			pModButtonData->m_miscButton = true;

			// Load the meta data file for the mod description and author
			std::string settingsIniFile = "Resources/mods/misc/" + listFiles[modButtonCounter] + "/description.ini";
			INIReader reader(settingsIniFile);

			if (reader.ParseError() >= 0)
			{
				pModButtonData->m_modDescription = reader.Get("MetaData", "Description", "[DECRIPTION]");
				pModButtonData->m_modAuthor = reader.Get("MetaData", "Author", "[AUTHOR]");
			}

			pNewButton->SetCallBackFunction(_ModButtonPressed);
			pNewButton->SetCallBackData(pModButtonData);

			pNewButton->SetEnterCallBackFunction(_ModButtonEntered);
			pNewButton->SetEnterCallBackData(pModButtonData);

			pNewButton->SetExitCallBackFunction(_ModButtonExited);
			pNewButton->SetExitCallBackData(pModButtonData);

			m_vpModButtonData.push_back(pModButtonData);

			m_pModsScrollbar->AddScrollAreaItem(pNewButton);

			m_vpMiscModButtons.push_back(pNewButton);

			buttonX += buttonAndSpacerWidth;

			modButtonCounter++;
		}

		yCounter++;

		buttonY -= buttonAndSpacerHeight;
	}
}

void ModMenu::RemoveMiscModButtons()
{
	m_pModsScrollbar->ClearScrollAreaItems();

	for (size_t i = 0; i < m_vpMiscModButtons.size(); ++i)
	{
		delete m_vpMiscModButtons[i];
		m_vpMiscModButtons[i] = nullptr;
	}

	m_vpMiscModButtons.clear();
}

// Tooltips
void ModMenu::UpdateToolTipAppear(float dt)
{
	if (m_toolTipVisible)
	{
		if (m_tooltipAppearDelayTimer <= 0.0f)
		{
			if (m_toolTipComponentsAdded == false)
			{
				switch (m_tooltipQuality)
				{
				case ItemQuality::Common: { m_pModWindow->AddComponent(m_pTooltipBackgroundCommon); break; }
				case ItemQuality::Uncommon: { m_pModWindow->AddComponent(m_pTooltipBackgroundUncommon); break; }
				case ItemQuality::Magical: { m_pModWindow->AddComponent(m_pTooltipBackgroundMagical); break; }
				case ItemQuality::Rare: { m_pModWindow->AddComponent(m_pTooltipBackgroundRare); break; }
				case ItemQuality::Epic: { m_pModWindow->AddComponent(m_pTooltipBackgroundEpic); break; }
				default: throw std::logic_error("Invalid ItemQuality in UpdateToolTipAppear()");
				}

				m_pModWindow->AddComponent(m_pTooltipNameLabel);
				m_pModWindow->AddComponent(m_pTooltipDescriptionLabel);
				m_pModWindow->AddComponent(m_pTooltipSlotLabel);
				//m_pModWindow->AddComponent(m_pTooltipQualityLabel);

				m_toolTipComponentsAdded = true;
			}
		}
		else
		{
			m_tooltipAppearDelayTimer -= dt;
		}
	}
}

void ModMenu::ShowTooltip(ModButtonData* pModButtonData)
{
	if (m_toolTipVisible == true)
	{
		return;
	}

	// Set the focused window when we show a tooltip
	m_pModWindow->SetFocusWindow();

	// Replace the tooltip name
	m_pTooltipNameLabel->SetText(pModButtonData->m_modName);

	// Replace the tooltip description
	m_pTooltipDescriptionLabel->SetText(pModButtonData->m_modDescription);

	// Replace the tooltip equip slot text
	char slotText[256];
	sprintf(slotText, "Author: %s", pModButtonData->m_modAuthor.c_str());
	m_pTooltipSlotLabel->SetText(slotText);

	Color qualityColor = Color(0.95f, 1.0f, 0.2f, 1.0f);
	m_pTooltipNameLabel->SetColor(qualityColor);

	// Set tooltip dimensions
	m_tooltipWidth = 200;
	m_tooltipHeight = 220;
	m_tooltipDescBorder = 15;

	int x;
	int y;
	Dimensions dimensions = pModButtonData->m_pModButton->GetDimensions();
	x = m_modWindowWidth + dimensions.x - 180;
	y = dimensions.y - 180;

	m_pTooltipBackgroundCommon->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);
	m_pTooltipBackgroundUncommon->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);
	m_pTooltipBackgroundMagical->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);
	m_pTooltipBackgroundRare->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);
	m_pTooltipBackgroundEpic->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);

	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont30(), "%s", m_pTooltipNameLabel->GetText().c_str());
	m_pTooltipNameLabel->SetLocation(x + static_cast<int>(m_tooltipWidth * 0.5f) - static_cast<int>(textWidth * 0.5f), y + m_tooltipHeight - 35);

	m_pTooltipDescriptionLabel->SetDimensions(x + m_tooltipDescBorder, y + m_tooltipDescBorder, m_tooltipWidth - (m_tooltipDescBorder * 2), m_tooltipHeight - (m_tooltipDescBorder * 2) - 35);

	m_pTooltipSlotLabel->SetLocation(x + m_tooltipDescBorder, y + m_tooltipDescBorder);

	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont20(), "%s", m_pTooltipQualityLabel->GetText().c_str());
	m_pTooltipQualityLabel->SetLocation(x + m_tooltipWidth - m_tooltipDescBorder - textWidth, y + m_tooltipDescBorder);

	m_tooltipAppearDelayTimer = m_pFrontendManager->GetToolTipAppearDelay();

	m_toolTipVisible = true;
	m_toolTipComponentsAdded = false;
}

void ModMenu::HideTooltip()
{
	m_pModWindow->RemoveComponent(m_pTooltipBackgroundCommon);
	m_pModWindow->RemoveComponent(m_pTooltipBackgroundUncommon);
	m_pModWindow->RemoveComponent(m_pTooltipBackgroundMagical);
	m_pModWindow->RemoveComponent(m_pTooltipBackgroundRare);
	m_pModWindow->RemoveComponent(m_pTooltipBackgroundEpic);
	m_pModWindow->RemoveComponent(m_pTooltipNameLabel);
	m_pModWindow->RemoveComponent(m_pTooltipDescriptionLabel);
	m_pModWindow->RemoveComponent(m_pTooltipSlotLabel);
	//m_pModWindow->RemoveComponent(m_pTooltipQualityLabel);

	m_toolTipVisible = false;
}

// Update
void ModMenu::Update(float dt)
{
	FrontendPage::Update(dt);

	UpdateToolTipAppear(dt);

	if (CubbyGame::GetInstance()->IsPaused() == false)
	{
		float orbitTimer = m_pFrontendManager->GetCameraOrbitTimer();
		orbitTimer += dt;

		float orbitTime = 120.0f;
		if (orbitTimer >= orbitTime)
		{
			orbitTimer = 0.0f;
		}
		m_pFrontendManager->SetCameraOrbitTimer(orbitTimer);

		float ratio = orbitTimer / orbitTime;
		float radius = 125.0f;
		float angle = DegreeToRadian((ratio * 360.0f));
		glm::vec3 position = glm::vec3(cos(angle) * radius, radius * 0.75f, sin(angle) * radius);

		m_cameraPosition = position;
		m_cameraView = glm::vec3(0.0f, 8.0f, 0.0f);
	}

	CubbyGame::GetInstance()->SetGlobalBlurAmount(0.00125f);

	int buttonHeight = m_modButtonheight;
	int buttonSpacer = m_modButtonSpace;
	int buttonAndSpacer = buttonHeight + buttonSpacer;

	// Gameplay
	if (m_pGameplayMode->GetToggled())
	{
		int visibleSize = m_pModsScrollbar->GetScrollArea().height;
		int numYLines = static_cast<int>(ceil(static_cast<float>(m_vpGameplayModButtons.size()) / 3.0f));
		int neededHeight = numYLines * buttonAndSpacer + 4;
		int heightDiff = neededHeight - visibleSize;
		if (heightDiff > 0)
		{
			m_pModsScrollbar->SetScrollSize(static_cast<float>(visibleSize) / static_cast<float>(neededHeight));
			m_pModsScrollbar->SetDisabled(false);

			float scrollRatio = m_pModsScrollbar->GetScrollRatio();
			int offsetButtonY = static_cast<int>(heightDiff * scrollRatio);

			int modButtonCounter = 0;
			int yCounter = 0;
			while (modButtonCounter < m_vpGameplayModButtons.size())
			{
				for (int x = 0; x < 3 && modButtonCounter < m_vpGameplayModButtons.size(); ++x)
				{
					int yPos = m_modWindowHeight - buttonHeight - 17 - (buttonAndSpacer * yCounter);
					Point position = m_vpGameplayModButtons[modButtonCounter]->GetLocation();
					m_vpGameplayModButtons[modButtonCounter]->SetLocation(position.x, yPos + offsetButtonY);

					modButtonCounter++;
				}

				yCounter++;
			}
		}
		else
		{
			m_pModsScrollbar->SetScrollSize(0.0f);
			m_pModsScrollbar->SetDisabled(true);
		}
	}

	// Graphics
	if (m_pGraphicsMode->GetToggled())
	{
		int visibleSize = m_pModsScrollbar->GetScrollArea().height;
		int numYLines = static_cast<int>(ceil(static_cast<float>(m_vpGraphicsModButtons.size()) / 3.0f));
		int neededHeight = numYLines * buttonAndSpacer + 4;
		int heightDiff = neededHeight - visibleSize;
		if (heightDiff > 0)
		{
			m_pModsScrollbar->SetScrollSize(static_cast<float>(visibleSize) / static_cast<float>(neededHeight));
			m_pModsScrollbar->SetDisabled(false);

			float scrollRatio = m_pModsScrollbar->GetScrollRatio();
			int offsetButtonY = static_cast<int>(heightDiff * scrollRatio);

			int modButtonCounter = 0;
			int yCounter = 0;
			while (modButtonCounter < m_vpGraphicsModButtons.size())
			{
				for (int x = 0; x < 3 && modButtonCounter < m_vpGraphicsModButtons.size(); ++x)
				{
					int yPos = m_modWindowHeight - buttonHeight - 17 - (buttonAndSpacer * yCounter);
					Point position = m_vpGraphicsModButtons[modButtonCounter]->GetLocation();
					m_vpGraphicsModButtons[modButtonCounter]->SetLocation(position.x, yPos + offsetButtonY);

					modButtonCounter++;
				}

				yCounter++;
			}
		}
		else
		{
			m_pModsScrollbar->SetScrollSize(0.0f);
			m_pModsScrollbar->SetDisabled(true);
		}
	}

	// Sound
	if (m_pSoundMode->GetToggled())
	{
		int visibleSize = m_pModsScrollbar->GetScrollArea().height;
		int numYLines = static_cast<int>(ceil(static_cast<float>(m_vpSoundModButtons.size()) / 3.0f));
		int neededHeight = numYLines * buttonAndSpacer + 4;
		int heightDiff = neededHeight - visibleSize;
		if (heightDiff > 0)
		{
			m_pModsScrollbar->SetScrollSize(static_cast<float>(visibleSize) / static_cast<float>(neededHeight));
			m_pModsScrollbar->SetDisabled(false);

			float scrollRatio = m_pModsScrollbar->GetScrollRatio();
			int offsetButtonY = static_cast<int>(heightDiff * scrollRatio);

			int modButtonCounter = 0;
			int yCounter = 0;
			while (modButtonCounter < m_vpSoundModButtons.size())
			{
				for (int x = 0; x < 3 && modButtonCounter < m_vpSoundModButtons.size(); ++x)
				{
					int yPos = m_modWindowHeight - buttonHeight - 17 - (buttonAndSpacer * yCounter);
					Point position = m_vpSoundModButtons[modButtonCounter]->GetLocation();
					m_vpSoundModButtons[modButtonCounter]->SetLocation(position.x, yPos + offsetButtonY);

					modButtonCounter++;
				}

				yCounter++;
			}
		}
		else
		{
			m_pModsScrollbar->SetScrollSize(0.0f);
			m_pModsScrollbar->SetDisabled(true);
		}
	}

	// HUD
	if (m_pHUDMode->GetToggled())
	{
		int visibleSize = m_pModsScrollbar->GetScrollArea().height;
		int numYLines = static_cast<int>(ceil(static_cast<float>(m_vpHUDModButtons.size()) / 3.0f));
		int neededHeight = numYLines * buttonAndSpacer + 4;
		int heightDiff = neededHeight - visibleSize;
		if (heightDiff > 0)
		{
			m_pModsScrollbar->SetScrollSize(static_cast<float>(visibleSize) / static_cast<float>(neededHeight));
			m_pModsScrollbar->SetDisabled(false);

			float scrollRatio = m_pModsScrollbar->GetScrollRatio();
			int offsetButtonY = static_cast<int>(heightDiff * scrollRatio);

			int modButtonCounter = 0;
			int yCounter = 0;
			while (modButtonCounter < m_vpHUDModButtons.size())
			{
				for (int x = 0; x < 3 && modButtonCounter < m_vpHUDModButtons.size(); ++x)
				{
					int yPos = m_modWindowHeight - buttonHeight - 17 - (buttonAndSpacer * yCounter);
					Point position = m_vpHUDModButtons[modButtonCounter]->GetLocation();
					m_vpHUDModButtons[modButtonCounter]->SetLocation(position.x, yPos + offsetButtonY);

					modButtonCounter++;
				}

				yCounter++;
			}
		}
		else
		{
			m_pModsScrollbar->SetScrollSize(0.0f);
			m_pModsScrollbar->SetDisabled(true);
		}
	}

	// Misc
	if (m_pMiscMode->GetToggled())
	{
		int visibleSize = m_pModsScrollbar->GetScrollArea().height;
		int numYLines = static_cast<int>(ceil(static_cast<float>(m_vpMiscModButtons.size()) / 3.0f));
		int neededHeight = numYLines * buttonAndSpacer + 4;
		int heightDiff = neededHeight - visibleSize;
		if (heightDiff > 0)
		{
			m_pModsScrollbar->SetScrollSize(static_cast<float>(visibleSize) / static_cast<float>(neededHeight));
			m_pModsScrollbar->SetDisabled(false);

			float scrollRatio = m_pModsScrollbar->GetScrollRatio();
			int offsetButtonY = static_cast<int>(heightDiff * scrollRatio);

			int modButtonCounter = 0;
			int yCounter = 0;
			while (modButtonCounter < m_vpMiscModButtons.size())
			{
				for (int x = 0; x < 3 && modButtonCounter < m_vpMiscModButtons.size(); ++x)
				{
					int yPos = m_modWindowHeight - buttonHeight - 17 - (buttonAndSpacer * yCounter);
					Point position = m_vpMiscModButtons[modButtonCounter]->GetLocation();
					m_vpMiscModButtons[modButtonCounter]->SetLocation(position.x, yPos + offsetButtonY);

					modButtonCounter++;
				}

				yCounter++;
			}
		}
		else
		{
			m_pModsScrollbar->SetScrollSize(0.0f);
			m_pModsScrollbar->SetDisabled(true);
		}
	}
}

// Render
void ModMenu::Render()
{
	FrontendPage::Render();
}

void ModMenu::Render2D()
{
	FrontendPage::Render2D();
}

// Static functionality
void ModMenu::_CloseExitPressed(void* pData)
{
	ModMenu* pModMenu = static_cast<ModMenu*>(pData);
	pModMenu->CloseExitPressed();
}

void ModMenu::CloseExitPressed() const
{
	if (m_returnToMainMenu)
	{
		m_pFrontendManager->SetFrontendScreen(FrontendScreen::MainMenu);
	}
	else
	{
		m_pFrontendManager->SetFrontendScreen(FrontendScreen::PauseMenu);
	}
}

void ModMenu::_GameplayTabPressed(void* pData)
{
	ModMenu* pModMenu = static_cast<ModMenu*>(pData);
	pModMenu->GameplayTabPressed();
}

void ModMenu::GameplayTabPressed()
{
	if (m_loaded)
	{
		m_pGUI->RemoveWindow(m_pModWindow);
	}

	ClearModButtonData();

	RemoveGameplayModButtons();
	RemoveGraphicsModButtons();
	RemoveSoundModButtons();
	RemoveHUDModButtons();
	RemoveMiscModButtons();

	CreateGameplayModButtons();

	SelectLoadedMods();

	Update(0.0f);
	m_pModsScrollbar->SetScrollPosition(1.0f);

	m_pGUI->AddWindow(m_pModWindow);
	m_pModWindow->Show();
}

void ModMenu::_GraphicsTabPressed(void* pData)
{
	ModMenu* pModMenu = static_cast<ModMenu*>(pData);
	pModMenu->GraphicsTabPressed();
}

void ModMenu::GraphicsTabPressed()
{
	if (m_loaded)
	{
		m_pGUI->RemoveWindow(m_pModWindow);
	}

	ClearModButtonData();

	RemoveGameplayModButtons();
	RemoveGraphicsModButtons();
	RemoveSoundModButtons();
	RemoveHUDModButtons();
	RemoveMiscModButtons();

	CreateGraphicsModButtons();

	SelectLoadedMods();

	Update(0.0f);
	m_pModsScrollbar->SetScrollPosition(1.0f);

	m_pGUI->AddWindow(m_pModWindow);
	m_pModWindow->Show();
}

void ModMenu::_SoundTabPressed(void* pData)
{
	ModMenu* pModMenu = static_cast<ModMenu*>(pData);
	pModMenu->SoundTabPressed();
}

void ModMenu::SoundTabPressed()
{
	if (m_loaded)
	{
		m_pGUI->RemoveWindow(m_pModWindow);
	}

	ClearModButtonData();

	RemoveGameplayModButtons();
	RemoveGraphicsModButtons();
	RemoveSoundModButtons();
	RemoveHUDModButtons();
	RemoveMiscModButtons();

	CreateSoundModButtons();

	SelectLoadedMods();

	Update(0.0f);
	m_pModsScrollbar->SetScrollPosition(1.0f);

	m_pGUI->AddWindow(m_pModWindow);
	m_pModWindow->Show();
}

void ModMenu::_GUITabPressed(void* pData)
{
	ModMenu* pModMenu = static_cast<ModMenu*>(pData);
	pModMenu->GUITabPressed();
}

void ModMenu::GUITabPressed()
{
	if (m_loaded)
	{
		m_pGUI->RemoveWindow(m_pModWindow);
	}

	ClearModButtonData();

	RemoveGameplayModButtons();
	RemoveGraphicsModButtons();
	RemoveSoundModButtons();
	RemoveHUDModButtons();
	RemoveMiscModButtons();

	CreateHUDModButtons();

	SelectLoadedMods();

	Update(0.0f);
	m_pModsScrollbar->SetScrollPosition(1.0f);

	m_pGUI->AddWindow(m_pModWindow);
	m_pModWindow->Show();
}

void ModMenu::_MiscTabPressed(void* pData)
{
	ModMenu* pModMenu = static_cast<ModMenu*>(pData);
	pModMenu->MiscTabPressed();
}

void ModMenu::MiscTabPressed()
{
	if (m_loaded)
	{
		m_pGUI->RemoveWindow(m_pModWindow);
	}

	ClearModButtonData();

	RemoveGameplayModButtons();
	RemoveGraphicsModButtons();
	RemoveSoundModButtons();
	RemoveHUDModButtons();
	RemoveMiscModButtons();

	CreateMiscModButtons();

	SelectLoadedMods();

	Update(0.0f);
	m_pModsScrollbar->SetScrollPosition(1.0f);

	m_pGUI->AddWindow(m_pModWindow);
	m_pModWindow->Show();
}

void ModMenu::_ModButtonPressed(void* pData)
{
	ModButtonData* pModButtonData = static_cast<ModButtonData*>(pData);
	pModButtonData->m_pModMenu->ModButtonPressed(pModButtonData);
}

void ModMenu::ModButtonPressed(ModButtonData* pModButtonData)
{
	int buttonWidth = m_modButtonWidth;

	// Decide on the state of the toggle flag for each existing mod button and also the one we pressed
	bool addTick = false;
	for (size_t i = 0; i < m_vpModButtonData.size(); ++i)
	{
		if (pModButtonData == m_vpModButtonData[i])
		{
			if (m_vpModButtonData[i]->m_toggled)
			{
				if (m_vpModButtonData[i]->m_allowToggleOff)
				{
					m_vpModButtonData[i]->m_toggled = false;
					std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
					std::string tickIcon = "Resources/textures/gui/" + themeName + "/common/tick.tga";
					m_vpModButtonData[i]->m_pModButton->RemoveIcon(tickIcon.c_str());
					CubbyGame::GetInstance()->GetModsManager()->RemoveMod(m_vpModButtonData[i]->m_modName, m_vpModButtonData[i]->m_gameplayButton, m_vpModButtonData[i]->m_graphicsButton, m_vpModButtonData[i]->m_soundButton, m_vpModButtonData[i]->m_HUDButton, m_vpModButtonData[i]->m_miscButton);
				}
			}
			else
			{
				addTick = true;
				pModButtonData->m_toggled = true;
				CubbyGame::GetInstance()->GetModsManager()->AddMod(m_vpModButtonData[i]->m_modName, m_vpModButtonData[i]->m_gameplayButton, m_vpModButtonData[i]->m_graphicsButton, m_vpModButtonData[i]->m_soundButton, m_vpModButtonData[i]->m_HUDButton, m_vpModButtonData[i]->m_miscButton);
			}
		}
		else
		{
			if (pModButtonData->m_allowMultipleSelection == false || m_vpModButtonData[i]->m_allowMultipleSelection == false)
			{
				m_vpModButtonData[i]->m_toggled = false;
				std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
				std::string tickIcon = "Resources/textures/gui/" + themeName + "/common/tick.tga";
				m_vpModButtonData[i]->m_pModButton->RemoveIcon(tickIcon.c_str());
				CubbyGame::GetInstance()->GetModsManager()->RemoveMod(m_vpModButtonData[i]->m_modName, m_vpModButtonData[i]->m_gameplayButton, m_vpModButtonData[i]->m_graphicsButton, m_vpModButtonData[i]->m_soundButton, m_vpModButtonData[i]->m_HUDButton, m_vpModButtonData[i]->m_miscButton);
			}
		}
	}

	// Gameplay

	// Graphics

	// Sound

	// HUD
	if (pModButtonData->m_HUDButton)
	{
		m_pFrontendManager->LoadCommonGraphics(pModButtonData->m_modName);
		CubbyGame::GetInstance()->SkinGUI();
	}

	// Misc

	// If we need to add the tick
	if (addTick)
	{
		std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
		std::string tickIcon = "Resources/textures/gui/" + themeName + "/common/tick.tga";
		pModButtonData->m_pModButton->AddIcon(m_pRenderer, tickIcon.c_str(), 32, 32, 32, 32, buttonWidth - 38, 4, 3.25f);
	}
}

void ModMenu::_ModButtonEntered(void* pData)
{
	ModButtonData* pModButtonData = static_cast<ModButtonData*>(pData);
	pModButtonData->m_pModMenu->ModButtonEntered(pModButtonData);
}

void ModMenu::ModButtonEntered(ModButtonData* pModButtonData)
{
	ShowTooltip(pModButtonData);
}

void ModMenu::_ModButtonExited(void* pData)
{
	ModButtonData* pModButtonData = static_cast<ModButtonData*>(pData);
	pModButtonData->m_pModMenu->ModButtonExited(pModButtonData);
}

void ModMenu::ModButtonExited(ModButtonData* pModButtonData)
{
	HideTooltip();
}