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

	for (auto iter = m_vLabelList.begin(); iter != m_vLabelList.end(); ++iter)
	{
		Label* pLabel = &(*iter);

		pLabel->SetOutline(m_outline);
		//pLabel->SetColor(m_color);
		pLabel->SetOutlineColor(m_outlineColor);
		pLabel->SetOutlineFont(m_outlineGUIFont);
	}
}

void FormattedLabel::ReAlignText()
{
	// Make sure we have enough space for all the lines
	assert(m_numLines < MAX_NUM_LINES);

	// Start off with the normal font
	unsigned int currentFontID = m_normalFont;  

	// We will need to know the height of the text												 
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(currentFontID, "a");

	// Keep a track of the running width of each line of text
	int lineTextWidth[MAX_NUM_LINES];
	for (int i = 0; i < MAX_NUM_LINES; ++i)
	{
		lineTextWidth[i] = 0;
	}

	// First we need to iterate through each line to find out the length text on each line. NOTE : There can be multiple labels on each line
	for (auto iter = m_vLabelList.begin(); iter != m_vLabelList.end(); ++iter)
	{
		int textYPosition = m_dimensions.height - (*iter).GetDimensions().y;
		int textLineIndex = (textYPosition / textHeight) - 1;

		lineTextWidth[textLineIndex] += m_pRenderer->GetFreeTypeTextWidth((*iter).GetFontID(), "%s", (*iter).GetText().c_str());
	}

	// Now we can work out the X and Y modifications to apply to each label component depending on what alignment we have...
	int xModification = 0;
	int yModification = 0;

	// Vertical alignment is easy to calculate and we only need to know how many lines we have in total to work out the Y modification
	int bottomTextY = m_dimensions.height - (m_numLines * textHeight);
	if (m_verticalAlignment == VerticalAlignment::Top)
	{
		yModification = 0;
	}
	else if (m_verticalAlignment == VerticalAlignment::Center)
	{
		yModification = bottomTextY / 2;
	}
	else if (m_verticalAlignment == VerticalAlignment::Bottom)
	{
		yModification = bottomTextY;
	}

	for (auto iter = m_vLabelList.begin(); iter != m_vLabelList.end(); ++iter)
	{
		int textYPosition = m_dimensions.height - (*iter).GetDimensions().y;
		int textLineIndex = (textYPosition / textHeight) - 1;

		// X modification needs to be calculated for each label individually, depending on what line the label is present
		if (m_horizontalAlignment == HorizontalAlignment::Left)
		{
			xModification = 0;
		}
		else if (m_horizontalAlignment == HorizontalAlignment::Center)
		{
			xModification = (m_dimensions.width - lineTextWidth[textLineIndex]) / 2;
		}
		else if (m_horizontalAlignment == HorizontalAlignment::Right)
		{
			xModification = m_dimensions.width - lineTextWidth[textLineIndex];
		}

		// Apply the position modifications to the label component
		int x = (*iter).GetLocation().x + xModification;
		int y = (*iter).GetLocation().y - yModification;

		(*iter).SetLocation(x, y);
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

	// Start off with the normal font
	unsigned int currentFontID = m_normalFont;  

	// Keep track of the text height, needed when we create new lines of text
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(currentFontID, "a");

	// Next go though every character in the text
	int charIndex = 0;
	int startLineIndex = 0;
	int lastGoodSpaceIndex = 0;
	bool isFoundGoodSpace = false;
	const char* pString = m_text.c_str();

	// Our current label position
	int currentTextX = 0;
	int currentTextY = m_dimensions.height - textHeight;

	// Current label color
	Color currentColor = m_color;

	while (pString[charIndex] != 0)
	{
		char pChar = pString[charIndex];

		// Word wrap
		if (m_wordWrap)
		{
			// Check for spaces
			if (pChar == ' ')
			{
				std::string tempString(m_text.substr(startLineIndex, charIndex - startLineIndex));
				int textLineWidth = m_pRenderer->GetFreeTypeTextWidth(currentFontID, "%s", tempString.c_str());

				// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
				if (currentTextX + textLineWidth > m_dimensions.width)
				{
					std::string textString;

					if (isFoundGoodSpace)
					{
						// We need to create a new line now, but only include up to the last good space in the previous label that we are making
						textString = std::string(m_text.substr(startLineIndex, lastGoodSpaceIndex - startLineIndex));
					}
					else
					{
						// We haven't been able to find a good space, that means that that a whole word is longer than the width of the box
						textString = std::string(m_text.substr(startLineIndex, charIndex - startLineIndex));
					}

					Label newLabel(m_pRenderer, currentFontID, textString, currentColor);
					newLabel.SetLocation(currentTextX, currentTextY);
					m_vLabelList.push_back(newLabel);

					// Modify our Y position, since we are moving down to a new line
					currentTextY -= textHeight;

					if (isFoundGoodSpace)
					{
						charIndex = lastGoodSpaceIndex;
					}

					// Skip over the space, we don't want to start the new line with a leading space
					startLineIndex = charIndex + 1;

					// Reset the current text X position, since we have created a new line
					currentTextX = 0;

					// Increase the line number count
					m_numLines++;

					// Reset the good space flag, since we have created a new line
					isFoundGoodSpace = false;
				}
				else
				{
					isFoundGoodSpace = true;

					lastGoodSpaceIndex = charIndex;
				}
			}

			// Check for new lines, being at the end of a word-wrapped line, this usually means the last word doesn't get wrapped properly
			if (pChar == '\n')
			{
				std::string tempString(m_text.substr(startLineIndex, charIndex - startLineIndex));
				int textLineWidth = m_pRenderer->GetFreeTypeTextWidth(currentFontID, "%s", tempString.c_str());

				// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
				if (currentTextX + textLineWidth > m_dimensions.width)
				{
					std::string textString(m_text.substr(startLineIndex, lastGoodSpaceIndex - startLineIndex));

					Label newLabel(m_pRenderer, currentFontID, textString, currentColor);
					newLabel.SetLocation(currentTextX, currentTextY);
					m_vLabelList.push_back(newLabel);

					// Modify our Y position, since we are moving down to a new line
					currentTextY -= textHeight;

					// Skip over the new line, else we will detect it on the next loop t
					startLineIndex = lastGoodSpaceIndex + 1;

					// Reset the current text X position, since we have created a new line
					currentTextX = 0;

					// Increase the line number count
					m_numLines++;

					// Reset the good space flag, since we have created a new line
					isFoundGoodSpace = false;
				}
			}

			// Check for the end of the string, need to make sure the final word gets wrapped, if it is needed
			char finalChar = pString[charIndex + 1];
			if (finalChar == 0)
			{
				std::string tempString(m_text.substr(startLineIndex, charIndex - startLineIndex));
				int textLineWidth = m_pRenderer->GetFreeTypeTextWidth(currentFontID, "%s", tempString.c_str());

				// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
				if (currentTextX + textLineWidth > m_dimensions.width)
				{
					std::string textString(m_text.substr(startLineIndex, lastGoodSpaceIndex - startLineIndex));

					Label newLabel(m_pRenderer, currentFontID, textString, currentColor);
					newLabel.SetLocation(currentTextX, currentTextY);
					m_vLabelList.push_back(newLabel);

					// Modify our Y position, since we are moving down to a new line
					currentTextY -= textHeight;

					// Skip over the new line, else we will detect it on the next loop t
					startLineIndex = lastGoodSpaceIndex + 1;

					// Reset the current text X position, since we have created a new line
					currentTextX = 0;

					// Increase the line number count
					m_numLines++;

					// Reset the good space flag, since we have created a new line
					isFoundGoodSpace = false;
				}

				tempString = std::string(m_text.substr(startLineIndex, (charIndex + 1) - startLineIndex));

				// Create the final label component
				Label newLabel(m_pRenderer, currentFontID, tempString, currentColor);

				newLabel.SetLocation(currentTextX, currentTextY);

				// Add the final label to the list
				m_vLabelList.push_back(newLabel);
			}
		}

		// Checking for new lines
		if (pChar == '\n')
		{
			// We have encountered a new line...

			// Create the substring of the text for our new label component
			std::string textString(m_text.substr(startLineIndex, charIndex - startLineIndex));

			// Create the new label component using the new string we have just generated
			Label newLabel(m_pRenderer, currentFontID, textString, currentColor);

			// Set the location of this new label component
			newLabel.SetLocation(currentTextX, currentTextY);

			// Add the new label to the label list
			m_vLabelList.push_back(newLabel);

			// Set the new starting index, REMEMBERING to skip over the new line char
			startLineIndex = charIndex + 1;

			// Modify our Y position, since we are moving down to a new line
			currentTextY -= textHeight;

			// Reset the current text X position, since we have created a new line
			currentTextX = 0;

			// Increase the line number count
			m_numLines++;

			// Reset the good space flag, since we have created a new line
			isFoundGoodSpace = false;
		}

		// TODO: Get text modification tags working with word wrap properly!

		// Checking for text modifying tags
		if (pChar == '[')
		{
			// If we have found a tag, then we need to bundle up the previous string and make a new label for it
			// Since we will be modifying the current params of all the new labels created from here on...
			std::string textString(m_text.substr(startLineIndex, charIndex - startLineIndex));
			Label newLabel(m_pRenderer, currentFontID, textString, currentColor);
			int textWidth = m_pRenderer->GetFreeTypeTextWidth(currentFontID, "%s", textString.c_str());
			newLabel.SetLocation(currentTextX, currentTextY);
			m_vLabelList.push_back(newLabel);

			// Reset the good space flag, since we have created a new line
			isFoundGoodSpace = false;

			int numCharsToSkip = 0;

			// Color tag
			if (pString[charIndex + 1] == 'C')
			{
				// Get the name of the color to change to

				int colorStringStart = charIndex + 3;  // Because of syntax e.g [C=??]

				if (strncmp(&pString[colorStringStart], "Red", 3) == 0)
				{
					currentColor = Color(1.0f, 0.0f, 0.0f);
					numCharsToSkip = 7; // [C=Red]
				}
				else if (strncmp(&pString[colorStringStart], "Blue", 4) == 0)
				{
					currentColor = Color(0.1f, 0.2f, 0.95f);
					numCharsToSkip = 8; // [C=Blue]
				}
				else if (strncmp(&pString[colorStringStart], "Green", 5) == 0)
				{
					currentColor = Color(0.0f, 1.0f, 0.0f);
					numCharsToSkip = 9; // [C=Green]
				}
				else if (strncmp(&pString[colorStringStart], "White", 5) == 0)
				{
					currentColor = Color(1.0f, 1.0f, 1.0f);
					numCharsToSkip = 9; // [C=White]
				}
				else if (strncmp(&pString[colorStringStart], "Black", 5) == 0)
				{
					currentColor = Color(0.0f, 0.0f, 0.0f);
					numCharsToSkip = 9; // [C=Black]
				}
				else if (strncmp(&pString[colorStringStart], "Yellow", 6) == 0)
				{
					currentColor = Color(1.0f, 1.0f, 0.0f);
					numCharsToSkip = 10; // [C=Yellow]
				}
				else if (strncmp(&pString[colorStringStart], "Custom", 6) == 0)
				{
					// Custom colors follow the syntax [C=Custom(FFFFFF)]

					// Read in the hex value
					int hexCharIndex = charIndex + 10;
					char red[3] = { pString[hexCharIndex], pString[hexCharIndex + 1], 0 };
					char green[3] = { pString[hexCharIndex + 2], pString[hexCharIndex + 3], 0 };
					char nlue[3] = { pString[hexCharIndex + 4], pString[hexCharIndex + 5], 0 };

					int redInt;
					int greenInt;
					int blueint;

					sscanf(red, "%x", &redInt);
					sscanf(green, "%x", &greenInt);
					sscanf(nlue, "%x", &blueint);

					float redValue = redInt / 255.0f;
					float greenValue = greenInt / 255.0f;
					float blueValue = blueint / 255.0f;

					currentColor = Color(redValue, greenValue, blueValue);

					numCharsToSkip = 18; // [C=Custom(FFFFFF)]
				}
			}

			// TODO: Space tag!
			// e.g. [S=050] leaves space for 50 pixels

			// TODO: Vertical spacing between new lines - allow for custom space passed as a parameter for vertical spacer

			// Bold tag
			if (pString[charIndex + 1] == 'B')
			{
				numCharsToSkip = 3;  // Because of syntax e.g [B]

				m_bold = true;
				m_italic = false;

				currentFontID = m_boldFont;  // Set the bold font
			}

			// Italic tag
			if (pString[charIndex + 1] == 'I')
			{
				numCharsToSkip = 3;  // Because of syntax e.g [I]

				m_italic = true;
				m_bold = false;

				currentFontID = m_italicFont;  // Set the italic font
			}

			// Normal tag
			if (pString[charIndex + 1] == 'N')
			{
				numCharsToSkip = 3;  // Because of syntax e.g [N]

				m_bold = false;
				m_italic = false;

				currentFontID = m_normalFont;  // Set the normal font
			}

			startLineIndex = charIndex + numCharsToSkip;  // Skip over the tag parts

			currentTextX += textWidth;  // Make sure we increase the starting X position, by the size of the text label we have just created
		}

		charIndex++;
	}

	// NOTE: Why do we need to create the final label here? It seems that it is already created above when lpChar = '0'
	// Create the final string
	//std::string textString(m_text.substr(startLineIndex, charIndex - startLineIndex));

	// Create the final label component
	//Label lNewLabel(m_pRenderer, currentFontID, textString, currentColor);

	// Set the final label position
	//int textWidth = m_pRenderer->GetFreeTypeTextWidth(currentFontID, "%s", textString.c_str());

	//newLabel.SetLocation(currentTextX, currentTextY);

	// Add the final label to the list
	//m_vLabelList.push_back(newLabel);
}

void FormattedLabel::ClearLabels()
{
	m_vLabelList.clear();
}

void FormattedLabel::DrawSelf()
{
	// Render all the normal label components that we contain
	for (auto iter = m_vLabelList.begin(); iter != m_vLabelList.end(); ++iter)
	{
		(*iter).Draw();
	}

	/*
	// DEBUG: Draw our bounds
	int outlineX1 = 0;
	int outlineX2 = m_dimensions.width;
	int outlineY1 = 0;
	int outlineY2 = m_dimensions.height;

	int depth = static_cast<int>(GetDepth()) + 2.0f;

	m_pRenderer->PushMatrix();

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINE_LOOP);

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);
	m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);
	m_pRenderer->ImmediateVertex(outlineX2, outlineY2, depth);
	m_pRenderer->ImmediateVertex(outlineX1, outlineY2, depth);

	m_pRenderer->DisableImmediateMode();

	m_pRenderer->PopMatrix();

	// DEBUG: Text bounds checking
	int stringWidth = m_pRenderer->GetFreeTypeTextWidth(m_normalFont, "%s", m_text.c_str());
	int stringHeight = m_pRenderer->GetFreeTypeTextHeight(m_normalFont, "%s", m_text.c_str());

	outlineX1 = 0;
	outlineX2 = stringWidth;
	outlineY1 = 0;
	outlineY2 = stringHeight;

	m_pRenderer->PushMatrix();

	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(outlineX1, outlineY1, static_cast<int>(m_depth));
	m_pRenderer->ImmediateVertex(outlineX2, outlineY1, static_cast<int>(m_depth));
	m_pRenderer->ImmediateVertex(outlineX2, outlineY2, static_cast<int>(m_depth));
	m_pRenderer->ImmediateVertex(outlineX1, outlineY2, static_cast<int>(m_depth));

	m_pRenderer->DisableImmediateMode();

	m_pRenderer->PopMatrix();
	*/
}