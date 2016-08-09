/*************************************************************************
> File Name: Button.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A simple button class that defines standard button behavior.
> Created Time: 2016/08/09
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BUTTON_H
#define CUBBY_BUTTON_H

#include "AbstractButton.h"

class Button : public AbstractButton, public MouseListener
{
public:
	// Constructor, Destructor
	Button(Renderer* pRenderer, unsigned int GUIFont, const std::string& label);
	Button(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color fontColor, Color outlineColor);
	Button(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, const std::string& defaultTexture, const std::string& selectedTexture, const std::string& hoverTexture, const std::string& disabledTexture);

	~Button();

	void AddText(Renderer* pRenderer, unsigned int GUIFont, unsigned int OutlineGUIFont, const std::string& label, Color color, int xOffset, int yOffset, bool outline = false, Color outlineColor = Color(1.0f, 1.0f, 1.0f, 1.0f));
	void AddIcon(Renderer* pRenderer, const std::string& fileName, int texWidth, int texHeight, int width, int height, int xOffset, int yOffset, float depth);

	void SetDimensions(int x, int y, int width, int height);
	void SetDimensions(const Dimensions& r);

	void SetLabelOffset(int x, int y);

	void SetPressedOffset(int x, int y);

	void SetNormalLabelColor(Color normal);
	void SetHoverLabelColor(Color hover);
	void SetPressedLabelColor(Color pressed);

	void SetLabelColor(Color color);
	void SetLabelOutlineColor(Color color);

	void AddEventListeners();
	void RemoveEventListeners();

	ComponentType GetComponentType() const;

protected:
	void MouseEntered(const MouseEvent& event);
	void MouseExited(const MouseEvent& event);
	void MousePressed(const MouseEvent& event);
	void MouseReleased(const MouseEvent& event);
	void MouseClicked(const MouseEvent& event);

	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnMousePressed();
	virtual void OnMouseReleased();
	virtual void OnMouseClicked();

	void DrawSelf();

private:
	ComponentList m_vpAddedComponentList;
	
	int m_pressedOffsetX;
	int m_pressedOffsetY;
	bool m_offsetApplied;

	bool m_changeLabelText;
	Color m_normalLabelColor;
	Color m_hoverLabelColor;
	Color m_PressedLabelColor;
};

#endif