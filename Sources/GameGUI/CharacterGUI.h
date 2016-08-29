/*************************************************************************
> File Name: CharacterGUI.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Character GUI class.
> Created Time: 2016/08/29
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_CHARACTER_GUI_H
#define CUBBY_CHARACTER_GUI_H

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

struct CharacterSlotItem
{
	CharacterGUI* m_pCharacterGUI;
	InventoryItem* m_pInventoryItem;
	DraggableRenderRectangle* m_pInventoryIcon;

	int m_slotX;
	int m_slotY;

	bool m_erase;
};

class CharacterGUI
{
public:
	// Constructor, Destructor
	CharacterGUI(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight);
	~CharacterGUI();

	void SetInventoryGUI(InventoryGUI* pInventoryGUI);
	void SetLootGUI(LootGUI* pLootGUI);
	void SetActionBar(ActionBar* pActionBar);

	void SetWindowDimensions(int windowWidth, int windowHeight);

	// Skinning the GUI
	void SkinGUI();
	void UnSkinGUI() const;

	// Loading
	void Load(bool loadDelay = false, float loadDelayTime = 0.0f);
	void Unload();
	bool IsLoadDelayed() const;
	bool IsLoaded() const;

	GUIWindow* GetCharacterWindow() const;

	void CreateInventoryItems();
	void DeleteInventoryItems();

	void ShowEquipHover(EquipSlot equipSlot);
	void UpdateEquipHover() const;
	void HideEquipHover() const;

	void StartEquipHoverAnimation();
	void StopEquipHoverAnimation();

	// Tooltips
	void UpdateToolTipAppear(float dt);
	void ShowTooltip(CharacterSlotItem* pCharacterItem);
	void HideTooltip();

	void GetPlayerPortraitDimensions(int* x, int* y, int* width, int* height) const;
	void GetPlayerWeaponLeftDimensions(int* x, int* y, int* width, int* height) const;
	void GetPlayerWeaponRightDimensions(int* x, int* y, int* width, int* height) const;
	void GetPlayerHeadDimensions(int* x, int* y, int* width, int* height) const;
	void GetPlayerShoulderDimensions(int* x, int* y, int* width, int* height) const;
	void GetPlayerBodyDimensions(int* x, int* y, int* width, int* height) const;
	void GetPlayerLegsDimensions(int* x, int* y, int* width, int* height) const;
	void GetPlayerHandDimensions(int* x, int* y, int* width, int* height) const;
	void GetPlayerFeetDimensions(int* x, int* y, int* width, int* height) const;
	void GetPlayerAccessory1Dimensions(int* x, int* y, int* width, int* height) const;
	void GetPlayerAccessory2Dimensions(int* x, int* y, int* width, int* height) const;

	void UpdatePlayerStats();

	void Update(float dt);

protected:
	static void _CharacterItemEntered(void* pData);
	void CharacterItemEntered(CharacterSlotItem* pCharacterItem);

	static void _CharacterItemExited(void* pData);
	void CharacterItemExited(CharacterSlotItem* pCharacterItem);

	static void _CharacterItemPressed(void* pData);
	void CharacterItemPressed(CharacterSlotItem* pCharacterItem);

	static void _CharacterItemReleased(void* pData);
	void CharacterItemReleased(CharacterSlotItem* pCharacterItem);

	static void _CloseExitPressed(void* pData);
	void CloseExitPressed();

	static void _OpenCloseTabPressed(void* pData);
	void OpenCloseTabPressed();

	static void _ArrowLeftPressed(void* pData);
	void ArrowLeftPressed();

	static void _ArrowLeftReleased(void* pData);
	void ArrowLeftReleased();

	static void _ArrowRightPressed(void* pData);
	void ArrowRightPressed();

	static void _ArrowRightReleased(void* pData);
	void ArrowRightReleased();

	static void _EquipHoverAnimationFinished(void* pData);
	void EquipHoverAnimationFinished();

	static void _Stat1IncreasePressed(void* pData);
	void Stat1IncreasePressed();

	static void _Stat2IncreasePressed(void* pData);
	void Stat2IncreasePressed();

	static void _Stat3IncreasePressed(void* pData);
	void Stat3IncreasePressed();

	static void _Stat4IncreasePressed(void* pData);
	void Stat4IncreasePressed();

	static void _Stat5IncreasePressed(void* pData);
	void Stat5IncreasePressed();

	static void _Stat6IncreasePressed(void* pData);
	void Stat6IncreasePressed();

private:
	Renderer* m_pRenderer;
	OpenGLGUI* m_pGUI;
	FrontendManager* m_pFrontendManager;
	ChunkManager* m_pChunkManager;
	Player* m_pPlayer;
	InventoryManager* m_pInventoryManager;
	InventoryGUI* m_pInventoryGUI;
	LootGUI* m_pLootGUI;
	ActionBar* m_pActionBar;

	int m_windowWidth;
	int m_windowHeight;

	bool m_loaded;

	// Load delay
	bool m_loadDelay;
	float m_loadDelayTime;

	int m_characterWindowWidth;
	int m_characterWindowHeight;
	int m_titlebarWidth;
	int m_titlebarHeight;
	int m_statsTabOpenButtonWidth;
	int m_statsTabOpenButtonHeight;
	int m_statsTabWidth;
	int m_statsTabHeight;
	int m_paperDollX;
	int m_paperDollY;
	int m_paperDollWidth;
	int m_paperDollHeight;
	int m_statsTitleLabelY;
	int m_statsHeaderLabelXOffset;
	int m_statsValueLabelXOffset;

	bool m_leftArrowPressed;
	bool m_rightArrowPressed;

	bool m_statsTabsOpen;
	bool m_statsTabLeftSide;

	// GUI components
	GUIWindow* m_pCharacterWindow;
	Icon* m_pDynamicPaperDollIcon;
	Icon* m_pTitleBarIcon;
	Icon* m_pTitleBarBackgroundIcon;
	Icon* m_pCharacterWindowBackgroundIcon;
	Button* m_pCloseExitButton;
	Button* m_pArrowLeftButton;
	Button* m_pArrowRightButton;
	Icon* m_pTabOpenCloseButtonIconRight;
	Icon* m_pTabOpenCloseButtonIconRightHover;
	Icon* m_pTabOpenCloseButtonIconRightPressed;
	Icon* m_pTabOpenCloseButtonIconLeft;
	Icon* m_pTabOpenCloseButtonIconLeftHover;
	Icon* m_pTabOpenCloseButtonIconLeftPressed;
	Button* m_pTabOpenCloseButton;
	Icon* m_pStatsTabIconLeft;
	Icon* m_pStatsTabIconRight;
	Label* m_pCharacterTitleLabel;
	Label* m_pStatsHeaderLabel1;
	Label* m_pStatsHeaderLabel2;
	Label* m_pStatsHeaderLabel3;
	Label* m_pStatsHeaderLabel4;
	Label* m_pStatsHeaderLabel5;
	Label* m_pStatsHeaderLabel6;
	FormattedLabel* m_pStatsValueLabel1;
	FormattedLabel* m_pStatsValueLabel2;
	FormattedLabel* m_pStatsValueLabel3;
	FormattedLabel* m_pStatsValueLabel4;
	FormattedLabel* m_pStatsValueLabel5;
	FormattedLabel* m_pStatsValueLabel6;
	Button* m_pStat1IncreaseButton;
	Button* m_pStat2IncreaseButton;
	Button* m_pStat3IncreaseButton;
	Button* m_pStat4IncreaseButton;
	Button* m_pStat5IncreaseButton;
	Button* m_pStat6IncreaseButton;

	Icon* m_pInventoryBackgroundSlotBorderCommon;
	Icon* m_pInventoryBackgroundSlotBorderUncommon;
	Icon* m_pInventoryBackgroundSlotBorderMagical;
	Icon* m_pInventoryBackgroundSlotBorderRare;
	Icon* m_pInventoryBackgroundSlotBorderEpic;

	Icon* pBlockSlotItem;

	Icon* m_pEquipHoverIcon;
	int m_equipHoverXOrigin;
	float m_equipHoverXOffset;

	// Pressed icons
	std::vector<CharacterSlotItem*> m_vpInventorySlotItems;
	CharacterSlotItem* m_pEquippedItems[static_cast<int>(EquipSlot::NumSlots)];

	int m_pressedX;
	int m_pressedY;
	CharacterSlotItem* m_pPressedCharacterItem;

	bool m_addedStatIncreaseButtons;

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