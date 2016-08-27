/*************************************************************************
> File Name: Slider.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A slider GUI component to allow quick selection of range values.
> Created Time: 2016/08/27
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DirectDrawRectangle.h"
#include "MultiTextureIcon.h"
#include "Slider.h"
#include "Icon.h"
#include "GUIWindow.h"

// Constructor, Destructor
Slider::Slider(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, float min, float max, float defaultValue, bool addIncrementButtons, float incrementAmount) :
	Container(pRenderer),
	m_value(pRenderer, GUIFont, "", Color(0.0f, 0.0f, 0.0f)),
	m_name(pRenderer, GUIFont, label, Color(0.0f, 0.0f, 0.0f)),
	m_minValue(min), m_maxValue(max), m_defaultValue(defaultValue),
	m_sliderDirection(SliderDirection::Horizontal), m_fontID(GUIFont)
{
	m_pScrollerDefault = new DirectDrawRectangle(pRenderer);
	m_pScrollerHover = new DirectDrawRectangle(pRenderer);
	m_pScrollerDragging = new DirectDrawRectangle(pRenderer);
	m_pScrollerDisabled = new DirectDrawRectangle(pRenderer);
	m_pScrollBackbarDefault = new DirectDrawRectangle(pRenderer);
	m_pScrollBackbarPressed = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollerDefault);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollerHover);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollerDragging);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollerDisabled);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollBackbarDefault);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollBackbarPressed);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.5f, 0.5f, 0.5f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.5f, 0.5f, 0.5f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.5f, 0.5f, 0.5f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.5f, 0.5f, 0.5f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.05f, 0.05f, 0.05f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.05f, 0.05f, 0.05f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.05f, 0.05f, 0.05f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.05f, 0.05f, 0.05f, 1.0f));

	m_pPlusButton = new Button(pRenderer, GUIFont, "+");
	m_pPlusButton->SetCallBackFunction(_PlusPressed);
	m_pPlusButton->SetCallBackData(this);
	m_pPlusButton->SetLabelOffset(0, 0);
	m_pMinusButton = new Button(pRenderer, GUIFont, "-");
	m_pMinusButton->SetCallBackFunction(_MinusPressed);
	m_pMinusButton->SetCallBackData(this);
	m_pMinusButton->SetLabelOffset(1, 2);

	m_pPlusButton->SetDepth(2.0f);
	m_pMinusButton->SetDepth(2.0f);

	m_currentValue = defaultValue;

	m_isShowValue = true;
	m_valuePrecision = 2;

	m_isDragging = false;
	m_isAllowDragging = true;
	m_isHover = false;
	m_isDragginOutside = false;
	m_isFirstOutsideDrag = false;
	m_isAllowDragginOutside = false;
	m_isPressedBar = false;
	m_isAllowReleasingOnBar = true;
	m_isOverBar = false;

	m_isRenderBackgroundBar = true;

	m_isDynamicColorBackground = false;

	m_lastX = 0;
	m_lastY = 0;

	m_isDisabled = false;

	Container::Add(m_pScrollerDefault);
	Container::Add(m_pScrollerHover);
	Container::Add(m_pScrollerDragging);
	Container::Add(m_pScrollerDisabled);
	Container::Add(m_pScrollBackbarDefault);
	Container::Add(m_pScrollBackbarPressed);

	m_labelOffsetX = 0;
	m_labelOffsetY = 0;
	m_valueOffsetX = 0;
	m_valueOffsetY = 0;
	m_incrementButtonsOffsetX = 0;
	m_incrementButtonsOffsetY = 0;

	m_incrementButtons = addIncrementButtons;
	m_incrementAmount = incrementAmount;

	if (addIncrementButtons)
	{
		Container::Add(m_pPlusButton);
		Container::Add(m_pMinusButton);
	}

	Container::Add(&m_value);
	Container::Add(&m_name);

	m_valueChangedCallback = nullptr;
	m_pValueChangedCallbackData = nullptr;

	ValueChanged();
}

Slider::Slider(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, float min, float max, float defaultValue, Color fontColor, Color outlineColor, bool addIncrementButtons, float incrementAmount) :
	Container(pRenderer),
	m_value(pRenderer, GUIFont, "", Color(0.0f, 0.0f, 0.0f)),
	m_name(pRenderer, GUIFont, label, Color(0.0f, 0.0f, 0.0f)),
	m_minValue(min), m_maxValue(max), m_defaultValue(defaultValue),
	m_sliderDirection(SliderDirection::Horizontal), m_fontID(GUIFont)
{
	m_pScrollerDefault = new DirectDrawRectangle(pRenderer);
	m_pScrollerHover = new DirectDrawRectangle(pRenderer);
	m_pScrollerDragging = new DirectDrawRectangle(pRenderer);
	m_pScrollerDisabled = new DirectDrawRectangle(pRenderer);
	m_pScrollBackbarDefault = new DirectDrawRectangle(pRenderer);
	m_pScrollBackbarPressed = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollerDefault);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollerHover);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollerDragging);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollerDisabled);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollBackbarDefault);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollBackbarPressed);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.5f, 0.5f, 0.5f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.5f, 0.5f, 0.5f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.5f, 0.5f, 0.5f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.5f, 0.5f, 0.5f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.05f, 0.05f, 0.05f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.05f, 0.05f, 0.05f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.05f, 0.05f, 0.05f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.05f, 0.05f, 0.05f, 1.0f));

	m_pPlusButton = new Button(pRenderer, GUIFont, outlineGUIFont, "+", fontColor, outlineColor);
	m_pPlusButton->SetCallBackFunction(_PlusPressed);
	m_pPlusButton->SetCallBackData(this);
	m_pPlusButton->SetLabelOffset(0, 0);
	m_pMinusButton = new Button(pRenderer, GUIFont, outlineGUIFont, "-", fontColor, outlineColor);
	m_pMinusButton->SetCallBackFunction(_MinusPressed);
	m_pMinusButton->SetCallBackData(this);
	m_pMinusButton->SetLabelOffset(1, 2);

	m_pPlusButton->SetDepth(2.0f);
	m_pMinusButton->SetDepth(2.0f);

	m_currentValue = defaultValue;

	m_isShowValue = true;
	m_valuePrecision = 2;

	m_isDragging = false;
	m_isAllowDragging = true;
	m_isHover = false;
	m_isDragginOutside = false;
	m_isFirstOutsideDrag = false;
	m_isAllowDragginOutside = false;
	m_isPressedBar = false;
	m_isAllowReleasingOnBar = true;
	m_isOverBar = false;

	m_isRenderBackgroundBar = true;

	m_isDynamicColorBackground = false;

	m_lastX = 0;
	m_lastY = 0;

	m_isDisabled = false;

	Container::Add(m_pScrollerDefault);
	Container::Add(m_pScrollerHover);
	Container::Add(m_pScrollerDragging);
	Container::Add(m_pScrollerDisabled);
	Container::Add(m_pScrollBackbarDefault);
	Container::Add(m_pScrollBackbarPressed);

	m_labelOffsetX = 0;
	m_labelOffsetY = 0;
	m_valueOffsetX = 0;
	m_valueOffsetY = 0;
	m_incrementButtonsOffsetX = 0;
	m_incrementButtonsOffsetY = 0;

	m_incrementButtons = addIncrementButtons;
	m_incrementAmount = incrementAmount;

	if (addIncrementButtons)
	{
		Container::Add(m_pPlusButton);
		Container::Add(m_pMinusButton);
	}

	m_value.SetLocation(0, 0);
	m_value.SetColor(fontColor);
	m_value.SetOutline(true);
	m_value.SetOutlineColor(outlineColor);
	m_value.SetOutlineFont(outlineGUIFont);
	Container::Add(&m_value);

	m_name.SetLocation(0, 0);
	m_name.SetColor(fontColor);
	m_name.SetOutline(true);
	m_name.SetOutlineColor(outlineColor);
	m_name.SetOutlineFont(outlineGUIFont);
	Container::Add(&m_name);

	m_valueChangedCallback = nullptr;
	m_pValueChangedCallbackData = nullptr;

	ValueChanged();
}

Slider::~Slider()
{
	delete m_pScrollerDefault;
	delete m_pScrollerHover;
	delete m_pScrollerDragging;
	delete m_pScrollerDisabled;
	delete m_pScrollBackbarDefault;
	delete m_pScrollBackbarPressed;

	delete m_pPlusButton;
	delete m_pMinusButton;
}

void Slider::AddEventListeners()
{
	Container::AddEventListeners();

	AddMouseListener(this);
}

void Slider::RemoveEventListeners()
{
	Container::RemoveEventListeners();

	RemoveMouseListener(this);
}

void Slider::SetScrollerDefaultIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pScrollerDefault)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pScrollerDefault);

		delete m_pScrollerDefault;
		m_pScrollerDefault = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pScrollerDefault = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pScrollerDefault = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pScrollerDefault);

	ValueChanged(true);
}

void Slider::SetScrollerHoverIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pScrollerHover)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pScrollerHover);

		delete m_pScrollerHover;
		m_pScrollerHover = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pScrollerHover = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pScrollerHover = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pScrollerHover);

	ValueChanged(true);
}

void Slider::SetScrollerDraggingIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pScrollerDragging)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pScrollerDragging);

		delete m_pScrollerDragging;
		m_pScrollerDragging = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pScrollerDragging = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pScrollerDragging = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pScrollerDragging);

	ValueChanged(true);
}

void Slider::SetScrollerDisabledIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pScrollerDisabled)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pScrollerDisabled);

		delete m_pScrollerDisabled;
		m_pScrollerDisabled = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pScrollerDisabled = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pScrollerDisabled = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pScrollerDisabled);

	ValueChanged(true);
}

void Slider::SetScrollBackbarDefault(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pScrollBackbarDefault)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pScrollBackbarDefault);

		delete m_pScrollBackbarDefault;
		m_pScrollBackbarDefault = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pScrollBackbarDefault = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pScrollBackbarDefault = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pScrollBackbarDefault);

	ValueChanged(true);
}

void Slider::SetScrollBackbarPressed(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pScrollBackbarPressed)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pScrollBackbarPressed);

		delete m_pScrollBackbarPressed;
		m_pScrollBackbarPressed = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pScrollBackbarPressed = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pScrollBackbarPressed = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pScrollBackbarPressed);

	ValueChanged(true);
}

void Slider::SetScrollBackbarIncrementIconDefault(RenderRectangle* icon) const
{
	m_pPlusButton->SetDefaultIcon(icon);
}

void Slider::SetScrollBackbarIncrementIconHover(RenderRectangle* icon) const
{
	m_pPlusButton->SetHoverIcon(icon);
}

void Slider::SetScrollBackbarIncrementIconPressed(RenderRectangle* icon) const
{
	m_pPlusButton->SetSelectedIcon(icon);
}

void Slider::SetScrollBackbarIncrementIconDisabled(RenderRectangle* icon) const
{
	m_pPlusButton->SetDisabledIcon(icon);
}

void Slider::SetScrollBackbarDecrementIconDefault(RenderRectangle* icon) const
{
	m_pMinusButton->SetDefaultIcon(icon);
}

void Slider::SetScrollBackbarDecrementIconHover(RenderRectangle* icon) const
{
	m_pMinusButton->SetHoverIcon(icon);
}

void Slider::SetScrollBackbarDecrementIconPressed(RenderRectangle* icon) const
{
	m_pMinusButton->SetSelectedIcon(icon);
}

void Slider::SetScrollBackbarDecrementIconDisabled(RenderRectangle* icon) const
{
	m_pMinusButton->SetDisabledIcon(icon);
}

void Slider::SetDimensions(int x, int y, int width, int height)
{
	int labelSpace = 4;

	Component::SetDimensions(x, y, width, height);

	// Set the position on the value label to be next to the slider horizontally but centered vertically
	int valueLabelHeight = m_pRenderer->GetFreeTypeTextHeight(m_fontID, "%s", m_value.GetText().c_str());
	int centerHeight = (height / 2) - (valueLabelHeight / 2);
	m_value.SetLocation(width + labelSpace + m_valueOffsetX, centerHeight + m_valueOffsetY);

	int nameSpace = 5;
	int nameLabelWidth = m_pRenderer->GetFreeTypeTextWidth(m_fontID, "%s", m_name.GetText().c_str());
	int nameLabelHeight = m_pRenderer->GetFreeTypeTextHeight(m_fontID, "%s", m_name.GetText().c_str());
	centerHeight = (height / 2) - (nameLabelHeight / 2);
	m_name.SetLocation(-nameLabelWidth - nameSpace + m_valueOffsetX, centerHeight + m_valueOffsetY);

	if (m_sliderDirection == SliderDirection::Horizontal)
	{
		int halfHeight = height / 2;
		m_pPlusButton->SetDimensions(width + labelSpace + 35 + m_incrementButtonsOffsetX, halfHeight + m_incrementButtonsOffsetY, halfHeight, halfHeight);
		m_pMinusButton->SetDimensions(width + labelSpace + 35 + m_incrementButtonsOffsetX, 0 + m_incrementButtonsOffsetY, halfHeight, halfHeight);
	}
	else
	{
		// TODO: Plus and Minus increment buttons when a vertical slider
	}
}

void Slider::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void Slider::SetShowValue(bool show)
{
	m_isShowValue = show;
}

void Slider::SetValuePrecision(int precision)
{
	m_valuePrecision = precision;

	UpdateTextString();
}

void Slider::SetValuePositionOffset(int offsetX, int offsetY)
{
	m_valueOffsetX = offsetX;
	m_valueOffsetY = offsetY;
}

void Slider::SetLabelPositionOffset(int offsetX, int offsetY)
{
	m_labelOffsetX = offsetX;
	m_labelOffsetY = offsetY;
}

void Slider::SetIncrementButtonsOffset(int offsetX, int offsetY)
{
	m_incrementButtonsOffsetX = offsetX;
	m_incrementButtonsOffsetY = offsetY;

	int labelSpace = 4;
	int width = GetDimensions().width;
	int height = GetDimensions().height;

	if (m_sliderDirection == SliderDirection::Horizontal)
	{
		int halfHeight = height / 2;
		m_pPlusButton->SetDimensions(width + labelSpace + 35 + m_incrementButtonsOffsetX, halfHeight + m_incrementButtonsOffsetY, halfHeight, halfHeight);
		m_pMinusButton->SetDimensions(width + labelSpace + 35 + m_incrementButtonsOffsetX, 0 + m_incrementButtonsOffsetY, halfHeight, halfHeight);
	}
	else
	{
		// TODO: Plus and Minus increment buttons when a vertical slider
	}
}

void Slider::UpdateTextString()
{
	// Update the text string
	char textStr[16];

	switch (m_valuePrecision)
	{
	case 0: sprintf(textStr, "%.0f", m_currentValue); break;
	case 1: sprintf(textStr, "%.1f", m_currentValue); break;
	case 2: sprintf(textStr, "%.2f", m_currentValue); break;
	case 3: sprintf(textStr, "%.3f", m_currentValue); break;
	default: sprintf(textStr, "%.2f", m_currentValue); break;
	};

	m_value.SetText(textStr);
}

void Slider::SetSliderParams(int barHeight, int sliderWidth, SliderDirection direction)
{
	m_barHeight = barHeight;
	m_sliderWidth = sliderWidth;
	m_sliderDirection = direction;

	ValueChanged();
}

float Slider::GetMinValue() const
{
	return m_minValue;
}

float Slider::GetMaxValue() const
{
	return m_maxValue;
}

float Slider::GetDefaultValue() const
{
	return m_defaultValue;
}

float Slider::GetCurrentValue() const
{
	return m_currentValue;
}

void Slider::SetMinValue(float minValue)
{
	m_minValue = minValue;
}

void Slider::SetMaxValue(float maxValue)
{
	m_maxValue = maxValue;
}

void Slider::SetDefaultValue(float defaultValue)
{
	m_defaultValue = defaultValue;
}

void Slider::SetCurrentValue(float currentValue, bool supressCallback)
{
	m_currentValue = currentValue;

	ValueChanged(supressCallback);
}

void Slider::SetRenderBackgroundBar(bool render)
{
	m_isRenderBackgroundBar = render;
}

void Slider::SetDynamicBackgroundColor(Color topLeft, Color bottomLeft, Color topRight, Color bottomRight)
{
	m_isDynamicColorBackground = true;
	m_backgroundColorTopLeft = topLeft;
	m_backgroundColorTopRight = topRight;
	m_backgroundColorBottomLeft = bottomLeft;
	m_backgroundColorBottomRight = bottomRight;
}

SliderDirection Slider::GetSliderDirection() const
{
	return m_sliderDirection;
}

void Slider::SetSliderDirection(SliderDirection direction)
{
	m_sliderDirection = direction;
}

void Slider::SetDisabled(bool val)
{
	m_isDisabled = val;

	SetEnabled(m_isDisabled == false);

	m_pPlusButton->SetDisabled(val);
	m_pMinusButton->SetDisabled(val);
}

ComponentType Slider::GetComponentType() const
{
	return ComponentType::Slider;
}

void Slider::SetVaueChangedCallBackFunction(FunctionCallback func)
{
	m_valueChangedCallback = func;
}

void Slider::SetVaueChangedCallBackData(void* pData)
{
	m_pValueChangedCallbackData = pData;
}

void Slider::MouseEntered(const MouseEvent& event)
{
	m_isDragginOutside = false;
	m_isFirstOutsideDrag = false;

	OnMouseEnter();
}

void Slider::MouseExited(const MouseEvent& event)
{
	m_isHover = false;
	m_isOverBar = false;
	m_isDragginOutside = true;

	OnMouseExit();
}

void Slider::MousePressed(const MouseEvent& event)
{
	int sliderX;
	int sliderY;
	int sliderWidth;
	int sliderHeight;

	int barX;
	int barY;
	int barWidth;
	int barHeight;

	if (m_sliderDirection == SliderDirection::Horizontal)
	{
		sliderX = GetLocationOnScreen().x + static_cast<int>((m_dimensions.width - m_sliderWidth) * ((m_currentValue - m_minValue) / (m_maxValue - m_minValue)));
		sliderY = GetLocationOnScreen().y;
		sliderWidth = m_sliderWidth;
		sliderHeight = m_dimensions.height;

		barX = GetLocationOnScreen().x + (m_sliderWidth / 2);
		barY = GetLocationOnScreen().y + (m_dimensions.height / 2) - (m_barHeight / 2);
		barWidth = m_dimensions.width - m_sliderWidth;
		barHeight = m_barHeight;
	}
	else
	{
		sliderX = GetLocationOnScreen().x;
		sliderY = GetLocationOnScreen().y + static_cast<int>((m_dimensions.height - m_sliderWidth) * ((m_currentValue - m_minValue) / (m_maxValue - m_minValue)));
		sliderWidth = m_dimensions.width;
		sliderHeight = m_sliderWidth;

		barX = GetLocationOnScreen().x + (m_dimensions.width / 2) - (m_barHeight / 2);
		barY = GetLocationOnScreen().y + (m_sliderWidth / 2);
		barWidth = m_barHeight;
		barHeight = m_dimensions.height - m_sliderWidth;
	}

	int mouseX = event.GetX();
	int mouseY = event.GetY();

	// Check to see if we have clicked the slider to start dragging
	if ((mouseX >= sliderX) && (mouseX < sliderX + sliderWidth) && (mouseY >= sliderY) && (mouseY < sliderY + sliderHeight))
	{
		if (m_isAllowDragging)
		{
			m_isDragging = true;
			m_isAllowDragginOutside = true;
		}
	}
	else
	{
		// Check to see if we have clicked on the bar to 'zoom' to a location
		if ((mouseX >= barX) && (mouseX < barX + barWidth) && (mouseY >= barY) && (mouseY < barY + barHeight))
		{
			if (m_isAllowReleasingOnBar && !m_isDragging)
			{
				m_isPressedBar = true;
			}
		}
		else
		{
			// Don't allow releasing on the bar since we didn't first click on the bar, have to wait for release now
			m_isAllowReleasingOnBar = false;
		}

		// Don't allow dragging, or dragging outside, since our first press wasn't on the slider, have to wait for release now
		if (!m_isDragging)
		{
			m_isAllowDragging = false;
			m_isAllowDragginOutside = false;
		}
	}

	// If our parent is a GUIWindow, then make this window have focus in the GUI, used to make it's depth the highest
	if (GetParent() != nullptr && GetParent()->GetComponentType() == ComponentType::GUIWindow)
	{
		GUIWindow* pParentWindow = static_cast<GUIWindow*>(GetParent());
		pParentWindow->SetFocusWindow();
	}

	OnMousePressed();
}

void Slider::MouseReleased(const MouseEvent& event)
{
	if (!m_isDragging && m_isPressedBar)
	{
		int barX;
		int barY;
		int barWidth;
		int barHeight;

		if (m_sliderDirection == SliderDirection::Horizontal)
		{
			barX = GetLocationOnScreen().x + (m_sliderWidth / 2);
			barY = GetLocationOnScreen().y + (m_dimensions.height / 2) - (m_barHeight / 2);
			barWidth = m_dimensions.width - m_sliderWidth;
			barHeight = m_barHeight;
		}
		else
		{
			barX = GetLocationOnScreen().x + (m_dimensions.width / 2) - (m_barHeight / 2);
			barY = GetLocationOnScreen().y + (m_sliderWidth / 2);
			barWidth = m_barHeight;
			barHeight = m_dimensions.height - m_sliderWidth;
		}

		int mouseX = event.GetX();
		int mouseY = event.GetY();

		// Store the value before we change it, to see if we have actually changed the value
		float valueBefore = m_currentValue;

		if ((mouseX >= barX) && (mouseX < barX + barWidth) && (mouseY >= barY) && (mouseY < barY + barHeight))
		{
			// Check to see if we have clicked on the bar to 'zoom' to a location

			if (m_sliderDirection == SliderDirection::Horizontal)
			{
				int relativeX = mouseX - GetLocationOnScreen().x;
				float ratio = static_cast<float>(relativeX) / static_cast<float>(barWidth);
				m_currentValue = m_minValue + ((m_maxValue - m_minValue) * ratio);
			}
			else
			{
				int relativeY = mouseY - GetLocationOnScreen().y;
				float ratio = static_cast<float>(relativeY) / static_cast<float>(barWidth);
				m_currentValue = m_minValue + ((m_maxValue - m_minValue) * ratio);
			}
		}

		if (m_currentValue != valueBefore)
		{
			ValueChanged();
		}

		m_isPressedBar = false;
	}

	m_isDragging = false;
	m_isPressedBar = false;

	// Allow dragging again now, since we have released the button, we can now check for dragging again on pressing
	m_isAllowDragging = true;

	// Allow releasing on the bar now, we can now check for this again on pressing
	m_isAllowReleasingOnBar = true;

	OnMouseReleased();
}

void Slider::MouseReleasedOutside(const MouseEvent& event)
{
	m_isDragging = false;
	m_isPressedBar = false;

	// Allow releasing on the bar now, we can now check for this again on pressing
	m_isAllowReleasingOnBar = true;
}

void Slider::MouseClicked(const MouseEvent& event)
{
	OnMouseClicked();
}

void Slider::MouseDragged(const MouseEvent& event)
{
	if (m_isDragging)
	{
		int changeX = event.GetX();
		int changeY = event.GetY();

		if (changeX == 0 && changeY == 0)
		{
			return;
		}

		float valueChange;
		if (m_sliderDirection == SliderDirection::Horizontal)
		{
			valueChange = static_cast<float>(changeX) * ((m_maxValue - m_minValue) / (static_cast<float>(m_dimensions.width) - static_cast<float>(m_sliderWidth)));
		}
		else //if(m_sliderDirection == ESliderDirection_Vertical)
		{
			valueChange = static_cast<float>(changeY) * ((m_maxValue - m_minValue) / (static_cast<float>(m_dimensions.height) - static_cast<float>(m_sliderWidth)));
		}

		// Store the value before we change it, to see if we have actually changed the value
		float valueBefore = m_currentValue;

		m_currentValue += valueChange;

		if (m_currentValue > m_maxValue)
		{
			m_currentValue = m_maxValue;
		}
		if (m_currentValue < m_minValue)
		{
			m_currentValue = m_minValue;
		}

		if (m_currentValue != valueBefore)
		{
			ValueChanged();
		}
	}
}

void Slider::MouseDraggedOutside(const MouseEvent& event)
{
	if (!m_isAllowDragginOutside)
	{
		// If we are not allowing dragging outside, just early out
		return;
	}

	if (!m_isFirstOutsideDrag)
	{
		m_lastX = event.GetX();
		m_lastY = event.GetY();
		m_isFirstOutsideDrag = true;
	}

	if (m_isDragginOutside)
	{
		int newX = event.GetX();
		int newY = event.GetY();

		int minX = GetLocationOnScreen().x;
		int minY = GetLocationOnScreen().y;
		int maxX = minX + m_dimensions.width;
		int maxY = minY + m_dimensions.height;

		int changeX = newX - m_lastX;
		int changeY = newY - m_lastY;

		if (changeX == 0 && changeY == 0)
		{
			return;
		}

		// Store the value before we change it, to see if we have actually changed the value
		float valueBefore = m_currentValue;

		float valueChange;
		if (m_sliderDirection == SliderDirection::Horizontal)
		{
			valueChange = static_cast<float>(changeX) * ((m_maxValue - m_minValue) / (static_cast<float>(m_dimensions.width) - static_cast<float>(m_sliderWidth)));

			if ((valueChange > 0) && (newX > minX))
			{
				m_currentValue += valueChange;
			}
			else if ((valueChange < 0) && (newX < maxX))
			{
				m_currentValue += valueChange;
			}
		}
		else
		{
			valueChange = static_cast<float>(changeY) * ((m_maxValue - m_minValue) / (static_cast<float>(m_dimensions.height) - static_cast<float>(m_sliderWidth)));

			if ((valueChange > 0) && (newY > minY))
			{
				m_currentValue += valueChange;
			}
			else if ((valueChange < 0) && (newY < maxY))
			{
				m_currentValue += valueChange;
			}
		}

		if (m_currentValue > m_maxValue)
		{
			m_currentValue = m_maxValue;
		}
		if (m_currentValue < m_minValue)
		{
			m_currentValue = m_minValue;
		}

		if (m_currentValue != valueBefore)
		{
			ValueChanged();
		}

		m_lastX = newX;
		m_lastY = newY;
	}
}

void Slider::MouseMotion(const MouseEvent& event)
{
	int sliderX;
	int sliderY;
	int sliderWidth;
	int sliderHeight;

	int barX;
	int barY;
	int barWidth;
	int barHeight;

	if (m_sliderDirection == SliderDirection::Horizontal)
	{
		sliderX = GetLocationOnScreen().x + static_cast<int>((m_dimensions.width - m_sliderWidth) * ((m_currentValue - m_minValue) / (m_maxValue - m_minValue)));
		sliderY = GetLocationOnScreen().y;
		sliderWidth = m_sliderWidth;
		sliderHeight = m_dimensions.height;

		barX = GetLocationOnScreen().x + (m_sliderWidth / 2);
		barY = GetLocationOnScreen().y + (m_dimensions.height / 2) - (m_barHeight / 2);
		barWidth = m_dimensions.width - m_sliderWidth;
		barHeight = m_barHeight;
	}
	else
	{
		sliderX = GetLocationOnScreen().x;
		sliderY = GetLocationOnScreen().y + static_cast<int>((m_dimensions.height - m_sliderWidth) * ((m_currentValue - m_minValue) / (m_maxValue - m_minValue)));
		sliderWidth = m_dimensions.width;
		sliderHeight = m_sliderWidth;

		barX = GetLocationOnScreen().x + (m_dimensions.width / 2) - (m_barHeight / 2);
		barY = GetLocationOnScreen().y + (m_sliderWidth / 2);
		barWidth = m_barHeight;
		barHeight = m_dimensions.height - m_sliderWidth;
	}

	int mouseX = event.GetX();
	int mouseY = event.GetY();

	// Check to see if we are over the slider
	if ((mouseX > sliderX) && (mouseX < sliderX + sliderWidth) && (mouseY > sliderY) && (mouseY < sliderY + sliderHeight))
	{
		if (!m_isPressedBar)
		{
			m_isHover = true;
		}
	}
	else
	{
		m_isHover = false;
	}

	// Check to see if we are over the bar
	if ((mouseX >= barX) && (mouseX < barX + barWidth) && (mouseY >= barY) && (mouseY < barY + barHeight))
	{
		m_isOverBar = true;
	}
	else
	{
		m_isOverBar = false;
	}
}

void Slider::OnMouseEnter()
{
	// Do nothing
}

void Slider::OnMouseExit()
{
	// Do nothing
}

void Slider::OnMousePressed()
{
	// Do nothing
}

void Slider::OnMouseReleased()
{
	// Do nothing
}

void Slider::OnMouseClicked()
{
	// Do nothing
}

void Slider::OnValueChanged()
{
	// Do nothing
}

void Slider::ValueChanged(bool supressCallback)
{
	// Update the text string
	UpdateTextString();

	// Update the slider dimensions
	int sliderX;
	int sliderY;
	int sliderWidth;
	int sliderHeight;

	int barX;
	int barY;
	int barWidth;
	int barHeight;

	if (m_sliderDirection == SliderDirection::Horizontal)
	{
		sliderX = static_cast<int>((m_dimensions.width - m_sliderWidth) * ((m_currentValue - m_minValue) / (m_maxValue - m_minValue)));
		sliderY = 0;
		sliderWidth = m_sliderWidth;
		sliderHeight = m_dimensions.height;

		barX = (m_sliderWidth / 2);
		barY = (m_dimensions.height / 2) - (m_barHeight / 2);
		barWidth = m_dimensions.width - m_sliderWidth;
		barHeight = m_barHeight;
	}
	else
	{
		sliderX = 0;
		sliderY = static_cast<int>((m_dimensions.height - m_sliderWidth) * ((m_currentValue - m_minValue) / (m_maxValue - m_minValue)));
		sliderWidth = m_dimensions.width;
		sliderHeight = m_sliderWidth;

		barX = (m_dimensions.width / 2) - (m_barHeight / 2);
		barY = (m_sliderWidth / 2);
		barWidth = m_barHeight;
		barHeight = m_dimensions.height - m_sliderWidth;
	}

	m_pScrollerDefault->SetDimensions(sliderX, sliderY, sliderWidth, sliderHeight);
	m_pScrollerHover->SetDimensions(sliderX, sliderY, sliderWidth, sliderHeight);
	m_pScrollerDragging->SetDimensions(sliderX, sliderY, sliderWidth, sliderHeight);
	m_pScrollerDisabled->SetDimensions(sliderX, sliderY, sliderWidth, sliderHeight);
	m_pScrollBackbarDefault->SetDimensions(barX, barY, barWidth, barHeight);
	m_pScrollBackbarPressed->SetDimensions(barX, barY, barWidth, barHeight);

	OnValueChanged();

	// Call the callback function callback if this class is just a simple button
	if (supressCallback == false && m_valueChangedCallback)
	{
		m_valueChangedCallback(m_pValueChangedCallbackData);
	}
}

void Slider::_PlusPressed(void* pData)
{
	Slider* slider = static_cast<Slider*>(pData);

	if (slider)
	{
		slider->PlusPressed();
	}
}

void Slider::PlusPressed()
{
	m_currentValue += m_incrementAmount;

	if (m_currentValue > m_maxValue)
	{
		m_currentValue = m_maxValue;
	}

	ValueChanged();
}

void Slider::_MinusPressed(void* pData)
{
	Slider* slider = static_cast<Slider*>(pData);

	if (slider)
	{
		slider->MinusPressed();
	}
}

void Slider::MinusPressed()
{
	m_currentValue -= m_incrementAmount;

	if (m_currentValue < m_minValue)
	{
		m_currentValue = m_minValue;
	}

	ValueChanged();
}

void Slider::DrawSelf()
{
	m_pScrollerDefault->SetVisible(false);
	m_pScrollerHover->SetVisible(false);
	m_pScrollerDragging->SetVisible(false);
	m_pScrollBackbarDefault->SetVisible(false);
	m_pScrollBackbarPressed->SetVisible(false);
	m_pScrollerDisabled->SetVisible(false);

	if (m_isDisabled)
	{
		m_pScrollerDisabled->SetVisible(true);
	}
	else if (m_isDragging)
	{
		m_pScrollerDragging->SetVisible(true);
	}
	else if (m_isHover)
	{
		m_pScrollerHover->SetVisible(true);
	}
	else
	{
		m_pScrollerDefault->SetVisible(true);
	}

	if (m_isRenderBackgroundBar)
	{
		if (m_isOverBar && m_isPressedBar)
		{
			m_pScrollBackbarPressed->SetVisible(true);
		}
		else
		{
			m_pScrollBackbarDefault->SetVisible(true);
		}
	}
	else if (m_isDynamicColorBackground)
	{
		int barX;
		int barY;
		int barWidth;
		int barHeight;
		int depth = static_cast<int>(GetDepth()) + 1;

		if (m_sliderDirection == SliderDirection::Horizontal)
		{
			barX = 0;
			barY = 0;
			barWidth = m_dimensions.width;
			barHeight = m_dimensions.height;
		}
		else //m_sliderDirection == ESliderDirection_Vertical
		{
			barX = 0;
			barY = 0;
			barWidth = m_dimensions.width;
			barHeight = m_dimensions.height;
		}

		m_pRenderer->PushMatrix();

		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

		m_pRenderer->ImmediateColorAlpha(m_backgroundColorBottomLeft.GetRed(), m_backgroundColorBottomLeft.GetGreen(), m_backgroundColorBottomLeft.GetBlue(), m_backgroundColorBottomLeft.GetAlpha());
		m_pRenderer->ImmediateVertex(barX, barY, depth);
		m_pRenderer->ImmediateColorAlpha(m_backgroundColorBottomRight.GetRed(), m_backgroundColorBottomRight.GetGreen(), m_backgroundColorBottomRight.GetBlue(), m_backgroundColorBottomRight.GetAlpha());
		m_pRenderer->ImmediateVertex(barX + barWidth, barY, depth);
		m_pRenderer->ImmediateColorAlpha(m_backgroundColorTopRight.GetRed(), m_backgroundColorTopRight.GetGreen(), m_backgroundColorTopRight.GetBlue(), m_backgroundColorTopRight.GetAlpha());
		m_pRenderer->ImmediateVertex(barX + barWidth, barY + barHeight, depth);
		m_pRenderer->ImmediateColorAlpha(m_backgroundColorTopLeft.GetRed(), m_backgroundColorTopLeft.GetGreen(), m_backgroundColorTopLeft.GetBlue(), m_backgroundColorTopLeft.GetAlpha());
		m_pRenderer->ImmediateVertex(barX, barY + barHeight, depth);
		
		m_pRenderer->DisableImmediateMode();
		
		m_pRenderer->PopMatrix();
	}

	// Set the value label visibility
	m_value.SetVisible(m_isShowValue);

	m_name.SetVisible(true);
}