/*************************************************************************
> File Name: OptionBox.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An option box (radio button) GUI widget.
> Created Time: 2016/08/23
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DirectDrawRectangle.h"
#include "GUIWindow.h"
#include "Icon.h"
#include "MultiTextureIcon.h"
#include "OptionController.h"
#include "OptionBox.h"

// Constructor, Destructor
OptionBox::OptionBox(Renderer* pRenderer, unsigned int GUIFont, const std::string& label) :
	AbstractButton(pRenderer, GUIFont, label),
	m_pToggledIcon(nullptr), m_pToggledSelectedIcon(nullptr),
	m_pToggledHoverIcon(nullptr), m_pToggledDisabledIcon(nullptr)
{
	OptionBox::SetDefaultIcons(pRenderer);

	m_label.SetLocation(0, 0);
	m_label.SetDepth(3.0f);
	Container::Add(&m_label);

	m_isToggled = false;

	m_pressedOffsetX = 0;
	m_pressedOffsetY = 0;
	m_isOffsetApplied = false;

	m_changeLabelText = false;
}

OptionBox::OptionBox(Renderer* pRenderer, unsigned int GUIFont, unsigned int OutlineGUIFont, const std::string& label, Color fontColor, Color outlineColor) :
	AbstractButton(pRenderer, GUIFont, label),
	m_pToggledIcon(nullptr), m_pToggledSelectedIcon(nullptr),
	m_pToggledHoverIcon(nullptr), m_pToggledDisabledIcon(nullptr)
{
	OptionBox::SetDefaultIcons(pRenderer);

	m_normalLabelColor = fontColor;
	m_hoverLabelColor = fontColor;
	m_PressedLabelColor = fontColor;

	m_label.SetLocation(0, 0);
	m_label.SetColor(fontColor);
	m_label.SetOutline(true);
	m_label.SetOutlineColor(outlineColor);
	m_label.SetOutlineFont(OutlineGUIFont);
	m_label.SetDepth(3.0f);
	Container::Add(&m_label);

	m_isToggled = false;

	m_pressedOffsetX = 0;
	m_pressedOffsetY = 0;
	m_isOffsetApplied = false;

	m_changeLabelText = false;
}

OptionBox::OptionBox(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, const std::string& defaultTexture, const std::string& selectedTexture, const std::string& hoverTexture, const std::string& disabledTexture,
	const std::string& toggledTexture, const std::string& toggledSelectedTexture, const std::string& toggledHoverTexture, const std::string& toggledDisabledTexture) :
	AbstractButton(pRenderer, GUIFont, label)
{
	m_pDefaultIcon = new Icon(pRenderer, defaultTexture);
	m_pSelectedIcon = new Icon(pRenderer, selectedTexture);
	m_pHoverIcon = new Icon(pRenderer, hoverTexture);
	m_pDisabledIcon = new Icon(pRenderer, disabledTexture);
	m_pToggledIcon = new Icon(pRenderer, toggledTexture);
	m_pToggledSelectedIcon = new Icon(pRenderer, toggledSelectedTexture);
	m_pToggledHoverIcon = new Icon(pRenderer, toggledHoverTexture);
	m_pToggledDisabledIcon = new Icon(pRenderer, toggledDisabledTexture);

	m_label.SetLocation(0, 0);
	Container::Add(&m_label);

	m_isToggled = false;

	Container::Add(m_pDefaultIcon);
	Container::Add(m_pSelectedIcon);
	Container::Add(m_pHoverIcon);
	Container::Add(m_pDisabledIcon);
	Container::Add(m_pToggledIcon);
	Container::Add(m_pToggledSelectedIcon);
	Container::Add(m_pToggledHoverIcon);
	Container::Add(m_pToggledDisabledIcon);

	m_changeLabelText = false;
}

OptionBox::~OptionBox()
{
	delete m_pDefaultIcon;
	delete m_pSelectedIcon;
	delete m_pHoverIcon;
	delete m_pDisabledIcon;
	delete m_pToggledIcon;
	delete m_pToggledSelectedIcon;
	delete m_pToggledHoverIcon;
	delete m_pToggledDisabledIcon;
}

void OptionBox::AddEventListeners()
{
	AddMouseListener(this);
}

void OptionBox::RemoveEventListeners()
{
	RemoveMouseListener(this);

	// Remove hover and select properties, since we have made this component un-selectable by the mouse
	SetHover(false);
	SetSelected(false);
}

void OptionBox::RemoveIcon(RenderRectangle* pRemoveIcon)
{
	if (pRemoveIcon)
	{
		Remove(pRemoveIcon);
		delete pRemoveIcon;
	}
}

void OptionBox::SetDefaultIcons(Renderer* pRenderer)
{
	RemoveIcon(m_pDefaultIcon);
	RemoveIcon(m_pHoverIcon);
	RemoveIcon(m_pSelectedIcon);
	RemoveIcon(m_pDisabledIcon);
	RemoveIcon(m_pToggledIcon);
	RemoveIcon(m_pToggledSelectedIcon);
	RemoveIcon(m_pToggledHoverIcon);
	RemoveIcon(m_pToggledDisabledIcon);

	m_pDefaultIcon = new DirectDrawRectangle(pRenderer);
	m_pHoverIcon = new DirectDrawRectangle(pRenderer);
	m_pSelectedIcon = new DirectDrawRectangle(pRenderer);
	m_pDisabledIcon = new DirectDrawRectangle(pRenderer);
	m_pToggledIcon = new DirectDrawRectangle(pRenderer);
	m_pToggledSelectedIcon = new DirectDrawRectangle(pRenderer);
	m_pToggledHoverIcon = new DirectDrawRectangle(pRenderer);
	m_pToggledDisabledIcon = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pDefaultIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pHoverIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pSelectedIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pDisabledIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pToggledIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(1.0f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(1.0f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(1.0f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(1.0f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pToggledSelectedIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pToggledHoverIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pToggledDisabledIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	Add(m_pDefaultIcon);
	Add(m_pSelectedIcon);
	Add(m_pHoverIcon);
	Add(m_pDisabledIcon);
	Add(m_pToggledIcon);
	Add(m_pToggledSelectedIcon);
	Add(m_pToggledHoverIcon);
	Add(m_pToggledDisabledIcon);

	// Set the dimensions of the icons
	Dimensions d = GetDimensions();
	m_pDefaultIcon->SetDimensions(0, 0, d.height, d.height);
	m_pSelectedIcon->SetDimensions(0, 0, d.height, d.height);
	m_pHoverIcon->SetDimensions(0, 0, d.height, d.height);
	m_pDisabledIcon->SetDimensions(0, 0, d.height, d.height);
	m_pToggledIcon->SetDimensions(0, 0, d.height, d.height);
	m_pToggledSelectedIcon->SetDimensions(0, 0, d.height, d.height);
	m_pToggledHoverIcon->SetDimensions(0, 0, d.height, d.height);
	m_pToggledDisabledIcon->SetDimensions(0, 0, d.height, d.height);
}

void OptionBox::SetToggledIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pToggledIcon)
	{
		// If we already own a toggled icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pToggledIcon);

		delete m_pToggledIcon;
		m_pToggledIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pToggledIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pToggledIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pToggledIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pToggledIcon);
}

void OptionBox::SetToggledSelectedIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pToggledSelectedIcon)
	{
		// If we already own a toggled selected icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pToggledSelectedIcon);

		delete m_pToggledSelectedIcon;
		m_pToggledSelectedIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pToggledSelectedIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pToggledSelectedIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pToggledSelectedIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pToggledSelectedIcon);
}

void OptionBox::SetToggledHoverIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pToggledHoverIcon)
	{
		// If we already own a toggled hover icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pToggledHoverIcon);

		delete m_pToggledHoverIcon;
		m_pToggledHoverIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pToggledHoverIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pToggledHoverIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pToggledHoverIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pToggledHoverIcon);
}

void OptionBox::SetToggledDisabledIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pToggledDisabledIcon)
	{
		// If we already own a toggled disabled icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pToggledDisabledIcon);

		delete m_pToggledDisabledIcon;
		m_pToggledDisabledIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pToggledDisabledIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pToggledDisabledIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pToggledDisabledIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pToggledDisabledIcon);
}

void OptionBox::SetDimensions(int x, int y, int width, int height)
{
	int iconLabelSpace = 4;

	// Also need to include the option box label as part of the mouse area (and the space between icon and text)
	int labelWidth = 0;
	if (m_displayLabel)
	{
		labelWidth = iconLabelSpace + m_pRenderer->GetFreeTypeTextWidth(m_label.GetFontID(), "%s", m_label.GetText().c_str());
	}

	Component::SetDimensions(x, y, width + labelWidth, height);

	// Also set the dimensions of the icons
	m_pDefaultIcon->SetDimensions(0, 0, width, height);
	m_pSelectedIcon->SetDimensions(0, 0, width, height);
	m_pHoverIcon->SetDimensions(0, 0, width, height);
	m_pDisabledIcon->SetDimensions(0, 0, width, height);
	m_pToggledIcon->SetDimensions(0, 0, width, height);
	m_pToggledSelectedIcon->SetDimensions(0, 0, width, height);
	m_pToggledHoverIcon->SetDimensions(0, 0, width, height);
	m_pToggledDisabledIcon->SetDimensions(0, 0, width, height);

	// Set the position on the label to be next to the icons horizontally but centered vertically
	int buttonLabelHeight = m_pRenderer->GetFreeTypeTextHeight(m_label.GetFontID(), "%s", m_label.GetText().c_str());
	int centerHeight = static_cast<int>((static_cast<float>(height) / 2.0f) - (static_cast<float>(buttonLabelHeight) / 2.0f));
	m_label.SetLocation(width + iconLabelSpace, centerHeight);

	m_isOffsetApplied = false;
}

void OptionBox::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void OptionBox::SetLabelOffset(int x, int y)
{
	m_labelOffsetX = x;
	m_labelOffsetY = y;

	m_label.SetLocation(m_label.GetLocation().x + m_labelOffsetX, m_label.GetLocation().y + m_labelOffsetY);
}

void OptionBox::SetLabelPosition(int x, int y)
{
	m_label.SetLocation(x, y);
}

void OptionBox::SetPressedOffset(int x, int y)
{
	m_pressedOffsetX = x;
	m_pressedOffsetY = y;
}

void OptionBox::SetLabelColor(Color color)
{
	m_label.SetColor(color);
}

void OptionBox::SetNormalLabelColor(Color normal)
{
	m_normalLabelColor = normal;

	m_changeLabelText = true;
}

void OptionBox::SetHoverLabelColor(Color hover)
{
	m_hoverLabelColor = hover;

	m_changeLabelText = true;
}

void OptionBox::SetPressedLabelColor(Color pressed)
{
	m_PressedLabelColor = pressed;

	m_changeLabelText = true;
}

bool OptionBox::GetToggled() const
{
	return m_isToggled;
}

void OptionBox::SetToggled(bool toggle)
{
	if (toggle)
	{
		OptionController* pParentController = static_cast<OptionController*>(GetParent());

		pParentController->ResetAllOptions();
		pParentController->SetActiveOption(this);
	}

	m_isToggled = toggle;
}

ComponentType OptionBox::GetComponentType() const
{
	return ComponentType::OptionBox;
}

void OptionBox::MouseEntered(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetHover(true);

	OnMouseEnter();

	if (m_changeLabelText)
	{
		m_label.SetColor(m_hoverLabelColor);
	}
}

void OptionBox::MouseExited(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetHover(false);

	// If we are selected when we exit, reposition back the offset
	if (IsSelected() && m_isOffsetApplied)
	{
		m_label.SetLocation(m_label.GetLocation().x - m_pressedOffsetX, m_label.GetLocation().y - m_pressedOffsetY);

		m_isOffsetApplied = false;
	}

	// Also removed the button selection if we exit it's dimensions
	SetSelected(false);

	OnMouseExit();

	if (m_changeLabelText)
	{
		m_label.SetColor(m_normalLabelColor);
	}
}

void OptionBox::MousePressed(const MouseEvent& event)
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

	if (m_isOffsetApplied == false)
	{
		m_label.SetLocation(m_label.GetLocation().x + m_pressedOffsetX, m_label.GetLocation().y + m_pressedOffsetY);
		m_isOffsetApplied = true;
	}

	if (m_changeLabelText)
	{
		m_label.SetColor(m_PressedLabelColor);
	}
}

void OptionBox::MouseReleased(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetSelected(false);

	OnMouseReleased();

	if (m_isOffsetApplied)
	{
		m_label.SetLocation(m_label.GetLocation().x - m_pressedOffsetX, m_label.GetLocation().y - m_pressedOffsetY);

		m_isOffsetApplied = false;
	}

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
}

void OptionBox::MouseClicked(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetToggled(true);

	m_isToggled = true;

	OnMouseClicked();

	// Call the callback function callback if this class is just a simple button
	if (m_callback)
	{
		m_callback(m_pCallbackData);
	}
}

void OptionBox::OnMouseEnter()
{
	// Do nothing
}

void OptionBox::OnMouseExit()
{
	// Do nothing
}

void OptionBox::OnMousePressed()
{
	// Do nothing
}

void OptionBox::OnMouseReleased()
{
	// Do nothing
}

void OptionBox::OnMouseClicked()
{
	// Do nothing
}

void OptionBox::DrawSelf()
{
	//Container::DrawSelf();

	// Reset visibility
	m_pSelectedIcon->SetVisible(false);
	m_pHoverIcon->SetVisible(false);
	m_pDisabledIcon->SetVisible(false);
	m_pDefaultIcon->SetVisible(false);
	m_pToggledIcon->SetVisible(false);
	m_pToggledSelectedIcon->SetVisible(false);
	m_pToggledHoverIcon->SetVisible(false);
	m_pToggledDisabledIcon->SetVisible(false);

	if (IsSelected())
	{
		if (m_isToggled)
		{
			m_pToggledSelectedIcon->SetVisible(true);
		}
		else
		{
			m_pSelectedIcon->SetVisible(true);
		}
	}
	else if (IsHover())
	{
		if (m_isToggled)
		{
			m_pToggledHoverIcon->SetVisible(true);
		}
		else
		{
			m_pHoverIcon->SetVisible(true);
		}
	}
	else if (IsDisabled())
	{
		if (m_isToggled)
		{
			m_pToggledDisabledIcon->SetVisible(true);
		}
		else
		{
			m_pDisabledIcon->SetVisible(true);
		}
	}
	else
	{
		if (m_isToggled)
		{
			m_pToggledIcon->SetVisible(true);
		}
		else
		{
			m_pDefaultIcon->SetVisible(true);
		}
	}

	// Set the label visibility
	m_label.SetVisible(m_displayLabel);
}