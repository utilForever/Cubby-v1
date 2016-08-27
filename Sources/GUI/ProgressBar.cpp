/*************************************************************************
> File Name: ProgressBar.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A progress bar component to show visual feedback of progress.
> Created Time: 2016/08/27
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "DirectDrawRectangle.h"
#include "ProgressBar.h"
#include "Icon.h"
#include "MultiTextureIcon.h"

// Constructor, Destructor
ProgressBar::ProgressBar(Renderer* pRenderer, unsigned int GUIFont, float min, float max) :
	Container(pRenderer), m_minValue(min), m_maxValue(max),
	m_progressDirection(ProgressDirection::Horizontal)
{
	m_currentValue = m_minValue;

	m_pProgressBackground = new DirectDrawRectangle(pRenderer);
	m_pProgressFiller = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pProgressBackground);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pProgressFiller);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	Container::Add(m_pProgressFiller);
	Container::Add(m_pProgressBackground);

	m_pProgressBackground->SetDepth(2.0f);
	m_pProgressFiller->SetDepth(3.0f);
}

ProgressBar::~ProgressBar()
{
	delete m_pProgressBackground;
	delete m_pProgressFiller;
}

float ProgressBar::GetMinValue() const
{
	return m_minValue;
}

float ProgressBar::GetMaxValue() const
{
	return m_maxValue;
}

float ProgressBar::GetCurrentValue() const
{
	return m_currentValue;
}

void ProgressBar::SetMinValue(float minValue)
{
	m_minValue = minValue;
}

void ProgressBar::SetMaxValue(float maxValue)
{
	m_maxValue = maxValue;
}

void ProgressBar::SetCurrentValue(float currentValue)
{
	m_currentValue = currentValue;

	// Bounds checking
	if (m_currentValue < m_minValue)
	{
		m_currentValue = m_minValue;

		OnMinValueReached();
	}
	else if (m_currentValue > m_maxValue)
	{
		m_currentValue = m_maxValue;

		OnMaxValueReached();
	}
}

ProgressDirection ProgressBar::GetProgressDirection() const
{
	return m_progressDirection;
}

void ProgressBar::SetProgressDirection(ProgressDirection direction)
{
	m_progressDirection = direction;
}

void ProgressBar::SetProgressFiller(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pProgressFiller)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pProgressFiller);

		delete m_pProgressFiller;
		m_pProgressFiller = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pProgressFiller = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pProgressFiller = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pProgressFiller);

	// Properly set the depth again, since this will have changed after we added the component again
	m_pProgressFiller->SetDepth(3.0f);
}

void ProgressBar::SetProgressBackground(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);

	if (m_pProgressBackground)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pProgressBackground);

		delete m_pProgressBackground;
		m_pProgressBackground = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pProgressBackground = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pProgressBackground = pNewIcon;
	}

	// Re-add this icon to the component list
	Add(m_pProgressBackground);

	// Properly set the depth again, since this will have changed after we added the component again
	m_pProgressBackground->SetDepth(2.0f);
}

ComponentType ProgressBar::GetComponentType() const
{
	return ComponentType::ProgressBar;
}

void ProgressBar::OnMinValueReached()
{
	// Do nothing
}

void ProgressBar::OnMaxValueReached()
{
	// TEMP!
	m_currentValue = m_minValue;
}

void ProgressBar::DrawSelf()
{
	int containerWidth = m_dimensions.width;
	int containerHeight = m_dimensions.height;
	//int depth = static_cast<int>(GetDepth());

	int backgroundX1;
	int backgroundX2;
	int backgroundY1;
	int backgroundY2;

	int progressionX1;
	int progressionX2;
	int progressionY1;
	int progressionY2;

	//int outlineX1;
	//int outlineX2;
	//int outlineY1;
	//int outlineY2;

	int progression;

	if (m_progressDirection == ProgressDirection::Horizontal)
	{
		progression = static_cast<int>(m_dimensions.width * ((m_currentValue - m_minValue) / (m_maxValue - m_minValue)));

		backgroundX1 = 0;
		backgroundX2 = containerWidth;
		backgroundY1 = 0;
		backgroundY2 = containerHeight;

		progressionX1 = 0;
		progressionX2 = progression;
		progressionY1 = 0;
		progressionY2 = containerHeight;

		//outlineX1 = 0;
		//outlineX2 = containerWidth + 1;
		//outlineY1 = 0;
		//outlineY2 = containerHeight + 1;

	}
	else
	{
		progression = static_cast<int>(m_dimensions.height * ((m_currentValue - m_minValue) / (m_maxValue - m_minValue)));

		backgroundX1 = 0;
		backgroundX2 = containerWidth;
		backgroundY1 = 0;
		backgroundY2 = containerHeight;

		progressionX1 = 0;
		progressionX2 = containerWidth;
		progressionY1 = 0;
		progressionY2 = progression;

		//outlineX1 = 0;
		//outlineX2 = containerWidth + 1;
		//outlineY1 = 0;
		//outlineY2 = containerHeight + 1;
	}

	//// Draw the progression
	//m_pRenderer->PushMatrix();

	//m_pRenderer->SetRenderMode(RenderMode::SOLID);
	//m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

	//m_pRenderer->ImmediateColorAlpha(1.0f, 0.0f, 0.0f, 1.0f);
	//m_pRenderer->ImmediateVertex(progressionX1, progressionY1, depth);
	//m_pRenderer->ImmediateVertex(progressionX2, progressionY1, depth);
	//m_pRenderer->ImmediateVertex(progressionX2, progressionY2, depth);
	//m_pRenderer->ImmediateVertex(progressionX1, progressionY2, depth);

	//m_pRenderer->DisableImmediateMode();

	//m_pRenderer->PopMatrix();

	//// Draw the background
	//m_pRenderer->PushMatrix();

	//m_pRenderer->SetRenderMode(RenderMode::SOLID);
	//m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

	//m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	//m_pRenderer->ImmediateVertex(backgroundX1, backgroundY1, depth);
	//m_pRenderer->ImmediateVertex(backgroundX2, backgroundY1, depth);
	//m_pRenderer->ImmediateVertex(backgroundX2, backgroundY2, depth);
	//m_pRenderer->ImmediateVertex(backgroundX1, backgroundY2, depth);

	//m_pRenderer->DisableImmediateMode();

	//m_pRenderer->PopMatrix();

	//// Draw the outline
	//m_pRenderer->PushMatrix();

	//m_pRenderer->SetLineWidth(1.0f);
	//m_pRenderer->SetRenderMode(RenderMode::SOLID);
	//m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

	//m_pRenderer->ImmediateColorAlpha(0.0f, 0.0f, 0.0f, 1.0f);
	//m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);
	//m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);
	//m_pRenderer->ImmediateVertex(outlineX2, outlineY2, depth);
	//m_pRenderer->ImmediateVertex(outlineX1, outlineY2, depth);

	//m_pRenderer->DisableImmediateMode();

	//m_pRenderer->PopMatrix();

	m_pProgressBackground->SetDimensions(backgroundX1, backgroundY1, backgroundX2, backgroundY2);
	m_pProgressFiller->SetDimensions(progressionX1, progressionY1, progressionX2, progressionY2);

	m_pProgressBackground->SetVisible(true);
	m_pProgressFiller->SetVisible(true);
}