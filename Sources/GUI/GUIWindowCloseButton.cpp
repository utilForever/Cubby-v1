/*************************************************************************
> File Name: GUIWindowCloseButton.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A button that is included in the title bar of a gui window
>    clicking on this button will close the window.
> Created Time: 2016/08/15
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "GUIWindow.h"
#include "GUIWindowCloseButton.h"

// Constructor, Destructor
GUIWindowCloseButton::GUIWindowCloseButton(Renderer* pRenderer, unsigned int GUIFont, const std::string& label)	:
	Button(pRenderer, GUIFont, label)
{
	// Get the height of the title bar
	// (TEMP: This shouldn't be the correct way to make the button dimensions!)
	//int distanceFromBorder = 2;
	//int buttonSize = m_pRenderer->GetFreeTypeTextHeight(GUIFont, "a") - distanceFromBorder;

	//int buttonX = parent->GetDimensions().m_width - buttonSize - distanceFromBorder;
	//SetDimensions(buttonX, distanceFromBorder/2, buttonSize, buttonSize);

	SetDisplayLabel(false);
}

GUIWindowCloseButton::~GUIWindowCloseButton()
{

}

void GUIWindowCloseButton::SetGUIWindowParent(GUIWindow* pParent)
{
	m_pParentGUIWindow = pParent;
}

void GUIWindowCloseButton::OnMouseEnter()
{

}

void GUIWindowCloseButton::OnMouseExit()
{

}

void GUIWindowCloseButton::OnMousePressed()
{

}

void GUIWindowCloseButton::OnMouseReleased()
{

}

void GUIWindowCloseButton::OnMouseClicked()
{
	// Don't allow closing the window if we have disabled this
	if (m_pParentGUIWindow->m_allowClosing == false)
	{
		return;
	}

	m_pParentGUIWindow->Hide();
}