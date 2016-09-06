/*************************************************************************
> File Name: ActionBar.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Action bar class.
> Created Time: 2016/08/28
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>
#include <algorithm>

#include "ActionBar.h"
#include "CubbyGame.h"

// Constructor, Destructor
ActionBar::ActionBar(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight)
{
	m_pRenderer = pRenderer;
	m_pGUI = pGUI;
	m_pFrontendManager = pFrontendManager;
	m_pChunkManager = pChunkManager;
	m_pPlayer = pPlayer;
	m_pInventoryManager = pInventoryManager;

	m_isLoaded = false;

	m_supressExport = false;

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	m_pActionButtonBackground6Icon = new Icon(m_pRenderer, "", 392, 76);
	m_pActionButtonBackground6Icon->SetDepth(2.0f);

	m_pActionButtonBackgroundSlotBorderCommon = new Icon(m_pRenderer, "", 64, 64);
	m_pActionButtonBackgroundSlotBorderCommon->SetDepth(2.0f);

	m_pActionButtonBackgroundSlotBorderUncommon = new Icon(m_pRenderer, "", 64, 64);
	m_pActionButtonBackgroundSlotBorderUncommon->SetDepth(2.0f);

	m_pActionButtonBackgroundSlotBorderMagical = new Icon(m_pRenderer, "", 64, 64);
	m_pActionButtonBackgroundSlotBorderMagical->SetDepth(2.0f);

	m_pActionButtonBackgroundSlotBorderRare = new Icon(m_pRenderer, "", 64, 64);
	m_pActionButtonBackgroundSlotBorderRare->SetDepth(2.0f);

	m_pActionButtonBackgroundSlotBorderEpic = new Icon(m_pRenderer, "", 64, 64);
	m_pActionButtonBackgroundSlotBorderEpic->SetDepth(2.0f);

	SetWindowDimensions(m_windowWidth, m_windowHeight);
}

ActionBar::~ActionBar()
{
	delete m_pActionButtonBackground6Icon;

	delete m_pActionButtonBackgroundSlotBorderCommon;
	delete m_pActionButtonBackgroundSlotBorderUncommon;
	delete m_pActionButtonBackgroundSlotBorderMagical;
	delete m_pActionButtonBackgroundSlotBorderRare;
	delete m_pActionButtonBackgroundSlotBorderEpic;

	DeleteActionSlots();
	ClearActionSlotItems();
}

void ActionBar::SetInventoryGUI(InventoryGUI* pInventoryGUI)
{
	m_pInventoryGUI = pInventoryGUI;
}

void ActionBar::SetCharacterGUI(CharacterGUI* pCharacterGUI)
{
	m_pCharacterGUI = pCharacterGUI;
}

void ActionBar::SetLootGUI(LootGUI* pLootGUI)
{
	m_pLootGUI = pLootGUI;
}

// Skinning the GUI
void ActionBar::SkinGUI() const
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/ActionBar/actionbar_background_6.tga";
	m_pActionButtonBackground6Icon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_common.tga";
	m_pActionButtonBackgroundSlotBorderCommon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_uncommon.tga";
	m_pActionButtonBackgroundSlotBorderUncommon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_magical.tga";
	m_pActionButtonBackgroundSlotBorderMagical->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_rare.tga";
	m_pActionButtonBackgroundSlotBorderRare->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/items/border_epic.tga";
	m_pActionButtonBackgroundSlotBorderEpic->SetIcon(iconName);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ActionBar::UnSkinGUI() const
{
	// Do nothing
}

// Loading
void ActionBar::Load()
{
	m_pGUI->AddComponent(m_pActionButtonBackground6Icon);

	CreateActionSlots();
	CreateActionButtons();

	for (size_t i = 0; i < m_vpActionSlots.size(); ++i)
	{
		m_pGUI->AddComponent(m_vpActionSlots[i]);
	}

	for (size_t i = 0; i < m_vpActionLabels.size(); ++i)
	{
		m_pGUI->AddComponent(m_vpActionLabels[i]);
	}

	m_pGUI->SetDraggingComponentPriority(nullptr);

	m_isLoaded = true;
}

void ActionBar::Unload()
{
	for (size_t i = 0; i < m_vpActionSlots.size(); ++i)
	{
		m_pGUI->RemoveComponent(m_vpActionSlots[i]);
	}

	for (size_t i = 0; i < m_vpActionLabels.size(); ++i)
	{
		m_pGUI->RemoveComponent(m_vpActionLabels[i]);
	}

	m_pGUI->RemoveComponent(m_pActionButtonBackground6Icon);

	ClearActionButtons();
	ClearActionSlotItems();
	DeleteActionSlots();

	m_isLoaded = false;
}

bool ActionBar::IsLoaded() const
{
	return m_isLoaded;
}

void ActionBar::SetWindowDimensions(int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	m_pActionButtonBackground6Icon->SetDimensions(m_windowWidth / 2 - 392 / 2, 12, 392, 76);

	m_actionSlotSize = 64;
	m_actionSlotSpacer = 0;
	m_actionBarX = static_cast<int>(m_windowWidth*0.5f - (m_actionSlotSize*MAX_NUM_ACTION_SLOTS)*0.5f - (m_actionSlotSpacer*(MAX_NUM_ACTION_SLOTS - 1))*0.5f);
	m_actionBarY = 20;

	RepositionActionSlots();
}

// Supress export
void ActionBar::SetSupressExport(bool supress)
{
	m_supressExport = supress;
}

// Import / Export action bar
void ActionBar::ExportActionBar(std::string playerName)
{
	if (m_supressExport)
	{
		return;
	}

	std::ofstream exportFile;
	char fileName[128];
	sprintf(fileName, "saves/characters/%s/%s.actionbar", playerName.c_str(), playerName.c_str());
	exportFile.open(fileName, std::ios_base::binary);

	if (exportFile.is_open())
	{
		// Export the stored inventory items
		exportFile << m_vpActionSlotItems.size() << "|";

		for (int i = 0; i < MAX_NUM_ACTION_SLOTS; ++i)
		{
			ActionButtonItem* pAction = GetActionButtonForSlot(i);

			if (pAction != nullptr)
			{
				InventoryItem* pInventoryItem = m_pInventoryManager->GetInventoryItemWithTitle(pAction->m_itemTitle);

				exportFile << pAction->m_slotNum << "|";
				exportFile << pAction->m_inventoryX << "|";
				exportFile << pAction->m_inventoryY << "|";
				exportFile << static_cast<int>(pAction->m_equipSlot) << "|";
				exportFile << (pInventoryItem == nullptr ? false : pInventoryItem->m_equipped) << "|";
			}
		}

		// Export the file signature
		exportFile << "signature: " << "\n";

		exportFile.close();
	}
}

void ActionBar::ImportActionBar(std::string playerName)
{
	std::ifstream importFile;
	char fileName[128];
	sprintf(fileName, "saves/characters/%s/%s.actionbar", playerName.c_str(), playerName.c_str());
	importFile.open(fileName, std::ios_base::binary);

	if (importFile.is_open())
	{
		char* input = new char[128];
		char* pEnd;

		// Import the stored inventory items
		importFile.getline(input, 128, '|');
		int numitems = strtol(input, &pEnd, 10);

		for (int i = 0; i < numitems; i++)
		{
			importFile.getline(input, 128, '|');
			int slotIndex = strtol(input, &pEnd, 10);

			importFile.getline(input, 128, '|');
			int inventoryX = strtol(input, &pEnd, 10);

			importFile.getline(input, 128, '|');
			int inventoryY = strtol(input, &pEnd, 10);

			importFile.getline(input, 128, '|');
			int equipSlot = strtol(input, &pEnd, 10);

			importFile.getline(input, 128, '|');
			int equipped = strtol(input, &pEnd, 10);

			InventoryItem* pItem;

			if (static_cast<bool>(equipped) == true)
			{
				pItem = m_pInventoryManager->GetInventoryItemForEquipSlot(static_cast<EquipSlot>(equipSlot));
			}
			else
			{
				pItem = m_pInventoryGUI->GetInventorySlotItem(inventoryX, inventoryY) != nullptr ? m_pInventoryGUI->GetInventorySlotItem(inventoryX, inventoryY)->m_pInventoryItem : nullptr;
			}

			if (pItem != nullptr)
			{
				AddItemToActionBar(pItem, slotIndex, inventoryX, inventoryY);
			}
		}

		// Import the file signature
		std::string tempString;
		std::string signature;
		importFile >> tempString >> signature;

		delete[] input;

		importFile.close();
	}
}

void ActionBar::GetActionSlotDimensions(int indexX, int* x, int* y, int* width, int* height) const
{
	int slotSize = m_actionSlotSize;

	int xSlotPos = m_actionBarX + (indexX * (m_actionSlotSpacer + m_actionSlotSize));
	int ySlotPos = m_actionBarY;

	*x = xSlotPos;
	*y = ySlotPos;

	*width = slotSize;
	*height = slotSize;
}

void ActionBar::AddItemToActionBar(InventoryItem* pInventoryItem, int slotIndex, int inventoryX, int inventoryY)
{
	// First check if any other action bar item already exists and remove it
	ActionButtonItem* pBarItem = GetActionButtonForSlot(slotIndex);
	if (pBarItem != nullptr)
	{
		RemoveItemFromActionBar(pBarItem);
	}

	ActionButtonItem* pActionButton = new ActionButtonItem();

	pActionButton->m_pActionBar = this;
	//pActionButton->m_pInventorySlotItem = pItem;
	//pActionButton->m_pItemIcon = pNewSlotItem;
	pActionButton->m_pItemIcon = nullptr;
	pActionButton->m_type = ActionBarItemType::Item;
	pActionButton->m_itemTitle = pInventoryItem->m_title;
	pActionButton->m_slotNum = slotIndex;
	pActionButton->m_inventoryX = inventoryX;
	pActionButton->m_inventoryY = inventoryY;
	pActionButton->m_equipSlot = pInventoryItem->m_equipSlot;
	pActionButton->m_erase = false;

	CreateActionButtonIcon(pActionButton);

	m_vpActionSlotItems.push_back(pActionButton);
}

bool IsNeedErase(ActionButtonItem* item)
{
	bool isNeedErase = item->m_erase;

	if (isNeedErase == true)
	{
		delete item;
	}

	return isNeedErase;
}

void ActionBar::RemoveItemFromActionBar(ActionButtonItem* pButton)
{
	pButton->m_erase = true;

	m_pGUI->RemoveComponent(pButton->m_pItemIcon);

	m_vpActionSlotItems.erase(remove_if(m_vpActionSlotItems.begin(), m_vpActionSlotItems.end(), IsNeedErase), m_vpActionSlotItems.end());
}

void ActionBar::RemoveInventoryItemFromActionBar(std::string itemTitle)
{
	for (size_t i = 0; i < m_vpActionSlotItems.size(); ++i)
	{
		if (strcmp(m_vpActionSlotItems[i]->m_itemTitle.c_str(), itemTitle.c_str()) == 0)
		{
			RemoveItemFromActionBar(m_vpActionSlotItems[i]);
		}
	}

	ExportActionBar(m_pPlayer->GetName());
}

void ActionBar::SwitchItems(int slot1, int slot2)
{
	ActionButtonItem* pSlot1 = GetActionButtonForSlot(slot1);
	ActionButtonItem* pSlot2 = GetActionButtonForSlot(slot2);

	if (pSlot1 != nullptr)
	{
		pSlot1->m_slotNum = slot2;

		int x;
		int y;
		int width;
		int height;
		GetActionSlotDimensions(slot2, &x, &y, &width, &height);
		pSlot1->m_pItemIcon->SetLocation(x, y);
	}

	if (pSlot2 != nullptr)
	{
		pSlot2->m_slotNum = slot1;

		int x;
		int y;
		int width;
		int height;
		GetActionSlotDimensions(slot1, &x, &y, &width, &height);
		pSlot2->m_pItemIcon->SetLocation(x, y);
	}
}

void ActionBar::UpdateActionBarSlots(InventoryItem* pInventoryItem, int slotX, int slotY)
{
	for (size_t i = 0; i < m_vpActionSlotItems.size(); ++i)
	{
		if (strcmp(m_vpActionSlotItems[i]->m_itemTitle.c_str(), pInventoryItem->m_title.c_str()) == 0)
		{
			m_vpActionSlotItems[i]->m_inventoryX = slotX;
			m_vpActionSlotItems[i]->m_inventoryY = slotY;
		}
	}

	ExportActionBar(m_pPlayer->GetName());
}

void ActionBar::UpdateActionBarSlots(std::string itemTitle, int slotX, int slotY)
{
	for (unsigned int i = 0; i < m_vpActionSlotItems.size(); i++)
	{
		if (strcmp(m_vpActionSlotItems[i]->m_itemTitle.c_str(), itemTitle.c_str()) == 0)
		{
			m_vpActionSlotItems[i]->m_inventoryX = slotX;
			m_vpActionSlotItems[i]->m_inventoryY = slotY;
		}
	}

	ExportActionBar(m_pPlayer->GetName());
}

void ActionBar::UseActionBarslot(int slotIndex)
{
	if (m_pInventoryGUI->IsLoaded())
	{
		// Don't allow to use the action bar when the inventory GUI is open.
		return;
	}

	if (m_pPlayer->IsCrafting())
	{
		// Don't allow to use the action bar when we are crafting.
		return;
	}

	ActionButtonItem* pItem = GetActionButtonForSlot(slotIndex);
	if (pItem != nullptr && pItem->m_inventoryX != -1 && pItem->m_inventoryY != -1)
	{
		if (pItem->m_type == ActionBarItemType::Item)
		{
			InventoryItem* pInventoryItem = m_pInventoryManager->GetInventoryItemWithTitle(pItem->m_itemTitle);

			if (pInventoryItem != nullptr)
			{
				if (pInventoryItem->m_itemType == InventoryType::Consumable)
				{
					// Consume the item
				}
				else
				{
					if (pInventoryItem->m_equipSlot != EquipSlot::NoSlot)
					{
						// Equip the item
						if (pInventoryItem->m_equipped == false)
						{
							// Dual handed weapon checks
							if (pInventoryItem->m_equipSlot == EquipSlot::RightHand)
							{
								InventoryItem* pLeftHanded = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::LeftHand);
								if (pInventoryItem->m_left || (pLeftHanded != nullptr && pLeftHanded->m_right))
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
										UpdateActionBarSlots(pLeftHanded, slotX, slotY);
									}
								}
							}
							if (pInventoryItem->m_equipSlot == EquipSlot::LeftHand)
							{
								InventoryItem* pRightHanded = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::RightHand);
								if (pInventoryItem->m_right || (pRightHanded != nullptr && pRightHanded->m_left))
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
										UpdateActionBarSlots(pRightHanded, slotX, slotY);
									}
								}
							}

							m_pPlayer->UnequipItem(pInventoryItem->m_equipSlot, pInventoryItem->m_left, pInventoryItem->m_right);

							m_pInventoryManager->EquipInventoryItem(pItem->m_inventoryX, pItem->m_inventoryY, pInventoryItem->m_equipSlot);
							m_pInventoryGUI->EquipItem(pInventoryItem, pItem->m_inventoryX, pItem->m_inventoryY);

							m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
							m_pInventoryManager->SetCharacterGUINeedsUpdate(true);
						}
					}
				}
			}
			else
			{
				// Try to see if we have an equipped consumable item that we need to consume,
				InventoryItem* pEquippedItem = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::LeftHand);

				if (pEquippedItem != nullptr)
				{
					if (strcmp(pItem->m_itemTitle.c_str(), pEquippedItem->m_title.c_str()) == 0)
					{
						if (pEquippedItem->m_itemType == InventoryType::Consumable)
						{
							// Consume the item
						}
					}
				}
			}
		}
	}

	ExportActionBar(m_pPlayer->GetName());
}

void ActionBar::CreateActionButtonIcon(ActionButtonItem* pActionButton) const
{
	int x;
	int y;
	int width;
	int height;
	GetActionSlotDimensions(pActionButton->m_slotNum, &x, &y, &width, &height);

	DraggableRenderRectangle* pNewSlotItem = new DraggableRenderRectangle(m_pRenderer);

	InventoryItem* pInventoryItem;
	if (pActionButton->m_inventoryX == -1 && pActionButton->m_inventoryY == -1)
	{
		pInventoryItem = m_pInventoryManager->GetInventoryItemForEquipSlot(pActionButton->m_equipSlot);
	}
	else
	{
		pInventoryItem = m_pInventoryManager->GetInventoryItemWithTitle(pActionButton->m_itemTitle);
	}

	switch (pInventoryItem->m_itemQuality)
	{
	case ItemQuality::Common: { pNewSlotItem->SetIcon(m_pActionButtonBackgroundSlotBorderCommon); break; }
	case ItemQuality::Uncommon: { pNewSlotItem->SetIcon(m_pActionButtonBackgroundSlotBorderUncommon); break; }
	case ItemQuality::Magical: { pNewSlotItem->SetIcon(m_pActionButtonBackgroundSlotBorderMagical); break; }
	case ItemQuality::Rare: { pNewSlotItem->SetIcon(m_pActionButtonBackgroundSlotBorderRare); break; }
	case ItemQuality::Epic: { pNewSlotItem->SetIcon(m_pActionButtonBackgroundSlotBorderEpic); break; }
	default: throw std::logic_error("Invalid Item Quality in CreateActionButtonIcon()");
	}
	pNewSlotItem->SetDimensions(x, y, width, height);
	pNewSlotItem->SetDepth(8.0f);

	char itemTexture[128];
	sprintf(itemTexture, "%s", pInventoryItem->m_iconFileName.c_str());
	pNewSlotItem->AddIcon(m_pRenderer, itemTexture, 64, 64, 56, 56, 4, 4, 1.5f);

	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
	switch (pInventoryItem->m_itemQuality)
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
	}

	pActionButton->m_pItemIcon = pNewSlotItem;

	pNewSlotItem->SetPressedCallBackFunction(_ActionButtonPressed);
	pNewSlotItem->SetPressedCallBackData(pActionButton);

	pNewSlotItem->SetReleasedCallBackFunction(_ActionButtonReleased);
	pNewSlotItem->SetReleasedCallBackData(pActionButton);

	pNewSlotItem->SetEnterCallBackFunction(_ActionButtonEntered);
	pNewSlotItem->SetEnterCallBackData(pActionButton);

	pNewSlotItem->SetExitCallBackFunction(_ActionButtonExited);
	pNewSlotItem->SetExitCallBackData(pActionButton);

	m_pGUI->AddComponent(pNewSlotItem);
}

void ActionBar::CreateActionButtons()
{
	for (size_t i = 0; i < m_vpActionSlotItems.size(); ++i)
	{
		if (m_vpActionSlotItems[i]->m_pItemIcon == nullptr)
		{
			CreateActionButtonIcon(m_vpActionSlotItems[i]);
		}
	}
}

void ActionBar::ClearActionButtons()
{
	for (size_t i = 0; i < m_vpActionSlotItems.size(); ++i)
	{
		if (m_vpActionSlotItems[i]->m_pItemIcon != nullptr)
		{
			m_pGUI->RemoveComponent(m_vpActionSlotItems[i]->m_pItemIcon);
			delete m_vpActionSlotItems[i]->m_pItemIcon;
			m_vpActionSlotItems[i]->m_pItemIcon = nullptr;
		}
	}
}

void ActionBar::ClearActionSlotItems()
{
	for (size_t i = 0; i < m_vpActionSlotItems.size(); ++i)
	{
		delete m_vpActionSlotItems[i];
		m_vpActionSlotItems[i] = nullptr;
	}
	m_vpActionSlotItems.clear();
}

void ActionBar::CreateActionSlots()
{
	for (int i = 0; i < MAX_NUM_ACTION_SLOTS; ++i)
	{
		int x;
		int y;
		int width;
		int height;
		GetActionSlotDimensions(i, &x, &y, &width, &height);

		// Create the action backgrounds
		std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
		std::string blankSlotIcon = "Resources/textures/gui/" + themeName + "/ActionBar/blank_slot.tga";
		Icon* pNewIcon = new Icon(m_pRenderer, blankSlotIcon.c_str(), 64, 64);
		pNewIcon->SetDimensions(x, y, width, height);
		pNewIcon->SetDepth(3.0f);

		m_vpActionSlots.push_back(pNewIcon);

		// Create the action labels
		char labelText[32];
		sprintf(labelText, "%i", (i < 9 ? i + 1 : 0));

		int xPos = x + 9;
		int yPos = y + 9;
		Label* pNewLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), labelText, Color(1.0f, 1.0f, 1.0f, 1.0f));
		pNewLabel->SetOutline(true);
		pNewLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
		pNewLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
		pNewLabel->SetLocation(xPos, yPos);
		pNewLabel->SetDepth(9.0f);

		m_vpActionLabels.push_back(pNewLabel);
	}
}

void ActionBar::DeleteActionSlots()
{
	// Clear action slots
	for (size_t i = 0; i < m_vpActionSlots.size(); ++i)
	{
		delete m_vpActionSlots[i];
		m_vpActionSlots[i] = nullptr;
	}
	m_vpActionSlots.clear();

	// Clear action labels
	for (size_t i = 0; i < m_vpActionLabels.size(); ++i)
	{
		delete m_vpActionLabels[i];
		m_vpActionLabels[i] = nullptr;
	}
	m_vpActionLabels.clear();

	ClearActionButtons();
}

void ActionBar::RepositionActionSlots()
{
	int x;
	int y;
	int width;
	int height;
	for (size_t i = 0; i < MAX_NUM_ACTION_SLOTS && i < m_vpActionSlots.size(); ++i)
	{
		GetActionSlotDimensions(i, &x, &y, &width, &height);

		m_vpActionSlots[i]->SetDimensions(x, y, width, height);
	}

	for (size_t i = 0; i < m_vpActionLabels.size(); ++i)
	{
		GetActionSlotDimensions(i, &x, &y, &width, &height);

		int xPos = x + 7;
		int yPos = y + 7;
		m_vpActionLabels[i]->SetLocation(xPos, yPos);
	}

	for (size_t i = 0; i < m_vpActionSlotItems.size(); ++i)
	{
		GetActionSlotDimensions(m_vpActionSlotItems[i]->m_slotNum, &x, &y, &width, &height);

		if (m_vpActionSlotItems[i]->m_pItemIcon != nullptr)
		{
			m_vpActionSlotItems[i]->m_pItemIcon->SetDimensions(x, y, width, height);
		}
	}
}

ActionButtonItem* ActionBar::GetActionButtonForSlot(int slotIndex)
{
	for (size_t i = 0; i < m_vpActionSlotItems.size(); ++i)
	{
		if (m_vpActionSlotItems[i]->m_slotNum == slotIndex)
		{
			return m_vpActionSlotItems[i];
		}
	}

	return nullptr;
}

void ActionBar::_ActionButtonPressed(void* pData)
{
	ActionButtonItem* pActionButton = static_cast<ActionButtonItem*>(pData);
	pActionButton->m_pActionBar->ActionButtonPressed(pActionButton);
}

void ActionBar::ActionButtonPressed(ActionButtonItem* pActionButton)
{
	m_pPressedActionButtonItem = pActionButton;

	Dimensions dimensions = pActionButton->m_pItemIcon->GetDimensions();
	m_pressedX = dimensions.x;
	m_pressedY = dimensions.y;

	m_pGUI->SetDraggingComponentPriority(pActionButton->m_pItemIcon);

	// Temporarily increase the depth of the dragged icon
	pActionButton->m_pItemIcon->SetDepth(100.0f);
}

void ActionBar::_ActionButtonReleased(void* pData)
{
	ActionButtonItem* pActionButton = static_cast<ActionButtonItem*>(pData);
	pActionButton->m_pActionBar->ActionButtonReleased(pActionButton);
}

void ActionBar::ActionButtonReleased(ActionButtonItem* pActionButton)
{
	if (m_pPressedActionButtonItem == nullptr)
	{
		return;
	}

	m_pPressedActionButtonItem = nullptr;

	m_pGUI->SetDraggingComponentPriority(nullptr);

	int x;
	int y;
	int width;
	int height;
	POINT mouse = { CubbyGame::GetInstance()->GetWindowCursorX(), (m_windowHeight - CubbyGame::GetInstance()->GetWindowCursorY()) };
	bool switched = false;
	bool sameSlot = false;
	int switchSlot = -1;

	for (int i = 0; i < MAX_NUM_ACTION_SLOTS && switched == false; ++i)
	{
		GetActionSlotDimensions(i, &x, &y, &width, &height);

		// Check if we released (mouse cursor) in the boundary of another slot
		if (mouse.x > x && mouse.x < x + width && mouse.y > y && mouse.y < y + height)
		{
			switched = true;
			switchSlot = i;

			if (pActionButton->m_slotNum == i)
			{
				sameSlot = true;
			}
		}
	}

	if (switched == true && sameSlot == false)
	{
		// Switch two action buttons
		SwitchItems(pActionButton->m_slotNum, switchSlot);
		ExportActionBar(m_pPlayer->GetName());
	}
	else if (switched == true && sameSlot == true)
	{
		// Return the action button to the original place
		pActionButton->m_pItemIcon->SetLocation(m_pressedX, m_pressedY);
	}
	else if (switched == false)
	{
		// Remove the action button
		RemoveItemFromActionBar(pActionButton);
		ExportActionBar(m_pPlayer->GetName());
	}

	if (switched == true)
	{
		// Revert depth back to normal for action button
		pActionButton->m_pItemIcon->SetDepth(8.0f);
	}
}

void ActionBar::_ActionButtonEntered(void* pData)
{
	ActionButtonItem* pActionButton = static_cast<ActionButtonItem*>(pData);
	pActionButton->m_pActionBar->ActionButtonEntered(pActionButton);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ActionBar::ActionButtonEntered(ActionButtonItem* pActionButton) const
{
	// Do nothing
}

void ActionBar::_ActionButtonExited(void* pData)
{
	ActionButtonItem* pActionButton = static_cast<ActionButtonItem*>(pData);
	pActionButton->m_pActionBar->ActionButtonExited(pActionButton);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ActionBar::ActionButtonExited(ActionButtonItem* pActionButton) const
{
	// Do nothing
}
