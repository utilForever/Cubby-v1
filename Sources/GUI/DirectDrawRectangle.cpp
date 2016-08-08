/*************************************************************************
> File Name: DirectDrawRectangle.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A rendered rectangle that is drawn directly by OpenGL, using primitives.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DirectDrawRectangle.h"

// Constructor, Destructor
DirectDrawRectangle::DirectDrawRectangle(Renderer* pRenderer) :
	RenderRectangle(pRenderer),
	m_lineSizeTop(0.0f), m_lineSizeBottom(0.0f),
	m_lineSizeLeft(0.0f), m_lineSizeRight(0.0f)
{

}

DirectDrawRectangle::~DirectDrawRectangle()
{

}

void DirectDrawRectangle::SetBackgroundColorTopLeft(Color color)
{
	m_backgroundColorTopLeft = color;
}

void DirectDrawRectangle::SetBackgroundColorTopRight(Color color)
{
	m_backgroundColorTopRight = color;
}

void DirectDrawRectangle::SetBackgroundColorBottomLeft(Color color)
{
	m_backgroundColorBottomLeft = color;
}

void DirectDrawRectangle::SetBackgroundColorBottomRight(Color color)
{
	m_backgroundColorBottomRight = color;
}

void DirectDrawRectangle::SetOutlineColorTop(Color color)
{
	m_outlineColorTop = color;
}

void DirectDrawRectangle::SetOutlineColorBottom(Color color)
{
	m_outlineColorBottom = color;
}

void DirectDrawRectangle::SetOutlineColorLeft(Color color)
{
	m_outlineColorLeft = color;
}

void DirectDrawRectangle::SetOutlineColorRight(Color color)
{
	m_outlineColorRight = color;
}

ComponentType DirectDrawRectangle::GetComponentType() const
{
	return ComponentType::DirectDrawRectangle;
}

void DirectDrawRectangle::DrawSelf()
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
	float outlineY1 = 0.0f;
	float outlineY2 = containerHeight + 0.5f;

	// Draw the background
	m_pRenderer->PushMatrix();

	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
	m_pRenderer->ImmediateColorAlpha(m_backgroundColorBottomLeft.GetRed(), m_backgroundColorBottomLeft.GetGreen(), m_backgroundColorBottomLeft.GetBlue(), m_backgroundColorBottomLeft.GetAlpha());
	m_pRenderer->ImmediateVertex(containerX1, containerY1, depth);
	m_pRenderer->ImmediateColorAlpha(m_backgroundColorBottomRight.GetRed(), m_backgroundColorBottomRight.GetGreen(), m_backgroundColorBottomRight.GetBlue(), m_backgroundColorBottomRight.GetAlpha());
	m_pRenderer->ImmediateVertex(containerX2, containerY1, depth);
	m_pRenderer->ImmediateColorAlpha(m_backgroundColorTopRight.GetRed(), m_backgroundColorTopRight.GetGreen(), m_backgroundColorTopRight.GetBlue(), m_backgroundColorTopRight.GetAlpha());
	m_pRenderer->ImmediateVertex(containerX2, containerY2, depth);
	m_pRenderer->ImmediateColorAlpha(m_backgroundColorTopLeft.GetRed(), m_backgroundColorTopLeft.GetGreen(), m_backgroundColorTopLeft.GetBlue(), m_backgroundColorTopLeft.GetAlpha());
	m_pRenderer->ImmediateVertex(containerX1, containerY2, depth);
	m_pRenderer->DisableImmediateMode();
	
	m_pRenderer->PopMatrix();

	// Draw the outline
	m_pRenderer->PushMatrix();

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);
	m_pRenderer->ImmediateColorAlpha(m_outlineColorBottom.GetRed(), m_outlineColorBottom.GetGreen(), m_outlineColorBottom.GetBlue(), m_outlineColorBottom.GetAlpha());
	m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);
	m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);

	m_pRenderer->ImmediateColorAlpha(m_outlineColorRight.GetRed(), m_outlineColorRight.GetGreen(), m_outlineColorRight.GetBlue(), m_outlineColorRight.GetAlpha());
	m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);
	m_pRenderer->ImmediateVertex(outlineX2, outlineY2, depth);

	m_pRenderer->ImmediateColorAlpha(m_outlineColorTop.GetRed(), m_outlineColorTop.GetGreen(), m_outlineColorTop.GetBlue(), m_outlineColorTop.GetAlpha());
	m_pRenderer->ImmediateVertex(outlineX2, outlineY2, depth);
	m_pRenderer->ImmediateVertex(outlineX1, outlineY2, depth);

	m_pRenderer->ImmediateColorAlpha(m_outlineColorLeft.GetRed(), m_outlineColorLeft.GetGreen(), m_outlineColorLeft.GetBlue(), m_outlineColorLeft.GetAlpha());
	m_pRenderer->ImmediateVertex(outlineX1, outlineY2, depth);
	m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);
	m_pRenderer->DisableImmediateMode();
	
	m_pRenderer->PopMatrix();
}