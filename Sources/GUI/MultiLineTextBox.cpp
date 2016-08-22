/*************************************************************************
> File Name: MultiLineTextBox.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A multi-line text entry box that accepts focus events and key events.
> Created Time: 2016/08/22
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Utils/TimeManager.h>

#include "DirectDrawRectangle.h"
#include "MultiLineTextBox.h"
#include "MultiTextureIcon.h"

// Constructor, Destructor
MultiLineTextBox::MultiLineTextBox(Renderer* pRenderer, unsigned int GUIFont, const std::string& defaultText) :
	Container(pRenderer), m_GUIFont(GUIFont), m_scrollBar(pRenderer)
{
	m_pBackgroundIcon = new DirectDrawRectangle(pRenderer);
	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pBackgroundIcon);

	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.7f, 0.7f, 0.85f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.7f, 0.7f, 0.85f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.7f, 0.7f, 0.85f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.7f, 0.7f, 0.85f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	Container::Add(m_pBackgroundIcon);
	m_pBackgroundIcon->SetDepth(1.0f);

	m_scrollBar.SetScrollDirection(ScrollBarDirection::Vertical);
	m_scrollBar.SetScrollPosition(0.75f);

	m_scrollBar.SetValueChangedCallback(_ScrollbarChanged);
	m_scrollBar.SetValueChangedCallBackData(this);

	Container::Add(&m_scrollBar);

	//m_text.SetLocation(0, 0);
	//m_text.SetDepth(2.0f);
	//Add(&m_text);

	m_pipeYOffset = 0;

	SetText(defaultText);

	UpdateText();

	m_highlightStartIndex = 0;
	if (m_labelList.size() >= 1)
	{
		m_pipeCharacterIndex = static_cast<int>(m_text.size());
	}

	m_highlightEndIndex = 0;

	m_pipeCharacterIndex = 0;
	m_displayPipe = false;

	m_newLinePipeRender = false;

	m_pipeHeight = 0;
	m_textIndent = 0;
	m_maxTextLength = 0;

	m_shiftHeld = false;

	m_hiddenText = false;
	m_hideCharacter = '*';

	m_callbackOnReturnPressed = nullptr;

	m_numLines = 0;

	m_verticalAlignment = VerticalAlignment::Top;
	m_horizontalAlignment = HorizontalAlignment::Left;

	m_pPipeDisplayCountDown = new CountdownTimer();
	m_pPipeDisplayCountDown->SetCountdownTime(0.5f);
	m_pPipeDisplayCountDown->SetLooping(true);
	m_pPipeDisplayCountDown->SetCallBackFunction(_TogglePipeDisplay);
	m_pPipeDisplayCountDown->SetCallBackData(this);
	m_pPipeDisplayCountDown->StartCountdown();
}

MultiLineTextBox::~MultiLineTextBox()
{
	TimeManager::GetInstance()->RemoveCountdownTimer(m_pPipeDisplayCountDown);

	delete m_pBackgroundIcon;

	// Clear the label list, since we still have labels present
	m_labelList.clear();
}

void MultiLineTextBox::AddEventListeners()
{
	Container::AddEventListeners();

	AddMouseListener(this);
	AddFocusListener(this);
	AddKeyListener(this);
}

void MultiLineTextBox::RemoveEventListeners()
{
	Container::RemoveEventListeners();

	RemoveMouseListener(this);
	RemoveFocusListener(this);
	RemoveKeyListener(this);
}

void MultiLineTextBox::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	// Also set the dimensions of the icons
	m_pBackgroundIcon->SetDimensions(0, 0, width, height);

	m_scrollBar.SetDimensions(width, 0, 20, height);
}

void MultiLineTextBox::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

// ReSharper disable CppMemberFunctionMayBeStatic
void MultiLineTextBox::SetTextColor(Color color) const
{
	//m_text.SetColor(color);
}

void MultiLineTextBox::SetBackgroundIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pBackgroundIcon)
	{
		// If we already own a background icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pBackgroundIcon);

		delete m_pBackgroundIcon;
		m_pBackgroundIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then create a new copy of it
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pBackgroundIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pBackgroundIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pBackgroundIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pBackgroundIcon);

	m_pBackgroundIcon->SetDepth(1.0f);

	DepthSortComponentChildren();
}

void MultiLineTextBox::SetPipHeight(int height)
{
	m_pipeHeight = height;
}

void MultiLineTextBox::SetTextIndent(int indent)
{
	m_textIndent = indent;
}

void MultiLineTextBox::SetMaxTextLength(int length)
{
	m_maxTextLength = length;
}

void MultiLineTextBox::SetPipeColor(Color pipeColor)
{
	m_pipeColor = pipeColor;
}

void MultiLineTextBox::SetText(const std::string& label)
{
	m_text = label;

	UpdateText();

	if (m_labelList.size() >= 1)
	{
		m_pipeCharacterIndex = static_cast<int>(m_text.size());
	}
}

const std::string& MultiLineTextBox::GetText() const
{
	return m_text;
}

void MultiLineTextBox::SetVerticalAlignment(VerticalAlignment alignment)
{
	m_verticalAlignment = alignment;

	// Also update the text, since we have changed vertical alignment
	UpdateText();
}

VerticalAlignment MultiLineTextBox::GetVerticalAlignment() const
{
	return m_verticalAlignment;
}

void MultiLineTextBox::SetHorizontalAlignment(HorizontalAlignment alignment)
{
	m_horizontalAlignment = alignment;

	// Also update the text, since we have changed horizontal alignment
	UpdateText();
}

HorizontalAlignment MultiLineTextBox::GetHorizontalAlignment() const
{
	return m_horizontalAlignment;
}

void MultiLineTextBox::SetHiddenText(bool lbHidden)
{
	m_hiddenText = lbHidden;
}

bool MultiLineTextBox::IsHiddenText() const
{
	return m_hiddenText;
}

void MultiLineTextBox::SetHideCharacter(char lHideChar)
{
	m_hideCharacter = lHideChar;
}

char MultiLineTextBox::GetHideCharacter() const
{
	return m_hideCharacter;
}

void MultiLineTextBox::SetCallBackFunction_OnReturnPressed(FunctionCallback lFunction)
{
	m_callbackOnReturnPressed = lFunction;
}

void MultiLineTextBox::SetCallBackData_OnReturnPressed(void* pData)
{
	m_pCallbackDataOnReturnPressed = pData;
}

ComponentType MultiLineTextBox::GetComponentType() const
{
	return ComponentType::MultiLineTextBox;
}

void MultiLineTextBox::KeyPressed(const KeyEvent& event)
{
	if (event.GetKeyCode() == 0x08)
	{
		int numHighlightChars = m_highlightEndIndex - m_highlightStartIndex;

		if (numHighlightChars > 0)
		{
			//m_text.Erase(m_highlightStartIndex, numHighlightChars);

			m_pipeCharacterIndex = m_highlightStartIndex;
		}
		else
		{
			// Remove the previous character and update the pipe character index
			if (m_pipeCharacterIndex > 0)
			{
				m_text.erase(m_pipeCharacterIndex - 1, 1);
				m_pipeCharacterIndex -= 1;
			}
		}

		m_highlightStartIndex = m_pipeCharacterIndex;
		m_highlightEndIndex = m_pipeCharacterIndex;

		m_displayPipe = true;
		m_pPipeDisplayCountDown->ResetCountdown();
	}

	if (event.GetKeyCode() == 0x0D)
	{
		char letter[2];

		letter[0] = '\n';
		letter[1] = 0; // Make sure we include a terminator

		m_text.insert(m_pipeCharacterIndex, &letter[0]);

		// Update the pipe character index
		m_pipeCharacterIndex += 1;

		m_displayPipe = true;
		m_pPipeDisplayCountDown->ResetCountdown();
	}

	UpdateText();
}

void MultiLineTextBox::KeyReleased(const KeyEvent& event)
{
	if (event.GetKeyCode() == 0x10 || event.GetKeyCode() == 0xA0 || event.GetKeyCode() == 0xA1)
	{
		m_shiftHeld = false;
	}
}

void MultiLineTextBox::KeyTyped(const KeyEvent& event)
{
	// Do nothing
}

void MultiLineTextBox::CharacterEntered(const KeyEvent& event)
{	
	//int numHighlightChars = m_highlightEndIndex - m_highlightStartIndex;

	//if (numHighlightChars > 0)
	//{
	//	m_text.erase(m_highlightStartIndex, numHighlightChars);

	//	m_pipeCharacterIndex = m_highlightStartIndex;
	//}
	
	int characterCode = event.GetKeyCode();

	if (m_maxTextLength == 0 || m_text.size() < m_maxTextLength)
	{
		// NOTE: This range excludes all the control characters such as backspace
		// and carriage return (code < 31), these should be handled in key events.
		if (characterCode > 31 && characterCode < 255)
		{
			char letter[2];

			letter[0] = event.GetKeyCode();
			letter[1] = 0; // Make sure we include a terminator


			m_text.insert(m_pipeCharacterIndex, &letter[0]);

			// Update the pipe character index
			m_pipeCharacterIndex += 1;

			m_displayPipe = true;
			m_pPipeDisplayCountDown->ResetCountdown();
		}
	}

	m_highlightStartIndex = m_pipeCharacterIndex;
	m_highlightEndIndex = m_pipeCharacterIndex;

	UpdateText();

	m_scrollBar.SetScrollPosition(0.0f);
}

void MultiLineTextBox::FocusLost(const FocusEvent& event)
{
	m_highlightStartIndex = m_pipeCharacterIndex;
	m_highlightEndIndex = m_pipeCharacterIndex;
}

void MultiLineTextBox::FocusGained(const FocusEvent& event)
{
	m_displayPipe = true;
	m_pPipeDisplayCountDown->ResetCountdown();

	m_highlightStartIndex = m_pipeCharacterIndex;
	m_highlightEndIndex = m_pipeCharacterIndex;
}

void MultiLineTextBox::OnReturnPressed()
{
	// Do nothing
}

void MultiLineTextBox::UpdateText()
{
	// Parse the text into separate label components that we will manage
	ParseText();

	// Re-align the label components
	ReAlignText();

	// Update the scrollbar
	UpdateScrollbar();

	DepthSortComponentChildren();
}

void MultiLineTextBox::ReAlignText()
{
	// We will need to know the height of the text
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_GUIFont, "a");

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

	for (auto iter = m_labelList.begin(); iter != m_labelList.end(); ++iter)
	{
		// Apply the position modifications to the label component
		int x = (*iter).GetLocation().x + xModification;
		int y = (*iter).GetLocation().y - yModification;

		(*iter).SetLocation(x, y);
		(*iter).SetDepth(3.0f);
	}
}

void MultiLineTextBox::ParseText()
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

	unsigned int currentFontID = m_GUIFont;  // Start off with the normal font

	// Keep track of the text height, needed when we create new lines of text
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(currentFontID, "a");

	// Next go though every character in the text
	int charIndex = 0;
	int startLineIndex = 0;
	int lastGoodSpaceIndex = 0;
	bool isFoundGoodSpace = false;
	bool isFinalCharFound = false;
	bool isLongWordwrapFound;
	m_newLinePipeRender = false;
	const char* pString = m_text.c_str();

	// Our current label position
	int currentTextX = m_textIndent;
	int currentTextPosition = m_textIndent;
	int currentTextY = m_dimensions.height - m_textIndent - textHeight;

	// Current label color
	Color currentColor(1.0f, 1.0f, 1.0f);

	while (pString[charIndex] != 0)
	{
		char pChar = pString[charIndex];

		isLongWordwrapFound = false;
		m_newLinePipeRender = false;

		std::string str1(m_text.substr(startLineIndex, charIndex + 1 - startLineIndex));
		if (isFoundGoodSpace)
		{
			str1 = m_text.substr(lastGoodSpaceIndex + 1, (charIndex + 1) - (lastGoodSpaceIndex + 1));
		}

		int textLineWidth1 = m_pRenderer->GetFreeTypeTextWidth(currentFontID, "%s", str1.c_str());

		// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
		if (currentTextPosition + textLineWidth1 > m_dimensions.width)
		{
			int indexToUse = charIndex;
		
			std::string str2(m_text.substr(startLineIndex, indexToUse - startLineIndex));
			if (isFoundGoodSpace)
			{
				str2 = m_text.substr(startLineIndex, lastGoodSpaceIndex - startLineIndex);
			}

			Label newLabel(m_pRenderer, currentFontID, str2, currentColor);
			newLabel.SetLocation(currentTextX, currentTextY);
			newLabel.SetDepth(3.0f);
			m_labelList.push_back(newLabel);

			// Modify our Y position, since we are moving down to a new line
			currentTextY -= textHeight;

			// Skip over the new line, else we will detect it on the next loop t
			startLineIndex = indexToUse;
			if (isFoundGoodSpace)
			{
				startLineIndex = lastGoodSpaceIndex + 1;
			}

			// Reset the current text X position, since we have created a new line
			currentTextX = m_textIndent;
			currentTextPosition = m_textIndent;

			// Increase the line number count
			m_numLines++;

			// Reset the good space flag, since we have created a new line
			isFoundGoodSpace = false;

			//m_newLinePipeRender = true;

			isLongWordwrapFound = true;
		}

		// Check for spaces
		if (isLongWordwrapFound == false && pChar == ' ')
		{
			int textLineWidth2 = m_pRenderer->GetFreeTypeTextWidth(currentFontID, "%s", str1.c_str());

			// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
			if (currentTextX + textLineWidth2 > m_dimensions.width)
			{
				std::string str2;

				if (isFoundGoodSpace)
				{
					// We need to create a new line now, but only include up to the last good space in the previous label that we are making
					str2 = std::string(m_text.substr(startLineIndex, lastGoodSpaceIndex - startLineIndex));
				}
				else
				{
					// We haven't been able to find a good space, that means that that a whole word is longer than the width of the box
					str2 = std::string(m_text.substr(startLineIndex, charIndex - startLineIndex));
				}

				Label newLabel(m_pRenderer, currentFontID, str2, currentColor);
				newLabel.SetLocation(currentTextX, currentTextY);
				newLabel.SetDepth(3.0f);
				m_labelList.push_back(newLabel);

				// Modify our Y position, since we are moving down to a new line
				currentTextY -= textHeight;

				if (isFoundGoodSpace)
				{
					charIndex = lastGoodSpaceIndex;
				}

				// Skip over the space, we don't want to start the new line with a leading space
				startLineIndex = charIndex + 1;

				// Reset the current text X position, since we have created a new line
				currentTextX = m_textIndent;
				currentTextPosition = m_textIndent;

				// Increase the line number count
				m_numLines++;

				// Reset the good space flag, since we have created a new line
				isFoundGoodSpace = false;
			}
			else
			{
				std::string lString(m_text.substr(startLineIndex, charIndex - startLineIndex));

				int textLineWidth3 = m_pRenderer->GetFreeTypeTextWidth(currentFontID, "%s", lString.c_str());

				currentTextPosition = m_textIndent + textLineWidth3;

				isFoundGoodSpace = true;

				lastGoodSpaceIndex = charIndex;
			}
		}

		// Check for new lines, being at the end of a word-wrapped line, this usually means the last word doesn't get wrapped properly
		if (isLongWordwrapFound == false && pChar == '\n')
		{
			std::string str2(m_text.substr(startLineIndex, charIndex - startLineIndex));

			int textLineWidth2 = m_pRenderer->GetFreeTypeTextWidth(currentFontID, "%s", str2.c_str());

			// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
			if (currentTextPosition + textLineWidth2 > m_dimensions.width)
			{
				if (isFoundGoodSpace)
				{
					std::string str3(m_text.substr(startLineIndex, lastGoodSpaceIndex - startLineIndex));

					Label newLabel(m_pRenderer, currentFontID, str3, currentColor);
					newLabel.SetLocation(currentTextX, currentTextY);
					newLabel.SetDepth(3.0f);
					m_labelList.push_back(newLabel);

					// Modify our Y position, since we are moving down to a new line
					currentTextY -= textHeight;

					// Skip over the new line, else we will detect it on the next loop t
					startLineIndex = lastGoodSpaceIndex + 1;

					// Reset the current text X position, since we have created a new line
					currentTextX = m_textIndent;
					currentTextPosition = m_textIndent;

					// Increase the line number count
					m_numLines++;
				}

				// Reset the good space flag, since we have created a new line
				isFoundGoodSpace = false;
			}
		}

		// Check for the end of the string, need to make sure the final word gets wrapped, if it is needed
		char pFinalChar = pString[charIndex + 1];
		if (pFinalChar == 0)
		{
			// Make sure we don't try to include new line characters in the width calculations
			char pPreviousChar = pString[charIndex];
			int indexToUse = charIndex;

			std::string str2(m_text.substr(startLineIndex, indexToUse - startLineIndex));
			if (isFoundGoodSpace)
			{
				str2 = m_text.substr(lastGoodSpaceIndex, indexToUse);
			}

			int textLineWidth2 = m_pRenderer->GetFreeTypeTextWidth(currentFontID, "%s", str2.c_str());

			// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
			if (currentTextPosition + textLineWidth2 > m_dimensions.width)
			{
				if (isFoundGoodSpace)
				{
					std::string str3(m_text.substr(startLineIndex, lastGoodSpaceIndex - startLineIndex));

					Label newLabel(m_pRenderer, currentFontID, str3, currentColor);
					newLabel.SetLocation(currentTextX, currentTextY);
					newLabel.SetDepth(3.0f);
					m_labelList.push_back(newLabel);

					// Modify our Y position, since we are moving down to a new line
					currentTextY -= textHeight;

					// Skip over the new line, else we will detect it on the next loop t
					startLineIndex = lastGoodSpaceIndex + 1;

					// Reset the current text X position, since we have created a new line
					currentTextX = m_textIndent;
					currentTextPosition = m_textIndent;

					// Increase the line number count
					m_numLines++;
				}

				// Reset the good space flag, since we have created a new line
				isFoundGoodSpace = false;
			}

			// Make sure we don't try to render new line characters
			indexToUse = charIndex + 1;
			if (pPreviousChar == '\n')
			{
				indexToUse = charIndex;
			}

			str2 = std::string(m_text.substr(startLineIndex, indexToUse - startLineIndex));

			// Create the final label component
			Label newLabel(m_pRenderer, currentFontID, str2, currentColor);

			newLabel.SetLocation(currentTextX, currentTextY);
			newLabel.SetDepth(3.0f);

			// Add the final label to the list
			m_labelList.push_back(newLabel);

			// Make sure we don't process any more rules.
			isFinalCharFound = true;
		}

		// Checking for new lines
		if (pChar == '\n')
		{
			// We have encountered a new line...

			if (isFinalCharFound == true)
			{
				// Modify our Y position, since we are moving down to a new line
				currentTextY -= textHeight;

				// Reset the current text X position, since we have created a new line
				currentTextX = m_textIndent;
				currentTextPosition = m_textIndent;

				// Increase the line number count
				m_numLines++;

				m_newLinePipeRender = true;
			}
			else
			{
				// Create the substring of the text for our new label component
				std::string str2(m_text.substr(startLineIndex, charIndex - startLineIndex));

				// Create the new label component using the new string we have just generated
				Label newLabel(m_pRenderer, currentFontID, str2, currentColor);

				// Set the location of this new label component
				newLabel.SetLocation(currentTextX, currentTextY);
				newLabel.SetDepth(3.0f);

				// Add the new label to the label list
				m_labelList.push_back(newLabel);

				// Set the new starting index, REMEMBERING to skip over the new line char
				startLineIndex = charIndex + 1;

				// Modify our Y position, since we are moving down to a new line
				currentTextY -= textHeight;

				// Reset the current text X position, since we have created a new line
				currentTextX = m_textIndent;
				currentTextPosition = m_textIndent;

				// Increase the line number count
				m_numLines++;

				// Reset the good space flag, since we have created a new line
				isFoundGoodSpace = false;
			}
		}

		charIndex++;
	}
}

void MultiLineTextBox::UpdateScrollbar()
{
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_GUIFont, "%s", "a");

	int visibleSize = GetDimensions().height;
	int neededHeight = m_numLines * textHeight;
	int heightDiff = neededHeight - visibleSize;
	
	if (heightDiff > 0)
	{
		m_scrollBar.SetScrollSize(static_cast<float>(visibleSize) / static_cast<float>(neededHeight));
		m_scrollBar.SetDisabled(false);

		float scrollRatio = m_scrollBar.GetScrollRatio();
		int offsetLabelY = static_cast<int>(heightDiff * scrollRatio);

		m_pipeYOffset = offsetLabelY;

		for (size_t i = 0; i < m_labelList.size(); ++i)
		{
			int xPos = m_textIndent;
			int yPos = GetDimensions().height - ((i + 1) * textHeight);

			m_labelList[i].SetLocation(xPos, yPos + offsetLabelY);
		}
	}
	else
	{
		m_scrollBar.SetScrollSize(0.0f);
		m_scrollBar.SetDisabled(true);
	}
}

void MultiLineTextBox::ClearLabels()
{
	m_labelList.clear();
}

void MultiLineTextBox::DrawChildren()
{
	// Do a scissor test here
	int scissorX = GetLocation().x - 1;
	int scissorY = GetLocation().y - 1;

	if (GetParent() != nullptr)
	{
		scissorX = GetParent()->GetLocation().x + scissorX;
		scissorY = GetParent()->GetLocation().y + scissorY;
	}

	int scissorWidth = GetDimensions().width + 20 + 2;
	int scissorHeight = GetDimensions().height + 2;

	bool isNeedScissor = true;

	if (isNeedScissor)
	{
		m_pRenderer->EnableScissorTest(scissorX, scissorY, scissorWidth, scissorHeight);;
	}

	Container::DrawChildren();

	// Render all the normal label components that we contain
	for (auto iter = m_labelList.begin(); iter != m_labelList.end(); ++iter)
	{
		(*iter).Draw();
	}

	if (isNeedScissor)
	{
		m_pRenderer->DisableScissorTest();
	}
}

void MultiLineTextBox::DrawSelf()
{
	// Do a scissor test here
	int scissorX = GetLocation().x - 1;
	int scissorY = GetLocation().y - 1;

	if (GetParent() != nullptr)
	{
		scissorX = GetParent()->GetLocation().x + scissorX;
		scissorY = GetParent()->GetLocation().y + scissorY;
	}

	int scissorWidth = GetDimensions().width + 20 + 2;
	int scissorHeight = GetDimensions().height + 2;

	bool isNeedScissor = true;

	if (isNeedScissor)
	{
		m_pRenderer->EnableScissorTest(scissorX, scissorY, scissorWidth, scissorHeight);;
	}

	int containerHeight = m_dimensions.height;
	float depth = GetDepth() + 1.0f;

	int textSpacer = 2;
	
	std::string displayText = "";
	if (m_labelList.size() >= 1)
	{
		displayText = m_labelList[m_labelList.size() - 1].GetText();
	}

	if (m_hiddenText)
	{
		displayText.replace(0, displayText.size(), displayText.size(), m_hideCharacter);
	}

	if (HasFocus() && m_displayPipe)
	{
		std::string pipSubString = displayText;
		pipSubString = pipSubString.substr(0, m_pipeCharacterIndex);

		int heightSpace = 3;
		int pipeWidth = 2;
		int textWidth = textSpacer + m_pRenderer->GetFreeTypeTextWidth(m_GUIFont, "%s", pipSubString.c_str());
		if (m_newLinePipeRender)
		{
			textWidth = 0;
		}
		int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_GUIFont, "%s", "a");
		int pipeX1 = m_textIndent + textWidth;
		int pipeX2 = m_textIndent + textWidth + pipeWidth;
		int pipeY1;
		int pipeY2;
		if (m_pipeHeight == 0)
		{
			pipeY1 = heightSpace + m_pipeYOffset;
			pipeY2 = containerHeight - heightSpace + m_pipeYOffset;
		}
		else
		{
			pipeY1 = containerHeight - textHeight + m_pipeYOffset;
			pipeY2 = containerHeight - textHeight + m_pipeHeight + m_pipeYOffset;
		}

		if (m_labelList.size() >= 1)
		{
			pipeY1 = containerHeight - (textHeight*m_numLines) + m_pipeYOffset;
			pipeY2 = containerHeight - (textHeight*m_numLines) + m_pipeHeight + m_pipeYOffset;
		}

		// Draw the pipe (vertical bar)
		m_pRenderer->PushMatrix();
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
		m_pRenderer->ImmediateColorAlpha(m_pipeColor.GetRed(), m_pipeColor.GetGreen(), m_pipeColor.GetBlue(), 1.0f);
		m_pRenderer->ImmediateVertex(pipeX1, pipeY1, static_cast<int>(depth));
		m_pRenderer->ImmediateVertex(pipeX2, pipeY1, static_cast<int>(depth));
		m_pRenderer->ImmediateVertex(pipeX2, pipeY2, static_cast<int>(depth));
		m_pRenderer->ImmediateVertex(pipeX1, pipeY2, static_cast<int>(depth));
		m_pRenderer->DisableImmediateMode();
		m_pRenderer->PopMatrix();
	}

	// Draw the highlighted area
	if (m_highlightEndIndex > m_highlightStartIndex)
	{
		std::string highlightSubStringStart = displayText;
		highlightSubStringStart = highlightSubStringStart.substr(0, m_highlightStartIndex);

		std::string highlightSubStringEnd = displayText;
		int numCharsToHighlight = m_highlightEndIndex - m_highlightStartIndex;
		highlightSubStringEnd = highlightSubStringEnd.substr(m_highlightStartIndex, numCharsToHighlight);

		int heightSpace = 3;
		int highlightStart = textSpacer + m_pRenderer->GetFreeTypeTextWidth(m_GUIFont, "%s", highlightSubStringStart.c_str());
		int highlightWidth = m_pRenderer->GetFreeTypeTextWidth(m_GUIFont, "%s", highlightSubStringEnd.c_str());
		int highlightX1 = m_textIndent + highlightStart;
		int highlightX2 = m_textIndent + highlightStart + highlightWidth;
		int highlightY1;
		int highlightY2;
		if (m_pipeHeight == 0)
		{
			highlightY1 = heightSpace;
			highlightY2 = containerHeight - heightSpace;
		}
		else
		{
			highlightY1 = static_cast<int>(((containerHeight - heightSpace) * 0.5f) - (m_pipeHeight * 0.5f));
			highlightY2 = static_cast<int>(((containerHeight - heightSpace) * 0.5f) + (m_pipeHeight * 0.5f));
		}

		// Draw the highlighted area
		m_pRenderer->PushMatrix();

		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
		
		m_pRenderer->ImmediateColorAlpha(0.52f, 0.53f, 0.91f, 1.0f);
		m_pRenderer->ImmediateVertex(highlightX1, highlightY1, (int)depth);
		m_pRenderer->ImmediateVertex(highlightX2, highlightY1, (int)depth);
		m_pRenderer->ImmediateVertex(highlightX2, highlightY2, (int)depth);
		m_pRenderer->ImmediateVertex(highlightX1, highlightY2, (int)depth);
		
		m_pRenderer->DisableImmediateMode();
		
		m_pRenderer->PopMatrix();
	}

	if (isNeedScissor)
	{
		m_pRenderer->DisableScissorTest();
	}
}

void MultiLineTextBox::_TogglePipeDisplay(void* pData)
{
	MultiLineTextBox* multiLineTextBox = static_cast<MultiLineTextBox*>(pData);

	if (multiLineTextBox)
	{
		multiLineTextBox->TogglePipeDisplay();
	}
}

void MultiLineTextBox::TogglePipeDisplay()
{
	m_displayPipe = !m_displayPipe;
}

void MultiLineTextBox::_ScrollbarChanged(void* pData)
{
	MultiLineTextBox* multiLineTextBox = static_cast<MultiLineTextBox*>(pData);
	multiLineTextBox->ScrollbarChanged();
}

void MultiLineTextBox::ScrollbarChanged()
{
	UpdateScrollbar();
}