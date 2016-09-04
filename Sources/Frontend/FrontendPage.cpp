/*************************************************************************
> File Name: FrontendPage.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The abstract base class for front-end screens. Contains all the common
>    functionality and data that all derived front-end pages must override.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <GUI/OpenGLGUI.h>

#include "FrontendManager.h"
#include "FrontendPage.h"

// Constructor, Destructor
FrontendPage::FrontendPage(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, FrontendScreen pageType, int windowWidth, int windowHeight)
{
	m_pRenderer = pRenderer;
	m_pGUI = pGUI;
	m_pFrontendManager = pFrontendManager;

	m_pageType = pageType;

	char titleText[] = "[TITLE]";
	m_pPageTitle = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont40(), titleText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pPageTitle->SetOutline(true);
	m_pPageTitle->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pPageTitle->SetOutlineFont(m_pFrontendManager->GetFrontendFont40Outline());
	m_pPageTitle->SetDepth(4.0f);

	char subtitleText[] = "[Subtitle]";
	m_pPageSubtitle = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), subtitleText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pPageSubtitle->SetOutline(true);
	m_pPageSubtitle->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pPageSubtitle->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pPageSubtitle->SetDepth(4.0f);

	m_addedTitles = false;
	m_updateTitleTextCenterLocation = true;

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
}

FrontendPage::~FrontendPage()
{
	delete m_pPageTitle;
	delete m_pPageSubtitle;
}

void FrontendPage::SetWindowDimensions(int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;

	m_updateTitleTextCenterLocation = true;
}

FrontendScreen FrontendPage::GetPageType() const
{
	return m_pageType;
}

// Page title and sub-title
void FrontendPage::SetTitleAndSubtitle(std::string title, std::string subtitle)
{
	m_pPageTitle->SetText(title);
	m_pPageSubtitle->SetText(subtitle);

	m_updateTitleTextCenterLocation = true;

	if (m_addedTitles == false)
	{
		m_pGUI->AddComponent(m_pPageTitle);
		m_pGUI->AddComponent(m_pPageSubtitle);

		m_addedTitles = true;
	}
}

void FrontendPage::RemoveTitlesAndSubTitle()
{
	if (m_addedTitles == true)
	{
		m_pGUI->RemoveComponent(m_pPageTitle);
		m_pGUI->RemoveComponent(m_pPageSubtitle);

		m_addedTitles = false;
	}
}

// Camera
glm::vec3 FrontendPage::GetCameraPosition() const
{
	return m_cameraPosition;
}

glm::vec3 FrontendPage::GetCameraView() const
{
	return m_cameraView;
}

// Update
void FrontendPage::Update(float dt)
{
	// Do nothing
}

// Render
void FrontendPage::Render()
{
	if (m_updateTitleTextCenterLocation)
	{
		int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont40(), "%s", m_pPageTitle->GetText().c_str());
		m_pPageTitle->SetLocation(static_cast<int>(m_windowWidth * 0.5f - textWidth * 0.5f), m_windowHeight - 50);

		textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont25(), "%s", m_pPageSubtitle->GetText().c_str());
		m_pPageSubtitle->SetLocation(static_cast<int>(m_windowWidth * 0.5f - textWidth * 0.5f), m_windowHeight - 75);

		m_updateTitleTextCenterLocation = false;
	}
}

void FrontendPage::Render2D()
{
	// Do nothing
}

void FrontendPage::RenderDebug()
{
	// Do nothing
}