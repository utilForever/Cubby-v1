/*************************************************************************
> File Name: TitleBar.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A title bar that is used for GUI windows, displays the window
>    title and any buttons for close, minimize, etc..
> Created Time: 2016/08/15
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DirectDrawRectangle.h"
#include "GUIWindow.h"
#include "Icon.h"
#include "MultiTextureIcon.h"
#include "TitleBar.h"

// Constructor, Destructor
TitleBar::TitleBar(Renderer* pRenderer, unsigned int GUIFont, const std::string& title) :
	Container(pRenderer), m_title(pRenderer, GUIFont, title, Color(1.0f, 1.0f, 1.0f))
{
	m_closeButton = new GUIWindowCloseButton(pRenderer, GUIFont, "Close");
	m_minimizeButton = new GUIWindowMinimizeButton(pRenderer, GUIFont, "Minimize");

	m_ownsButtons = true;

	m_pBackgroundIcon = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pBackgroundIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	Container::Add(m_pBackgroundIcon);

	int titleBarSpace = 1;
	m_title.SetLocation(titleBarSpace, titleBarSpace);
	m_title.SetDepth(2.0f);

	Container::Add(&m_title);
	Container::Add(m_closeButton);
	Container::Add(m_minimizeButton);

	m_nextX = 0;
	m_nextY = 0;

	m_lastX = 0;
	m_lastY = 0;

	m_dragging = false;
	m_firstOutsideDrag = false;

	m_outlineRender = false;
}

TitleBar::TitleBar(Renderer* pRenderer, unsigned int GUIFont, const std::string& title, GUIWindowCloseButton* pCloseButton, GUIWindowMinimizeButton* pMinimizeButton) :
	Container(pRenderer), m_title(pRenderer, GUIFont, title, Color(1.0f, 1.0f, 1.0f))
{
	m_closeButton = pCloseButton;
	m_minimizeButton = pMinimizeButton;

	m_ownsButtons = false;

	m_pBackgroundIcon = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pBackgroundIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	Container::Add(m_pBackgroundIcon);

	int titleBarSpace = 1;
	m_title.SetLocation(titleBarSpace, titleBarSpace);
	m_title.SetDepth(2.0f);

	Container::Add(&m_title);
	Container::Add(m_closeButton);
	Container::Add(m_minimizeButton);

	m_nextX = 0;
	m_nextY = 0;

	m_lastX = 0;
	m_lastY = 0;

	m_dragging = false;
	m_firstOutsideDrag = false;

	m_outlineRender = false;
}

TitleBar::~TitleBar()
{
	if (m_ownsButtons)
	{
		delete m_closeButton;
		delete m_minimizeButton;
	}

	Container::Remove(m_pBackgroundIcon);
	delete m_pBackgroundIcon;
}

void TitleBar::AddEventListeners()
{
	Container::AddEventListeners();

	//m_closeButton->AddEventListeners();
	//m_minimizeButton->AddEventListeners();

	AddMouseListener(this);
}

void TitleBar::RemoveEventListeners()
{
	Container::RemoveEventListeners();

	//m_closeButton->RemoveEventListeners();
	//m_minimizeButton->RemoveEventListeners();

	RemoveMouseListener(this);
}

GUIWindowCloseButton* TitleBar::GetCloseButton() const
{
	return m_closeButton;
}

GUIWindowMinimizeButton* TitleBar::GetMinimizeButton() const
{
	return m_minimizeButton;
}

void TitleBar::SetGUIWindowParent(GUIWindow* pParent)
{
	m_pParent = pParent;

	// Also set the close and minimize buttons to have the same GUI window parent
	m_closeButton->SetGUIWindowParent(pParent);
	m_minimizeButton->SetGUIWindowParent(pParent);
}

void TitleBar::MouseEntered(const MouseEvent& event)
{
	m_firstOutsideDrag = false;

	OnMouseEnter();
}

void TitleBar::MouseExited(const MouseEvent& event)
{
	OnMouseExit();
}

void TitleBar::MousePressed(const MouseEvent& event)
{
	if (m_pParent != nullptr)
	{
		m_pParent->SetFocusWindow();
	}

	m_dragging = true;

	OnMousePressed();
}

void TitleBar::MouseReleased(const MouseEvent& event)
{
	m_dragging = false;

	OnMouseReleased();
}

void TitleBar::MouseClicked(const MouseEvent& event)
{
	OnMouseClicked();
}

void TitleBar::MouseDragged(const MouseEvent& event)
{
	// Don't allow moving if we have disabled this
	if (m_pParent->m_allowMoving)
	{
		if (event.GetSource() == this)
		{
			if (m_dragging)
			{
				m_nextX = GetParent()->GetLocationOnScreen().x + event.GetX();
				m_nextY = GetParent()->GetLocationOnScreen().y + event.GetY();
				GetParent()->SetLocation(m_nextX, m_nextY);
			}
		}
	}

	OnMouseDragged();
}

void TitleBar::MouseDraggedOutside(const MouseEvent& event)
{
	if (!m_firstOutsideDrag)
	{
		m_lastX = event.GetX();
		m_lastY = event.GetY();
		m_firstOutsideDrag = true;
	}

	if (m_pParent->m_allowMoving)
	{
		if (event.GetSource() == this)
		{
			if (m_dragging)
			{
				int newX = event.GetX();
				int newY = event.GetY();

				int changeX = newX - m_lastX;
				int changeY = newY - m_lastY;

				if (changeX == 0 && changeY == 0)
				{
					return;
				}

				m_nextX = GetParent()->GetLocationOnScreen().x + changeX;
				m_nextY = GetParent()->GetLocationOnScreen().y + changeY;
				GetParent()->SetLocation(m_nextX, m_nextY);

				m_lastX = newX;
				m_lastY = newY;
			}
		}
	}

	OnMouseDraggedOutside();
}

void TitleBar::DrawSelf()
{
	if (m_outlineRender)
	{
		int containerWidth = m_dimensions.width;
		int containerHeight = m_dimensions.height;
		float depth = GetDepth();

		float containerX1 = 0.5f;
		float containerX2 = static_cast<float>(containerWidth);
		float containerY1 = 0.5f;
		float containerY2 = static_cast<float>(containerHeight);

		float outlineX1 = -0.5f;
		float outlineX2 = containerWidth + 0.5f;
		float outlineY1 = -0.0f;
		float outlineY2 = containerHeight + 0.5f;

		// Draw the background
		m_pRenderer->PushMatrix();
		
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
		
		m_pRenderer->ImmediateColorAlpha(0.039f, 0.141f, 0.415f, 1.0f);
		m_pRenderer->ImmediateVertex(containerX1, containerY1, depth);
		m_pRenderer->ImmediateColorAlpha(0.650f, 0.792f, 0.941f, 1.0f);
		m_pRenderer->ImmediateVertex(containerX2, containerY1, depth);
		m_pRenderer->ImmediateVertex(containerX2, containerY2, depth);
		m_pRenderer->ImmediateColorAlpha(0.039f, 0.141f, 0.415f, 1.0f);
		m_pRenderer->ImmediateVertex(containerX1, containerY2, depth);
		
		m_pRenderer->DisableImmediateMode();
		
		m_pRenderer->PopMatrix();

		// Draw the outline
		m_pRenderer->PushMatrix();
		
		m_pRenderer->SetLineWidth(1.0f);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINE_LOOP);
		
		m_pRenderer->ImmediateColorAlpha(0.650f, 0.792f, 0.941f, 1.0f);
		m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);
		m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);
		m_pRenderer->ImmediateVertex(outlineX2, outlineY2, depth);
		m_pRenderer->ImmediateVertex(outlineX1, outlineY2, depth);
		
		m_pRenderer->DisableImmediateMode();
		
		m_pRenderer->PopMatrix();
	}
}

void TitleBar::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	// Also set the background
	m_pBackgroundIcon->SetDimensions(0, 0, width, height);

	// Also set the dimensions of the close button
	int distanceFromBorder = 2;
	int buttonHeight = height - (distanceFromBorder * 2);
	int buttonX = width;

	if (m_pParent->m_allowClosing)
	{
		buttonX -= (buttonHeight + distanceFromBorder);
		m_closeButton->SetDimensions(buttonX, distanceFromBorder, buttonHeight, buttonHeight);
	}

	if (m_pParent->m_allowMinimizing)
	{
		buttonX -= (buttonHeight + distanceFromBorder * 2);
		m_minimizeButton->SetDimensions(buttonX, distanceFromBorder, buttonHeight, buttonHeight);
	}
}

void TitleBar::SetTitleOffset(int xOffset, int yOffset)
{
	m_title.SetLocation(xOffset, yOffset);
}

void TitleBar::SetBackgroundIcon(RenderRectangle* icon)
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

	// Check what type of render rectangle we have been given, and then assign our new data
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
}

void TitleBar::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void TitleBar::SetTitle(const std::string &title)
{
	m_title.SetText(title);
}

std::string TitleBar::GetTitle() const
{
	return m_title.GetText();
}

void TitleBar::SetOutlineRender(bool outline)
{
	m_outlineRender = outline;

	m_pBackgroundIcon->SetVisible(outline == false);
}

void TitleBar::AllowMinimizing(bool val) const
{
	m_minimizeButton->SetVisible(val);
}

void TitleBar::AllowClosing(bool val) const
{
	m_closeButton->SetVisible(val);
}

ComponentType TitleBar::GetComponentType() const
{
	return ComponentType::TitleBar;
}

void TitleBar::OnMouseEnter()
{
	// Do nothing
}

void TitleBar::OnMouseExit()
{
	// Do nothing
}

void TitleBar::OnMousePressed()
{
	// Do nothing
}

void TitleBar::OnMouseReleased()
{
	// Do nothing
}

void TitleBar::OnMouseClicked()
{
	// Do nothing
}

void TitleBar::OnMouseDragged()
{
	// Do nothing
}

void TitleBar::OnMouseDraggedOutside()
{
	// Do nothing
}