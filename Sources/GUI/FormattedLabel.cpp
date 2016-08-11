/*************************************************************************
> File Name: FormattedLabel.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A formatted label component that can render formatted text strings.
> 	 Constructs multiple simple labels when the formatted text is parsed and
> 	 is in charge of managing and rendering these labels.
> Created Time: 2016/08/11
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "FormattedLabel.h"

// Constructor, Destructor
FormattedLabel::FormattedLabel(Renderer* pRenderer, unsigned int normalFont, unsigned int boldFont, unsigned int italicFont, const std::string& label) :
	Component(pRenderer), m_normalFont(normalFont), m_boldFont(boldFont), m_italicFont(italicFont)
{
	m_numLines = 0;

	m_verticalAlignment = VerticalAlignment::Top;
	m_horizontalAlignment = HorizontalAlignment::Left;

	m_wordWrap = false;

	m_newLineSpacer = 0;

	m_bold = false;
	m_italic = false;

	m_outline = false;

	// Set default dimensions
	FormattedLabel::SetDimensions(0, 0, 1, 1);

	SetText(label);
}

FormattedLabel::~FormattedLabel()
{
	// Clear the label list, since we still have labels present
	m_vLabelList.clear();
}

void FormattedLabel::SetColor(Color color)
{
	m_color = color;

	for (auto iter = m_vLabelList.begin(); iter != m_vLabelList.end(); ++iter)
	{
		Label* pLabel = &(*iter);

		// Only update the alpha
		float r; float g; float b; float a;
		pLabel->GetColor(&r, &g, &b, &a);
		Color newColor(r, g, b, color.GetAlpha());

		pLabel->SetColor(newColor);
	}
}

void FormattedLabel::SetOutlineColor(Color color)
{
	m_outlineColor = color;

	for (auto iter = m_vLabelList.begin(); iter != m_vLabelList.end(); ++iter)
	{
		Label *pLabel = &(*iter);

		// Only update the alpha
		float r; float g; float b; float a;
		pLabel->GetOutlineColor(&r, &g, &b, &a);
		Color newColor(r, g, b, color.GetAlpha());

		pLabel->SetOutlineColor(newColor);
	}
}

void FormattedLabel::SetOutlineFont(unsigned int outlineGUIFont)
{
	m_outlineGUIFont = outlineGUIFont;
}

void FormattedLabel::SetOutline(bool outline)
{
	m_outline = outline;
}

void FormattedLabel::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	// Make sure we update the text, after we have changed the formatted text dimensions
	UpdateText();
}

void FormattedLabel::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void FormattedLabel::SetVerticalAlignment(VerticalAlignment alignment)
{
	m_verticalAlignment = alignment;

	// Also update the text, since we have changed vertical alignment
	UpdateText();
}

VerticalAlignment FormattedLabel::GetVerticalAlignment() const
{
	return m_verticalAlignment;
}

void FormattedLabel::SetHorizontalAlignment(HorizontalAlignment alignment)
{
	m_horizontalAlignment = alignment;

	// Also update the text, since we have changed horizontal alignment
	UpdateText();
}

HorizontalAlignment FormattedLabel::GetHorizontalAlignment() const
{
	return m_horizontalAlignment;
}

void FormattedLabel::SetWordWrap(bool wrap)
{
	m_wordWrap = wrap;

	UpdateText();
}

bool FormattedLabel::IsWordWrapEnabled() const
{
	return m_wordWrap;
}

void FormattedLabel::SetNewLineSpacer(int spacer)
{
	m_newLineSpacer = spacer;
}

void FormattedLabel::SetText(const std::string& label)
{
	m_text = label;

	UpdateText();
}

const std::string& FormattedLabel::GetText() const
{
	return m_text;
}

bool FormattedLabel::DoesTextExist(const char* pText)
{
	for (auto iter = m_vLabelList.begin(); iter != m_vLabelList.end(); ++iter)
	{
		if (strcmp(pText, (*iter).GetText().c_str()) == 0)
		{
			// We found the text string
			return true;
		}
	}

	// We couldn't find the label we was looking for
	return false;
}

Label* FormattedLabel::GetLabel(const char* pText)
{
	for (auto iter = m_vLabelList.begin(); iter != m_vLabelList.end(); ++iter)
	{
		if (strcmp(pText, (*iter).GetText().c_str()) == 0)
		{
			// We found the text string, return the label
			return &(*iter);
		}
	}

	// We couldn't find the label we was looking for
	return nullptr;
}

int FormattedLabel::GetNumberOfLines() const
{
	return m_numLines;
}

int FormattedLabel::GetNumberOfLabels() const
{
	return static_cast<int>(m_vLabelList.size());
}

int FormattedLabel::GetMinimumTextHeight() const
{
	// We will need to know the height of the text
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_normalFont, "a");

	return m_numLines * textHeight;
}

void FormattedLabel::UpdateText()
{
	// Parse the text into separate label components that we will manage
	ParseText();

	// Re-align the label components
	ReAlignText();

	std::vector<Label>::iterator iterator;
	for (iterator = m_vLabelList.begin(); iterator != m_vLabelList.end(); ++iterator)
	{
		Label *pLabel = &(*iterator);

		pLabel->SetOutline(m_outline);
		//pLabel->SetColor(m_color);
		pLabel->SetOutlineColor(m_outlineColor);
		pLabel->SetOutlineFont(m_outlineGUIFont);
	}
}

void FormattedLabel::ReAlignText()
{
	assert(m_numLines < MAX_NUM_LINES); // Make sure we have enough space for all the lines

	unsigned int lCurrentFontID = m_normalFont;  // Start off with the normal font

												 // We will need to know the height of the text
	int lTextHeight = m_pRenderer->GetFreeTypeTextHeight(lCurrentFontID, "a");

	// Keep a track of the running width of each line of text
	int lLineTextWidth[MAX_NUM_LINES];
	for (int i = 0; i < MAX_NUM_LINES; i++)
	{
		lLineTextWidth[i] = 0;
	}

	// First we need to iterate through each line to find out the length text on each line. NOTE : There can be multiple labels on each line
	std::vector<Label>::iterator iterator;
	for (iterator = m_vLabelList.begin(); iterator != m_vLabelList.end(); ++iterator)
	{
		int lTextYPosition = m_dimensions.height - (*iterator).GetDimensions().y;
		int lTextLineIndex = (lTextYPosition / lTextHeight) - 1;

		lLineTextWidth[lTextLineIndex] += m_pRenderer->GetFreeTypeTextWidth((*iterator).GetFontID(), "%s", (*iterator).GetText().c_str());
	}

	// Now we can work out the X and Y modifications to apply to each label component depending on what alignment we have...

	int XModification = 0;
	int YModification = 0;

	// Vertical alignment is easy to calculate and we only need to know how many lines we have in total to work out the Y modification
	int lBottomTextY = m_dimensions.height - (m_numLines * lTextHeight);
	if (m_verticalAlignment == VerticalAlignment::Top)
	{
		YModification = 0;
	}
	else if (m_verticalAlignment == VerticalAlignment::Center)
	{
		YModification = lBottomTextY / 2;
	}
	else if (m_verticalAlignment == VerticalAlignment::Bottom)
	{
		YModification = lBottomTextY;
	}


	for (iterator = m_vLabelList.begin(); iterator != m_vLabelList.end(); ++iterator)
	{
		int lTextYPosition = m_dimensions.height - (*iterator).GetDimensions().y;
		int lTextLineIndex = (lTextYPosition / lTextHeight) - 1;

		// X modification needs to be calculated for each label individually, depending on what line the label is present
		if (m_horizontalAlignment == HorizontalAlignment::Left)
		{
			XModification = 0;
		}
		else if (m_horizontalAlignment == HorizontalAlignment::Center)
		{
			XModification = (m_dimensions.width - lLineTextWidth[lTextLineIndex]) / 2;
		}
		else if (m_horizontalAlignment == HorizontalAlignment::Right)
		{
			XModification = m_dimensions.width - lLineTextWidth[lTextLineIndex];
		}

		// Apply the position modifications to the label component
		int lX = (*iterator).GetLocation().x + XModification;
		int lY = (*iterator).GetLocation().y - YModification;

		(*iterator).SetLocation(lX, lY);
	}
}

void FormattedLabel::ParseText()
{
	// First clear out any labels we already have
	ClearLabels();

	if (strcmp(m_text.c_str(), "") == 0)
	{
		// If we have no text then set some default values and early out
		m_numLines = 0;

		return;
	}

	m_numLines = 1;  // We have at least 1 line

	unsigned int lCurrentFontID = m_normalFont;  // Start off with the normal font

												 // Keep track of the text height, needed when we create new lines of text
	int lTextHeight = m_pRenderer->GetFreeTypeTextHeight(lCurrentFontID, "a");

	// Next go though every character in the text
	int lCharIndex = 0;
	int lStartLineIndex = 0;
	int lLastGoodSpaceIndex = 0;
	bool lbFoundGoodSpace = false;
	const char *lpString = m_text.c_str();

	// Our current label position
	int lCurrentTextX = 0;
	int lCurrentTextY = m_dimensions.height - lTextHeight;

	// Current label colour
	Color lCurrentColor = m_color;

	while (lpString[lCharIndex] != 0)
	{
		char lpChar = lpString[lCharIndex];

		// Word wrap
		if (m_wordWrap)
		{
			// Check for spaces
			if (lpChar == ' ')
			{
				std::string lString(m_text.substr(lStartLineIndex, lCharIndex - lStartLineIndex));

				int lTextLineWidth = m_pRenderer->GetFreeTypeTextWidth(lCurrentFontID, "%s", lString.c_str());

				// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
				if (lCurrentTextX + lTextLineWidth > m_dimensions.width)
				{
					std::string lString;

					if (lbFoundGoodSpace)
					{
						// We need to create a new line now, but only include up to the last good space in the previous label that we are making
						lString = std::string(m_text.substr(lStartLineIndex, lLastGoodSpaceIndex - lStartLineIndex));
					}
					else
					{
						// We haven't been able to find a good space, that means that that a whole word is longer than the width of the box
						lString = std::string(m_text.substr(lStartLineIndex, lCharIndex - lStartLineIndex));
					}

					Label lNewLabel(m_pRenderer, lCurrentFontID, lString, lCurrentColor);
					int lTextWidth = m_pRenderer->GetFreeTypeTextWidth(lCurrentFontID, "%s", lString.c_str());
					lNewLabel.SetLocation(lCurrentTextX, lCurrentTextY);
					m_vLabelList.push_back(lNewLabel);

					// Modify our Y position, since we are moving down to a new line
					lCurrentTextY -= lTextHeight;

					if (lbFoundGoodSpace)
					{
						lCharIndex = lLastGoodSpaceIndex;
					}

					// Skip over the space, we don't want to start the new line with a leading space
					lStartLineIndex = lCharIndex + 1;

					// Reset the current text X position, since we have created a new line
					lCurrentTextX = 0;

					// Increase the line number count
					m_numLines++;

					// Reset the good space flag, since we have created a new line
					lbFoundGoodSpace = false;
				}
				else
				{
					lbFoundGoodSpace = true;

					lLastGoodSpaceIndex = lCharIndex;
				}
			}

			// Check for new lines, being at the end of a word-wrapped line, this usually means the last word doesnt get wrapped properly
			if (lpChar == '\n')
			{
				std::string lString(m_text.substr(lStartLineIndex, lCharIndex - lStartLineIndex));

				int lTextLineWidth = m_pRenderer->GetFreeTypeTextWidth(lCurrentFontID, "%s", lString.c_str());

				// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
				if (lCurrentTextX + lTextLineWidth > m_dimensions.width)
				{
					std::string lString(m_text.substr(lStartLineIndex, lLastGoodSpaceIndex - lStartLineIndex));

					Label lNewLabel(m_pRenderer, lCurrentFontID, lString, lCurrentColor);
					int lTextWidth = m_pRenderer->GetFreeTypeTextWidth(lCurrentFontID, "%s", lString.c_str());
					lNewLabel.SetLocation(lCurrentTextX, lCurrentTextY);
					m_vLabelList.push_back(lNewLabel);

					// Modify our Y position, since we are moving down to a new line
					lCurrentTextY -= lTextHeight;

					// Skip over the new line, else we will detect it on the next loop t
					lStartLineIndex = lLastGoodSpaceIndex + 1;

					// Reset the current text X position, since we have created a new line
					lCurrentTextX = 0;

					// Increase the line number count
					m_numLines++;

					// Reset the good space flag, since we have created a new line
					lbFoundGoodSpace = false;
				}
			}

			// Check for the end of the string, need to make sure the final word gets wrapped, if it is needed
			char lpFinalChar = lpString[lCharIndex + 1];
			if (lpFinalChar == 0)
			{
				std::string lString(m_text.substr(lStartLineIndex, lCharIndex - lStartLineIndex));

				int lTextLineWidth = m_pRenderer->GetFreeTypeTextWidth(lCurrentFontID, "%s", lString.c_str());

				// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
				if (lCurrentTextX + lTextLineWidth > m_dimensions.width)
				{
					std::string lString(m_text.substr(lStartLineIndex, lLastGoodSpaceIndex - lStartLineIndex));

					Label lNewLabel(m_pRenderer, lCurrentFontID, lString, lCurrentColor);
					int lTextWidth = m_pRenderer->GetFreeTypeTextWidth(lCurrentFontID, "%s", lString.c_str());
					lNewLabel.SetLocation(lCurrentTextX, lCurrentTextY);
					m_vLabelList.push_back(lNewLabel);

					// Modify our Y position, since we are moving down to a new line
					lCurrentTextY -= lTextHeight;

					// Skip over the new line, else we will detect it on the next loop t
					lStartLineIndex = lLastGoodSpaceIndex + 1;

					// Reset the current text X position, since we have created a new line
					lCurrentTextX = 0;

					// Increase the line number count
					m_numLines++;

					// Reset the good space flag, since we have created a new line
					lbFoundGoodSpace = false;
				}

				lString = string(m_text.substr(lStartLineIndex, (lCharIndex + 1) - lStartLineIndex));

				// Create the final label component
				Label lNewLabel(m_pRenderer, lCurrentFontID, lString, lCurrentColor);

				// Set the final label position
				int lTextWidth = m_pRenderer->GetFreeTypeTextWidth(lCurrentFontID, "%s", lString.c_str());

				lNewLabel.SetLocation(lCurrentTextX, lCurrentTextY);

				// Add the final label to the list
				m_vLabelList.push_back(lNewLabel);
			}
		}


		// Checking for new lines
		if (lpChar == '\n')
		{
			// We have encountered a new line...

			// Create the substring of the text for our new label component
			std::string lString(m_text.substr(lStartLineIndex, lCharIndex - lStartLineIndex));

			// Create the new label component using the new string we have just generated
			Label lNewLabel(m_pRenderer, lCurrentFontID, lString, lCurrentColor);

			// Set the location of this new label component
			int lTextWidth = m_pRenderer->GetFreeTypeTextWidth(lCurrentFontID, "%s", lString.c_str());
			lNewLabel.SetLocation(lCurrentTextX, lCurrentTextY);

			// Add the new label to the label list
			m_vLabelList.push_back(lNewLabel);

			// Set the new starting index, REMEMBERING to skip over the new line char
			lStartLineIndex = lCharIndex + 1;

			// Modify our Y position, since we are moving down to a new line
			lCurrentTextY -= lTextHeight;

			// Reset the current text X position, since we have created a new line
			lCurrentTextX = 0;

			// Increase the line number count
			m_numLines++;

			// Reset the good space flag, since we have created a new line
			lbFoundGoodSpace = false;
		}


		// TODO : Get text modification tags working with word wrap properly!


		// Checking for text modifying tags
		if (lpChar == '[')
		{
			// If we have found a tag, then we need to bundle up the previous string and make a new label for it
			// Since we will be modifying the current params of all the new labels created from here on...

			std::string lString(m_text.substr(lStartLineIndex, lCharIndex - lStartLineIndex));
			Label lNewLabel(m_pRenderer, lCurrentFontID, lString, lCurrentColor);
			int lTextWidth = m_pRenderer->GetFreeTypeTextWidth(lCurrentFontID, "%s", lString.c_str());
			lNewLabel.SetLocation(lCurrentTextX, lCurrentTextY);
			m_vLabelList.push_back(lNewLabel);

			// Set the new starting index, REMEMBERING to skip over the new line char
			lStartLineIndex = lCharIndex + 1;

			// Reset the good space flag, since we have created a new line
			lbFoundGoodSpace = false;

			int lNumCharsToSkip = 0;

			// Color tag
			if (lpString[lCharIndex + 1] == 'C')
			{
				// Get the name of the colour to change to

				int lColorStringStart = lCharIndex + 3;  // Because of syntax e.g [C=??]

				if (strncmp(&lpString[lColorStringStart], "Red", 3) == 0)
				{
					lCurrentColor = Color(1.0f, 0.0f, 0.0f);

					lNumCharsToSkip = 7; //[C=Red]
				}
				else if (strncmp(&lpString[lColorStringStart], "Blue", 4) == 0)
				{
					lCurrentColor = Color(0.1f, 0.2f, 0.95f);

					lNumCharsToSkip = 8; //[C=Blue]
				}
				else if (strncmp(&lpString[lColorStringStart], "Green", 5) == 0)
				{
					lCurrentColor = Color(0.0f, 1.0f, 0.0f);

					lNumCharsToSkip = 9; //[C=Green]
				}
				else if (strncmp(&lpString[lColorStringStart], "White", 5) == 0)
				{
					lCurrentColor = Color(1.0f, 1.0f, 1.0f);

					lNumCharsToSkip = 9; //[C=White]
				}
				else if (strncmp(&lpString[lColorStringStart], "Black", 5) == 0)
				{
					lCurrentColor = Color(0.0f, 0.0f, 0.0f);

					lNumCharsToSkip = 9; //[C=Black]
				}
				else if (strncmp(&lpString[lColorStringStart], "Yellow", 6) == 0)
				{
					lCurrentColor = Color(1.0f, 1.0f, 0.0f);

					lNumCharsToSkip = 10; //[C=Yellow]
				}
				else if (strncmp(&lpString[lColorStringStart], "Custom", 6) == 0)
				{
					// Custom colours follow the syntax [C=Custom(FFFFFF)]

					// Read in the hex value
					int lHexCharIndex = lCharIndex + 10;
					char lRed[3] = { lpString[lHexCharIndex], lpString[lHexCharIndex + 1], 0 };
					char lGreen[3] = { lpString[lHexCharIndex + 2], lpString[lHexCharIndex + 3], 0 };
					char lBlue[3] = { lpString[lHexCharIndex + 4], lpString[lHexCharIndex + 5], 0 };

					int lRedi;
					int lGreeni;
					int lBluei;

					sscanf(lRed, "%x", &lRedi);
					sscanf(lGreen, "%x", &lGreeni);
					sscanf(lBlue, "%x", &lBluei);

					float lRedValue = lRedi / 255.0f;
					float lGreenValue = lGreeni / 255.0f;
					float lBlueValue = lBluei / 255.0f;

					lCurrentColor = Color(lRedValue, lGreenValue, lBlueValue);

					lNumCharsToSkip = 18; //[C=Custom(FFFFFF)]
				}
			}


			// TODO : Space tag!
			// e.g. [S=050] leaves space for 50 pixels


			// TODO : Vertical spacing between new lines - allow for custom space passed as a parameter for vertical spacer


			// Bold tag
			if (lpString[lCharIndex + 1] == 'B')
			{
				lNumCharsToSkip = 3;  // Because of syntax e.g [B]

				m_bold = true;
				m_italic = false;

				lCurrentFontID = m_boldFont;  // Set the bold font
			}

			// Italic tag
			if (lpString[lCharIndex + 1] == 'I')
			{
				lNumCharsToSkip = 3;  // Because of syntax e.g [I]

				m_italic = true;
				m_bold = false;

				lCurrentFontID = m_italicFont;  // Set the italic font
			}

			// Normal tag
			if (lpString[lCharIndex + 1] == 'N')
			{
				lNumCharsToSkip = 3;  // Because of syntax e.g [N]

				m_bold = false;
				m_italic = false;

				lCurrentFontID = m_normalFont;  // Set the normal font
			}

			lStartLineIndex = lCharIndex + lNumCharsToSkip;  // Skip over the tag parts

			lCurrentTextX += lTextWidth;  // Make sure we increase the starting X position, by the size of the text label we have just created
		}

		lCharIndex++;
	}

	// NOTE : Why do we need to create the final label here? It seems that it is already created above when lpChar = '0'
	// Create the final string
	//std::string lString(m_text.substr(lStartLineIndex, lCharIndex - lStartLineIndex));

	// Create the final label component
	//Label lNewLabel(m_pRenderer, lCurrentFontID, lString, lCurrentColor);

	// Set the final label position
	//int lTextWidth = m_pRenderer->GetFreeTypeTextWidth(lCurrentFontID, "%s", lString.c_str());

	//lNewLabel.SetLocation(lCurrentTextX, lCurrentTextY);

	// Add the final label to the list
	//m_vLabelList.push_back(lNewLabel);
}

void FormattedLabel::ClearLabels()
{
	m_vLabelList.clear();
}

void FormattedLabel::DrawSelf()
{
	// Render all the normal label components that we contain
	std::vector<Label>::iterator iterator;
	for (iterator = m_vLabelList.begin(); iterator != m_vLabelList.end(); ++iterator)
	{
		(*iterator).Draw();
	}

	/*
	// DEBUG : Draw our bounds
	int l_outlineX1 = 0;
	int l_outlineX2 = m_dimensions.width;
	int l_outlineY1 = 0;
	int l_outlineY2 = m_dimensions.height;

	int l_depth = static_cast<int>(GetDepth()) + 2.0f;

	m_pRenderer->PushMatrix();
	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->EnableImmediateMode(IM_LINE_LOOP);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(l_outlineX1, l_outlineY1, l_depth);
	m_pRenderer->ImmediateVertex(l_outlineX2, l_outlineY1, l_depth);
	m_pRenderer->ImmediateVertex(l_outlineX2, l_outlineY2, l_depth);
	m_pRenderer->ImmediateVertex(l_outlineX1, l_outlineY2, l_depth);
	m_pRenderer->DisableImmediateMode();
	m_pRenderer->PopMatrix();

	// DEBUG : Text bounds checking
	int l_stringWidth = m_pRenderer->GetFreeTypeTextWidth(m_normalFont, "%s", m_text.c_str());
	int l_stringHeight = m_pRenderer->GetFreeTypeTextHeight(m_normalFont, "%s", m_text.c_str());

	l_outlineX1 = 0;
	l_outlineX2 = l_stringWidth;
	l_outlineY1 = 0;
	l_outlineY2 = l_stringHeight;

	m_pRenderer->PushMatrix();
	m_pRenderer->SetRenderMode(RM_WIREFRAME);
	m_pRenderer->EnableImmediateMode(IM_QUADS);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(l_outlineX1, l_outlineY1, (int)m_depth);
	m_pRenderer->ImmediateVertex(l_outlineX2, l_outlineY1, (int)m_depth);
	m_pRenderer->ImmediateVertex(l_outlineX2, l_outlineY2, (int)m_depth);
	m_pRenderer->ImmediateVertex(l_outlineX1, l_outlineY2, (int)m_depth);
	m_pRenderer->DisableImmediateMode();
	m_pRenderer->PopMatrix();
	*/
}