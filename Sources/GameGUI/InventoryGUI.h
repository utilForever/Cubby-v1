/*************************************************************************
> File Name: InventoryGUI.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Inventory GUI class.
> Created Time: 2016/09/01
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_INVENTORY_GUI_H
#define CUBBY_INVENTORY_GUI_H

#include <Blocks/ChunkManager.h>
#include <GUI/DraggableRenderRectangle.h>
#include <GUI/FormattedLabel.h>
#include <GUI/Icon.h>
#include <GUI/OpenGLGUI.h>
#include <Inventory/InventoryManager.h>

// Forward declaration
class FrontendManager;
class InventoryGUI;
class CharacterGUI;
class LootGUI;
class ActionBar;
struct LootSlotItem;
class ItemManager;

class InventorySlotItem
{
public:
	InventoryGUI* m_pInventoryGUI;
	InventoryItem* m_pInventoryItem;
	DraggableRenderRectangle* m_pInventoryIcon;

	int m_slotX;
	int m_slotY;

	bool m_dropshadowAdded;
	bool m_erase;
};

class InventoryGUI
{
public:
	// Constructor, Destructor
	InventoryGUI(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight);
	~InventoryGUI();

	void SetCharacterGUI(CharacterGUI* pCharacterGUI);
	void SetLootGUI(LootGUI* pLootGUI);
	void SetActionBar(ActionBar* pActionBar);
	void SetItemManager(ItemManager *pItemManager);

	// Skinning the GUI
	void SkinGUI();
	void UnSkinGUI() const;

	// Loading
	void Load(bool loadDelay = false, float loadDelayTime = 0.0f);
	void Unload();
	bool IsLoadDelayed() const;
	bool IsLoaded() const;

	void SetWindowDimensions(int windowWidth, int windowHeight);

	void GetInventoryDimensions(int indexX, int indexY, int* x, int* y, int* width, int* height) const;
	void GetDestroySlotDimensions(int* x, int* y, int* width, int* height) const;
	void GetDropSlotDimensions(int* x, int* y, int* width, int* height) const;

	GUIWindow* GetInventoryWindow() const;

	void CreateInventoryItems();
	void DeleteInventoryItems();

	void UpdateActionBar();

	// Tooltips
	void UpdateToolTipAppear(float dt);
	void ShowTooltip(InventorySlotItem* pInventoryItem);
	void HideTooltip();

	// Popup
	void OpenPopup(std::string popupTitle, std::string popupText) const;
	void ClosePopup() const;

	InventorySlotItem* GetInventorySlotItem(int x, int y);
	InventorySlotItem* GetInventorySlotItemEquipped(EquipSlot equipSlot);

	void SetEquippedItem(EquipSlot equipSlot, std::string title);

	void EquipItem(InventoryItem* pInventoryItem, int inventoryX, int inventoryY);
	void EquipItem(LootSlotItem* pLootItem) const;
	void EquipItem(InventorySlotItem* pInventoryItem);
	void UnequipItem(EquipSlot equipSlot, bool left, bool right);

	void Update(float dt);

protected:
	/* Protected methods */
	static void _InventoryItemPressed(void* pData);
	void InventoryItemPressed(InventorySlotItem* pInventoryItem);

	static void _InventoryItemReleased(void* pData);
	void InventoryItemReleased(InventorySlotItem* pInventoryItem);

	static void _InventoryItemEntered(void* pData);
	void InventoryItemEntered(InventorySlotItem* pInventoryItem);

	static void _InventoryItemExited(void* pData);
	void InventoryItemExited(InventorySlotItem* pInventoryItem);

	static void _CloseExitPressed(void *pData);
	void CloseExitPressed();

	static void _PopupConfirmPressed(void* pData);
	void PopupConfirmPressed();

	static void _PopupCancelPressed(void* pData);
	void PopupCancelPressed();

public:
	static const int MAX_NUM_SLOTS_HORIZONTAL = 6;
	static const int MAX_NUM_SLOTS_VERTICAL = 3;

private:
	Renderer* m_pRenderer;
	OpenGLGUI* m_pGUI;
	FrontendManager* m_pFrontendManager;
	ChunkManager* m_pChunkManager;
	Player* m_pPlayer;
	InventoryManager* m_pInventoryManager;
	CharacterGUI* m_pCharacterGUI;
	LootGUI* m_pLootGUI;
	ActionBar* m_pActionBar;
	ItemManager* m_pItemManager;

	bool m_loaded;

	int m_windowWidth;
	int m_windowHeight;

	int m_inventoryWindowWidth;
	int m_inventoryWindowHeight;
	int m_titlebarWidth;
	int m_titlebarHeight;

	// Load delay
	bool m_loadDelay;
	float m_loadDelayTime;

	// GUI components
	GUIWindow* m_pInventoryWindow;
	Icon* m_pTitleBarIcon;
	Icon* m_pTitleBarBackgroundIcon;
	Icon* m_pInventoryWindowBackgroundIcon;
	Button* m_pCloseExitButton;
	Icon* m_pDestroyIcon;
	Label* m_pDestroyLabel;
	Icon* m_pDropIcon;
	Label* m_pDropLabel;

	Icon* m_pInventoryBackgroundSlotBorderCommon;
	Icon* m_pInventoryBackgroundSlotBorderUncommon;
	Icon* m_pInventoryBackgroundSlotBorderMagical;
	Icon* m_pInventoryBackgroundSlotBorderRare;
	Icon* m_pInventoryBackgroundSlotBorderEpic;

	// Pressed icons
	std::vector<InventorySlotItem*> m_vpInventorySlotItems;
	std::string m_pEquippedItems[static_cast<int>(EquipSlot::NumSlots)];

	int m_pressedX;
	int m_pressedY;
	InventorySlotItem* m_pPressedInventoryItem;
	InventorySlotItem* m_pInventoryItemToDelete;

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

	// Popup
	int m_popupWidth;
	int m_popupHeight;
	int m_popupBorderSpacer;
	int m_popupTitleSpacer;
	int m_popupIconSize;
	int m_popupIconSpacer;
	Label* m_popupTitle;
	FormattedLabel* m_popupText;
	Button* m_pPopupConfirmButton;
	Button* m_pPopupCancelButton;
	Icon* m_pPopupBackgroundIcon;
};

#endif