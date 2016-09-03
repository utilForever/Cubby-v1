/*************************************************************************
> File Name: PauseMenu.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A front-end page to show a pause menu from within the game. Allows the
>    player access to the options/mod menu and also functionality to quit back
>    to the front-end (main menu).
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_PAUSE_MENU_H
#define CUBBY_PAUSE_MENU_H

#include <GUI/Button.h>
#include <GUI/Icon.h>

#include "../FrontendPage.h"

class PauseMenu : public FrontendPage
{
public:
	// Constructor, Destructor
	PauseMenu(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, int windowWidth, int windowHeight);
	~PauseMenu();

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
	static void _ResumePressed(void* pData);
	void ResumePressed();

	static void _OptionsPressed(void* pData);
	void OptionsPressed() const;

	static void _ModsPressed(void* pData);
	void ModsPressed() const;

	static void _QuitPressed(void* pData);
	void QuitPressed() const;

private:
	Button* m_pResumeButton;
	Button* m_pOptionsButton;
	Button* m_pModsButton;
	Button* m_pQuitButton;

	Icon* m_pBackgroundIcon;
};

#endif