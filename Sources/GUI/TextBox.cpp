/*************************************************************************
> File Name: TextBox.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A text entry box that accepts focus events and key events.
> Created Time: 2016/08/28
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Utils/TimeManager.h>

#include "DirectDrawRectangle.h"
#include "TextBox.h"
#include "MultiTextureIcon.h"
#include "Icon.h"
#include "FocusManager.h"

#include <../Libraries/glfw/include/GLFW/glfw3.h>

// Constructor, Destructor
TextBox::TextBox(Renderer* pRenderer, unsigned int GUIFont, const std::string &defaultText, const std::string &name) :
	Container(pRenderer),
	m_text(pRenderer, GUIFont, defaultText, Color(0.0f, 0.0f, 0.0f)), 
	m_name(pRenderer, GUIFont, name, Color(0.0f, 0.0f, 0.0f)),
	m_GUIFont(GUIFont)
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

	m_text.SetLocation(0, 0);
	m_text.SetDepth(2.0f);
	Container::Add(&m_text);

	m_name.SetLocation(0, 0);
	m_name.SetDepth(2.0f);
	Container::Add(&m_name);

	m_highlightStartIndex = 0;
	m_highlightEndIndex = 0;

	m_pipeCharacterIndex = static_cast<int>(m_text.GetText().size());
	m_isDisplayPipe = false;

	m_pipeHeight = 0;
	m_textIndent = 0;
	m_maxTextLength = 0;

	SetText(defaultText);

	m_isShiftHeld = false;

	m_isHiddenText = false;
	m_hideCharacter = '*';

	m_isDontLoseFocus = false;

	m_callbackOnReturnPressed = nullptr;

	m_pPipeDisplayCountDown = new CountdownTimer();
	m_pPipeDisplayCountDown->SetCountdownTime(0.5f);
	m_pPipeDisplayCountDown->SetLooping(true);
	m_pPipeDisplayCountDown->SetCallBackFunction(_TogglePipeDisplay);
	m_pPipeDisplayCountDown->SetCallBackData(this);
	m_pPipeDisplayCountDown->StartCountdown();
}

TextBox::~TextBox()
{
	TimeManager::GetInstance()->RemoveCountdownTimer(m_pPipeDisplayCountDown);

	delete m_pBackgroundIcon;
}

void TextBox::AddEventListeners()
{
	Container::AddEventListeners();

	AddMouseListener(this);
	AddFocusListener(this);
	AddKeyListener(this);
}

void TextBox::RemoveEventListeners()
{
	Container::RemoveEventListeners();

	RemoveMouseListener(this);
	RemoveFocusListener(this);
	RemoveKeyListener(this);
}

void TextBox::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	// Also set the dimensions of the icons
	m_pBackgroundIcon->SetDimensions(0, 0, width, height);

	int nameSpace = 5;
	int nameLabelWidth = m_pRenderer->GetFreeTypeTextWidth(m_GUIFont, "%s", m_name.GetText().c_str());
	int nameLabelHeight = m_pRenderer->GetFreeTypeTextHeight(m_GUIFont, "%s", m_name.GetText().c_str());
	int centerHeight = (height / 2) - (nameLabelHeight / 2);
	m_name.SetLocation(-nameLabelWidth - nameSpace, centerHeight);
}

void TextBox::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void TextBox::SetLabelPositionOffset(int offsetX, int offsetY)
{
	m_name.SetLocation(offsetX, offsetY);
}

void TextBox::SetTextColor(Color color)
{
	m_text.SetColor(color);
}

void TextBox::SetBackgroundIcon(RenderRectangle *icon)
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

void TextBox::SetPipHeight(int height)
{
	m_pipeHeight = height;
}

void TextBox::SetTextIndent(int indent)
{
	m_textIndent = indent;
}

void TextBox::SetMaxTextLength(int length)
{
	m_maxTextLength = length;
}

void TextBox::SetPipeColor(Color pipeColor)
{
	m_pipeColor = pipeColor;
}

void TextBox::SetText(const std::string &label)
{
	m_text.SetText(label);
	m_pipeCharacterIndex = static_cast<int>(m_text.GetText().size());
}

const std::string& TextBox::GetText() const
{
	return m_text.GetText();
}

void TextBox::SetHiddenText(bool isHidden)
{
	m_isHiddenText = isHidden;
}

bool TextBox::IsHiddenText() const
{
	return m_isHiddenText;
}

void TextBox::SetHideCharacter(char hideChar)
{
	m_hideCharacter = hideChar;
}

char TextBox::GetHideCharacter() const
{
	return m_hideCharacter;
}

void TextBox::SetDontLoseFocus(bool isDontLoseFocus)
{
	m_isDontLoseFocus = isDontLoseFocus;
}

bool TextBox::IsDontLoseFocusSet() const
{
	return m_isDontLoseFocus;
}

void TextBox::SetCallBackFunction_OnReturnPressed(FunctionCallback func)
{
	m_callbackOnReturnPressed = func;
}

void TextBox::SetCallBackData_OnReturnPressed(void* pData)
{
	m_pCallbackDataOnReturnPressed = pData;
}

ComponentType TextBox::GetComponentType() const
{
	return ComponentType::TextBox;
}

void TextBox::KeyPressed(const KeyEvent& event)
{
	if (event.GetKeyCode() == GLFW_KEY_BACKSPACE)
	{
		int numHighlightChars = m_highlightEndIndex - m_highlightStartIndex;

		if (numHighlightChars > 0)
		{
			m_text.Erase(m_highlightStartIndex, numHighlightChars);

			m_pipeCharacterIndex = m_highlightStartIndex;
		}
		else
		{
			// Remove the previous character and update the pipe character index
			if (m_pipeCharacterIndex > 0)
			{
				m_text.Erase(m_pipeCharacterIndex - 1, 1);
				m_pipeCharacterIndex -= 1;
			}
		}

		m_highlightStartIndex = m_pipeCharacterIndex;
		m_highlightEndIndex = m_pipeCharacterIndex;

		m_isDisplayPipe = true;
		m_pPipeDisplayCountDown->ResetCountdown();
	}

	if (event.GetKeyCode() == GLFW_KEY_DELETE)
	{
		int numHighlightChars = m_highlightEndIndex - m_highlightStartIndex;

		if (numHighlightChars > 0)
		{
			m_text.Erase(m_highlightStartIndex, numHighlightChars);

			m_pipeCharacterIndex = m_highlightStartIndex;
		}
		else
		{
			// Remove the current character and leave the pipe character index the same
			int stringSize = static_cast<int>(m_text.GetText().size());
			if (m_pipeCharacterIndex < stringSize)
			{
				m_text.Erase(m_pipeCharacterIndex, 1);
			}
		}

		m_highlightStartIndex = m_pipeCharacterIndex;
		m_highlightEndIndex = m_pipeCharacterIndex;

		m_isDisplayPipe = true;
		m_pPipeDisplayCountDown->ResetCountdown();
	}

	if (event.GetKeyCode() == GLFW_KEY_HOME)
	{
		// Set pipe to start
		m_pipeCharacterIndex = 0;

		if (m_isShiftHeld)
		{
			m_highlightEndIndex = m_highlightStartIndex;
			m_highlightStartIndex = m_pipeCharacterIndex;
		}
		else
		{
			m_highlightStartIndex = m_pipeCharacterIndex;
			m_highlightEndIndex = m_pipeCharacterIndex;
		}

		m_isDisplayPipe = true;
		m_pPipeDisplayCountDown->ResetCountdown();
	}

	if (event.GetKeyCode() == GLFW_KEY_END)
	{
		// Set pipe to end
		int stringSize = static_cast<int>(m_text.GetText().size());
		m_pipeCharacterIndex = stringSize;

		if (m_isShiftHeld)
		{
			m_highlightStartIndex = m_highlightEndIndex;
			m_highlightEndIndex = m_pipeCharacterIndex;
		}
		else
		{
			m_highlightStartIndex = m_pipeCharacterIndex;
			m_highlightEndIndex = m_pipeCharacterIndex;
		}

		m_isDisplayPipe = true;
		m_pPipeDisplayCountDown->ResetCountdown();
	}

	if (event.GetKeyCode() == GLFW_KEY_ENTER)
	{
		if (m_isDontLoseFocus == false)
		{
			FocusManager::GetInstance()->SetFocusOwner(nullptr);
		}

		// Try to call our callback function, if we have one
		if (m_callbackOnReturnPressed)
		{
			m_callbackOnReturnPressed(m_pCallbackDataOnReturnPressed);
		}

		// Signal that we have pressed return, allows custom sub-class behaviors
		OnReturnPressed();
	}

	if (event.GetKeyCode() == GLFW_KEY_LEFT)
	{
		if (m_pipeCharacterIndex > 0)
		{
			if (m_isShiftHeld)
			{
				if (m_highlightStartIndex == m_pipeCharacterIndex)
				{
					m_highlightStartIndex -= 1;
				}
				else
				{
					m_highlightEndIndex -= 1;
				}
			}
			else
			{
				m_highlightStartIndex = m_pipeCharacterIndex - 1;
				m_highlightEndIndex = m_pipeCharacterIndex - 1;
			}

			m_pipeCharacterIndex -= 1;
		}
		else
		{
			if (!m_isShiftHeld)
			{
				m_highlightStartIndex = m_pipeCharacterIndex;
				m_highlightEndIndex = m_pipeCharacterIndex;
			}
		}

		m_isDisplayPipe = true;
		m_pPipeDisplayCountDown->ResetCountdown();
	}

	if (event.GetKeyCode() == GLFW_KEY_RIGHT)
	{
		int stringSize = static_cast<int>(m_text.GetText().size());
		
		if (m_pipeCharacterIndex < stringSize)
		{
			if (m_isShiftHeld)
			{
				if (m_highlightEndIndex == m_pipeCharacterIndex)
				{
					m_highlightEndIndex += 1;
				}
				else
				{
					m_highlightStartIndex += 1;
				}
			}
			else
			{
				m_highlightStartIndex = m_pipeCharacterIndex + 1;
				m_highlightEndIndex = m_pipeCharacterIndex + 1;
			}

			m_pipeCharacterIndex += 1;
		}
		else
		{
			if (!m_isShiftHeld)
			{
				m_highlightStartIndex = m_pipeCharacterIndex;
				m_highlightEndIndex = m_pipeCharacterIndex;
			}
		}

		m_isDisplayPipe = true;
		m_pPipeDisplayCountDown->ResetCountdown();
	}

	if (event.GetKeyCode() == GLFW_KEY_LEFT_SHIFT || event.GetKeyCode() == GLFW_KEY_RIGHT_SHIFT)
	{
		m_isShiftHeld = true;
	}
}

void TextBox::KeyReleased(const KeyEvent& event)
{
	if (event.GetKeyCode() == GLFW_KEY_LEFT_SHIFT || event.GetKeyCode() == GLFW_KEY_RIGHT_SHIFT)
	{
		m_isShiftHeld = false;
	}
}

void TextBox::KeyTyped(const KeyEvent& event)
{
	// Do nothing
}

void TextBox::CharacterEntered(const KeyEvent& event)
{
	int numHighlightChars = m_highlightEndIndex - m_highlightStartIndex;

	if (numHighlightChars > 0)
	{
		m_text.Erase(m_highlightStartIndex, numHighlightChars);

		m_pipeCharacterIndex = m_highlightStartIndex;
	}

	int characterCode = event.GetKeyCode();

	if (m_maxTextLength == 0 || m_text.GetText().size() < static_cast<size_t>(m_maxTextLength))
	{
		// NOTE: This range excludes all the control characters such as backspace
		// and carriage return (code < 31), these should be handled in key events.
		if (characterCode > 31 && characterCode < 255)
		{
			char ch[2];

			ch[0] = event.GetKeyCode();
			ch[1] = 0; // Make sure we include a terminator

			m_text.Insert(m_pipeCharacterIndex, &ch[0]);

			// Update the pipe character index
			m_pipeCharacterIndex += 1;

			m_isDisplayPipe = true;
			m_pPipeDisplayCountDown->ResetCountdown();
		}
	}

	m_highlightStartIndex = m_pipeCharacterIndex;
	m_highlightEndIndex = m_pipeCharacterIndex;
}

void TextBox::FocusLost(const FocusEvent& event)
{
	m_highlightStartIndex = m_pipeCharacterIndex;
	m_highlightEndIndex = m_pipeCharacterIndex;
}

void TextBox::FocusGained(const FocusEvent& event)
{
	m_isDisplayPipe = true;
	m_pPipeDisplayCountDown->ResetCountdown();

	m_highlightStartIndex = m_pipeCharacterIndex;
	m_highlightEndIndex = m_pipeCharacterIndex;
}

void TextBox::OnReturnPressed()
{
	// Do nothing
}

void TextBox::DrawSelf()
{
	//int containerWidth = m_dimensions.width;
	int containerHeight = m_dimensions.height;
	int depth = static_cast<int>(GetDepth() + 1);

	//int containerX1 = 0;
	//int containerX2 = containerWidth;
	//int containerY1 = 0;
	//int containerY2 = containerHeight;

	//int outlineX1 = 0;
	//int outlineX2 = containerWidth + 1;
	//int outlineY1 = 0;
	//int outlineY2 = containerHeight + 1;

	std::string displayText = m_text.GetText();

	int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_GUIFont, "%s", m_text.GetText().c_str());

	int textSpacer = 2;
	int pipeStringLength = 0;

	if (m_text.GetText().size() > 0)
	{
		std::string pipeSubString = displayText;
		pipeSubString = pipeSubString.substr(0, m_pipeCharacterIndex);
		pipeStringLength = m_pRenderer->GetFreeTypeTextWidth(m_GUIFont, "%s", pipeSubString.c_str());;

		if (pipeStringLength < GetDimensions().width - (textSpacer * 2))
		{
			pipeStringLength = 0;
		}
		else
		{
			pipeStringLength = pipeStringLength - GetDimensions().width + (textSpacer * 2);
		}

		std::string pipeReverseSubString = displayText;
		pipeReverseSubString = pipeReverseSubString.substr(m_pipeCharacterIndex, m_text.GetText().size());
	}

	int xDiff = -pipeStringLength;
	int xDiff2 = 0;
	int centerHeight = (GetDimensions().height / 2) - (textHeight / 2);
	int posX = m_textIndent + textSpacer + xDiff + xDiff2;
	int posY = centerHeight;

	m_text.SetLocation(posX, posY);
	
	// Do a scissor test here
	int scissorX = GetLocation().x;
	int scissorY = GetLocation().y;

	if (GetParent() != nullptr)
	{
		scissorX = GetParent()->GetLocation().x + scissorX;
		scissorY = GetParent()->GetLocation().y + scissorY;
	}
	
	int scissorWidth = GetDimensions().width;
	int scissorHeight = GetDimensions().height;

	if (m_isHiddenText)
	{
		displayText.replace(0, displayText.size(), displayText.size(), m_hideCharacter);
	}

	if (HasFocus() && m_isDisplayPipe)
	{
		std::string pipSubString = displayText;
		pipSubString = pipSubString.substr(0, m_pipeCharacterIndex);

		int heightSpace = 3;
		int pipeWidth = 2;
		int textWidth = textSpacer + m_pRenderer->GetFreeTypeTextWidth(m_GUIFont, "%s", pipSubString.c_str());
		int pipeX1 = m_textIndent + textWidth + xDiff + xDiff2;
		int pipeX2 = m_textIndent + textWidth + pipeWidth + xDiff + xDiff2;
		
		int pipeY1;
		int pipeY2;

		if (m_pipeHeight == 0)
		{
			pipeY1 = heightSpace;
			pipeY2 = containerHeight - heightSpace;
		}
		else
		{
			pipeY1 = static_cast<int>(((containerHeight - heightSpace) * 0.5f) - (m_pipeHeight * 0.5f));
			pipeY2 = static_cast<int>(((containerHeight - heightSpace) * 0.5f) + (m_pipeHeight * 0.5f));
		}

		// Draw the pipe (vertical bar)
		m_pRenderer->PushMatrix();

		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

		m_pRenderer->ImmediateColorAlpha(m_pipeColor.GetRed(), m_pipeColor.GetGreen(), m_pipeColor.GetBlue(), 1.0f);
		m_pRenderer->ImmediateVertex(pipeX1, pipeY1, depth);
		m_pRenderer->ImmediateVertex(pipeX2, pipeY1, depth);
		m_pRenderer->ImmediateVertex(pipeX2, pipeY2, depth);
		m_pRenderer->ImmediateVertex(pipeX1, pipeY2, depth);
		
		m_pRenderer->DisableImmediateMode();
		
		m_pRenderer->PopMatrix();
	}

	bool isNeedScissor = true;
	if (isNeedScissor)
	{
		m_pRenderer->EnableScissorTest(scissorX, scissorY, scissorWidth, scissorHeight);
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
		int highlightX1 = m_textIndent + highlightStart + xDiff + xDiff2;
		int highlightX2 = m_textIndent + highlightStart + highlightWidth + xDiff + xDiff2;
		
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
		m_pRenderer->ImmediateVertex(highlightX1, highlightY1, depth);
		m_pRenderer->ImmediateVertex(highlightX2, highlightY1, depth);
		m_pRenderer->ImmediateVertex(highlightX2, highlightY2, depth);
		m_pRenderer->ImmediateVertex(highlightX1, highlightY2, depth);
		
		m_pRenderer->DisableImmediateMode();
		
		m_pRenderer->PopMatrix();
	}

	if (isNeedScissor)
	{
		m_pRenderer->DisableScissorTest();
	}

	m_name.SetVisible(true);

	//// Draw the background
	//m_pRenderer->PushMatrix();

	//m_pRenderer->SetRenderMode(RenderMode::SOLID);
	//m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
	//
	//if (HasFocus())
	//{
	//	m_pRenderer->ImmediateColorAlpha(0.75f, 0.75f, 0.75f, 1.0f);
	//}
	//else
	//{
	//	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	//}

	//m_pRenderer->ImmediateVertex(containerX1, containerY1, depth);
	//m_pRenderer->ImmediateVertex(containerX2, containerY1, depth);
	//m_pRenderer->ImmediateVertex(containerX2, containerY2, depth);
	//m_pRenderer->ImmediateVertex(containerX1, containerY2, depth);
	//
	//m_pRenderer->DisableImmediateMode();
	//
	//m_pRenderer->PopMatrix();

	//// Draw the outline
	//m_pRenderer->PushMatrix();
	//
	//m_pRenderer->SetLineWidth(1.0f);
	//m_pRenderer->ImmediateColorAlpha(0.0f, 0.0f, 0.0f, 1.0f);
	//
	//m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINE_LOOP);
	//m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);
	//m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);
	//m_pRenderer->ImmediateVertex(outlineX2, outlineY2, depth);
	//m_pRenderer->ImmediateVertex(outlineX1, outlineY2, depth);
	//
	//m_pRenderer->DisableImmediateMode();
	//
	//m_pRenderer->PopMatrix();

	//m_pRenderer->RenderFreeTypeText(m_GUIFont, static_cast<float>(posX), static_cast<float>(posY), GetDepth()+1.0f, m_pipeColor, "%s", displayText.c_str());
}

void TextBox::DrawChildren()
{
	// Do a scissor test here
	int scissorX = GetLocation().x - 1;
	int scissorY = GetLocation().y - 1;

	if (GetParent() != nullptr)
	{
		scissorX = GetParent()->GetLocation().x + scissorX;
		scissorY = GetParent()->GetLocation().y + scissorY;
	}

	int scissorWidth = GetDimensions().width + 2;
	int scissorHeight = GetDimensions().height + 2;
	//int depth = static_cast<int>(GetDepth());

	bool isNeedScissor = true;

	if (isNeedScissor)
	{
		m_pRenderer->EnableScissorTest(scissorX, scissorY, scissorWidth, scissorHeight);;
	}

	Container::DrawChildren();

	// DEBUG: Draw the scissor region
	//m_pRenderer->PushMatrix();

	//m_pRenderer->SetLineWidth(1.0f);
	//m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
	//m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINE_LOOP);

	//m_pRenderer->ImmediateVertex((scissorX - GetLocation().x),						(scissorY - GetLocation().y),					depth);
	//m_pRenderer->ImmediateVertex((scissorX - GetLocation().x) + scissorWidth,		(scissorY - GetLocation().y),					depth);
	//m_pRenderer->ImmediateVertex((scissorX - GetLocation().x) + scissorWidth + 1,	(scissorY - GetLocation().y) + scissorHeight,	depth);
	//m_pRenderer->ImmediateVertex((scissorX - GetLocation().x),						(scissorY - GetLocation().y) + scissorHeight,	depth);
	//
	//m_pRenderer->DisableImmediateMode();
	//
	//m_pRenderer->PopMatrix();

	if (IsContainer() && HasChildren() && IsVisible())
	{
		for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
		{
			if ((*iter) == &m_name)
			{
				continue;  // Don't render name header inside scissor test
			}

			if ((*iter)->IsVisible())
			{
				(*iter)->Draw();
			}
		}
	}

	if (isNeedScissor)
	{
		m_pRenderer->DisableScissorTest();
	}

	// Render name header outside of scissor test
	m_name.Draw();
}

void TextBox::_TogglePipeDisplay(void* pData)
{
	TextBox* textBox = static_cast<TextBox*>(pData);

	if (textBox)
	{
		textBox->TogglePipeDisplay();
	}
}

void TextBox::TogglePipeDisplay()
{
	m_isDisplayPipe = !m_isDisplayPipe;
}