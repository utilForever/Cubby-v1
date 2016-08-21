/*************************************************************************
> File Name: ScrollBar.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A scrollbar GUI component to allow scrolling horizontal or vertically. Can
>    be used for a variety of purposes, but mainly for scrolling large windows.
> Note
>    When being used in vertical mode, the left arrow is on top and the right
>    arrow is on the bottom.
> Created Time: 2016/08/21
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Utils/TimeManager.h>

#include "DirectDrawRectangle.h"
#include "ScrollBar.h"
#include "MultiTextureIcon.h"

// Constructor, Destructor
ScrollBar::ScrollBar(Renderer* pRenderer) :
	Container(pRenderer),
	m_pLeftArrowDefault(nullptr), m_pLeftArrowHover(nullptr), m_pLeftArrowPressed(nullptr), m_pLeftArrowDisabled(nullptr),
	m_pRightArrowDefault(nullptr), m_pRightArrowHover(nullptr), m_pRightArrowPressed(nullptr), m_pRightArrowDisabled(nullptr),
	m_pScrollbarDefault(nullptr), m_pScrollbarHover(nullptr), m_pScrollbarPressed(nullptr), m_pScrollbarDisabled(nullptr),
	m_pBackbar(nullptr)
{
	SetDefaultIcons(pRenderer);

	m_pLeftArrowDefault->SetDepth(1.5f);
	m_pLeftArrowHover->SetDepth(1.5f);
	m_pLeftArrowPressed->SetDepth(1.5f);
	m_pLeftArrowDisabled->SetDepth(1.5f);
	m_pRightArrowDefault->SetDepth(1.5f);
	m_pRightArrowHover->SetDepth(1.5f);
	m_pRightArrowPressed->SetDepth(1.5f);
	m_pRightArrowDisabled->SetDepth(1.5f);

	m_pScrollbarDefault->SetDepth(2.0f);
	m_pScrollbarHover->SetDepth(2.0f);
	m_pScrollbarPressed->SetDepth(2.0f);
	m_pScrollbarDisabled->SetDepth(2.0f);

	m_pBackbar->SetDepth(2.0f);

	m_selectedLeft = false;
	m_hoverLeft = false;
	m_selectedRight = false;
	m_hoverRight = false;
	m_disabled = false;

	m_hoverScrollBar = false;
	m_draggingScrollBar = false;
	m_firstOutsideDrag = false;

	m_scrollPosition = 0.5f;
	m_ScrollSize = 1.0f;

	m_lastX = 0;
	m_lastY = 0;

	m_scissorEnabled = true;

	m_pArrowButtonUpdate = new CountdownTimer();
	m_pArrowButtonUpdate->SetCountdownTime(0.01f);
	m_pArrowButtonUpdate->SetLooping(true);
	m_pArrowButtonUpdate->SetCallBackFunction(_UpdateArrowButtons);
	m_pArrowButtonUpdate->SetCallBackData(this);
	m_pArrowButtonUpdate->StartCountdown();

	m_scrollDirection = ScrollBarDirection::Horizontal;

	m_valueChangedCallback = nullptr;
	m_pValueChangedCallbackData = nullptr;
}

ScrollBar::ScrollBar(Renderer* pRenderer, const std::string& leftDefaultTexture, const std::string& leftPressedTexture, const std::string& leftHoverTexture, const std::string& leftDisabledTexture,
	const std::string& rightDefaultTexture, const std::string& rightPressedTexture, const std::string& rightHoverTexture, const std::string& rightDisabledTexture,
	const std::string& scrollDefaultTexture, const std::string& scrollPressedTexture, const std::string& scrollHoverTexture, const std::string& scrollDisabledTexture,
	const std::string& backbarTexture) :
	Container(pRenderer),
	m_pLeftArrowDefault(nullptr), m_pLeftArrowHover(nullptr), m_pLeftArrowPressed(nullptr), m_pLeftArrowDisabled(nullptr),
	m_pRightArrowDefault(nullptr), m_pRightArrowHover(nullptr), m_pRightArrowPressed(nullptr), m_pRightArrowDisabled(nullptr),
	m_pScrollbarDefault(nullptr), m_pScrollbarHover(nullptr), m_pScrollbarPressed(nullptr), m_pScrollbarDisabled(nullptr),
	m_pBackbar(nullptr)
{
	m_pLeftArrowDefault = new Icon(pRenderer, leftDefaultTexture);
	m_pLeftArrowHover = new Icon(pRenderer, leftHoverTexture);
	m_pLeftArrowPressed = new Icon(pRenderer, leftPressedTexture);
	m_pLeftArrowDisabled = new Icon(pRenderer, leftDisabledTexture);
	m_pRightArrowDefault = new Icon(pRenderer, rightDefaultTexture);
	m_pRightArrowHover = new Icon(pRenderer, rightHoverTexture);
	m_pRightArrowPressed = new Icon(pRenderer, rightPressedTexture);
	m_pRightArrowDisabled = new Icon(pRenderer, rightDisabledTexture);

	m_pScrollbarDefault = new Icon(pRenderer, scrollDefaultTexture);
	m_pScrollbarHover = new Icon(pRenderer, scrollHoverTexture);
	m_pScrollbarPressed = new Icon(pRenderer, scrollPressedTexture);
	m_pScrollbarDisabled = new Icon(pRenderer, scrollDisabledTexture);

	m_pBackbar = new Icon(pRenderer, backbarTexture);

	Container::Add(m_pLeftArrowDefault);
	Container::Add(m_pLeftArrowHover);
	Container::Add(m_pLeftArrowPressed);
	Container::Add(m_pLeftArrowDisabled);
	Container::Add(m_pRightArrowDefault);
	Container::Add(m_pRightArrowHover);
	Container::Add(m_pRightArrowPressed);
	Container::Add(m_pRightArrowDisabled);

	Container::Add(m_pScrollbarDefault);
	Container::Add(m_pScrollbarHover);
	Container::Add(m_pScrollbarPressed);
	Container::Add(m_pScrollbarDisabled);

	Container::Add(m_pBackbar);

	m_pLeftArrowDefault->SetDepth(1.5f);
	m_pLeftArrowHover->SetDepth(1.5f);
	m_pLeftArrowPressed->SetDepth(1.5f);
	m_pLeftArrowDisabled->SetDepth(1.5f);
	m_pRightArrowDefault->SetDepth(1.5f);
	m_pRightArrowHover->SetDepth(1.5f);
	m_pRightArrowPressed->SetDepth(1.5f);
	m_pRightArrowDisabled->SetDepth(1.5f);

	m_pScrollbarDefault->SetDepth(2.0f);
	m_pScrollbarHover->SetDepth(2.0f);
	m_pScrollbarPressed->SetDepth(2.0f);
	m_pScrollbarDisabled->SetDepth(2.0f);

	m_pBackbar->SetDepth(2.0f);

	m_selectedLeft = false;
	m_hoverLeft = false;
	m_selectedRight = false;
	m_hoverRight = false;
	m_disabled = false;

	m_hoverScrollBar = false;
	m_draggingScrollBar = false;
	m_firstOutsideDrag = false;

	m_scrollPosition = 0.5f;
	m_ScrollSize = 1.0f;

	m_lastX = 0;
	m_lastY = 0;

	m_scissorEnabled = true;

	m_pArrowButtonUpdate = new CountdownTimer();
	m_pArrowButtonUpdate->SetCountdownTime(0.01f);
	m_pArrowButtonUpdate->SetLooping(true);
	m_pArrowButtonUpdate->SetCallBackFunction(_UpdateArrowButtons);
	m_pArrowButtonUpdate->SetCallBackData(this);
	m_pArrowButtonUpdate->StartCountdown();

	m_scrollDirection = ScrollBarDirection::Horizontal;

	m_valueChangedCallback = nullptr;
	m_pValueChangedCallbackData = nullptr;
}

ScrollBar::~ScrollBar()
{
	TimeManager::GetInstance()->RemoveCountdownTimer(m_pArrowButtonUpdate);

	delete m_pLeftArrowDefault;
	delete m_pLeftArrowHover;
	delete m_pLeftArrowPressed;
	delete m_pLeftArrowDisabled;
	delete m_pRightArrowDefault;
	delete m_pRightArrowHover;
	delete m_pRightArrowPressed;
	delete m_pRightArrowDisabled;

	delete m_pScrollbarDefault;
	delete m_pScrollbarHover;
	delete m_pScrollbarPressed;
	delete m_pScrollbarDisabled;

	delete m_pBackbar;
}

void ScrollBar::AddEventListeners()
{
	Container::AddEventListeners();

	AddMouseListener(this);
}

void ScrollBar::RemoveEventListeners()
{
	Container::RemoveEventListeners();

	RemoveMouseListener(this);

	// Remove hover and select properties, since we have made this component unable to select by the mouse
	SetHoverRight(false);
	SetSelectedRight(false);
	SetHoverLeft(false);
	SetSelectedLeft(false);
}

void ScrollBar::RemoveIcon(RenderRectangle* pRemoveIcon)
{
	if (pRemoveIcon)
	{
		Remove(pRemoveIcon);
		delete pRemoveIcon;
	}
}

void ScrollBar::SetDefaultIcons(Renderer* pRenderer)
{
	RemoveIcon(m_pLeftArrowDefault);
	RemoveIcon(m_pLeftArrowHover);
	RemoveIcon(m_pLeftArrowPressed);
	RemoveIcon(m_pLeftArrowDisabled);
	RemoveIcon(m_pRightArrowDefault);
	RemoveIcon(m_pRightArrowHover);
	RemoveIcon(m_pRightArrowPressed);
	RemoveIcon(m_pRightArrowDisabled);

	RemoveIcon(m_pScrollbarDefault);
	RemoveIcon(m_pScrollbarHover);
	RemoveIcon(m_pScrollbarPressed);
	RemoveIcon(m_pScrollbarDisabled);

	RemoveIcon(m_pBackbar);

	m_pLeftArrowDefault = new DirectDrawRectangle(pRenderer);
	m_pLeftArrowHover = new DirectDrawRectangle(pRenderer);
	m_pLeftArrowPressed = new DirectDrawRectangle(pRenderer);
	m_pLeftArrowDisabled = new DirectDrawRectangle(pRenderer);
	m_pRightArrowDefault = new DirectDrawRectangle(pRenderer);
	m_pRightArrowHover = new DirectDrawRectangle(pRenderer);
	m_pRightArrowPressed = new DirectDrawRectangle(pRenderer);
	m_pRightArrowDisabled = new DirectDrawRectangle(pRenderer);

	m_pScrollbarDefault = new DirectDrawRectangle(pRenderer);
	m_pScrollbarHover = new DirectDrawRectangle(pRenderer);
	m_pScrollbarPressed = new DirectDrawRectangle(pRenderer);
	m_pScrollbarDisabled = new DirectDrawRectangle(pRenderer);

	m_pBackbar = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pLeftArrowDefault);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pLeftArrowHover);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pLeftArrowPressed);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pLeftArrowDisabled);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pRightArrowDefault);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pRightArrowHover);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pRightArrowPressed);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pRightArrowDisabled);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollbarDefault);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollbarHover);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollbarPressed);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pScrollbarDisabled);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pBackbar);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	Add(m_pLeftArrowDefault);
	Add(m_pLeftArrowHover);
	Add(m_pLeftArrowPressed);
	Add(m_pLeftArrowDisabled);
	Add(m_pRightArrowDefault);
	Add(m_pRightArrowHover);
	Add(m_pRightArrowPressed);
	Add(m_pRightArrowDisabled);

	Add(m_pScrollbarDefault);
	Add(m_pScrollbarHover);
	Add(m_pScrollbarPressed);
	Add(m_pScrollbarDisabled);

	Add(m_pBackbar);

	Dimensions d = GetDimensions();
	if (m_scrollDirection == ScrollBarDirection::Horizontal)
	{
		// TEMP: For the time being, make all arrow graphics square...
		m_pLeftArrowDefault->SetDimensions(0, 0, d.height, d.height);
		m_pLeftArrowHover->SetDimensions(0, 0, d.height, d.height);
		m_pLeftArrowPressed->SetDimensions(0, 0, d.height, d.height);
		m_pLeftArrowDisabled->SetDimensions(0, 0, d.height, d.height);
		m_pRightArrowDefault->SetDimensions(d.width - d.height, 0, d.height, d.height);
		m_pRightArrowHover->SetDimensions(d.width - d.height, 0, d.height, d.height);
		m_pRightArrowPressed->SetDimensions(d.width - d.height, 0, d.height, d.height);
		m_pRightArrowDisabled->SetDimensions(d.width - d.height, 0, d.height, d.height);
	}
	else
	{
		// TEMP: For the time being, make all arrow graphics square...
		m_pLeftArrowDefault->SetDimensions(0, d.height - d.width, d.width, d.width);
		m_pLeftArrowHover->SetDimensions(0, d.height - d.width, d.width, d.width);
		m_pLeftArrowPressed->SetDimensions(0, d.height - d.width, d.width, d.width);
		m_pLeftArrowDisabled->SetDimensions(0, d.height - d.width, d.width, d.width);
		m_pRightArrowDefault->SetDimensions(0, 0, d.width, d.width);
		m_pRightArrowHover->SetDimensions(0, 0, d.width, d.width);
		m_pRightArrowPressed->SetDimensions(0, 0, d.width, d.width);
		m_pRightArrowDisabled->SetDimensions(0, 0, d.width, d.width);
	}
}

void ScrollBar::SetLeftArrowDefaultIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pLeftArrowDefault)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pLeftArrowDefault);

		delete m_pLeftArrowDefault;
		m_pLeftArrowDefault = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pLeftArrowDefault = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pLeftArrowDefault = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pLeftArrowDefault);

	Dimensions dimensions = GetDimensions();
	SetDimensions(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
}

void ScrollBar::SetLeftArrowHoverIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pLeftArrowHover)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pLeftArrowHover);

		delete m_pLeftArrowHover;
		m_pLeftArrowHover = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pLeftArrowHover = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pLeftArrowHover = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pLeftArrowHover);

	Dimensions dimensions = GetDimensions();
	SetDimensions(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
}

void ScrollBar::SetLeftArrowSelectedIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pLeftArrowPressed)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pLeftArrowPressed);

		delete m_pLeftArrowPressed;
		m_pLeftArrowPressed = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pLeftArrowPressed = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pLeftArrowPressed = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pLeftArrowPressed);

	Dimensions dimensions = GetDimensions();
	SetDimensions(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
}

void ScrollBar::SetLeftArrowDisabledIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pLeftArrowDisabled)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pLeftArrowDisabled);

		delete m_pLeftArrowDisabled;
		m_pLeftArrowDisabled = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pLeftArrowDisabled = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pLeftArrowDisabled = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pLeftArrowDisabled);

	Dimensions dimensions = GetDimensions();
	SetDimensions(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
}

void ScrollBar::SetRightArrowDefaultIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pRightArrowDefault)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pRightArrowDefault);

		delete m_pRightArrowDefault;
		m_pRightArrowDefault = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pRightArrowDefault = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pRightArrowDefault = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pRightArrowDefault);

	Dimensions dimensions = GetDimensions();
	SetDimensions(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
}

void ScrollBar::SetRightArrowHoverIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pRightArrowHover)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pRightArrowHover);

		delete m_pRightArrowHover;
		m_pRightArrowHover = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pRightArrowHover = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pRightArrowHover = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pRightArrowHover);

	Dimensions dimensions = GetDimensions();
	SetDimensions(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
}

void ScrollBar::SetRightArrowSelectedIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pRightArrowPressed)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pRightArrowPressed);

		delete m_pRightArrowPressed;
		m_pRightArrowPressed = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pRightArrowPressed = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pRightArrowPressed = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pRightArrowPressed);

	Dimensions dimensions = GetDimensions();
	SetDimensions(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
}

void ScrollBar::SetRightArrowDisabledIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pRightArrowDisabled)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pRightArrowDisabled);

		delete m_pRightArrowDisabled;
		m_pRightArrowDisabled = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pRightArrowDisabled = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pRightArrowDisabled = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pRightArrowDisabled);

	Dimensions dimensions = GetDimensions();
	SetDimensions(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
}

void ScrollBar::SetScrollbarDefaultIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pScrollbarDefault)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pScrollbarDefault);

		delete m_pScrollbarDefault;
		m_pScrollbarDefault = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pScrollbarDefault = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pScrollbarDefault = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pScrollbarDefault);

	Dimensions dimensions = GetDimensions();
	SetDimensions(dimensions.x, dimensions.y, dimensions.width, dimensions.height);
}

void ScrollBar::SetScrollbarHoverIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pScrollbarHover)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pScrollbarHover);

		delete m_pScrollbarHover;
		m_pScrollbarHover = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pScrollbarHover = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pScrollbarHover = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pScrollbarHover);
}

void ScrollBar::SetScrollbarSelectedIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pScrollbarPressed)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pScrollbarPressed);

		delete m_pScrollbarPressed;
		m_pScrollbarPressed = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pScrollbarPressed = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pScrollbarPressed = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pScrollbarPressed);
}

void ScrollBar::SetScrollbarDisabledIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pScrollbarDisabled)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pScrollbarDisabled);

		delete m_pScrollbarDisabled;
		m_pScrollbarDisabled = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pScrollbarDisabled = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pScrollbarDisabled = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pScrollbarDisabled);
}

void ScrollBar::SetBackbarIcon(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pBackbar)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pBackbar);

		delete m_pBackbar;
		m_pBackbar = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pBackbar = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pBackbar = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pBackbar);
}

void ScrollBar::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	// Also set the dimensions of the icons
	if (m_scrollDirection == ScrollBarDirection::Horizontal)
	{
		// TEMP: For the time being, make all arrow graphics square...
		m_pLeftArrowDefault->SetDimensions(0, 0, height, height);
		m_pLeftArrowHover->SetDimensions(0, 0, height, height);
		m_pLeftArrowPressed->SetDimensions(0, 0, height, height);
		m_pLeftArrowDisabled->SetDimensions(0, 0, height, height);
		m_pRightArrowDefault->SetDimensions(width - height, 0, height, height);
		m_pRightArrowHover->SetDimensions(width - height, 0, height, height);
		m_pRightArrowPressed->SetDimensions(width - height, 0, height, height);
		m_pRightArrowDisabled->SetDimensions(width - height, 0, height, height);
	}
	else
	{
		// TEMP: For the time being, make all arrow graphics square...
		m_pLeftArrowDefault->SetDimensions(0, height - width, width, width);
		m_pLeftArrowHover->SetDimensions(0, height - width, width, width);
		m_pLeftArrowPressed->SetDimensions(0, height - width, width, width);
		m_pLeftArrowDisabled->SetDimensions(0, height - width, width, width);
		m_pRightArrowDefault->SetDimensions(0, 0, width, width);
		m_pRightArrowHover->SetDimensions(0, 0, width, width);
		m_pRightArrowPressed->SetDimensions(0, 0, width, width);
		m_pRightArrowDisabled->SetDimensions(0, 0, width, width);
	}
}

void ScrollBar::SetArrowDimensions(int width, int height) const
{
	Dimensions leftDimensions = m_pLeftArrowDefault->GetDimensions();
	Dimensions rightDimensions = m_pRightArrowDefault->GetDimensions();

	// TEMP: For the time being, make all arrow graphics square...
	m_pLeftArrowDefault->SetDimensions(leftDimensions.x, leftDimensions.y, width, height);
	m_pLeftArrowHover->SetDimensions(leftDimensions.x, leftDimensions.y, width, height);
	m_pLeftArrowPressed->SetDimensions(leftDimensions.x, leftDimensions.y, width, height);
	m_pLeftArrowDisabled->SetDimensions(leftDimensions.x, leftDimensions.y, width, height);
	m_pRightArrowDefault->SetDimensions(rightDimensions.x, rightDimensions.y, width, height);
	m_pRightArrowHover->SetDimensions(rightDimensions.x, rightDimensions.y, width, height);
	m_pRightArrowPressed->SetDimensions(rightDimensions.x, rightDimensions.y, width, height);
	m_pRightArrowDisabled->SetDimensions(rightDimensions.x, rightDimensions.y, width, height);
}

void ScrollBar::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void ScrollBar::AddScrollAreaItem(Component* component)
{
	component->SetParent(this);

	component->AddEventListeners();

	component->SetAudio(m_audio);
	component->SetAudioVolume(m_audioVolume);

	m_vpScrollAreaComponentList.push_back(component);
}

void ScrollBar::ClearScrollAreaItems()
{
	for (auto iter = m_vpScrollAreaComponentList.begin(); iter != m_vpScrollAreaComponentList.end(); ++iter)
	{
		Component* pComponent = static_cast<Component*>(*iter);

		pComponent->RemoveEventListeners();
	}

	m_vpScrollAreaComponentList.clear();
}

void ScrollBar::SetScrollArea(int x, int y, int width, int height)
{
	m_scrollArea.x = x;
	m_scrollArea.y = y;
	m_scrollArea.width = width;
	m_scrollArea.height = height;
}

Dimensions ScrollBar::GetScrollArea() const
{
	return m_scrollArea;
}

void ScrollBar::SetScrollPosition(float lPosition)
{
	m_scrollPosition = lPosition;

	if (m_scrollPosition < 0.0f)
	{
		m_scrollPosition = 0.0f;
	}

	if (m_scrollPosition > 1.0f)
	{
		m_scrollPosition = 1.0f;
	}
}

float ScrollBar::GetScrollPosition() const
{
	return m_scrollPosition;
}

void ScrollBar::SetScrollSize(float size)
{
	m_ScrollSize = size;

	if (m_ScrollSize < 0.0f)
	{
		m_ScrollSize = 0.0f;
	}

	if (m_ScrollSize > 1.0f)
	{
		m_ScrollSize = 1.0f;
	}

	ClampScrollBarPosition();

	UpdateScrollbarDimension();
}

float ScrollBar::GetScrollSize() const
{
	return m_ScrollSize;
}

float ScrollBar::GetScrollRatio() const
{
	float scrollBorder = m_ScrollSize * 0.5f;
	float scrollRatio = 1.0f - ((m_scrollPosition - scrollBorder) / (1.0f - m_ScrollSize));

	if (m_ScrollSize <= 0.0f)
	{
		return 0.0f;
	}

	if (m_ScrollSize >= 1.0f)
	{
		return 1.0f;
	}

	if (scrollRatio < 0.0f)
	{
		scrollRatio = 0.0f;
	}

	if (scrollRatio > 1.0f)
	{
		scrollRatio = 1.0f;
	}
	
	return scrollRatio;
}

void ScrollBar::UpdateScrollbarDimension() const
{
	int scrollX;
	int scrollY;

	if (m_scrollDirection == ScrollBarDirection::Horizontal)
	{
		int totalScrollWidth = m_dimensions.width - (m_dimensions.height * 2);
		int scrollbarWidth = static_cast<int>(totalScrollWidth * m_ScrollSize);
		int scrollbarPosition = static_cast<int>(totalScrollWidth * m_scrollPosition);
		scrollX = m_dimensions.height + scrollbarPosition - (scrollbarWidth / 2);
		scrollY = 0;

		m_pScrollbarDefault->SetDimensions(scrollX, scrollY, scrollbarWidth, m_dimensions.height);
		m_pScrollbarHover->SetDimensions(scrollX, scrollY, scrollbarWidth, m_dimensions.height);
		m_pScrollbarPressed->SetDimensions(scrollX, scrollY, scrollbarWidth, m_dimensions.height);
		m_pScrollbarDisabled->SetDimensions(scrollX, scrollY, scrollbarWidth, m_dimensions.height);

		m_pBackbar->SetDimensions(m_dimensions.height, 0, totalScrollWidth, m_dimensions.height);
	}
	else
	{
		int totalScrollHeight = m_dimensions.height - (m_dimensions.width * 2);
		int scrollbarHeight = static_cast<int>(totalScrollHeight * m_ScrollSize);
		int scrollbarPosition = static_cast<int>(totalScrollHeight * m_scrollPosition);
		scrollX = 0;
		scrollY = m_dimensions.width + scrollbarPosition - (scrollbarHeight / 2);

		m_pScrollbarDefault->SetDimensions(scrollX, scrollY, m_dimensions.width, scrollbarHeight);
		m_pScrollbarHover->SetDimensions(scrollX, scrollY, m_dimensions.width, scrollbarHeight);
		m_pScrollbarPressed->SetDimensions(scrollX, scrollY, m_dimensions.width, scrollbarHeight);
		m_pScrollbarDisabled->SetDimensions(scrollX, scrollY, m_dimensions.width, scrollbarHeight);

		m_pBackbar->SetDimensions(0, m_dimensions.width, m_dimensions.width, totalScrollHeight);
	}
}

void ScrollBar::ClampScrollBarPosition()
{
	float minPos = m_ScrollSize / 2;
	float maxPos = 1.0f - (m_ScrollSize / 2);

	if (m_scrollPosition < minPos)
	{
		m_scrollPosition = minPos;
	}

	if (m_scrollPosition > maxPos)
	{
		m_scrollPosition = maxPos;
	}
}

void ScrollBar::SetScissorEnabled(bool scissor)
{
	m_scissorEnabled = scissor;
}

ScrollBarDirection ScrollBar::GetScrollDirection() const
{
	return m_scrollDirection;
}

void ScrollBar::SetScrollDirection(ScrollBarDirection direction)
{
	m_scrollDirection = direction;

	// Force a re-setup of the dimensions, since we have changed the orientation
	SetDimensions(m_dimensions);
}

void ScrollBar::SetSelectedRight(bool val)
{
	m_selectedRight = val;
}

void ScrollBar::SetHoverRight(bool val)
{
	m_hoverRight = val;
}

void ScrollBar::SetSelectedLeft(bool val)
{
	m_selectedLeft = val;
}

void ScrollBar::SetHoverLeft(bool val)
{
	m_hoverLeft = val;
}

void ScrollBar::SetDisabled(bool val)
{
	m_disabled = val;
}

bool ScrollBar::IsSelectedRight() const
{
	return m_selectedRight;
}

bool ScrollBar::IsHoverRight() const
{
	return m_hoverRight;
}

bool ScrollBar::IsSelectedLeft() const
{
	return m_selectedLeft;
}

bool ScrollBar::IsHoverLeft() const
{
	return m_hoverLeft;
}

bool ScrollBar::IsDisabled() const
{
	return m_disabled;
}

ComponentType ScrollBar::GetComponentType() const
{
	return ComponentType::ScrollBar;
}

void ScrollBar::SetValueChangedCallback(FunctionCallback func)
{
	m_valueChangedCallback = func;
}

void ScrollBar::SetValueChangedCallBackData(void* pData)
{
	m_pValueChangedCallbackData = pData;
}

void ScrollBar::SetAudio(bool set)
{
	Container::SetAudio(set);

	for (auto iter = m_vpScrollAreaComponentList.begin(); iter != m_vpScrollAreaComponentList.end(); ++iter)
	{
		(*iter)->SetAudio(set);
	}
}

void ScrollBar::SetAudioVolume(float volume)
{
	Container::SetAudioVolume(volume);

	for (auto iter = m_vpScrollAreaComponentList.begin(); iter != m_vpScrollAreaComponentList.end(); ++iter)
	{
		(*iter)->SetAudioVolume(volume);
	}
}

void ScrollBar::MouseEntered(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	m_firstOutsideDrag = false;

	OnMouseEnter();
}

void ScrollBar::MouseExited(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetHoverRight(false);
	SetHoverLeft(false);

	SetSelectedRight(false);
	SetSelectedLeft(false);

	m_hoverScrollBar = false;

	OnMouseExit();
}

void ScrollBar::MousePressed(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	int leftX1, leftX2, leftY1, leftY2;
	int rightX1, rightX2, rightY1, rightY2;
	int scrollX1, scrollX2, scrollY1, scrollY2;

	if (m_scrollDirection == ScrollBarDirection::Horizontal)
	{
		leftX1 = GetLocationOnScreen().x;
		leftX2 = GetLocationOnScreen().x + GetDimensions().height;
		leftY1 = GetLocationOnScreen().y;
		leftY2 = GetLocationOnScreen().y + GetDimensions().height;

		rightX1 = GetLocationOnScreen().x + GetDimensions().width - GetDimensions().height;
		rightX2 = GetLocationOnScreen().x + GetDimensions().width;
		rightY1 = GetLocationOnScreen().y;
		rightY2 = GetLocationOnScreen().y + GetDimensions().height;

		int totalScrollWidth = m_dimensions.width - (m_dimensions.height * 2);
		int scrollbarWidth = static_cast<int>(totalScrollWidth * m_ScrollSize);
		int scrollbarPosition = static_cast<int>(totalScrollWidth * m_scrollPosition);

		scrollX1 = GetLocationOnScreen().x + m_dimensions.height + scrollbarPosition - (scrollbarWidth / 2);
		scrollX2 = GetLocationOnScreen().x + m_dimensions.height + scrollbarPosition + (scrollbarWidth / 2);
		scrollY1 = GetLocationOnScreen().y;
		scrollY2 = GetLocationOnScreen().y + m_dimensions.height;
	}
	else //m_scrollDirection == ScrollBarDirection::Vertical
	{
		// Remember, this is really the top arrow
		leftX1 = GetLocationOnScreen().x;
		leftX2 = GetLocationOnScreen().x + GetDimensions().width;
		leftY1 = GetLocationOnScreen().y + GetDimensions().height - GetDimensions().width;
		leftY2 = GetLocationOnScreen().y + GetDimensions().height;

		// Remember, this is really the bottom arrow
		rightX1 = GetLocationOnScreen().x;
		rightX2 = GetLocationOnScreen().x + GetDimensions().width;
		rightY1 = GetLocationOnScreen().y;
		rightY2 = GetLocationOnScreen().y + GetDimensions().width;

		int totalScrollHeight = m_dimensions.height - (m_dimensions.width * 2);
		int scrollbarHeight = static_cast<int>(totalScrollHeight * m_ScrollSize);
		int scrollbarPosition = static_cast<int>(totalScrollHeight * m_scrollPosition);
		
		scrollX1 = GetLocationOnScreen().x;
		scrollX2 = GetLocationOnScreen().x + m_dimensions.width;
		scrollY1 = GetLocationOnScreen().y + m_dimensions.width + scrollbarPosition - (scrollbarHeight / 2);
		scrollY2 = GetLocationOnScreen().y + m_dimensions.width + scrollbarPosition + (scrollbarHeight / 2);
	}

	// Check for arrow buttons
	if (event.GetX() >= leftX1 && event.GetX() < leftX2 && event.GetY() >= leftY1 && event.GetY() < leftY2)
	{
		SetSelectedLeft(true);
		UpdateArrowButtons();
	}
	if (event.GetX() >= rightX1 && event.GetX() < rightX2 && event.GetY() >= rightY1 && event.GetY() < rightY2)
	{
		SetSelectedRight(true);
		UpdateArrowButtons();
	}

	// Check for scrollbar
	if (event.GetX() >= scrollX1 && event.GetX() < scrollX2 && event.GetY() >= scrollY1 && event.GetY() < scrollY2)
	{
		m_draggingScrollBar = true;
	}
	else
	{
		m_draggingScrollBar = false;
	}

	ClampScrollBarPosition();

	// If our parent is a GUIWindow, then make this window have focus in the GUI, used to make it's depth the highest
	//if(GetParent() != nullptr && GetParent()->GetComponentType() == ComponentType::GUIWindow)
	//{
	//GUIWindow* pParentWindow = static_cast<GUIWindow*>(GetParent());
	//pParentWindow->SetFocusWindow();
	//}

	OnMousePressed();
}

void ScrollBar::MouseReleased(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetSelectedRight(false);
	SetSelectedLeft(false);

	m_draggingScrollBar = false;

	OnMouseReleased();
}

void ScrollBar::MouseReleasedOutside(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	m_draggingScrollBar = false;
}

void ScrollBar::MouseClicked(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	OnMouseClicked();
}

void ScrollBar::MouseDragged(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	if (m_draggingScrollBar && !m_hoverRight && !m_hoverLeft)
	{
		int changeX = event.GetX();
		int changeY = event.GetY();

		if (changeX == 0 && changeY == 0)
		{
			return;
		}

		float valueChange = 0.0f;
		if (m_ScrollSize < 1.0f)
		{
			if (m_scrollDirection == ScrollBarDirection::Horizontal)
			{
				int totalScrollWidth = m_dimensions.width - (m_dimensions.height * 2);
				int scrollbarWidth = static_cast<int>(totalScrollWidth * m_ScrollSize);

				valueChange = static_cast<float>(changeX) * (1.0f - m_ScrollSize) / static_cast<float>(totalScrollWidth - scrollbarWidth);
			}
			else
			{
				int totalScrollHeight = m_dimensions.height - (m_dimensions.width * 2);
				int scrollbarHeight = static_cast<int>(totalScrollHeight * m_ScrollSize);
				
				valueChange = static_cast<float>(changeY) * (1.0f - m_ScrollSize) / static_cast<float>(totalScrollHeight - scrollbarHeight);
			}
		}

		// Store the value before we change it, to see if we have actually changed the value
		float valueBefore = m_scrollPosition;

		m_scrollPosition += valueChange;

		ClampScrollBarPosition();

		if (m_scrollPosition != valueBefore)
		{
			OnValueChanged();

			if (m_valueChangedCallback)
			{
				m_valueChangedCallback(m_pValueChangedCallbackData);
			}
		}
	}
}

void ScrollBar::MouseDraggedOutside(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	if (!m_firstOutsideDrag)
	{
		m_lastX = event.GetX();
		m_lastY = event.GetY();
		m_firstOutsideDrag = true;
	}

	if (m_draggingScrollBar)
	{
		int newX = event.GetX();
		int newY = event.GetY();

		int minX = GetLocationOnScreen().x + m_dimensions.height;			// Used for horizontal scrollbars
		int maxX = minX + m_dimensions.width - m_dimensions.height * 2;		// used for horizontal scrollbars
		int minY = GetLocationOnScreen().y + m_dimensions.width;			// Used for vertical scrollbars
		int maxY = minY + m_dimensions.height - m_dimensions.width * 2;		// used for vertical scrollbars

		int changeX = newX - m_lastX;
		int changeY = newY - m_lastY;

		if (changeX == 0 && changeY == 0)
		{
			return;
		}

		float valueChange = 0.0f;
		if (m_ScrollSize < 1.0f)
		{
			if (m_scrollDirection == ScrollBarDirection::Horizontal)
			{
				int totalScrollWidth = m_dimensions.width - (m_dimensions.height * 2);
				int scrollbarWidth = static_cast<int>(totalScrollWidth * m_ScrollSize);

				valueChange = static_cast<float>(changeX) * (1.0f - m_ScrollSize) / static_cast<float>(totalScrollWidth - scrollbarWidth);
			}
			else
			{
				int totalScrollHeight = m_dimensions.height - (m_dimensions.width * 2);
				int scrollbarHeight = static_cast<int>(totalScrollHeight * m_ScrollSize);
				
				valueChange = static_cast<float>(changeY) * (1.0f - m_ScrollSize) / static_cast<float>(totalScrollHeight - scrollbarHeight);
			}
		}

		// Store the value before we change it, to see if we have actually changed the value
		float valueBefore = m_scrollPosition;

		if (m_scrollDirection == ScrollBarDirection::Horizontal)
		{
			if ((valueChange > 0) && (newX > minX))
			{
				m_scrollPosition += valueChange;
			}
			else if ((valueChange < 0) && (newX < maxX))
			{
				m_scrollPosition += valueChange;
			}
		}
		else
		{
			if ((valueChange > 0) && (newY > minY))
			{
				m_scrollPosition += valueChange;
			}
			else if ((valueChange < 0) && (newY < maxY))
			{
				m_scrollPosition += valueChange;
			}
		}

		ClampScrollBarPosition();

		if (m_scrollPosition != valueBefore)
		{
			OnValueChanged();

			if (m_valueChangedCallback)
			{
				m_valueChangedCallback(m_pValueChangedCallbackData);
			}
		}

		m_lastX = newX;
		m_lastY = newY;
	}
}

void ScrollBar::MouseMotion(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	int leftX1, leftX2, leftY1, leftY2;
	int rightX1, rightX2, rightY1, rightY2;
	int scrollX1, scrollX2, scrollY1, scrollY2;

	if (m_scrollDirection == ScrollBarDirection::Horizontal)
	{
		leftX1 = GetLocationOnScreen().x;
		leftX2 = GetLocationOnScreen().x + GetDimensions().height;
		leftY1 = GetLocationOnScreen().y;
		leftY2 = GetLocationOnScreen().y + GetDimensions().height;

		rightX1 = GetLocationOnScreen().x + GetDimensions().width - GetDimensions().height;
		rightX2 = GetLocationOnScreen().x + GetDimensions().width;
		rightY1 = GetLocationOnScreen().y;
		rightY2 = GetLocationOnScreen().y + GetDimensions().height;

		int totalScrollWidth = m_dimensions.width - (m_dimensions.height * 2);
		int scrollbarWidth = static_cast<int>(totalScrollWidth * m_ScrollSize);
		int scrollbarPosition = static_cast<int>(totalScrollWidth * m_scrollPosition);
		
		scrollX1 = GetLocationOnScreen().x + m_dimensions.height + scrollbarPosition - (scrollbarWidth / 2);
		scrollX2 = GetLocationOnScreen().x + m_dimensions.height + scrollbarPosition + (scrollbarWidth / 2);
		scrollY1 = GetLocationOnScreen().y;
		scrollY2 = GetLocationOnScreen().y + m_dimensions.height;
	}
	else
	{
		// Remember, this is really the top arrow
		leftX1 = GetLocationOnScreen().x;
		leftX2 = GetLocationOnScreen().x + GetDimensions().width;
		leftY1 = GetLocationOnScreen().y + GetDimensions().height - GetDimensions().width;
		leftY2 = GetLocationOnScreen().y + GetDimensions().height;

		// Remember, this is really the bottom arrow
		rightX1 = GetLocationOnScreen().x;
		rightX2 = GetLocationOnScreen().x + GetDimensions().width;
		rightY1 = GetLocationOnScreen().y;
		rightY2 = GetLocationOnScreen().y + GetDimensions().width;

		int totalScrollHeight = m_dimensions.height - (m_dimensions.width * 2);
		int scrollbarHeight = static_cast<int>(totalScrollHeight * m_ScrollSize);
		int scrollbarPosition = static_cast<int>(totalScrollHeight * m_scrollPosition);
		
		scrollX1 = GetLocationOnScreen().x;
		scrollX2 = GetLocationOnScreen().x + m_dimensions.width;
		scrollY1 = GetLocationOnScreen().y + m_dimensions.width + scrollbarPosition - (scrollbarHeight / 2);
		scrollY2 = GetLocationOnScreen().y + m_dimensions.width + scrollbarPosition + (scrollbarHeight / 2);
	}

	// Check for arrow buttons
	if (event.GetX() >= leftX1 && event.GetX() < leftX2 && event.GetY() >= leftY1 && event.GetY() < leftY2)
	{
		SetHoverLeft(true);
	}
	else
	{
		SetHoverLeft(false);
		SetSelectedLeft(false);
	}
	if (event.GetX() >= rightX1 && event.GetX() < rightX2 && event.GetY() >= rightY1 && event.GetY() < rightY2)
	{
		SetHoverRight(true);
	}
	else
	{
		SetHoverRight(false);
		SetSelectedRight(false);
	}

	// Check for scrollbar
	if (event.GetX() >= scrollX1 && event.GetX() < scrollX2 && event.GetY() >= scrollY1 && event.GetY() < scrollY2)
	{
		m_hoverScrollBar = true;
	}
	else
	{
		m_hoverScrollBar = false;
	}

	OnMouseMotion();
}

void ScrollBar::OnMouseEnter()
{
	// Do nothing
}

void ScrollBar::OnMouseExit()
{
	// Do nothing
}

void ScrollBar::OnMousePressed()
{
	// Do nothing
}

void ScrollBar::OnMouseReleased()
{
	// Do nothing
}

void ScrollBar::OnMouseClicked()
{
	// Do nothing
}

void ScrollBar::OnMouseMotion()
{
	// Do nothing
}

void ScrollBar::OnValueChanged()
{
	// Do nothing
}

void ScrollBar::DrawSelf()
{
	// We need to update some scrollbar params and dimensions
	ClampScrollBarPosition();

	UpdateScrollbarDimension();

	// Reset arrow icon visibility
	m_pLeftArrowDefault->SetVisible(false);
	m_pLeftArrowHover->SetVisible(false);
	m_pLeftArrowPressed->SetVisible(false);
	m_pLeftArrowDisabled->SetVisible(false);
	m_pRightArrowDefault->SetVisible(false);
	m_pRightArrowHover->SetVisible(false);
	m_pRightArrowPressed->SetVisible(false);
	m_pRightArrowDisabled->SetVisible(false);

	if (IsSelectedRight())
	{
		m_pRightArrowPressed->SetVisible(true);
	}
	else if (IsHoverRight())
	{
		m_pRightArrowHover->SetVisible(true);
	}
	else if (IsDisabled())
	{
		m_pRightArrowDisabled->SetVisible(true);
	}
	else
	{
		m_pRightArrowDefault->SetVisible(true);
	}

	if (IsSelectedLeft())
	{
		m_pLeftArrowPressed->SetVisible(true);
	}
	else if (IsHoverLeft())
	{
		m_pLeftArrowHover->SetVisible(true);
	}
	else if (IsDisabled())
	{
		m_pLeftArrowDisabled->SetVisible(true);
	}
	else
	{
		m_pLeftArrowDefault->SetVisible(true);
	}

	// Scrollbar visibility
	m_pScrollbarDefault->SetVisible(false);
	m_pScrollbarHover->SetVisible(false);
	m_pScrollbarPressed->SetVisible(false);
	m_pScrollbarDisabled->SetVisible(false);

	m_pBackbar->SetVisible(true);

	if (m_draggingScrollBar)
	{
		m_pScrollbarPressed->SetVisible(true);
	}
	else if (m_hoverScrollBar)
	{
		m_pScrollbarHover->SetVisible(true);
	}
	else if (IsDisabled())
	{
		m_pScrollbarDisabled->SetVisible(true);
	}
	else
	{
		m_pScrollbarDefault->SetVisible(true);
	}
}

void ScrollBar::DrawChildren()
{
	Container::DrawChildren();

	Dimensions scrollbarParent;
	if (GetParent() != nullptr)
	{
		scrollbarParent = GetParent()->GetDimensions();
	}

	int scissorX = scrollbarParent.x + GetLocation().x + m_scrollArea.x;
	int scissorWidth = m_scrollArea.width;
	int scissorY = scrollbarParent.y + GetLocation().y + m_scrollArea.y;
	int scissorHeight = m_scrollArea.height;

	if (IsContainer() && HasChildren() && IsVisible())
	{
		// Draw all children
		for (auto iter = m_vpScrollAreaComponentList.begin(); iter != m_vpScrollAreaComponentList.end(); ++iter)
		{
			if ((*iter)->IsVisible())
			{
				if (m_scissorEnabled)
				{
					m_pRenderer->EnableScissorTest(scissorX, scissorY, scissorWidth, scissorHeight);;
				}

				(*iter)->Draw();

				if (m_scissorEnabled)
				{
					m_pRenderer->DisableScissorTest();
				}
			}
		}
	}
}

void ScrollBar::_UpdateArrowButtons(void* pData)
{
	ScrollBar* scrollBar = static_cast<ScrollBar*>(pData);

	if (scrollBar)
	{
		scrollBar->UpdateArrowButtons();
	}
}

void ScrollBar::UpdateArrowButtons()
{
	float updateSpeed = 0.01f;

	if (IsSelectedLeft())
	{
		if (m_scrollDirection == ScrollBarDirection::Horizontal)
		{
			m_scrollPosition -= updateSpeed;
		}
		else
		{
			m_scrollPosition += updateSpeed;
		}

		OnValueChanged();

		if (m_valueChangedCallback)
		{
			m_valueChangedCallback(m_pValueChangedCallbackData);
		}
	}

	if (IsSelectedRight())
	{
		if (m_scrollDirection == ScrollBarDirection::Horizontal)
		{
			m_scrollPosition += updateSpeed;
		}
		else
		{
			m_scrollPosition -= updateSpeed;
		}

		OnValueChanged();

		if (m_valueChangedCallback)
		{
			m_valueChangedCallback(m_pValueChangedCallbackData);
		}
	}

	ClampScrollBarPosition();
}