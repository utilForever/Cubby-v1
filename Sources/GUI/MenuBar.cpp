/*************************************************************************
> File Name: MenuBar.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A menu bar is a container of menus.
> Created Time: 2016/08/20
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "MenuBar.h"

// Constructor, Destructor
MenuBar::MenuBar(Renderer* pRenderer) :
	Container(pRenderer), m_biggestHeight(0), m_currentMenuBarWidth(0)
{

}

MenuBar::~MenuBar()
{

}

void MenuBar::Add(Menu* pMenu)
{
	Container::Add(pMenu);

	int newMenuWidth = pMenu->GetDimensions().width - (pMenu->GetMenuItemSpacer() * 2);
	int newMenuHeight = pMenu->GetDimensions().height - (pMenu->GetMenuItemSpacer() * 2);

	// Check to see if this new menu has a bigger height
	if (newMenuHeight > m_biggestHeight)
	{
		m_biggestHeight = newMenuHeight;

		SetHeight(m_biggestHeight + (pMenu->GetMenuItemSpacer() * 2));

		// Notify all our children menu items of the NEW biggest height
		for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
		{
			if ((*iter)->GetComponentType() == ComponentType::Menu)
			{
				(*iter)->SetHeight(m_biggestHeight);
			}
		}
	}

	// Set the location of the new menu
	pMenu->SetDimensions(m_currentMenuBarWidth, 0, newMenuWidth, m_biggestHeight);

	// Reset the main menu to be the width of its original text length
	pMenu->ResetToTextWidth();

	// Get this original menu width and use this as the new width marker, this means menu bars dont have massive
	// gaps for the biggest entry size.
	int originalMenuWidth = pMenu->GetDimensions().width - (pMenu->GetMenuItemSpacer() * 2);

	m_currentMenuBarWidth += originalMenuWidth + (pMenu->GetMenuItemSpacer() * 2);

	// Extend our width
	SetWidth(m_currentMenuBarWidth);
}

void MenuBar::CloseAllMenus()
{
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if ((*iter)->GetComponentType() == ComponentType::Menu)
		{
			Menu* pMenu = static_cast<Menu*>(*iter);

			pMenu->CloseMenu();
		}
	}
}

ComponentType MenuBar::GetComponentType() const
{
	return ComponentType::MenuBar;
}

void MenuBar::DrawSelf()
{
	//Container::DrawSelf();
}