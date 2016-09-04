/*************************************************************************
> File Name: CharacterGUI.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Character GUI class.
> Created Time: 2016/08/29
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include <CubbyGame.h>

#include <Utils/Interpolator.h>

#include "CharacterGUI.h"

// Constructor, Destructor
CharacterGUI::CharacterGUI(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight)
{
	m_pRenderer = pRenderer;
	m_pGUI = pGUI;
	m_pFrontendManager = pFrontendManager;
	m_pChunkManager = pChunkManager;
	m_pPlayer = pPlayer;
	m_pInventoryManager = pInventoryManager;

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	// Character Window
	m_pCharacterWindow = new GUIWindow(m_pRenderer, m_pFrontendManager->GetFrontendFontMedium(), "Character");
	m_pCharacterWindow->AllowMoving(true);
	m_pCharacterWindow->AllowClosing(false);
	m_pCharacterWindow->AllowMinimizing(false);
	m_pCharacterWindow->AllowScrolling(false);
	m_pCharacterWindow->SetRenderTitleBar(true);
	m_pCharacterWindow->SetRenderWindowBackground(true);
	m_pCharacterWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pCharacterWindow->Hide();

	m_pDynamicPaperDollIcon = new Icon(m_pRenderer);
	m_pDynamicPaperDollIcon->SetDepth(2.5f);

	m_pTitleBarIcon = new Icon(m_pRenderer, "", 44, 44);
	m_pTitleBarIcon->SetDepth(4.0f);

	m_pCharacterWindowBackgroundIcon = new Icon(m_pRenderer, "", 304, 281);
	m_pCharacterWindowBackgroundIcon->SetDepth(1.0f);

	m_pTitleBarBackgroundIcon = new Icon(m_pRenderer, "", 133, 35);
	m_pTitleBarBackgroundIcon->SetDepth(1.0f);

	m_pCloseExitButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCloseExitButton->SetLabelOffset(0, 5);
	m_pCloseExitButton->SetCallBackFunction(_CloseExitPressed);
	m_pCloseExitButton->SetCallBackData(this);

	m_pTabOpenCloseButtonIconLeft = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconLeft->SetDepth(0.5f);

	m_pTabOpenCloseButtonIconLeftHover = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconLeftHover->SetDepth(0.5f);

	m_pTabOpenCloseButtonIconLeftPressed = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconLeftPressed->SetDepth(0.5f);

	m_pTabOpenCloseButtonIconRight = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconRight->SetDepth(0.5f);

	m_pTabOpenCloseButtonIconRightHover = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconRightHover->SetDepth(0.5f);

	m_pTabOpenCloseButtonIconRightPressed = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconRightPressed->SetDepth(0.5f);

	m_pStatsTabIconLeft = new Icon(m_pRenderer, "", 164, 198);
	m_pStatsTabIconLeft->SetDepth(1.0f);

	m_pStatsTabIconRight = new Icon(m_pRenderer, "", 164, 198);
	m_pStatsTabIconRight->SetDepth(0.5f);

	m_pCharacterTitleLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFontMedium(), "[NAME]", Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pCharacterTitleLabel->SetDepth(2.0f);

	m_pStatsHeaderLabel1 = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), "Strength", Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsHeaderLabel1->SetOutline(true);
	m_pStatsHeaderLabel1->SetOutlineColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsHeaderLabel1->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsHeaderLabel1->SetDepth(2.0f);
	m_pStatsHeaderLabel2 = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), "Dexterity", Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsHeaderLabel2->SetOutline(true);
	m_pStatsHeaderLabel2->SetOutlineColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsHeaderLabel2->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsHeaderLabel2->SetDepth(2.0f);
	m_pStatsHeaderLabel3 = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), "Intelligence", Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsHeaderLabel3->SetOutline(true);
	m_pStatsHeaderLabel3->SetOutlineColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsHeaderLabel3->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsHeaderLabel3->SetDepth(2.0f);
	m_pStatsHeaderLabel4 = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), "Vitality", Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsHeaderLabel4->SetOutline(true);
	m_pStatsHeaderLabel4->SetOutlineColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsHeaderLabel4->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsHeaderLabel4->SetDepth(2.0f);
	m_pStatsHeaderLabel5 = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), "Armor", Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsHeaderLabel5->SetOutline(true);
	m_pStatsHeaderLabel5->SetOutlineColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsHeaderLabel5->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsHeaderLabel5->SetDepth(2.0f);
	m_pStatsHeaderLabel6 = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), "Luck", Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsHeaderLabel6->SetOutline(true);
	m_pStatsHeaderLabel6->SetOutlineColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsHeaderLabel6->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsHeaderLabel6->SetDepth(2.0f);

	m_pStatsValueLabel1 = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), "1");
	m_pStatsValueLabel1->SetOutline(true);
	m_pStatsValueLabel1->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsValueLabel1->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsValueLabel1->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsValueLabel1->SetDepth(5.5f);
	m_pStatsValueLabel1->SetWordWrap(true);
	m_pStatsValueLabel1->SetHorizontalAlignment(HorizontalAlignment::Center);
	m_pStatsValueLabel2 = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), "2");
	m_pStatsValueLabel2->SetOutline(true);
	m_pStatsValueLabel2->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsValueLabel2->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsValueLabel2->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsValueLabel2->SetDepth(5.5f);
	m_pStatsValueLabel2->SetWordWrap(true);
	m_pStatsValueLabel2->SetHorizontalAlignment(HorizontalAlignment::Center);
	m_pStatsValueLabel3 = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), "3");
	m_pStatsValueLabel3->SetOutline(true);
	m_pStatsValueLabel3->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsValueLabel3->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsValueLabel3->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsValueLabel3->SetDepth(5.5f);
	m_pStatsValueLabel3->SetWordWrap(true);
	m_pStatsValueLabel3->SetHorizontalAlignment(HorizontalAlignment::Center);
	m_pStatsValueLabel4 = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), "4");
	m_pStatsValueLabel4->SetOutline(true);
	m_pStatsValueLabel4->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsValueLabel4->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsValueLabel4->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsValueLabel4->SetDepth(5.5f);
	m_pStatsValueLabel4->SetWordWrap(true);
	m_pStatsValueLabel4->SetHorizontalAlignment(HorizontalAlignment::Center);
	m_pStatsValueLabel5 = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), "5");
	m_pStatsValueLabel5->SetOutline(true);
	m_pStatsValueLabel5->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsValueLabel5->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsValueLabel5->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsValueLabel5->SetDepth(5.5f);
	m_pStatsValueLabel5->SetWordWrap(true);
	m_pStatsValueLabel5->SetHorizontalAlignment(HorizontalAlignment::Center);
	m_pStatsValueLabel6 = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), "6");
	m_pStatsValueLabel6->SetOutline(true);
	m_pStatsValueLabel6->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pStatsValueLabel6->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStatsValueLabel6->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pStatsValueLabel6->SetDepth(5.5f);
	m_pStatsValueLabel6->SetWordWrap(true);
	m_pStatsValueLabel6->SetHorizontalAlignment(HorizontalAlignment::Center);

	m_pStat1IncreaseButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStat1IncreaseButton->SetCallBackFunction(_Stat1IncreasePressed);
	m_pStat1IncreaseButton->SetCallBackData(this);
	//m_pStat1IncreaseButton->SetDefaultIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat1IncreaseButton->SetHoverIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat1IncreaseButton->SetSelectedIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat1IncreaseButton->SetDisabledIcon(m_pStatsIncreaseButtonIcon);
	m_pStat1IncreaseButton->SetDepth(1.1f);
	m_pStat2IncreaseButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStat2IncreaseButton->SetCallBackFunction(_Stat2IncreasePressed);
	m_pStat2IncreaseButton->SetCallBackData(this);
	//m_pStat2IncreaseButton->SetDefaultIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat2IncreaseButton->SetHoverIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat2IncreaseButton->SetSelectedIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat2IncreaseButton->SetDisabledIcon(m_pStatsIncreaseButtonIcon);
	m_pStat2IncreaseButton->SetDepth(1.2f);
	m_pStat3IncreaseButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStat3IncreaseButton->SetCallBackFunction(_Stat3IncreasePressed);
	m_pStat3IncreaseButton->SetCallBackData(this);
	//m_pStat3IncreaseButton->SetDefaultIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat3IncreaseButton->SetHoverIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat3IncreaseButton->SetSelectedIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat3IncreaseButton->SetDisabledIcon(m_pStatsIncreaseButtonIcon);
	m_pStat3IncreaseButton->SetDepth(1.3f);
	m_pStat4IncreaseButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStat4IncreaseButton->SetCallBackFunction(_Stat4IncreasePressed);
	m_pStat4IncreaseButton->SetCallBackData(this);
	//m_pStat4IncreaseButton->SetDefaultIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat4IncreaseButton->SetHoverIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat4IncreaseButton->SetSelectedIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat4IncreaseButton->SetDisabledIcon(m_pStatsIncreaseButtonIcon);
	m_pStat4IncreaseButton->SetDepth(1.4f);
	m_pStat5IncreaseButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStat5IncreaseButton->SetCallBackFunction(_Stat5IncreasePressed);
	m_pStat5IncreaseButton->SetCallBackData(this);
	//m_pStat5IncreaseButton->SetDefaultIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat5IncreaseButton->SetHoverIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat5IncreaseButton->SetSelectedIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat5IncreaseButton->SetDisabledIcon(m_pStatsIncreaseButtonIcon);
	m_pStat5IncreaseButton->SetDepth(1.5f);
	m_pStat6IncreaseButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pStat6IncreaseButton->SetCallBackFunction(_Stat6IncreasePressed);
	m_pStat6IncreaseButton->SetCallBackData(this);
	//m_pStat6IncreaseButton->SetDefaultIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat6IncreaseButton->SetHoverIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat6IncreaseButton->SetSelectedIcon(m_pStatsIncreaseButtonIcon);
	//m_pStat6IncreaseButton->SetDisabledIcon(m_pStatsIncreaseButtonIcon);
	m_pStat6IncreaseButton->SetDepth(1.6f);

	m_pTabOpenCloseButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTabOpenCloseButton->SetDefaultIcon(m_pTabOpenCloseButtonIconLeft);
	m_pTabOpenCloseButton->SetHoverIcon(m_pTabOpenCloseButtonIconLeftHover);
	m_pTabOpenCloseButton->SetSelectedIcon(m_pTabOpenCloseButtonIconLeftPressed);
	m_pTabOpenCloseButton->SetDisabledIcon(m_pTabOpenCloseButtonIconLeft);
	m_pTabOpenCloseButton->SetLabelOffset(0, 5);
	m_pTabOpenCloseButton->SetCallBackFunction(_OpenCloseTabPressed);
	m_pTabOpenCloseButton->SetCallBackData(this);

	m_pArrowLeftButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pArrowLeftButton->SetLabelOffset(0, 5);
	m_pArrowLeftButton->SetCallBackFunctionPressed(_ArrowLeftPressed);
	m_pArrowLeftButton->SetCallBackDataPressed(this);
	m_pArrowLeftButton->SetCallBackFunctionReleased(_ArrowLeftReleased);
	m_pArrowLeftButton->SetCallBackDataReleased(this);
	m_pArrowLeftButton->SetDepth(3.0f);

	m_pArrowRightButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pArrowRightButton->SetLabelOffset(0, 5);
	m_pArrowRightButton->SetCallBackFunctionPressed(_ArrowRightPressed);
	m_pArrowRightButton->SetCallBackDataPressed(this);
	m_pArrowRightButton->SetCallBackFunctionReleased(_ArrowRightReleased);
	m_pArrowRightButton->SetCallBackDataReleased(this);
	m_pArrowRightButton->SetDepth(3.0f);

	m_pEquipHoverIcon = new Icon(m_pRenderer, "", 80, 80);
	m_pEquipHoverIcon->SetDepth(2.0f);

	m_pCharacterWindow->SetBackgroundIcon(m_pCharacterWindowBackgroundIcon);
	m_pCharacterWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	m_pCharacterWindow->AddComponent(m_pDynamicPaperDollIcon);
	m_pCharacterWindow->AddComponent(m_pCloseExitButton);
	m_pCharacterWindow->AddComponent(m_pTitleBarIcon);
	m_pCharacterWindow->AddComponent(m_pArrowLeftButton);
	m_pCharacterWindow->AddComponent(m_pArrowRightButton);
	m_pCharacterWindow->AddComponent(m_pStatsTabIconLeft);
	m_pCharacterWindow->AddComponent(m_pStatsTabIconRight);
	m_pCharacterWindow->AddComponent(m_pTabOpenCloseButton);
	m_pCharacterWindow->AddComponent(m_pCharacterTitleLabel);
	m_pCharacterWindow->AddComponent(m_pStatsHeaderLabel1);
	m_pCharacterWindow->AddComponent(m_pStatsHeaderLabel2);
	m_pCharacterWindow->AddComponent(m_pStatsHeaderLabel3);
	m_pCharacterWindow->AddComponent(m_pStatsHeaderLabel4);
	m_pCharacterWindow->AddComponent(m_pStatsHeaderLabel5);
	m_pCharacterWindow->AddComponent(m_pStatsHeaderLabel6);
	m_pCharacterWindow->AddComponent(m_pStatsValueLabel1);
	m_pCharacterWindow->AddComponent(m_pStatsValueLabel2);
	m_pCharacterWindow->AddComponent(m_pStatsValueLabel3);
	m_pCharacterWindow->AddComponent(m_pStatsValueLabel4);
	m_pCharacterWindow->AddComponent(m_pStatsValueLabel5);
	m_pCharacterWindow->AddComponent(m_pStatsValueLabel6);

	m_pInventoryBackgroundSlotBorderCommon = new Icon(m_pRenderer, "", 64, 64);
	m_pInventoryBackgroundSlotBorderCommon->SetDepth(2.0f);

	m_pInventoryBackgroundSlotBorderUncommon = new Icon(m_pRenderer, "", 64, 64);
	m_pInventoryBackgroundSlotBorderUncommon->SetDepth(2.0f);

	m_pInventoryBackgroundSlotBorderMagical = new Icon(m_pRenderer, "", 64, 64);
	m_pInventoryBackgroundSlotBorderMagical->SetDepth(2.0f);

	m_pInventoryBackgroundSlotBorderRare = new Icon(m_pRenderer, "", 64, 64);
	m_pInventoryBackgroundSlotBorderRare->SetDepth(2.0f);

	m_pInventoryBackgroundSlotBorderEpic = new Icon(m_pRenderer, "", 64, 64);
	m_pInventoryBackgroundSlotBorderEpic->SetDepth(2.0f);

	// Tooltip
	m_pTooltipBackgroundCommon = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundCommon->SetDepth(5.5f);

	m_pTooltipBackgroundUncommon = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundUncommon->SetDepth(5.5f);

	m_pTooltipBackgroundMagical = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundMagical->SetDepth(5.5f);

	m_pTooltipBackgroundRare = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundRare->SetDepth(5.5f);

	m_pTooltipBackgroundEpic = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundEpic->SetDepth(5.5f);

	char nameText[] = "[ITEM]";
	m_pTooltipNameLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), nameText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pTooltipNameLabel->SetOutline(true);
	m_pTooltipNameLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipNameLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont30Outline());
	m_pTooltipNameLabel->SetDepth(5.5f);

	char descText[] = "[REPLACE ME]";
	m_pTooltipDescriptionLabel = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), descText);
	m_pTooltipDescriptionLabel->SetOutline(true);
	m_pTooltipDescriptionLabel->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pTooltipDescriptionLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipDescriptionLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pTooltipDescriptionLabel->SetDepth(5.5f);
	m_pTooltipDescriptionLabel->SetWordWrap(true);

	char slotText[] = "[SLOT]";
	m_pTooltipSlotLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), slotText, Color(0.5f, 0.5f, 0.5f, 1.0f));
	m_pTooltipSlotLabel->SetOutline(true);
	m_pTooltipSlotLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipSlotLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont20Outline());
	m_pTooltipSlotLabel->SetDepth(5.5f);

	char qualityText[] = "[QUALITY]";
	m_pTooltipQualityLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), qualityText, Color(0.5f, 0.5f, 0.5f, 1.0f));
	m_pTooltipQualityLabel->SetOutline(true);
	m_pTooltipQualityLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipQualityLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont20Outline());
	m_pTooltipQualityLabel->SetDepth(5.5f);

	SetWindowDimensions(m_windowWidth, m_windowHeight);

	for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
	{
		m_pEquippedItems[i] = nullptr;
	}

	m_pressedX = 0;
	m_pressedY = 0;
	m_pPressedCharacterItem = nullptr;

	m_leftArrowPressed = false;
	m_rightArrowPressed = false;

	pBlockSlotItem = nullptr;

	m_statsTabsOpen = false;
	m_statsTabLeftSide = true;

	m_addedStatIncreaseButtons = false;

	// Load delay
	m_loadDelay = false;
	m_loadDelayTime = 0.0f;

	m_loaded = false;
}

CharacterGUI::~CharacterGUI()
{
	delete m_pCharacterWindow;
	delete m_pDynamicPaperDollIcon;
	delete m_pTitleBarIcon;
	delete m_pTitleBarBackgroundIcon;
	delete m_pCharacterWindowBackgroundIcon;
	delete m_pCloseExitButton;
	delete m_pArrowLeftButton;
	delete m_pArrowRightButton;
	delete m_pTabOpenCloseButtonIconRight;
	delete m_pTabOpenCloseButtonIconRightHover;
	delete m_pTabOpenCloseButtonIconRightPressed;
	delete m_pTabOpenCloseButtonIconLeft;
	delete m_pTabOpenCloseButtonIconLeftHover;
	delete m_pTabOpenCloseButtonIconLeftPressed;
	delete m_pTabOpenCloseButton;
	delete m_pStatsTabIconLeft;
	delete m_pStatsTabIconRight;
	delete m_pCharacterTitleLabel;
	delete m_pStatsHeaderLabel1;
	delete m_pStatsHeaderLabel2;
	delete m_pStatsHeaderLabel3;
	delete m_pStatsHeaderLabel4;
	delete m_pStatsHeaderLabel5;
	delete m_pStatsHeaderLabel6;
	delete m_pStatsValueLabel1;
	delete m_pStatsValueLabel2;
	delete m_pStatsValueLabel3;
	delete m_pStatsValueLabel4;
	delete m_pStatsValueLabel5;
	delete m_pStatsValueLabel6;
	delete m_pStat1IncreaseButton;
	delete m_pStat2IncreaseButton;
	delete m_pStat3IncreaseButton;
	delete m_pStat4IncreaseButton;
	delete m_pStat5IncreaseButton;
	delete m_pStat6IncreaseButton;

	delete m_pEquipHoverIcon;

	delete m_pInventoryBackgroundSlotBorderCommon;
	delete m_pInventoryBackgroundSlotBorderUncommon;
	delete m_pInventoryBackgroundSlotBorderMagical;
	delete m_pInventoryBackgroundSlotBorderRare;
	delete m_pInventoryBackgroundSlotBorderEpic;

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

void CharacterGUI::SetInventoryGUI(InventoryGUI* pInventoryGUI)
{
	m_pInventoryGUI = pInventoryGUI;
}

void CharacterGUI::SetLootGUI(LootGUI* pLootGUI)
{
	m_pLootGUI = pLootGUI;
}

void CharacterGUI::SetActionBar(ActionBar* pActionBar)
{
	m_pActionBar = pActionBar;
}

void CharacterGUI::SetWindowDimensions(int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	m_characterWindowWidth = 304;
	m_characterWindowHeight = 281;
	m_titlebarWidth = 153;
	m_titlebarHeight = 35;
	m_statsTabOpenButtonWidth = 12;
	m_statsTabOpenButtonHeight = 80;
	m_statsTabWidth = 164;
	m_statsTabHeight = 198;
	m_paperDollX = 72;
	m_paperDollY = 86;
	m_paperDollWidth = 160;
	m_paperDollHeight = 175;
	m_statsTitleLabelY = 208;
	m_statsHeaderLabelXOffset = 7;
	m_statsValueLabelXOffset = 105;

	m_pCharacterWindow->SetDimensions(windowWidth - 370, 450, m_characterWindowWidth, m_characterWindowHeight);
	m_pCharacterWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);
	m_pCharacterWindow->SetTitleOffset(50, 5);
	m_pCharacterWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pCharacterWindow->SetApplicationBorder(25, 15, 10, 10);

	m_pDynamicPaperDollIcon->SetDimensions(m_paperDollX, m_paperDollY, m_paperDollWidth, m_paperDollHeight);

	m_pTitleBarIcon->SetDimensions(0, m_characterWindowHeight, 44, 44);

	m_pCloseExitButton->SetDimensions(m_characterWindowWidth - 32, m_characterWindowHeight, 32, 32);

	m_pArrowLeftButton->SetDimensions(84, 87, 32, 32);
	m_pArrowRightButton->SetDimensions(189, 87, 32, 32);
}

// Skinning the GUI
void CharacterGUI::SkinGUI()
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/common/items/border_common.tga";
	m_pInventoryBackgroundSlotBorderCommon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_uncommon.tga";
	m_pInventoryBackgroundSlotBorderUncommon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_magical.tga";
	m_pInventoryBackgroundSlotBorderMagical->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_rare.tga";
	m_pInventoryBackgroundSlotBorderRare->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_epic.tga";
	m_pInventoryBackgroundSlotBorderEpic->SetIcon(iconName);
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

	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/tab_openclose_left.tga";
	m_pTabOpenCloseButtonIconLeft->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/tab_openclose_left_hover.tga";
	m_pTabOpenCloseButtonIconLeftHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/tab_openclose_left_pressed.tga";
	m_pTabOpenCloseButtonIconLeftPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/tab_openclose_right.tga";
	m_pTabOpenCloseButtonIconRight->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/tab_openclose_right_hover.tga";
	m_pTabOpenCloseButtonIconRightHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/tab_openclose_right_pressed.tga";
	m_pTabOpenCloseButtonIconRightPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/stats_tab_left.tga";
	m_pStatsTabIconLeft->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/stats_tab_right.tga";
	m_pStatsTabIconRight->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/equip_hover_highlight.tga";
	m_pEquipHoverIcon->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/character_title_icon.tga";
	m_pTitleBarIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/character_window_background.tga";
	m_pCharacterWindowBackgroundIcon->SetIcon(iconName);
	m_pCharacterWindowBackgroundIcon->SetDepth(2.0f);
	iconName = "Resources/textures/gui/" + themeName + "/CharacterGUI/titlebar_background.tga";
	m_pTitleBarBackgroundIcon->SetIcon(iconName);

	m_pCharacterWindow->SetBackgroundIcon(m_pCharacterWindowBackgroundIcon);
	m_pCharacterWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	Point location = m_pCharacterWindow->GetLocation();
	m_pCharacterWindow->SetDimensions(location.x, location.y, m_characterWindowWidth, m_characterWindowHeight);
	m_pCharacterWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);

	m_pCloseExitButton->SetDefaultIcon(m_pFrontendManager->GetCloseExitButtonIcon());
	m_pCloseExitButton->SetHoverIcon(m_pFrontendManager->GetCloseExitButtonIconHover());
	m_pCloseExitButton->SetSelectedIcon(m_pFrontendManager->GetCloseExitButtonIconPressed());
	m_pCloseExitButton->SetDisabledIcon(m_pFrontendManager->GetCloseExitButtonIcon());

	m_pArrowLeftButton->SetDefaultIcon(m_pFrontendManager->GetLeftRotateArrow());
	m_pArrowLeftButton->SetHoverIcon(m_pFrontendManager->GetLeftRotateArrowHover());
	m_pArrowLeftButton->SetSelectedIcon(m_pFrontendManager->GetLeftRotateArrowPressed());
	m_pArrowLeftButton->SetDisabledIcon(m_pFrontendManager->GetLeftRotateArrow());
	m_pArrowRightButton->SetDefaultIcon(m_pFrontendManager->GetRightRotateArrow());
	m_pArrowRightButton->SetHoverIcon(m_pFrontendManager->GetRightRotateArrowHover());
	m_pArrowRightButton->SetSelectedIcon(m_pFrontendManager->GetRightRotateArrowPressed());
	m_pArrowRightButton->SetDisabledIcon(m_pFrontendManager->GetRightRotateArrow());
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::UnSkinGUI() const
{
	// Do nothing
}

// Loading
void CharacterGUI::Load(bool loadDelay, float loadDelayTime)
{
	m_loadDelay = loadDelay;
	m_loadDelayTime = loadDelayTime;

	CreateInventoryItems();

	for (size_t i = 0; i < m_vpInventorySlotItems.size(); ++i)
	{
		m_pCharacterWindow->AddComponent(m_vpInventorySlotItems[i]->m_pInventoryIcon);
	}

	if (m_pPlayer->GetPlayerStats()->GetNumPointsAvailable() > 0)
	{
		m_pCharacterWindow->AddComponent(m_pStat1IncreaseButton);
		m_pCharacterWindow->AddComponent(m_pStat2IncreaseButton);
		m_pCharacterWindow->AddComponent(m_pStat3IncreaseButton);
		m_pCharacterWindow->AddComponent(m_pStat4IncreaseButton);
		m_pCharacterWindow->AddComponent(m_pStat5IncreaseButton);
		m_pCharacterWindow->AddComponent(m_pStat6IncreaseButton);

		m_addedStatIncreaseButtons = true;
	}

	m_pCharacterWindow->DepthSortComponentChildren();
	if (m_loadDelay == false)
	{
		m_pGUI->AddWindow(m_pCharacterWindow);
		m_pCharacterWindow->Show();
	}

	UpdatePlayerStats();

	StartEquipHoverAnimation();

	m_statsTabsOpen = false;
	Update(0.01f);

	m_pressedX = 0;
	m_pressedY = 0;
	m_pPressedCharacterItem = nullptr;

	m_toolTipVisible = false;
	m_tooltipAppearDelayTimer = 0.0f;
	m_toolTipComponentsAdded = false;
	m_tooltipQuality = ItemQuality::Common;

	m_leftArrowPressed = false;
	m_rightArrowPressed = false;

	CubbyGame::GetInstance()->SetPaperdollRotation(0.0f);

	m_loaded = true;
}

void CharacterGUI::Unload()
{
	m_loadDelay = false;
	m_loadDelayTime = 0.0f;

	HideTooltip();

	HideEquipHover();

	StopEquipHoverAnimation();

	m_pCharacterWindow->RemoveComponent(m_pStat1IncreaseButton);
	m_pCharacterWindow->RemoveComponent(m_pStat2IncreaseButton);
	m_pCharacterWindow->RemoveComponent(m_pStat3IncreaseButton);
	m_pCharacterWindow->RemoveComponent(m_pStat4IncreaseButton);
	m_pCharacterWindow->RemoveComponent(m_pStat5IncreaseButton);
	m_pCharacterWindow->RemoveComponent(m_pStat6IncreaseButton);

	m_pGUI->RemoveWindow(m_pCharacterWindow);

	DeleteInventoryItems();

	m_loaded = false;

	if (CubbyGame::GetInstance()->IsGUIWindowStillDisplayed() == false)
	{
		CubbyGame::GetInstance()->TurnCursorOff(false);
		if (CubbyGame::GetInstance()->ShouldRestorePreviousCameraMode())
		{
			CubbyGame::GetInstance()->RestorePreviousCameraMode();
			CubbyGame::GetInstance()->InitializeCameraRotation();
		}
	}
}

bool CharacterGUI::IsLoadDelayed() const
{
	return (m_loadDelay == true && m_loadDelayTime > 0.0f);
}

bool CharacterGUI::IsLoaded() const
{
	return m_loaded;
}

GUIWindow* CharacterGUI::GetCharacterWindow() const
{
	return m_pCharacterWindow;
}

void CharacterGUI::CreateInventoryItems()
{
	// Also go through all the equipped items and reset their positions
	for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); i++)
	{
		if (i == static_cast<int>(EquipSlot::NoSlot))
		{
			continue;
		}

		InventoryItem* pItem = m_pInventoryManager->GetInventoryItemForEquipSlot(static_cast<EquipSlot>(i));
		if (pItem != nullptr)
		{
			int x;
			int y;
			int width;
			int height;

			switch (pItem->m_equipSlot)
			{
			case EquipSlot::NoSlot: { x = 0; y = 0; width = 0; height = 0; } break;
			case EquipSlot::RightHand: { GetPlayerWeaponRightDimensions(&x, &y, &width, &height); } break;
			case EquipSlot::LeftHand: { GetPlayerWeaponLeftDimensions(&x, &y, &width, &height); } break;
			case EquipSlot::Head: { GetPlayerHeadDimensions(&x, &y, &width, &height); } break;
			case EquipSlot::Shoulders: { GetPlayerShoulderDimensions(&x, &y, &width, &height); } break;
			case EquipSlot::Body: { GetPlayerBodyDimensions(&x, &y, &width, &height); } break;
			case EquipSlot::Legs: { GetPlayerLegsDimensions(&x, &y, &width, &height); } break;
			case EquipSlot::Hand: { GetPlayerHandDimensions(&x, &y, &width, &height); } break;
			case EquipSlot::Feet: { GetPlayerFeetDimensions(&x, &y, &width, &height); } break;
			case EquipSlot::Accessory1: { GetPlayerAccessory1Dimensions(&x, &y, &width, &height); } break;
			case EquipSlot::Accessory2: { GetPlayerAccessory2Dimensions(&x, &y, &width, &height); } break;
			case EquipSlot::NumSlots: { throw std::logic_error("Invalid EquipSlot in CreateInventoryItems()"); }
			default: { throw std::logic_error("Invalid EquipSlot in CreateInventoryItems()"); }
			}

			DraggableRenderRectangle* pNewSlotItem = new DraggableRenderRectangle(m_pRenderer);
			
			switch (pItem->m_itemQuality)
			{
			case ItemQuality::Common: { pNewSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderCommon); break; }
			case ItemQuality::Uncommon: { pNewSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderUncommon); break; }
			case ItemQuality::Magical: { pNewSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderMagical); break; }
			case ItemQuality::Rare: { pNewSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderRare); break; }
			case ItemQuality::Epic: { pNewSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderEpic); break; }
			default: { throw std::logic_error("Invalid ItemQuality in CreateInventoryItems()"); }
			}
			pNewSlotItem->SetDimensions(x, y, width, height);
			pNewSlotItem->SetDepth(3.0f);

			char lItemTexture[128];
			sprintf(lItemTexture, "%s", pItem->m_iconFileName.c_str());
			pNewSlotItem->AddIcon(m_pRenderer, lItemTexture, 64, 64, 56, 56, 4, 4, 1.5f);

			std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
			
			switch (pItem->m_itemQuality)
			{
			case ItemQuality::Common:
			{
				std::string itemBackgroundIcon = "Resources/textures/gui/" + themeName + "/common/items/item_background_common.tga";
				pNewSlotItem->AddIcon(m_pRenderer, itemBackgroundIcon.c_str(), 64, 64, 64, 64, 0, 0, 1.0f);
				break;
			}
			case ItemQuality::Uncommon:
			{
				std::string itemBackgroundIcon = "Resources/textures/gui/" + themeName + "/common/items/item_background_uncommon.tga";
				pNewSlotItem->AddIcon(m_pRenderer, itemBackgroundIcon.c_str(), 64, 64, 64, 64, 0, 0, 1.0f);
				break;
			}
			case ItemQuality::Magical:
			{
				std::string itemBackgroundIcon = "Resources/textures/gui/" + themeName + "/common/items/item_background_magical.tga";
				pNewSlotItem->AddIcon(m_pRenderer, itemBackgroundIcon.c_str(), 64, 64, 64, 64, 0, 0, 1.0f);
				break;
			}
			case ItemQuality::Rare:
			{
				std::string itemBackgroundIcon = "Resources/textures/gui/" + themeName + "/common/items/item_background_rare.tga";
				pNewSlotItem->AddIcon(m_pRenderer, itemBackgroundIcon.c_str(), 64, 64, 64, 64, 0, 0, 1.0f);
				break;
			}
			case ItemQuality::Epic:
			{
				std::string itemBackgroundIcon = "Resources/textures/gui/" + themeName + "/common/items/item_background_epic.tga";
				pNewSlotItem->AddIcon(m_pRenderer, itemBackgroundIcon.c_str(), 64, 64, 64, 64, 0, 0, 1.0f);
				break;
			}
			default:
			{
				throw std::logic_error("Invalid ItemQuality in CreateInventoryItems()");
			}
			}

			if (pItem->m_quantity != -1)
			{
				char quantity[128];
				sprintf(quantity, "%i", pItem->m_quantity);
				int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18(), "%s", quantity);
				pNewSlotItem->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), quantity, Color(1.0f, 1.0f, 1.0f, 1.0f), width - 9 - textWidth, 7, true, Color(0.0f, 0.0f, 0.0f, 1.0f));
			}

			CharacterSlotItem* pNewItem = new CharacterSlotItem();
			pNewItem->m_pCharacterGUI = this;
			pNewItem->m_pInventoryItem = pItem;
			pNewItem->m_pInventoryIcon = pNewSlotItem;
			pNewItem->m_slotX = -1;
			pNewItem->m_slotY = -1;
			m_vpInventorySlotItems.push_back(pNewItem);

			pNewSlotItem->SetPressedCallBackFunction(_CharacterItemPressed);
			pNewSlotItem->SetPressedCallBackData(pNewItem);

			pNewSlotItem->SetReleasedCallBackFunction(_CharacterItemReleased);
			pNewSlotItem->SetReleasedCallBackData(pNewItem);

			pNewSlotItem->SetEnterCallBackFunction(_CharacterItemEntered);
			pNewSlotItem->SetEnterCallBackData(pNewItem);

			pNewSlotItem->SetExitCallBackFunction(_CharacterItemExited);
			pNewSlotItem->SetExitCallBackData(pNewItem);

			pNewItem->m_pInventoryIcon->SetLocation(x, y);

			m_pEquippedItems[static_cast<int>(pItem->m_equipSlot)] = pNewItem;

			if (pItem->m_equipSlot == EquipSlot::LeftHand && pItem->m_right)
			{
				int x2;
				int y2;
				int width2;
				int height2;
				GetPlayerWeaponRightDimensions(&x2, &y2, &width2, &height2);

				pBlockSlotItem = new Icon(m_pRenderer);
				pBlockSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderCommon);
				std::string themeName2 = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
				std::string itemForegroundIcon = "Resources/textures/gui/" + themeName2 + "/common/items/item_foreground_alpha.tga";
				pBlockSlotItem->AddIcon(m_pRenderer, itemForegroundIcon.c_str(), 64, 64, 64, 64, 0, 0, 3.0f);

				char itemTexture[128];
				sprintf(itemTexture, "%s", pItem->m_iconFileName.c_str());
				pBlockSlotItem->AddIcon(m_pRenderer, itemTexture, 64, 64, 56, 56, 4, 4, 1.5f);

				pBlockSlotItem->SetDimensions(x2, y2, width2, height2);
				pBlockSlotItem->SetDepth(2.0f);

				m_pCharacterWindow->AddComponent(pBlockSlotItem);
			}
			if (pItem->m_equipSlot == EquipSlot::RightHand && pItem->m_left)
			{
				int x2;
				int y2;
				int width2;
				int height2;
				GetPlayerWeaponLeftDimensions(&x2, &y2, &width2, &height2);

				pBlockSlotItem = new Icon(m_pRenderer);
				pBlockSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderCommon);
				std::string themeName2 = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
				std::string itemForegroundIcon = "Resources/textures/gui/" + themeName2 + "/common/items/item_foreground_alpha.tga";
				pBlockSlotItem->AddIcon(m_pRenderer, itemForegroundIcon.c_str(), 64, 64, 64, 64, 0, 0, 3.0f);

				char itemTexture[128];
				sprintf(itemTexture, "%s", pItem->m_iconFileName.c_str());
				pBlockSlotItem->AddIcon(m_pRenderer, itemTexture, 64, 64, 56, 56, 4, 4, 1.5f);

				pBlockSlotItem->SetDimensions(x2, y2, width2, height2);
				pBlockSlotItem->SetDepth(2.0f);

				m_pCharacterWindow->AddComponent(pBlockSlotItem);
			}
		}
	}
}

void CharacterGUI::DeleteInventoryItems()
{
	// Clear equipped items
	for (size_t i = 0; i < static_cast<unsigned int>(EquipSlot::NumSlots); ++i)
	{
		m_pEquippedItems[i] = nullptr;
	}

	// Clear item draggable buttons
	for (size_t i = 0; i < m_vpInventorySlotItems.size(); ++i)
	{
		m_pCharacterWindow->RemoveComponent(m_vpInventorySlotItems[i]->m_pInventoryIcon);
		delete m_vpInventorySlotItems[i]->m_pInventoryIcon;
		m_vpInventorySlotItems[i]->m_pInventoryIcon = nullptr;

		delete m_vpInventorySlotItems[i];
		m_vpInventorySlotItems[i] = nullptr;
	}
	m_vpInventorySlotItems.clear();

	if (pBlockSlotItem != nullptr)
	{
		m_pCharacterWindow->RemoveComponent(pBlockSlotItem);
		delete pBlockSlotItem;
		pBlockSlotItem = nullptr;
	}
}

void CharacterGUI::ShowEquipHover(EquipSlot equipSlot)
{
	if (CubbyGame::GetInstance()->GetCubbySettings()->m_equipHelper == false)
	{
		// If we have turned off the equip helper in the options menu, do nothing
		return;
	}

	int x;
	int y;
	int width;
	int height;

	switch (equipSlot)
	{
	case EquipSlot::NoSlot: { x = 0; y = 0; width = 0; height = 0; } break;
	case EquipSlot::RightHand: { GetPlayerWeaponRightDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::LeftHand: {  GetPlayerWeaponLeftDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Head: {  GetPlayerHeadDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Shoulders: {  GetPlayerShoulderDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Body: {  GetPlayerBodyDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Legs: {  GetPlayerLegsDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Hand: {  GetPlayerHandDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Feet: {  GetPlayerFeetDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Accessory1: {  GetPlayerAccessory1Dimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Accessory2: {  GetPlayerAccessory2Dimensions(&x, &y, &width, &height); } break;
	case EquipSlot::NumSlots: { throw std::logic_error("Invalid EquipSlot in ShowEquipHover()"); }
	default: { throw std::logic_error("Invalid EquipSlot in ShowEquipHover()"); }
	}
	
	m_equipHoverXOrigin = x;
	m_pEquipHoverIcon->SetDimensions(m_equipHoverXOrigin + static_cast<int>(m_equipHoverXOffset), y, width + 16, height + 16);
	m_pEquipHoverIcon->SetDepth(5.5f);

	m_pCharacterWindow->AddComponent(m_pEquipHoverIcon);
}

void CharacterGUI::HideEquipHover() const
{
	if (CubbyGame::GetInstance()->GetCubbySettings()->m_equipHelper == false)
	{
		// If we have turned off the equip helper in the options menu, do nothing
		return;
	}

	m_pCharacterWindow->RemoveComponent(m_pEquipHoverIcon);
}

void CharacterGUI::UpdateEquipHover() const
{
	Dimensions equipHoverDimensions = m_pEquipHoverIcon->GetDimensions();
	m_pEquipHoverIcon->SetDimensions(m_equipHoverXOrigin + static_cast<int>(m_equipHoverXOffset), equipHoverDimensions.y, equipHoverDimensions.width, equipHoverDimensions.height);
}

void CharacterGUI::StartEquipHoverAnimation()
{
	m_equipHoverXOffset = 0.0f;

	FloatInterpolation* pXPosInterp1;
	FloatInterpolation* pXPosInterp2;
	pXPosInterp1 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_equipHoverXOffset, m_equipHoverXOffset, m_equipHoverXOffset + 10.0f, 0.5f, 100.0f);
	pXPosInterp2 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_equipHoverXOffset, m_equipHoverXOffset + 10.0f, m_equipHoverXOffset, 0.5f, -100.0f, nullptr, _EquipHoverAnimationFinished, this);
	Interpolator::GetInstance()->LinkFloatInterpolation(pXPosInterp1, pXPosInterp2);
	Interpolator::GetInstance()->AddFloatInterpolation(pXPosInterp1);
}

void CharacterGUI::StopEquipHoverAnimation()
{
	Interpolator::GetInstance()->RemoveFloatInterpolationByVariable(&m_equipHoverXOffset);
}

// Tooltips
void CharacterGUI::UpdateToolTipAppear(float dt)
{
	if (m_loadDelay == true)
	{
		if (m_loadDelayTime <= 0.0f)
		{
			m_loadDelay = false;
			m_pGUI->AddWindow(m_pCharacterWindow);
			m_pCharacterWindow->Show();
		}
		else
		{
			m_loadDelayTime -= dt;
		}
	}

	if (m_toolTipVisible)
	{
		if (m_tooltipAppearDelayTimer <= 0.0f)
		{
			if (m_toolTipComponentsAdded == false)
			{
				switch (m_tooltipQuality)
				{
				case ItemQuality::Common: { m_pCharacterWindow->AddComponent(m_pTooltipBackgroundCommon); break; }
				case ItemQuality::Uncommon: { m_pCharacterWindow->AddComponent(m_pTooltipBackgroundUncommon); break; }
				case ItemQuality::Magical: { m_pCharacterWindow->AddComponent(m_pTooltipBackgroundMagical); break; }
				case ItemQuality::Rare: { m_pCharacterWindow->AddComponent(m_pTooltipBackgroundRare); break; }
				case ItemQuality::Epic: { m_pCharacterWindow->AddComponent(m_pTooltipBackgroundEpic); break; }
				default: { throw std::logic_error("Invalid ItemQuality in UpdateToolTipAppear()"); }
				}

				m_pCharacterWindow->AddComponent(m_pTooltipNameLabel);
				m_pCharacterWindow->AddComponent(m_pTooltipDescriptionLabel);
				m_pCharacterWindow->AddComponent(m_pTooltipSlotLabel);
				m_pCharacterWindow->AddComponent(m_pTooltipQualityLabel);

				m_toolTipComponentsAdded = true;
			}
		}
		else
		{
			m_tooltipAppearDelayTimer -= dt;
		}
	}
}

void CharacterGUI::ShowTooltip(CharacterSlotItem* pCharacterItem)
{
	if (m_toolTipVisible == true)
	{
		return;
	}

	// Set the focused window when we show a tooltip
	m_pCharacterWindow->SetFocusWindow();

	// Replace the tooltip name
	m_pTooltipNameLabel->SetText(pCharacterItem->m_pInventoryItem->m_title);

	// Replace the tooltip description
	std::string DescriptionText = pCharacterItem->m_pInventoryItem->m_description + pCharacterItem->m_pInventoryItem->GetStatsAttributeString();
	m_pTooltipDescriptionLabel->SetText(DescriptionText);

	// Replace the tooltip equip slot text
	char slotText[32];
	switch (pCharacterItem->m_pInventoryItem->m_equipSlot)
	{
	case EquipSlot::NoSlot: { sprintf(slotText, ""); break; }
	case EquipSlot::LeftHand:
	{
		if (pCharacterItem->m_pInventoryItem->m_right)
		{
			sprintf(slotText, "Two Handed");
		}
		else
		{
			sprintf(slotText, "Left Hand");
		}

		break;
	}
	case EquipSlot::RightHand:
	{
		if (pCharacterItem->m_pInventoryItem->m_left)
		{
			sprintf(slotText, "Two Handed");
		}
		else
		{
			sprintf(slotText, "Right Hand");
		}

		break;
	}
	case EquipSlot::Head: { sprintf(slotText, "Head"); break; }
	case EquipSlot::Shoulders: { sprintf(slotText, "Shoulders"); break; }
	case EquipSlot::Body: { sprintf(slotText, "Body"); break; }
	case EquipSlot::Legs: { sprintf(slotText, "Legs"); break; }
	case EquipSlot::Hand: { sprintf(slotText, "Hand"); break; }
	case EquipSlot::Feet: { sprintf(slotText, "Feet"); break; }
	case EquipSlot::Accessory1: { sprintf(slotText, "Accessory 1"); break; }
	case EquipSlot::Accessory2: { sprintf(slotText, "Accessory 2"); break; }
	default: { throw std::logic_error("Invalid EquipSlot in ShowTooltip()"); }
	}
	m_pTooltipSlotLabel->SetText(slotText);

	// Replace the tooltip quality text
	char qualityText[32];
	Color qualityColor;
	switch (pCharacterItem->m_pInventoryItem->m_itemQuality)
	{
	case ItemQuality::Common: { sprintf(qualityText, "Common"); qualityColor = Color(0.5f, 0.5f, 0.5f, 1.0f); break; }
	case ItemQuality::Uncommon: { sprintf(qualityText, "Uncommon"); qualityColor = Color(0.95f, 1.0f, 0.2f, 1.0f); break; }
	case ItemQuality::Magical: { sprintf(qualityText, "Magical"); qualityColor = Color(0.0f, 1.0f, 0.0f, 1.0f); break; }
	case ItemQuality::Rare: { sprintf(qualityText, "Rare"); qualityColor = Color(0.0f, 0.5f, 1.0f, 1.0f); break; }
	case ItemQuality::Epic: { sprintf(qualityText, "Epic"); qualityColor = Color(0.64f, 0.2f, 0.93f, 1.0f); break; }
	default: { throw std::logic_error("Invalid ItemQuality in ShowTooltip()"); }
	}
	m_pTooltipQualityLabel->SetText(qualityText);
	m_pTooltipQualityLabel->SetColor(qualityColor);

	m_pTooltipNameLabel->SetColor(qualityColor);

	// Set Component dimensions
	m_tooltipWidth = 200;
	m_tooltipHeight = 220;
	m_tooltipDescBorder = 15;

	int x;
	int y;
	int width;
	int height;
	switch (pCharacterItem->m_pInventoryItem->m_equipSlot)
	{
	case EquipSlot::NoSlot: { x = 0; y = 0; } break;
	case EquipSlot::RightHand: { GetPlayerWeaponRightDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::LeftHand: { GetPlayerWeaponLeftDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Head: { GetPlayerHeadDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Shoulders: { GetPlayerShoulderDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Body: { GetPlayerBodyDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Legs: { GetPlayerLegsDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Hand: { GetPlayerHandDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Feet: { GetPlayerFeetDimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Accessory1: { GetPlayerAccessory1Dimensions(&x, &y, &width, &height); } break;
	case EquipSlot::Accessory2: { GetPlayerAccessory2Dimensions(&x, &y, &width, &height); } break;
	case EquipSlot::NumSlots: { throw std::logic_error("Invalid EquipSlot in ShowTooltip()"); }
	default: { throw std::logic_error("Invalid EquipSlot in ShowTooltip()"); }
	}

	if (CubbyGame::GetInstance()->GetWindowCursorX() > m_tooltipWidth + 50)
	{
		x = x + 20 - m_tooltipWidth;
	}
	else
	{
		x = x + 30;
	}
	if ((m_windowHeight - CubbyGame::GetInstance()->GetWindowCursorY()) > m_windowHeight - m_tooltipHeight - 50)
	{
		y = y + 20 - m_tooltipHeight;
	}
	else
	{
		y = y + 30;
	}

	m_pTooltipBackgroundCommon->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);
	m_pTooltipBackgroundUncommon->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);
	m_pTooltipBackgroundMagical->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);
	m_pTooltipBackgroundRare->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);
	m_pTooltipBackgroundEpic->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);

	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont30(), "%s", m_pTooltipNameLabel->GetText().c_str());
	m_pTooltipNameLabel->SetLocation(x + static_cast<int>(m_tooltipWidth * 0.5f) - static_cast<int>(textWidth * 0.5f), y + m_tooltipHeight - 35);

	//textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont25(), "%s", m_pTooltipDescriptionLabel->GetText().c_str());
	m_pTooltipDescriptionLabel->SetDimensions(x + m_tooltipDescBorder, y + m_tooltipDescBorder, m_tooltipWidth - (m_tooltipDescBorder * 2), m_tooltipHeight - (m_tooltipDescBorder * 2) - 35);

	m_pTooltipSlotLabel->SetLocation(x + m_tooltipDescBorder, y + m_tooltipDescBorder);

	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont20(), "%s", m_pTooltipQualityLabel->GetText().c_str());
	m_pTooltipQualityLabel->SetLocation(x + m_tooltipWidth - m_tooltipDescBorder - textWidth, y + m_tooltipDescBorder);

	m_tooltipQuality = pCharacterItem->m_pInventoryItem->m_itemQuality;

	m_tooltipAppearDelayTimer = m_pFrontendManager->GetToolTipAppearDelay();

	m_toolTipVisible = true;
	m_toolTipComponentsAdded = false;
}

void CharacterGUI::HideTooltip()
{
	m_pCharacterWindow->RemoveComponent(m_pTooltipBackgroundCommon);
	m_pCharacterWindow->RemoveComponent(m_pTooltipBackgroundUncommon);
	m_pCharacterWindow->RemoveComponent(m_pTooltipBackgroundMagical);
	m_pCharacterWindow->RemoveComponent(m_pTooltipBackgroundRare);
	m_pCharacterWindow->RemoveComponent(m_pTooltipBackgroundEpic);
	m_pCharacterWindow->RemoveComponent(m_pTooltipNameLabel);
	m_pCharacterWindow->RemoveComponent(m_pTooltipDescriptionLabel);
	m_pCharacterWindow->RemoveComponent(m_pTooltipSlotLabel);
	m_pCharacterWindow->RemoveComponent(m_pTooltipQualityLabel);

	m_toolTipVisible = false;
}

void CharacterGUI::GetPlayerPortraitDimensions(int* x, int* y, int* width, int* height) const
{
	*x = m_paperDollX;
	*y = m_paperDollY;

	*width = m_paperDollWidth;
	*height = m_paperDollHeight;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::GetPlayerWeaponLeftDimensions(int* x, int* y, int* width, int* height) const
{
	*x = 157;
	*y = 18;

	*width = 64;
	*height = 64;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::GetPlayerWeaponRightDimensions(int* x, int* y, int* width, int* height) const
{
	*x = 84;
	*y = 18;

	*width = 64;
	*height = 64;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::GetPlayerHeadDimensions(int* x, int* y, int* width, int* height) const
{
	*x = 4;
	*y = 213;

	*width = 64;
	*height = 64;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::GetPlayerShoulderDimensions(int* x, int* y, int* width, int* height) const
{
	*x = 236;
	*y = 213;

	*width = 64;
	*height = 64;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::GetPlayerBodyDimensions(int* x, int* y, int* width, int* height) const
{
	*x = 4;
	*y = 145;

	*width = 64;
	*height = 64;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::GetPlayerLegsDimensions(int* x, int* y, int* width, int* height) const
{
	*x = 236;
	*y = 145;

	*width = 64;
	*height = 64;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::GetPlayerHandDimensions(int* x, int* y, int* width, int* height) const
{
	*x = 4;
	*y = 77;

	*width = 64;
	*height = 64;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::GetPlayerFeetDimensions(int* x, int* y, int* width, int* height) const
{
	*x = 236;
	*y = 77;

	*width = 64;
	*height = 64;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::GetPlayerAccessory1Dimensions(int* x, int* y, int* width, int* height) const
{
	*x = 4;
	*y = 8;

	*width = 64;
	*height = 64;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CharacterGUI::GetPlayerAccessory2Dimensions(int* x, int* y, int* width, int* height) const
{
	*x = 236;
	*y = 8;

	*width = 64;
	*height = 64;
}

void CharacterGUI::UpdatePlayerStats()
{
	// Player name
	m_pCharacterTitleLabel->SetText(m_pPlayer->GetName());

	// Stats
	// Strength
	char stat1Value[32];
	sprintf(stat1Value, "%i %s", m_pPlayer->GetPlayerStats()->GetStrengthStat(), m_pPlayer->GetStrengthModifierString().c_str());
	m_pStatsValueLabel1->SetText(stat1Value);

	// Dexterity
	char stat2Value[32];
	sprintf(stat2Value, "%i %s", m_pPlayer->GetPlayerStats()->GetDexterityStat(), m_pPlayer->GetDexterityModifierString().c_str());
	m_pStatsValueLabel2->SetText(stat2Value);

	// Magic
	char stat3Value[32];
	sprintf(stat3Value, "%i %s", m_pPlayer->GetPlayerStats()->GetIntelligenceStat(), m_pPlayer->GetIntelligenceModifierString().c_str());
	m_pStatsValueLabel3->SetText(stat3Value);

	// Vitality
	char stat4Value[32];
	sprintf(stat4Value, "%i %s", m_pPlayer->GetPlayerStats()->GetVitalityStat(), m_pPlayer->GetVitalityModifierString().c_str());
	m_pStatsValueLabel4->SetText(stat4Value);

	// Armor
	char stat5Value[32];
	sprintf(stat5Value, "%i %s", m_pPlayer->GetPlayerStats()->GetArmorStat(), m_pPlayer->GetArmorModifierString().c_str());
	m_pStatsValueLabel5->SetText(stat5Value);

	// Luck
	char stat6Value[32];
	sprintf(stat6Value, "%i %s", m_pPlayer->GetPlayerStats()->GetLuckStat(), m_pPlayer->GetLuckModifierString().c_str());
	m_pStatsValueLabel6->SetText(stat6Value);

	if (m_pPlayer->GetPlayerStats()->GetNumPointsAvailable() <= 0)
	{
		m_pCharacterWindow->RemoveComponent(m_pStat1IncreaseButton);
		m_pCharacterWindow->RemoveComponent(m_pStat2IncreaseButton);
		m_pCharacterWindow->RemoveComponent(m_pStat3IncreaseButton);
		m_pCharacterWindow->RemoveComponent(m_pStat4IncreaseButton);
		m_pCharacterWindow->RemoveComponent(m_pStat5IncreaseButton);
		m_pCharacterWindow->RemoveComponent(m_pStat6IncreaseButton);

		m_addedStatIncreaseButtons = false;
	}
	else
	{
		if (m_loaded)
		{
			if (m_addedStatIncreaseButtons == false)
			{
				m_pGUI->RemoveWindow(m_pCharacterWindow);

				m_pCharacterWindow->AddComponent(m_pStat1IncreaseButton);
				m_pCharacterWindow->AddComponent(m_pStat2IncreaseButton);
				m_pCharacterWindow->AddComponent(m_pStat3IncreaseButton);
				m_pCharacterWindow->AddComponent(m_pStat4IncreaseButton);
				m_pCharacterWindow->AddComponent(m_pStat5IncreaseButton);
				m_pCharacterWindow->AddComponent(m_pStat6IncreaseButton);

				m_pGUI->AddWindow(m_pCharacterWindow);
				m_pCharacterWindow->Show();

				m_addedStatIncreaseButtons = true;
			}
		}
	}
}

void CharacterGUI::Update(float dt)
{
	UpdateToolTipAppear(dt);

	m_pDynamicPaperDollIcon->SetDynamicTexture(CubbyGame::GetInstance()->GetDynamicPaperdollTexture());

	// Check if the character GUI needs update (we have equipped items, or quantity changed, etc)
	if (m_pInventoryManager->CharacterGUINeedsUpdate() && IsLoaded() == true)
	{
		m_pGUI->RemoveWindow(m_pCharacterWindow);

		DeleteInventoryItems();
		CreateInventoryItems();

		for (size_t i = 0; i < m_vpInventorySlotItems.size(); ++i)
		{
			m_pCharacterWindow->AddComponent(m_vpInventorySlotItems[i]->m_pInventoryIcon);
		}

		m_pGUI->AddWindow(m_pCharacterWindow);
		m_pCharacterWindow->Show();

		m_pInventoryManager->SetCharacterGUINeedsUpdate(false);
	}

	// Update equip hover icon
	UpdateEquipHover();

	if (m_rightArrowPressed)
	{
		CubbyGame::GetInstance()->RotatePaperdollModel(150.0f * dt);
	}
	if (m_leftArrowPressed)
	{
		CubbyGame::GetInstance()->RotatePaperdollModel(-150.0f * dt);
	}

	// Update the stats tab left and right sides
	if (m_statsTabLeftSide)
	{
		m_pTabOpenCloseButton->SetDefaultIcon(m_pTabOpenCloseButtonIconLeft);
		m_pTabOpenCloseButton->SetHoverIcon(m_pTabOpenCloseButtonIconLeftHover);
		m_pTabOpenCloseButton->SetSelectedIcon(m_pTabOpenCloseButtonIconLeftPressed);
		m_pTabOpenCloseButton->SetDisabledIcon(m_pTabOpenCloseButtonIconLeft);

		if (m_statsTabsOpen)
		{
			m_pTabOpenCloseButton->SetDimensions(-m_statsTabWidth - m_statsTabOpenButtonWidth, (m_characterWindowHeight / 2) - (m_statsTabOpenButtonHeight / 2), m_statsTabOpenButtonWidth, m_statsTabOpenButtonHeight);
			m_pStatsTabIconLeft->SetDimensions(-m_statsTabWidth, (m_characterWindowHeight / 2) - (m_statsTabHeight / 2), m_statsTabWidth, m_statsTabHeight);
			m_pStatsTabIconRight->SetDimensions(0, (m_characterWindowHeight / 2) - (m_statsTabHeight / 2), m_statsTabWidth, m_statsTabHeight);

			int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFontMedium(), "%s", m_pCharacterTitleLabel->GetText().c_str());
			m_pCharacterTitleLabel->SetLocation(-m_statsTabWidth / 2 - textWidth / 2, m_statsTitleLabelY);

			m_pStatsHeaderLabel1->SetLocation(-m_statsTabWidth + m_statsHeaderLabelXOffset, m_statsTitleLabelY - 26);
			m_pStatsHeaderLabel2->SetLocation(-m_statsTabWidth + m_statsHeaderLabelXOffset, m_statsTitleLabelY - 52);
			m_pStatsHeaderLabel3->SetLocation(-m_statsTabWidth + m_statsHeaderLabelXOffset, m_statsTitleLabelY - 78);
			m_pStatsHeaderLabel4->SetLocation(-m_statsTabWidth + m_statsHeaderLabelXOffset, m_statsTitleLabelY - 104);
			m_pStatsHeaderLabel5->SetLocation(-m_statsTabWidth + m_statsHeaderLabelXOffset, m_statsTitleLabelY - 130);
			m_pStatsHeaderLabel6->SetLocation(-m_statsTabWidth + m_statsHeaderLabelXOffset, m_statsTitleLabelY - 156);

			m_pStatsValueLabel1->SetDimensions(-m_statsTabWidth + m_statsValueLabelXOffset, m_statsTitleLabelY - 26, 59, 26);
			m_pStatsValueLabel2->SetDimensions(-m_statsTabWidth + m_statsValueLabelXOffset, m_statsTitleLabelY - 52, 59, 26);
			m_pStatsValueLabel3->SetDimensions(-m_statsTabWidth + m_statsValueLabelXOffset, m_statsTitleLabelY - 78, 59, 26);
			m_pStatsValueLabel4->SetDimensions(-m_statsTabWidth + m_statsValueLabelXOffset, m_statsTitleLabelY - 104, 59, 26);
			m_pStatsValueLabel5->SetDimensions(-m_statsTabWidth + m_statsValueLabelXOffset, m_statsTitleLabelY - 130, 59, 26);
			m_pStatsValueLabel6->SetDimensions(-m_statsTabWidth + m_statsValueLabelXOffset, m_statsTitleLabelY - 156, 59, 26);

			if (m_pPlayer->GetPlayerStats()->GetNumPointsAvailable() > 0)
			{
				m_pStat1IncreaseButton->SetDimensions(-73, m_statsTitleLabelY - 25, 16, 16);
				m_pStat2IncreaseButton->SetDimensions(-73, m_statsTitleLabelY - 51, 16, 16);
				m_pStat3IncreaseButton->SetDimensions(-73, m_statsTitleLabelY - 77, 16, 16);
				m_pStat4IncreaseButton->SetDimensions(-73, m_statsTitleLabelY - 103, 16, 16);
				m_pStat5IncreaseButton->SetDimensions(-73, m_statsTitleLabelY - 129, 16, 16);
				m_pStat6IncreaseButton->SetDimensions(-73, m_statsTitleLabelY - 155, 16, 16);
			}
		}
		else
		{
			m_pTabOpenCloseButton->SetDimensions(-m_statsTabOpenButtonWidth, (m_characterWindowHeight / 2) - (m_statsTabOpenButtonHeight / 2), m_statsTabOpenButtonWidth, m_statsTabOpenButtonHeight);
			m_pStatsTabIconLeft->SetDimensions(0, (m_characterWindowHeight / 2) - (m_statsTabHeight / 2), m_statsTabWidth, m_statsTabHeight);
			m_pStatsTabIconRight->SetDimensions(0, (m_characterWindowHeight / 2) - (m_statsTabHeight / 2), m_statsTabWidth, m_statsTabHeight);

			m_pCharacterTitleLabel->SetLocation(-5000, -5000);

			m_pStatsHeaderLabel1->SetLocation(-5000, -5000);
			m_pStatsHeaderLabel2->SetLocation(-5000, -5000);
			m_pStatsHeaderLabel3->SetLocation(-5000, -5000);
			m_pStatsHeaderLabel4->SetLocation(-5000, -5000);
			m_pStatsHeaderLabel5->SetLocation(-5000, -5000);
			m_pStatsHeaderLabel6->SetLocation(-5000, -5000);
			m_pStatsValueLabel1->SetLocation(-5000, -5000);
			m_pStatsValueLabel2->SetLocation(-5000, -5000);
			m_pStatsValueLabel3->SetLocation(-5000, -5000);
			m_pStatsValueLabel4->SetLocation(-5000, -5000);
			m_pStatsValueLabel5->SetLocation(-5000, -5000);
			m_pStatsValueLabel6->SetLocation(-5000, -5000);

			m_pStat1IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
			m_pStat2IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
			m_pStat3IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
			m_pStat4IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
			m_pStat5IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
			m_pStat6IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
		}
	}
	else
	{
		m_pTabOpenCloseButton->SetDefaultIcon(m_pTabOpenCloseButtonIconRight);
		m_pTabOpenCloseButton->SetHoverIcon(m_pTabOpenCloseButtonIconRightHover);
		m_pTabOpenCloseButton->SetSelectedIcon(m_pTabOpenCloseButtonIconRightPressed);
		m_pTabOpenCloseButton->SetDisabledIcon(m_pTabOpenCloseButtonIconRightPressed);

		if (m_statsTabsOpen)
		{
			m_pTabOpenCloseButton->SetDimensions(m_characterWindowWidth + m_statsTabWidth, (m_characterWindowHeight / 2) - (m_statsTabOpenButtonHeight / 2), m_statsTabOpenButtonWidth, m_statsTabOpenButtonHeight);
			m_pStatsTabIconLeft->SetDimensions(0, (m_characterWindowHeight / 2) - (m_statsTabHeight / 2), m_statsTabWidth, m_statsTabHeight);
			m_pStatsTabIconRight->SetDimensions(m_characterWindowWidth, (m_characterWindowHeight / 2) - (m_statsTabHeight / 2), m_statsTabWidth, m_statsTabHeight);

			int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFontMedium(), "%s", m_pCharacterTitleLabel->GetText().c_str());
			m_pCharacterTitleLabel->SetLocation(m_characterWindowWidth + m_statsTabWidth / 2 - textWidth / 2, m_statsTitleLabelY);

			m_pStatsHeaderLabel1->SetLocation(m_characterWindowWidth + m_statsHeaderLabelXOffset - 4, m_statsTitleLabelY - 26);
			m_pStatsHeaderLabel2->SetLocation(m_characterWindowWidth + m_statsHeaderLabelXOffset - 4, m_statsTitleLabelY - 52);
			m_pStatsHeaderLabel3->SetLocation(m_characterWindowWidth + m_statsHeaderLabelXOffset - 4, m_statsTitleLabelY - 78);
			m_pStatsHeaderLabel4->SetLocation(m_characterWindowWidth + m_statsHeaderLabelXOffset - 4, m_statsTitleLabelY - 104);
			m_pStatsHeaderLabel5->SetLocation(m_characterWindowWidth + m_statsHeaderLabelXOffset - 4, m_statsTitleLabelY - 130);
			m_pStatsHeaderLabel6->SetLocation(m_characterWindowWidth + m_statsHeaderLabelXOffset - 4, m_statsTitleLabelY - 156);

			m_pStatsValueLabel1->SetDimensions(m_characterWindowWidth + m_statsValueLabelXOffset - 4, m_statsTitleLabelY - 26, 59, 26);
			m_pStatsValueLabel2->SetDimensions(m_characterWindowWidth + m_statsValueLabelXOffset - 4, m_statsTitleLabelY - 52, 59, 26);
			m_pStatsValueLabel3->SetDimensions(m_characterWindowWidth + m_statsValueLabelXOffset - 4, m_statsTitleLabelY - 78, 59, 26);
			m_pStatsValueLabel4->SetDimensions(m_characterWindowWidth + m_statsValueLabelXOffset - 4, m_statsTitleLabelY - 104, 59, 26);
			m_pStatsValueLabel5->SetDimensions(m_characterWindowWidth + m_statsValueLabelXOffset - 4, m_statsTitleLabelY - 130, 59, 26);
			m_pStatsValueLabel6->SetDimensions(m_characterWindowWidth + m_statsValueLabelXOffset - 4, m_statsTitleLabelY - 156, 59, 26);

			if (m_pPlayer->GetPlayerStats()->GetNumPointsAvailable() > 0)
			{
				m_pStat1IncreaseButton->SetDimensions(m_characterWindowWidth + 87, m_statsTitleLabelY - 25, 16, 16);
				m_pStat2IncreaseButton->SetDimensions(m_characterWindowWidth + 87, m_statsTitleLabelY - 51, 16, 16);
				m_pStat3IncreaseButton->SetDimensions(m_characterWindowWidth + 87, m_statsTitleLabelY - 77, 16, 16);
				m_pStat4IncreaseButton->SetDimensions(m_characterWindowWidth + 87, m_statsTitleLabelY - 103, 16, 16);
				m_pStat5IncreaseButton->SetDimensions(m_characterWindowWidth + 87, m_statsTitleLabelY - 129, 16, 16);
				m_pStat6IncreaseButton->SetDimensions(m_characterWindowWidth + 87, m_statsTitleLabelY - 155, 16, 16);
			}
		}
		else
		{
			m_pTabOpenCloseButton->SetDimensions(m_characterWindowWidth, (m_characterWindowHeight / 2) - (m_statsTabOpenButtonHeight / 2), m_statsTabOpenButtonWidth, m_statsTabOpenButtonHeight);
			m_pStatsTabIconLeft->SetDimensions(0, (m_characterWindowHeight / 2) - (m_statsTabHeight / 2), m_statsTabWidth, m_statsTabHeight);
			m_pStatsTabIconRight->SetDimensions(0, (m_characterWindowHeight / 2) - (m_statsTabHeight / 2), m_statsTabWidth, m_statsTabHeight);

			m_pCharacterTitleLabel->SetLocation(-5000, -5000);

			m_pStatsHeaderLabel1->SetLocation(-5000, -5000);
			m_pStatsHeaderLabel2->SetLocation(-5000, -5000);
			m_pStatsHeaderLabel3->SetLocation(-5000, -5000);
			m_pStatsHeaderLabel4->SetLocation(-5000, -5000);
			m_pStatsHeaderLabel5->SetLocation(-5000, -5000);
			m_pStatsHeaderLabel6->SetLocation(-5000, -5000);
			m_pStatsValueLabel1->SetLocation(-5000, -5000);
			m_pStatsValueLabel2->SetLocation(-5000, -5000);
			m_pStatsValueLabel3->SetLocation(-5000, -5000);
			m_pStatsValueLabel4->SetLocation(-5000, -5000);
			m_pStatsValueLabel5->SetLocation(-5000, -5000);
			m_pStatsValueLabel6->SetLocation(-5000, -5000);

			m_pStat1IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
			m_pStat2IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
			m_pStat3IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
			m_pStat4IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
			m_pStat5IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
			m_pStat6IncreaseButton->SetDimensions(-5000, -5000, 20, 20);
		}
	}

	if (m_pCharacterWindow->GetDimensions().x < 200)
	{
		m_statsTabLeftSide = false;
	}
	else if (m_pCharacterWindow->GetDimensions().x + m_pCharacterWindow->GetDimensions().width > m_windowWidth - 200)
	{
		m_statsTabLeftSide = true;
	}
}

void CharacterGUI::_CharacterItemEntered(void* pData)
{
	CharacterSlotItem* pCharacterSlotItem = static_cast<CharacterSlotItem*>(pData);
	pCharacterSlotItem->m_pCharacterGUI->CharacterItemEntered(pCharacterSlotItem);
}

void CharacterGUI::CharacterItemEntered(CharacterSlotItem* pCharacterItem)
{
	ShowTooltip(pCharacterItem);
}

void CharacterGUI::_CharacterItemExited(void* pData)
{
	CharacterSlotItem* pCharacterSlotItem = static_cast<CharacterSlotItem*>(pData);
	pCharacterSlotItem->m_pCharacterGUI->CharacterItemExited(pCharacterSlotItem);
}

void CharacterGUI::CharacterItemExited(CharacterSlotItem* pCharacterItem)
{
	HideTooltip();
}

void CharacterGUI::_CharacterItemPressed(void* pData)
{
	CharacterSlotItem* pCharacterSlotItem = static_cast<CharacterSlotItem*>(pData);
	pCharacterSlotItem->m_pCharacterGUI->CharacterItemPressed(pCharacterSlotItem);
}

void CharacterGUI::CharacterItemPressed(CharacterSlotItem* pCharacterItem)
{
	m_pPressedCharacterItem = pCharacterItem;

	Dimensions dimensions = m_pPressedCharacterItem->m_pInventoryIcon->GetDimensions();
	m_pressedX = dimensions.x;
	m_pressedY = dimensions.y;

	// Temporarily increase the depth of the dragged icon
	m_pPressedCharacterItem->m_pInventoryIcon->SetDepth(5.0f);
	//m_pPressedCharacterItem->m_pInventoryIcon->SetLocation(m_pressedX - 4, m_pressedY + 4);
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
	std::string dropShadowIcon = "Resources/textures/gui/" + themeName + "/common/items/drop_shadow.tga";
	m_pPressedCharacterItem->m_pInventoryIcon->AddIcon(m_pRenderer, dropShadowIcon.c_str(), 64, 64, 64, 64, 4, -4, 0.5f);
	m_pCharacterWindow->DepthSortComponentChildren();

	HideTooltip();
}

void CharacterGUI::_CharacterItemReleased(void* pData)
{
	CharacterSlotItem* pCharacterSlotItem = static_cast<CharacterSlotItem*>(pData);
	pCharacterSlotItem->m_pCharacterGUI->CharacterItemReleased(pCharacterSlotItem);
}

bool IsNeedErase(CharacterSlotItem* item)
{
	bool isNeedErase = item->m_erase;

	if (isNeedErase == true)
	{
		delete item->m_pInventoryIcon;
		delete item;
	}

	return isNeedErase;
}

void CharacterGUI::CharacterItemReleased(CharacterSlotItem* pCharacterItem)
{
	if (m_pPressedCharacterItem == nullptr)
	{
		return;
	}

	m_pPressedCharacterItem = nullptr;

	int x;
	int y;
	int width;
	int height;
	POINT mouse = { CubbyGame::GetInstance()->GetWindowCursorX(), (m_windowHeight - CubbyGame::GetInstance()->GetWindowCursorY()) };
	bool unequip = false;

	// Check if we have released on an inventory slot
	for (int i = 0; i < InventoryGUI::MAX_NUM_SLOTS_VERTICAL; ++i)
	{
		for (int j = 0; j < InventoryGUI::MAX_NUM_SLOTS_HORIZONTAL; ++j)
		{
			m_pInventoryGUI->GetInventoryDimensions(j, i, &x, &y, &width, &height);
			GUIWindow* pInventoryWindow = m_pInventoryGUI->GetInventoryWindow();

			if (mouse.x > pInventoryWindow->GetDimensions().x + x && mouse.x < pInventoryWindow->GetDimensions().x + x + width && mouse.y > pInventoryWindow->GetDimensions().y + y && mouse.y < pInventoryWindow->GetDimensions().y + y + height)
			{
				if (pCharacterItem->m_pInventoryItem->m_equipped == true)
				{
					// Check if another inventory item already exists in this slot
					InventorySlotItem* pInventorySlotItem = m_pInventoryGUI->GetInventorySlotItem(j, i);
					if (pInventorySlotItem == nullptr)
					{
						// We are unquipping an item that is in one of the equipment slots
						m_pInventoryManager->UnequipItem(j, i, pCharacterItem->m_pInventoryItem->m_equipSlot);

						m_pInventoryGUI->UnequipItem(pCharacterItem->m_pInventoryItem->m_equipSlot, pCharacterItem->m_pInventoryItem->m_left, pCharacterItem->m_pInventoryItem->m_right);

						// Set the new location for the released inventory icon
						pCharacterItem->m_slotX = j;
						pCharacterItem->m_slotY = i;
						pCharacterItem->m_pInventoryIcon->SetLocation(x, y);
						m_pActionBar->UpdateActionBarSlots(pCharacterItem->m_pInventoryItem, j, i);

						m_pCharacterWindow->RemoveComponent(pCharacterItem->m_pInventoryIcon);

						pCharacterItem->m_erase = true;
						m_vpInventorySlotItems.erase(remove_if(m_vpInventorySlotItems.begin(), m_vpInventorySlotItems.end(), IsNeedErase), m_vpInventorySlotItems.end());

						m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
						m_pInventoryManager->SetCharacterGUINeedsUpdate(true);

						m_pPlayer->RefreshStatModifierCacheValues();

						unequip = true;
					}
					else
					{
						if (pInventorySlotItem->m_pInventoryItem->m_equipSlot == pCharacterItem->m_pInventoryItem->m_equipSlot)
						{
							// Dual handed weapon checks
							if (pInventorySlotItem->m_pInventoryItem->m_equipSlot == EquipSlot::RightHand)
							{
								InventoryItem* pLeftHanded = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::LeftHand);
								if (pInventorySlotItem->m_pInventoryItem->m_left || (pLeftHanded != nullptr && pLeftHanded->m_right))
								{
									int slotX;
									int slotY;
									// Unequip the left hand slot since we are dual handed, OR the already equipped left hand item needs both hands
									m_pInventoryGUI->UnequipItem(EquipSlot::LeftHand, false, false);
									if (m_pInventoryManager->UnequipItemToFreeInventorySlot(EquipSlot::LeftHand, &slotX, &slotY) == false)
									{
										// We can't fit the other item in the inventory
									}
									else if (pLeftHanded != nullptr)
									{
										m_pActionBar->UpdateActionBarSlots(pLeftHanded, slotX, slotY);
									}
								}
							}
							if (pInventorySlotItem->m_pInventoryItem->m_equipSlot == EquipSlot::LeftHand)
							{
								InventoryItem* pRightHanded = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::RightHand);
								if (pInventorySlotItem->m_pInventoryItem->m_right || (pRightHanded != nullptr && pRightHanded->m_left))
								{
									int slotX;
									int slotY;
									// Unequip the right hand slot since we are dual handed, OR the already equipped right hand item needs both hands
									m_pInventoryGUI->UnequipItem(EquipSlot::RightHand, false, false);
									if (m_pInventoryManager->UnequipItemToFreeInventorySlot(EquipSlot::RightHand, &slotX, &slotY) == false)
									{
										// We can't fit the other item in the inventory
									}
									else if (pRightHanded != nullptr)
									{
										m_pActionBar->UpdateActionBarSlots(pRightHanded, slotX, slotY);
									}
								}
							}

							// We are swapping an equipped item for one in the inventory
							m_pInventoryGUI->UnequipItem(pCharacterItem->m_pInventoryItem->m_equipSlot, pCharacterItem->m_pInventoryItem->m_left, pCharacterItem->m_pInventoryItem->m_right);
							m_pInventoryManager->UnequipItem(j, i, pCharacterItem->m_pInventoryItem->m_equipSlot);

							// Equip the new item
							//m_pInventoryManager->EquipInventoryItem(pInventorySlotItem->m_slotX, pInventorySlotItem->m_slotY, pInventorySlotItem->m_pInventoryItem->m_equipSlot);
							m_pInventoryGUI->EquipItem(pInventorySlotItem);

							// Set the new location for the released inventory icon
							pCharacterItem->m_slotX = j;
							pCharacterItem->m_slotY = i;
							pCharacterItem->m_pInventoryIcon->SetLocation(x, y);
							m_pActionBar->UpdateActionBarSlots(pCharacterItem->m_pInventoryItem, j, i);

							m_pCharacterWindow->RemoveComponent(pCharacterItem->m_pInventoryIcon);

							pCharacterItem->m_erase = true;
							m_vpInventorySlotItems.erase(remove_if(m_vpInventorySlotItems.begin(), m_vpInventorySlotItems.end(), IsNeedErase), m_vpInventorySlotItems.end());

							m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
							m_pInventoryManager->SetCharacterGUINeedsUpdate(true);

							m_pPlayer->RefreshStatModifierCacheValues();

							unequip = true;
						}
					}
				}
			}
		}
	}

	if (unequip == false)
	{
		// Check if we have released on a loot slot
		if (m_pLootGUI->IsLoaded())
		{
			for (int i = 0; i < LootGUI::MAX_NUM_SLOTS_VERTICAL; ++i)
			{
				for (int j = 0; j < LootGUI::MAX_NUM_SLOTS_HORIZONTAL; ++j)
				{
					m_pLootGUI->GetLootDimensions(j, i, &x, &y, &width, &height);
					GUIWindow* pLootWindow = m_pLootGUI->GetLootWindow();
					// Check if we released (mouse cursor) in the boundary of another slot
					if (mouse.x > pLootWindow->GetDimensions().x + x && mouse.x < pLootWindow->GetDimensions().x + x + width && mouse.y > pLootWindow->GetDimensions().y + y && mouse.y < pLootWindow->GetDimensions().y + y + height)
					{
						if (m_pLootGUI->GetLootSlotItem(j, i) == NULL) // ONLY if an item doesn't already exist in the loot slot position
						{
							m_pInventoryGUI->UnequipItem(pCharacterItem->m_pInventoryItem->m_equipSlot, pCharacterItem->m_pInventoryItem->m_left, pCharacterItem->m_pInventoryItem->m_right);
							m_pInventoryManager->UnequipItemToLootGUI(pCharacterItem->m_pInventoryItem->m_equipSlot);

							m_pActionBar->RemoveInventoryItemFromActionBar(pCharacterItem->m_pInventoryItem->m_title);

							m_pLootGUI->AddLootItemFromInventory(pCharacterItem->m_pInventoryItem, j, i);

							m_pCharacterWindow->RemoveComponent(pCharacterItem->m_pInventoryIcon);

							pCharacterItem->m_erase = true;
							m_vpInventorySlotItems.erase(remove_if(m_vpInventorySlotItems.begin(), m_vpInventorySlotItems.end(), IsNeedErase), m_vpInventorySlotItems.end());

							m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
							m_pInventoryManager->SetCharacterGUINeedsUpdate(true);

							m_pPlayer->RefreshStatModifierCacheValues();

							unequip = true;
						}
					}
				}
			}
		}

		// Check if we released on a actionbar slot
		if (m_pActionBar->IsLoaded())
		{
			for (int i = 0; i < ActionBar::MAX_NUM_ACTION_SLOTS; ++i)
			{
				m_pActionBar->GetActionSlotDimensions(i, &x, &y, &width, &height);

				// Check if we released (mouse cursor) in the boundary of another slot
				if (mouse.x > x && mouse.x < x + width && mouse.y > y && mouse.y < y + height)
				{
					m_pActionBar->AddItemToActionBar(pCharacterItem->m_pInventoryItem, i, pCharacterItem->m_slotX, pCharacterItem->m_slotY);
					m_pActionBar->ExportActionBar(m_pPlayer->GetName());
				}
			}
		}
	}


	if (unequip == false)
	{
		// Revert depth back to normal for character icons
		pCharacterItem->m_pInventoryIcon->SetDepth(3.0f);
		m_pCharacterWindow->DepthSortComponentChildren();

		// Reset back to the original position
		pCharacterItem->m_pInventoryIcon->SetLocation(m_pressedX, m_pressedY);
		std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
		std::string dropShadowIcon = "Resources/textures/gui/" + themeName + "/common/items/drop_shadow.tga";
		pCharacterItem->m_pInventoryIcon->RemoveIcon(dropShadowIcon.c_str());

		// Figure out if we are still in the same slot, to re-show the tooltip
		int x2;
		int y2;
		int width2;
		int height2;
		switch (pCharacterItem->m_pInventoryItem->m_equipSlot)
		{
		case EquipSlot::NoSlot: { x2 = 0; y2 = 0; width2 = 0; height2 = 0; } break;
		case EquipSlot::RightHand: { GetPlayerWeaponRightDimensions(&x2, &y2, &width2, &height2); } break;
		case EquipSlot::LeftHand: { GetPlayerWeaponLeftDimensions(&x2, &y2, &width2, &height2); } break;
		case EquipSlot::Head: { GetPlayerHeadDimensions(&x2, &y2, &width2, &height2); } break;
		case EquipSlot::Shoulders: { GetPlayerShoulderDimensions(&x2, &y2, &width2, &height2); } break;
		case EquipSlot::Body: { GetPlayerBodyDimensions(&x2, &y2, &width2, &height2); } break;
		case EquipSlot::Legs: { GetPlayerLegsDimensions(&x2, &y2, &width2, &height2); } break;
		case EquipSlot::Hand: { GetPlayerHandDimensions(&x2, &y2, &width2, &height2); } break;
		case EquipSlot::Feet: { GetPlayerFeetDimensions(&x2, &y2, &width2, &height2); } break;
		case EquipSlot::Accessory1: { GetPlayerAccessory1Dimensions(&x2, &y2, &width2, &height2); } break;
		case EquipSlot::Accessory2: { GetPlayerAccessory2Dimensions(&x2, &y2, &width2, &height2); } break;
		case EquipSlot::NumSlots: { throw std::logic_error("Invalid EquipSlot in CharacterItemReleased()"); }
		default: { throw std::logic_error("Invalid EquipSlot in CharacterItemReleased()"); }
		}
		if (mouse.x > m_pCharacterWindow->GetDimensions().x + x2 && mouse.x < m_pCharacterWindow->GetDimensions().x + x2 + width2 && mouse.y > m_pCharacterWindow->GetDimensions().y + y2 && mouse.y < m_pCharacterWindow->GetDimensions().y + y2 + height2)
		{
			ShowTooltip(pCharacterItem);
		}
	}
}

void CharacterGUI::_CloseExitPressed(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->CloseExitPressed();
}

void CharacterGUI::CloseExitPressed()
{
	Unload();
}

void CharacterGUI::_OpenCloseTabPressed(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->OpenCloseTabPressed();
}

void CharacterGUI::OpenCloseTabPressed()
{
	m_statsTabsOpen = !m_statsTabsOpen;
}

void CharacterGUI::_ArrowLeftPressed(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->ArrowLeftPressed();
}

void CharacterGUI::ArrowLeftPressed()
{
	m_leftArrowPressed = true;
}

void CharacterGUI::_ArrowLeftReleased(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->ArrowLeftReleased();
}

void CharacterGUI::ArrowLeftReleased()
{
	m_leftArrowPressed = false;
}

void CharacterGUI::_ArrowRightPressed(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->ArrowRightPressed();
}

void CharacterGUI::ArrowRightPressed()
{
	m_rightArrowPressed = true;
}

void CharacterGUI::_ArrowRightReleased(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->ArrowRightReleased();
}

void CharacterGUI::ArrowRightReleased()
{
	m_rightArrowPressed = false;
}

void CharacterGUI::_EquipHoverAnimationFinished(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->EquipHoverAnimationFinished();
}

void CharacterGUI::EquipHoverAnimationFinished()
{
	StartEquipHoverAnimation();
}

void CharacterGUI::_Stat1IncreasePressed(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->Stat1IncreasePressed();
}

void CharacterGUI::Stat1IncreasePressed()
{
	m_pPlayer->GetPlayerStats()->SetStrengthStat(m_pPlayer->GetPlayerStats()->GetStrengthStat() + 1);
	m_pPlayer->GetPlayerStats()->SetNumPointsAvailable(m_pPlayer->GetPlayerStats()->GetNumPointsAvailable() - 1);
	UpdatePlayerStats();
}

void CharacterGUI::_Stat2IncreasePressed(void* pData)
{
	CharacterGUI* lpCharacterGUI = static_cast<CharacterGUI*>(pData);
	lpCharacterGUI->Stat2IncreasePressed();
}

void CharacterGUI::Stat2IncreasePressed()
{
	m_pPlayer->GetPlayerStats()->SetDexterityStat(m_pPlayer->GetPlayerStats()->GetDexterityStat() + 1);
	m_pPlayer->GetPlayerStats()->SetNumPointsAvailable(m_pPlayer->GetPlayerStats()->GetNumPointsAvailable() - 1);
	UpdatePlayerStats();
}

void CharacterGUI::_Stat3IncreasePressed(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->Stat3IncreasePressed();
}

void CharacterGUI::Stat3IncreasePressed()
{
	m_pPlayer->GetPlayerStats()->SetIntelligenceStat(m_pPlayer->GetPlayerStats()->GetIntelligenceStat() + 1);
	m_pPlayer->GetPlayerStats()->SetNumPointsAvailable(m_pPlayer->GetPlayerStats()->GetNumPointsAvailable() - 1);
	UpdatePlayerStats();
}

void CharacterGUI::_Stat4IncreasePressed(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->Stat4IncreasePressed();
}

void CharacterGUI::Stat4IncreasePressed()
{
	m_pPlayer->GetPlayerStats()->SetVitalityStat(m_pPlayer->GetPlayerStats()->GetVitalityStat() + 1);
	m_pPlayer->GetPlayerStats()->SetNumPointsAvailable(m_pPlayer->GetPlayerStats()->GetNumPointsAvailable() - 1);
	UpdatePlayerStats();
}

void CharacterGUI::_Stat5IncreasePressed(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->Stat5IncreasePressed();
}

void CharacterGUI::Stat5IncreasePressed()
{
	m_pPlayer->GetPlayerStats()->SetArmorStat(m_pPlayer->GetPlayerStats()->GetArmorStat() + 1);
	m_pPlayer->GetPlayerStats()->SetNumPointsAvailable(m_pPlayer->GetPlayerStats()->GetNumPointsAvailable() - 1);
	UpdatePlayerStats();
}

void CharacterGUI::_Stat6IncreasePressed(void* pData)
{
	CharacterGUI* pCharacterGUI = static_cast<CharacterGUI*>(pData);
	pCharacterGUI->Stat6IncreasePressed();
}

void CharacterGUI::Stat6IncreasePressed()
{
	m_pPlayer->GetPlayerStats()->SetLuckStat(m_pPlayer->GetPlayerStats()->GetLuckStat() + 1);
	m_pPlayer->GetPlayerStats()->SetNumPointsAvailable(m_pPlayer->GetPlayerStats()->GetNumPointsAvailable() - 1);
	UpdatePlayerStats();
}
