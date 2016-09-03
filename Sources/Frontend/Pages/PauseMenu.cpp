/*************************************************************************
> File Name: PauseMenu.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A front-end page to show a pause menu from within the game. Allows the
>    player access to the options/mod menu and also functionality to quit back
>    to the front-end (main menu).
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <CubbyGame.h>

#include "../FrontendManager.h"

#include "PauseMenu.h"

// Constructor, Destructor
PauseMenu::PauseMenu(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, int windowWidth, int windowHeight) :
	FrontendPage(pRenderer, pGUI, pFrontendManager, FrontendScreen::PauseMenu, windowWidth, windowHeight)
{
	m_pBackgroundIcon = new Icon(m_pRenderer, "", 230, 265);
	m_pBackgroundIcon->SetDepth(1.5f);

	m_pResumeButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "Resume", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pResumeButton->SetCallBackFunction(_ResumePressed);
	m_pResumeButton->SetCallBackData(this);
	m_pResumeButton->SetDepth(2.0f);
	m_pResumeButton->SetLabelOffset(0, 3);
	m_pResumeButton->SetPressedOffset(0, -3);

	m_pOptionsButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "Options", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pOptionsButton->SetCallBackFunction(_OptionsPressed);
	m_pOptionsButton->SetCallBackData(this);
	m_pOptionsButton->SetDepth(2.0f);
	m_pOptionsButton->SetLabelOffset(0, 3);
	m_pOptionsButton->SetPressedOffset(0, -3);

	m_pModsButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "Mods", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pModsButton->SetCallBackFunction(_ModsPressed);
	m_pModsButton->SetCallBackData(this);
	m_pModsButton->SetDepth(2.0f);
	m_pModsButton->SetLabelOffset(0, 3);
	m_pModsButton->SetPressedOffset(0, -3);

	m_pQuitButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "Quit", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pQuitButton->SetCallBackFunction(_QuitPressed);
	m_pQuitButton->SetCallBackData(this);
	m_pQuitButton->SetDepth(2.0f);
	m_pQuitButton->SetLabelOffset(0, 3);
	m_pQuitButton->SetPressedOffset(0, -3);

	PauseMenu::SetWindowDimensions(m_windowWidth, m_windowHeight);
}

PauseMenu::~PauseMenu()
{
	delete m_pBackgroundIcon;

	delete m_pResumeButton;
	delete m_pOptionsButton;
	delete m_pModsButton;
	delete m_pQuitButton;
}

void PauseMenu::Reset()
{
	// Do nothing
}

void PauseMenu::SetWindowDimensions(int windowWidth, int windowHeight)
{
	FrontendPage::SetWindowDimensions(windowWidth, windowHeight);

	m_pBackgroundIcon->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (230 * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) - (335 * 0.5f)), 230, 305);

	m_pResumeButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (110 * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) + (265 * 0.5f) - 65), 110, 47);
	m_pOptionsButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (110 * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) + (265 * 0.5f) - 125), 110, 47);
	m_pModsButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (110 * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) + (265 * 0.5f) - 185), 110, 47);
	m_pQuitButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (110 * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) + (265 * 0.5f) - 275), 110, 47);
}

void PauseMenu::SkinGUI()
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/PauseMenu/pause_background.tga";
	m_pBackgroundIcon->SetIcon(iconName);

	m_pFrontendManager->SetButtonIcons(m_pResumeButton, ButtonSize::Size110x47);
	m_pFrontendManager->SetButtonIcons(m_pModsButton, ButtonSize::Size110x47);
	m_pFrontendManager->SetButtonIcons(m_pOptionsButton, ButtonSize::Size110x47);
	m_pFrontendManager->SetButtonIcons(m_pQuitButton, ButtonSize::Size110x47);

	m_pResumeButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pResumeButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pResumeButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pModsButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pModsButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pModsButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pOptionsButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pOptionsButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pOptionsButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pQuitButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pQuitButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pQuitButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
}

void PauseMenu::UnSkinGUI()
{
	m_pResumeButton->SetDefaultIcons(m_pRenderer);
	m_pModsButton->SetDefaultIcons(m_pRenderer);
	m_pOptionsButton->SetDefaultIcons(m_pRenderer);
	m_pQuitButton->SetDefaultIcons(m_pRenderer);
}

void PauseMenu::Load()
{
	m_pResumeButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pModsButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pOptionsButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pQuitButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());

	m_pGUI->AddComponent(m_pResumeButton);
	m_pGUI->AddComponent(m_pOptionsButton);
	m_pGUI->AddComponent(m_pModsButton);
	m_pGUI->AddComponent(m_pQuitButton);

	m_pGUI->AddComponent(m_pBackgroundIcon);
}

void PauseMenu::Unload()
{
	m_pGUI->RemoveComponent(m_pBackgroundIcon);

	m_pGUI->RemoveComponent(m_pResumeButton);
	m_pGUI->RemoveComponent(m_pOptionsButton);
	m_pGUI->RemoveComponent(m_pModsButton);
	m_pGUI->RemoveComponent(m_pQuitButton);
}

void PauseMenu::Update(float dt)
{
	FrontendPage::Update(dt);
}

void PauseMenu::Render()
{
	FrontendPage::Render();
}

void PauseMenu::Render2D()
{
	FrontendPage::Render2D();
}

void PauseMenu::_ResumePressed(void* pData)
{
	PauseMenu* pMainMenu = static_cast<PauseMenu*>(pData);
	pMainMenu->ResumePressed();
}

void PauseMenu::ResumePressed()
{
	Unload();

	CubbyGame::GetInstance()->UnsetPauseMenu();
}

void PauseMenu::_OptionsPressed(void* pData)
{
	PauseMenu* pMainMenu = static_cast<PauseMenu*>(pData);
	pMainMenu->OptionsPressed();
}

void PauseMenu::OptionsPressed() const
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::OptionsMenu);
	m_pFrontendManager->SetOptionsReturnToMainMenu(false);
}

void PauseMenu::_ModsPressed(void* pData)
{
	PauseMenu* pMainMenu = static_cast<PauseMenu*>(pData);
	pMainMenu->ModsPressed();
}

void PauseMenu::ModsPressed() const
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::ModMenu);
	m_pFrontendManager->SetModsMenuReturnToMainMenu(false);
}

void PauseMenu::_QuitPressed(void* pData)
{
	PauseMenu* pMainMenu = static_cast<PauseMenu*>(pData);
	pMainMenu->QuitPressed();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void PauseMenu::QuitPressed() const
{
	CubbyGame::GetInstance()->UnsetPauseMenu();
	CubbyGame::GetInstance()->QuitToFrontEnd();
}