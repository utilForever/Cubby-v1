/*************************************************************************
> File Name: ActionBar.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Action bar class.
> Created Time: 2016/08/28
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ACTION_BAR_H
#define CUBBY_ACTION_BAR_H

#include <Blocks/ChunkManager.h>
#include <GUI/DraggableRenderRectangle.h>
#include <GUI/Icon.h>
#include <GUI/OpenGLGUI.h>
#include <Inventory/InventoryManager.h>

// Forward declaration
class FrontendManager;
class ActionBar;
class InventoryGUI;
class LootGUI;
class CharacterGUI;

enum class ActionBarItemType
{
	Item = 0,
	Skill,
};

class ActionButtonItem
{
public:
	ActionBar* m_pActionBar;
	DraggableRenderRectangle* m_pItemIcon;

	ActionBarItemType m_type;

	int m_slotNum;

	std::string m_itemTitle;

	int m_inventoryX;
	int m_inventoryY;
	EquipSlot m_equipSlot;

	bool m_erase;
};

class ActionBar
{
public:
	// Constructor, Destructor
	ActionBar(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight);
	~ActionBar();

	void SetInventoryGUI(InventoryGUI* pInventoryGUI);
	void SetCharacterGUI(CharacterGUI* pCharacterGUI);
	void SetLootGUI(LootGUI* pLootGUI);

	// Skinning the GUI
	void SkinGUI() const;
	void UnSkinGUI() const;

	// Loading
	void Load();
	void Unload();
	bool IsLoaded() const;

	void SetWindowDimensions(int windowWidth, int windowHeight);

	// Supress export
	void SetSupressExport(bool supress);

	// Import / Export action bar
	void ExportActionBar(std::string playerName);
	void ImportActionBar(std::string playerName);

	void GetActionSlotDimensions(int indexX, int* x, int* y, int* width, int* height) const;

	void AddItemToActionBar(InventoryItem* pInventoryItem, int slotIndex, int inventoryX, int inventoryY);
	void RemoveItemFromActionBar(ActionButtonItem* pButton);
	void RemoveInventoryItemFromActionBar(std::string itemTitle);
	void SwitchItems(int slot1, int slot2);
	void UpdateActionBarSlots(InventoryItem* pInventoryItem, int slotX, int slotY);
	void UpdateActionBarSlots(std::string itemTitle, int slotX, int slotY);
	ActionButtonItem* GetActionButtonForSlot(int slotIndex);

	void UseActionBarslot(int slotIndex);

	void CreateActionButtonIcon(ActionButtonItem* pActionButton) const;
	void CreateActionButtons();
	void ClearActionButtons();
	void ClearActionSlotItems();

	void CreateActionSlots();
	void DeleteActionSlots();

	void RepositionActionSlots();

protected:
	static void _ActionButtonPressed(void* pData);
	void ActionButtonPressed(ActionButtonItem* pActionButton);

	static void _ActionButtonReleased(void* pData);
	void ActionButtonReleased(ActionButtonItem* pActionButton);

	static void _ActionButtonEntered(void* pData);
	void ActionButtonEntered(ActionButtonItem* pActionButton) const;

	static void _ActionButtonExited(void* pData);
	void ActionButtonExited(ActionButtonItem* pActionButton) const;

public:
	static const int MAX_NUM_ACTION_SLOTS = 6;

private:
	Renderer* m_pRenderer;
	OpenGLGUI* m_pGUI;
	FrontendManager* m_pFrontendManager;
	ChunkManager* m_pChunkManager;
	Player* m_pPlayer;
	InventoryManager* m_pInventoryManager;
	CharacterGUI* m_pCharacterGUI;
	LootGUI* m_pLootGUI;
	InventoryGUI* m_pInventoryGUI;

	bool m_isLoaded;

	int m_windowWidth;
	int m_windowHeight;

	// Force to stop export action bar file
	bool m_supressExport;

	int m_actionSlotSize;
	int m_actionSlotSpacer;
	int m_actionBarX;
	int m_actionBarY;

	Icon* m_pActionButtonBackground6Icon;

	Icon* m_pActionButtonBackgroundSlotBorderCommon;
	Icon* m_pActionButtonBackgroundSlotBorderUncommon;
	Icon* m_pActionButtonBackgroundSlotBorderMagical;
	Icon* m_pActionButtonBackgroundSlotBorderRare;
	Icon* m_pActionButtonBackgroundSlotBorderEpic;

	int m_pressedX;
	int m_pressedY;
	ActionButtonItem* m_pPressedActionButtonItem;

	std::vector<RenderRectangle*> m_vpActionSlots;
	std::vector<Label*> m_vpActionLabels;
	std::vector<ActionButtonItem*> m_vpActionSlotItems;
};

#endif