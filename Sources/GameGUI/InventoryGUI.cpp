/*************************************************************************
> File Name: InventoryGUI.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Inventory GUI class.
> Created Time: 2016/09/01
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include <CubbyGame.h>

#include <Models/VoxelObject.h>
#include <Utils/Random.h>

#include "ActionBar.h"
#include "InventoryGUI.h"

// Constructor, Destructor
InventoryGUI::InventoryGUI(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight)
{
	m_pRenderer = pRenderer;
	m_pGUI = pGUI;
	m_pFrontendManager = pFrontendManager;
	m_pChunkManager = pChunkManager;
	m_pPlayer = pPlayer;
	m_pInventoryManager = pInventoryManager;

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	// Inventory Window
	m_pInventoryWindow = new GUIWindow(m_pRenderer, m_pFrontendManager->GetFrontendFontMedium(), "Inventory");
	m_pInventoryWindow->AllowMoving(true);
	m_pInventoryWindow->AllowClosing(false);
	m_pInventoryWindow->AllowMinimizing(false);
	m_pInventoryWindow->AllowScrolling(false);
	m_pInventoryWindow->SetRenderTitleBar(true);
	m_pInventoryWindow->SetRenderWindowBackground(true);
	m_pInventoryWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pInventoryWindow->Hide();

	m_pTitleBarIcon = new Icon(m_pRenderer, "", 44, 44);
	m_pTitleBarIcon->SetDepth(4.0f);

	m_pInventoryWindowBackgroundIcon = new Icon(m_pRenderer, "", 400, 211);
	m_pInventoryWindowBackgroundIcon->SetDepth(1.0f);

	m_pTitleBarBackgroundIcon = new Icon(m_pRenderer, "", 133, 35);
	m_pTitleBarBackgroundIcon->SetDepth(1.0f);

	m_pCloseExitButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCloseExitButton->SetLabelOffset(0, 5);
	m_pCloseExitButton->SetCallBackFunction(_CloseExitPressed);
	m_pCloseExitButton->SetCallBackData(this);

	m_pDestroyIcon = new Icon(m_pRenderer, "", 175, 65);
	m_pDestroyIcon->SetDepth(2.1f);

	char destroyText[] = "DESTROY";
	m_pDestroyLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont50(), destroyText, Color(1.0f, 1.0f, 1.0f, 0.25f));
	m_pDestroyLabel->SetOutline(true);
	m_pDestroyLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pDestroyLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont50Outline());
	m_pDestroyLabel->SetDepth(3.0f);

	m_pDropIcon = new Icon(m_pRenderer, "", 175, 65);
	m_pDropIcon->SetDepth(2.1f);

	char dropText[] = "DROP";
	m_pDropLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont50(), dropText, Color(1.0f, 1.0f, 1.0f, 0.25f));
	m_pDropLabel->SetOutline(true);
	m_pDropLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pDropLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont50Outline());
	m_pDropLabel->SetDepth(3.0f);

	m_pInventoryWindow->SetBackgroundIcon(m_pInventoryWindowBackgroundIcon);
	m_pInventoryWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	m_pInventoryWindow->AddComponent(m_pTitleBarIcon);
	m_pInventoryWindow->AddComponent(m_pCloseExitButton);

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

	// Popup
	char popupTitleText[] = "[POPUP TITLE]";
	m_popupTitle = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont40(), popupTitleText, Color(1.0f, 0.0f, 0.0f, 1.0f));
	m_popupTitle->SetOutline(true);
	m_popupTitle->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_popupTitle->SetOutlineFont(m_pFrontendManager->GetFrontendFont40Outline());
	m_popupTitle->SetDepth(9.0f);

	char popupText[] = "[POPUP TEXT]";
	m_popupText = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), popupText);
	m_popupText->SetOutline(true);
	m_popupText->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_popupText->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_popupText->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_popupText->SetDepth(9.0f);
	m_popupText->SetWordWrap(true);
	m_popupText->SetHorizontalAlignment(HorizontalAlignment::Center);

	m_pPopupBackgroundIcon = new Icon(m_pRenderer, "", 270, 200);
	m_pPopupBackgroundIcon->SetDepth(2.0f);

	m_pPopupConfirmButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "Yes", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pPopupConfirmButton->SetLabelOffset(0, 3);
	m_pPopupConfirmButton->SetPressedOffset(0, -4);
	m_pPopupConfirmButton->SetCallBackFunction(_PopupConfirmPressed);
	m_pPopupConfirmButton->SetCallBackData(this);
	m_pPopupConfirmButton->SetDepth(9.0f);

	m_pPopupCancelButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "No", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pPopupCancelButton->SetLabelOffset(0, 3);
	m_pPopupCancelButton->SetPressedOffset(0, -4);
	m_pPopupCancelButton->SetCallBackFunction(_PopupCancelPressed);
	m_pPopupCancelButton->SetCallBackData(this);
	m_pPopupCancelButton->SetDepth(9.1f);

	SetWindowDimensions(m_windowWidth, m_windowHeight);

	m_pInventoryItemToDelete = nullptr;

	// Load delay
	m_loadDelay = false;
	m_loadDelayTime = 0.0f;

	m_loaded = false;
}

InventoryGUI::~InventoryGUI()
{
	delete m_pInventoryWindow;
	delete m_pTitleBarIcon;
	delete m_pTitleBarBackgroundIcon;
	delete m_pInventoryWindowBackgroundIcon;
	delete m_pCloseExitButton;
	delete m_pDestroyIcon;
	delete m_pDestroyLabel;
	delete m_pDropIcon;
	delete m_pDropLabel;

	delete m_pInventoryBackgroundSlotBorderCommon;
	delete m_pInventoryBackgroundSlotBorderUncommon;
	delete m_pInventoryBackgroundSlotBorderMagical;
	delete m_pInventoryBackgroundSlotBorderRare;
	delete m_pInventoryBackgroundSlotBorderEpic;

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

	// Popup
	delete m_popupTitle;
	delete m_popupText;
	delete m_pPopupConfirmButton;
	delete m_pPopupCancelButton;
	delete m_pPopupBackgroundIcon;
}

void InventoryGUI::SetCharacterGUI(CharacterGUI* pCharacterGUI)
{
	m_pCharacterGUI = pCharacterGUI;
}

void InventoryGUI::SetLootGUI(LootGUI* pLootGUI)
{
	m_pLootGUI = pLootGUI;
}

void InventoryGUI::SetActionBar(ActionBar* pActionBar)
{
	m_pActionBar = pActionBar;
}

void InventoryGUI::SetItemManager(ItemManager *pItemManager)
{
	m_pItemManager = pItemManager;
}

// Skinning the GUI
void InventoryGUI::SkinGUI()
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/InventoryGUI/inventory_title_icon.tga";
	m_pTitleBarIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/InventoryGUI/inventory_window_background.tga";
	m_pInventoryWindowBackgroundIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/InventoryGUI/titlebar_background.tga";
	m_pTitleBarBackgroundIcon->SetIcon(iconName);

	m_pInventoryWindow->SetBackgroundIcon(m_pInventoryWindowBackgroundIcon);
	m_pInventoryWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	Point location = m_pInventoryWindow->GetLocation();
	m_pInventoryWindow->SetDimensions(location.x, location.y, m_inventoryWindowWidth, m_inventoryWindowHeight);
	m_pInventoryWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);

	iconName = "Resources/textures/gui/" + themeName + "/InventoryGUI/delete_background.tga";
	m_pDestroyIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/InventoryGUI/drop_background.tga";
	m_pDropIcon->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/common/popup_background.tga";
	m_pPopupBackgroundIcon->SetIcon(iconName);

	m_pFrontendManager->SetButtonIcons(m_pPopupConfirmButton, ButtonSize::Size110x47);
	m_pFrontendManager->SetButtonIcons(m_pPopupCancelButton, ButtonSize::Size110x47);

	m_pCloseExitButton->SetDefaultIcon(m_pFrontendManager->GetCloseExitButtonIcon());
	m_pCloseExitButton->SetHoverIcon(m_pFrontendManager->GetCloseExitButtonIconHover());
	m_pCloseExitButton->SetSelectedIcon(m_pFrontendManager->GetCloseExitButtonIconPressed());
	m_pCloseExitButton->SetDisabledIcon(m_pFrontendManager->GetCloseExitButtonIcon());

	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_common.tga";
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

	m_pPopupConfirmButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pPopupConfirmButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pPopupConfirmButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pPopupCancelButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pPopupCancelButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pPopupCancelButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void InventoryGUI::UnSkinGUI() const
{
	// Do nothing
}

// Loading
void InventoryGUI::Load(bool loadDelay, float loadDelayTime)
{
	m_loadDelay = loadDelay;
	m_loadDelayTime = loadDelayTime;

	if (m_pInventoryManager->InventoryGUINeedsUpdate())
	{
		DeleteInventoryItems();
		CreateInventoryItems();
		UpdateActionBar();

		for (size_t i = 0; i < m_vpInventorySlotItems.size(); ++i)
		{
			m_pInventoryWindow->AddComponent(m_vpInventorySlotItems[i]->m_pInventoryIcon);
		}
	}

	if (m_loadDelay == false)
	{
		m_pGUI->AddWindow(m_pInventoryWindow);
		m_pInventoryWindow->Show();
	}

	m_pPopupConfirmButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pPopupCancelButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());

	m_pressedX = 0;
	m_pressedY = 0;
	m_pPressedInventoryItem = nullptr;

	m_toolTipVisible = false;
	m_tooltipAppearDelayTimer = 0.0f;
	m_toolTipComponentsAdded = false;
	m_tooltipQuality = ItemQuality::Common;

	m_pInventoryItemToDelete = nullptr;

	m_loaded = true;
}

void InventoryGUI::Unload()
{
	m_loadDelay = false;
	m_loadDelayTime = 0.0f;

	HideTooltip();

	m_pGUI->RemoveWindow(m_pInventoryWindow);

	if (m_pPressedInventoryItem != nullptr)
	{
		m_pPressedInventoryItem->m_pInventoryIcon->SetDepth(3.0f);
		m_pInventoryWindow->DepthSortComponentChildren();
		m_pPressedInventoryItem->m_pInventoryIcon->SetLocation(m_pressedX, m_pressedY);
	}

	ClosePopup();

	m_pGUI->RemoveComponent(m_pPopupBackgroundIcon);

	m_pInventoryWindow->RemoveComponent(m_pDestroyIcon);
	m_pInventoryWindow->RemoveComponent(m_pDestroyLabel);
	m_pInventoryWindow->RemoveComponent(m_pDropIcon);
	m_pInventoryWindow->RemoveComponent(m_pDropLabel);

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

bool InventoryGUI::IsLoadDelayed() const
{
	return (m_loadDelay == true && m_loadDelayTime > 0.0f);
}

void InventoryGUI::SetWindowDimensions(int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	m_inventoryWindowWidth = 412;
	m_inventoryWindowHeight = 212;
	m_titlebarWidth = 153;
	m_titlebarHeight = 35;

	m_popupWidth = 270;
	m_popupHeight = 200;
	m_popupBorderSpacer = 25;
	m_popupTitleSpacer = 35;
	m_popupIconSize = 50;
	m_popupIconSpacer = 10;

	m_pInventoryWindow->SetDimensions(m_windowWidth - 434, 175, m_inventoryWindowWidth, m_inventoryWindowHeight);
	m_pInventoryWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);
	m_pInventoryWindow->SetTitleOffset(50, 5);
	m_pInventoryWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pInventoryWindow->SetApplicationBorder(25, 15, 10, 40);

	m_pTitleBarIcon->SetDimensions(0, m_inventoryWindowHeight, 44, 44);
	m_pCloseExitButton->SetDimensions(m_inventoryWindowWidth - 32, m_inventoryWindowHeight, 32, 32);

	int x;
	int y;
	int width;
	int height;
	GetDestroySlotDimensions(&x, &y, &width, &height);
	m_pDestroyIcon->SetDimensions(x, y, width, height);

	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont50(), "%s", m_pDestroyLabel->GetText().c_str());
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_pFrontendManager->GetFrontendFont50(), "%s", m_pDestroyLabel->GetText().c_str());
	m_pDestroyLabel->SetLocation(x + static_cast<int>((width * 0.5f) - (textWidth * 0.5f)), y + static_cast<int>((height * 0.5f) - (textHeight * 0.5f)) + 5);

	GetDropSlotDimensions(&x, &y, &width, &height);
	m_pDropIcon->SetDimensions(x, y, width, height);

	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont50(), "%s", m_pDropLabel->GetText().c_str());
	textHeight = m_pRenderer->GetFreeTypeTextHeight(m_pFrontendManager->GetFrontendFont50(), "%s", m_pDropLabel->GetText().c_str());
	m_pDropLabel->SetLocation(x + static_cast<int>((width * 0.5f) - (textWidth * 0.5f)), y + static_cast<int>((height * 0.5f) - (textHeight * 0.5f)) + 5);

	// Popup
	m_pPopupBackgroundIcon->SetDimensions(static_cast<int>((windowWidth * 0.5f) - (m_popupWidth * 0.5f)), static_cast<int>((windowHeight * 0.5f) - (m_popupHeight * 0.5f)) + 100, m_popupWidth, m_popupHeight);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void InventoryGUI::GetInventoryDimensions(int indexX, int indexY, int* x, int* y, int* width, int *height) const
{
	int slotSize = 64;
	int slotSpacer = 4;
	int borderSize = 4;

	int xSlotPos = borderSize + ((slotSize + slotSpacer) * indexX);
	int ySlotPos = 4 + borderSize + ((slotSize + slotSpacer) * indexY);

	*x = xSlotPos;
	*y = ySlotPos;

	*width = slotSize;
	*height = slotSize;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void InventoryGUI::GetDestroySlotDimensions(int* x, int* y, int* width, int* height) const
{
	*x = 220;
	*y = -75;

	*width = 175;
	*height = 65;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void InventoryGUI::GetDropSlotDimensions(int* x, int* y, int* width, int* height) const
{
	*x = 10;
	*y = -75;

	*width = 175;
	*height = 65;
}

GUIWindow* InventoryGUI::GetInventoryWindow() const
{
	return m_pInventoryWindow;
}

void InventoryGUI::CreateInventoryItems()
{
	// Item draggable Buttons
	for (int i = 0; i < MAX_NUM_SLOTS_VERTICAL; ++i)
	{
		for (int j = 0; j < MAX_NUM_SLOTS_HORIZONTAL; ++j)
		{
			InventoryItem* pItem = m_pInventoryManager->GetInventoryItemForSlot(j, i);

			if (pItem != nullptr)
			{
				int x;
				int y;
				int width;
				int height;
				GetInventoryDimensions(j, i, &x, &y, &width, &height);

				DraggableRenderRectangle* pNewSlotItem = new DraggableRenderRectangle(m_pRenderer);
				switch (pItem->m_itemQuality)
				{
				case ItemQuality::Common: { pNewSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderCommon); break; }
				case ItemQuality::Uncommon: { pNewSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderUncommon); break; }
				case ItemQuality::Magical: { pNewSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderMagical); break; }
				case ItemQuality::Rare: { pNewSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderRare); break; }
				case ItemQuality::Epic: { pNewSlotItem->SetIcon(m_pInventoryBackgroundSlotBorderEpic); break; }
				default: throw std::logic_error("Invalid ItemQuality in CreateInventoryItems()");
				}
				pNewSlotItem->SetDimensions(x, y, width, height);
				pNewSlotItem->SetDepth(3.0f);
				char itemTexture[128];
				sprintf(itemTexture, "%s", pItem->m_iconFileName.c_str());
				pNewSlotItem->AddIcon(m_pRenderer, itemTexture, 64, 64, 56, 56, 4, 4, 1.5f);

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
				default: throw std::logic_error("Invalid ItemQuality in CreateInventoryItems()");
				}

				if (pItem->m_quantity != -1)
				{
					char quantity[128];
					sprintf(quantity, "%i", pItem->m_quantity);
					int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18(), "%s", quantity);
					pNewSlotItem->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), quantity, Color(1.0f, 1.0f, 1.0f, 1.0f), width - 10 - textWidth, 8, true, Color(0.0f, 0.0f, 0.0f, 1.0f));
				}

				InventorySlotItem* pNewItem = new InventorySlotItem();
				pNewItem->m_pInventoryGUI = this;
				pNewItem->m_pInventoryItem = pItem;
				pNewItem->m_pInventoryIcon = pNewSlotItem;
				pNewItem->m_slotX = j;
				pNewItem->m_slotY = i;
				pNewItem->m_dropshadowAdded = false;
				pNewItem->m_erase = false;
				m_vpInventorySlotItems.push_back(pNewItem);

				pNewSlotItem->SetPressedCallBackFunction(_InventoryItemPressed);
				pNewSlotItem->SetPressedCallBackData(pNewItem);

				pNewSlotItem->SetReleasedCallBackFunction(_InventoryItemReleased);
				pNewSlotItem->SetReleasedCallBackData(pNewItem);

				pNewSlotItem->SetEnterCallBackFunction(_InventoryItemEntered);
				pNewSlotItem->SetEnterCallBackData(pNewItem);

				pNewSlotItem->SetExitCallBackFunction(_InventoryItemExited);
				pNewSlotItem->SetExitCallBackData(pNewItem);
			}
		}
	}

	m_pInventoryManager->SetInventoryGUINeedsUpdate(false);
}

void InventoryGUI::DeleteInventoryItems()
{
	// Clear item draggable buttons
	for (size_t i = 0; i < m_vpInventorySlotItems.size(); ++i)
	{
		m_pInventoryWindow->RemoveComponent(m_vpInventorySlotItems[i]->m_pInventoryIcon);
		delete m_vpInventorySlotItems[i]->m_pInventoryIcon;
		m_vpInventorySlotItems[i]->m_pInventoryIcon = nullptr;

		delete m_vpInventorySlotItems[i];
		m_vpInventorySlotItems[i] = nullptr;
	}
	m_vpInventorySlotItems.clear();
}

void InventoryGUI::UpdateActionBar()
{
	// We need to relink the action bar to the inventory GUI, since we have deleted and re-created the inventory buttons
	for (size_t i = 0; i < m_vpInventorySlotItems.size(); ++i)
	{
		for (int j = 0; j < ActionBar::MAX_NUM_ACTION_SLOTS; ++j)
		{
			ActionButtonItem* pActionBarItem = m_pActionBar->GetActionButtonForSlot(j);

			if (pActionBarItem != nullptr)
			{
				if (pActionBarItem->m_inventoryX == m_vpInventorySlotItems[i]->m_slotX && pActionBarItem->m_inventoryY == m_vpInventorySlotItems[i]->m_slotY)
				{
					if (pActionBarItem->m_inventoryX == -1 && pActionBarItem->m_inventoryY == -1)
					{
						if (pActionBarItem->m_equipSlot == m_vpInventorySlotItems[i]->m_pInventoryItem->m_equipSlot)
						{
							// In the situation where the loaded action button has a -1, -1 slot index, we also need to check the item slot type before assigning this pointer
							pActionBarItem->m_itemTitle = m_vpInventorySlotItems[i]->m_pInventoryItem->m_title;
						}
					}
					else
					{
						pActionBarItem->m_itemTitle = m_vpInventorySlotItems[i]->m_pInventoryItem->m_title;
					}
				}
			}
		}
	}
}

// Tooltips
void InventoryGUI::UpdateToolTipAppear(float dt)
{
	if (m_toolTipVisible)
	{
		if (m_tooltipAppearDelayTimer <= 0.0f)
		{
			if (m_toolTipComponentsAdded == false)
			{
				switch (m_tooltipQuality)
				{
				case ItemQuality::Common: { m_pInventoryWindow->AddComponent(m_pTooltipBackgroundCommon); break; }
				case ItemQuality::Uncommon: { m_pInventoryWindow->AddComponent(m_pTooltipBackgroundUncommon); break; }
				case ItemQuality::Magical: { m_pInventoryWindow->AddComponent(m_pTooltipBackgroundMagical); break; }
				case ItemQuality::Rare: { m_pInventoryWindow->AddComponent(m_pTooltipBackgroundRare); break; }
				case ItemQuality::Epic: { m_pInventoryWindow->AddComponent(m_pTooltipBackgroundEpic); break; }
				default: throw std::logic_error("Invalid ItemQuality in UpdateToolTipAppear()");
				}

				m_pInventoryWindow->AddComponent(m_pTooltipNameLabel);
				m_pInventoryWindow->AddComponent(m_pTooltipDescriptionLabel);
				m_pInventoryWindow->AddComponent(m_pTooltipSlotLabel);
				m_pInventoryWindow->AddComponent(m_pTooltipQualityLabel);

				m_toolTipComponentsAdded = true;
			}
		}
		else
		{
			m_tooltipAppearDelayTimer -= dt;
		}
	}
}

void InventoryGUI::ShowTooltip(InventorySlotItem* pInventoryItem)
{
	if (m_toolTipVisible == true)
	{
		return;
	}

	// Set the focused window when we show a tooltip
	m_pInventoryWindow->SetFocusWindow();

	// Replace the tooltip name
	m_pTooltipNameLabel->SetText(pInventoryItem->m_pInventoryItem->m_title);

	// Replace the tooltip description
	std::string DescriptionText = pInventoryItem->m_pInventoryItem->m_description + pInventoryItem->m_pInventoryItem->GetStatsAttributeString();
	m_pTooltipDescriptionLabel->SetText(DescriptionText);

	// Replace the tooltip equipslot text
	char slotText[32];
	switch (pInventoryItem->m_pInventoryItem->m_equipSlot)
	{
	case EquipSlot::NoSlot: { sprintf(slotText, ""); break; }
	case EquipSlot::LeftHand:
	{
		if (pInventoryItem->m_pInventoryItem->m_right)
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
		if (pInventoryItem->m_pInventoryItem->m_left)
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
	switch (pInventoryItem->m_pInventoryItem->m_itemQuality)
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

	int x;
	int y;
	int width;
	int height;
	GetInventoryDimensions(pInventoryItem->m_slotX, pInventoryItem->m_slotY, &x, &y, &width, &height);

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

	m_pTooltipDescriptionLabel->SetDimensions(x + m_tooltipDescBorder, y + m_tooltipDescBorder, m_tooltipWidth - (m_tooltipDescBorder * 2), m_tooltipHeight - (m_tooltipDescBorder * 2) - 35);

	m_pTooltipSlotLabel->SetLocation(x + m_tooltipDescBorder, y + m_tooltipDescBorder);

	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont20(), "%s", m_pTooltipQualityLabel->GetText().c_str());
	m_pTooltipQualityLabel->SetLocation(x + m_tooltipWidth - m_tooltipDescBorder - textWidth, y + m_tooltipDescBorder);

	m_tooltipQuality = pInventoryItem->m_pInventoryItem->m_itemQuality;

	m_tooltipAppearDelayTimer = m_pFrontendManager->GetToolTipAppearDelay();

	m_toolTipVisible = true;
	m_toolTipComponentsAdded = false;
}

void InventoryGUI::HideTooltip()
{
	m_pInventoryWindow->RemoveComponent(m_pTooltipBackgroundCommon);
	m_pInventoryWindow->RemoveComponent(m_pTooltipBackgroundUncommon);
	m_pInventoryWindow->RemoveComponent(m_pTooltipBackgroundMagical);
	m_pInventoryWindow->RemoveComponent(m_pTooltipBackgroundRare);
	m_pInventoryWindow->RemoveComponent(m_pTooltipBackgroundEpic);
	m_pInventoryWindow->RemoveComponent(m_pTooltipNameLabel);
	m_pInventoryWindow->RemoveComponent(m_pTooltipDescriptionLabel);
	m_pInventoryWindow->RemoveComponent(m_pTooltipSlotLabel);
	m_pInventoryWindow->RemoveComponent(m_pTooltipQualityLabel);

	m_toolTipVisible = false;
}

void InventoryGUI::OpenPopup(std::string popupTitle, std::string popupText) const
{
	m_pPopupConfirmButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pPopupCancelButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());

	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont40(), "%s", popupTitle.c_str());
	m_popupTitle->SetLocation(static_cast<int>((m_windowWidth * 0.5f) - (textWidth * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) + (m_popupHeight * 0.5f)) - m_popupTitleSpacer - m_popupBorderSpacer + 100);
	m_popupText->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (m_popupWidth * 0.5f)) + m_popupBorderSpacer, static_cast<int>((m_windowHeight * 0.5f) - (m_popupHeight * 0.5f)) + 100, m_popupWidth - (m_popupBorderSpacer * 2), m_popupHeight - m_popupBorderSpacer - m_popupTitleSpacer);
	m_pPopupConfirmButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) + (m_popupWidth * 0.5f)) - static_cast<int>(m_popupBorderSpacer * 0.5f) - 110, static_cast<int>((m_windowHeight * 0.5f) - (m_popupIconSize * 0.5f)) - 50 + 100, 110, 47);
	m_pPopupCancelButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (m_popupWidth * 0.5f)) + static_cast<int>(m_popupBorderSpacer * 0.5f), static_cast<int>((m_windowHeight * 0.5f) - (m_popupIconSize * 0.5f)) - 50 + 100, 110, 47);

	m_popupTitle->SetText(popupTitle);
	m_popupText->SetText(popupText);

	m_pGUI->AddComponent(m_popupTitle);
	m_pGUI->AddComponent(m_popupText);
	m_pGUI->AddComponent(m_pPopupConfirmButton);
	m_pGUI->AddComponent(m_pPopupCancelButton);
	m_pGUI->AddComponent(m_pPopupBackgroundIcon);
}

void InventoryGUI::ClosePopup() const
{
	m_pGUI->RemoveComponent(m_popupTitle);
	m_pGUI->RemoveComponent(m_popupText);
	m_pGUI->RemoveComponent(m_pPopupConfirmButton);
	m_pGUI->RemoveComponent(m_pPopupCancelButton);
	m_pGUI->RemoveComponent(m_pPopupBackgroundIcon);
}

bool InventoryGUI::IsLoaded() const
{
	return m_loaded;
}

InventorySlotItem* InventoryGUI::GetInventorySlotItem(int x, int y)
{
	for (size_t i = 0; i < m_vpInventorySlotItems.size(); ++i)
	{
		if (m_vpInventorySlotItems[i]->m_slotX == x && m_vpInventorySlotItems[i]->m_slotY == y)
		{
			return m_vpInventorySlotItems[i];
		}
	}

	return nullptr;
}

InventorySlotItem* InventoryGUI::GetInventorySlotItemEquipped(EquipSlot equipSlot)
{
	InventoryItem* pInventoryItem = m_pInventoryManager->GetInventoryItemForEquipSlot(equipSlot);

	if (pInventoryItem != nullptr)
	{
		for (size_t i = 0; i < m_vpInventorySlotItems.size(); ++i)
		{
			if (m_vpInventorySlotItems[i]->m_pInventoryItem == pInventoryItem)
			{
				return m_vpInventorySlotItems[i];
			}
		}
	}

	return nullptr;
}

void InventoryGUI::SetEquippedItem(EquipSlot equipSlot, std::string title)
{
	m_pEquippedItems[static_cast<int>(equipSlot)] = title;
}

void InventoryGUI::EquipItem(InventoryItem* pInventoryItem, int inventoryX, int inventoryY)
{
	// Set the player to know that we have equipped an item
	m_pPlayer->EquipItem(pInventoryItem);

	// If we already have an item in this equipment slot, switch it out
	if (m_pEquippedItems[static_cast<int>(pInventoryItem->m_equipSlot)] != "")
	{
		m_pActionBar->UpdateActionBarSlots(m_pEquippedItems[static_cast<int>(pInventoryItem->m_equipSlot)], inventoryX, inventoryY);
	}

	m_pActionBar->UpdateActionBarSlots(pInventoryItem, -1, -1);

	// Equip the new item
	m_pEquippedItems[static_cast<int>(pInventoryItem->m_equipSlot)] = pInventoryItem->m_title;
}

void InventoryGUI::EquipItem(LootSlotItem* pLootItem) const
{
	// Set the player to know that we have equipped an item
	m_pPlayer->EquipItem(pLootItem->m_pInventoryItem);
}

void InventoryGUI::EquipItem(InventorySlotItem* pInventoryItem)
{
	// Set the player to know that we have equipped an item
	m_pPlayer->EquipItem(pInventoryItem->m_pInventoryItem);

	// If we already have an item in this equipment slot, switch it out
	if (m_pEquippedItems[static_cast<int>(pInventoryItem->m_pInventoryItem->m_equipSlot)] != "")
	{
		m_pActionBar->UpdateActionBarSlots(m_pEquippedItems[static_cast<int>(pInventoryItem->m_pInventoryItem->m_equipSlot)], pInventoryItem->m_slotX, pInventoryItem->m_slotY);
	}

	m_pActionBar->UpdateActionBarSlots(pInventoryItem->m_pInventoryItem, -1, -1);

	// Equip the new item
	m_pEquippedItems[static_cast<int>(pInventoryItem->m_pInventoryItem->m_equipSlot)] = pInventoryItem->m_pInventoryItem->m_title;
	pInventoryItem->m_slotX = -1;
	pInventoryItem->m_slotY = -1;
}

void InventoryGUI::UnequipItem(EquipSlot equipSlot, bool left, bool right)
{
	m_pEquippedItems[static_cast<int>(equipSlot)] = "";

	m_pPlayer->UnequipItem(equipSlot, left, right);
}

void InventoryGUI::Update(float dt)
{
	if (m_loadDelay == true)
	{
		if (m_loadDelayTime <= 0.0f)
		{
			m_loadDelay = false;
			m_pGUI->AddWindow(m_pInventoryWindow);
			m_pInventoryWindow->Show();
		}
		else
		{
			m_loadDelayTime -= dt;
		}
	}

	UpdateToolTipAppear(dt);

	// Check if the inventory GUI needs update (we have moved items in the inventory or got new items)
	if (m_pInventoryManager->InventoryGUINeedsUpdate() && IsLoaded() == true)
	{
		m_pGUI->RemoveWindow(m_pInventoryWindow);

		DeleteInventoryItems();
		CreateInventoryItems();
		UpdateActionBar();

		for (size_t i = 0; i < m_vpInventorySlotItems.size(); ++i)
		{
			m_pInventoryWindow->AddComponent(m_vpInventorySlotItems[i]->m_pInventoryIcon);
		}

		m_pGUI->AddWindow(m_pInventoryWindow);
		m_pInventoryWindow->Show();
	}
}

void InventoryGUI::_InventoryItemPressed(void* pData)
{
	InventorySlotItem* pInventoryItem = static_cast<InventorySlotItem*>(pData);
	pInventoryItem->m_pInventoryGUI->InventoryItemPressed(pInventoryItem);
}

void InventoryGUI::InventoryItemPressed(InventorySlotItem* pInventoryItem)
{
	m_pPressedInventoryItem = pInventoryItem;

	Dimensions dimensions = m_pPressedInventoryItem->m_pInventoryIcon->GetDimensions();
	m_pressedX = dimensions.x;
	m_pressedY = dimensions.y;

	m_pInventoryWindow->AddComponent(m_pDestroyIcon);
	m_pInventoryWindow->AddComponent(m_pDestroyLabel);
	m_pInventoryWindow->AddComponent(m_pDropIcon);
	m_pInventoryWindow->AddComponent(m_pDropLabel);

	// Temporarily increase the depth of the dragged icon
	if (m_pPressedInventoryItem->m_dropshadowAdded == false)
	{
		m_pPressedInventoryItem->m_dropshadowAdded = true;
		m_pPressedInventoryItem->m_pInventoryIcon->SetDepth(5.0f);
		//m_pPressedInventoryItem->m_pInventoryIcon->SetLocation(m_pressedX - 4, m_pressedY + 4);
		std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
		std::string dropShadowIcon = "Resources/textures/gui/" + themeName + "/common/items/drop_shadow.tga";
		m_pPressedInventoryItem->m_pInventoryIcon->AddIcon(m_pRenderer, dropShadowIcon.c_str(), 64, 64, 64, 64, 4, -4, 0.5f);
	}

	m_pInventoryWindow->DepthSortComponentChildren();

	HideTooltip();
}

bool IsNeedErase(InventorySlotItem* item)
{
	bool isNeedErase = item->m_erase;

	if (isNeedErase == true)
	{
		delete item->m_pInventoryIcon;
		delete item;
	}

	return isNeedErase;
}

void InventoryGUI::_InventoryItemReleased(void* pData)
{
	InventorySlotItem* pInventoryItem = static_cast<InventorySlotItem*>(pData);
	pInventoryItem->m_pInventoryGUI->InventoryItemReleased(pInventoryItem);
}

void InventoryGUI::InventoryItemReleased(InventorySlotItem* pInventoryItem)
{
	if (m_pPressedInventoryItem == nullptr)
	{
		return;
	}

	m_pPressedInventoryItem = nullptr;

	if (m_pPlayer->IsCrafting())
	{
		// Don't allow to do any inventory changing if we are crafting.

		// Reset back to the original position
		pInventoryItem->m_pInventoryIcon->SetLocation(m_pressedX, m_pressedY);
		if (pInventoryItem->m_dropshadowAdded == true)
		{
			pInventoryItem->m_dropshadowAdded = false;
			std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
			std::string dropShadowIcon = "Resources/textures/gui/" + themeName + "/common/items/drop_shadow.tga";
			pInventoryItem->m_pInventoryIcon->RemoveIcon(dropShadowIcon.c_str());
		}

		m_pInventoryWindow->RemoveComponent(m_pDestroyIcon);
		m_pInventoryWindow->RemoveComponent(m_pDestroyLabel);
		m_pInventoryWindow->RemoveComponent(m_pDropIcon);
		m_pInventoryWindow->RemoveComponent(m_pDropLabel);

		return;
	}

	// Figure out if we need to change to a different inventory slot
	int x;
	int y;
	int width;
	int height;
	POINT mouse = { CubbyGame::GetInstance()->GetWindowCursorX(), (m_windowHeight - CubbyGame::GetInstance()->GetWindowCursorY()) };
	bool switched = false;
	for (int i = 0; i < MAX_NUM_SLOTS_VERTICAL; ++i)
	{
		for (int j = 0; j < MAX_NUM_SLOTS_HORIZONTAL; ++j)
		{
			GetInventoryDimensions(j, i, &x, &y, &width, &height);

			// Check if we released (mouse cursor) in the boundary of another slot
			if (mouse.x > m_pInventoryWindow->GetDimensions().x + x && mouse.x < m_pInventoryWindow->GetDimensions().x + x + width && mouse.y > m_pInventoryWindow->GetDimensions().y + y && mouse.y < m_pInventoryWindow->GetDimensions().y + y + height)
			{
				if (pInventoryItem->m_pInventoryItem->m_equipped == true)
				{
					// Check if another inventory item already exists in this slot
					InventorySlotItem* pInventorySlotItem = GetInventorySlotItem(j, i);
					if (pInventorySlotItem == nullptr)
					{
						// We are unequipping an item that is in one of the equipment slots
						m_pInventoryManager->UnequipItem(j, i, pInventoryItem->m_pInventoryItem->m_equipSlot);

						UnequipItem(pInventoryItem->m_pInventoryItem->m_equipSlot, pInventoryItem->m_pInventoryItem->m_left, pInventoryItem->m_pInventoryItem->m_right);

						// Set the new location for the released inventory icon
						pInventoryItem->m_slotX = j;
						pInventoryItem->m_slotY = i;
						pInventoryItem->m_pInventoryIcon->SetLocation(x, y);
						m_pActionBar->UpdateActionBarSlots(pInventoryItem->m_pInventoryItem, j, i);

						switched = true;
					}
					else
					{
						if (pInventorySlotItem->m_pInventoryItem->m_equipSlot == pInventoryItem->m_pInventoryItem->m_equipSlot)
						{
							// We are swapping an equipped item for one in the inventory
							UnequipItem(pInventoryItem->m_pInventoryItem->m_equipSlot, pInventoryItem->m_pInventoryItem->m_left, pInventoryItem->m_pInventoryItem->m_right);
							m_pInventoryManager->UnequipItem(j, i, pInventoryItem->m_pInventoryItem->m_equipSlot);

							// Equip the new item
							//m_pInventoryManager->EquipInventoryItem(pInventorySlotItem->m_slotX, pInventorySlotItem->m_slotY, pInventorySlotItem->m_pInventoryItem->m_equipSlot);
							EquipItem(pInventorySlotItem);

							// Set the new location for the released inventory icon
							pInventoryItem->m_slotX = j;
							pInventoryItem->m_slotY = i;
							pInventoryItem->m_pInventoryIcon->SetLocation(x, y);

							switched = true;
						}
					}
				}
				else
				{
					// Check if another inventory item already exists in this slot
					InventorySlotItem* pInventorySlotItem = GetInventorySlotItem(j, i);

					// Switch the inventory item in this slot to the pressed (previous) position
					if (pInventorySlotItem != nullptr)
					{
						pInventorySlotItem->m_slotX = pInventoryItem->m_slotX;
						pInventorySlotItem->m_slotY = pInventoryItem->m_slotY;
						pInventorySlotItem->m_pInventoryIcon->SetLocation(m_pressedX, m_pressedY);
						m_pActionBar->UpdateActionBarSlots(pInventorySlotItem->m_pInventoryItem, pInventoryItem->m_slotX, pInventoryItem->m_slotY);
					}

					// Switch the items in the inventory manager
					m_pInventoryManager->SwitchInventoryItems(pInventoryItem->m_slotX, pInventoryItem->m_slotY, j, i);

					// Set the new location for the released inventory icon
					pInventoryItem->m_slotX = j;
					pInventoryItem->m_slotY = i;
					pInventoryItem->m_pInventoryIcon->SetLocation(x, y);
					m_pActionBar->UpdateActionBarSlots(pInventoryItem->m_pInventoryItem, j, i);

					switched = true;
				}
			}
		}
	}

	if (switched)
	{
		ShowTooltip(pInventoryItem);
	}


	bool equipped = false;
	bool deleted = false;
	if (switched == false)
	{
		if (pInventoryItem->m_pInventoryItem->m_equipSlot != EquipSlot::NoSlot)
		{
			// Check if we need to equip the item after dropping on the player portrait
			if (m_pCharacterGUI->IsLoaded())
			{
				m_pCharacterGUI->GetPlayerPortraitDimensions(&x, &y, &width, &height);
				GUIWindow* pCharacterWindow = m_pCharacterGUI->GetCharacterWindow();
				if (mouse.x > pCharacterWindow->GetDimensions().x + x && mouse.x < pCharacterWindow->GetDimensions().x + x + width && mouse.y > pCharacterWindow->GetDimensions().y + y && mouse.y < pCharacterWindow->GetDimensions().y + y + height)
				{
					if (pInventoryItem->m_pInventoryItem->m_equipped == false)
					{
						// Dual handed weapon checks
						if (pInventoryItem->m_pInventoryItem->m_equipSlot == EquipSlot::RightHand)
						{
							InventoryItem* pLeftHanded = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::LeftHand);
							if (pInventoryItem->m_pInventoryItem->m_left || (pLeftHanded != nullptr && pLeftHanded->m_right))
							{
								int slotX;
								int slotY;
								// Unequip the left hand slot since we are dual handed, OR the already equipped left hand item needs both hands
								UnequipItem(EquipSlot::LeftHand, false, false);
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
						if (pInventoryItem->m_pInventoryItem->m_equipSlot == EquipSlot::LeftHand)
						{
							InventoryItem* pRightHanded = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::RightHand);
							if (pInventoryItem->m_pInventoryItem->m_right || (pRightHanded != nullptr && pRightHanded->m_left))
							{
								int slotX;
								int slotY;
								// Unequip the right hand slot since we are dual handed, OR the already equipped right hand item needs both hands
								UnequipItem(EquipSlot::RightHand, false, false);
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

						m_pPlayer->UnequipItem(pInventoryItem->m_pInventoryItem->m_equipSlot, pInventoryItem->m_pInventoryItem->m_left, pInventoryItem->m_pInventoryItem->m_right);
						m_pInventoryManager->EquipInventoryItem(pInventoryItem->m_slotX, pInventoryItem->m_slotY, pInventoryItem->m_pInventoryItem->m_equipSlot);
						EquipItem(pInventoryItem);

						m_pInventoryWindow->RemoveComponent(pInventoryItem->m_pInventoryIcon);

						pInventoryItem->m_erase = true;
						m_vpInventorySlotItems.erase(remove_if(m_vpInventorySlotItems.begin(), m_vpInventorySlotItems.end(), IsNeedErase), m_vpInventorySlotItems.end());

						m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
						m_pInventoryManager->SetCharacterGUINeedsUpdate(true);

						m_pCharacterGUI->HideEquipHover();

						equipped = true;
					}
				}

				if (equipped == false)
				{
					// Check if we need to equip the item after dropping on a equipment slot
					switch (pInventoryItem->m_pInventoryItem->m_equipSlot)
					{
					case EquipSlot::NoSlot: { x = 0; y = 0; width = 0; height = 0; } break;
					case EquipSlot::RightHand: { m_pCharacterGUI->GetPlayerWeaponRightDimensions(&x, &y, &width, &height); } break;
					case EquipSlot::LeftHand: { m_pCharacterGUI->GetPlayerWeaponLeftDimensions(&x, &y, &width, &height); } break;
					case EquipSlot::Head: { m_pCharacterGUI->GetPlayerHeadDimensions(&x, &y, &width, &height); } break;
					case EquipSlot::Shoulders: { m_pCharacterGUI->GetPlayerShoulderDimensions(&x, &y, &width, &height); } break;
					case EquipSlot::Body: { m_pCharacterGUI->GetPlayerBodyDimensions(&x, &y, &width, &height); } break;
					case EquipSlot::Legs: { m_pCharacterGUI->GetPlayerLegsDimensions(&x, &y, &width, &height); } break;
					case EquipSlot::Hand: { m_pCharacterGUI->GetPlayerHandDimensions(&x, &y, &width, &height); } break;
					case EquipSlot::Feet: { m_pCharacterGUI->GetPlayerFeetDimensions(&x, &y, &width, &height); } break;
					case EquipSlot::Accessory1: { m_pCharacterGUI->GetPlayerAccessory1Dimensions(&x, &y, &width, &height); } break;
					case EquipSlot::Accessory2: { m_pCharacterGUI->GetPlayerAccessory2Dimensions(&x, &y, &width, &height); } break;
					default: throw std::logic_error("Invalid EquipSlot in InventoryItemReleased()");
					}
					GUIWindow* pCharacterWindow2 = m_pCharacterGUI->GetCharacterWindow();
					if (mouse.x > pCharacterWindow2->GetDimensions().x + x && mouse.x < pCharacterWindow2->GetDimensions().x + x + width && mouse.y > pCharacterWindow2->GetDimensions().y + y && mouse.y < pCharacterWindow2->GetDimensions().y + y + height)
					{
						if (pInventoryItem->m_pInventoryItem->m_equipped == false)
						{
							// Dual handed weapon checks
							if (pInventoryItem->m_pInventoryItem->m_equipSlot == EquipSlot::RightHand)
							{
								InventoryItem* pLeftHanded = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::LeftHand);
								if (pInventoryItem->m_pInventoryItem->m_left || (pLeftHanded != nullptr && pLeftHanded->m_right))
								{
									int slotX;
									int slotY;
									// Unequip the left hand slot since we are dual handed, OR the already equipped left hand item needs both hands
									UnequipItem(EquipSlot::LeftHand, false, false);
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
							if (pInventoryItem->m_pInventoryItem->m_equipSlot == EquipSlot::LeftHand)
							{
								InventoryItem* pRightHanded = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::RightHand);
								if (pInventoryItem->m_pInventoryItem->m_right || (pRightHanded != nullptr && pRightHanded->m_left))
								{
									int slotX;
									int slotY;
									// Unequip the right hand slot since we are dual handed, OR the already equipped right hand item needs both hands
									UnequipItem(EquipSlot::RightHand, false, false);
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

							m_pPlayer->UnequipItem(pInventoryItem->m_pInventoryItem->m_equipSlot, pInventoryItem->m_pInventoryItem->m_left, pInventoryItem->m_pInventoryItem->m_right);
							m_pInventoryManager->EquipInventoryItem(pInventoryItem->m_slotX, pInventoryItem->m_slotY, pInventoryItem->m_pInventoryItem->m_equipSlot);
							EquipItem(pInventoryItem);

							m_pInventoryWindow->RemoveComponent(pInventoryItem->m_pInventoryIcon);

							pInventoryItem->m_erase = true;
							m_vpInventorySlotItems.erase(remove_if(m_vpInventorySlotItems.begin(), m_vpInventorySlotItems.end(), IsNeedErase), m_vpInventorySlotItems.end());

							m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
							m_pInventoryManager->SetCharacterGUINeedsUpdate(true);

							m_pCharacterGUI->HideEquipHover();

							equipped = true;
						}
					}
				}
			}
		}


		if (equipped == false)
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
								m_pLootGUI->AddLootItemFromInventory(pInventoryItem->m_pInventoryItem, j, i);

								m_pInventoryManager->RemoveInventoryItem(pInventoryItem->m_slotX, pInventoryItem->m_slotY);

								m_pActionBar->RemoveInventoryItemFromActionBar(pInventoryItem->m_pInventoryItem->m_title);

								m_pInventoryWindow->RemoveComponent(pInventoryItem->m_pInventoryIcon);

								pInventoryItem->m_erase = true;
								m_vpInventorySlotItems.erase(remove_if(m_vpInventorySlotItems.begin(), m_vpInventorySlotItems.end(), IsNeedErase), m_vpInventorySlotItems.end());

								m_pCharacterGUI->HideEquipHover();

								switched = true;
								deleted = true;
							}
						}
					}
				}
			}

			// Check if we released on a action bar slot
			if (CubbyGame::GetInstance()->GetCubbySettings()->m_renderGUI)
			{
				if (m_pActionBar->IsLoaded())
				{
					for (int i = 0; i < ActionBar::MAX_NUM_ACTION_SLOTS; ++i)
					{
						m_pActionBar->GetActionSlotDimensions(i, &x, &y, &width, &height);

						// Check if we released (mouse cursor) in the boundary of another slot
						if (mouse.x > x && mouse.x < x + width && mouse.y > y && mouse.y < y + height)
						{
							m_pActionBar->AddItemToActionBar(pInventoryItem->m_pInventoryItem, i, pInventoryItem->m_slotX, pInventoryItem->m_slotY);
							m_pActionBar->ExportActionBar(m_pPlayer->GetName());
						}
					}
				}
			}

			if (switched == false)
			{
				// Check if we need to drop the item into the world
				GetDropSlotDimensions(&x, &y, &width, &height);
				if (mouse.x > m_pInventoryWindow->GetDimensions().x + x && mouse.x < m_pInventoryWindow->GetDimensions().x + x + width && mouse.y > m_pInventoryWindow->GetDimensions().y + y && mouse.y < m_pInventoryWindow->GetDimensions().y + y + height)
				{
					if (pInventoryItem->m_slotX == -1 && pInventoryItem->m_slotY == -1)
					{
						// Do nothing
					}
					else
					{
						// Drop the item in the world
						glm::vec3 vel = glm::vec3(GetRandomNumber(-1, 1, 2), 0.0f, GetRandomNumber(-1, 1, 2)) * GetRandomNumber(2, 3, 2);

						Item* pItem = m_pItemManager->CreateItem(m_pPlayer->GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), pInventoryItem->m_pInventoryItem->m_fileName.c_str(), ItemType::DroppedItem, pInventoryItem->m_pInventoryItem->m_title.c_str(), true, false, 0.08f);
						if (pItem != nullptr)
						{
							pItem->SetGravityDirection(glm::vec3(0.0f, -1.0f, 0.0f));
							pItem->SetVelocity(normalize(vel)*4.5f + glm::vec3(0.0f, 9.5f + GetRandomNumber(3, 6, 2), 0.0f));
							pItem->SetRotation(glm::vec3(0.0f, GetRandomNumber(0, 360, 2), 0.0f));
							pItem->SetAngularVelocity(glm::vec3(0.0f, 90.0f, 0.0f));
							pItem->SetDroppedItem(pInventoryItem->m_pInventoryItem->m_fileName.c_str(), pInventoryItem->m_pInventoryItem->m_iconFileName.c_str(), pInventoryItem->m_pInventoryItem->m_itemType, pInventoryItem->m_pInventoryItem->m_item, pInventoryItem->m_pInventoryItem->m_status, pInventoryItem->m_pInventoryItem->m_equipSlot, pInventoryItem->m_pInventoryItem->m_itemQuality, pInventoryItem->m_pInventoryItem->m_left, pInventoryItem->m_pInventoryItem->m_right, pInventoryItem->m_pInventoryItem->m_title.c_str(), pInventoryItem->m_pInventoryItem->m_description.c_str(), pInventoryItem->m_pInventoryItem->m_placementR, pInventoryItem->m_pInventoryItem->m_placementG, pInventoryItem->m_pInventoryItem->m_placementB, pInventoryItem->m_pInventoryItem->m_quantity);
							pItem->SetCollisionEnabled(false);

							for (int i = 0; i < static_cast<int>(pInventoryItem->m_pInventoryItem->m_vpStatAttributes.size()); ++i)
							{
								pItem->GetDroppedInventoryItem()->AddStatAttribute(pInventoryItem->m_pInventoryItem->m_vpStatAttributes[i]->GetType(), pInventoryItem->m_pInventoryItem->m_vpStatAttributes[i]->GetModifyAmount());
							}

							int numY = pItem->GetVoxelItem()->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix(0)->m_matrixSizeY;
							pItem->GetVoxelItem()->SetRenderOffset(glm::vec3(0.0f, numY * 0.5f, 0.0f));
						}

						m_pInventoryManager->RemoveInventoryItem(pInventoryItem->m_slotX, pInventoryItem->m_slotY);

						m_pActionBar->RemoveInventoryItemFromActionBar(pInventoryItem->m_pInventoryItem->m_title);

						m_pInventoryWindow->RemoveComponent(pInventoryItem->m_pInventoryIcon);

						pInventoryItem->m_erase = true;
						m_vpInventorySlotItems.erase(remove_if(m_vpInventorySlotItems.begin(), m_vpInventorySlotItems.end(), IsNeedErase), m_vpInventorySlotItems.end());

						m_pCharacterGUI->HideEquipHover();

						switched = true;
						deleted = true;
					}
				}

				// Check if we need to delete the item
				GetDestroySlotDimensions(&x, &y, &width, &height);
				if (mouse.x > m_pInventoryWindow->GetDimensions().x + x && mouse.x < m_pInventoryWindow->GetDimensions().x + x + width && mouse.y > m_pInventoryWindow->GetDimensions().y + y && mouse.y < m_pInventoryWindow->GetDimensions().y + y + height)
				{
					if (pInventoryItem->m_slotX == -1 && pInventoryItem->m_slotY == -1)
					{
						// Do nothing
					}
					else
					{
						if (CubbyGame::GetInstance()->GetCubbySettings()->m_confirmItemDelete)
						{
							char popupTitle[256];
							sprintf(popupTitle, "Delete");

							char popupText[256];
							sprintf(popupText, "Are you sure you want to delete [C=Custom(00A2E8)]%s[C=White]?", pInventoryItem->m_pInventoryItem->m_title.c_str());

							OpenPopup(popupTitle, popupText);

							m_pInventoryItemToDelete = pInventoryItem;

							m_pCharacterGUI->HideEquipHover();

							switched = false;
							deleted = false;
						}
						else
						{
							if (pInventoryItem != nullptr)
							{
								m_pInventoryManager->RemoveInventoryItem(pInventoryItem->m_slotX, pInventoryItem->m_slotY);

								m_pActionBar->RemoveInventoryItemFromActionBar(pInventoryItem->m_pInventoryItem->m_title);

								m_pInventoryWindow->RemoveComponent(pInventoryItem->m_pInventoryIcon);

								pInventoryItem->m_erase = true;
								m_vpInventorySlotItems.erase(remove_if(m_vpInventorySlotItems.begin(), m_vpInventorySlotItems.end(), IsNeedErase), m_vpInventorySlotItems.end());

								switched = true;
								deleted = true;
							}
						}
					}
				}
			}
		}
	}

	// Revert depth back to normal for inventory icons
	if (deleted == false && equipped == false)
	{
		pInventoryItem->m_pInventoryIcon->SetDepth(3.0f);
		if (pInventoryItem->m_dropshadowAdded == true)
		{
			pInventoryItem->m_dropshadowAdded = false;
			std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
			std::string dropShadowIcon = "Resources/textures/gui/" + themeName + "/common/items/drop_shadow.tga";
			pInventoryItem->m_pInventoryIcon->RemoveIcon(dropShadowIcon.c_str());
		}

		m_pInventoryWindow->DepthSortComponentChildren();
	}

	// Reset back to the original position
	if (switched == false && equipped == false)
	{
		pInventoryItem->m_pInventoryIcon->SetLocation(m_pressedX, m_pressedY);
		if (pInventoryItem->m_dropshadowAdded == true)
		{
			pInventoryItem->m_dropshadowAdded = false;
			std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
			std::string dropShadowIcon = "Resources/textures/gui/" + themeName + "/common/items/drop_shadow.tga";
			pInventoryItem->m_pInventoryIcon->RemoveIcon(dropShadowIcon.c_str());
		}
	}

	m_pInventoryWindow->RemoveComponent(m_pDestroyIcon);
	m_pInventoryWindow->RemoveComponent(m_pDestroyLabel);
	m_pInventoryWindow->RemoveComponent(m_pDropIcon);
	m_pInventoryWindow->RemoveComponent(m_pDropLabel);
}

void InventoryGUI::_InventoryItemEntered(void* pData)
{
	InventorySlotItem* pInventoryItem = static_cast<InventorySlotItem*>(pData);
	pInventoryItem->m_pInventoryGUI->InventoryItemEntered(pInventoryItem);
}

void InventoryGUI::InventoryItemEntered(InventorySlotItem* pInventoryItem)
{
	ShowTooltip(pInventoryItem);

	if (m_pCharacterGUI->IsLoaded())
	{
		m_pCharacterGUI->ShowEquipHover(pInventoryItem->m_pInventoryItem->m_equipSlot);
	}
}

void InventoryGUI::_InventoryItemExited(void* pData)
{
	InventorySlotItem* pInventoryItem = static_cast<InventorySlotItem*>(pData);
	pInventoryItem->m_pInventoryGUI->InventoryItemExited(pInventoryItem);
}

void InventoryGUI::InventoryItemExited(InventorySlotItem* pInventoryItem)
{
	HideTooltip();
	m_pCharacterGUI->HideEquipHover();
}

void InventoryGUI::_CloseExitPressed(void *pData)
{
	InventoryGUI* pInventoryGUI = static_cast<InventoryGUI*>(pData);
	pInventoryGUI->CloseExitPressed();
}

void InventoryGUI::CloseExitPressed()
{
	Unload();
}

void InventoryGUI::_PopupConfirmPressed(void* pData)
{
	InventoryGUI* pInventoryGUI = static_cast<InventoryGUI*>(pData);
	pInventoryGUI->PopupConfirmPressed();
}

void InventoryGUI::PopupConfirmPressed()
{
	if (m_pInventoryItemToDelete != nullptr)
	{
		m_pInventoryManager->RemoveInventoryItem(m_pInventoryItemToDelete->m_slotX, m_pInventoryItemToDelete->m_slotY);

		m_pActionBar->RemoveInventoryItemFromActionBar(m_pInventoryItemToDelete->m_pInventoryItem->m_title);

		m_pInventoryWindow->RemoveComponent(m_pInventoryItemToDelete->m_pInventoryIcon);

		m_pInventoryItemToDelete->m_erase = true;
		m_vpInventorySlotItems.erase(remove_if(m_vpInventorySlotItems.begin(), m_vpInventorySlotItems.end(), IsNeedErase), m_vpInventorySlotItems.end());
	}

	ClosePopup();
}

void InventoryGUI::_PopupCancelPressed(void* pData)
{
	InventoryGUI* pInventoryGUI = static_cast<InventoryGUI*>(pData);
	pInventoryGUI->PopupCancelPressed();
}

void InventoryGUI::PopupCancelPressed()
{
	m_pInventoryItemToDelete = nullptr;

	ClosePopup();
}