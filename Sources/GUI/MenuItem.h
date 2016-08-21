/*************************************************************************
> File Name: MenuItem.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A menu item component that represents a clickable item in a menu.
> Created Time: 2016/08/16
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MENU_ITEM_H
#define CUBBY_MENU_ITEM_H

#include "AbstractButton.h"

class MenuItem : public AbstractButton, public MouseListener, public FocusListener
{
public:
	// Constructor, Destructor
	MenuItem(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, Color labelColor);
	MenuItem(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color labelColor, Color outlineColor);
	MenuItem(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, Color labelColor, const std::string& defaultTexture, const std::string& selectedTexture, const std::string& hoverTexture, const std::string& disabledTexture);

	~MenuItem();

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;
	void SetWidth(int width) override;
	void SetHeight(int height) override;

	void SetOnlyShowSelectedIcons(bool isShow);
	bool IsOnlyShowSelectedIcons() const;

	ComponentType GetComponentType() const override;

	bool IsParentMenuOpen() const;

	void MenuItemSelectedFromPulldowmMenu();

protected:
	void MouseEntered(const MouseEvent& event) override;
	void MouseExited(const MouseEvent& event) override;
	void MousePressed(const MouseEvent& event) override;
	void MouseReleased(const MouseEvent& event) override;
	void MouseReleasedOutside(const MouseEvent& event) override;
	void MouseClicked(const MouseEvent& event) override;

	void MenuItemPressed() const;

	void FocusLost(const FocusEvent& event) override;
	void FocusGained(const FocusEvent& event) override;

	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnMousePressed();
	virtual void OnMouseReleased();
	virtual void OnMouseClicked();

	void DrawSelf() override;

private:
	bool m_onlyShowSelectedItems;
};

#endif