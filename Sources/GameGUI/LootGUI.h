/*************************************************************************
> File Name: LootGUI.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Loot GUI class.
> Created Time: 2016/09/02
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_LOOT_GUI_H
#define CUBBY_LOOT_GUI_H

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
class Item;

struct LootSlotItem
{
	LootGUI* m_pLootGUI;
	InventoryItem* m_pInventoryItem;
	DraggableRenderRectangle* m_pInventoryIcon;

	int m_slotX;
	int m_slotY;

	bool m_lootingAnimation;

	bool m_erase;
};

class LootGUI
{
public:
	// Constructor, Destructor
	LootGUI(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight);
	~LootGUI();

	void SetInventoryGUI(InventoryGUI* pInventoryGUI);
	void SetCharacterGUI(CharacterGUI* pCharacterGUI);
	void SetActionBar(ActionBar* pActionBar);

	void SetWindowDimensions(int windowWidth, int windowHeight);

	// Skinning the GUI
	void SkinGUI() const;
	void UnSkinGUI() const;

	// Loading
	void Load(bool loadDelay = false, float loadDelayTime = 0.0f);
	void Unload();
	bool IsLoadDelayed() const;
	bool IsLoaded() const;

	void GetLootDimensions(int indexX, int indexY, int* x, int* y, int* width, int* height) const;

	GUIWindow* GetLootWindow() const;

	LootSlotItem* GetLootSlotItem(int x, int y);

	void AddLootItemFromInventory(InventoryItem* pInventoryItem, int x, int y);
	void LoadItems(Item* pItem);
	void AddLootItem(InventoryItem* pInventoryItem, int x, int y);
	void AddLootItem(const char* fileName, const char* iconFileName, InventoryType itemType, ItemType item, EquipSlot equipSlot, ItemQuality itemQuality, const char* title, const char* description, bool left, bool right, float r, float g, float b, int quantity, int x, int y);
	void RemoveLootItem(LootSlotItem* plootItemSlot);

	void CreateLootItems();
	void DeleteLootItems();

	// Tooltips
	void UpdateToolTipAppear(float dt);
	void ShowTooltip(LootSlotItem* pInventoryItem);
	void HideTooltip();

	void Update(float dt);

	static const int MAX_NUM_SLOTS_HORIZONTAL = 6;
	static const int MAX_NUM_SLOTS_VERTICAL = 3;

protected:
	static void _LootItemPressed(void* pData);
	void LootItemPressed(LootSlotItem* pInventoryItem);

	static void _LootItemReleased(void* pData);
	void LootItemReleased(LootSlotItem* pInventoryItem);

	static void _LootItemEntered(void* pData);
	void LootItemEntered(LootSlotItem* pInventoryItem);

	static void _LootItemExited(void* pData);
	void LootItemExited(LootSlotItem* pInventoryItem);

	static void _CloseExitPressed(void *pData);
	void CloseExitPressed();

private:
	Renderer* m_pRenderer;
	OpenGLGUI* m_pGUI;
	FrontendManager* m_pFrontendManager;
	ChunkManager* m_pChunkManager;
	Player* m_pPlayer;
	InventoryManager* m_pInventoryManager;
	InventoryGUI* m_pInventoryGUI;
	CharacterGUI* m_pCharacterGUI;
	ActionBar* m_pActionBar;

	int m_windowWidth;
	int m_windowHeight;

	bool m_loaded;

	int m_lootWindowWidth;
	int m_lootWindowHeight;
	int m_titlebarWidth;
	int m_titlebarHeight;

	Item* m_pStorageItem;

	// Load delay
	bool m_loadDelay;
	float m_loadDelayTime;

	// GUI components
	GUIWindow* m_pLootWindow;
	Icon* m_pTitleBarIcon;
	Icon* m_pTitleBarBackgroundIcon;
	Icon* m_pLootWindowBackgroundIcon;
	Button* m_pCloseExitButton;

	Icon* m_pLootButtonBackgroundSlotBorderCommon;
	Icon* m_pLootButtonBackgroundSlotBorderUncommon;
	Icon* m_pLootButtonBackgroundSlotBorderMagical;
	Icon* m_pLootButtonBackgroundSlotBorderRare;
	Icon* m_pLootButtonBackgroundSlotBorderEpic;

	// Pressed icons
	std::vector<LootSlotItem*> m_vpLootSlotItems;

	int m_pressedX;
	int m_pressedY;
	LootSlotItem* m_pPressedLootSlotItem;

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