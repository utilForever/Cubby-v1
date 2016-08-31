/*************************************************************************
> File Name: CraftingGUI.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Crafting GUI class.
> Created Time: 2016/08/29
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_CRAFTING_GUI_H
#define CUBBY_CRAFTING_GUI_H

#include <vector>

#include <Blocks/ChunkManager.h>
#include <GUI/Button.h>
#include <GUI/FormattedLabel.h>
#include <GUI/Icon.h>
#include <GUI/OpenGLGUI.h>
#include <GUI/ProgressBar.h>
#include <GUI/ScrollBar.h>
#include <GUI/TextBox.h>
#include <Inventory/InventoryManager.h>
#include <Player/Player.h>

// Forward declaration
class FrontendManager;
class CraftingGUI;

struct CraftingRecipe
{
	~CraftingRecipe()
	{
		// Don't DELETE the items, just clear the array, they will be deleted by the inventory manager
		//for(size_t i = 0; i < m_vpCraftingItems.size(); ++i)
		//{
		//delete m_vpCraftingItems[i];
		//m_vpCraftingItems[i] = nullptr;
		//}
		//
		m_vpCraftingItems.clear();
	}

	std::vector<InventoryItem*> m_vpCraftingItems;
	InventoryItem* m_pResultItem;
};

struct RecipeSlotItem
{
	CraftingGUI* m_pCraftingGUI;
	InventoryItem* m_pInventoryItem;
	Button* m_pResultsIcon;
	std::string m_recipeName;
	CraftingRecipe* m_pCraftingReceipe;
	int m_slotIndex;

	bool m_erase;
};

struct IngredientsSlotItem
{
	CraftingGUI* m_pCraftingGUI;
	InventoryItem* m_pInventoryItem;
	Button* m_pIngredientIcon;
	int m_slotXIndex;
	int m_slotYIndex;

	bool m_erase;
};


class CraftingGUI
{
public:
	// Constructor, Destructor
	CraftingGUI(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight);
	~CraftingGUI();

	void SetWindowDimensions(int windowWidth, int windowHeight);

	// Skinning the GUI
	void SkinGUI() const;
	void UnSkinGUI() const;

	// Loading
	void Load(bool loadDelay = false, float loadDelayTime = 0.0f);
	void Unload();
	bool IsLoadDelayed() const;
	bool IsLoaded() const;

	void CraftingComplete();

	void SetInteractionitem(Item* pInteractionItem);

	// Recipes
	void SetCraftingRecipesForItem(ItemType craftingItem);
	void AddCraftingRecipe(CraftingRecipe* pRecipe);
	void DeleteCraftingRecipes();
	void CreateRecipeButtons();
	void DeleteRecipeButtons();
	void CreateIngredientsButtons();
	void DeleteIngredientsButtons();
	bool CanCraftRecipe() const;

	// Tooltips
	void UpdateToolTipAppear(float dt);
	void ShowTooltip(RecipeSlotItem* pRecipeItem);
	void ShowTooltip(IngredientsSlotItem* pIngredientItem);
	void HideTooltip();

	// Updating
	void Update(float dt);
	void UpdateCraftButton() const;
	void UpdateFilteredRecipes();

	// Rendering
	void Render() const;

protected:
	static void _CloseExitPressed(void* pData);
	void CloseExitPressed();

	static void _RecipeItemEntered(void* pData);
	void RecipeItemEntered(RecipeSlotItem* pRecipeButtonData);

	static void _RecipeItemExited(void* pData);
	void RecipeItemExited(RecipeSlotItem* pRecipeButtonData);

	static void _IngredientItemEntered(void* pData);
	void IngredientItemEntered(IngredientsSlotItem* pIngredientButtonData);

	static void _IngredientItemExited(void* pData);
	void IngredientItemExited(IngredientsSlotItem* pIngredientButtonData);

	static void _CraftPressed(void* pData);
	void CraftPressed(bool fakeCraft = false);

	static void _ResultsItemPressed(void* pData);
	void ResultsItemPressed(RecipeSlotItem* pRecipeButtonData);

private:
	Renderer* m_pRenderer;
	OpenGLGUI* m_pGUI;
	FrontendManager* m_pFrontendManager;
	ChunkManager* m_pChunkManager;
	Player* m_pPlayer;
	InventoryManager* m_pInventoryManager;

	int m_windowWidth;
	int m_windowHeight;

	bool m_loaded;

	int m_craftingWindowWidth;
	int m_craftingWindowHeight;
	int m_titlebarWidth;
	int m_titlebarHeight;
	int m_craftingResultsScrollAreaX;
	int m_craftingResultsScrollAreaY;
	int m_craftingResultsScrollAreaWidth;
	int m_craftingResultsScrollAreaHeight;
	int m_craftingProgressX;
	int m_craftingProgressY;
	int m_craftingProgressWidth;
	int m_craftingProgressHeight;

	// Load delay
	bool m_loadDelay;
	float m_loadDelayTime;

	// GUI components
	GUIWindow* m_pCraftingWindow;
	Icon* m_pTitleBarIcon;
	Icon* m_pTitleBarBackgroundIcon;
	Icon* m_pCraftingWindowBackgroundIcon;
	Button* m_pCloseExitButton;
	ScrollBar* m_pResultsScrollbar;
	Icon* m_pCraftButtonIcon;
	Icon* m_pCraftButtonIconHover;
	Icon* m_pCraftButtonIconPressed;
	Icon* m_pCraftButtonIconDisabled;
	Button* m_pCraftButton;
	ProgressBar* m_pCraftingProgressBar;
	Icon* m_pCraftingProgressBarFiller;
	Icon* m_pCraftingProgressBarBackgroundFiller;
	TextBox* m_pSearchBox;
	Icon* m_pSearchTextboxIcon;
	Icon* m_pRecipeButtonIcon;
	Icon* m_pRecipeButtonHoverIcon;
	Icon* m_pRecipeButtonPressedIcon;
	Icon* m_pRecipeButtonSelectedIcon;

	Icon* m_pInventoryBackgroundSlotBorderCommon;
	Icon* m_pInventoryBackgroundSlotBorderUncommon;
	Icon* m_pInventoryBackgroundSlotBorderMagical;
	Icon* m_pInventoryBackgroundSlotBorderRare;
	Icon* m_pInventoryBackgroundSlotBorderEpic;

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

	// Crafting params
	bool m_crafting;
	float m_craftingTime;
	float m_craftingTimer;

	std::string m_cachedSearchText;

	std::vector<RecipeSlotItem*> m_vpRecipeSlotItem;
	std::vector<RecipeSlotItem*> m_vpRecipeSlotItemFiltered;
	std::vector<CraftingRecipe*> m_vpCraftingRecipes;
	std::vector<IngredientsSlotItem*> m_vpIngredientsSlotItem;

	// Selected recipe
	RecipeSlotItem* m_pRecipeSlotItemSelected;

	// Interaction item
	Item* m_pInteractionItem;
};

#endif