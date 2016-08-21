/*************************************************************************
> File Name: Menu.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A menu is a container of menu items.
> Created Time: 2016/08/21
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Menu.h"
#include "DirectDrawRectangle.h"
#include "Icon.h"
#include "MenuBar.h"
#include "GUIWindow.h"

// Constructor, Destructor
Menu::Menu(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, bool showTopLevelMenu) :
	AbstractButton(pRenderer, GUIFont, label), m_showTopLevelMenu(showTopLevelMenu)
{
	m_pDefaultIcon = new DirectDrawRectangle(pRenderer);
	m_pHoverIcon = new DirectDrawRectangle(pRenderer);
	m_pSelectedIcon = new DirectDrawRectangle(pRenderer);
	m_pDisabledIcon = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle *pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pDefaultIcon);
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

	if (m_showTopLevelMenu)
	{
		Container::Add(m_pDefaultIcon);
		Container::Add(m_pSelectedIcon);
		Container::Add(m_pHoverIcon);
		Container::Add(m_pDisabledIcon);

		m_label.SetLocation(0, 0);
		Container::Add(&m_label);
	}

	m_menuOpen = false;

	m_autosizeMenuBase = true;

	m_biggestWidth = 0;
	m_currentMenuheight = 0;
	m_menuItemSpacer = 0;

	m_pPullDownMenuParent = nullptr;
}


Menu::Menu(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, bool showTopLevelMenu, const std::string& defaultTexture, const std::string& selectedTexture, const std::string& hoverTexture, const std::string& disabledTexture) :
	AbstractButton(pRenderer, GUIFont, label), m_showTopLevelMenu(showTopLevelMenu)
{
	m_pDefaultIcon = new Icon(pRenderer, defaultTexture);
	m_pSelectedIcon = new Icon(pRenderer, selectedTexture);
	m_pHoverIcon = new Icon(pRenderer, hoverTexture);
	m_pDisabledIcon = new Icon(pRenderer, disabledTexture);

	if (m_showTopLevelMenu)
	{
		Container::Add(m_pDefaultIcon);
		Container::Add(m_pSelectedIcon);
		Container::Add(m_pHoverIcon);
		Container::Add(m_pDisabledIcon);

		m_label.SetLocation(0, 0);
		Container::Add(&m_label);
	}

	m_menuOpen = false;

	m_autosizeMenuBase = true;

	m_biggestWidth = 0;
	m_currentMenuheight = 0;
	m_menuItemSpacer = 0;

	m_pPullDownMenuParent = nullptr;
}

Menu::~Menu()
{
	delete m_pDefaultIcon;
	delete m_pSelectedIcon;
	delete m_pHoverIcon;
	delete m_pDisabledIcon;
}

void Menu::ResetMenu()
{
	m_menuOpen = false;

	m_biggestWidth = GetDimensions().width;
	m_currentMenuheight = 0;
	m_menuItemSpacer = 2;
}

void Menu::AddEventListeners()
{
	if (m_showTopLevelMenu)
	{
		AddMouseListener(this);
		AddFocusListener(this);
	}

	// Also add all children's event listeners
	Container::AddEventListeners();
}

void Menu::RemoveEventListeners()
{
	if (m_showTopLevelMenu)
	{
		RemoveMouseListener(this);
		RemoveFocusListener(this);
	}

	// Also remove all children's event listeners
	Container::RemoveEventListeners();

	// Remove hover and select properties, since we have made this component un-selectable by the mouse
	SetHover(false);
	SetSelected(false);

	SetFocus(false);
}

void Menu::SetPullDownMenuParent(PulldownMenu* pPullDownMenuParent)
{
	m_pPullDownMenuParent = pPullDownMenuParent;
}

PulldownMenu* Menu::GetPullDownMenuParent() const
{
	return m_pPullDownMenuParent;
}

void Menu::Add(MenuItem* pMenuItem, bool scrollBarPresent)
{
	Container::Add(pMenuItem);

	int newMenuItemWidth = pMenuItem->GetDimensions().width;
	int newMenuItemHeight = pMenuItem->GetDimensions().height;

	// PULLDOWN MENU HACK: Bit of a hack, but if the scrollbar is not present on pulldown menus, then take up this space with the menu item
	if (!scrollBarPresent)
	{
		newMenuItemWidth += (newMenuItemHeight + m_menuItemSpacer * 2);
	}

	// Check if this new menu item has a bigger width than our current biggest width
	if (newMenuItemWidth > m_biggestWidth)
	{
		m_biggestWidth = newMenuItemWidth;

		if (m_autosizeMenuBase)
		{
			if (!scrollBarPresent)
			{
				SetWidth(m_biggestWidth - (newMenuItemHeight + m_menuItemSpacer * 2));
			}
			else
			{
				SetWidth(m_biggestWidth);
			}
		}

		// Notify all our children menu items of the NEW biggest width
		for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
		{
			if ((*iter)->GetComponentType() == ComponentType::MenuItem)
			{
				(*iter)->SetWidth(m_biggestWidth);
			}
		}
	}

	// Set the location of the new menu item
	pMenuItem->SetDimensions(0, -m_currentMenuheight, m_biggestWidth - 1, newMenuItemHeight);

	m_currentMenuheight += newMenuItemHeight + m_menuItemSpacer * 2;
}

MenuItem* Menu::FindItem(const std::string& label)
{
	// Go through all our children and ensure that 
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if ((*iter)->GetComponentType() == ComponentType::MenuItem)
		{
			MenuItem* pMenuItem = static_cast<MenuItem*>(*iter);
			if (strcmp(pMenuItem->GetLabel().GetText().c_str(), label.c_str()) == 0)
			{
				return pMenuItem;
			}
		}
	}

	return nullptr;
}

void Menu::SetAutoSizeMenuWidth(bool autoSize)
{
	m_autosizeMenuBase = autoSize;
}

void Menu::SetDimensions(int x, int y, int width, int height)
{
	if (width < m_biggestWidth)
	{
		width = m_biggestWidth;
	}
	else
	{
		m_biggestWidth = width;
	}

	//width += m_menuItemSpacer * 2;
	height += m_menuItemSpacer * 2;
	if (m_showTopLevelMenu)
	{
		m_currentMenuheight += height;
	}

	Component::SetDimensions(x, y, width, height);

	// Also set the dimensions of the icons
	m_pDefaultIcon->SetDimensions(0, 0, width, height);
	m_pSelectedIcon->SetDimensions(0, 0, width, height);
	m_pHoverIcon->SetDimensions(0, 0, width, height);
	m_pDisabledIcon->SetDimensions(0, 0, width, height);

	// Also set the dimensions of the label
	m_label.SetLocation(m_menuItemSpacer, m_menuItemSpacer);
}

void Menu::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void Menu::SetWidth(int width)
{
	//width += m_menuItemSpacer * 2;

	Component::SetWidth(width);

	// Also set the width of the icons
	m_pDefaultIcon->SetWidth(width);
	m_pSelectedIcon->SetWidth(width);
	m_pHoverIcon->SetWidth(width);
	m_pDisabledIcon->SetWidth(width);
}

void Menu::SetHeight(int height)
{
	height += m_menuItemSpacer * 2;
	if (m_showTopLevelMenu)
	{
		m_currentMenuheight += height;
	}

	Component::SetHeight(height);

	// Also set the height of the icons
	m_pDefaultIcon->SetHeight(height);
	m_pSelectedIcon->SetHeight(height);
	m_pHoverIcon->SetHeight(height);
	m_pDisabledIcon->SetHeight(height);
}

void Menu::ResetToTextWidth()
{
	int lWidth = m_pRenderer->GetFreeTypeTextWidth(m_label.GetFontID(), "%s", m_label.GetText().c_str());

	SetWidth(lWidth);
}

void Menu::SetMenuItemSpacer(int spacer)
{
	m_menuItemSpacer = spacer;
}

int Menu::GetMenuItemSpacer() const
{
	return m_menuItemSpacer;
}

int Menu::GetBiggestWidth() const
{
	return m_biggestWidth;
}

int Menu::GetCurrentMenuheight() const
{
	return m_currentMenuheight;
}

std::string Menu::GetMenuTitle() const
{
	return m_label.GetText();
}

void Menu::OpenMenu()
{
	m_menuOpen = true;

	// Enable all our menu items when we open the menu, so that they can get focus
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if ((*iter)->GetComponentType() == ComponentType::MenuItem)
		{
			(*iter)->SetEnabled(true);
		}
	}
}

void Menu::CloseMenu()
{
	m_menuOpen = false;

	// Disable all our menu items when we close the menu, so that they cannot get focus
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if ((*iter)->GetComponentType() == ComponentType::MenuItem)
		{
			(*iter)->SetEnabled(false);
		}
	}
}

bool Menu::IsMenuOpen() const
{
	return m_menuOpen;
}
bool Menu::ChildHasFocus()
{
	// Check to see if one of our children gained focus
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if ((*iter)->GetComponentType() == ComponentType::MenuItem)
		{
			if ((*iter)->HasFocus())
			{
				return true;
			}
		}
	}

	return false;
}

ComponentType Menu::GetComponentType() const
{
	return ComponentType::Menu;
}

void Menu::MouseEntered(const MouseEvent& event)
{
	SetHover(true);

	OnMouseEnter();
}

void Menu::MouseExited(const MouseEvent& event)
{
	SetHover(false);

	// Also removed the button selection if we exit it's dimensions
	SetSelected(false);

	OnMouseExit();
}

void Menu::MousePressed(const MouseEvent& event)
{
	if (m_menuOpen)
	{
		MenuBar* pMenuBar = static_cast<MenuBar*>(GetParent());

		if (pMenuBar != nullptr)
		{
			pMenuBar->CloseAllMenus();
		}
	}
	else
	{
		MenuBar* pMenuBar = static_cast<MenuBar*>(GetParent());

		if (pMenuBar != nullptr)
		{
			pMenuBar->CloseAllMenus();
		}

		OpenMenu();
	}

	// If our parent is a GUIWindow, then make this window have focus in the GUI, used to make it's depth the highest
	if (GetParent() != nullptr && GetParent()->GetComponentType() == ComponentType::GUIWindow)
	{
		GUIWindow* pParentWindow = static_cast<GUIWindow*>(GetParent());
		pParentWindow->SetFocusWindow();
	}

	OnMousePressed();
}

void Menu::MouseReleased(const MouseEvent& event)
{
	SetSelected(false);

	OnMouseReleased();
}

void Menu::MouseReleasedOutside(const MouseEvent& event)
{
	CloseMenu();
}

void Menu::MouseClicked(const MouseEvent& event)
{
	OnMouseClicked();
}

void Menu::FocusLost(const FocusEvent& event)
{
	if (!ChildHasFocus())
	{
		CloseMenu();
	}
}

void Menu::FocusGained(const FocusEvent& event)
{

}

void Menu::OnMouseEnter()
{
	// Do nothing
}

void Menu::OnMouseExit()
{
	// Do nothing
}

void Menu::OnMousePressed()
{
	// Do nothing
}

void Menu::OnMouseReleased()
{
	// Do nothing
}

void Menu::OnMouseClicked()
{
	// Do nothing
}

void Menu::DrawSelf()
{
	// Reset visibility
	m_pSelectedIcon->SetVisible(false);
	m_pHoverIcon->SetVisible(false);
	m_pDisabledIcon->SetVisible(false);
	m_pDefaultIcon->SetVisible(false);

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

	// Set the label visibility
	m_label.SetVisible(true);
}