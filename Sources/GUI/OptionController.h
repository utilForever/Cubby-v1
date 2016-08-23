/*************************************************************************
> File Name: OptionController.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A controller for a group of option boxes.
> Created Time: 2016/08/24
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_OPTION_CONTROLLER_H
#define CUBBY_OPTION_CONTROLLER_H

#include "Container.h"
#include "OptionBox.h"

class OptionController : public Container
{
public:
	// Constructor, Destructor
	OptionController(Renderer* pRenderer, unsigned int GUIFont, const std::string& label);
	OptionController(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color fontColor, Color outlineColor);

	~OptionController();

	void SetVisible(bool visible);
	bool IsVisible() const;

	void SetDimensions(int x, int y, int width, int height);
	void SetDimensions(const Dimensions& r);

	void SetDisplayLabel(bool display);
	void SetDisplayBorder(bool display);

	void SetActiveOption(OptionBox* pOption);
	OptionBox* GetActiveOption() const;

	void ResetAllOptions();

	ComponentType GetComponentType() const;

protected:
	void DrawSelf();

private:
	Label m_label;

	int m_labelIndent;

	bool m_displayLabel;
	bool m_displayBorder;

	OptionBox* m_pActiveOption;
};

#endif