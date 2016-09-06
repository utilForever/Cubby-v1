/*************************************************************************
> File Name: MainMenu.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A front-end page for the main menu of the front-end, this is the main
>    entry point of the menu system used for front-end flow, after the intro.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <CubbyGame.h>

#include <Maths/3DMaths.h>

#include "../FrontendManager.h"

#include "MainMenu.h"

// Constructor, Destructor
MainMenu::MainMenu(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontPageManager, int windowWidth, int windowHeight) :
	FrontendPage(pRenderer, pGUI, pFrontPageManager, FrontendScreen::MainMenu, windowWidth, windowHeight)
{
	m_logoTexture = -1;
	int texturewidth2, textureheight2;
	m_pRenderer->LoadTexture("Resources/textures/vox_logo.tga", &m_logoWidth, &m_logoHeight, &texturewidth2, &textureheight2, &m_logoTexture);

	m_pCreditLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont35(), "Developed by utilForever", Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pCreditLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont35Outline());
	m_pCreditLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCreditLabel->SetOutline(true);

	m_pSinglePlayerButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont50(), m_pFrontendManager->GetFrontendFont50Outline(), "Single Player", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pSinglePlayerButton->SetLabelOffset(0, 5);
	m_pSinglePlayerButton->SetPressedOffset(0, -4);
	m_pSinglePlayerButton->SetCallBackFunction(_SinglePlayerPressed);
	m_pSinglePlayerButton->SetCallBackData(this);

	m_pMultiPlayerButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont50(), m_pFrontendManager->GetFrontendFont50Outline(), "Multiplayer", Color(0.75f, 0.75f, 0.75f, 1.0f), Color(0.25f, 0.25f, 0.25f, 1.0f));
	m_pMultiPlayerButton->SetLabelOffset(0, 5);
	m_pMultiPlayerButton->SetPressedOffset(0, -4);
	m_pMultiPlayerButton->SetCallBackFunction(_MultiPlayerPressed);
	m_pMultiPlayerButton->SetCallBackData(this);

	m_pOptionsButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont50(), m_pFrontendManager->GetFrontendFont50Outline(), "Options", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pOptionsButton->SetLabelOffset(0, 5);
	m_pOptionsButton->SetPressedOffset(0, -4);
	m_pOptionsButton->SetCallBackFunction(_OptionsPressed);
	m_pOptionsButton->SetCallBackData(this);

	m_pModsButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont50(), m_pFrontendManager->GetFrontendFont50Outline(), "Mods", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pModsButton->SetLabelOffset(0, 5);
	m_pModsButton->SetPressedOffset(0, -4);
	m_pModsButton->SetCallBackFunction(_ModsPressed);
	m_pModsButton->SetCallBackData(this);

	m_pCreditsButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont50(), m_pFrontendManager->GetFrontendFont50Outline(), "Credits", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCreditsButton->SetLabelOffset(0, 5);
	m_pCreditsButton->SetPressedOffset(0, -4);
	m_pCreditsButton->SetCallBackFunction(_CreditsPressed);
	m_pCreditsButton->SetCallBackData(this);

	m_pQuitButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont50(), m_pFrontendManager->GetFrontendFont50Outline(), "Quit", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pQuitButton->SetLabelOffset(0, 5);
	m_pQuitButton->SetPressedOffset(0, -4);
	m_pQuitButton->SetCallBackFunction(_QuitPressed);
	m_pQuitButton->SetCallBackData(this);

	m_blurAmount = 0.0f;

	MainMenu::Reset();

	MainMenu::SetWindowDimensions(m_windowWidth, m_windowHeight);
}

MainMenu::~MainMenu()
{
	delete m_pCreditLabel;
	delete m_pSinglePlayerButton;
	delete m_pMultiPlayerButton;
	delete m_pOptionsButton;
	delete m_pModsButton;
	delete m_pCreditsButton;
	delete m_pQuitButton;
}

void MainMenu::Reset()
{
	m_textFadeInTime = 3.0f;
	m_textFadeInTimer = m_textFadeInTime;
	m_titleFadeInTime = 1.0f;
	m_titleFadeInTimer = m_titleFadeInTime;
	m_textAlpha = 0.0f;
	m_titleAlpha = 0.0f;
}

void MainMenu::SetWindowDimensions(int width, int height)
{
	FrontendPage::SetWindowDimensions(width, height);

	m_pSinglePlayerButton->SetDimensions(30, 490, 225, 75);
	m_pMultiPlayerButton->SetDimensions(30, 405, 225, 75);
	m_pOptionsButton->SetDimensions(30, 320, 225, 75);
	m_pModsButton->SetDimensions(30, 235, 225, 75);
	m_pCreditsButton->SetDimensions(30, 150, 225, 75);
	m_pQuitButton->SetDimensions(30, 30, 225, 75);

	int borderSpacer = 8;
	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont35(), "%s", m_pCreditLabel->GetText().c_str());
	m_pCreditLabel->SetLocation(m_windowWidth - textWidth - borderSpacer, borderSpacer);
}

void MainMenu::SkinGUI()
{
	m_pFrontendManager->SetButtonIcons(m_pSinglePlayerButton, ButtonSize::Size225x75);
	m_pFrontendManager->SetButtonIcons(m_pMultiPlayerButton, ButtonSize::Size225x75);
	m_pFrontendManager->SetButtonIcons(m_pOptionsButton, ButtonSize::Size225x75);
	m_pFrontendManager->SetButtonIcons(m_pModsButton, ButtonSize::Size225x75);
	m_pFrontendManager->SetButtonIcons(m_pCreditsButton, ButtonSize::Size225x75);
	m_pFrontendManager->SetButtonIcons(m_pQuitButton, ButtonSize::Size225x75);

	m_pSinglePlayerButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pSinglePlayerButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pSinglePlayerButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pMultiPlayerButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pMultiPlayerButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pMultiPlayerButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pOptionsButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pOptionsButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pOptionsButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pModsButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pModsButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pModsButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pCreditsButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pCreditsButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pCreditsButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pQuitButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pQuitButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pQuitButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
}

void MainMenu::UnSkinGUI()
{
	m_pSinglePlayerButton->SetDefaultIcons(m_pRenderer);
	m_pMultiPlayerButton->SetDefaultIcons(m_pRenderer);
	m_pOptionsButton->SetDefaultIcons(m_pRenderer);
	m_pModsButton->SetDefaultIcons(m_pRenderer);
	m_pCreditsButton->SetDefaultIcons(m_pRenderer);
	m_pQuitButton->SetDefaultIcons(m_pRenderer);
}

void MainMenu::Load()
{
	m_pSinglePlayerButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pMultiPlayerButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pOptionsButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pModsButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pCreditsButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pQuitButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());

	//m_blurAmount = 0.0f;
	//Interpolator::GetInstance()->AddFloatInterpolation(&m_blurAmount, 0.0f, 0.00125f, 3.0f, 0.0f);
	//CubbyGame::GetInstance()->SetLightingShader(false);

	m_pGUI->AddComponent(m_pCreditLabel);
	m_pGUI->AddComponent(m_pSinglePlayerButton);
	//m_pGUI->AddComponent(m_pMultiPlayerButton);
	m_pGUI->AddComponent(m_pOptionsButton);
	m_pGUI->AddComponent(m_pModsButton);
	m_pGUI->AddComponent(m_pCreditsButton);
	m_pGUI->AddComponent(m_pQuitButton);

	Reset();
}

void MainMenu::Unload()
{
	m_pGUI->RemoveComponent(m_pCreditLabel);
	m_pGUI->RemoveComponent(m_pSinglePlayerButton);
	m_pGUI->RemoveComponent(m_pMultiPlayerButton);
	m_pGUI->RemoveComponent(m_pOptionsButton);
	m_pGUI->RemoveComponent(m_pModsButton);
	m_pGUI->RemoveComponent(m_pCreditsButton);
	m_pGUI->RemoveComponent(m_pQuitButton);

	CubbyGame::GetInstance()->SetGlobalBlurAmount(0.0f);
	//m_pFrontendManager->GetGameWindow()->SetLightingShader(true);
}

void MainMenu::Update(float dt)
{
	FrontendPage::Update(dt);

	CubbyGame::GetInstance()->SetGlobalBlurAmount(0.00125f);

	if (CubbyGame::GetInstance()->IsPaused() == false)
	{
		float orbitTimer = m_pFrontendManager->GetCameraOrbitTimer();
		orbitTimer += dt;

		float orbitTime = 120.0f;
		if (orbitTimer >= orbitTime)
		{
			orbitTimer = 0.0f;
		}
		m_pFrontendManager->SetCameraOrbitTimer(orbitTimer);

		float ratio = orbitTimer / orbitTime;
		float radius = 125.0f;
		float angle = DegreeToRadian((ratio * 360.0f));
		glm::vec3 position = glm::vec3(cos(angle) * radius, radius * 0.75f, sin(angle) * radius);

		m_cameraPosition = position;
		m_cameraView = glm::vec3(0.0f, 8.0f, 0.0f);

		// Text fade in
		if (m_textFadeInTimer >= 0.0f)
		{
			m_textFadeInTimer -= dt;

			m_textAlpha = 1.0f - (m_textFadeInTimer / m_textFadeInTime);

			m_pCreditLabel->SetColor(Color(1.0f, 1.0f, 1.0f, m_textAlpha));
			m_pCreditLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, m_textAlpha));
		}
		// Title fade in 
		if (m_titleFadeInTimer >= 0.0f)
		{
			m_titleFadeInTimer -= dt;

			m_titleAlpha = 1.0f - (m_titleFadeInTimer / m_titleFadeInTime);
		}
	}
}

void MainMenu::Render()
{
	FrontendPage::Render();
}

void MainMenu::Render2D()
{
	FrontendPage::Render2D();

	float logoScale = 1.25f;
	int topLogoSpacer = 25;

	float length = m_logoWidth * logoScale;
	float height = m_logoHeight * logoScale;

	m_pRenderer->PushMatrix();
	
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, m_titleAlpha);
	m_pRenderer->SetRenderMode(RenderMode::TEXTURED);
	m_pRenderer->BindTexture(m_logoTexture);
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);

	// Bring to front of full screen shader textures
	m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, 5.0f);

	// Center
	m_pRenderer->TranslateWorldMatrix(static_cast<float>(m_windowWidth * 0.5f - length * 0.5f), static_cast<float>(m_windowHeight - topLogoSpacer - height), 0.0f);

	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
	
	m_pRenderer->ImmediateTextureCoordinate(0.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, 1.0f);
	m_pRenderer->ImmediateTextureCoordinate(1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(static_cast<float>(length), 0.0f, 1.0f);
	m_pRenderer->ImmediateTextureCoordinate(1.0f, 0.0f);
	m_pRenderer->ImmediateVertex(static_cast<float>(length), static_cast<float>(height), 1.0f);
	m_pRenderer->ImmediateTextureCoordinate(0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(0.0f, static_cast<float>(height), 1.0f);
	
	m_pRenderer->DisableImmediateMode();

	m_pRenderer->DisableTransparency();
	m_pRenderer->PopMatrix();
}

void MainMenu::_SinglePlayerPressed(void* pData)
{
	MainMenu* pMainMenu = static_cast<MainMenu*>(pData);
	pMainMenu->SinglePlayerPressed();
}

void MainMenu::SinglePlayerPressed() const
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::SelectCharacter);
}

void MainMenu::_MultiPlayerPressed(void* pData)
{
	MainMenu* pMainMenu = static_cast<MainMenu*>(pData);
	pMainMenu->MultiPlayerPressed();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void MainMenu::MultiPlayerPressed() const
{
	// Do nothing
}

void MainMenu::_OptionsPressed(void* pData)
{
	MainMenu* pMainMenu = static_cast<MainMenu*>(pData);
	pMainMenu->OptionsPressed();
}

void MainMenu::OptionsPressed() const
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::OptionsMenu);
	m_pFrontendManager->SetOptionsReturnToMainMenu(true);
}

void MainMenu::_ModsPressed(void* pData)
{
	MainMenu* pMainMenu = static_cast<MainMenu*>(pData);
	pMainMenu->ModsPressed();
}

void MainMenu::ModsPressed() const
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::ModMenu);
	m_pFrontendManager->SetModsMenuReturnToMainMenu(true);
}

void MainMenu::_CreditsPressed(void* pData)
{
	MainMenu* pMainMenu = static_cast<MainMenu*>(pData);
	pMainMenu->CreditsPressed();
}

void MainMenu::CreditsPressed() const
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::Credits);
}

void MainMenu::_QuitPressed(void* pData)
{
	MainMenu* pMainMenu = static_cast<MainMenu*>(pData);
	pMainMenu->QuitPressed();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void MainMenu::QuitPressed() const
{
	CubbyGame::GetInstance()->SetGameQuit(true);
}
