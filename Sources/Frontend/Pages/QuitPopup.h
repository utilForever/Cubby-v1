/*************************************************************************
> File Name: QuitPopup.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A front-end page to show a confirm/cancel popup when the player goes to
>    close the Cubby application while in game. This happens for all types of
>    close requests that the application receives.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_QUIT_POPUP_H
#define CUBBY_QUIT_POPUP_H

#include "../FrontendPage.h"

#include <GUI/Button.h>
#include <GUI/Icon.h>

class QuitPopup : public FrontendPage
{
public:
	// Constructor, Destructor
	QuitPopup(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, int windowWidth, int windowHeight);
	~QuitPopup();

	void Reset();

	void SetWindowDimensions(int width, int height);

	void SkinGUI();
	void UnSkinGUI();

	void Load();
	void Unload();

	void Update(float dt);

	void Render();
	void Render2D();

protected:
	static void _ConfirmPressed(void* pData);
	void ConfirmPressed() const;

	static void _CancelPressed(void* pData);
	void CancelPressed();

private:
	int m_backgroundWidth;
	int m_backgroundHeight;

	Label* m_pQuitText;

	Icon* m_pBackgroundIcon;

	Button* m_pCancelButton;
	Button* m_pConfirmButton;
};

#endif