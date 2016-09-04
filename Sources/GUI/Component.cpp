/*************************************************************************
> File Name: Component.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An abstract class for GUI components.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include "FocusEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "SelectionManager.h"

#include "Component.h"

// Constructor, Destructor
Component::Component(Renderer* pRenderer)
	: m_pRenderer(pRenderer)
{
	m_visible = true;
	m_enabled = true;
	m_focused = false;

	m_debugRender = false;

	m_transparency = 1.0f;

	m_depth = 1.0f;

	m_audio = true;
	m_audioVolume = 1.0f;

	m_pParent = nullptr;

	m_dimensions = Dimensions(0, 0, 0, 0);

	m_mouseListenerRemoved = false;
	m_keyListenerRemoved = false;
	m_focusListenerRemoved = false;
}

Component::~Component()
{
	// Do nothing
}

void Component::SetVisible(bool Visible)
{
	m_visible = Visible;
}

bool Component::IsVisible() const
{
	return m_visible;
}

void Component::SetEnabled(bool enabled)
{
	m_enabled = enabled;
}

bool Component::IsEnabled() const
{
	return m_enabled;
}

bool Component::IsParentVisible() const
{
	if (m_pParent == nullptr)
	{
		return IsVisible();
	}

	return m_pParent->IsParentVisible();
}

bool Component::IsParentEnabled() const
{
	if (m_pParent == nullptr)
	{
		return IsEnabled();
	}

	return m_pParent->IsParentEnabled();
}

void Component::SetTransparency(float transparency)
{
	m_transparency = transparency;
}

float Component::GetTransparency() const
{
	return m_transparency;
}

void Component::SetDepth(float depth)
{
	m_depth = depth;
}

float Component::GetDepth() const
{
	float depth = m_depth;

	if (m_pParent != nullptr)
	{
		depth += m_pParent->GetDepth();
	}

	return depth;
}

void Component::SetParent(Component *pParent)
{
	m_pParent = pParent;
}

Component* Component::GetParent() const
{
	return m_pParent;
}

ComponentType Component::GetComponentType() const
{
	return ComponentType::Component;
}

void Component::SetLocation(int x, int y)
{
	m_dimensions.x = x;
	m_dimensions.y = y;
}

void Component::SetLocation(const Point& p)
{
	m_dimensions.x = p.x;
	m_dimensions.y = p.y;
}

void Component::SetX(int x)
{
	m_dimensions.x = x;
}

void Component::SetY(int y)
{
	m_dimensions.y = y;
}

void Component::GetLocation(int& x, int& y) const
{
	x = m_dimensions.x;
	y = m_dimensions.y;
}

Point Component::GetLocation() const
{
	return Point(m_dimensions.x, m_dimensions.y);
}

void Component::SetSize(int width, int height)
{
	m_dimensions.width = width;
	m_dimensions.height = height;
}

void Component::GetSize(int& width, int& height) const
{
	width = m_dimensions.width;
	height = m_dimensions.height;
}

void Component::SetDebugRender(bool debug)
{
	m_debugRender = debug;
}

bool Component::IsDebugRender() const
{
	return m_debugRender;
}

Point Component::GetLocationOnScreen() const
{
	// Start with this component's location, and add all parents locations
	Point location = GetLocation();

	for (Component* parent = GetParent(); parent != nullptr;)
	{
		Point parentLocation = parent->GetLocation();

		location.x += parentLocation.x;
		location.y += parentLocation.y;

		parent = parent->GetParent();
	}

	return location;
}

void Component::SetDimensions(int x, int y, int width, int height)
{
	m_dimensions.x = x;
	m_dimensions.y = y;
	m_dimensions.width = width;
	m_dimensions.height = height;
}

void Component::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void Component::SetWidth(int width)
{
	m_dimensions.width = width;
}

void Component::SetHeight(int height)
{
	m_dimensions.height = height;
}

Dimensions Component::GetDimensions() const
{
	return m_dimensions;
}

Dimensions* Component::GetDimensionsPointer()
{
	return &m_dimensions;
}

void Component::AddEventListeners()
{
	// Do nothing
}

void Component::RemoveEventListeners()
{
	// Do nothing
}

bool Component::HasMouseListener() const
{
	return m_vpMouseListenerList.size() > 0;
}

void Component::AddMouseListener(MouseListener* listener)
{
	m_vpMouseListenerList.push_back(listener);
	SelectionManager::GetInstance()->AddComponent(this);
}

void Component::RemoveMouseListener(MouseListener* listener)
{
	m_vpMouseListenerList.erase(remove(m_vpMouseListenerList.begin(), m_vpMouseListenerList.end(), listener), m_vpMouseListenerList.end());
	SelectionManager::GetInstance()->RemoveComponent(this);

	m_mouseListenerRemoved = true;
}

void Component::RemoveMouseListeners()
{
	m_vpMouseListenerList.clear();
	SelectionManager::GetInstance()->RemoveComponent(this);

	m_mouseListenerRemoved = true;
}

void Component::ProcessMouseEvent(const MouseEvent& event)
{
	if (!IsEnabled())
	{
		return;
	}

	for (auto iter = m_vpMouseListenerList.begin(); iter != m_vpMouseListenerList.end(); ++iter)
	{
		switch (static_cast<MouseEventType>(event.GetID()))
		{
		case MouseEventType::PRESSED:
		{
			(*iter)->MousePressed(event);
			break;
		}
		case MouseEventType::RELEASED:
		{
			(*iter)->MouseReleased(event);
			break;
		}
		case MouseEventType::RELEASED_OUTSIDE:
		{
			(*iter)->MouseReleasedOutside(event);
			break;
		}
		case MouseEventType::ENTERED:
		{
			(*iter)->MouseEntered(event);
			break;
		}
		case MouseEventType::EXITED:
		{
			(*iter)->MouseExited(event);
			break;
		}
		case MouseEventType::CLICKED:
		{
			(*iter)->MouseClicked(event);
			break;
		}
		case MouseEventType::DRAGGED:
		{
			(*iter)->MouseDragged(event);
			break;
		}
		case MouseEventType::DRAGGED_OUTSIDE: // HACK
		{
			(*iter)->MouseDraggedOutside(event);
			break;
		}
		case MouseEventType::MOTION:
		{
			(*iter)->MouseMotion(event);
			break;
		}
		}

		if (m_mouseListenerRemoved)
		{
			// We have removed a mouse listener while processing this iterator, all following iterators will be invalid, lets just break out the process loop
			m_mouseListenerRemoved = false;
			break;
		}
	}
}

bool Component::HasKeyListener() const
{
	return(m_vpKeyListenerList.size() > 0);
}

void Component::AddKeyListener(KeyListener* listener)
{
	m_vpKeyListenerList.push_back(listener);
}

void Component::RemoveKeyListener(KeyListener* listener)
{
	m_vpKeyListenerList.erase(remove(m_vpKeyListenerList.begin(), m_vpKeyListenerList.end(), listener), m_vpKeyListenerList.end());

	m_keyListenerRemoved = true;
}

void Component::RemoveKeyListeners()
{
	m_vpKeyListenerList.clear();

	m_keyListenerRemoved = true;
}

void Component::ProcessKeyEvent(const KeyEvent& event)
{
	if (!IsEnabled())
	{
		return;
	}

	for (auto iter = m_vpKeyListenerList.begin(); iter != m_vpKeyListenerList.end(); ++iter)
	{
		switch (static_cast<KeyEventType>(event.GetID()))
		{
		case KeyEventType::PRESSED:
		{
			(*iter)->KeyPressed(event);
			break;
		}
		case KeyEventType::RELEASED:
		{
			(*iter)->KeyReleased(event);
			break;
		}
		case KeyEventType::TYPED:
		{
			(*iter)->KeyTyped(event);
			break;
		}

		case KeyEventType::CHARACTER_ENTERED:
		{
			(*iter)->CharacterEntered(event);
			break;
		}
		}

		if (m_keyListenerRemoved)
		{
			// We have removed a key listener while processing this iterator, all following iterators will be invalid, lets just break out the process loop
			m_keyListenerRemoved = false;
			break;
		}
	}
}

bool Component::HasFocusListener() const
{
	return(m_vpFocusListenerList.size() > 0);
}

void Component::AddFocusListener(FocusListener* listener)
{
	m_vpFocusListenerList.push_back(listener);
}

void Component::RemoveFocusListener(FocusListener* listener)
{
	m_vpFocusListenerList.erase(std::remove(m_vpFocusListenerList.begin(), m_vpFocusListenerList.end(), listener), m_vpFocusListenerList.end());

	m_focusListenerRemoved = true;
}

void Component::RemoveFocusListeners()
{
	m_vpFocusListenerList.clear();

	m_focusListenerRemoved = true;
}

void Component::ProcessFocusEvent(const FocusEvent& event)
{
	for (auto iter = m_vpFocusListenerList.begin(); iter != m_vpFocusListenerList.end(); ++iter)
	{
		switch (static_cast<FocusEventType>(event.GetID()))
		{
		case FocusEventType::LOST:
		{
			if (HasFocus())
			{
				(*iter)->FocusLost(event);
			}
			break;
		}
		case FocusEventType::GAINED:
		{
			if (!HasFocus())
			{
				(*iter)->FocusGained(event);
			}
			break;
		}
		}

		if (m_focusListenerRemoved)
		{
			// We have removed a focus listener while processing this iterator, all following iterators will be invalid, lets just break out the process loop
			m_focusListenerRemoved = false;
			break;
		}
	}

	if (static_cast<FocusEventType>(event.GetID()) == FocusEventType::GAINED)
	{
		if (!HasFocus())
		{
			SetFocus(true);
		}
	}
	else
	{
		if (HasFocus())
		{
			SetFocus(false);
		}
	}
}

bool Component::HasFocus() const
{
	return m_focused;
}

void Component::SetFocus(bool enable)
{
	m_focused = enable;
}

void Component::SetAudio(bool set)
{
	m_audio = set;
}

void Component::SetAudioVolume(float volume)
{
	m_audioVolume = volume;
}

bool Component::IsContainer() const
{
	return false;
}

bool Component::IsRootContainer() const
{
	return false;
}

void Component::Update(float deltaTime)
{
	assert(deltaTime >= 0.0f);
}

void Component::Draw()
{
	if (IsVisible() && m_dimensions.width > 0 && m_dimensions.height > 0)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->TranslateWorldMatrix(static_cast<float>(m_dimensions.x), static_cast<float>(m_dimensions.y), 0.0f);

		m_pRenderer->PushMatrix();

		// First draw ourself
		DrawSelf();

		// Draw the base component parts
		Component::DrawSelf();

		// Debug render
		if (m_debugRender)
		{
			DrawDebug();
		}

		// Then draw our children
		DrawChildren();

		m_pRenderer->PopMatrix();

		m_pRenderer->PopMatrix();
	}
}

// < Operator (Used for window pane sorting)
bool Component::operator<(const Component& w) const
{
	return GetDepth() < w.GetDepth();
}

bool Component::DepthLessThan(const Component* lhs, const Component* rhs)
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

void Component::DrawSelf()
{
	// Do nothing
}

void Component::DrawChildren()
{
	// Do nothing
}

void Component::DrawDebug() const
{
	int containerWidth = m_dimensions.width;
	int containerHeight = m_dimensions.height;
	float depth = m_depth + 2.0f;

	float outlineX1 = 0.5f;
	float outlineX2 = containerWidth + 0.5f;
	float outlineY1 = 0.5f;
	float outlineY2 = containerHeight + 0.5f;

	// Draw the outline
	m_pRenderer->PushMatrix();

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINE_LOOP);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
	m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);
	m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);
	m_pRenderer->ImmediateVertex(outlineX2, outlineY2, depth);
	m_pRenderer->ImmediateVertex(outlineX1, outlineY2, depth);
	m_pRenderer->DisableImmediateMode();

	m_pRenderer->PopMatrix();
}
