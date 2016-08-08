/*************************************************************************
> File Name: RenderRectangle.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A rectangle drawing base class, used to define abstract behavior for
> 	 drawing of rectangular bounded areas.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Icon.h"
#include "Label.h"
#include "RenderRectangle.h"

// Constructor, Destructor
RenderRectangle::RenderRectangle(Renderer* pRenderer) :
	Container(pRenderer)
{

}

RenderRectangle::~RenderRectangle()
{
	for (size_t i = 0; i < m_vpAddedComponentList.size(); ++i)
	{
		Container::Remove(m_vpAddedComponentList[i]);

		delete m_vpAddedComponentList[i];
		m_vpAddedComponentList[i] = nullptr;
	}

	m_vpAddedComponentList.clear();
}

void RenderRectangle::AddText(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color color, int xOffset, int yOffset, bool outline, Color outlineColor)
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

void RenderRectangle::AddIcon(Renderer* pRenderer, const std::string& fileName, int texWidth, int texHeight, int width, int height, int xOffset, int yOffset, float depth)
{
	// texWidth and texHeight should always be a power of 2.
	Icon* pNewIcon = new Icon(pRenderer, fileName, texWidth, texHeight);
	pNewIcon->SetDimensions(xOffset, yOffset, width, height);
	pNewIcon->SetDepth(depth);

	m_vpAddedComponentList.push_back(pNewIcon);

	Add(pNewIcon);
}

ComponentType RenderRectangle::GetComponentType() const
{
	return ComponentType::RenderRectangle;
}

void RenderRectangle::DrawSelf()
{
	// Do nothing
}

void RenderRectangle::DrawChildren()
{
	Container::DrawChildren();
}