/*************************************************************************
> File Name: OptionBox.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An option box (radio button) GUI widget.
> Created Time: 2016/08/23
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_OPTION_BOX_H
#define CUBBY_OPTION_BOX_H

#include "AbstractButton.h"

class OptionBox : public AbstractButton, public MouseListener
{
public:
	// Constructor, Destructor
	OptionBox(Renderer* pRenderer, unsigned int GUIFont, const std::string& label);
	OptionBox(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color fontColor, Color outlineColor);
	OptionBox(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, const std::string& defaultTexture, const std::string& selectedTexture, const std::string& hoverTexture, const std::string& disabledTexture,
		const std::string& toggledTexture, const std::string& toggledSelectedTexture, const std::string& toggledHoverTexture, const std::string& toggledDisabledTexture);

	~OptionBox();

	void AddEventListeners();
	void RemoveEventListeners();

	void RemoveIcon(RenderRectangle* pRemoveIcon);
	void SetDefaultIcons(Renderer* pRenderer);
	void SetToggledIcon(RenderRectangle* icon);
	void SetToggledSelectedIcon(RenderRectangle* icon);
	void SetToggledHoverIcon(RenderRectangle* icon);
	void SetToggledDisabledIcon(RenderRectangle* icon);

	void SetDimensions(int x, int y, int width, int height);
	void SetDimensions(const Dimensions& r);

	void SetLabelOffset(int x, int y);
	void SetLabelPosition(int x, int y);

	void SetPressedOffset(int x, int y);

	void SetLabelColor(Color color);
	void SetNormalLabelColor(Color normal);
	void SetHoverLabelColor(Color hover);
	void SetPressedLabelColor(Color pressed);

	bool GetToggled() const;
	void SetToggled(bool toggle);

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
	RenderRectangle* m_pToggledIcon;
	RenderRectangle* m_pToggledSelectedIcon;
	RenderRectangle* m_pToggledHoverIcon;
	RenderRectangle* m_pToggledDisabledIcon;

	bool m_isToggled;

	int m_pressedOffsetX;
	int m_pressedOffsetY;
	bool m_isOffsetApplied;

	bool m_changeLabelText;
	Color m_normalLabelColor;
	Color m_hoverLabelColor;
	Color m_PressedLabelColor;
};

#endif