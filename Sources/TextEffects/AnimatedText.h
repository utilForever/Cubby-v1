/*************************************************************************
> File Name: AnimatedText.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Animated text class.
> Created Time: 2016/07/17
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ANIMATED_TEXT_H
#define CUBBY_ANIMATED_TEXT_H

#include <string>

#include <Renderer/Color.h>

#include <glm/vec3.hpp>

enum class TextEffect
{
	NoMovement = 0,
	VerticalBounce,
	RandomBounce,
	FadeUp,

	NumEffects,
};

enum class TextDrawMode
{
	Screen2D = 0,
	Screen3D,
	World3D,

	NumDrawModes,
};

enum class TextDrawStyle
{
	Normal = 0,
	Outline,

	NumDrawStyles,
};

class AnimatedText
{
public:
	// Constructor, Destructor
	AnimatedText();
	~AnimatedText();

	void SetText(const std::string& text);
	const std::string& GetText() const;

	void SetTextWidth(int width);
	int GetTextWidth() const;

	// Playing effects
	void StartEffect();
	void StopEffect();
	void PauseEffect();
	void UnPauseEffect();

	// Deletion
	bool GetErased() const;
	void SetErased(bool isErased);

	void SetAutoDelete(bool autoDelete);

	glm::vec3 m_position;
	glm::vec3 m_velocity;

	Color m_color;
	Color m_outlineColor;

	unsigned int m_fontID;
	unsigned int m_outlineFontID;
	unsigned int m_viewportID;

	TextEffect m_effect;

	TextDrawMode m_drawMode;
	TextDrawStyle m_drawStyle;

	float m_lifeTimeMax;
	float m_lifeTime;

	float m_scale;

	bool m_isStarted;
	bool m_isPaused;

	bool m_isErased;

	bool m_autoDelete;

private:
	std::string m_text;

	int m_textWidth;
};

#endif