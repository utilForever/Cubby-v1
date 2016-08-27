/*************************************************************************
> File Name: PulldownMenu.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A pulldown menu GUI component.
> Created Time: 2016/08/27
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_PULLDOWN_MENU_H
#define CUBBY_PULLDOWN_MENU_H

#include "MenuItem.h"
#include "ScrollBar.h"
#include "Menu.h"

using MenuItemList = std::vector<MenuItem*>;

class PulldownMenu : public Container, public MouseListener, public FocusListener
{
public:
	// Constructor, Destructor
	PulldownMenu(Renderer* pRenderer, unsigned int GUIFont, const std::string& menuTitle);
	PulldownMenu(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& menuTitle, Color fontColor, Color outlineColor);
	PulldownMenu(Renderer* pRenderer, unsigned int GUIFont, const std::string& menuTitle,
		const std::string& defaultPulldownTexture, const std::string& selectedPulldownTexture, const std::string& hoverPulldownTexture, const std::string& disabledPulldowTexture,
		const std::string& defaultTitleTexture, const std::string& selectedTitleTexture, const std::string& hoverTitleTexture, const std::string& disabledTitleTexture,
		const std::string& defaultMenuTexture, const std::string& selectedMenuTexture, const std::string& hoverMenuTexture, const std::string& disabledMenuTexture);

	~PulldownMenu();

	void RemoveIcon(RenderRectangle* pRemoveIcon);
	void SetDefaultIcons(Renderer* pRenderer);
	void SetPulldownIconDefault(RenderRectangle* icon);
	void SetPulldownIconHover(RenderRectangle* icon);
	void SetPulldownIconPressed(RenderRectangle* icon);
	void SetPulldownIconDisabled(RenderRectangle* icon);

	unsigned int GetGUIFont() const;

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	void AddPulldownItem(const std::string& label);
	int GetNumItems() const;
	int GetSelectedItem() const;

	void RemoveAllPullDownMenuItems();

	void ResetPullDownMenu();

	void SetSelectedItem(const std::string& label);
	void SetSelectedMenuItem(MenuItem* pMenuItem);
	MenuItem* GetSelectedMenuItem() const;

	void SetMaxNumItemsDisplayed(int maxDisplay);
	int GetMaxNumItemsDisplayed() const;

	void UpdateScrollbar();

	void SetTitleString(const std::string& label);

	void SetRenderHeader(bool render);

	void SetSelected(bool val);
	void SetHover(bool val);
	void SetDisabled(bool val);

	bool IsSelected() const;
	bool IsHover() const;
	bool IsDisabled() const;

	void SetDisplayScrollBar(bool show);

	void OpenMenu();
	void CloseMenu();
	bool IsMenuOpen() const;

	bool ChildHasFocus();
	bool MenuChildHasFocus();

	ComponentType GetComponentType() const override;

	ScrollBar* GetPulldownScrollBar();

	void MenuItemChanged() const;

	void SetMenuItemChangedCallBackFunction(FunctionCallback func);
	void SetMenuItemChangedCallBackData(void* pData);

protected:
	void MouseEntered(const MouseEvent& event) override;
	void MouseExited(const MouseEvent& event) override;
	void MousePressed(const MouseEvent& event) override;
	void MouseReleased(const MouseEvent& event) override;
	void MouseReleasedOutside(const MouseEvent& event) override;
	void MouseClicked(const MouseEvent& event) override;

	void FocusLost(const FocusEvent& event) override;
	void FocusGained(const FocusEvent& event) override;

	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnMousePressed();
	virtual void OnMouseReleased();
	virtual void OnMouseClicked();

	void DrawChildren() override;
	void DrawSelf() override;

private:
	static void _MenuItemSelected(void* pData);

	RenderRectangle* m_pPulldownIcon;
	RenderRectangle* m_pPulldownSelectedIcon;
	RenderRectangle* m_pPulldownHoverIcon;
	RenderRectangle* m_pPulldownDisabledIcon;

	RenderRectangle* m_pMenuBackgroundDefault;
	RenderRectangle* m_pMenuBackgroundHover;
	RenderRectangle* m_pMenuBackgroundSelected;
	RenderRectangle* m_pMenuBackgroundDisabled;

	RenderRectangle* m_pTitleBackgroundDefault;
	RenderRectangle* m_pTitleBackgroundHover;
	RenderRectangle* m_pTitleBackgroundSelected;
	RenderRectangle* m_pTitleBackgroundDisabled;

	unsigned int m_GUIFont;
	unsigned int m_outlineGUIFont;

	bool m_outline;
	Color m_outlineColor;

	Label m_menuName;

	bool m_renderMenuHeader;

	Label m_menuTitle;
	Color m_textColor;

	Menu m_pulldownMenu;

	ScrollBar m_scrollBar;

	MenuItemList m_vpMenuItemList;

	bool m_isSelected;
	bool m_isHover;
	bool m_isDisabled;

	bool m_isDisplayScrollbar;

	int m_numItems;
	int m_selectedIndex;
	int m_maxDisplayItems;

	MenuItem* m_pSelectedMenuItem;

	// Function callback for when this slider has a value changed event
	FunctionCallback m_menuItemChangedCallback;
	void* m_pMenuItemChangedCallbackData;
};

#endif