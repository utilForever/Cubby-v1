/*************************************************************************
> File Name: LootGUI.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Loot GUI class.
> Created Time: 2016/09/02
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include <CubbyGame.h>

#include "InventoryGUI.h"
#include "LootGUI.h"

// Constructor, Destructor
LootGUI::LootGUI(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight)
{
	m_pRenderer = pRenderer;
	m_pGUI = pGUI;
	m_pFrontendManager = pFrontendManager;
	m_pChunkManager = pChunkManager;
	m_pPlayer = pPlayer;
	m_pInventoryManager = pInventoryManager;

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	// Loot Window
	m_pLootWindow = new GUIWindow(m_pRenderer, m_pFrontendManager->GetFrontendFontMedium(), "Loot");
	m_pLootWindow->AllowMoving(true);
	m_pLootWindow->AllowClosing(false);
	m_pLootWindow->AllowMinimizing(false);
	m_pLootWindow->AllowScrolling(false);
	m_pLootWindow->SetRenderTitleBar(true);
	m_pLootWindow->SetRenderWindowBackground(true);
	m_pLootWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pLootWindow->Hide();

	m_pTitleBarIcon = new Icon(m_pRenderer, "", 44, 44);
	m_pTitleBarIcon->SetDepth(4.0f);

	m_pLootWindowBackgroundIcon = new Icon(m_pRenderer, "", 400, 211);
	m_pLootWindowBackgroundIcon->SetDepth(1.0f);

	m_pTitleBarBackgroundIcon = new Icon(m_pRenderer, "", 108, 35);
	m_pTitleBarBackgroundIcon->SetDepth(1.0f);

	m_pCloseExitButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCloseExitButton->SetDefaultIcon(m_pFrontendManager->GetCloseExitButtonIcon());
	m_pCloseExitButton->SetHoverIcon(m_pFrontendManager->GetCloseExitButtonIconHover());
	m_pCloseExitButton->SetSelectedIcon(m_pFrontendManager->GetCloseExitButtonIconPressed());
	m_pCloseExitButton->SetDisabledIcon(m_pFrontendManager->GetCloseExitButtonIcon());
	m_pCloseExitButton->SetLabelOffset(0, 5);
	m_pCloseExitButton->SetCallBackFunction(_CloseExitPressed);
	m_pCloseExitButton->SetCallBackData(this);

	m_pLootWindow->SetBackgroundIcon(m_pLootWindowBackgroundIcon);
	m_pLootWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	m_pLootWindow->AddComponent(m_pTitleBarIcon);
	m_pLootWindow->AddComponent(m_pCloseExitButton);

	m_pLootButtonBackgroundSlotBorderCommon = new Icon(m_pRenderer, "", 64, 64);
	m_pLootButtonBackgroundSlotBorderCommon->SetDepth(2.0f);

	m_pLootButtonBackgroundSlotBorderUncommon = new Icon(m_pRenderer, "", 64, 64);
	m_pLootButtonBackgroundSlotBorderUncommon->SetDepth(2.0f);

	m_pLootButtonBackgroundSlotBorderMagical = new Icon(m_pRenderer, "", 64, 64);
	m_pLootButtonBackgroundSlotBorderMagical->SetDepth(2.0f);

	m_pLootButtonBackgroundSlotBorderRare = new Icon(m_pRenderer, "", 64, 64);
	m_pLootButtonBackgroundSlotBorderRare->SetDepth(2.0f);

	m_pLootButtonBackgroundSlotBorderEpic = new Icon(m_pRenderer, "", 64, 64);
	m_pLootButtonBackgroundSlotBorderEpic->SetDepth(2.0f);

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

	m_pressedX = 0;
	m_pressedY = 0;
	m_pPressedLootSlotItem = nullptr;

	m_pStorageItem = nullptr;

	// Load delay
	m_loadDelay = false;
	m_loadDelayTime = 0.0f;

	m_loaded = false;
}

LootGUI::~LootGUI()
{
	DeleteLootItems();

	delete m_pLootWindow;
	delete m_pTitleBarIcon;
	delete m_pTitleBarBackgroundIcon;
	delete m_pLootWindowBackgroundIcon;
	delete m_pCloseExitButton;

	delete m_pLootButtonBackgroundSlotBorderCommon;
	delete m_pLootButtonBackgroundSlotBorderUncommon;
	delete m_pLootButtonBackgroundSlotBorderMagical;
	delete m_pLootButtonBackgroundSlotBorderRare;
	delete m_pLootButtonBackgroundSlotBorderEpic;

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

void LootGUI::SetInventoryGUI(InventoryGUI* pInventoryGUI)
{
	m_pInventoryGUI = pInventoryGUI;
}

void LootGUI::SetCharacterGUI(CharacterGUI* pCharacterGUI)
{
	m_pCharacterGUI = pCharacterGUI;
}

void LootGUI::SetActionBar(ActionBar* pActionBar)
{
	m_pActionBar = pActionBar;
}

void LootGUI::SetWindowDimensions(int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	m_lootWindowWidth = 412;
	m_lootWindowHeight = 212;
	m_titlebarWidth = 108;
	m_titlebarHeight = 35;

	m_pLootWindow->SetDimensions(m_windowWidth - 434, 450, m_lootWindowWidth, m_lootWindowHeight);
	m_pLootWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);
	m_pLootWindow->SetTitleOffset(50, 5);
	m_pLootWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pLootWindow->SetApplicationBorder(25, 15, 10, 40);

	m_pTitleBarIcon->SetDimensions(0, m_lootWindowHeight, 44, 44);

	m_pCloseExitButton->SetDimensions(m_lootWindowWidth - 32, m_lootWindowHeight, 32, 32);
}

// Skinning the GUI
void LootGUI::SkinGUI() const
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/common/items/border_common.tga";
	m_pLootButtonBackgroundSlotBorderCommon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_uncommon.tga";
	m_pLootButtonBackgroundSlotBorderUncommon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_magical.tga";
	m_pLootButtonBackgroundSlotBorderMagical->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_rare.tga";
	m_pLootButtonBackgroundSlotBorderRare->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_epic.tga";
	m_pLootButtonBackgroundSlotBorderEpic->SetIcon(iconName);
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

	iconName = "Resources/textures/gui/" + themeName + "/LootGUI/loot_title_icon.tga";
	m_pTitleBarIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/LootGUI/loot_window_background.tga";
	m_pLootWindowBackgroundIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/LootGUI/titlebar_background.tga";
	m_pTitleBarBackgroundIcon->SetIcon(iconName);

	m_pLootWindow->SetBackgroundIcon(m_pLootWindowBackgroundIcon);
	m_pLootWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	Point location = m_pLootWindow->GetLocation();
	m_pLootWindow->SetDimensions(location.x, location.y, m_lootWindowWidth, m_lootWindowHeight);
	m_pLootWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void LootGUI::UnSkinGUI() const
{
	// Do nothing
}

// Loading
void LootGUI::Load(bool loadDelay, float loadDelayTime)
{
	m_loadDelay = loadDelay;
	m_loadDelayTime = loadDelayTime;

	CreateLootItems();

	m_pLootWindow->DepthSortComponentChildren();
	if (m_loadDelay == false)
	{
		m_pGUI->AddWindow(m_pLootWindow);
		m_pLootWindow->Show();
	}

	m_pressedX = 0;
	m_pressedY = 0;
	m_pPressedLootSlotItem = nullptr;

	m_toolTipVisible = false;
	m_tooltipAppearDelayTimer = 0.0f;
	m_toolTipComponentsAdded = false;
	m_tooltipQuality = ItemQuality::Common;

	m_loaded = true;
}

void LootGUI::Unload()
{
	m_loadDelay = false;
	m_loadDelayTime = 0.0f;

	HideTooltip();

	DeleteLootItems();

	m_pGUI->RemoveWindow(m_pLootWindow);

	if (CubbyGame::GetInstance()->GetInteractItem() != nullptr && CubbyGame::GetInstance()->GetInteractItem()->GetItemType() == ItemType::Chest)
	{
		CubbyGame::GetInstance()->GetInteractItem()->Interact();
	}

	m_loaded = false;
}

bool LootGUI::IsLoadDelayed() const
{
	return (m_loadDelay == true && m_loadDelayTime > 0.0f);
}

bool LootGUI::IsLoaded() const
{
	return m_loaded;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void LootGUI::GetLootDimensions(int indexX, int indexY, int* x, int* y, int* width, int* height) const
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

GUIWindow* LootGUI::GetLootWindow() const
{
	return m_pLootWindow;
}

LootSlotItem* LootGUI::GetLootSlotItem(int x, int y)
{
	for (size_t i = 0; i < m_vpLootSlotItems.size(); ++i)
	{
		if (m_vpLootSlotItems[i]->m_slotX == x && m_vpLootSlotItems[i]->m_slotY == y)
		{
			return m_vpLootSlotItems[i];
		}
	}

	return nullptr;
}

void LootGUI::LoadItems(Item* pItem)
{
	m_pStorageItem = pItem;

	for (int i = 0; i < pItem->GetNumLootItems(); ++i)
	{
		InventoryItem* pAddItem = pItem->GetLootItem(i);

		//AddLootItem(pAddItem->m_fileName, pAddItem->m_iconFileName, pAddItem->m_itemType, pAddItem->m_item, pAddItem->m_equipSlot, pAddItem->m_title, pAddItem->m_description, pAddItem->m_placementR, pAddItem->m_placementG, pAddItem->m_placementB, pAddItem->m_quantity, pAddItem->m_lootSlotX, pAddItem->m_lootSlotY);
		AddLootItem(pAddItem, pAddItem->m_lootSlotX, pAddItem->m_lootSlotY);
	}
}

void LootGUI::AddLootItemFromInventory(InventoryItem* pInventoryItem, int x, int y)
{
	InventoryItem* pItem = m_pStorageItem->AddLootItem(pInventoryItem->m_fileName.c_str(), pInventoryItem->m_iconFileName.c_str(), pInventoryItem->m_itemType, pInventoryItem->m_item, pInventoryItem->m_status, pInventoryItem->m_equipSlot, pInventoryItem->m_itemQuality, pInventoryItem->m_title.c_str(), pInventoryItem->m_description.c_str(), pInventoryItem->m_left, pInventoryItem->m_right, pInventoryItem->m_placementR, pInventoryItem->m_placementG, pInventoryItem->m_placementB, pInventoryItem->m_quantity, x, y);

	for (size_t i = 0; i < pInventoryItem->m_vpStatAttributes.size(); ++i)
	{
		pItem->AddStatAttribute(pInventoryItem->m_vpStatAttributes[i]->GetType(), pInventoryItem->m_vpStatAttributes[i]->GetModifyAmount());
	}

	pItem->m_scale = pInventoryItem->m_scale;
	pItem->m_offsetX = pInventoryItem->m_offsetX;
	pItem->m_offsetY = pInventoryItem->m_offsetY;
	pItem->m_offsetZ = pInventoryItem->m_offsetZ;

	m_pGUI->RemoveWindow(m_pLootWindow);

	DeleteLootItems();
	LoadItems(m_pStorageItem);
	CreateLootItems();

	m_pGUI->AddWindow(m_pLootWindow);
	m_pLootWindow->Show();
}

void LootGUI::AddLootItem(InventoryItem* pInventoryItem, int x, int y)
{
	LootSlotItem* lpSlotItem = new LootSlotItem();

	// Copying pointer directly, no need to duplicate or allocate new space for contents or attributes
	lpSlotItem->m_pInventoryItem = pInventoryItem;

	lpSlotItem->m_slotX = x;
	lpSlotItem->m_slotY = y;

	lpSlotItem->m_lootingAnimation = false;

	lpSlotItem->m_erase = false;

	lpSlotItem->m_pLootGUI = this;

	m_vpLootSlotItems.push_back(lpSlotItem);
}

void LootGUI::AddLootItem(const char* filename, const char* iconFilename, InventoryType itemType, ItemType item, EquipSlot equipSlot, ItemQuality itemQuality, const char* title, const char* description, bool left, bool right, float r, float g, float b, int quantity, int x, int y)
{
	LootSlotItem* pSlotItem = new LootSlotItem();

	// Create the actual inventory item
	{
		InventoryItem* pNewItem = new InventoryItem();

		pNewItem->m_fileName = filename;
		pNewItem->m_iconFileName = iconFilename;
		pNewItem->m_title = title;
		pNewItem->m_description = description;

		pNewItem->m_itemType = itemType;
		pNewItem->m_item = item;
		pNewItem->m_equipSlot = equipSlot;
		pNewItem->m_itemQuality = itemQuality;

		pNewItem->m_left = left;
		pNewItem->m_right = right;

		pNewItem->m_placementR = r;
		pNewItem->m_placementG = g;
		pNewItem->m_placementB = b;

		pNewItem->m_equipped = false;

		pNewItem->m_quantity = quantity;

		pNewItem->m_remove = false;

		pSlotItem->m_pInventoryItem = pNewItem;
	}

	pSlotItem->m_slotX = x;
	pSlotItem->m_slotY = y;

	pSlotItem->m_lootingAnimation = false;

	pSlotItem->m_erase = false;

	pSlotItem->m_pLootGUI = this;

	m_vpLootSlotItems.push_back(pSlotItem);
}

bool IsNeedErase(LootSlotItem* item)
{
	bool isNeedErase = item->m_erase;

	if (isNeedErase == true)
	{
		delete item;
	}

	return isNeedErase;
}

void LootGUI::RemoveLootItem(LootSlotItem* plootItemSlot)
{
	m_pLootWindow->RemoveComponent(plootItemSlot->m_pInventoryIcon);

	delete plootItemSlot->m_pInventoryIcon;

	plootItemSlot->m_erase = true;
	m_vpLootSlotItems.erase(remove_if(m_vpLootSlotItems.begin(), m_vpLootSlotItems.end(), IsNeedErase), m_vpLootSlotItems.end());
}

void LootGUI::CreateLootItems()
{
	// Item draggable Buttons
	for (size_t i = 0; i < m_vpLootSlotItems.size(); ++i)
	{
		InventoryItem* pItem = m_vpLootSlotItems[i]->m_pInventoryItem;

		if (pItem != nullptr)
		{
			int x;
			int y;
			int width;
			int height;
			GetLootDimensions(m_vpLootSlotItems[i]->m_slotX, m_vpLootSlotItems[i]->m_slotY, &x, &y, &width, &height);

			DraggableRenderRectangle* pNewSlotItem = new DraggableRenderRectangle(m_pRenderer);
			switch (pItem->m_itemQuality)
			{
			case ItemQuality::Common: { pNewSlotItem->SetIcon(m_pLootButtonBackgroundSlotBorderCommon); break; }
			case ItemQuality::Uncommon: { pNewSlotItem->SetIcon(m_pLootButtonBackgroundSlotBorderUncommon); break; }
			case ItemQuality::Magical: { pNewSlotItem->SetIcon(m_pLootButtonBackgroundSlotBorderMagical); break; }
			case ItemQuality::Rare: { pNewSlotItem->SetIcon(m_pLootButtonBackgroundSlotBorderRare); break; }
			case ItemQuality::Epic: { pNewSlotItem->SetIcon(m_pLootButtonBackgroundSlotBorderEpic); break; }
			default: throw std::logic_error("Invalid ItemQuality in CreateLootItems()");
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
			default: throw std::logic_error("Invalid ItemQuality in CreateLootItems()");
			}

			if (pItem->m_quantity != -1)
			{
				char quantity[128];
				sprintf(quantity, "%i", pItem->m_quantity);
				int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18(), "%s", quantity);
				pNewSlotItem->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), quantity, Color(1.0f, 1.0f, 1.0f, 1.0f), width - 10 - textWidth, 8, true, Color(0.0f, 0.0f, 0.0f, 1.0f));
			}

			m_vpLootSlotItems[i]->m_pInventoryIcon = pNewSlotItem;

			pNewSlotItem->SetPressedCallBackFunction(_LootItemPressed);
			pNewSlotItem->SetPressedCallBackData(m_vpLootSlotItems[i]);

			pNewSlotItem->SetReleasedCallBackFunction(_LootItemReleased);
			pNewSlotItem->SetReleasedCallBackData(m_vpLootSlotItems[i]);

			pNewSlotItem->SetEnterCallBackFunction(_LootItemEntered);
			pNewSlotItem->SetEnterCallBackData(m_vpLootSlotItems[i]);

			pNewSlotItem->SetExitCallBackFunction(_LootItemExited);
			pNewSlotItem->SetExitCallBackData(m_vpLootSlotItems[i]);

			m_pLootWindow->AddComponent(pNewSlotItem);
		}
	}
}

void LootGUI::DeleteLootItems()
{
	// Clear item draggable buttons
	for (size_t i = 0; i < m_vpLootSlotItems.size(); ++i)
	{
		if (m_vpLootSlotItems[i]->m_lootingAnimation == false)
		{
			m_pLootWindow->RemoveComponent(m_vpLootSlotItems[i]->m_pInventoryIcon);
			delete m_vpLootSlotItems[i]->m_pInventoryIcon;

			delete m_vpLootSlotItems[i];
			m_vpLootSlotItems[i] = nullptr;
		}
	}
	m_vpLootSlotItems.clear();
}

// Tooltips
void LootGUI::UpdateToolTipAppear(float dt)
{
	if (m_toolTipVisible)
	{
		if (m_tooltipAppearDelayTimer <= 0.0f)
		{
			if (m_toolTipComponentsAdded == false)
			{
				switch (m_tooltipQuality)
				{
				case ItemQuality::Common: { m_pLootWindow->AddComponent(m_pTooltipBackgroundCommon); break; }
				case ItemQuality::Uncommon: { m_pLootWindow->AddComponent(m_pTooltipBackgroundUncommon); break; }
				case ItemQuality::Magical: { m_pLootWindow->AddComponent(m_pTooltipBackgroundMagical); break; }
				case ItemQuality::Rare: { m_pLootWindow->AddComponent(m_pTooltipBackgroundRare); break; }
				case ItemQuality::Epic: { m_pLootWindow->AddComponent(m_pTooltipBackgroundEpic); break; }
				default: throw std::logic_error("Invalid ItemQuality in UpdateToolTipAppear()");
				}

				m_pLootWindow->AddComponent(m_pTooltipNameLabel);
				m_pLootWindow->AddComponent(m_pTooltipDescriptionLabel);
				m_pLootWindow->AddComponent(m_pTooltipSlotLabel);
				m_pLootWindow->AddComponent(m_pTooltipQualityLabel);

				m_toolTipComponentsAdded = true;
			}
		}
		else
		{
			m_tooltipAppearDelayTimer -= dt;
		}
	}
}

void LootGUI::ShowTooltip(LootSlotItem* pInventoryItem)
{
	if (m_toolTipVisible == true)
	{
		return;
	}

	// Set the focused window when we show a tooltip
	m_pLootWindow->SetFocusWindow();

	// Replace the tooltip name
	m_pTooltipNameLabel->SetText(pInventoryItem->m_pInventoryItem->m_title);

	// Replace the tooltip description
	std::string descriptionText = pInventoryItem->m_pInventoryItem->m_description + pInventoryItem->m_pInventoryItem->GetStatsAttributeString();
	m_pTooltipDescriptionLabel->SetText(descriptionText);

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
	GetLootDimensions(pInventoryItem->m_slotX, pInventoryItem->m_slotY, &x, &y, &width, &height);

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

void LootGUI::HideTooltip()
{
	m_pLootWindow->RemoveComponent(m_pTooltipBackgroundCommon);
	m_pLootWindow->RemoveComponent(m_pTooltipBackgroundUncommon);
	m_pLootWindow->RemoveComponent(m_pTooltipBackgroundMagical);
	m_pLootWindow->RemoveComponent(m_pTooltipBackgroundRare);
	m_pLootWindow->RemoveComponent(m_pTooltipBackgroundEpic);
	m_pLootWindow->RemoveComponent(m_pTooltipNameLabel);
	m_pLootWindow->RemoveComponent(m_pTooltipDescriptionLabel);
	m_pLootWindow->RemoveComponent(m_pTooltipSlotLabel);
	m_pLootWindow->RemoveComponent(m_pTooltipQualityLabel);

	m_toolTipVisible = false;
}

void LootGUI::Update(float dt)
{
	if (m_loadDelay == true)
	{
		if (m_loadDelayTime <= 0.0f)
		{
			m_loadDelay = false;
			m_pGUI->AddWindow(m_pLootWindow);
			m_pLootWindow->Show();
		}
		else
		{
			m_loadDelayTime -= dt;
		}
	}

	UpdateToolTipAppear(dt);
}

void LootGUI::_LootItemPressed(void* pData)
{
	LootSlotItem* pLootItem = static_cast<LootSlotItem*>(pData);
	pLootItem->m_pLootGUI->LootItemPressed(pLootItem);
}

void LootGUI::LootItemPressed(LootSlotItem* pLootItem)
{
	m_pPressedLootSlotItem = pLootItem;

	Dimensions dimensions = m_pPressedLootSlotItem->m_pInventoryIcon->GetDimensions();
	m_pressedX = dimensions.x;
	m_pressedY = dimensions.y;

	// Temporarily increase the depth of the dragged icon
	m_pPressedLootSlotItem->m_pInventoryIcon->SetDepth(5.0f);
	//m_pPressedLootSlotItem->m_pInventoryIcon->SetLocation(m_pressedX - 4, m_pressedY + 4);
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
	std::string dropShadowIcon = "Resources/textures/gui/" + themeName + "/common/items/drop_shadow.tga";
	m_pPressedLootSlotItem->m_pInventoryIcon->AddIcon(m_pRenderer, dropShadowIcon.c_str(), 64, 64, 64, 64, 4, -4, 0.5f);
	m_pLootWindow->DepthSortComponentChildren();

	HideTooltip();
}

void LootGUI::_LootItemReleased(void* pData)
{
	LootSlotItem* pLootItem = static_cast<LootSlotItem*>(pData);
	pLootItem->m_pLootGUI->LootItemReleased(pLootItem);
}

void LootGUI::LootItemReleased(LootSlotItem* pLootItem)
{
	if (m_pPressedLootSlotItem == nullptr)
	{
		return;
	}

	m_pPressedLootSlotItem = nullptr;

	int x;
	int y;
	int width;
	int height;
	POINT mouse = { CubbyGame::GetInstance()->GetWindowCursorX(), (m_windowHeight - CubbyGame::GetInstance()->GetWindowCursorY()) };
	bool switched = false;
	bool sameSlot = false;
	bool draggedToInventory = false;

	// Figure out if we need to change to a different loot slot
	for (int i = 0; i < MAX_NUM_SLOTS_VERTICAL && switched == false; ++i)
	{
		for (int j = 0; j < MAX_NUM_SLOTS_HORIZONTAL && switched == false; ++j)
		{
			GetLootDimensions(j, i, &x, &y, &width, &height);

			// Check if we released (mouse cursor) in the boundary of another slot
			if (mouse.x > m_pLootWindow->GetDimensions().x + x && mouse.x < m_pLootWindow->GetDimensions().x + x + width && mouse.y > m_pLootWindow->GetDimensions().y + y && mouse.y < m_pLootWindow->GetDimensions().y + y + height)
			{
				// Check if another loot item already exists in this slot
				LootSlotItem* pLootSlotItem = GetLootSlotItem(j, i);

				if (pLootSlotItem == pLootItem)
				{
					sameSlot = true;
				}

				// Switch the loot item in this slot to the pressed (previous) position
				if (pLootSlotItem != nullptr)
				{
					pLootSlotItem->m_slotX = pLootItem->m_slotX;
					pLootSlotItem->m_slotY = pLootItem->m_slotY;
					pLootSlotItem->m_pInventoryItem->m_lootSlotX = pLootItem->m_slotX;
					pLootSlotItem->m_pInventoryItem->m_lootSlotY = pLootItem->m_slotY;
					pLootSlotItem->m_pInventoryIcon->SetLocation(m_pressedX, m_pressedY);
				}

				// Set the new location for the released loot icon
				pLootItem->m_slotX = j;
				pLootItem->m_slotY = i;
				pLootItem->m_pInventoryItem->m_lootSlotX = j;
				pLootItem->m_pInventoryItem->m_lootSlotY = i;
				pLootItem->m_pInventoryIcon->SetLocation(x, y);

				switched = true;
			}
		}
	}

	bool equipped = false;
	if (switched == false)
	{
		if (pLootItem->m_pInventoryItem->m_equipSlot != EquipSlot::NoSlot)
		{
			// Check if we need to equip the item after dropping on the player portrait
			if (m_pCharacterGUI->IsLoaded())
			{
				m_pCharacterGUI->GetPlayerPortraitDimensions(&x, &y, &width, &height);
				GUIWindow* pCharacterWindow = m_pCharacterGUI->GetCharacterWindow();
				if (mouse.x > pCharacterWindow->GetDimensions().x + x && mouse.x < pCharacterWindow->GetDimensions().x + x + width && mouse.y > pCharacterWindow->GetDimensions().y + y && mouse.y < pCharacterWindow->GetDimensions().y + y + height)
				{
					if (pLootItem->m_pInventoryItem->m_equipped == false)
					{
						InventoryItem* pPreviousEquipped = m_pInventoryManager->GetInventoryItemForEquipSlot(pLootItem->m_pInventoryItem->m_equipSlot);
						int lootSlotX = pLootItem->m_slotX;
						int lootSlotY = pLootItem->m_slotY;

						InventoryItem* pInventoryItem = m_pInventoryManager->GetInventoryItemForEquipSlot(pLootItem->m_pInventoryItem->m_equipSlot);
						if (pInventoryItem != nullptr)
						{
							m_pActionBar->RemoveInventoryItemFromActionBar(pInventoryItem->m_title);
						}

						m_pPlayer->UnequipItem(pLootItem->m_pInventoryItem->m_equipSlot, pLootItem->m_pInventoryItem->m_left, pLootItem->m_pInventoryItem->m_right);
						m_pInventoryManager->EquipLootItem(pLootItem->m_slotX, pLootItem->m_slotY, pLootItem->m_pInventoryItem, pLootItem->m_pInventoryItem->m_equipSlot);
						m_pInventoryGUI->EquipItem(pLootItem);

						m_pLootWindow->RemoveComponent(pLootItem->m_pInventoryIcon);

						m_pStorageItem->RemoveLootItem(pLootItem->m_pInventoryItem);
						m_pInventoryManager->ExportInventory(m_pPlayer->GetName());

						pLootItem->m_erase = true;
						m_vpLootSlotItems.erase(remove_if(m_vpLootSlotItems.begin(), m_vpLootSlotItems.end(), IsNeedErase), m_vpLootSlotItems.end());

						if (pPreviousEquipped != nullptr)
						{
							AddLootItemFromInventory(pPreviousEquipped, lootSlotX, lootSlotY);
						}

						m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
						m_pInventoryManager->SetCharacterGUINeedsUpdate(true);

						m_pCharacterGUI->HideEquipHover();

						equipped = true;
					}
				}

				if (equipped == false)
				{
					// Check if we need to equip the item after dropping on a equipment slot
					switch (pLootItem->m_pInventoryItem->m_equipSlot)
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
					default: throw std::logic_error("Invalid EquipSlot in LootItemReleased()");
					}
					GUIWindow* pCharacterWindow2 = m_pCharacterGUI->GetCharacterWindow();
					if (mouse.x > pCharacterWindow2->GetDimensions().x + x && mouse.x < pCharacterWindow2->GetDimensions().x + x + width && mouse.y > pCharacterWindow2->GetDimensions().y + y && mouse.y < pCharacterWindow2->GetDimensions().y + y + height)
					{
						if (pLootItem->m_pInventoryItem->m_equipped == false)
						{
							InventoryItem* pPreviousEquipped = m_pInventoryManager->GetInventoryItemForEquipSlot(pLootItem->m_pInventoryItem->m_equipSlot);
							int lootSlotX = pLootItem->m_slotX;
							int lootSlotY = pLootItem->m_slotY;

							InventoryItem* pInventoryItem = m_pInventoryManager->GetInventoryItemForEquipSlot(pLootItem->m_pInventoryItem->m_equipSlot);
							if (pInventoryItem != nullptr)
							{
								m_pActionBar->RemoveInventoryItemFromActionBar(pInventoryItem->m_title);
							}

							//m_pPlayer->UnequipItem(pLootItem->m_pInventoryItem->m_equipSlot);
							m_pInventoryManager->EquipLootItem(pLootItem->m_slotX, pLootItem->m_slotY, pLootItem->m_pInventoryItem, pLootItem->m_pInventoryItem->m_equipSlot);
							m_pInventoryGUI->EquipItem(pLootItem);

							m_pLootWindow->RemoveComponent(pLootItem->m_pInventoryIcon);

							m_pStorageItem->RemoveLootItem(pLootItem->m_pInventoryItem);
							m_pInventoryManager->ExportInventory(m_pPlayer->GetName());

							pLootItem->m_erase = true;
							m_vpLootSlotItems.erase(remove_if(m_vpLootSlotItems.begin(), m_vpLootSlotItems.end(), IsNeedErase), m_vpLootSlotItems.end());

							if (pPreviousEquipped != nullptr)
							{
								AddLootItemFromInventory(pPreviousEquipped, lootSlotX, lootSlotY);
							}

							m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
							m_pInventoryManager->SetCharacterGUINeedsUpdate(true);

							m_pCharacterGUI->HideEquipHover();

							equipped = true;
						}
					}
				}
			}
		}
	}


	if (equipped == false)
	{
		// Figure out if we have released on an inventory slot (if the inventory is open)
		if (m_pInventoryGUI->IsLoaded())
		{
			for (int i = 0; i < InventoryGUI::MAX_NUM_SLOTS_VERTICAL; ++i)
			{
				for (int j = 0; j < InventoryGUI::MAX_NUM_SLOTS_HORIZONTAL; ++j)
				{
					m_pInventoryGUI->GetInventoryDimensions(j, i, &x, &y, &width, &height);
					GUIWindow* pInventoryWindow = m_pInventoryGUI->GetInventoryWindow();
					if (mouse.x > pInventoryWindow->GetDimensions().x + x && mouse.x < pInventoryWindow->GetDimensions().x + x + width && mouse.y > pInventoryWindow->GetDimensions().y + y && mouse.y < pInventoryWindow->GetDimensions().y + y + height)
					{
						// Check if another inventory item already exists in this slot
						InventorySlotItem* pInventorySlotItem = m_pInventoryGUI->GetInventorySlotItem(j, i);
						if (pInventorySlotItem == nullptr)
						{
							m_pGUI->RemoveComponent(pLootItem->m_pInventoryIcon);

							m_pInventoryManager->AddInventoryItem(pLootItem->m_pInventoryItem, j, i);
							m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
							m_pInventoryManager->SetCharacterGUINeedsUpdate(true);
							m_pInventoryManager->ExportInventory(m_pPlayer->GetName());

							m_pStorageItem->RemoveLootItem(pLootItem->m_pInventoryItem);

							RemoveLootItem(pLootItem);

							switched = true;
							draggedToInventory = true;
						}
						else
						{
						}
					}
				}
			}
		}
	}

	if (switched && draggedToInventory == false && sameSlot == false && equipped == false)
	{
		ShowTooltip(pLootItem);
	}

	if (sameSlot)
	{
		if (m_pInventoryManager->CanAddInventoryItem(pLootItem->m_pInventoryItem->m_title.c_str(), pLootItem->m_pInventoryItem->m_item, pLootItem->m_pInventoryItem->m_quantity))
		{
			m_pGUI->RemoveComponent(pLootItem->m_pInventoryIcon);

			m_pInventoryManager->AddInventoryItem(pLootItem->m_pInventoryItem, -1, -1);
			m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
			m_pInventoryManager->SetCharacterGUINeedsUpdate(true);
			m_pInventoryManager->ExportInventory(m_pPlayer->GetName());

			m_pStorageItem->RemoveLootItem(pLootItem->m_pInventoryItem);

			RemoveLootItem(pLootItem);

			m_pCharacterGUI->HideEquipHover();
		}
	}

	// Reset back to the original position
	if (switched == false && equipped == false)
	{
		pLootItem->m_pInventoryIcon->SetLocation(m_pressedX, m_pressedY);
		std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
		std::string dropShadowIcon = "Resources/textures/gui/" + themeName + "/common/items/drop_shadow.tga";
		pLootItem->m_pInventoryIcon->RemoveIcon(dropShadowIcon.c_str());
	}

	if (sameSlot == false && equipped == false && draggedToInventory == false)
	{
		// Revert depth back to normal for loot icons
		pLootItem->m_pInventoryIcon->SetDepth(3.0f);
		std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
		std::string dropShadowIcon = "Resources/textures/gui/" + themeName + "/common/items/drop_shadow.tga";
		pLootItem->m_pInventoryIcon->RemoveIcon(dropShadowIcon.c_str());
		m_pLootWindow->DepthSortComponentChildren();
	}
}

void LootGUI::_LootItemEntered(void* pData)
{
	LootSlotItem* pLootItem = static_cast<LootSlotItem*>(pData);
	pLootItem->m_pLootGUI->LootItemEntered(pLootItem);
}

void LootGUI::LootItemEntered(LootSlotItem* pLootItem)
{
	if (pLootItem->m_lootingAnimation == false)
	{
		ShowTooltip(pLootItem);
		if (m_pCharacterGUI->IsLoaded())
		{
			m_pCharacterGUI->ShowEquipHover(pLootItem->m_pInventoryItem->m_equipSlot);
		}
	}
}

void LootGUI::_LootItemExited(void* pData)
{
	LootSlotItem* pLootItem = static_cast<LootSlotItem*>(pData);
	pLootItem->m_pLootGUI->LootItemExited(pLootItem);
}

void LootGUI::LootItemExited(LootSlotItem* pLootItem)
{
	HideTooltip();
	m_pCharacterGUI->HideEquipHover();
}

void LootGUI::_CloseExitPressed(void *pData)
{
	LootGUI* pLootGUI = static_cast<LootGUI*>(pData);
	pLootGUI->CloseExitPressed();
}

void LootGUI::CloseExitPressed()
{
	Unload();

	if (CubbyGame::GetInstance()->IsGUIWindowStillDisplayed() == false)
	{
		CubbyGame::GetInstance()->TurnCursorOff(false);
	}
}