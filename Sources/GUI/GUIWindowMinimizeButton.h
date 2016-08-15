/*************************************************************************
> File Name: GUIWindowMinimizeButton.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A button that is included in the title bar of a gui window
>    clicking on this button will minimize the window.
> Created Time: 2016/08/15
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_GUI_WINDOW_MINIMIZE_BUTTON_H
#define CUBBY_GUI_WINDOW_MINIMIZE_BUTTON_H

#include "Button.h"

// Forward declaration
class GUIWindow;

class GUIWindowMinimizeButton : public Button
{
public:
	// Constructor, Destructor
	GUIWindowMinimizeButton(Renderer* pRenderer, unsigned int GUIFont, const std::string& label);
	~GUIWindowMinimizeButton();

	void SetGUIWindowParent(GUIWindow* pParent);

protected:
	void OnMouseEnter() override;
	void OnMouseExit() override;
	void OnMousePressed() override;
	void OnMouseReleased() override;
	void OnMouseClicked() override;

private:
	GUIWindow* m_pParentGUIWindow;
};

#endif