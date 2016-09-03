/*************************************************************************
> File Name: MainMenu.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A front-end page for the main menu of the front-end, this is the main
>    entry point of the menu system used for front-end flow, after the intro.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MAIN_MENU_H
#define CUBBY_MAIN_MENU_H

#include <GUI/Button.h>

#include "../FrontendPage.h"

class MainMenu : public FrontendPage
{
public:
	// Constructor, Destructor
	MainMenu(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontPageManager, int windowWidth, int windowHeight);
	~MainMenu();

	void Reset() override;

	void SetWindowDimensions(int width, int height) override;

	void SkinGUI() override;
	void UnSkinGUI() override;

	void Load() override;
	void Unload() override;

	void Update(float dt) override;

	void Render() override;
	void Render2D() override;

protected:
	static void _SinglePlayerPressed(void* pData);
	void SinglePlayerPressed() const;

	static void _MultiPlayerPressed(void* pData);
	void MultiPlayerPressed() const;

	static void _OptionsPressed(void* pData);
	void OptionsPressed() const;

	static void _ModsPressed(void* pData);
	void ModsPressed() const;

	static void _CreditsPressed(void* pData);
	void CreditsPressed() const;

	static void _QuitPressed(void* pData);
	void QuitPressed() const;

private:
	// Cubby logo texture
	int m_logoWidth;
	int m_logoHeight;

	unsigned int m_logoTexture;

	float m_blurAmount;

	Label* m_pCreditLabel;

	Button* m_pSinglePlayerButton;
	Button* m_pMultiPlayerButton;
	Button* m_pOptionsButton;
	Button* m_pModsButton;
	Button* m_pCreditsButton;
	Button* m_pQuitButton;

	float m_textFadeInTime;
	float m_textFadeInTimer;
	float m_titleFadeInTime;
	float m_titleFadeInTimer;

	float m_textAlpha;
	float m_titleAlpha;
};

#endif