/*************************************************************************
> File Name: DraggableRenderRectangle.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A Draggable render rectangle widget.
> Created Time: 2016/08/10
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DirectDrawRectangle.h"
#include "DraggableRenderRectangle.h"
#include "GUIWindow.h"
#include "Icon.h"
#include "Label.h"
#include "MultiTextureIcon.h"

// Constructor, Destructor
DraggableRenderRectangle::DraggableRenderRectangle(Renderer* pRenderer) :
	Container(pRenderer)
{
	m_pIcon = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	Container::Add(m_pIcon);

	m_dragging = false;

	m_nextX = 0;
	m_nextY = 0;

	m_pressedCallback = nullptr;
	m_pPressedCallbackData = nullptr;

	m_releasedCallback = nullptr;
	m_pReleasedCallbackData = nullptr;

	m_enterCallback = nullptr;
	m_pEnterCallbackData = nullptr;

	m_exitCallback = nullptr;
	m_pExitCallbackData = nullptr;
}

DraggableRenderRectangle::~DraggableRenderRectangle()
{
	Container::Remove(m_pIcon);

	delete m_pIcon;
	m_pIcon = nullptr;

	for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
	{
		Container::Remove(m_vpAddedComponentList[i]);

		delete m_vpAddedComponentList[i];
		m_vpAddedComponentList[i] = nullptr;
	}
	m_vpAddedComponentList.clear();
}

void DraggableRenderRectangle::SetIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pIcon)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pIcon);

		delete m_pIcon;
		m_pIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pIcon = pNewDirectDraw;
	}

	// Re-add the icon to the component list
	Add(m_pIcon);
}

void DraggableRenderRectangle::AddText(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string &label, Color color, int xOffset, int yOffset, bool outline, Color outlineColor)
{
	Label* pNewLabel = new Label(pRenderer, GUIFont, label, color);
	pNewLabel->SetLocation(xOffset, yOffset);
	pNewLabel->SetDepth(pNewLabel->GetDepth() + 1.0f);

	if (outline)
	{
		pNewLabel->SetOutline(true);
		pNewLabel->SetOutlineColor(outlineColor);
		pNewLabel->SetOutlineFont(outlineGUIFont);
	}

	m_vpAddedComponentList.push_back(pNewLabel);

	Add(pNewLabel);
}

void DraggableRenderRectangle::AddIcon(Renderer* pRenderer, const std::string &fileName, int texWidth, int texHeight, int width, int height, int xOffset, int yOffset, float depth)
{
	// texWidth and texHeight should always be a power of 2.
	Icon* pNewIcon = new Icon(pRenderer, fileName, texWidth, texHeight);
	pNewIcon->SetDimensions(xOffset, yOffset, width, height);
	pNewIcon->SetDepth(depth);

	m_vpAddedComponentList.push_back(pNewIcon);

	Add(pNewIcon);
}

void DraggableRenderRectangle::RemoveIcon(const std::string& fileName)
{
	bool removed = false;
	for (size_t i = 0; i < m_vpAddedComponentList.size() && removed == false; ++i)
	{
		if (m_vpAddedComponentList[i]->GetComponentType() == ComponentType::Icon)
		{
			Icon* pIcon = static_cast<Icon*>(m_vpAddedComponentList[i]);	
			if (pIcon->GetFileName() == fileName)
			{
				Remove(pIcon);

				auto iter = find(m_vpAddedComponentList.begin(), m_vpAddedComponentList.end(), pIcon);
				if (iter != m_vpAddedComponentList.end())
				{
					// Erase the component
					m_vpAddedComponentList.erase(iter);
				}

				delete pIcon;

				removed = true;
			}
		}
	}
}

void DraggableRenderRectangle::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	// Also set the dimensions of the icon
	m_pIcon->SetDimensions(0, 0, width, height);
}

void DraggableRenderRectangle::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void DraggableRenderRectangle::AddEventListeners()
{
	Container::AddEventListeners();

	AddMouseListener(this);
}

void DraggableRenderRectangle::RemoveEventListeners()
{
	Container::RemoveEventListeners();

	RemoveMouseListener(this);
}

void DraggableRenderRectangle::SetPressedCallBackFunction(std::function<void(void*)> func)
{
	m_pressedCallback = func;
}

void DraggableRenderRectangle::SetPressedCallBackData(void* pData)
{
	m_pPressedCallbackData = pData;
}

void DraggableRenderRectangle::SetReleasedCallBackFunction(std::function<void(void*)> func)
{
	m_releasedCallback = func;
}

void DraggableRenderRectangle::SetReleasedCallBackData(void* pData)
{
	m_pReleasedCallbackData = pData;
}

void DraggableRenderRectangle::SetEnterCallBackFunction(std::function<void(void*)> func)
{
	m_enterCallback = func;
}

void DraggableRenderRectangle::SetEnterCallBackData(void* pData)
{
	m_pEnterCallbackData = pData;
}

void DraggableRenderRectangle::SetExitCallBackFunction(std::function<void(void*)> func)
{
	m_exitCallback = func;
}

void DraggableRenderRectangle::SetExitCallBackData(void* pData)
{
	m_pExitCallbackData = pData;
}

void DraggableRenderRectangle::MouseEntered(const MouseEvent& event)
{
	if (IsEnabled() == false)
	{
		return;
	}

	OnMouseEnter();

	// Call the callback function
	if (m_enterCallback)
	{
		m_enterCallback(m_pEnterCallbackData);
	}
}

void DraggableRenderRectangle::MouseExited(const MouseEvent& event)
{
	if (IsEnabled() == false)
	{
		return;
	}

	OnMouseExit();

	// Call the callback function
	if (m_exitCallback)
	{
		m_exitCallback(m_pExitCallbackData);
	}
}

void DraggableRenderRectangle::MousePressed(const MouseEvent& event)
{
	if (IsEnabled() == false)
	{
		return;
	}

	if (GetParent() != nullptr && GetParent()->GetComponentType() == ComponentType::GUIWindow)
	{
		GUIWindow* pParentWindow = static_cast<GUIWindow*>(GetParent());
		pParentWindow->SetFocusWindow();
	}

	m_dragging = true;

	OnMousePressed();

	// Call the callback function
	if (m_pressedCallback)
	{
		m_pressedCallback(m_pPressedCallbackData);
	}
}

void DraggableRenderRectangle::MouseReleased(const MouseEvent& event)
{
	if (IsEnabled() == false)
	{
		return;
	}

	m_dragging = false;

	OnMouseReleased();

	// Call the callback function
	if (m_releasedCallback)
	{
		m_releasedCallback(m_pReleasedCallbackData);
	}
}

void DraggableRenderRectangle::MouseClicked(const MouseEvent& event)
{
	OnMouseClicked();
}

void DraggableRenderRectangle::MouseDragged(const MouseEvent& event)
{
	if (IsEnabled() == false)
	{
		return;
	}

	if (event.GetSource() == this)
	{
		if (m_dragging)
		{
			Point location = GetLocation();

			for (Component* parent = GetParent(); parent != nullptr;)
			{
				Point parentLocation = parent->GetLocation();

				parent = parent->GetParent();
			}

			m_nextX = location.x + event.GetX();
			m_nextY = location.y + event.GetY();
			SetLocation(m_nextX, m_nextY);
		}
	}

	OnMouseDragged();
}

void DraggableRenderRectangle::MouseDraggedOutside(const MouseEvent& event)
{
	OnMouseDraggedOutside();
}

ComponentType DraggableRenderRectangle::GetComponentType() const
{
	return ComponentType::DraggableRenderRectangle;
}

void DraggableRenderRectangle::OnMouseEnter()
{
	// Do nothing
}

void DraggableRenderRectangle::OnMouseExit()
{
	// Do nothing
}

void DraggableRenderRectangle::OnMousePressed()
{
	// Do nothing
}

void DraggableRenderRectangle::OnMouseReleased()
{
	// Do nothing
}

void DraggableRenderRectangle::OnMouseClicked()
{
	// Do nothing
}

void DraggableRenderRectangle::OnMouseDragged()
{
	// Do nothing
}

void DraggableRenderRectangle::OnMouseDraggedOutside()
{
	// Do nothing
}

void DraggableRenderRectangle::DrawSelf()
{
	// Do nothing
}