/*************************************************************************
> File Name: Label.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A simple GUI component that simple displays a text label.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_LABEL_H
#define CUBBY_LABEL_H

#include "Component.h"

class Label : public Component
{
public:
	// Constructor, Destructor
	Label(Renderer* pRenderer, unsigned int GUIFont, const std::string &label, Color color);
	~Label();

	void SetText(const std::string& label);
	const std::string& GetText() const;

	void SetFontID(unsigned int fontID);
	unsigned int GetFontID() const;

	void SetColor(Color color);
	void SetOutlineColor(Color color);
	void SetOutlineFont(unsigned int outlineGUIFont);

	void GetColor(float* r, float* g, float* b, float* a) const;
	void GetOutlineColor(float* r, float* g, float* b, float* a) const;

	void SetOutline(bool outline);

	void SetVerticalAlignment(VerticalAlignment alignment);
	VerticalAlignment GetVerticalAlignment() const;

	void SetHorizontalAlignment(HorizontalAlignment alignment);
	HorizontalAlignment GetHorizontalAlignment() const;

	ComponentType GetComponentType() const override;

	void Insert(int index, char* character);
	void Erase(int start, int num);

protected:
	void DrawSelf() override;

private:
	std::string m_text;

	Color m_color;
	Color m_outlineColor;

	bool m_outline;

	VerticalAlignment m_eVerticalAlignment;
	HorizontalAlignment m_eHorizontalAlignment;

	unsigned int m_GUIFont;
	unsigned int m_outlineGUIFont;
};


#endif