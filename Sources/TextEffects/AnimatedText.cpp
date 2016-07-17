/*************************************************************************
> File Name: AnimatedText.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Animated text class.
> Created Time: 2016/07/17
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "AnimatedText.h"

// Constructor, Destructor
AnimatedText::AnimatedText() :
	m_fontID(-1), m_outlineFontID(-1), m_viewportID(-1),
	m_effect(TextEffect::NoMovement),
	m_drawMode(TextDrawMode::Screen2D), m_drawStyle(TextDrawStyle::Normal),
	m_lifeTimeMax(0.0f), m_lifeTime(0.0f), m_scale(1.0f),
	m_isStarted(false), m_isPaused(false), m_isErased(false),
	m_autoDelete(false), m_textWidth(0)
{

}

AnimatedText::~AnimatedText()
{

}

void AnimatedText::SetText(const std::string& text)
{
	m_text = text;

	// TODO: URGENT - Also need to work out the text width again!
}

const std::string& AnimatedText::GetText() const
{
	return m_text;
}

void AnimatedText::SetTextWidth(int width)
{
	m_textWidth = width;
}

int AnimatedText::GetTextWidth() const
{
	return m_textWidth;
}

// Playing effects
void AnimatedText::StartEffect()
{
	m_isStarted = true;
}

void AnimatedText::StopEffect()
{
	m_isStarted = false;
}

void AnimatedText::PauseEffect()
{
	m_isPaused = true;
}

void AnimatedText::UnPauseEffect()
{
	m_isPaused = false;
}

// Deletion
bool AnimatedText::GetErased() const
{
	return m_isErased;
}

void AnimatedText::SetErased(bool isErase)
{
	m_isErased = isErase;
}

void AnimatedText::SetAutoDelete(bool autoDelete)
{
	m_autoDelete = autoDelete;
}