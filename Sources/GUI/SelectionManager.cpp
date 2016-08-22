/*************************************************************************
> File Name: SelectionManager.coo
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Handles the picking of components, events are sent to the currently
>    selected component. This is worked out using component bounding boxes.
> Note
>    The actual algorithm for working out what component is currently
>    selected doesn't have to be mathematically, we could use the OpenGL
>    selection buffer, color picking or a number of other methods.
> Created Time: 2016/08/22
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "SelectionManager.h"
#include "GUIWindow.h"
#include "ScrollBar.h"
#include "FocusManager.h"

// Initialize the singleton instance
SelectionManager* SelectionManager::m_instance = nullptr;

SelectionManager* SelectionManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new SelectionManager;
	}

	return m_instance;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void SelectionManager::Destroy() const
{
	if (m_instance)
	{
		delete m_instance;
	}
}

void SelectionManager::ResetCurrentComponent() const
{
	if (m_foundComponent != nullptr)
	{
		MouseEvent exitEvent(m_foundComponent, static_cast<int>(MouseEventType::EXITED), m_mouseX, m_mouseY, 0);
		m_foundComponent->ProcessMouseEvent(exitEvent);
	}
}

SelectionManager::SelectionManager() :
	m_foundComponent(nullptr), m_hoverOverComponent(nullptr), m_firstClickedComponent(nullptr),
	m_isPressed(false), m_buttonPressed(0),
	m_mouseX(0), m_mouseY(0), m_lastX(0), m_lastY(0)
{

}

void SelectionManager::AddComponent(Component* component)
{
	m_vpComponentList.push_back(component);
}

void SelectionManager::RemoveComponent(Component* component)
{
	if (component == m_foundComponent)
	{
		m_foundComponent = nullptr;
	}
	if (component == m_hoverOverComponent)
	{
		m_hoverOverComponent = nullptr;
	}
	if (component == m_firstClickedComponent)
	{
		m_firstClickedComponent = nullptr;
	}

	auto iter = find(m_vpComponentList.begin(), m_vpComponentList.end(), component);
	if (iter != m_vpComponentList.end())
	{
		// Erase the component
		m_vpComponentList.erase(iter);
	}
}

void SelectionManager::Update(int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;

	m_foundComponent = GetComponentAt(x, y);
}

Component* SelectionManager::GetComponentAt(int x, int y)
{
	// TODO: This needs much work
	// Depth testing
	// Focus ordering
	
	Component* pWindowComponent = nullptr;
	float currentDepth = 0;

	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		// We have only found a selection if the component is enabled
		if ((*iter)->IsEnabled() && (*iter)->IsVisible() && (*iter)->IsParentEnabled() && (*iter)->IsParentVisible())
		{
			Point location = (*iter)->GetLocationOnScreen();
			Dimensions dimensions = (*iter)->GetDimensions();
			int left = location.x;
			int right = location.x + dimensions.width;
			int top = location.y + dimensions.height;
			int bottom = location.y;

			if ((x < right && x >= left) && (y <= top && y > bottom))
			{
				if ((*iter)->GetComponentType() == ComponentType::GUIWindow)
				{
					// Hack: If we have found a window component, then don't return it just yet, let us see if we can find something else
					GUIWindow* pWindow = static_cast<GUIWindow*>(*iter);

					if (!pWindow->GetMinimized())
					{
						// Only if we are not minimized... if we are minimized then don't return this window
						if (pWindow->GetDepth() > currentDepth)
						{
							currentDepth = pWindow->GetDepth();
							pWindowComponent = *iter;
						}

					}
				}
				else
				{
					if ((*iter)->GetDepth() > currentDepth)
					{
						if ((*iter)->GetParent() && (*iter)->GetParent()->GetComponentType() == ComponentType::ScrollBar)
						{
							// Check scroll area of parent
							ScrollBar* pScrollBar = static_cast<ScrollBar*>((*iter)->GetParent());

							Dimensions scrollbarParent;
							if (pScrollBar->GetParent() != nullptr)
							{
								scrollbarParent = pScrollBar->GetParent()->GetDimensions();
							}
							Dimensions scrollbar = pScrollBar->GetDimensions();
							Dimensions scrollArea = pScrollBar->GetScrollArea();
							int scrollleft = scrollbarParent.x + scrollbar.x + scrollArea.x;
							int scrollright = scrollbarParent.x + scrollbar.x + scrollArea.x + scrollArea.width;
							int scrolltop = scrollbarParent.y + scrollbar.y + scrollArea.y + scrollArea.height;
							int scrollbottom = scrollbarParent.y + scrollbar.y + scrollArea.y;

							if (x > scrollright || x < scrollleft || y > scrolltop || y < scrollbottom)
							{
								continue;
							}
						}

						currentDepth = (*iter)->GetDepth();
						pWindowComponent = *iter;
					}
				}
			}
		}
	}

	// If we haven't found any components, but we DO have a GUI window, then return that, else NOTHING
	return pWindowComponent;
}

Component* SelectionManager::GetCurrentComponent() const
{
	return m_foundComponent;
}

void SelectionManager::MouseMotion(int x, int y)
{
	// Mouse Dragging
	if ((m_foundComponent != nullptr) && (m_foundComponent == m_firstClickedComponent) && (m_isPressed))
	{
		if (m_lastX == 0 && m_lastY == 0)
		{
			m_lastX = x;
			m_lastY = y;
		}

		int draggedX = x - m_lastX;
		int draggedY = y - m_lastY;

		MouseEvent dragEvent(m_foundComponent, static_cast<int>(MouseEventType::DRAGGED), draggedX, draggedY, m_buttonPressed);
		m_foundComponent->ProcessMouseEvent(dragEvent);
	}
	else
	{
		// HACK - For components to work 'outside' of their boundaries
		if (m_firstClickedComponent && m_isPressed)
		{
			MouseEvent dragEvent(m_firstClickedComponent, static_cast<int>(MouseEventType::DRAGGED_OUTSIDE), x, y, m_buttonPressed);
			m_firstClickedComponent->ProcessMouseEvent(dragEvent);
		}
	}

	// Mouse motion
	if (m_foundComponent != nullptr)
	{
		MouseEvent motionEvent(m_foundComponent, static_cast<int>(MouseEventType::MOTION), x, y, 0);
		m_foundComponent->ProcessMouseEvent(motionEvent);
	}

	// Mouse enter and exit
	if (m_foundComponent != m_hoverOverComponent)
	{
		// Exit the current component that is set as hover
		if (m_hoverOverComponent != nullptr)
		{
			MouseEvent exitEvent(m_hoverOverComponent, static_cast<int>(MouseEventType::EXITED), m_mouseX, m_mouseY, 0);
			m_hoverOverComponent->ProcessMouseEvent(exitEvent);
		}

		// Enter the new component
		if (m_foundComponent != nullptr)
		{
			MouseEvent enterEvent(m_foundComponent, static_cast<int>(MouseEventType::ENTERED), m_mouseX, m_mouseY, 0);
			m_foundComponent->ProcessMouseEvent(enterEvent);

			if (m_foundComponent == m_firstClickedComponent && m_isPressed)
			{
				MouseEvent pressedEvent(m_foundComponent, static_cast<int>(MouseEventType::PRESSED), m_mouseX, m_mouseY, 0);
				m_foundComponent->ProcessMouseEvent(pressedEvent);
			}
		}

		m_hoverOverComponent = m_foundComponent;
	}
	else if (m_foundComponent == nullptr)
	{
		// Exit the current component that is set as hover
		if (m_hoverOverComponent != nullptr)
		{
			MouseEvent exitEvent(m_hoverOverComponent, static_cast<int>(MouseEventType::EXITED), m_mouseX, m_mouseY, 0);
			m_hoverOverComponent->ProcessMouseEvent(exitEvent);
		}
	}

	m_lastX = x;
	m_lastY = y;
}

void SelectionManager::MousePressed(int button)
{
	if (button != static_cast<int>(MouseButton::BUTTON1))
	{
		// Only allow GUI interactions with the left mouse button
		return;
	}

	if (!m_isPressed)
	{
		m_firstClickedComponent = m_foundComponent;
	}

	if (m_foundComponent != nullptr)
	{
		if (m_firstClickedComponent == m_foundComponent)
		{
			// Mouse button is pressed down
			MouseEvent mouseEvent(m_foundComponent, static_cast<int>(MouseEventType::PRESSED), m_mouseX, m_mouseY, button);
			m_foundComponent->ProcessMouseEvent(mouseEvent);
		}

		// Also set focus
		if (m_foundComponent->HasFocusListener())
		{
			// Only visible and enabled components can have focus
			if (m_foundComponent->IsEnabled())
			{
				FocusManager::GetInstance()->SetFocusOwner(m_foundComponent);
			}
		}
		else
		{
			FocusManager::GetInstance()->SetFocusOwner(nullptr);
		}
	}
	else
	{
		FocusManager::GetInstance()->SetFocusOwner(nullptr);
	}

	m_isPressed = true;
	m_buttonPressed = button;
}

void SelectionManager::MouseReleased(int button)
{
	if (button != static_cast<int>(MouseButton::BUTTON1))
	{
		// Only allow GUI interactions with the left mouse button
		return;
	}

	if (m_foundComponent != nullptr)
	{
		// Mouse button is released
		MouseEvent mouseEvent(m_foundComponent, static_cast<int>(MouseEventType::RELEASED), m_mouseX, m_mouseY, button);
		m_foundComponent->ProcessMouseEvent(mouseEvent);

		// Only allow a mouse click if we release on the same component that we initially clicked
		if (m_foundComponent == m_firstClickedComponent)
		{
			if (m_foundComponent != nullptr)
			{
				// Mouse button has been clicked
				MouseEvent mouseClickedEvent(m_foundComponent, static_cast<int>(MouseEventType::CLICKED), m_mouseX, m_mouseY, button);
				m_foundComponent->ProcessMouseEvent(mouseClickedEvent);
			}
		}
	}

	// HACK - For components to work 'outside' of their boundaries
	if ((m_firstClickedComponent != nullptr) && (m_firstClickedComponent != m_foundComponent))
	{
		// Mouse button is released
		MouseEvent mouseEvent(m_firstClickedComponent, static_cast<int>(MouseEventType::RELEASED_OUTSIDE), m_mouseX, m_mouseY, button);
		m_firstClickedComponent->ProcessMouseEvent(mouseEvent);

		m_firstClickedComponent = nullptr;
	}

	m_isPressed = false;
	m_buttonPressed = -1;

	m_lastX = 0;
	m_lastY = 0;
}