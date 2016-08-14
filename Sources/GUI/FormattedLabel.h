/*************************************************************************
> File Name: FormattedLabel.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A formatted label component that can render formatted text strings.
> 	 Constructs multiple simple labels when the formatted text is parsed and
> 	 is in charge of managing and rendering these labels.
> Created Time: 2016/08/11
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_FORMATTED_LABEL_H
#define CUBBY_FORMATTED_LABEL_H

#include "Component.h"
#include "Label.h"

// Global constants
static const int MAX_NUM_LINES = 30;

class FormattedLabel : public Component
{
public:
	// Constructor, Destructor
	FormattedLabel(Renderer* pRenderer, unsigned int normalFont, unsigned int boldFont, unsigned int italicFont, const std::string& label);
	~FormattedLabel();

	void SetColor(Color color);
	void SetOutlineColor(Color color);
	void SetOutlineFont(unsigned int outlineGUIFont);

	void SetOutline(bool outline);

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	void SetVerticalAlignment(VerticalAlignment alignment);
	VerticalAlignment GetVerticalAlignment() const;

	void SetHorizontalAlignment(HorizontalAlignment alignment);
	HorizontalAlignment GetHorizontalAlignment() const;

	void SetWordWrap(bool wrap);
	bool IsWordWrapEnabled() const;

	void SetNewLineSpacer(int spacer);

	void SetText(const std::string& label);
	const std::string& GetText() const;

	bool DoesTextExist(const char* pText);
	Label* GetLabel(const char* pText);

	int GetNumberOfLines() const;
	int GetNumberOfLabels() const;
	int GetMinimumTextHeight() const;

	void UpdateText();

	void ReAlignText();

protected:
	void ParseText();

	void ClearLabels();

	void DrawSelf() override;

private:
	std::string m_text;

	Color m_color;
	Color m_outlineColor;

	bool m_outline;

	unsigned int m_normalFont;
	unsigned int m_boldFont;
	unsigned int m_italicFont;
	unsigned int m_outlineGUIFont;

	std::vector<Label> m_vLabelList;

	int m_numLines;

	VerticalAlignment m_verticalAlignment;
	HorizontalAlignment m_horizontalAlignment;

	bool m_wordWrap;

	int m_newLineSpacer;

	bool m_bold;
	bool m_italic;
};

#endif