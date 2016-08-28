/*************************************************************************
> File Name: GUIWindow.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A window container class, defines basic window functionality.
> Created Time: 2016/08/15
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DirectDrawRectangle.h"
#include "Icon.h"
#include "MultiTextureIcon.h"
#include "OpenGLGUI.h"
#include "GUIWindow.h"

// Constructor, Destructor
GUIWindow::GUIWindow(Renderer* pRenderer, unsigned int GUIFont, const std::string& title) :
	Container(pRenderer), m_GUIFont(GUIFont)
{
	m_titleBar = new TitleBar(pRenderer, GUIFont, title);

	// We created the title bar, so we own it (Must delete it)
	m_ownsTitleBar = true;

	m_titleBar->SetParent(this); // The base class component parent functionality
	m_titleBar->SetGUIWindowParent(this);
	m_titleBar->SetDepth(2.0f);

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

	Container::Add(m_titleBar);

	m_minimized = false;

	m_allowMoving = true;
	m_allowClosing = true;
	m_allowMinimizing = true;
	m_allowScrolling = true;
	m_snapToWindow = true;

	m_bRenderTitleBar = true;
	m_bRenderWindowBackground = true;

	m_applicationBorderLeft = 0;
	m_applicationBorderRight = 0;
	m_applicationBorderTop = 0;
	m_applicationBorderBottom = 0;

	m_outlineRender = false;

	m_pParentGUI = nullptr;
}

GUIWindow::GUIWindow(Renderer* pRenderer, unsigned int GUIFont, TitleBar* ptitleBar) :
	Container(pRenderer), m_GUIFont(GUIFont)
{
	m_titleBar = ptitleBar;

	// We didn't create the title bar, so we don't own it. (Cant delete it)
	m_ownsTitleBar = false;

	m_titleBar->SetParent(this);  // The base class component parent functionality
	m_titleBar->SetGUIWindowParent(this);
	m_titleBar->SetDepth(2.0f);

	m_pBackgroundIcon = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle *pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pBackgroundIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	Container::Add(m_pBackgroundIcon);

	Container::Add(m_titleBar);

	m_minimized = false;

	m_allowMoving = true;
	m_allowClosing = true;
	m_allowMinimizing = true;
	m_allowScrolling = true;

	m_applicationBorderLeft = 0;
	m_applicationBorderRight = 0;
	m_applicationBorderTop = 0;
	m_applicationBorderBottom = 0;

	m_pParentGUI = nullptr;

	m_outlineRender = false;
}

GUIWindow::~GUIWindow()
{
	if (m_pParentGUI)
	{
		m_pParentGUI->RemoveWindow(this);
	}

	if (m_ownsTitleBar)
	{
		// If we own the title bar, we are allowed to delete it
		delete m_titleBar;
	}

	Container::Remove(m_pBackgroundIcon);
	delete m_pBackgroundIcon;

	m_vpGUIWindowList.clear();
}

void GUIWindow::AddEventListeners()
{
	AddMouseListener(this);

	// Also add all children's event listeners
	Container::AddEventListeners();

	//m_titleBar->AddEventListeners();
}

void GUIWindow::RemoveEventListeners()
{
	RemoveMouseListener(this);

	// Also remove all children's event listeners
	Container::RemoveEventListeners();

	//m_titleBar->RemoveEventListeners();
}

bool GUIWindow::IsRootContainer() const
{
	return true;
}

void GUIWindow::AddGUIWindow(GUIWindow *window)
{
	window->SetParent(this);

	window->SetDepth(m_depth + 1.0f);

	m_vpGUIWindowList.push_back(window);

	// Sort the GUIWindow vector list, by depth
	DepthSortGUIWindowChildren();
}

void GUIWindow::RemoveGUIWindow(GUIWindow *window)
{
	// Find this window in the vector
	auto iter = find(m_vpGUIWindowList.begin(), m_vpGUIWindowList.end(), window);

	if (iter != m_vpGUIWindowList.end())
	{
		(*iter)->SetParent(nullptr);

		// Erase the window
		m_vpGUIWindowList.erase(iter);
	}
}

void GUIWindow::RemoveAllGUIWindows()
{
	m_vpGUIWindowList.clear();
}

const GUIWindowList& GUIWindow::GetGUIWindows() const
{
	return m_vpGUIWindowList;
}

TitleBar* GUIWindow::GetTitleBar() const
{
	return m_titleBar;
}

void GUIWindow::SetTitleBarDimensions(int xOffset, int yOffset, int width, int height) const
{
	m_titleBar->SetDimensions(xOffset, GetDimensions().height + yOffset, width, height);
}

void GUIWindow::SetTitleOffset(int xOffset, int yOffset) const
{
	m_titleBar->SetTitleOffset(xOffset, yOffset);
}

void GUIWindow::SetTitlebarBackgroundIcon(RenderRectangle* icon) const
{
	m_titleBar->SetBackgroundIcon(icon);
}

void GUIWindow::SetBackgroundIcon(RenderRectangle* icon)
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

void GUIWindow::AddComponent(Component* component)
{
	Container::Add(component);

	component->SetVisible(IsVisible());

	component->SetAudio(m_audio);
	component->SetAudioVolume(m_audioVolume);
}

void GUIWindow::RemoveComponent(Component *component)
{
	component->RemoveEventListeners();

	Container::Remove(component);
}

void GUIWindow::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	// Also set the background
	m_pBackgroundIcon->SetDimensions(0, 0, width, height);

	// Also set the title bar dimensions
	int titleBarSpacer = 1;
	int titleBarHeight = m_pRenderer->GetFreeTypeTextHeight(m_GUIFont, "a");
	m_titleBar->SetDimensions(0, height, width, (titleBarHeight + titleBarSpacer * 2));
}

void GUIWindow::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void GUIWindow::SetLocation(int x, int y)
{
	int widthToUse = m_dimensions.width;
	int heightAdjustment = 0;

	if (m_minimized)
	{
		widthToUse = m_titleBar->GetDimensions().width;
		heightAdjustment = m_dimensions.height;
	}

	if (x < 1 + m_applicationBorderLeft)
	{
		x = 1 + m_applicationBorderLeft;
	}
	if (x + 1 + widthToUse > m_applicationWidth - m_applicationBorderRight)
	{
		x = m_applicationWidth - 1 - widthToUse - m_applicationBorderRight;
	}
	if (y < 0 - heightAdjustment + m_applicationBorderBottom)
	{
		y = 0 - heightAdjustment + m_applicationBorderBottom;
	}
	if (y + 1 + m_dimensions.height + m_titleBar->GetDimensions().height > m_applicationHeight - m_applicationBorderTop)
	{
		y = m_applicationHeight - 1 - m_dimensions.height - m_titleBar->GetDimensions().height - m_applicationBorderTop;
	}

	m_dimensions.x = x;
	m_dimensions.y = y;
}

void GUIWindow::SetLocation(const Point& p)
{
	int x = p.x;
	int y = p.x;

	SetLocation(x, y);
}

void GUIWindow::SetTitle(const std::string& title) const
{
	m_titleBar->SetTitle(title);
}

std::string GUIWindow::GetTitle() const
{
	return m_titleBar->GetTitle();
}

void GUIWindow::SetDebugRender(bool debug)
{
	Container::SetDebugRender(debug);

	m_titleBar->SetDebugRender(debug);

	for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
	{
		(*iter)->SetDebugRender(debug);
	}
}

void GUIWindow::SetOutlineRender(bool outline)
{
	m_outlineRender = outline;

	m_titleBar->SetOutlineRender(outline);
}

void GUIWindow::Show()
{
	SetVisible(true);
	m_titleBar->SetVisible(true);

	// Show all our children components
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		(*iter)->SetVisible(true);
	}

	// Show all our children GUI Windows
	for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
	{
		(*iter)->SetVisible(true);
	}
}

void GUIWindow::Hide()
{
	SetVisible(false);
	m_titleBar->SetVisible(false);

	// Hide all our children components
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		(*iter)->SetVisible(false);
	}

	// Hide all our children GUI Windows
	for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
	{
		(*iter)->SetVisible(false);
	}
}

bool GUIWindow::GetMinimized() const
{
	return m_minimized;
}

void GUIWindow::SetMinimized(bool minimized)
{
	m_minimized = minimized;

	if (m_minimized)
	{
		// Hide all our children components when we are minimized
		for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
		{
			(*iter)->SetVisible(false);
			(*iter)->SetEnabled(false);
		}
	}
	else
	{
		// Else show all our children components
		for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
		{
			(*iter)->SetVisible(true);
			(*iter)->SetEnabled(true);
		}
	}

	m_titleBar->SetVisible(true);
	m_titleBar->SetEnabled(true);

	SetLocation(GetLocation().x, GetLocation().y);
}

void GUIWindow::SetMinimizedDefaultIcon(RenderRectangle* icon)
{
	m_titleBar->GetMinimizeButton()->SetDefaultIcon(icon);
}

void GUIWindow::SetMinimizedSelectedIcon(RenderRectangle* icon)
{
	m_titleBar->GetMinimizeButton()->SetSelectedIcon(icon);
}

void GUIWindow::SetMinimizedHoverIcon(RenderRectangle* icon)
{
	m_titleBar->GetMinimizeButton()->SetHoverIcon(icon);
}

void GUIWindow::SetMinimizedDisabledIcon(RenderRectangle* icon)
{
	m_titleBar->GetMinimizeButton()->SetDisabledIcon(icon);
}

void GUIWindow::SetCloseDefaultIcon(RenderRectangle* icon)
{
	m_titleBar->GetCloseButton()->SetDefaultIcon(icon);
}

void GUIWindow::SetCloseSelectedIcon(RenderRectangle* icon)
{
	m_titleBar->GetCloseButton()->SetSelectedIcon(icon);
}

void GUIWindow::SetCloseHoverIcon(RenderRectangle* icon)
{
	m_titleBar->GetCloseButton()->SetHoverIcon(icon);
}

void GUIWindow::SetCloseDisabledIcon(RenderRectangle* icon)
{
	m_titleBar->GetCloseButton()->SetDisabledIcon(icon);
}

void GUIWindow::AllowMoving(bool val)
{
	m_allowMoving = val;
}

void GUIWindow::AllowClosing(bool val)
{
	m_allowClosing = val;

	m_titleBar->AllowClosing(val);
}

void GUIWindow::AllowMinimizing(bool val)
{
	m_allowMinimizing = val;

	m_titleBar->AllowMinimizing(val);
}

void GUIWindow::AllowScrolling(bool val)
{
	m_allowScrolling = val;
}

void GUIWindow::SnapToApplication(bool val)
{
	m_snapToWindow = val;
}

void GUIWindow::SetApplicationDimensions(int width, int height)
{
	m_applicationWidth = width;
	m_applicationHeight = height;
}

void GUIWindow::SetApplicationBorder(int left, int right, int top, int bottom)
{
	m_applicationBorderLeft = left;
	m_applicationBorderRight = right;
	m_applicationBorderTop = top;
	m_applicationBorderBottom = bottom;
}

void GUIWindow::SetRenderTitleBar(bool lbRender)
{
	m_bRenderTitleBar = lbRender;

	if (m_bRenderTitleBar)
	{
		m_titleBar->SetVisible(true);
	}
	else
	{
		m_titleBar->SetVisible(false);
	}
}

void GUIWindow::SetRenderWindowBackground(bool lbRender)
{
	m_bRenderWindowBackground = lbRender;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void GUIWindow::DepthSortGUIWindowChildren() const
{

}

ComponentType GUIWindow::GetComponentType() const
{
	return ComponentType::GUIWindow;
}

void GUIWindow::SetGUIParent(OpenGLGUI* pParent)
{
	m_pParentGUI = pParent;
}

void GUIWindow::SetFocusWindow()
{
	if (m_pParentGUI)
	{
		m_pParentGUI->SetFocusedWindow(this);
	}
}

void GUIWindow::Update(float deltaTime)
{
	// Update our components
	Container::Update(deltaTime);

	// Update all the windows which we contain
	for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
	{
		if ((*iter)->IsVisible())
		{
			(*iter)->Update(deltaTime);
		}
	}
}

// < Operator (Used for GUIWindow depth sorting)
bool GUIWindow::operator<(const GUIWindow& w) const
{
	return GetDepth() < w.GetDepth();
}

bool GUIWindow::DepthLessThan(const GUIWindow* lhs, const GUIWindow* rhs)
{
	if (*lhs < *rhs)
	{
		return true;
	}
	if (*rhs < *lhs)
	{
		return false;
	}

	return *lhs < *rhs;
}

void GUIWindow::MousePressed(const MouseEvent& event)
{
	m_pParentGUI->SetFocusedWindow(this);
}

void GUIWindow::DrawSelf()
{
	m_pBackgroundIcon->SetVisible(false);

	if (m_outlineRender == false)
	{
		if (!m_minimized && m_bRenderWindowBackground)
		{
			m_pBackgroundIcon->SetVisible(true);
		}
	}

	if (m_outlineRender)
	{
		int containerWidth = m_dimensions.width;
		int containerHeight = m_dimensions.height;
		float depth = m_depth;

		float containerX1 = 0.5f;
		float containerX2 = static_cast<float>(containerWidth);
		float containerY1 = 0.5f;
		float containerY2 = static_cast<float>(containerHeight);

		float outlineX1 = -0.5f;
		float outlineX2 = containerWidth + 0.5f;
		float outlineY1 = -0.0f;
		float outlineY2 = containerHeight + 0.5f;

		if (!m_minimized && m_bRenderWindowBackground)
		{
			// Draw the background
			m_pRenderer->PushMatrix();

			m_pRenderer->SetRenderMode(RenderMode::SOLID);
			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

			m_pRenderer->ImmediateColorAlpha(0.288f, 0.288f, 0.288f, 1.0f);
			m_pRenderer->ImmediateVertex(containerX1, containerY1, depth);
			m_pRenderer->ImmediateVertex(containerX2, containerY1, depth);
			m_pRenderer->ImmediateColorAlpha(0.784f, 0.784f, 0.784f, 1.0f);
			m_pRenderer->ImmediateVertex(containerX2, containerY2, depth);
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

	if (m_minimized)
	{
		// Draw the window title
		if (m_bRenderTitleBar)
		{
			m_titleBar->Draw();
		}
	}

	// Draw the GUIWindow's children before the title bar 
	DrawChildrenFirst();
}

void GUIWindow::DrawChildrenFirst()
{
	if (!m_minimized)
	{
		// Draw our components
		Container::DrawChildren();
	
		// Draw all the windows which we contain
		for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
		{
			if ((*iter)->IsVisible())
			{
				(*iter)->Draw();
			}
		}
	}
}

void GUIWindow::DrawChildren()
{

}