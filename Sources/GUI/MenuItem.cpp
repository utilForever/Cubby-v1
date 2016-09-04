/*************************************************************************
> File Name: MenuItem.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A menu item component that represents a clickable item in a menu.
> Created Time: 2016/08/20
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DirectDrawRectangle.h"
#include "FocusManager.h"
#include "GUIWindow.h"
#include "Icon.h"
#include "Menu.h"
#include "MenuItem.h"
#include "PulldownMenu.h"

// Constructor, Destructor
MenuItem::MenuItem(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, Color labelColor) :
	AbstractButton(pRenderer, GUIFont, label)
{
	m_pDefaultIcon = new DirectDrawRectangle(pRenderer);
	m_pHoverIcon = new DirectDrawRectangle(pRenderer);
	m_pSelectedIcon = new DirectDrawRectangle(pRenderer);
	m_pDisabledIcon = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pDefaultIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pHoverIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pSelectedIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pDisabledIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	Container::Add(m_pDefaultIcon);
	Container::Add(m_pSelectedIcon);
	Container::Add(m_pHoverIcon);
	Container::Add(m_pDisabledIcon);

	m_label.SetLocation(0, 0);
	m_label.SetColor(labelColor);
	Container::Add(&m_label);

	m_pDefaultIcon->SetDepth(3.0f);
	m_pSelectedIcon->SetDepth(3.0f);
	m_pHoverIcon->SetDepth(3.0f);
	m_pDisabledIcon->SetDepth(3.0f);

	// Start out disabled, since most menus are not open to start with...
	Container::SetEnabled(false);

	m_onlyShowSelectedItems = false;
}

MenuItem::MenuItem(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color labelColor, Color outlineColor) :
	AbstractButton(pRenderer, GUIFont, label)
{
	m_pDefaultIcon = new DirectDrawRectangle(pRenderer);
	m_pHoverIcon = new DirectDrawRectangle(pRenderer);
	m_pSelectedIcon = new DirectDrawRectangle(pRenderer);
	m_pDisabledIcon = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pDefaultIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pHoverIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pSelectedIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pDisabledIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	Container::Add(m_pDefaultIcon);
	Container::Add(m_pSelectedIcon);
	Container::Add(m_pHoverIcon);
	Container::Add(m_pDisabledIcon);

	m_label.SetLocation(0, 0);
	m_label.SetColor(labelColor);
	m_label.SetOutline(true);
	m_label.SetOutlineColor(outlineColor);
	m_label.SetOutlineFont(outlineGUIFont);
	Container::Add(&m_label);

	m_pDefaultIcon->SetDepth(3.0f);
	m_pSelectedIcon->SetDepth(3.0f);
	m_pHoverIcon->SetDepth(3.0f);
	m_pDisabledIcon->SetDepth(3.0f);

	// Start out disabled, since most menus are not open to start with...
	Container::SetEnabled(false);

	m_onlyShowSelectedItems = false;
}

MenuItem::MenuItem(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, Color labelColor, const std::string& defaultTexture, const std::string& selectedTexture, const std::string& hoverTexture, const std::string& disabledTexture) :
	AbstractButton(pRenderer, GUIFont, label)
{
	m_pDefaultIcon = new Icon(pRenderer, defaultTexture);
	m_pSelectedIcon = new Icon(pRenderer, selectedTexture);
	m_pHoverIcon = new Icon(pRenderer, hoverTexture);
	m_pDisabledIcon = new Icon(pRenderer, disabledTexture);

	Container::Add(m_pDefaultIcon);
	Container::Add(m_pSelectedIcon);
	Container::Add(m_pHoverIcon);
	Container::Add(m_pDisabledIcon);

	m_label.SetLocation(0, 0);
	m_label.SetColor(labelColor);
	Container::Add(&m_label);

	m_pDefaultIcon->SetDepth(3.0f);
	m_pSelectedIcon->SetDepth(3.0f);
	m_pHoverIcon->SetDepth(3.0f);
	m_pDisabledIcon->SetDepth(3.0f);

	// Start out disabled, since most menus are not open to start with...
	Container::SetEnabled(false);

	m_onlyShowSelectedItems = false;
}

MenuItem::~MenuItem()
{
	delete m_pDefaultIcon;
	delete m_pSelectedIcon;
	delete m_pHoverIcon;
	delete m_pDisabledIcon;
}

void MenuItem::AddEventListeners()
{
	AddMouseListener(this);
	AddFocusListener(this);
}

void MenuItem::RemoveEventListeners()
{
	RemoveMouseListener(this);
	RemoveFocusListener(this);

	// Remove hover and select properties, since we have made this component un-selectable by the mouse
	SetHover(false);
	SetSelected(false);

	SetFocus(false);
}

void MenuItem::SetDimensions(int x, int y, int width, int height)
{
	int menuItemSpacer = 0;

	Menu* pMenuParent = static_cast<Menu*>(GetParent());
	if (pMenuParent)
	{
		menuItemSpacer = pMenuParent->GetMenuItemSpacer();
	}

	//width += lMenuItemSpacer * 2;
	height += menuItemSpacer * 2;

	Component::SetDimensions(x, y, width, height);

	// Also set the dimensions of the icons
	m_pDefaultIcon->SetDimensions(0, 0, width, height);
	m_pSelectedIcon->SetDimensions(0, 0, width, height);
	m_pHoverIcon->SetDimensions(0, 0, width, height);
	m_pDisabledIcon->SetDimensions(0, 0, width, height);

	// Also set the dimensions of the label
	//m_label.SetLocation(menuItemSpacer, menuItemSpacer);
	m_label.SetLocation(3, 3);
}

void MenuItem::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void MenuItem::SetWidth(int width)
{
	//Menu* pMenuParent = static_cast<Menu*>(GetParent());
	//if(pMenuParent)
	//{
	//	width += pMenuParent->GetMenuItemSpacer() * 2;
	//}

	Component::SetWidth(width);

	// Also set the width of the icons
	m_pDefaultIcon->SetWidth(width);
	m_pSelectedIcon->SetWidth(width);
	m_pHoverIcon->SetWidth(width);
	m_pDisabledIcon->SetWidth(width);
}

void MenuItem::SetHeight(int height)
{
	Menu* pMenuParent = static_cast<Menu*>(GetParent());
	if (pMenuParent)
	{
		height += pMenuParent->GetMenuItemSpacer() * 2;
	}

	Component::SetHeight(height);

	// Also set the height of the icons
	m_pDefaultIcon->SetHeight(height);
	m_pSelectedIcon->SetHeight(height);
	m_pHoverIcon->SetHeight(height);
	m_pDisabledIcon->SetHeight(height);
}

void MenuItem::SetOnlyShowSelectedIcons(bool show)
{
	m_onlyShowSelectedItems = show;

	if (m_onlyShowSelectedItems)
	{
		m_pSelectedIcon->SetVisible(false);
		m_pHoverIcon->SetVisible(false);
		m_pDisabledIcon->SetVisible(false);
		m_pDefaultIcon->SetVisible(false);
	}
}

bool MenuItem::IsOnlyShowSelectedIcons() const
{
	return m_onlyShowSelectedItems;
}

ComponentType MenuItem::GetComponentType() const
{
	return ComponentType::MenuItem;
}

bool MenuItem::IsParentMenuOpen() const
{
	Menu* pParentMenu = static_cast<Menu*>(GetParent());

	// Check to see if our parent menu is currently open
	if (pParentMenu != nullptr)
	{
		if (pParentMenu->IsMenuOpen())
		{
			return true;
		}
	}

	return false;
}

void MenuItem::MenuItemSelectedFromPulldowmMenu()
{
	Menu* pParentMenu = static_cast<Menu*>(GetParent());

	if (pParentMenu && pParentMenu->GetComponentType() == ComponentType::Menu)
	{
		PulldownMenu* pParentPulldownMenu = reinterpret_cast<PulldownMenu*>(pParentMenu->GetParent());

		if (pParentPulldownMenu && pParentPulldownMenu->GetComponentType() == ComponentType::PulldownMenu)
		{
			// Store this as the selected menu item
			pParentPulldownMenu->SetSelectedMenuItem(this);

			pParentPulldownMenu->SetTitleString(GetLabel().GetText());

			// Pulldown menu item changed callback
			pParentPulldownMenu->MenuItemChanged();
		}
	}
}

void MenuItem::MouseEntered(const MouseEvent& event)
{
	if (!IsParentMenuOpen())
	{
		return;
	}

	Menu* pParentMenu = static_cast<Menu*>(GetParent());

	// Make sure that we are inside the bounds of the parent menu
	if (pParentMenu->GetPullDownMenuParent() != nullptr)
	{
		int textHeight = m_pRenderer->GetFreeTypeTextHeight(pParentMenu->GetPullDownMenuParent()->GetGUIFont(), "%s", pParentMenu->GetMenuTitle().c_str());
		int menuHeight = textHeight + (pParentMenu->GetMenuItemSpacer() * 2);
		int fullMenuDisplayHeight = pParentMenu->GetPullDownMenuParent()->GetMaxNumItemsDisplayed() * menuHeight;

		Point location = pParentMenu->GetPullDownMenuParent()->GetLocation();
		for (Component* parent = pParentMenu->GetPullDownMenuParent()->GetParent(); parent != nullptr;)
		{
			Point parentLocation = parent->GetLocation();

			location.x += parentLocation.x;
			location.y += parentLocation.y;

			parent = parent->GetParent();
		}
		int menuX = location.x;
		int menuY = location.y - fullMenuDisplayHeight;
		int menuWidth = pParentMenu->GetBiggestWidth() + (pParentMenu->GetMenuItemSpacer() * 2);

		if (event.GetX() > menuX && event.GetX() <= menuX + menuWidth && event.GetY() > menuY && event.GetY() <= menuY + fullMenuDisplayHeight)
		{
			SetHover(true);

			OnMouseEnter();
		}
	}
}

void MenuItem::MouseExited(const MouseEvent& event)
{
	if (!IsParentMenuOpen())
	{
		return;
	}

	SetHover(false);

	// Also removed the button selection if we exit it's dimensions
	SetSelected(false);

	OnMouseExit();
}

void MenuItem::MousePressed(const MouseEvent& event)
{
	if (!IsParentMenuOpen())
	{
		return;
	}

	// If our parent is a GUIWindow, then make this window have focus in the GUI, used to make it's depth the highest
	if (GetParent() != nullptr && GetParent()->GetComponentType() == ComponentType::GUIWindow)
	{
		GUIWindow* pParentWindow = static_cast<GUIWindow*>(GetParent());
		pParentWindow->SetFocusWindow();
	}

	SetSelected(true);

	OnMousePressed();
}

void MenuItem::MouseReleased(const MouseEvent& event)
{
	if (!IsParentMenuOpen())
	{
		return;
	}

	Menu* pParentMenu = static_cast<Menu*>(GetParent());

	// Make sure that we are inside the bounds of the parent menu
	if (pParentMenu->GetPullDownMenuParent() != nullptr)
	{
		int textHeight = m_pRenderer->GetFreeTypeTextHeight(pParentMenu->GetPullDownMenuParent()->GetGUIFont(), "%s", pParentMenu->GetMenuTitle().c_str());
		int menuHeight = textHeight + (pParentMenu->GetMenuItemSpacer() * 2);
		int fullMenuDisplayHeight = pParentMenu->GetPullDownMenuParent()->GetMaxNumItemsDisplayed() * menuHeight;

		Point location = pParentMenu->GetPullDownMenuParent()->GetLocation();
		for (Component* parent = pParentMenu->GetPullDownMenuParent()->GetParent(); parent != nullptr;)
		{
			Point parentLocation = parent->GetLocation();

			location.x += parentLocation.x;
			location.y += parentLocation.y;

			parent = parent->GetParent();
		}
		int menuX = location.x;
		int menuY = location.y - fullMenuDisplayHeight;
		int menuWidth = pParentMenu->GetBiggestWidth() + (pParentMenu->GetMenuItemSpacer() * 2);

		if (event.GetX() > menuX && event.GetX() <= menuX + menuWidth && event.GetY() > menuY && event.GetY() <= menuY + fullMenuDisplayHeight)
		{
			// Close the menu, since we have clicked this menu item
			pParentMenu->CloseMenu();

			SetHover(false);
			SetSelected(false);

			// Signal that we have pressed this menu item
			MenuItemPressed();
		}
	}


	FocusManager::GetInstance()->SetFocusOwner(nullptr);

	OnMouseReleased();
}

void MenuItem::MouseReleasedOutside(const MouseEvent& event)
{
	FocusManager::GetInstance()->SetFocusOwner(nullptr);

	Menu* pParentMenu = static_cast<Menu*>(GetParent());
	pParentMenu->CloseMenu();
}

void MenuItem::MouseClicked(const MouseEvent& event)
{
	if (!IsParentMenuOpen())
	{
		return;
	}

	SetHover(false);
	SetSelected(false);

	// Close the menu, since we have clicked this menu item
	Menu* pParentMenu = static_cast<Menu*>(GetParent());
	pParentMenu->CloseMenu();

	// Make sure that we are inside the bounds of the parent menu
	if (pParentMenu->GetPullDownMenuParent() != nullptr)
	{
		int textHeight = m_pRenderer->GetFreeTypeTextHeight(pParentMenu->GetPullDownMenuParent()->GetGUIFont(), "%s", pParentMenu->GetMenuTitle().c_str());
		int menuHeight = textHeight + (pParentMenu->GetMenuItemSpacer() * 2);
		int fullMenuDisplayHeight = pParentMenu->GetPullDownMenuParent()->GetMaxNumItemsDisplayed() * menuHeight;

		Point location = pParentMenu->GetPullDownMenuParent()->GetLocation();
		for (Component* parent = pParentMenu->GetPullDownMenuParent()->GetParent(); parent != nullptr;)
		{
			Point parentLocation = parent->GetLocation();

			location.x += parentLocation.x;
			location.y += parentLocation.y;

			parent = parent->GetParent();
		}
		int menuX = location.x;
		int menuY = location.y - fullMenuDisplayHeight;
		int menuWidth = pParentMenu->GetBiggestWidth() + (pParentMenu->GetMenuItemSpacer() * 2);

		if (event.GetX() > menuX && event.GetX() <= menuX + menuWidth && event.GetY() > menuY && event.GetY() <= menuY + fullMenuDisplayHeight)
		{
			// Signal that we have pressed this menu item
			MenuItemPressed();
		}
	}

	FocusManager::GetInstance()->SetFocusOwner(nullptr);

	OnMouseClicked();
}

void MenuItem::MenuItemPressed() const
{
	// Call the callback function callback if this class is just a simple button
	if (m_callback)
	{
		m_callback(m_pCallbackData);
	}
}

void MenuItem::FocusLost(const FocusEvent& event)
{
	if (!IsParentMenuOpen())
	{
		return;
	}

	Menu* pParentMenu = static_cast<Menu*>(GetParent());
	pParentMenu->CloseMenu();
}

void MenuItem::FocusGained(const FocusEvent& event)
{
	if (!IsParentMenuOpen())
	{
		FocusManager::GetInstance()->SetFocusOwner(nullptr);
	}
}

void MenuItem::OnMouseEnter()
{
	// Do nothing
}

void MenuItem::OnMouseExit()
{
	// Do nothing
}

void MenuItem::OnMousePressed()
{
	// Do nothing
}

void MenuItem::OnMouseReleased()
{
	// Do nothing
}

void MenuItem::OnMouseClicked()
{
	// Do nothing
}

void MenuItem::DrawSelf()
{
	// Reset visibility
	m_pSelectedIcon->SetVisible(false);
	m_pHoverIcon->SetVisible(false);
	m_pDisabledIcon->SetVisible(false);
	m_pDefaultIcon->SetVisible(false);

	if (!IsParentMenuOpen())
	{
		m_label.SetVisible(false);

		return;
	}

	if (m_onlyShowSelectedItems)
	{
		if (IsSelected())
		{
			m_pSelectedIcon->SetVisible(true);
		}
		else if (IsHover())
		{
			m_pHoverIcon->SetVisible(true);
		}
	}
	else
	{
		if (IsSelected())
		{
			m_pSelectedIcon->SetVisible(true);
		}
		else if (IsHover())
		{
			m_pHoverIcon->SetVisible(true);
		}
		else if (IsDisabled())
		{
			m_pDisabledIcon->SetVisible(true);
		}
		else
		{
			m_pDefaultIcon->SetVisible(true);
		}
	}

	// Set the label visibility
	m_label.SetVisible(true);
}