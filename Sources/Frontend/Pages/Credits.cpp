/*************************************************************************
> File Name: Credits.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A front-end page that displays the credits for the game.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <CubbyGame.h>

#include "Credits.h"

// Constructor, Destructor
Credits::Credits(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, int windowWidth, int windowHeight) :
	FrontendPage(pRenderer, pGUI, pFrontendManager, FrontendScreen::Credits, windowWidth, windowHeight)
{
	Credits::SetWindowDimensions(m_windowWidth, m_windowHeight);

	m_pCreditsNPC = nullptr;

	m_clickToNextTimer = 0.0f;
}

Credits::~Credits()
{
	ClearCreditTextLines();
	ClearCreditsNPC();
}

void Credits::Reset()
{
	// Do nothing
}

void Credits::CreateCreditTextLinesForPage(int pageIndex)
{
	if (pageIndex == 0)
	{
		{
			CreditTextLine* pNewCreditsLine = new CreditTextLine();
			pNewCreditsLine->m_text = "Chris Ohk";
			pNewCreditsLine->m_screenWidthRatio = 0.75f;
			pNewCreditsLine->m_screenHeightDifference = 315.0f;
			pNewCreditsLine->m_fontId = m_pFrontendManager->GetFrontendFont80();
			pNewCreditsLine->m_outlineFontId = m_pFrontendManager->GetFrontendFont80Outline();

			m_vpCreditLineList.push_back(pNewCreditsLine);
		}
		{
			CreditTextLine* pNewCreditsLine = new CreditTextLine();
			pNewCreditsLine->m_text = "@utilForever";
			pNewCreditsLine->m_screenWidthRatio = 0.75f;
			pNewCreditsLine->m_screenHeightDifference = 365.0f;
			pNewCreditsLine->m_fontId = m_pFrontendManager->GetFrontendFont50();
			pNewCreditsLine->m_outlineFontId = m_pFrontendManager->GetFrontendFont50Outline();

			m_vpCreditLineList.push_back(pNewCreditsLine);
		}
		{
			CreditTextLine* pNewCreditsLine = new CreditTextLine();
			pNewCreditsLine->m_text = "---------------------------";
			pNewCreditsLine->m_screenWidthRatio = 0.75f;
			pNewCreditsLine->m_screenHeightDifference = 420.0f;
			pNewCreditsLine->m_fontId = m_pFrontendManager->GetFrontendFont50();
			pNewCreditsLine->m_outlineFontId = m_pFrontendManager->GetFrontendFont50Outline();

			m_vpCreditLineList.push_back(pNewCreditsLine);
		}
		{
			CreditTextLine* pNewCreditsLine = new CreditTextLine();
			pNewCreditsLine->m_text = "Team Leader and Development";
			pNewCreditsLine->m_screenWidthRatio = 0.75f;
			pNewCreditsLine->m_screenHeightDifference = 475.0f;
			pNewCreditsLine->m_fontId = m_pFrontendManager->GetFrontendFont40();
			pNewCreditsLine->m_outlineFontId = m_pFrontendManager->GetFrontendFont40Outline();

			m_vpCreditLineList.push_back(pNewCreditsLine);
		}
		{
			CreditTextLine* pNewCreditsLine = new CreditTextLine();
			pNewCreditsLine->m_text = "Programming";
			pNewCreditsLine->m_screenWidthRatio = 0.75f;
			pNewCreditsLine->m_screenHeightDifference = 520.0f;
			pNewCreditsLine->m_fontId = m_pFrontendManager->GetFrontendFont40();
			pNewCreditsLine->m_outlineFontId = m_pFrontendManager->GetFrontendFont40Outline();

			m_vpCreditLineList.push_back(pNewCreditsLine);
		}
	}
}

void Credits::ClearCreditTextLines()
{
	for (size_t i = 0; i < static_cast<int>(m_vpCreditLineList.size()); ++i)
	{
		delete m_vpCreditLineList[i];
		m_vpCreditLineList[i] = nullptr;
	}
	m_vpCreditLineList.clear();
}

void Credits::CreateCreditsNPCForPage(int pageIndex)
{
	if (pageIndex == 0)
	{
		m_pCreditsNPC = CubbyGame::GetInstance()->GetNPCManager()->CreateNPC("CreditsCharacter", "Human", "Steve", glm::vec3(24.0f, 0.51f, 4.5f), 0.08f, false, false);
		m_pCreditsNPC->SetCreditsNPC(true);
		//m_pCreditsNPC->LookAtPoint(vec3(0.0f, 8.0f, 1.0f));
	}
	if (pageIndex == 1)
	{
		m_pCreditsNPC = CubbyGame::GetInstance()->GetNPCManager()->CreateNPC("CreditsCharacter", "Human", "Mummy", glm::vec3(26.0f, 0.51f, 4.5f), 0.08f, false, false);
		m_pCreditsNPC->SetCreditsNPC(true);
		//m_pCreditsNPC->LookAtPoint(vec3(0.0f, 8.0f, 1.0f));
		m_pCreditsNPC->SetAnimationSpeed(0.5f, true, AnimationSections::FullBody);
		m_pCreditsNPC->GetVoxelCharacter()->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "MummyWalk");
	}
	if (pageIndex == 2)
	{
		m_pCreditsNPC = CubbyGame::GetInstance()->GetNPCManager()->CreateNPC("CreditsCharacter", "Human", "AshElemental", glm::vec3(24.0f, 0.51f, 4.5f), 0.08f, false, false);
		m_pCreditsNPC->SetCreditsNPC(true);
		//m_pCreditsNPC->LookAtPoint(vec3(0.0f, 8.0f, 1.0f));
	}
	if (pageIndex == 3)
	{
		m_pCreditsNPC = CubbyGame::GetInstance()->GetNPCManager()->CreateNPC("CreditsCharacter", "Human", "Ranger", glm::vec3(26.0f, 0.51f, 4.5f), 0.08f, false, false);
		m_pCreditsNPC->SetCreditsNPC(true);
		//m_pCreditsNPC->LookAtPoint(vec3(0.0f, 8.0f, 1.0f));
	}
}

void Credits::ClearCreditsNPC()
{
	if (m_pCreditsNPC != nullptr)
	{
		CubbyGame::GetInstance()->GetNPCManager()->DeleteNPC(m_pCreditsNPC->GetName());
		m_pCreditsNPC = nullptr;
	}
}

void Credits::SetWindowDimensions(int windowWidth, int windowHeight)
{
	FrontendPage::SetWindowDimensions(windowWidth, windowHeight);
}

// Skinning the GUI
void Credits::SkinGUI()
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();
}

void Credits::UnSkinGUI()
{
}

// Loading
void Credits::Load()
{
	m_currentCreditPage = 0;
	m_numCreditPages = 4;

	SetTitleAndSubtitle("CREDITS", "See who made this wonderful game.");

	m_pCreditsNPC = nullptr;

	GotoNextCreditPage();
}

void Credits::Unload()
{
	RemoveTitlesAndSubTitle();
	ClearCreditTextLines();
	ClearCreditsNPC();
}

void Credits::GotoNextCreditPage()
{
	if (m_clickToNextTimer > 0.0f)
	{
		// Don't allow clicking through the credits too quickly!
		return;
	}

	if (m_currentCreditPage == m_numCreditPages)
	{
		m_pFrontendManager->SetFrontendScreen(FrontendScreen::MainMenu);
	}
	else
	{
		ClearCreditTextLines();
		CreateCreditTextLinesForPage(m_currentCreditPage);
		ClearCreditsNPC();
		CreateCreditsNPCForPage(m_currentCreditPage);

		m_clickToNextTimer = 1.5f;

		m_currentCreditPage++;
	}
}

void Credits::Update(float dt)
{
	FrontendPage::Update(dt);

	m_cameraPosition = glm::vec3(25.0f, 2.25f, 8.0f);
	m_cameraView = glm::vec3(25.0f, 1.5f, 0.0f);

	if (m_clickToNextTimer > 0.0f)
	{
		m_clickToNextTimer -= dt;
	}
}

void Credits::Render()
{
	FrontendPage::Render();
}

void Credits::Render2D()
{
	FrontendPage::Render2D();

	for (size_t i = 0; i < static_cast<int>(m_vpCreditLineList.size()); ++i)
	{
		CreditTextLine* pCreditsLine = m_vpCreditLineList[i];

		float xText = m_windowWidth * pCreditsLine->m_screenWidthRatio;
		float yText = m_windowHeight - pCreditsLine->m_screenHeightDifference;

		char displayText[256];
		sprintf(displayText, "%s", pCreditsLine->m_text.c_str());

		unsigned int fontID = pCreditsLine->m_fontId;
		unsigned int outlineFontID = pCreditsLine->m_outlineFontId;

		int textWidth = m_pRenderer->GetFreeTypeTextWidth(fontID, "%s", displayText);
		xText -= static_cast<int>(textWidth * 0.5f);

		m_pRenderer->PushMatrix();
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		m_pRenderer->SetProjectionMode(ProjectionMode::TWO_DIMENSION, CubbyGame::GetInstance()->GetDefaultViewport());
		m_pRenderer->SetLookAtCamera(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		m_pRenderer->RenderFreeTypeText(fontID, xText, yText, 1.0f, Color(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, displayText);
		m_pRenderer->RenderFreeTypeText(outlineFontID, xText, yText, 1.0f, Color(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, displayText);
		m_pRenderer->DisableTransparency();
		m_pRenderer->PopMatrix();
	}
}