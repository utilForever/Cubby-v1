/*************************************************************************
> File Name: AbstractButton.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    An abstract button class used as a base
>    class for all button related components.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DirectDrawRectangle.h"
#include "Icon.h"
#include "MultiTextureIcon.h"

#include "AbstractButton.h"

// Constructor, Destructor
AbstractButton::AbstractButton(Renderer* pRenderer, unsigned int GUIFont, const std::string& label) :
	Container(pRenderer), m_pDefaultIcon(nullptr), m_pSelectedIcon(nullptr), m_pHoverIcon(nullptr), m_pDisabledIcon(nullptr),
	m_label(pRenderer, GUIFont, label, Color(1.0f, 1.0f, 1.0f)),
	m_selected(false), m_hover(false), m_disabled(false)
{
	m_displayLabel = true;

	m_callback = nullptr;
	m_pCallbackData = nullptr;

	m_callbackPressed = nullptr;
	m_pCallbackDataPressed = nullptr;

	m_callbackReleased = nullptr;
	m_pCallbackDataReleased = nullptr;

	m_EnterCallback = nullptr;
	m_pEnterCallbackData = nullptr;

	m_ExitCallback = nullptr;
	m_pExitCallbackData = nullptr;

	m_labelOffsetX = 0;
	m_labelOffsetY = 0;
}

AbstractButton::~AbstractButton()
{

}

void AbstractButton::SetLabel(const std::string& label)
{
	m_label.SetText(label);

	// Set the label location to be the center of the button
	int buttonLabelHeight = m_pRenderer->GetFreeTypeTextHeight(m_label.GetFontID(), "%s", m_label.GetText().c_str());
	int buttonLabelWidth = m_pRenderer->GetFreeTypeTextWidth(m_label.GetFontID(), "%s", m_label.GetText().c_str());
	int centerWidth = static_cast<int>((m_dimensions.width * 0.5f) - (buttonLabelWidth * 0.5f));
	int centerHeight = static_cast<int>((m_dimensions.height * 0.5f) - (buttonLabelHeight * 0.5f));
	
	m_label.SetLocation(centerWidth + m_labelOffsetX, centerHeight + m_labelOffsetY);
}


void AbstractButton::SetDisplayLabel(bool display)
{
	m_displayLabel = display;
}

void AbstractButton::RemoveIcon(RenderRectangle* pRemoveIcon)
{
	if (pRemoveIcon)
	{
		Remove(pRemoveIcon);
		delete pRemoveIcon;
	}
}

void AbstractButton::RemoveIcon(const std::string& fileName)
{
	bool removed = false;

	for (int i = 0; i < GetNumberOfChildren() && removed == false; ++i)
	{
		if (GetChild(i)->GetComponentType() == ComponentType::Icon)
		{
			Icon* pIcon = static_cast<Icon*>(GetChild(i));
			if (pIcon->GetFileName() == fileName)
			{
				Remove(pIcon);

				removed = true;
			}
		}
	}
}

void AbstractButton::SetDefaultIcons(Renderer* pRenderer)
{
	RemoveIcon(m_pDefaultIcon);
	RemoveIcon(m_pHoverIcon);
	RemoveIcon(m_pSelectedIcon);
	RemoveIcon(m_pDisabledIcon);

	m_pDefaultIcon = new DirectDrawRectangle(pRenderer);
	m_pHoverIcon = new DirectDrawRectangle(pRenderer);
	m_pSelectedIcon = new DirectDrawRectangle(pRenderer);
	m_pDisabledIcon = new DirectDrawRectangle(pRenderer);

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

	Add(m_pDefaultIcon);
	Add(m_pSelectedIcon);
	Add(m_pHoverIcon);
	Add(m_pDisabledIcon);

	// Set the dimensions of the icons
	Dimensions d = GetDimensions();
	m_pDefaultIcon->SetDimensions(0, 0, d.width, d.height);
	m_pSelectedIcon->SetDimensions(0, 0, d.width, d.height);
	m_pHoverIcon->SetDimensions(0, 0, d.width, d.height);
	m_pDisabledIcon->SetDimensions(0, 0, d.width, d.height);
}

void AbstractButton::SetDefaultIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pDefaultIcon)
	{
		// If we already own a default icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pDefaultIcon);

		delete m_pDefaultIcon;
		m_pDefaultIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pDefaultIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pDefaultIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pDefaultIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pDefaultIcon);
}

void AbstractButton::SetSelectedIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pSelectedIcon)
	{
		// If we already own a selected icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pSelectedIcon);

		delete m_pSelectedIcon;
		m_pSelectedIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then create a new copy of it
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pSelectedIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pSelectedIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pSelectedIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pSelectedIcon);
}

void AbstractButton::SetHoverIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pHoverIcon)
	{
		// If we already own a hover icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pHoverIcon);

		delete m_pHoverIcon;
		m_pHoverIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pHoverIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pHoverIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pHoverIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pHoverIcon);
}

void AbstractButton::SetDisabledIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pDisabledIcon)
	{
		// If we already own a disabled icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pDisabledIcon);

		delete m_pDisabledIcon;
		m_pDisabledIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pDisabledIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pDisabledIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pDisabledIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pDisabledIcon);
}

Label AbstractButton::GetLabel() const
{
	return m_label;
}

void AbstractButton::SetSelected(bool val)
{
	m_selected = val;
}

void AbstractButton::SetHover(bool val)
{
	m_hover = val;
}

void AbstractButton::SetDisabled(bool val)
{
	m_disabled = val;

	if (m_disabled)
	{
		SetSelected(false);
		SetHover(false);
	}
}

bool AbstractButton::IsSelected() const
{
	return m_selected;
}

bool AbstractButton::IsHover() const
{
	return m_hover;
}

bool AbstractButton::IsDisabled() const
{
	return m_disabled;
}

void AbstractButton::SetCallBackFunction(std::function<void(void*)> func)
{
	m_callback = func;
}

void AbstractButton::SetCallBackData(void* pData)
{
	m_pCallbackData = pData;
}

void AbstractButton::SetCallBackFunctionPressed(std::function<void(void*)> func)
{
	m_callbackPressed = func;
}

void AbstractButton::SetCallBackDataPressed(void* pData)
{
	m_pCallbackDataPressed = pData;
}

void AbstractButton::SetCallBackFunctionReleased(std::function<void(void*)> func)
{
	m_callbackReleased = func;
}

void AbstractButton::SetCallBackDataReleased(void* pData)
{
	m_pCallbackDataReleased = pData;
}


void AbstractButton::SetEnterCallBackFunction(std::function<void(void*)> func)
{
	m_EnterCallback = func;
}

void AbstractButton::SetEnterCallBackData(void* pData)
{
	m_pEnterCallbackData = pData;
}

void AbstractButton::SetExitCallBackFunction(std::function<void(void*)> func)
{
	m_ExitCallback = func;
}

void AbstractButton::SetExitCallBackData(void* pData)
{
	m_pExitCallbackData = pData;
}

ComponentType AbstractButton::GetComponentType() const
{
	return ComponentType::AbstractButton;
}