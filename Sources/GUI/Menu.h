/*************************************************************************
> File Name: Menu.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A menu is a container of menu items.
> Created Time: 2016/08/16
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MENU_H
#define CUBBY_MENU_H

#include "AbstractButton.h"
#include "MenuItem.h"

// Forward declaration
class PulldownMenu;

class Menu : public AbstractButton, public MouseListener, public FocusListener
{
public:
	// Constructor, Destructor
	Menu(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, bool showTopLevelMenu);
	Menu(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, bool showTopLevelMenu, const std::string& defaultTexture, const std::string& selectedTexture, const std::string& hoverTexture, const std::string& disabledTexture);

	~Menu();

	void ResetMenu();

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	void SetPullDownMenuParent(PulldownMenu* pPullDownMenuParent);
	PulldownMenu* GetPullDownMenuParent() const;

	void Add(MenuItem* pMenuItem, bool scrollBarPresent = true);

	MenuItem* FindItem(const std::string& label);

	void SetAutoSizeMenuWidth(bool autoSize);

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;
	void SetWidth(int width) override;
	void SetHeight(int height) override;
	void ResetToTextWidth();

	void SetMenuItemSpacer(int spacer);
	int GetMenuItemSpacer() const;

	int GetBiggestWidth() const;

	int GetCurrentMenuheight() const;

	std::string GetMenuTitle() const;

	void OpenMenu();
	void CloseMenu();
	bool IsMenuOpen() const;

	bool ChildHasFocus();

	ComponentType GetComponentType() const override;

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

	void DrawSelf() override;

private:
	bool m_showTopLevelMenu;
	bool m_menuOpen;

	bool m_autosizeMenuBase;

	int m_biggestWidth;
	int m_currentMenuheight;
	int m_menuItemSpacer;

	PulldownMenu* m_pPullDownMenuParent;
};

#endif