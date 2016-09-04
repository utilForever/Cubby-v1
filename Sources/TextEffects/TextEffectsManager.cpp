/*************************************************************************
> File Name: TextEffectsManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: TextEffects manager class.
> Created Time: 2016/07/17
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Utils/Random.h>
#include <Utils/Interpolator.h>

#include "TextEffectsManager.h"

// Constructor, Destructor
TextEffectsManager::TextEffectsManager(Renderer* pRenderer) :
	m_pRenderer(pRenderer), m_pCamera(nullptr)
{

}

TextEffectsManager::~TextEffectsManager()
{
	// Erase all text effects
	m_animatedTextMutexLock.lock();
	for (auto iter = m_vpAnimatedTextList.begin(); iter != m_vpAnimatedTextList.end();)
	{
		if ((*iter)->m_autoDelete)
		{
			delete *iter;
		}

		iter = m_vpAnimatedTextList.erase(iter);
	}

	m_animatedTextMutexLock.unlock();
}

void TextEffectsManager::SetCamera(Camera* pCamera)
{
	m_pCamera = pCamera;
}

AnimatedText* TextEffectsManager::CreateTextEffect(unsigned int fontID, unsigned int outlineFontID, unsigned int viewportID, TextDrawMode drawMode, TextEffect effect, TextDrawStyle drawStyle, glm::vec3 position, Color color, Color outlineColor, const std::string& text, float lifeTime)
{
	AnimatedText* pEffect = new AnimatedText();

	// Actually create the text effect
	pEffect->m_position = position;
	pEffect->m_color = color;
	pEffect->m_outlineColor = outlineColor;

	pEffect->m_effect = effect;
	pEffect->m_drawMode = drawMode;
	pEffect->m_drawStyle = drawStyle;

	pEffect->m_fontID = fontID;
	pEffect->m_outlineFontID = outlineFontID;
	pEffect->m_viewportID = viewportID;

	pEffect->SetText(text);

	pEffect->m_isErased = false;

	pEffect->SetTextWidth(m_pRenderer->GetFreeTypeTextWidth(pEffect->m_fontID, "%s", pEffect->GetText().c_str()));

	float randomX = GetRandomNumber(-3, 3, 1) * 0.5f;
	float randomZ = GetRandomNumber(-3, 3, 1) * 0.5f;

	switch (effect)
	{
	case TextEffect::NoMovement:
		pEffect->m_lifeTimeMax = lifeTime;
		pEffect->m_lifeTime = lifeTime;
		pEffect->m_scale = 1.0f;

		pEffect->m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	
		break;

	case TextEffect::VerticalBounce:
		pEffect->m_lifeTimeMax = lifeTime;
		pEffect->m_lifeTime = lifeTime;
		pEffect->m_scale = 1.0f;

		pEffect->m_velocity = glm::vec3(0.0f, 3.0f, 0.0f);
	
		break;

	case TextEffect::RandomBounce:
		pEffect->m_lifeTimeMax = lifeTime;
		pEffect->m_lifeTime = lifeTime;
		pEffect->m_scale = 1.0f;

		pEffect->m_velocity = glm::vec3(randomX, 3.0f, randomZ);
	
		break;

	case TextEffect::FadeUp:
		pEffect->m_lifeTimeMax = lifeTime;
		pEffect->m_lifeTime = lifeTime;
		pEffect->m_scale = 0.2f;

		Interpolator::GetInstance()->AddFloatInterpolation(&pEffect->m_scale, 0.2f, 1.0f, 0.1f, -100.0f);

		pEffect->m_velocity = glm::vec3(0.0f, 1.0f, 0.0f);

		break;
	case TextEffect::NumEffects:
	default:
		throw std::logic_error("CreateTextEffect Error!\n");
	}

	// Push onto the vector list
	m_animatedTextMutexLock.lock();

	m_vpAnimatedTextList.push_back(pEffect);

	m_animatedTextMutexLock.unlock();

	// Return a pointer to this particle effect
	return pEffect;
}

void TextEffectsManager::Update(float deltaTime)
{
	// Update all effects
	m_animatedTextMutexLock.lock();

	for (auto iter = m_vpAnimatedTextList.begin(); iter != m_vpAnimatedTextList.end(); ++iter)
	{
		AnimatedText* pAnimatedText = *iter;

		if (!pAnimatedText->m_isStarted)
		{
			// Don't update effects that have not started yet
			continue;
		}

		if (!pAnimatedText->m_isPaused)
		{
			// If we are not paused then tick down the life timer
			pAnimatedText->m_lifeTime -= deltaTime;
		}

		if (pAnimatedText->m_lifeTime <= 0.0f)
		{
			// If we have elapsed our timer, then erase the effect and continue
			pAnimatedText->SetErased(true);

			continue;
		}

		float interpolateRatio = pAnimatedText->m_lifeTime / pAnimatedText->m_lifeTimeMax;

		switch (pAnimatedText->m_effect)
		{
		case TextEffect::NoMovement:
			pAnimatedText->m_color.SetAlpha(interpolateRatio);
			pAnimatedText->m_outlineColor.SetAlpha(interpolateRatio);
		
			break;

		case TextEffect::VerticalBounce:
		case TextEffect::RandomBounce:
			// Simulate gravity
			pAnimatedText->m_velocity += glm::vec3(0.0f, -9.81f, 0.0f) * deltaTime;
			pAnimatedText->m_position += pAnimatedText->m_velocity * deltaTime;

			pAnimatedText->m_color.SetAlpha(interpolateRatio);
			pAnimatedText->m_outlineColor.SetAlpha(interpolateRatio);
		
			break;

		case TextEffect::FadeUp:
			pAnimatedText->m_position += pAnimatedText->m_velocity * deltaTime;

			pAnimatedText->m_color.SetAlpha(interpolateRatio);
			pAnimatedText->m_outlineColor.SetAlpha(interpolateRatio);
	
			break;
		case TextEffect::NumEffects:
		default:
			throw std::logic_error("TextEffect Update Error!\n");
		}
	}

	// Go through and erase any effects that are waiting to be erased
	for (auto iter = m_vpAnimatedTextList.begin(); iter != m_vpAnimatedTextList.end();)
	{
		if ((*iter)->GetErased())
		{
			if ((*iter)->m_autoDelete)
			{
				delete *iter;
			}

			iter = m_vpAnimatedTextList.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	m_animatedTextMutexLock.unlock();
}

void TextEffectsManager::Render()
{
	m_pRenderer->PushMatrix();

	// Render all effects
	m_animatedTextMutexLock.lock();

	for (auto iter = m_vpAnimatedTextList.begin(); iter != m_vpAnimatedTextList.end(); ++iter)
	{
		AnimatedText* pAnimatedText = *iter;

		if (!pAnimatedText->m_isStarted)
		{
			// Don't render effects that have not started yet
			continue;
		}

		glm::vec3 pTextPosition;
		if (pAnimatedText->m_drawMode == TextDrawMode::Screen2D)
		{
			pTextPosition = pAnimatedText->m_position;
		}
		else if (pAnimatedText->m_drawMode == TextDrawMode::Screen3D)
		{
			if (m_pRenderer->PointInFrustum(pAnimatedText->m_viewportID, pAnimatedText->m_position) == false)
			{
				continue;
			}

			m_pRenderer->PushMatrix();

			m_pRenderer->SetProjectionMode(ProjectionMode::PERSPECTIVE, pAnimatedText->m_viewportID);
			m_pCamera->Look();

			int winX, winY;
			m_pRenderer->GetScreenCoordinatesFromWorldPosition(pAnimatedText->m_position, &winX, &winY);

			m_pRenderer->PopMatrix();

			pTextPosition = glm::vec3(static_cast<float>(winX) - pAnimatedText->GetTextWidth() / 2, static_cast<float>(winY), 0.0f);
		}
		else if (pAnimatedText->m_drawMode == TextDrawMode::World3D)
		{
			m_pRenderer->SetProjectionMode(ProjectionMode::PERSPECTIVE, pAnimatedText->m_viewportID);
			m_pCamera->Look();

			pTextPosition = pAnimatedText->m_position;
		}

		m_pRenderer->PushMatrix();
		
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->SetProjectionMode(ProjectionMode::TWO_DIMENSION, pAnimatedText->m_viewportID);
		m_pRenderer->SetLookAtCamera(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Draw styles
		m_pRenderer->RenderFreeTypeText(pAnimatedText->m_fontID, pTextPosition.x, pTextPosition.y, 0.0f, pAnimatedText->m_color, pAnimatedText->m_scale, "%s", pAnimatedText->GetText().c_str());

		if (pAnimatedText->m_drawStyle == TextDrawStyle::Outline)
		{
			m_pRenderer->RenderFreeTypeText(pAnimatedText->m_outlineFontID, pTextPosition.x, pTextPosition.y, 0.0f, pAnimatedText->m_outlineColor, pAnimatedText->m_scale, "%s", pAnimatedText->GetText().c_str());
		}

		m_pRenderer->PopMatrix();
	}

	m_animatedTextMutexLock.unlock();

	m_pRenderer->PopMatrix();
}