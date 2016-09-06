/*************************************************************************
> File Name: CraftingGUI.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Crafting GUI class.
> Created Time: 2016/08/31
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include <CubbyGame.h>

#include <Models/VoxelObject.h>
#include <Utils/Random.h>

#include "CraftingGUI.h"

// Constructor, Destructor
CraftingGUI::CraftingGUI(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight)
{
	m_pRenderer = pRenderer;
	m_pGUI = pGUI;
	m_pFrontendManager = pFrontendManager;
	m_pChunkManager = pChunkManager;
	m_pPlayer = pPlayer;
	m_pInventoryManager = pInventoryManager;

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	// Crafting Window
	m_pCraftingWindow = new GUIWindow(m_pRenderer, m_pFrontendManager->GetFrontendFontMedium(), "Crafting");
	m_pCraftingWindow->AllowMoving(true);
	m_pCraftingWindow->AllowClosing(false);
	m_pCraftingWindow->AllowMinimizing(false);
	m_pCraftingWindow->AllowScrolling(false);
	m_pCraftingWindow->SetRenderTitleBar(true);
	m_pCraftingWindow->SetRenderWindowBackground(true);
	m_pCraftingWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pCraftingWindow->Hide();

	m_pTitleBarIcon = new Icon(m_pRenderer, "", 44, 44);
	m_pTitleBarIcon->SetDepth(4.0f);

	m_pCraftingWindowBackgroundIcon = new Icon(m_pRenderer, "", 408, 314);
	m_pCraftingWindowBackgroundIcon->SetDepth(1.0f);

	m_pTitleBarBackgroundIcon = new Icon(m_pRenderer, "", 133, 35);
	m_pTitleBarBackgroundIcon->SetDepth(1.0f);

	m_pCloseExitButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCloseExitButton->SetLabelOffset(0, 5);
	m_pCloseExitButton->SetCallBackFunction(_CloseExitPressed);
	m_pCloseExitButton->SetCallBackData(this);

	m_pResultsScrollbar = new ScrollBar(m_pRenderer);
	m_pResultsScrollbar->SetScrollDirection(ScrollBarDirection::Vertical);
	m_pResultsScrollbar->SetScrollSize(0.25f);
	m_pResultsScrollbar->SetScrollPosition(1.0f);
	m_pResultsScrollbar->SetDepth(2.0f);

	m_pCraftButtonIcon = new Icon(m_pRenderer, "", 115, 32);
	m_pCraftButtonIcon->SetDepth(2.0f);
	m_pCraftButtonIconHover = new Icon(m_pRenderer, "", 115, 32);
	m_pCraftButtonIconHover->SetDepth(2.0f);
	m_pCraftButtonIconPressed = new Icon(m_pRenderer, "", 115, 32);
	m_pCraftButtonIconPressed->SetDepth(2.0f);
	m_pCraftButtonIconDisabled = new Icon(m_pRenderer, "", 115, 32);
	m_pCraftButtonIconDisabled->SetDepth(2.0f);

	m_pCraftButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25Outline(), "Craft", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCraftButton->SetLabelOffset(0, 3);
	m_pCraftButton->SetCallBackFunction(_CraftPressed);
	m_pCraftButton->SetCallBackData(this);
	m_pCraftButton->SetDepth(2.0f);
	m_pCraftButton->SetPressedOffset(0, -4);

	m_pCraftingProgressBarFiller = new Icon(m_pRenderer, "", 96, 14);
	m_pCraftingProgressBarFiller->SetDepth(1.0f);

	m_pCraftingProgressBarBackgroundFiller = new Icon(m_pRenderer, "", 53, 14);
	m_pCraftingProgressBarBackgroundFiller->SetDepth(1.5f);

	m_pCraftingProgressBar = new ProgressBar(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), 0.0f, 100.0f);
	m_pCraftingProgressBar->SetProgressFiller(m_pCraftingProgressBarFiller);
	m_pCraftingProgressBar->SetProgressBackground(m_pCraftingProgressBarBackgroundFiller);

	m_pSearchTextboxIcon = new Icon(m_pRenderer, "", 156, 24);
	m_pSearchTextboxIcon->SetDepth(1.0f);

	m_pSearchBox = new TextBox(m_pRenderer, m_pFrontendManager->GetFrontendFontMedium(), "", "");
	m_pSearchBox->SetBackgroundIcon(m_pSearchTextboxIcon);
	m_pSearchBox->SetTextIndent(0);
	m_pSearchBox->SetPipHeight(20);
	m_pSearchBox->SetPipeColor(Color(0.96f, 0.59f, 0.28f));
	m_pSearchBox->SetTextColor(Color(1.0f, 1.0f, 1.0f));
	m_pSearchBox->SetDepth(2.0f);

	m_pRecipeButtonIcon = new Icon(m_pRenderer, "", 152, 32);
	m_pRecipeButtonIcon->SetDepth(1.0f);

	m_pRecipeButtonHoverIcon = new Icon(m_pRenderer, "", 152, 32);
	m_pRecipeButtonHoverIcon->SetDepth(1.0f);

	m_pRecipeButtonPressedIcon = new Icon(m_pRenderer, "", 152, 32);
	m_pRecipeButtonPressedIcon->SetDepth(1.0f);

	m_pRecipeButtonSelectedIcon = new Icon(m_pRenderer, "", 152, 32);
	m_pRecipeButtonSelectedIcon->SetDepth(1.0f);

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
	m_pTooltipBackgroundCommon->SetDepth(7.5f);

	m_pTooltipBackgroundUncommon = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundUncommon->SetDepth(7.5f);

	m_pTooltipBackgroundMagical = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundMagical->SetDepth(7.5f);

	m_pTooltipBackgroundRare = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundRare->SetDepth(7.5f);

	m_pTooltipBackgroundEpic = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackgroundEpic->SetDepth(7.5f);

	char nameText[] = "[ITEM]";
	m_pTooltipNameLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), nameText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pTooltipNameLabel->SetOutline(true);
	m_pTooltipNameLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipNameLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont30Outline());
	m_pTooltipNameLabel->SetDepth(7.5f);

	char descText[] = "[REPLACE ME]";
	m_pTooltipDescriptionLabel = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), descText);
	m_pTooltipDescriptionLabel->SetOutline(true);
	m_pTooltipDescriptionLabel->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pTooltipDescriptionLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipDescriptionLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pTooltipDescriptionLabel->SetDepth(7.5f);
	m_pTooltipDescriptionLabel->SetWordWrap(true);

	char slotText[] = "[SLOT]";
	m_pTooltipSlotLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), slotText, Color(0.5f, 0.5f, 0.5f, 1.0f));
	m_pTooltipSlotLabel->SetOutline(true);
	m_pTooltipSlotLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipSlotLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont20Outline());
	m_pTooltipSlotLabel->SetDepth(7.5f);

	char qualityText[] = "[QUALITY]";
	m_pTooltipQualityLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), qualityText, Color(0.5f, 0.5f, 0.5f, 1.0f));
	m_pTooltipQualityLabel->SetOutline(true);
	m_pTooltipQualityLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipQualityLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont20Outline());
	m_pTooltipQualityLabel->SetDepth(7.5f);

	m_pCraftingWindow->SetBackgroundIcon(m_pCraftingWindowBackgroundIcon);
	m_pCraftingWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	m_pCraftingWindow->AddComponent(m_pTitleBarIcon);
	m_pCraftingWindow->AddComponent(m_pCloseExitButton);
	m_pCraftingWindow->AddComponent(m_pCraftButton);
	m_pCraftingWindow->AddComponent(m_pCraftingProgressBar);
	m_pCraftingWindow->AddComponent(m_pSearchBox);

	SetWindowDimensions(m_windowWidth, m_windowHeight);

	m_crafting = false;
	m_craftingTime = 2.5f;
	m_craftingTimer = 0.0f;

	m_pRecipeSlotItemSelected = nullptr;

	m_pInteractionItem = nullptr;

	// Load delay
	m_loadDelay = false;
	m_loadDelayTime = 0.0f;

	m_loaded = false;
}

CraftingGUI::~CraftingGUI()
{
	DeleteRecipeButtons();
	DeleteIngredientsButtons();
	DeleteCraftingRecipes();

	delete m_pCraftingWindow;
	delete m_pTitleBarIcon;
	delete m_pTitleBarBackgroundIcon;
	delete m_pCraftingWindowBackgroundIcon;
	delete m_pCloseExitButton;
	delete m_pResultsScrollbar;
	delete m_pCraftButtonIcon;
	delete m_pCraftButtonIconHover;
	delete m_pCraftButtonIconPressed;
	delete m_pCraftButtonIconDisabled;
	delete m_pCraftButton;
	delete m_pCraftingProgressBar;
	delete m_pCraftingProgressBarFiller;
	delete m_pCraftingProgressBarBackgroundFiller;
	delete m_pSearchBox;
	delete m_pSearchTextboxIcon;
	delete m_pRecipeButtonIcon;
	delete m_pRecipeButtonHoverIcon;
	delete m_pRecipeButtonPressedIcon;
	delete m_pRecipeButtonSelectedIcon;

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

void CraftingGUI::SetWindowDimensions(int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	m_craftingWindowWidth = 408;
	m_craftingWindowHeight = 314;
	m_titlebarWidth = 153;
	m_titlebarHeight = 35;
	m_craftingResultsScrollAreaX = -160;
	m_craftingResultsScrollAreaY = -4;
	m_craftingResultsScrollAreaWidth = 160;
	m_craftingResultsScrollAreaHeight = 238;
	m_craftingProgressX = 4;
	m_craftingProgressY = 4;
	m_craftingProgressWidth = 400;
	m_craftingProgressHeight = 20;

	m_pCraftingWindow->SetDimensions(75, 425, m_craftingWindowWidth, m_craftingWindowHeight);
	m_pCraftingWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);
	m_pCraftingWindow->SetTitleOffset(50, 5);
	m_pCraftingWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pCraftingWindow->SetApplicationBorder(25, 15, 10, 10);

	m_pTitleBarIcon->SetDimensions(0, m_craftingWindowHeight, 44, 44);

	m_pCloseExitButton->SetDimensions(m_craftingWindowWidth - 32, m_craftingWindowHeight, 32, 32);

	m_pResultsScrollbar->SetDimensions(175, 36, 24, m_craftingResultsScrollAreaHeight - 8);
	m_pResultsScrollbar->SetScrollArea(m_craftingResultsScrollAreaX, m_craftingResultsScrollAreaY, m_craftingResultsScrollAreaWidth, m_craftingResultsScrollAreaHeight);

	m_pFrontendManager->SetScrollbarIcons(m_pResultsScrollbar);

	m_pCraftButton->SetDimensions(284, 41, 115, 32);

	m_pCraftingProgressBar->SetDimensions(m_craftingProgressX, m_craftingProgressY, m_craftingProgressWidth, m_craftingProgressHeight);

	m_pSearchTextboxIcon->SetDimensions(0, 0, 156, 24);
	m_pSearchBox->SetDimensions(8, 282, 156, 24);
}

// Skinning the GUI
void CraftingGUI::SkinGUI() const
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

	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/craft_button.tga";
	m_pCraftButtonIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/craft_button_hover.tga";
	m_pCraftButtonIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/craft_button_pressed.tga";
	m_pCraftButtonIconPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/craft_button_disabled.tga";
	m_pCraftButtonIconDisabled->SetIcon(iconName);
	m_pCraftButton->SetDefaultIcon(m_pCraftButtonIcon);
	m_pCraftButton->SetHoverIcon(m_pCraftButtonIconHover);
	m_pCraftButton->SetSelectedIcon(m_pCraftButtonIconPressed);
	m_pCraftButton->SetDisabledIcon(m_pCraftButtonIconDisabled);

	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/craft_progress_filler.tga";
	m_pCraftingProgressBarFiller->SetIcon(iconName);;
	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/craft_progress_filler_background.tga";
	m_pCraftingProgressBarBackgroundFiller->SetIcon(iconName);

	m_pCraftingProgressBar->SetProgressFiller(m_pCraftingProgressBarFiller);
	m_pCraftingProgressBar->SetProgressBackground(m_pCraftingProgressBarBackgroundFiller);

	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/search_textbox.tga";
	m_pSearchTextboxIcon->SetIcon(iconName);

	m_pSearchBox->SetBackgroundIcon(m_pSearchTextboxIcon);

	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/recipe_item_button.tga";
	m_pRecipeButtonIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/recipe_item_hover_button.tga";
	m_pRecipeButtonHoverIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/recipe_item_pressed_button.tga";
	m_pRecipeButtonPressedIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/recipe_item_selected_button.tga";
	m_pRecipeButtonSelectedIcon->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/crafting_title_icon.tga";
	m_pTitleBarIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/crafting_window_background.tga";
	m_pCraftingWindowBackgroundIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/CraftingGUI/titlebar_background.tga";
	m_pTitleBarBackgroundIcon->SetIcon(iconName);

	m_pCraftingWindow->SetBackgroundIcon(m_pCraftingWindowBackgroundIcon);
	m_pCraftingWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	Point location = m_pCraftingWindow->GetLocation();
	m_pCraftingWindow->SetDimensions(location.x, location.y, m_craftingWindowWidth, m_craftingWindowHeight);
	m_pCraftingWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);

	m_pCloseExitButton->SetDefaultIcon(m_pFrontendManager->GetCloseExitButtonIcon());
	m_pCloseExitButton->SetHoverIcon(m_pFrontendManager->GetCloseExitButtonIconHover());
	m_pCloseExitButton->SetSelectedIcon(m_pFrontendManager->GetCloseExitButtonIconPressed());
	m_pCloseExitButton->SetDisabledIcon(m_pFrontendManager->GetCloseExitButtonIcon());

	m_pCraftButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pCraftButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pCraftButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CraftingGUI::UnSkinGUI() const
{
	// Do nothing
}

// Loading
void CraftingGUI::Load(bool loadDelay, float loadDelayTime)
{
	m_loadDelay = loadDelay;
	m_loadDelayTime = loadDelayTime;

	m_pCraftingWindow->AddComponent(m_pResultsScrollbar);

	m_crafting = false;
	m_craftingTime = 2.5f;
	m_craftingTimer = 0.0f;
	m_pCraftingProgressBar->SetCurrentValue(0.0f);

	m_pSearchBox->SetText("");

	m_pRecipeSlotItemSelected = nullptr;

	m_pInteractionItem = nullptr;

	CreateRecipeButtons();
	UpdateFilteredRecipes();

	UpdateCraftButton();

	m_pCraftingWindow->DepthSortComponentChildren();
	if (m_loadDelay == false)
	{
		m_pGUI->AddWindow(m_pCraftingWindow);
		m_pCraftingWindow->Show();
	}

	m_toolTipVisible = false;
	m_tooltipAppearDelayTimer = 0.0f;
	m_toolTipComponentsAdded = false;
	m_tooltipQuality = ItemQuality::Common;

	m_loaded = true;
}

void CraftingGUI::Unload()
{
	m_loadDelay = false;
	m_loadDelayTime = 0.0f;

	HideTooltip();

	DeleteIngredientsButtons();
	DeleteRecipeButtons();

	m_pCraftingWindow->RemoveComponent(m_pResultsScrollbar);

	m_pGUI->RemoveWindow(m_pCraftingWindow);

	m_pPlayer->SetCrafting(false);
	m_pPlayer->SetCraftingItem(false);
	m_pPlayer->DisableMoveToTargetPosition();

	if (m_pInteractionItem != nullptr)
	{
		m_pInteractionItem->SetCollisionEnabled(true);
	}

	CubbyGame::GetInstance()->CloseLetterBox();

	m_loaded = false;

	// Needs to be after setting loaded to false, so that we correctly turn the cursor back off
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

bool CraftingGUI::IsLoadDelayed() const
{
	return (m_loadDelay == true && m_loadDelayTime > 0.0f);
}

bool CraftingGUI::IsLoaded() const
{
	return m_loaded;
}

void CraftingGUI::CraftingComplete()
{
	m_pPlayer->SetCrafting(false);

	bool canCraft = CanCraftRecipe();

	if (canCraft)
	{
		// Remove the resource items from the inventory, since we have used them in the crafting process
		for (size_t i = 0; i < m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_vpCraftingItems.size() && canCraft; ++i)
		{
			m_pInventoryManager->RemoveInventoryItem(m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_vpCraftingItems[i]->m_title.c_str(), m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_vpCraftingItems[i]->m_item, m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_vpCraftingItems[i]->m_quantity);
		}

		// Add the new item to the inventory
		if (m_pInventoryManager->CanAddInventoryItem(m_pRecipeSlotItemSelected->m_pInventoryItem->m_title.c_str(), m_pRecipeSlotItemSelected->m_pInventoryItem->m_item, m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_pResultItem->m_quantity))
		{
			// Add to inventory
			m_pInventoryManager->AddInventoryItem(m_pRecipeSlotItemSelected->m_pInventoryItem, -1, -1);
		}
		else
		{
			// Drop the item in the world
			glm::vec3 vel = glm::vec3(GetRandomNumber(-1, 1, 2), 0.0f, GetRandomNumber(-1, 1, 2)) * GetRandomNumber(2, 3, 2);

			Item* pItem = CubbyGame::GetInstance()->GetItemManager()->CreateItem(m_pInteractionItem->GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), m_pRecipeSlotItemSelected->m_pInventoryItem->m_fileName.c_str(), ItemType::DroppedItem, m_pRecipeSlotItemSelected->m_pInventoryItem->m_title.c_str(), true, false, 0.08f);
			if (pItem != nullptr)
			{
				pItem->SetGravityDirection(glm::vec3(0.0f, -1.0f, 0.0f));
				pItem->SetVelocity(normalize(vel)*4.5f + glm::vec3(0.0f, 9.5f + GetRandomNumber(3, 6, 2), 0.0f));
				pItem->SetRotation(glm::vec3(0.0f, GetRandomNumber(0, 360, 2), 0.0f));
				pItem->SetAngularVelocity(glm::vec3(0.0f, 90.0f, 0.0f));
				pItem->SetDroppedItem(m_pRecipeSlotItemSelected->m_pInventoryItem->m_fileName.c_str(), m_pRecipeSlotItemSelected->m_pInventoryItem->m_iconFileName.c_str(),
					m_pRecipeSlotItemSelected->m_pInventoryItem->m_itemType, m_pRecipeSlotItemSelected->m_pInventoryItem->m_item, m_pRecipeSlotItemSelected->m_pInventoryItem->m_status,
					m_pRecipeSlotItemSelected->m_pInventoryItem->m_equipSlot, m_pRecipeSlotItemSelected->m_pInventoryItem->m_itemQuality, m_pRecipeSlotItemSelected->m_pInventoryItem->m_left,
					m_pRecipeSlotItemSelected->m_pInventoryItem->m_right, m_pRecipeSlotItemSelected->m_pInventoryItem->m_title.c_str(), m_pRecipeSlotItemSelected->m_pInventoryItem->m_description.c_str(),
					m_pRecipeSlotItemSelected->m_pInventoryItem->m_placementR, m_pRecipeSlotItemSelected->m_pInventoryItem->m_placementG, m_pRecipeSlotItemSelected->m_pInventoryItem->m_placementB,
					m_pRecipeSlotItemSelected->m_pInventoryItem->m_quantity);
				pItem->SetCollisionEnabled(false);

				for (size_t i = 0; i < m_pRecipeSlotItemSelected->m_pInventoryItem->m_vpStatAttributes.size(); ++i)
				{
					pItem->GetDroppedInventoryItem()->AddStatAttribute(m_pRecipeSlotItemSelected->m_pInventoryItem->m_vpStatAttributes[i]->GetType(), m_pRecipeSlotItemSelected->m_pInventoryItem->m_vpStatAttributes[i]->GetModifyAmount());
				}

				int numY = pItem->GetVoxelItem()->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix(0)->m_matrixSizeY;
				pItem->GetVoxelItem()->SetRenderOffset(glm::vec3(0.0f, numY*0.5f, 0.0f));
			}
		}
	}
	else
	{
	}

	for (size_t i = 0; i < m_vpRecipeSlotItem.size(); ++i)
	{
		if (m_vpRecipeSlotItem[i]->m_pResultsIcon == m_pRecipeSlotItemSelected->m_pResultsIcon)
		{
			continue;
		}

		m_vpRecipeSlotItem[i]->m_pResultsIcon->SetEnabled(true);
		m_vpRecipeSlotItem[i]->m_pResultsIcon->SetSelected(false);
		m_vpRecipeSlotItem[i]->m_pResultsIcon->SetHover(false);
	}

	UpdateCraftButton();
}

void CraftingGUI::SetInteractionitem(Item* pInteractionItem)
{
	m_pInteractionItem = pInteractionItem;

	if (m_pInteractionItem != nullptr)
	{
		m_pInteractionItem->SetCollisionEnabled(false);
	}
}

// Recipes
void CraftingGUI::SetCraftingRecipesForItem(ItemType craftingItem)
{
	DeleteCraftingRecipes();

	if (craftingItem == ItemType::Anvil)
	{
		// Pickaxe
		{
			CraftingRecipe* pPickaxeRecipe = new CraftingRecipe();
			InventoryItem* pStone = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::BlockStone, 3, ItemQuality::Common);
			InventoryItem* pWood = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::BlockWood, 4, ItemQuality::Common);
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 3, ItemQuality::Common);
			pPickaxeRecipe->m_vpCraftingItems.push_back(pStone);
			pPickaxeRecipe->m_vpCraftingItems.push_back(pWood);
			pPickaxeRecipe->m_vpCraftingItems.push_back(pIronBar);

			InventoryItem* pPickaxe = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::NormalPickaxe);
			pPickaxe->m_scale = 0.0f; pPickaxe->m_offsetX = 0.5f; pPickaxe->m_offsetY = 0.95f; pPickaxe->m_offsetZ = 0.5f;
			pPickaxeRecipe->m_pResultItem = pPickaxe;

			AddCraftingRecipe(pPickaxeRecipe);
		}

		// Hammer
		{
			CraftingRecipe* pHammerRecipe = new CraftingRecipe();
			InventoryItem* pStone = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::BlockStone, 4, ItemQuality::Common);
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 4, ItemQuality::Common);
			pHammerRecipe->m_vpCraftingItems.push_back(pStone);
			pHammerRecipe->m_vpCraftingItems.push_back(pIronBar);

			InventoryItem* pHammer = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::Hammer);
			pHammer->m_scale = 0.5f; pHammer->m_offsetX = 0.5f; pHammer->m_offsetY = 0.83f; pHammer->m_offsetZ = 0.5f;
			pHammerRecipe->m_pResultItem = pHammer;

			AddCraftingRecipe(pHammerRecipe);
		}

		// Iron Sword
		{
			CraftingRecipe* pIronSwordRecipe = new CraftingRecipe();
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 3, ItemQuality::Common);
			pIronSwordRecipe->m_vpCraftingItems.push_back(pIronBar);

			InventoryItem* pIronSword = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::IronSword);
			pIronSword->m_scale = 0.5f; pIronSword->m_offsetX = 0.5f; pIronSword->m_offsetY = 0.83f; pIronSword->m_offsetZ = 0.5f;
			pIronSwordRecipe->m_pResultItem = pIronSword;

			AddCraftingRecipe(pIronSwordRecipe);
		}

		// Iron Shield
		{
			CraftingRecipe* pIronShieldRecipe = new CraftingRecipe();
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 4, ItemQuality::Common);
			pIronShieldRecipe->m_vpCraftingItems.push_back(pIronBar);

			InventoryItem* pIronShield = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::IronShield);
			pIronShieldRecipe->m_pResultItem = pIronShield;

			AddCraftingRecipe(pIronShieldRecipe);
		}

		// Iron Helm
		{
			CraftingRecipe* pIronHelmRecipe = new CraftingRecipe();
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 10, ItemQuality::Common);
			pIronHelmRecipe->m_vpCraftingItems.push_back(pIronBar);

			InventoryItem* pIronHelm = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::IronHelm);
			pIronHelmRecipe->m_pResultItem = pIronHelm;

			AddCraftingRecipe(pIronHelmRecipe);
		}

		// Iron Armor
		{
			CraftingRecipe* pIronArmorRecipe = new CraftingRecipe();
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 10, ItemQuality::Common);
			pIronArmorRecipe->m_vpCraftingItems.push_back(pIronBar);

			InventoryItem* pIronArmor = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::IronArmor);
			pIronArmorRecipe->m_pResultItem = pIronArmor;

			AddCraftingRecipe(pIronArmorRecipe);
		}

		// Iron Pants
		{
			CraftingRecipe* pIronPantsRecipe = new CraftingRecipe();
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 10, ItemQuality::Common);
			pIronPantsRecipe->m_vpCraftingItems.push_back(pIronBar);

			InventoryItem* pIronPants = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::IronPants);
			pIronPantsRecipe->m_pResultItem = pIronPants;

			AddCraftingRecipe(pIronPantsRecipe);
		}

		// Iron Gloves
		{
			CraftingRecipe* pIronGlovesRecipe = new CraftingRecipe();
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 10, ItemQuality::Common);
			pIronGlovesRecipe->m_vpCraftingItems.push_back(pIronBar);

			InventoryItem* pIronGloves = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::IronGloves);
			pIronGlovesRecipe->m_pResultItem = pIronGloves;

			AddCraftingRecipe(pIronGlovesRecipe);
		}

		// Iron Boots
		{
			CraftingRecipe* pIronBootsRecipe = new CraftingRecipe();
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 10, ItemQuality::Common);
			pIronBootsRecipe->m_vpCraftingItems.push_back(pIronBar);

			InventoryItem* pIronBoots = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::IronBoots);
			pIronBootsRecipe->m_pResultItem = pIronBoots;

			AddCraftingRecipe(pIronBootsRecipe);
		}

		// Iron Shoulders
		{
			CraftingRecipe* pIronShouldersRecipe = new CraftingRecipe();
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 10, ItemQuality::Common);
			pIronShouldersRecipe->m_vpCraftingItems.push_back(pIronBar);

			InventoryItem* pIronShoulders = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::IronShoulders);
			pIronShouldersRecipe->m_pResultItem = pIronShoulders;

			AddCraftingRecipe(pIronShouldersRecipe);
		}

		// 2 Handed Sword
		{
			CraftingRecipe* p2HandedSwordRecipe = new CraftingRecipe();
			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 10, ItemQuality::Common);
			InventoryItem* pCopperBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::CopperBar, 5, ItemQuality::Common);
			InventoryItem* pWood = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::BlockWood, 3, ItemQuality::Common);
			p2HandedSwordRecipe->m_vpCraftingItems.push_back(pIronBar);
			p2HandedSwordRecipe->m_vpCraftingItems.push_back(pCopperBar);
			p2HandedSwordRecipe->m_vpCraftingItems.push_back(pWood);

			InventoryItem* p2HandedSword = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::TwoHandedSword);
			p2HandedSword->m_scale = 0.5f; p2HandedSword->m_offsetX = 0.5f; p2HandedSword->m_offsetY = 0.83f; p2HandedSword->m_offsetZ = 0.5f;
			p2HandedSwordRecipe->m_pResultItem = p2HandedSword;

			AddCraftingRecipe(p2HandedSwordRecipe);
		}
	}
	else if (craftingItem == ItemType::Furnace)
	{
		// Copper
		{
			CraftingRecipe* pCopperRecipe = new CraftingRecipe();
			InventoryItem* pCopperNugget = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::CopperOre, 1, ItemQuality::Common);
			pCopperRecipe->m_vpCraftingItems.push_back(pCopperNugget);

			InventoryItem* pCopperBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::CopperBar, 1, ItemQuality::Common);
			pCopperRecipe->m_pResultItem = pCopperBar;

			AddCraftingRecipe(pCopperRecipe);
		}

		// Iron
		{
			CraftingRecipe* pIronRecipe = new CraftingRecipe();
			InventoryItem* pIronNugget = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronOre, 2, ItemQuality::Common);
			pIronRecipe->m_vpCraftingItems.push_back(pIronNugget);

			InventoryItem* pIronBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::IronBar, 1, ItemQuality::Common);
			pIronRecipe->m_pResultItem = pIronBar;

			AddCraftingRecipe(pIronRecipe);
		}

		// Silver
		{
			CraftingRecipe* pSilverRecipe = new CraftingRecipe();
			InventoryItem* pSilverNugget = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::SilverOre, 3, ItemQuality::Common);
			pSilverRecipe->m_vpCraftingItems.push_back(pSilverNugget);

			InventoryItem* pSilverBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::SilverBar, 1, ItemQuality::Common);
			pSilverRecipe->m_pResultItem = pSilverBar;

			AddCraftingRecipe(pSilverRecipe);
		}

		// Gold
		{
			CraftingRecipe* pGoldRecipe = new CraftingRecipe();
			InventoryItem* pGoldNugget = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::GoldOre, 3, ItemQuality::Common);
			pGoldRecipe->m_vpCraftingItems.push_back(pGoldNugget);

			InventoryItem* pGoldBar = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::GoldBar, 1, ItemQuality::Common);
			pGoldRecipe->m_pResultItem = pGoldBar;

			AddCraftingRecipe(pGoldRecipe);
		}
	}
	else
	{
	}
}

void CraftingGUI::AddCraftingRecipe(CraftingRecipe* pRecipe)
{
	m_vpCraftingRecipes.push_back(pRecipe);
}

void CraftingGUI::DeleteCraftingRecipes()
{
	for (size_t i = 0; i < m_vpCraftingRecipes.size(); ++i)
	{
		delete m_vpCraftingRecipes[i];
		m_vpCraftingRecipes[i] = nullptr;
	}

	m_vpCraftingRecipes.clear();
}

void CraftingGUI::CreateRecipeButtons()
{
	int numToCreate = 50;

	for (int i = 0; i < numToCreate; ++i)
	{
		if (m_vpCraftingRecipes.size() == 0 || i > m_vpCraftingRecipes.size() - 1)
		{
			continue;
		}

		InventoryItem* pResultsItem = m_vpCraftingRecipes[i]->m_pResultItem;

		RecipeSlotItem* pSlotItem = new RecipeSlotItem();

		// Create the item icon
		Button* pNewResultsItem = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
		pNewResultsItem->SetDefaultIcon(m_pRecipeButtonIcon);
		pNewResultsItem->SetHoverIcon(m_pRecipeButtonHoverIcon);
		pNewResultsItem->SetSelectedIcon(m_pRecipeButtonPressedIcon);
		pNewResultsItem->SetDisabledIcon(m_pRecipeButtonIcon);
		pNewResultsItem->SetPressedOffset(0, -4);
		pNewResultsItem->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
		pNewResultsItem->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
		pNewResultsItem->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
		pNewResultsItem->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

		int xPos = -m_craftingResultsScrollAreaWidth;
		int yPos = m_craftingResultsScrollAreaHeight - ((i + 1) * 36);

		pNewResultsItem->SetDimensions(xPos, yPos, 152, 32);
		pNewResultsItem->SetDepth(2.0f);

		char itemTexture[128];
		sprintf(itemTexture, "%s", pResultsItem->m_iconFileName.c_str());
		pNewResultsItem->AddIcon(m_pRenderer, itemTexture, 64, 64, 24, 24, 4, 4, 2.5f);

		// Create the item label
		pNewResultsItem->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), pResultsItem->m_title, Color(1.0f, 1.0f, 1.0f, 1.0f), 34, 7, true, Color(0.0f, 0.0f, 0.0f, 1.0f));

		pNewResultsItem->SetCallBackFunction(_ResultsItemPressed);
		pNewResultsItem->SetCallBackData(pSlotItem);

		pNewResultsItem->SetEnterCallBackFunction(_RecipeItemEntered);
		pNewResultsItem->SetEnterCallBackData(pSlotItem);

		pNewResultsItem->SetExitCallBackFunction(_RecipeItemExited);
		pNewResultsItem->SetExitCallBackData(pSlotItem);

		pSlotItem->m_pInventoryItem = pResultsItem;
		pSlotItem->m_pCraftingGUI = this;
		pSlotItem->m_pResultsIcon = pNewResultsItem;
		pSlotItem->m_recipeName = pResultsItem->m_title;
		pSlotItem->m_pCraftingReceipe = m_vpCraftingRecipes[i];
		pSlotItem->m_slotIndex = i;
		pSlotItem->m_erase = false;

		m_vpRecipeSlotItem.push_back(pSlotItem);
	}
}

void CraftingGUI::DeleteRecipeButtons()
{
	m_pResultsScrollbar->ClearScrollAreaItems();

	for (size_t i = 0; i < m_vpRecipeSlotItem.size(); ++i)
	{
		m_pCraftingWindow->RemoveComponent(m_vpRecipeSlotItem[i]->m_pResultsIcon);
		delete m_vpRecipeSlotItem[i]->m_pResultsIcon;

		delete m_vpRecipeSlotItem[i];
		m_vpRecipeSlotItem[i] = nullptr;
	}

	m_vpRecipeSlotItem.clear();
}

void CraftingGUI::CreateIngredientsButtons()
{
	DeleteIngredientsButtons();

	if (m_pRecipeSlotItemSelected == nullptr)
	{
		return;
	}

	int ingredientsHorizontalIndex = 0;
	int ingredientsVerticalIndex = 0;

	m_pGUI->RemoveWindow(m_pCraftingWindow);

	for (size_t i = 0; i < m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_vpCraftingItems.size(); ++i)
	{
		InventoryItem* pInventoryItem = m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_vpCraftingItems[i];

		IngredientsSlotItem* pSlotItem = new IngredientsSlotItem();

		// Create the requirement button
		Button* pNewCraftingItem = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
		switch (pInventoryItem->m_itemQuality)
		{
		case ItemQuality::Common: { pNewCraftingItem->SetDefaultIcon(m_pInventoryBackgroundSlotBorderCommon); pNewCraftingItem->SetHoverIcon(m_pInventoryBackgroundSlotBorderCommon); pNewCraftingItem->SetSelectedIcon(m_pInventoryBackgroundSlotBorderCommon); break; }
		case ItemQuality::Uncommon: { pNewCraftingItem->SetDefaultIcon(m_pInventoryBackgroundSlotBorderUncommon); pNewCraftingItem->SetHoverIcon(m_pInventoryBackgroundSlotBorderUncommon); pNewCraftingItem->SetSelectedIcon(m_pInventoryBackgroundSlotBorderUncommon); break; }
		case ItemQuality::Magical: { pNewCraftingItem->SetDefaultIcon(m_pInventoryBackgroundSlotBorderMagical); pNewCraftingItem->SetHoverIcon(m_pInventoryBackgroundSlotBorderMagical); pNewCraftingItem->SetSelectedIcon(m_pInventoryBackgroundSlotBorderMagical); break; }
		case ItemQuality::Rare: { pNewCraftingItem->SetDefaultIcon(m_pInventoryBackgroundSlotBorderRare); pNewCraftingItem->SetHoverIcon(m_pInventoryBackgroundSlotBorderRare); pNewCraftingItem->SetSelectedIcon(m_pInventoryBackgroundSlotBorderRare); break; }
		case ItemQuality::Epic: { pNewCraftingItem->SetDefaultIcon(m_pInventoryBackgroundSlotBorderEpic); pNewCraftingItem->SetHoverIcon(m_pInventoryBackgroundSlotBorderEpic); pNewCraftingItem->SetSelectedIcon(m_pInventoryBackgroundSlotBorderEpic); break; }
		default: throw std::logic_error("Invalid ItemQuality in CreateIngredientsButtons()");
		}

		//pNewCraftingItem->SetDisabled(true);

		int xPos = 216 + (ingredientsHorizontalIndex * 62);
		int yPos = 246 - (ingredientsVerticalIndex * 62);

		pNewCraftingItem->SetDimensions(xPos, yPos, 54, 54);
		pNewCraftingItem->SetDepth(2.0f);

		char itemTexture[128];
		sprintf(itemTexture, "%s", pInventoryItem->m_iconFileName.c_str());
		pNewCraftingItem->AddIcon(m_pRenderer, itemTexture, 64, 64, 50, 50, 2, 2, 1.5f);

		std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
		switch (pInventoryItem->m_itemQuality)
		{
		case ItemQuality::Common:
		{
			std::string itemBackgroundIcon = "Resources/textures/gui/" + themeName + "/common/items/item_background_common.tga";
			pNewCraftingItem->AddIcon(m_pRenderer, itemBackgroundIcon.c_str(), 64, 64, 54, 54, 0, 0, 1.0f);
			break;
		}
		case ItemQuality::Uncommon:
		{
			std::string itemBackgroundIcon = "Resources/textures/gui/" + themeName + "/common/items/item_background_uncommon.tga";
			pNewCraftingItem->AddIcon(m_pRenderer, itemBackgroundIcon.c_str(), 64, 64, 54, 54, 0, 0, 1.0f);
			break;
		}
		case ItemQuality::Magical:
		{
			std::string itemBackgroundIcon = "Resources/textures/gui/" + themeName + "/common/items/item_background_magical.tga";
			pNewCraftingItem->AddIcon(m_pRenderer, itemBackgroundIcon.c_str(), 64, 64, 54, 54, 0, 0, 1.0f);
			break;
		}
		case ItemQuality::Rare:
		{
			std::string itemBackgroundIcon = "Resources/textures/gui/" + themeName + "/common/items/item_background_rare.tga";
			pNewCraftingItem->AddIcon(m_pRenderer, itemBackgroundIcon.c_str(), 64, 64, 54, 54, 0, 0, 1.0f);
			break;
		}
		case ItemQuality::Epic:
		{
			std::string itemBackgroundIcon = "Resources/textures/gui/" + themeName + "/common/items/item_background_epic.tga";
			pNewCraftingItem->AddIcon(m_pRenderer, itemBackgroundIcon.c_str(), 64, 64, 54, 54, 0, 0, 1.0f);
			break;
		}
		default: throw std::logic_error("Invalid ItemQuality in CreateIngredientsButtons()");
		}

		if (pInventoryItem->m_quantity != -1)
		{
			char quantity[128];
			sprintf(quantity, "%i", pInventoryItem->m_quantity);
			int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18(), "%s", quantity);
			pNewCraftingItem->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), quantity, Color(1.0f, 1.0f, 1.0f, 1.0f), 48 - textWidth, 4, true, Color(0.0f, 0.0f, 0.0f, 1.0f));
		}

		pNewCraftingItem->SetEnterCallBackFunction(_IngredientItemEntered);
		pNewCraftingItem->SetEnterCallBackData(pSlotItem);

		pNewCraftingItem->SetExitCallBackFunction(_IngredientItemExited);
		pNewCraftingItem->SetExitCallBackData(pSlotItem);

		pSlotItem->m_pInventoryItem = pInventoryItem;
		pSlotItem->m_pCraftingGUI = this;
		pSlotItem->m_pIngredientIcon = pNewCraftingItem;
		pSlotItem->m_slotXIndex = ingredientsHorizontalIndex;
		pSlotItem->m_slotYIndex = ingredientsVerticalIndex;
		pSlotItem->m_erase = false;

		ingredientsHorizontalIndex++;
		if (ingredientsHorizontalIndex == 3)
		{
			ingredientsVerticalIndex++;
			ingredientsHorizontalIndex = 0;
		}

		m_pCraftingWindow->AddComponent(pNewCraftingItem);

		m_vpIngredientsSlotItem.push_back(pSlotItem);
	}

	m_pGUI->AddWindow(m_pCraftingWindow);
	m_pCraftingWindow->Show();
}

void CraftingGUI::DeleteIngredientsButtons()
{
	for (size_t i = 0; i < m_vpIngredientsSlotItem.size(); ++i)
	{
		m_pCraftingWindow->RemoveComponent(m_vpIngredientsSlotItem[i]->m_pIngredientIcon);
		delete m_vpIngredientsSlotItem[i]->m_pIngredientIcon;

		delete m_vpIngredientsSlotItem[i];
		m_vpIngredientsSlotItem[i] = nullptr;
	}

	m_vpIngredientsSlotItem.clear();
}

bool CraftingGUI::CanCraftRecipe() const
{
	bool canCraft = true;

	if (m_pRecipeSlotItemSelected != nullptr)
	{
		for (size_t i = 0; i < m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_vpCraftingItems.size() && canCraft == true; ++i)
		{
			InventoryItem* pIntentoryItem = m_pInventoryManager->GetInventoryItemWithTitle(m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_vpCraftingItems[i]->m_title);

			if (pIntentoryItem == nullptr)
			{
				canCraft = false;
			}
			else
			{
				if (m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_vpCraftingItems[i]->m_quantity != -1)
				{
					if (pIntentoryItem->m_quantity < m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_vpCraftingItems[i]->m_quantity)
					{
						canCraft = false;
					}
				}
			}
		}
	}
	else
	{
		canCraft = false;
	}

	return canCraft;
}

// Tooltips
void CraftingGUI::UpdateToolTipAppear(float dt)
{
	if (m_toolTipVisible)
	{
		if (m_tooltipAppearDelayTimer <= 0.0f)
		{
			if (m_toolTipComponentsAdded == false)
			{
				switch (m_tooltipQuality)
				{
				case ItemQuality::Common: { m_pCraftingWindow->AddComponent(m_pTooltipBackgroundCommon); break; }
				case ItemQuality::Uncommon: { m_pCraftingWindow->AddComponent(m_pTooltipBackgroundUncommon); break; }
				case ItemQuality::Magical: { m_pCraftingWindow->AddComponent(m_pTooltipBackgroundMagical); break; }
				case ItemQuality::Rare: { m_pCraftingWindow->AddComponent(m_pTooltipBackgroundRare); break; }
				case ItemQuality::Epic: { m_pCraftingWindow->AddComponent(m_pTooltipBackgroundEpic); break; }
				default: throw std::logic_error("Invalid ItemQuality in UpdateToolTipAppear()");
				}

				m_pCraftingWindow->AddComponent(m_pTooltipNameLabel);
				m_pCraftingWindow->AddComponent(m_pTooltipDescriptionLabel);
				m_pCraftingWindow->AddComponent(m_pTooltipSlotLabel);
				m_pCraftingWindow->AddComponent(m_pTooltipQualityLabel);

				m_toolTipComponentsAdded = true;
			}
		}
		else
		{
			m_tooltipAppearDelayTimer -= dt;
		}
	}
}

void CraftingGUI::ShowTooltip(RecipeSlotItem* pRecipeItem)
{
	if (m_toolTipVisible == true)
	{
		return;
	}

	// Set the focused window when we show a tooltip
	m_pCraftingWindow->SetFocusWindow();

	// Replace the tooltip name
	m_pTooltipNameLabel->SetText(pRecipeItem->m_pInventoryItem->m_title);

	// Replace the tooltip description
	std::string DescriptionText = pRecipeItem->m_pInventoryItem->m_description + pRecipeItem->m_pInventoryItem->GetStatsAttributeString();
	m_pTooltipDescriptionLabel->SetText(DescriptionText);

	// Replace the tooltip equipslot text
	char slotText[32];
	switch (pRecipeItem->m_pInventoryItem->m_equipSlot)
	{
	case EquipSlot::NoSlot: { sprintf(slotText, ""); break; }
	case EquipSlot::LeftHand:
	{
		if (pRecipeItem->m_pInventoryItem->m_right)
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
		if (pRecipeItem->m_pInventoryItem->m_left)
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
	default: throw std::logic_error("Invalid EquipSlot in ShowTooltip()");
	}
	m_pTooltipSlotLabel->SetText(slotText);

	// Replace the tooltip quality text
	char qualityText[32];
	Color qualityColor;
	switch (pRecipeItem->m_pInventoryItem->m_itemQuality)
	{
	case ItemQuality::Common: { sprintf(qualityText, "Common"); qualityColor = Color(0.5f, 0.5f, 0.5f, 1.0f); break; }
	case ItemQuality::Uncommon: { sprintf(qualityText, "Uncommon"); qualityColor = Color(0.95f, 1.0f, 0.2f, 1.0f); break; }
	case ItemQuality::Magical: { sprintf(qualityText, "Magical"); qualityColor = Color(0.0f, 1.0f, 0.0f, 1.0f); break; }
	case ItemQuality::Rare: { sprintf(qualityText, "Rare"); qualityColor = Color(0.0f, 0.5f, 1.0f, 1.0f); break; }
	case ItemQuality::Epic: { sprintf(qualityText, "Epic"); qualityColor = Color(0.64f, 0.2f, 0.93f, 1.0f); break; }
	default: throw std::logic_error("Invalid ItemQuality in ShowTooltip()");
	}
	m_pTooltipQualityLabel->SetText(qualityText);
	m_pTooltipQualityLabel->SetColor(qualityColor);

	m_pTooltipNameLabel->SetColor(qualityColor);

	// Set tooltip dimensions
	m_tooltipWidth = 200;
	m_tooltipHeight = 220;
	m_tooltipDescBorder = 15;

	int x = pRecipeItem->m_pResultsIcon->GetLocation().x - m_craftingResultsScrollAreaX + 20;
	int y = pRecipeItem->m_pResultsIcon->GetLocation().y + 20;

	if (CubbyGame::GetInstance()->GetWindowCursorX() > m_windowWidth - m_tooltipWidth - 50)
	{
		x = x + 20 - m_tooltipWidth;
	}
	else
	{
		x = x + 15;
	}

	if ((m_windowHeight - CubbyGame::GetInstance()->GetWindowCursorY()) > m_windowHeight - m_tooltipHeight - 50)
	{
		y = y + 35 - m_tooltipHeight;
	}
	else
	{
		y = y + 20;
	}

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

	m_tooltipQuality = pRecipeItem->m_pInventoryItem->m_itemQuality;

	m_tooltipAppearDelayTimer = m_pFrontendManager->GetToolTipAppearDelay();

	m_toolTipVisible = true;
	m_toolTipComponentsAdded = false;
}

void CraftingGUI::ShowTooltip(IngredientsSlotItem* pIngredientItem)
{
	if (m_toolTipVisible == true)
	{
		return;
	}

	// Set the focused window when we show a tooltip
	m_pCraftingWindow->SetFocusWindow();

	// Replace the tooltip name
	m_pTooltipNameLabel->SetText(pIngredientItem->m_pInventoryItem->m_title);

	// Replace the tooltip description
	std::string DescriptionText = pIngredientItem->m_pInventoryItem->m_description + pIngredientItem->m_pInventoryItem->GetStatsAttributeString();
	m_pTooltipDescriptionLabel->SetText(DescriptionText);

	// Replace the tooltip equip slot text
	char slotText[32];
	switch (pIngredientItem->m_pInventoryItem->m_equipSlot)
	{
	case EquipSlot::NoSlot: { sprintf(slotText, ""); break; }
	case EquipSlot::LeftHand:
	{
		if (pIngredientItem->m_pInventoryItem->m_right)
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
		if (pIngredientItem->m_pInventoryItem->m_left)
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
	default: throw std::logic_error("Invalid EquipSlot in ShowTooltip()");
	}
	m_pTooltipSlotLabel->SetText(slotText);

	// Replace the tooltip quality text
	char qualityText[32];
	Color qualityColor;
	switch (pIngredientItem->m_pInventoryItem->m_itemQuality)
	{
	case ItemQuality::Common: { sprintf(qualityText, "Common"); qualityColor = Color(0.5f, 0.5f, 0.5f, 1.0f); break; }
	case ItemQuality::Uncommon: { sprintf(qualityText, "Uncommon"); qualityColor = Color(0.95f, 1.0f, 0.2f, 1.0f); break; }
	case ItemQuality::Magical: { sprintf(qualityText, "Magical"); qualityColor = Color(0.0f, 1.0f, 0.0f, 1.0f); break; }
	case ItemQuality::Rare: { sprintf(qualityText, "Rare"); qualityColor = Color(0.0f, 0.5f, 1.0f, 1.0f); break; }
	case ItemQuality::Epic: { sprintf(qualityText, "Epic"); qualityColor = Color(0.64f, 0.2f, 0.93f, 1.0f); break; }
	default: throw std::logic_error("Invalid ItemQuality in ShowTooltip()");
	}
	m_pTooltipQualityLabel->SetText(qualityText);
	m_pTooltipQualityLabel->SetColor(qualityColor);

	m_pTooltipNameLabel->SetColor(qualityColor);

	// Set tooltip dimensions
	m_tooltipWidth = 200;
	m_tooltipHeight = 220;
	m_tooltipDescBorder = 15;

	int x = pIngredientItem->m_pIngredientIcon->GetLocation().x;
	int y = pIngredientItem->m_pIngredientIcon->GetLocation().y;

	if (CubbyGame::GetInstance()->GetWindowCursorX() > m_windowWidth - m_tooltipWidth - 50)
	{
		x = x + 20 - m_tooltipWidth;
	}
	else
	{
		x = x + 15;
	}
	if ((m_windowHeight - CubbyGame::GetInstance()->GetWindowCursorY()) > m_windowHeight - m_tooltipHeight - 50)
	{
		y = y + 20 - m_tooltipHeight;
	}
	else
	{
		y = y + 20;
	}

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

	m_tooltipQuality = pIngredientItem->m_pInventoryItem->m_itemQuality;

	m_tooltipAppearDelayTimer = m_pFrontendManager->GetToolTipAppearDelay();

	m_toolTipVisible = true;
	m_toolTipComponentsAdded = false;
}

void CraftingGUI::HideTooltip()
{
	m_pCraftingWindow->RemoveComponent(m_pTooltipBackgroundCommon);
	m_pCraftingWindow->RemoveComponent(m_pTooltipBackgroundUncommon);
	m_pCraftingWindow->RemoveComponent(m_pTooltipBackgroundMagical);
	m_pCraftingWindow->RemoveComponent(m_pTooltipBackgroundRare);
	m_pCraftingWindow->RemoveComponent(m_pTooltipBackgroundEpic);
	m_pCraftingWindow->RemoveComponent(m_pTooltipNameLabel);
	m_pCraftingWindow->RemoveComponent(m_pTooltipDescriptionLabel);
	m_pCraftingWindow->RemoveComponent(m_pTooltipSlotLabel);
	m_pCraftingWindow->RemoveComponent(m_pTooltipQualityLabel);

	m_toolTipVisible = false;
}

// Updating
void CraftingGUI::Update(float dt)
{
	if (m_loadDelay == true)
	{
		if (m_loadDelayTime <= 0.0f)
		{
			m_loadDelay = false;
			m_pGUI->AddWindow(m_pCraftingWindow);
			m_pCraftingWindow->Show();
		}
		else
		{
			m_loadDelayTime -= dt;
		}
	}

	UpdateToolTipAppear(dt);

	if (CubbyGame::GetInstance()->IsPaused() == false)
	{
		if (m_crafting)
		{
			m_craftingTimer += dt;

			if (m_craftingTimer > m_craftingTime)
			{
				m_craftingTimer = m_craftingTime;

				m_crafting = false;
				CraftingComplete();
			}

			m_pCraftingProgressBar->SetCurrentValue((m_craftingTimer / m_craftingTime) * 100.0f);
		}
	}

	// Update the filtered
	if (strcmp(m_cachedSearchText.c_str(), m_pSearchBox->GetText().c_str()) != 0)
	{
		m_cachedSearchText = m_pSearchBox->GetText();
		UpdateFilteredRecipes();
	}

	// Update the results items based on the scroll position and scroll ratio
	int visibleSize = m_pResultsScrollbar->GetScrollArea().height;
	int neededHeight = (static_cast<int>(m_vpRecipeSlotItemFiltered.size()) * 36) + 4;
	int heightDiff = neededHeight - visibleSize;

	if (heightDiff > 0)
	{
		m_pResultsScrollbar->SetScrollSize(static_cast<float>(visibleSize) / static_cast<float>(neededHeight));
		m_pResultsScrollbar->SetDisabled(false);

		float scrollRatio = m_pResultsScrollbar->GetScrollRatio();
		int offsetButtonY = static_cast<int>(heightDiff * scrollRatio);

		for (size_t i = 0; i < m_vpRecipeSlotItemFiltered.size(); ++i)
		{
			int xPos = -m_craftingResultsScrollAreaWidth;
			int yPos = 4 + m_pResultsScrollbar->GetScrollArea().y + m_pResultsScrollbar->GetScrollArea().height - 4 - ((i + 1) * 36);

			m_vpRecipeSlotItemFiltered[i]->m_pResultsIcon->SetLocation(xPos, yPos + offsetButtonY);
		}
	}
	else
	{
		m_pResultsScrollbar->SetScrollSize(0.0f);
		m_pResultsScrollbar->SetDisabled(true);
	}
}

void CraftingGUI::UpdateCraftButton() const
{
	bool canCraft = CanCraftRecipe();

	if (m_crafting == false && canCraft)
	{
		m_pCraftButton->SetDisabled(false);
		m_pCraftButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
		m_pCraftButton->SetLabelOutlineColor(Color(0.0f, 0.0f, 0.0f));
	}
	else
	{
		m_pCraftButton->SetDisabled(true);
		m_pCraftButton->SetLabelColor(m_pFrontendManager->GetDisabledFontColor());
		m_pCraftButton->SetLabelOutlineColor(Color(0.25f, 0.25f, 0.25f));
	}
}

void CraftingGUI::UpdateFilteredRecipes()
{
	m_pResultsScrollbar->ClearScrollAreaItems();

	m_vpRecipeSlotItemFiltered.clear();
	for (size_t i = 0; i < m_vpRecipeSlotItem.size(); ++i)
	{
		std::string lowerSearchBox = m_pSearchBox->GetText();
		transform(lowerSearchBox.begin(), lowerSearchBox.end(), lowerSearchBox.begin(), tolower);
		std::string lowerRecipeName = m_vpRecipeSlotItem[i]->m_recipeName;
		transform(lowerRecipeName.begin(), lowerRecipeName.end(), lowerRecipeName.begin(), tolower);

		bool foundString = false;
		if (lowerRecipeName.find(lowerSearchBox) != std::string::npos)
		{
			foundString = true;
		}

		for (size_t j = 0; j < m_vpRecipeSlotItem[i]->m_pCraftingReceipe->m_vpCraftingItems.size() && foundString == false; ++j)
		{
			std::string lowerIngredientName = m_vpRecipeSlotItem[i]->m_pCraftingReceipe->m_vpCraftingItems[j]->m_title;
			transform(lowerIngredientName.begin(), lowerIngredientName.end(), lowerIngredientName.begin(), tolower);

			if (lowerIngredientName.find(lowerSearchBox) != std::string::npos)
			{
				foundString = true;
			}
		}

		if (lowerSearchBox == "" || foundString)
		{
			m_vpRecipeSlotItemFiltered.push_back(m_vpRecipeSlotItem[i]);
		}
	}

	bool stillContainsSelected = false;

	for (size_t i = 0; i < m_vpRecipeSlotItemFiltered.size(); ++i)
	{
		int xPos = -m_craftingResultsScrollAreaWidth;
		int yPos = m_craftingResultsScrollAreaHeight - ((i + 1) * 36);

		m_vpRecipeSlotItemFiltered[i]->m_pResultsIcon->SetDimensions(xPos, yPos, 152, 32);
		m_vpRecipeSlotItemFiltered[i]->m_slotIndex = i;

		m_pResultsScrollbar->AddScrollAreaItem(m_vpRecipeSlotItemFiltered[i]->m_pResultsIcon);

		if (m_pRecipeSlotItemSelected != nullptr)
		{
			if (strcmp(m_pRecipeSlotItemSelected->m_pCraftingReceipe->m_pResultItem->m_title.c_str(), m_vpRecipeSlotItemFiltered[i]->m_recipeName.c_str()) == 0)
			{
				stillContainsSelected = true;
			}
		}
	}

	if (stillContainsSelected == false)
	{
		// Reset pressed button
		ResultsItemPressed(nullptr);
	}
}

// Rendering
// ReSharper disable once CppMemberFunctionMayBeStatic
void CraftingGUI::Render() const
{

}

void CraftingGUI::_CloseExitPressed(void* pData)
{
	CraftingGUI* pCraftingGUI = static_cast<CraftingGUI*>(pData);
	pCraftingGUI->CloseExitPressed();
}

void CraftingGUI::CloseExitPressed()
{
	Unload();
}

void CraftingGUI::_RecipeItemEntered(void* pData)
{
	RecipeSlotItem* pRecipeSlotItem = static_cast<RecipeSlotItem*>(pData);
	pRecipeSlotItem->m_pCraftingGUI->RecipeItemEntered(pRecipeSlotItem);
}

void CraftingGUI::RecipeItemEntered(RecipeSlotItem* pRecipeButtonData)
{
	ShowTooltip(pRecipeButtonData);
}

void CraftingGUI::_RecipeItemExited(void* pData)
{
	RecipeSlotItem* pRecipeSlotItem = static_cast<RecipeSlotItem*>(pData);
	pRecipeSlotItem->m_pCraftingGUI->RecipeItemExited(pRecipeSlotItem);
}

void CraftingGUI::RecipeItemExited(RecipeSlotItem* pRecipeButtonData)
{
	HideTooltip();
}

void CraftingGUI::_IngredientItemEntered(void* pData)
{
	IngredientsSlotItem* pIngredientsSlotItem = static_cast<IngredientsSlotItem*>(pData);
	pIngredientsSlotItem->m_pCraftingGUI->IngredientItemEntered(pIngredientsSlotItem);
}

void CraftingGUI::IngredientItemEntered(IngredientsSlotItem* pIngredientButtonData)
{
	ShowTooltip(pIngredientButtonData);
}

void CraftingGUI::_IngredientItemExited(void* pData)
{
	IngredientsSlotItem* pIngredientsSlotItem = static_cast<IngredientsSlotItem*>(pData);
	pIngredientsSlotItem->m_pCraftingGUI->IngredientItemExited(pIngredientsSlotItem);
}

void CraftingGUI::IngredientItemExited(IngredientsSlotItem* pIngredientButtonData)
{
	HideTooltip();
}

void CraftingGUI::_CraftPressed(void* pData)
{
	CraftingGUI* pCraftingGUI = static_cast<CraftingGUI*>(pData);
	pCraftingGUI->CraftPressed(false);
}

void CraftingGUI::CraftPressed(bool fakeCraft)
{
	if (m_crafting == false)
	{
		m_crafting = true;
		m_craftingTime = 3.5f;
		m_craftingTimer = 0.0f;

		m_pPlayer->SetCrafting(true);
		m_pPlayer->SetCraftingItem(true);

		for (size_t i = 0; i < m_vpRecipeSlotItemFiltered.size(); ++i)
		{
			m_vpRecipeSlotItemFiltered[i]->m_pResultsIcon->SetEnabled(false);
		}

		UpdateCraftButton();
	}
}

void CraftingGUI::_ResultsItemPressed(void* pData)
{
	RecipeSlotItem* pRecipeSlotItem = static_cast<RecipeSlotItem*>(pData);
	pRecipeSlotItem->m_pCraftingGUI->ResultsItemPressed(pRecipeSlotItem);
}

void CraftingGUI::ResultsItemPressed(RecipeSlotItem* pRecipeButtonData)
{
	// First reset previous selected
	if (m_pRecipeSlotItemSelected != nullptr)
	{
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetDefaultIcon(m_pRecipeButtonIcon);
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetHoverIcon(m_pRecipeButtonHoverIcon);
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetSelectedIcon(m_pRecipeButtonPressedIcon);
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetEnabled(true);
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetSelected(false);
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetHover(false);
	}

	// Assign new
	m_pRecipeSlotItemSelected = pRecipeButtonData;

	if (m_pRecipeSlotItemSelected != nullptr)
	{
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetDefaultIcon(m_pRecipeButtonSelectedIcon);
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetHoverIcon(m_pRecipeButtonSelectedIcon);
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetSelectedIcon(m_pRecipeButtonSelectedIcon);
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetLabelColor(Color(0.73f, 0.26f, 0.47f));
		m_pRecipeSlotItemSelected->m_pResultsIcon->SetEnabled(false);
	}

	CreateIngredientsButtons();

	UpdateCraftButton();

	HideTooltip();
}