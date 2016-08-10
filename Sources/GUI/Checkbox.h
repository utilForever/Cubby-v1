/*************************************************************************
> File Name: Checkbox.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A checkbox GUI widget.
> Created Time: 2016/08/10
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_CHECKBOX_H
#define CUBBY_CHECKBOX_H

#include "AbstractButton.h"

class CheckBox : public AbstractButton, public MouseListener
{
public:
	// Constructor, Destructor
	CheckBox(Renderer* pRenderer, unsigned int GUIFont, const std::string& label);
	CheckBox(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color fontColor, Color outlineColor);
	CheckBox(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, const std::string& defaultTexture, const std::string& selectedTexture, const std::string& hoverTexture, const std::string& disabledTexture,
		const std::string& toggledTexture, const std::string& toggledSelectedTexture, const std::string& toggledHoverTexture, const std::string& toggledDisabledTexture);

	~CheckBox();

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	void RemoveIcon(RenderRectangle* pRemoveIcon) override;
	void SetDefaultIcons(Renderer* pRenderer) override;
	void SetToggledIcon(RenderRectangle* icon);
	void SetToggledSelectedIcon(RenderRectangle* icon);
	void SetToggledHoverIcon(RenderRectangle* icon);
	void SetToggledDisabledIcon(RenderRectangle* icon);

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	void SetPressedOffset(int x, int y);

	void SetLabelColor(Color color);
	void SetNormalLabelColor(Color normal);
	void SetHoverLabelColor(Color hover);
	void SetPressedLabelColor(Color pressed);

	bool GetToggled() const;
	void SetToggled(bool toggle);

	ComponentType GetComponentType() const override;

protected:
	void MouseEntered(const MouseEvent& event) override;
	void MouseExited(const MouseEvent& event) override;
	void MousePressed(const MouseEvent& event) override;
	void MouseReleased(const MouseEvent& event) override;
	void MouseClicked(const MouseEvent& event) override;

	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnMousePressed();
	virtual void OnMouseReleased();
	virtual void OnMouseClicked();

	void DrawSelf() override;

private:
	RenderRectangle* m_pToggledIcon;
	RenderRectangle* m_pToggledSelectedIcon;
	RenderRectangle* m_pToggledHoverIcon;
	RenderRectangle* m_pToggledDisabledIcon;

	bool m_toggled;

	int m_pressedOffsetX;
	int m_pressedOffsetY;
	bool m_offsetApplied;

	bool m_changeLabelText;
	Color m_normalLabelColor;
	Color m_hoverLabelColor;
	Color m_PressedLabelColor;
};


#endif