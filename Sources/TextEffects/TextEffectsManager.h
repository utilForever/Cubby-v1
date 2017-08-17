/*************************************************************************
> File Name: TextEffectsManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: TextEffects manager class.
> Created Time: 2016/07/17
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_TEXT_EFFECTS_MANAGER_H
#define CUBBY_TEXT_EFFECTS_MANAGER_H

#include <vector>

#include <Renderer/Camera.h>
#include <Renderer/Renderer.h>

#include "AnimatedText.h"

using AnimatedTextList = std::vector<AnimatedText*>;

class TextEffectsManager
{
public:
	// Constructor, Destructor
	TextEffectsManager(Renderer* pRenderer);
	~TextEffectsManager();

	void SetCamera(Camera* pCamera);

	AnimatedText* CreateTextEffect(unsigned int fontID, unsigned int outlineFontID, unsigned int viewportID, TextDrawMode drawMode, TextEffect effect, TextDrawStyle drawStyle, glm::vec3 position, Color color, Color outlineColor, const std::string& text, float lifeTime);

	void Update(float deltaTime);
	void Render();

private:
	Renderer* m_pRenderer;
	Camera* m_pCamera;

	tthread::mutex m_animatedTextMutexLock;
	AnimatedTextList m_vpAnimatedTextList;
};


#endif