/*************************************************************************
> File Name: Button.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A simple button class that defines standard button behavior.
> Created Time: 2016/08/09
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "GUIWindow.h"
#include "Icon.h"
#include "Button.h"

// Constructor, Destructor
Button::Button(Renderer* pRenderer, unsigned int GUIFont, const std::string& label) :
	AbstractButton(pRenderer, GUIFont, label)
{
	AbstractButton::SetDefaultIcons(pRenderer);

	m_label.SetLocation(0, 0);
	Container::Add(&m_label);

	m_label.SetDepth(3.0f);

	m_pressedOffsetX = 0;
	m_pressedOffsetY = 0;
	m_offsetApplied = false;

	m_changeLabelText = false;
}

Button::Button(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color fontColor, Color outlineColor) :
	AbstractButton(pRenderer, GUIFont, label)
{
	AbstractButton::SetDefaultIcons(pRenderer);

	m_normalLabelColor = fontColor;
	m_hoverLabelColor = fontColor;
	m_PressedLabelColor = fontColor;

	m_label.SetLocation(0, 0);
	m_label.SetColor(fontColor);
	m_label.SetOutline(true);
	m_label.SetOutlineColor(outlineColor);
	m_label.SetOutlineFont(outlineGUIFont);
	Container::Add(&m_label);

	m_label.SetDepth(3.0f);

	m_pressedOffsetX = 0;
	m_pressedOffsetY = 0;
	m_offsetApplied = false;

	m_changeLabelText = false;
}

Button::Button(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, const std::string& defaultTexture, const std::string& selectedTexture, const std::string& hoverTexture, const std::string& disabledTexture) :
	AbstractButton(pRenderer, GUIFont, label)
{
	m_pDefaultIcon = new Icon(pRenderer, defaultTexture);
	m_pHoverIcon = new Icon(pRenderer, hoverTexture);
	m_pSelectedIcon = new Icon(pRenderer, selectedTexture);
	m_pDisabledIcon = new Icon(pRenderer, disabledTexture);

	Container::Add(m_pDefaultIcon);
	Container::Add(m_pSelectedIcon);
	Container::Add(m_pHoverIcon);
	Container::Add(m_pDisabledIcon);

	m_label.SetLocation(0, 0);
	Container::Add(&m_label);

	m_label.SetDepth(3.0f);

	m_pressedOffsetX = 0;
	m_pressedOffsetY = 0;
	m_offsetApplied = false;

	m_changeLabelText = false;
}

Button::~Button()
{
	Container::Remove(m_pDefaultIcon);
	Container::Remove(m_pSelectedIcon);
	Container::Remove(m_pHoverIcon);
	Container::Remove(m_pDisabledIcon);

	delete m_pDefaultIcon;
	delete m_pSelectedIcon;
	delete m_pHoverIcon;
	delete m_pDisabledIcon;

	for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
	{
		Container::Remove(m_vpAddedComponentList[i]);

		delete m_vpAddedComponentList[i];
		m_vpAddedComponentList[i] = nullptr;
	}
}

void Button::AddText(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color color, int xOffset, int yOffset, bool outline, Color outlineColor)
{
	Label* pNewLabel = new Label(pRenderer, GUIFont, label, color);
	pNewLabel->SetLocation(xOffset, yOffset);
	pNewLabel->SetDepth(pNewLabel->GetDepth() + 2.0f);

	if (outline)
	{
		pNewLabel->SetOutline(true);
		pNewLabel->SetOutlineColor(outlineColor);
		pNewLabel->SetOutlineFont(outlineGUIFont);
	}

	m_vpAddedComponentList.push_back(pNewLabel);

	Add(pNewLabel);
}

void Button::AddIcon(Renderer* pRenderer, const std::string& fileName, int texWidth, int texHeight, int width, int height, int xOffset, int yOffset, float depth)
{
	// texWidth and texHeight should always be a power of 2.
	Icon* pNewIcon = new Icon(pRenderer, fileName, texWidth, texHeight);
	pNewIcon->SetDimensions(xOffset, yOffset, width, height);
	pNewIcon->SetDepth(depth);

	m_vpAddedComponentList.push_back(pNewIcon);

	Add(pNewIcon);
}

void Button::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	// Also set the dimensions of the icons
	m_pDefaultIcon->SetDimensions(0, 0, width, height);
	m_pSelectedIcon->SetDimensions(0, 0, width, height);
	m_pHoverIcon->SetDimensions(0, 0, width, height);
	m_pDisabledIcon->SetDimensions(0, 0, width, height);

	// Set the label location to be the center of the button
	int buttonLabelHeight = m_pRenderer->GetFreeTypeTextHeight(m_label.GetFontID(), "%s", m_label.GetText().c_str());
	int buttonLabelWidth = m_pRenderer->GetFreeTypeTextWidth(m_label.GetFontID(), "%s", m_label.GetText().c_str());
	int centerWidth = static_cast<int>((width * 0.5f) - (buttonLabelWidth * 0.5f));
	int centerHeight = static_cast<int>((height * 0.5f) - (buttonLabelHeight * 0.5f));
	m_label.SetLocation(centerWidth + m_labelOffsetX, centerHeight + m_labelOffsetY);
	m_offsetApplied = false;
}

void Button::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void Button::SetLabelOffset(int x, int y)
{
	m_labelOffsetX = x;
	m_labelOffsetY = y;

	m_label.SetLocation(m_label.GetLocation().x + m_labelOffsetX, m_label.GetLocation().y + m_labelOffsetY);
}

void Button::SetPressedOffset(int x, int y)
{
	m_pressedOffsetX = x;
	m_pressedOffsetY = y;
}

void Button::SetNormalLabelColor(Color normal)
{
	m_normalLabelColor = normal;
}

void Button::SetHoverLabelColor(Color hover)
{
	m_hoverLabelColor = hover;

	m_changeLabelText = true;
}

void Button::SetPressedLabelColor(Color pressed)
{
	m_PressedLabelColor = pressed;

	m_changeLabelText = true;
}

void Button::SetLabelColor(Color color)
{
	m_label.SetColor(color);

	for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
	{
		if (m_vpAddedComponentList[i]->GetComponentType() == ComponentType::Label)
		{
			static_cast<Label*>(m_vpAddedComponentList[i])->SetColor(color);
		}
	}
}

void Button::SetLabelOutlineColor(Color color)
{
	m_label.SetOutlineColor(color);

	for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
	{
		if (m_vpAddedComponentList[i]->GetComponentType() == ComponentType::Label)
		{
			static_cast<Label*>(m_vpAddedComponentList[i])->SetOutlineColor(color);
		}
	}
}

void Button::AddEventListeners()
{
	AddMouseListener(this);
}

void Button::RemoveEventListeners()
{
	RemoveMouseListener(this);

	// Remove hover and select properties, since we have made this component unselectable by the mouse
	SetHover(false);
	SetSelected(false);
}

ComponentType Button::GetComponentType() const
{
	return ComponentType::Button;
}

void Button::MouseEntered(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetHover(true);

	OnMouseEnter();

	// Call the callback function
	if (m_EnterCallback)
	{
		m_EnterCallback(m_pEnterCallbackData);
	}

	if (m_changeLabelText)
	{
		for (unsigned int i = 0; i < m_vpAddedComponentList.size(); i++)
		{
			if (m_vpAddedComponentList[i]->GetComponentType() == ComponentType::Label)
			{
				static_cast<Label*>(m_vpAddedComponentList[i])->SetColor(m_hoverLabelColor);
			}
		}

		m_label.SetColor(m_hoverLabelColor);
	}
}

void Button::MouseExited(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	if (IsSelected() && m_callbackReleased)
	{
		// If we have pressed the button, but not released when we move outside of the button
		m_callbackReleased(m_pCallbackDataReleased);
	}

	SetHover(false);

	// Call the callback function
	if (m_ExitCallback)
	{
		m_ExitCallback(m_pExitCallbackData);
	}

	// If we are selected when we exit, reposition back the offset
	if (IsSelected() && m_offsetApplied)
	{
		for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
		{
			m_vpAddedComponentList[i]->SetLocation(m_vpAddedComponentList[i]->GetLocation().x - m_pressedOffsetX, m_vpAddedComponentList[i]->GetLocation().y - m_pressedOffsetY);
		}

		m_label.SetLocation(m_label.GetLocation().x - m_pressedOffsetX, m_label.GetLocation().y - m_pressedOffsetY);

		m_offsetApplied = false;
	}

	if (m_changeLabelText)
	{
		for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
		{
			if (m_vpAddedComponentList[i]->GetComponentType() == ComponentType::Label)
			{
				static_cast<Label*>(m_vpAddedComponentList[i])->SetColor(m_normalLabelColor);
			}
		}
		m_label.SetColor(m_normalLabelColor);
	}

	// Also removed the button selection if we exit it's dimensions
	SetSelected(false);

	OnMouseExit();
}

void Button::MousePressed(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	// If our parent is a GUIWindow, then makew this window have focus in the GUI, used to make it's depth the highest
	if (GetParent() != nullptr && GetParent()->GetComponentType() == ComponentType::GUIWindow)
	{
		GUIWindow* pParentWindow = static_cast<GUIWindow*>(GetParent());
		pParentWindow->SetFocusWindow();
	}

	SetSelected(true);

	OnMousePressed();

	// Color change
	if (m_changeLabelText)
	{
		for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
		{
			if (m_vpAddedComponentList[i]->GetComponentType() == ComponentType::Label)
			{
				static_cast<Label*>(m_vpAddedComponentList[i])->SetColor(m_PressedLabelColor);
			}
		}

		m_label.SetColor(m_PressedLabelColor);
	}

	// Position change
	if (m_offsetApplied == false)
	{
		for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
		{
			m_vpAddedComponentList[i]->SetLocation(m_vpAddedComponentList[i]->GetLocation().x + m_pressedOffsetX, m_vpAddedComponentList[i]->GetLocation().y + m_pressedOffsetY);
		}

		m_label.SetLocation(m_label.GetLocation().x + m_pressedOffsetX, m_label.GetLocation().y + m_pressedOffsetY);

		m_offsetApplied = true;
	}

	if (m_callbackPressed)
	{
		m_callbackPressed(m_pCallbackDataPressed);
	}
}

void Button::MouseReleased(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetSelected(false);

	OnMouseReleased();

	if (m_offsetApplied)
	{
		for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
		{
			if (m_changeLabelText)
			{
				if (m_vpAddedComponentList[i]->GetComponentType() == ComponentType::Label)
				{
					if (IsHover())
					{
						static_cast<Label*>(m_vpAddedComponentList[i])->SetColor(m_hoverLabelColor);
					}
					else
					{
						static_cast<Label*>(m_vpAddedComponentList[i])->SetColor(m_normalLabelColor);
					}

				}
			}

			m_vpAddedComponentList[i]->SetLocation(m_vpAddedComponentList[i]->GetLocation().x - m_pressedOffsetX, m_vpAddedComponentList[i]->GetLocation().y - m_pressedOffsetY);
		}

		m_label.SetLocation(m_label.GetLocation().x - m_pressedOffsetX, m_label.GetLocation().y - m_pressedOffsetY);
		
		if (m_changeLabelText)
		{
			if (IsHover())
			{
				m_label.SetColor(m_hoverLabelColor);
			}
			else
			{
				m_label.SetColor(m_normalLabelColor);
			}
		}

		m_offsetApplied = false;
	}

	if (m_callbackReleased)
	{
		m_callbackReleased(m_pCallbackDataReleased);
	}
}

void Button::MouseClicked(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	// Call the function if this class has a derived sub-class
	OnMouseClicked();

	if (m_changeLabelText)
	{
		if (IsHover())
		{
			for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
			{
				if (m_vpAddedComponentList[i]->GetComponentType() == ComponentType::Label)
				{
					static_cast<Label*>(m_vpAddedComponentList[i])->SetColor(m_hoverLabelColor);
				}
			}

			m_label.SetColor(m_hoverLabelColor);
		}
		else
		{
			for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
			{
				if (m_vpAddedComponentList[i]->GetComponentType() == ComponentType::Label)
				{
					static_cast<Label*>(m_vpAddedComponentList[i])->SetColor(m_normalLabelColor);
				}
			}

			m_label.SetColor(m_normalLabelColor);
		}
	}

	// Call the callback function callback if this class is just a simple button
	if (m_callback)
	{
		m_callback(m_pCallbackData);
	}
}

void Button::OnMouseEnter()
{
	// Do nothing
}

void Button::OnMouseExit()
{
	// Do nothing
}

void Button::OnMousePressed()
{
	// Do nothing
}

void Button::OnMouseReleased()
{
	// Do nothing
}

void Button::OnMouseClicked()
{
	// Do nothing
}

void Button::DrawSelf()
{
	// Reset visibility
	m_pSelectedIcon->SetVisible(false);
	m_pHoverIcon->SetVisible(false);
	m_pDisabledIcon->SetVisible(false);
	m_pDefaultIcon->SetVisible(false);

	if (IsSelected())
	{
		m_pSelectedIcon->SetVisible(true);
	}
	else if (IsHover())
	{
		m_pHoverIcon->SetVisible(true);
	}
	else if (IsDisabled())
	{
		m_pDisabledIcon->SetVisible(true);
	}
	else
	{
		m_pDefaultIcon->SetVisible(true);
	}

	// Set the label visibility
	m_label.SetVisible(m_displayLabel);
}