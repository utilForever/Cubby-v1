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

	void AddEventListeners();
	void RemoveEventListeners();

	void SetPullDownMenuParent(PulldownMenu* pPullDownMenuParent);
	PulldownMenu* GetPullDownMenuParent();

	void Add(MenuItem* pMenuItem, bool scrollBarPresent = true);

	MenuItem* FindItem(const std::string& label);

	void SetAutoSizeMenuWidth(bool autoSize);

	void SetDimensions(int x, int y, int width, int height);
	void SetDimensions(const Dimensions& r);
	void SetWidth(int width);
	void SetHeight(int height);
	void ResetToTextWidth();

	void SetMenuItemSpacer(int spacer);
	int GetMenuItemSpacer() const;

	int GetBiggestWidth() const;

	int GetCurrentMenuheight() const;

	std::string GetMenuTitle() const;

	void OpenMenu();
	void CloseMenu();
	bool IsMenuOpen();

	bool ChildHasFocus();

	ComponentType GetComponentType() const;

protected:
	void MouseEntered(const MouseEvent& event);
	void MouseExited(const MouseEvent& event);
	void MousePressed(const MouseEvent& event);
	void MouseReleased(const MouseEvent& event);
	void MouseReleasedOutside(const MouseEvent& event);
	void MouseClicked(const MouseEvent& event);

	void FocusLost(const FocusEvent& event);
	void FocusGained(const FocusEvent& event);

	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnMousePressed();
	virtual void OnMouseReleased();
	virtual void OnMouseClicked();

	void DrawSelf();

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