/*************************************************************************
> File Name: MenuBar.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A menu bar is a container of menus.
> Created Time: 2016/08/20
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MENU_BAR_H
#define CUBBY_MENU_BAR_H

#include "Menu.h"

class MenuBar : public Container, public MouseListener
{
public:
	// Constructor, Destructor
	MenuBar(Renderer* pRenderer);
	~MenuBar();

	void Add(Menu* pMenu);

	void CloseAllMenus();

	ComponentType GetComponentType() const override;

protected:
	void DrawSelf() override;

private:
	int m_biggestHeight;
	int m_currentMenuBarWidth;
};

#endif