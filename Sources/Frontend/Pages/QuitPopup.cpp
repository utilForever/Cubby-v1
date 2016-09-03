/*************************************************************************
> File Name: QuitPopup.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A front-end page to show a confirm/cancel popup when the player goes to
>    close the Cubby application while in game. This happens for all types of
>    close requests that the application receives.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <CubbyGame.h>

#include "../FrontendManager.h"

#include "QuitPopup.h"

// Constructor, Destructor
QuitPopup::QuitPopup(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, int windowWidth, int windowHeight) :
	FrontendPage(pRenderer, pGUI, pFrontendManager, FrontendScreen::QuitPopup, windowWidth, windowHeight)
{
	char quitText[] = "Are you sure you want to quit?";
	m_pQuitText = new Label(m_pRenderer, pFrontendManager->GetFrontendFont40(), quitText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pQuitText->SetOutline(true);
	m_pQuitText->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pQuitText->SetOutlineFont(pFrontendManager->GetFrontendFont40Outline());
	m_pQuitText->SetDepth(3.0f);

	m_pBackgroundIcon = new Icon(m_pRenderer, "", 400, 200);
	m_pBackgroundIcon->SetDepth(2.0f);

	m_pConfirmButton = new Button(m_pRenderer, pFrontendManager->GetFrontendFont30(), pFrontendManager->GetFrontendFont30Outline(), "Yes", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pConfirmButton->SetLabelOffset(0, 3);
	m_pConfirmButton->SetPressedOffset(0, -3);
	m_pConfirmButton->SetDepth(3.0f);
	m_pConfirmButton->SetCallBackFunction(_ConfirmPressed);
	m_pConfirmButton->SetCallBackData(this);

	m_pCancelButton = new Button(m_pRenderer, pFrontendManager->GetFrontendFont30(), pFrontendManager->GetFrontendFont30Outline(), "No", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCancelButton->SetLabelOffset(0, 3);
	m_pCancelButton->SetPressedOffset(0, -3);
	m_pCancelButton->SetDepth(3.0f);
	m_pCancelButton->SetCallBackFunction(_CancelPressed);
	m_pCancelButton->SetCallBackData(this);

	QuitPopup::SetWindowDimensions(m_windowWidth, m_windowHeight);
}

QuitPopup::~QuitPopup()
{
	delete m_pQuitText;
	delete m_pBackgroundIcon;
	delete m_pCancelButton;
	delete m_pConfirmButton;
}

void QuitPopup::Reset()
{
	// Do nothing
}

void QuitPopup::SetWindowDimensions(int windowWidth, int windowHeight)
{
	FrontendPage::SetWindowDimensions(windowWidth, windowHeight);

	m_backgroundWidth = 400;
	m_backgroundHeight = 175;

	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont40(), "%s", m_pQuitText->GetText().c_str());
	m_pQuitText->SetLocation(static_cast<int>((m_windowWidth * 0.5f) - (textWidth * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) + 25));

	m_pBackgroundIcon->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (m_backgroundWidth * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) - (m_backgroundHeight * 0.5f) + 5), m_backgroundWidth, m_backgroundHeight);

	m_pCancelButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - 65 - (110 * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) - 50), 110, 47);
	m_pConfirmButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) + 65 - (110 * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) - 50), 110, 47);
}

void QuitPopup::SkinGUI()
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/QuitPopup/background.tga";
	m_pBackgroundIcon->SetIcon(iconName);

	m_pFrontendManager->SetButtonIcons(m_pConfirmButton, ButtonSize::Size110x47);
	m_pFrontendManager->SetButtonIcons(m_pCancelButton, ButtonSize::Size110x47);

	m_pConfirmButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pConfirmButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pConfirmButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pCancelButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pCancelButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pCancelButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
}

void QuitPopup::UnSkinGUI()
{
	m_pConfirmButton->SetDefaultIcons(m_pRenderer);
	m_pCancelButton->SetDefaultIcons(m_pRenderer);
}

void QuitPopup::Load()
{
	m_pConfirmButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pCancelButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());

	m_pGUI->AddComponent(m_pQuitText);
	m_pGUI->AddComponent(m_pBackgroundIcon);
	m_pGUI->AddComponent(m_pCancelButton);
	m_pGUI->AddComponent(m_pConfirmButton);
}

void QuitPopup::Unload()
{
	m_pGUI->RemoveComponent(m_pQuitText);
	m_pGUI->RemoveComponent(m_pBackgroundIcon);
	m_pGUI->RemoveComponent(m_pCancelButton);
	m_pGUI->RemoveComponent(m_pConfirmButton);
}

void QuitPopup::Update(float dt)
{
	FrontendPage::Update(dt);

	m_cameraPosition = glm::vec3(0.0f, 9.0f, 7.0f);
	m_cameraView = glm::vec3(0.0f, 9.0f, 0.0f);
}

void QuitPopup::Render()
{
	FrontendPage::Render();
}

void QuitPopup::Render2D()
{
	FrontendPage::Render2D();
}

void QuitPopup::_ConfirmPressed(void* pData)
{
	QuitPopup* pQuitPopup = static_cast<QuitPopup*>(pData);
	pQuitPopup->ConfirmPressed();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void QuitPopup::ConfirmPressed() const
{
	CubbyGame::GetInstance()->SetGameQuit(true);
}

void QuitPopup::_CancelPressed(void* pData)
{
	QuitPopup* pQuitPopup = static_cast<QuitPopup*>(pData);
	pQuitPopup->CancelPressed();
}

void QuitPopup::CancelPressed()
{
	CubbyGame::GetInstance()->CancelQuitPopup();

	Unload();
}