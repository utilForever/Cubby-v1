/*************************************************************************
> File Name: SelectCharacter.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A front-end page to allow the player to select which character they want
>    to play as, this page shows a line up of characters (NPCs) that the player
>    can select from. Also includes character customization and items equipped.
> Created Time: 2016/09/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif //_WIN32

#include <CubbyGame.h>

#include <Player/PlayerStats.h>
#include <Utils/FileUtils.h>

#include "../FrontendManager.h"

#include "SelectCharacter.h"

// Constructor, Destructor
SelectCharacter::SelectCharacter(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontPageManager, int windowWidth, int windowHeight) :
	FrontendPage(pRenderer, pGUI, pFrontPageManager, FrontendScreen::SelectCharacter, windowWidth, windowHeight)
{
	m_pSelectedNPC = nullptr;

	m_toolTipVisible = false;

	// Tooltip
	m_pTooltipBackground = new Icon(m_pRenderer, "", 200, 220);
	m_pTooltipBackground->SetDepth(5.5f);

	char nameText[] = "[NAME]";
	m_pTooltipNameLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), nameText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pTooltipNameLabel->SetOutline(true);
	m_pTooltipNameLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipNameLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont30Outline());
	m_pTooltipNameLabel->SetDepth(5.5f);

	char descText[] = "[DESCRIPTION]";
	m_pTooltipDescriptionLabel = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), descText);
	m_pTooltipDescriptionLabel->SetOutline(true);
	m_pTooltipDescriptionLabel->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pTooltipDescriptionLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTooltipDescriptionLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_pTooltipDescriptionLabel->SetDepth(5.5f);
	m_pTooltipDescriptionLabel->SetWordWrap(true);

	// Popup
	char popupTitleText[] = "[POPUP TITLE]";
	m_popupTitle = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont40(), popupTitleText, Color(1.0f, 0.0f, 0.0f, 1.0f));
	m_popupTitle->SetOutline(true);
	m_popupTitle->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_popupTitle->SetOutlineFont(m_pFrontendManager->GetFrontendFont40Outline());
	m_popupTitle->SetDepth(9.0f);

	char popupText[] = "[POPUP TEXT]";
	m_popupText = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25(), popupText);
	m_popupText->SetOutline(true);
	m_popupText->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_popupText->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_popupText->SetOutlineFont(m_pFrontendManager->GetFrontendFont25Outline());
	m_popupText->SetDepth(9.0f);
	m_popupText->SetWordWrap(true);
	m_popupText->SetHorizontalAlignment(HorizontalAlignment::Center);

	m_pPopupBackgroundIcon = new Icon(m_pRenderer, "", 270, 200);
	m_pPopupBackgroundIcon->SetDepth(2.0f);

	m_pPopupConfirmButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "Yes", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pPopupConfirmButton->SetLabelOffset(0, 3);
	m_pPopupConfirmButton->SetPressedOffset(0, -4);
	m_pPopupConfirmButton->SetCallBackFunction(_PopupConfirmPressed);
	m_pPopupConfirmButton->SetCallBackData(this);
	m_pPopupConfirmButton->SetDepth(9.0f);

	m_pPopupCancelButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "No", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pPopupCancelButton->SetLabelOffset(0, 3);
	m_pPopupCancelButton->SetPressedOffset(0, -4);
	m_pPopupCancelButton->SetCallBackFunction(_PopupCancelPressed);
	m_pPopupCancelButton->SetCallBackData(this);
	m_pPopupCancelButton->SetDepth(9.1f);

	m_pCreateCharacterButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont40(), m_pFrontendManager->GetFrontendFont40Outline(), "Create Character", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCreateCharacterButton->SetLabelOffset(0, 5);
	m_pCreateCharacterButton->SetPressedOffset(0, -4);
	m_pCreateCharacterButton->SetCallBackFunction(_CreateCharacterPressed);
	m_pCreateCharacterButton->SetCallBackData(this);

	m_pSelectCharacterButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont50(), m_pFrontendManager->GetFrontendFont50Outline(), "Select", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pSelectCharacterButton->SetLabelOffset(0, 5);
	m_pSelectCharacterButton->SetPressedOffset(0, -4);
	m_pSelectCharacterButton->SetCallBackFunction(_SelectCharacterPressed);
	m_pSelectCharacterButton->SetCallBackData(this);

	m_pDeleteCharacterButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont35(), m_pFrontendManager->GetFrontendFont35Outline(), "Delete", Color(0.9f, 0.15f, 0.15f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pDeleteCharacterButton->SetLabelOffset(0, 3);
	m_pDeleteCharacterButton->SetPressedOffset(0, -4);
	m_pDeleteCharacterButton->SetCallBackFunction(_DeleteCharacterPressed);
	m_pDeleteCharacterButton->SetCallBackData(this);

	m_pBackButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "Back", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pBackButton->SetLabelOffset(0, 3);
	m_pBackButton->SetPressedOffset(0, -4);
	m_pBackButton->SetCallBackFunction(_BackPressed);
	m_pBackButton->SetCallBackData(this);

	SelectCharacter::SetWindowDimensions(m_windowWidth, m_windowHeight);
}

SelectCharacter::~SelectCharacter()
{
	delete m_pTooltipBackground;
	delete m_pTooltipNameLabel;
	delete m_pTooltipDescriptionLabel;

	delete m_popupTitle;
	delete m_popupText;
	delete m_pPopupConfirmButton;
	delete m_pPopupCancelButton;
	delete m_pPopupBackgroundIcon;

	delete m_pSelectCharacterButton;
	delete m_pDeleteCharacterButton;
	delete m_pCreateCharacterButton;
	delete m_pBackButton;
}

void SelectCharacter::Reset()
{
	// Do nothing
}

void SelectCharacter::SetWindowDimensions(int windowWidth, int windowHeight)
{
	FrontendPage::SetWindowDimensions(windowWidth, windowHeight);

	m_tooltipWidth = 200;
	m_tooltipHeight = 220;
	m_tooltipDescBorder = 15;

	m_popupWidth = 270;
	m_popupHeight = 200;
	m_popupBorderSpacer = 25;
	m_popupTitleSpacer = 35;
	m_popupIconSize = 50;
	m_popupIconSpacer = 10;

	m_pPopupBackgroundIcon->SetDimensions(static_cast<int>((windowWidth * 0.5f) - (m_popupWidth * 0.5f)), static_cast<int>((windowHeight * 0.5f) - (m_popupHeight * 0.5f)) + 225, m_popupWidth, m_popupHeight);

	m_pCreateCharacterButton->SetDimensions(48, 40, 225, 75);
	m_pSelectCharacterButton->SetDimensions(static_cast<int>((m_pFrontendManager->GetWindowWidth() * 0.5f) - (225 * 0.5f)), 40, 225, 75);
	m_pDeleteCharacterButton->SetDimensions((m_pFrontendManager->GetWindowWidth() - 225 + 92 - 82), 100, 150, 65);
	m_pBackButton->SetDimensions((m_pFrontendManager->GetWindowWidth() - 225 + 112 - 82), 20, 110, 47);
}

void SelectCharacter::SkinGUI()
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/common/popup_background.tga";
	m_pPopupBackgroundIcon->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/common/Tooltips/tooltip_background_common.tga";
	m_pTooltipBackground->SetIcon(iconName);

	m_pFrontendManager->SetButtonIcons(m_pPopupConfirmButton, ButtonSize::Size110x47);
	m_pFrontendManager->SetButtonIcons(m_pPopupCancelButton, ButtonSize::Size110x47);

	m_pFrontendManager->SetButtonIcons(m_pSelectCharacterButton, ButtonSize::Size225x75);
	m_pFrontendManager->SetButtonIcons(m_pCreateCharacterButton, ButtonSize::Size225x75);
	m_pFrontendManager->SetButtonIcons(m_pDeleteCharacterButton, ButtonSize::Size110x47);
	m_pFrontendManager->SetButtonIcons(m_pBackButton, ButtonSize::Size110x47);

	m_pDeleteCharacterButton->SetDimensions((m_pFrontendManager->GetWindowWidth() - 225 + 92 - 82), 100, 150, 65);

	m_pSelectCharacterButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pSelectCharacterButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pSelectCharacterButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pCreateCharacterButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pCreateCharacterButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pCreateCharacterButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pBackButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pBackButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pBackButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pPopupConfirmButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pPopupConfirmButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pPopupConfirmButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pPopupCancelButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pPopupCancelButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pPopupCancelButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
}

void SelectCharacter::UnSkinGUI()
{
	// Do nothing
}

void SelectCharacter::Load()
{
	m_pSelectCharacterButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pDeleteCharacterButton->SetLabelColor(Color(0.9f, 0.15f, 0.15f, 1.0f));
	m_pCreateCharacterButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pBackButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());

	m_pPopupConfirmButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pPopupCancelButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());

	LoadSelectionCharacters();

	m_pHoverSelectionNPC = nullptr;
	m_pSelectedNPC = nullptr;

	m_nameTextFadeInAlpha = 0.0f;
	m_nameTextFadeInFadeInTime = 2.5f;
	m_nameTextFadeInFadeInTimer = 0.0f;

	m_addedCreateButton = false;
	m_updateCreateButton = false;
	if (GetNumCreatedCharacters() < 6)
	{
		m_pGUI->AddComponent(m_pCreateCharacterButton);
		m_addedCreateButton = true;
	}
	m_pGUI->AddComponent(m_pBackButton);

	SetTitleAndSubtitle("SELECT CHARACTER", "Which adventurer do you want to play as?");
}

void SelectCharacter::Unload()
{
	HideTooltip();

	ClosePopup();

	m_pGUI->RemoveComponent(m_pPopupBackgroundIcon);

	m_pGUI->RemoveComponent(m_pSelectCharacterButton);
	m_pGUI->RemoveComponent(m_pDeleteCharacterButton);
	m_pGUI->RemoveComponent(m_pCreateCharacterButton);
	m_pGUI->RemoveComponent(m_pBackButton);

	for (size_t i = 0; i < m_vpCharacterLineUp.size(); ++i)
	{
		m_vpCharacterLineUp[i]->UnloadWeapon(true);
		m_vpCharacterLineUp[i]->UnloadWeapon(false);
		CubbyGame::GetInstance()->GetNPCManager()->DeleteNPC(m_vpCharacterLineUp[i]->GetName());
	}
	m_vpCharacterLineUp.clear();

	RemoveTitlesAndSubTitle();
}

void SelectCharacter::LoadSelectionCharacters()
{
	float xPosition = 0.0f;
	float zPosition = 0.0f;

	char importDirectory[128];
	sprintf(importDirectory, "Saves/characters/*.*");

	std::vector<std::string> listFiles;
	listFiles = ListFilesInDirectory(importDirectory);
	int characterNumCounter = 0;
	for (size_t i = 0; i < listFiles.size(); ++i)
	{
		if (strcmp(listFiles[i].c_str(), ".") == 0 || strcmp(listFiles[i].c_str(), "..") == 0)
		{
			continue;
		}

		switch (characterNumCounter)
		{
		case 0: { xPosition = 21.0f; zPosition = 1.5f; } break;
		case 1: { xPosition = 22.5f; zPosition = 0.5f; } break;
		case 2: { xPosition = 24.0f; zPosition = -0.5f; } break;
		case 3: { xPosition = 26.0f; zPosition = -0.5f; } break;
		case 4: { xPosition = 27.5f; zPosition = 0.5f; } break;
		case 5: { xPosition = 29.0f; zPosition = 1.5f; } break;
		default: throw std::logic_error("Invalid Character Number in LoadSelectionCharacters()");
		}

		glm::vec3 centralLookPoint = glm::vec3(25.0f, 1.0f, 5.0f);

		NPC* pCharacter = CubbyGame::GetInstance()->GetNPCManager()->CreateNPC(listFiles[i].c_str(), "Human", listFiles[i].c_str(), glm::vec3(xPosition, 0.5f, zPosition), 0.08f, true, false);
		pCharacter->SetTargetForwardToLookAtPoint(centralLookPoint);
		pCharacter->SetBodyTurnStopThreshold(0.01f);
		pCharacter->SetBodyTurnSpeedMultiplier(6.0f);
		pCharacter->SetPushingCollisionEnabled(false);
		pCharacter->SetFrontEndNPC(true);

		PlayerStats playerStats(nullptr);
		playerStats.ImportStats(listFiles[i].c_str());
		pCharacter->SetPlayerClass(playerStats.GetClass());

		char inventoryFile[128];
		sprintf(inventoryFile, "Saves/characters/%s/%s.inv", listFiles[i].c_str(), listFiles[i].c_str());
		pCharacter->ImportEquippedItems(inventoryFile);

		m_vpCharacterLineUp.push_back(pCharacter);

		characterNumCounter++;
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
int SelectCharacter::GetNumCreatedCharacters() const
{
	char importDirectory[128];
	sprintf(importDirectory, "Saves/characters/*.*");

	std::vector<std::string> listFiles;
	listFiles = ListFilesInDirectory(importDirectory);
	int characterNumCounter = 0;
	for (size_t i = 0; i < listFiles.size(); ++i)
	{
		if (strcmp(listFiles[i].c_str(), ".") == 0 || strcmp(listFiles[i].c_str(), "..") == 0)
		{
			continue;
		}

		characterNumCounter++;
	}

	return characterNumCounter;
}

void SelectCharacter::SetHoverNPC(NPC* pHoverNPC)
{
	if (pHoverNPC != m_pHoverSelectionNPC)
	{
		m_nameTextFadeInFadeInTimer = 0.0f;
	}

	m_pHoverSelectionNPC = pHoverNPC;
}

void SelectCharacter::SetSelectedNPC(NPC* pSelectedNPC)
{
	if (m_pSelectedNPC != nullptr && pSelectedNPC == nullptr)
	{
		// If we are resetting the selected character, make sure to set the current selected character back to normal
		m_pSelectedNPC->GetVoxelCharacter()->PlayFacialExpression("Normal");
		m_pSelectedNPC->GetVoxelCharacter()->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose", 0.25f);
	}

	ClosePopup();

	m_pSelectedNPC = pSelectedNPC;

	if (m_pSelectedNPC != nullptr)
	{
		//glm::vec2 screenPos = m_pSelectedNPC->GetScreenPosition();
		//screenPos.y -= 250;
		//screenPos.x -= m_tooltipWidth * 0.5f;

		//ShowTooltip((int)screenPos.x, (int)screenPos.y);

		if (m_pGUI->GetComponent(m_pSelectCharacterButton) == nullptr)
		{
			m_pGUI->AddComponent(m_pSelectCharacterButton);
		}

		if (m_pGUI->GetComponent(m_pDeleteCharacterButton) == nullptr)
		{
			m_pGUI->AddComponent(m_pDeleteCharacterButton);
		}
	}
	else
	{
		HideTooltip();

		m_pGUI->RemoveComponent(m_pSelectCharacterButton);
		m_pGUI->RemoveComponent(m_pDeleteCharacterButton);
	}
}

void SelectCharacter::ShowTooltip(int x, int y)
{
	if (m_pSelectedNPC != nullptr)
	{
		m_pTooltipNameLabel->SetText(m_pSelectedNPC->GetName());

		std::string characterDescription = "[C=Custom(888888)]Level: [C=White]" + std::string("8") + "\n[C=Custom(888888)]Race: [C=Green]" + m_pSelectedNPC->GetType() + "\n[C=Custom(888888)]Class: [C=Red]" + m_pSelectedNPC->GetModelName();
		m_pTooltipDescriptionLabel->SetText(characterDescription);
	}

	m_pTooltipBackground->SetDimensions(x, y, m_tooltipWidth, m_tooltipHeight);

	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont30(), "%s", m_pTooltipNameLabel->GetText().c_str());
	m_pTooltipNameLabel->SetLocation(x + static_cast<int>(m_tooltipWidth * 0.5f) - static_cast<int>(textWidth * 0.5f), y + m_tooltipHeight - 35);

	m_pTooltipDescriptionLabel->SetDimensions(x + m_tooltipDescBorder, y + m_tooltipDescBorder, m_tooltipWidth - (m_tooltipDescBorder * 2), m_tooltipHeight - (m_tooltipDescBorder * 2) - 35);

	if (m_toolTipVisible == false)
	{
		m_pGUI->AddComponent(m_pTooltipBackground);
		m_pGUI->AddComponent(m_pTooltipNameLabel);
		m_pGUI->AddComponent(m_pTooltipDescriptionLabel);
	}

	m_toolTipVisible = true;
}

void SelectCharacter::HideTooltip()
{
	m_pGUI->RemoveComponent(m_pTooltipBackground);
	m_pGUI->RemoveComponent(m_pTooltipNameLabel);
	m_pGUI->RemoveComponent(m_pTooltipDescriptionLabel);

	m_toolTipVisible = false;
}

void SelectCharacter::OpenPopup(std::string popupTitle, std::string popupText)
{
	m_pPopupConfirmButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pPopupCancelButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());

	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont40(), "%s", popupTitle.c_str());
	m_popupTitle->SetLocation(static_cast<int>((m_windowWidth * 0.5f) - (textWidth * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) + (m_popupHeight * 0.5f)) - m_popupTitleSpacer - m_popupBorderSpacer + 225);
	m_popupText->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (m_popupWidth * 0.5f)) + m_popupBorderSpacer, static_cast<int>((m_windowHeight * 0.5f) - (m_popupHeight * 0.5f)) + 225, m_popupWidth - (m_popupBorderSpacer * 2), m_popupHeight - m_popupBorderSpacer - m_popupTitleSpacer);
	m_pPopupConfirmButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) + (m_popupWidth * 0.5f)) - static_cast<int>(m_popupBorderSpacer * 0.5f) - 110, static_cast<int>((m_windowHeight * 0.5f) - (m_popupIconSize * 0.5f)) - 50 + 225, 110, 47);
	m_pPopupCancelButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (m_popupWidth * 0.5f)) + static_cast<int>(m_popupBorderSpacer * 0.5f), static_cast<int>((m_windowHeight * 0.5f) - (m_popupIconSize * 0.5f)) - 50 + 225, 110, 47);

	m_popupTitle->SetText(popupTitle);
	m_popupText->SetText(popupText);

	m_pGUI->AddComponent(m_popupTitle);
	m_pGUI->AddComponent(m_popupText);
	m_pGUI->AddComponent(m_pPopupConfirmButton);
	m_pGUI->AddComponent(m_pPopupCancelButton);
	m_pGUI->AddComponent(m_pPopupBackgroundIcon);
}

void SelectCharacter::ClosePopup()
{
	m_pGUI->RemoveComponent(m_popupTitle);
	m_pGUI->RemoveComponent(m_popupText);
	m_pGUI->RemoveComponent(m_pPopupConfirmButton);
	m_pGUI->RemoveComponent(m_pPopupCancelButton);
	m_pGUI->RemoveComponent(m_pPopupBackgroundIcon);
}

void SelectCharacter::Update(float dt)
{
	FrontendPage::Update(dt);

	m_cameraPosition = glm::vec3(25.0f, 2.75f, 8.0f);
	m_cameraView = glm::vec3(25.0f, 2.0f, 0.0f);

	// Character fade in text alpha
	if (m_nameTextFadeInFadeInTimer < m_nameTextFadeInFadeInTime)
	{
		m_nameTextFadeInFadeInTimer += dt;
	}
	else
	{
		m_nameTextFadeInFadeInTimer = m_nameTextFadeInFadeInTime;
	}

	if (m_updateCreateButton)
	{
		if (GetNumCreatedCharacters() < 6)
		{
			if (m_addedCreateButton == false)
			{
				m_pGUI->AddComponent(m_pCreateCharacterButton);
				m_addedCreateButton = true;
			}
		}
		else
		{
			if (m_addedCreateButton == true)
			{
				m_pGUI->RemoveComponent(m_pCreateCharacterButton);
				m_addedCreateButton = false;
			}
		}

		m_updateCreateButton = false;
	}
}

void SelectCharacter::Render()
{
	FrontendPage::Render();
}

void SelectCharacter::Render2D()
{
	FrontendPage::Render2D();

	if (m_pHoverSelectionNPC != nullptr && m_pHoverSelectionNPC != m_pSelectedNPC)
	{
		glm::vec2 screenPos = m_pHoverSelectionNPC->GetScreenPosition();
		char characterName[32];
		sprintf(characterName, "%s", m_pHoverSelectionNPC->GetName().c_str());

		int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont80(), "%s", characterName);
		float xText = screenPos.x - static_cast<int>(textWidth * 0.5f);
		float yText = screenPos.y + 250.0f;

		m_nameTextFadeInAlpha = m_nameTextFadeInFadeInTimer / m_nameTextFadeInFadeInTime;

		m_pRenderer->PushMatrix();
		
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		
		m_pRenderer->SetProjectionMode(ProjectionMode::TWO_DIMENSION, CubbyGame::GetInstance()->GetDefaultViewport());
		m_pRenderer->SetLookAtCamera(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		m_pRenderer->RenderFreeTypeText(m_pFrontendManager->GetFrontendFont80(), xText, yText, 1.0f, Color(1.0f, 1.0f, 1.0f, m_nameTextFadeInAlpha), 1.0f, characterName);
		m_pRenderer->RenderFreeTypeText(m_pFrontendManager->GetFrontendFont80Outline(), xText, yText, 1.0f, Color(0.0f, 0.0f, 0.0f, m_nameTextFadeInAlpha), 1.0f, characterName);
		
		m_pRenderer->DisableTransparency();
		
		m_pRenderer->PopMatrix();
	}

	if (m_pSelectedNPC != nullptr)
	{
		glm::vec2 screenPos = m_pSelectedNPC->GetScreenPosition();
		char characterName[32];
		sprintf(characterName, "%s", m_pSelectedNPC->GetModelName().c_str());

		int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont80(), "%s", characterName);
		float xText = screenPos.x - static_cast<int>(textWidth * 0.5f);
		float yText = screenPos.y + 250.0f;

		m_pRenderer->PushMatrix();
		
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		
		m_pRenderer->SetProjectionMode(ProjectionMode::TWO_DIMENSION, CubbyGame::GetInstance()->GetDefaultViewport());
		m_pRenderer->SetLookAtCamera(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		m_pRenderer->RenderFreeTypeText(m_pFrontendManager->GetFrontendFont80(), xText, yText, 1.0f, Color(1.0f, 1.0f, 0.0f, 1.0f), 1.0f, characterName);
		m_pRenderer->RenderFreeTypeText(m_pFrontendManager->GetFrontendFont80Outline(), xText, yText, 1.0f, Color(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, characterName);
		
		m_pRenderer->DisableTransparency();
		
		m_pRenderer->PopMatrix();
	}
}

void SelectCharacter::_SelectCharacterPressed(void* pData)
{
	SelectCharacter* pSelectCharacter = static_cast<SelectCharacter*>(pData);
	pSelectCharacter->SelectCharacterPressed();
}

void SelectCharacter::SelectCharacterPressed()
{
	std::string characterName = m_pSelectedNPC->GetName();

	CubbyGame::GetInstance()->GetPlayer()->ResetPlayer();
	CubbyGame::GetInstance()->GetPlayer()->SetName(characterName.c_str());
	CubbyGame::GetInstance()->GetPlayer()->SetClass(m_pSelectedNPC->GetPlayerClass());
	CubbyGame::GetInstance()->GetPlayer()->LoadCharacter(characterName.c_str(), true);

	m_pSelectedNPC = nullptr;

	//m_pFrontendManager->SetFrontendScreen(FrontendScreen::SelectWorld);

	CubbyGame::GetInstance()->StartGameFromFrontEnd();
}

void SelectCharacter::_DeleteCharacterPressed(void* pData)
{
	SelectCharacter* pSelectCharacter = static_cast<SelectCharacter*>(pData);
	pSelectCharacter->DeleteCharacterPressed();
}

void SelectCharacter::DeleteCharacterPressed()
{
	char popupTitle[256];
	sprintf(popupTitle, "Delete");

	char popupText[256];
	sprintf(popupText, "Are you sure you want to delete [C=Custom(00A2E8)]%s[C=White]?", m_pSelectedNPC->GetName().c_str());

	OpenPopup(popupTitle, popupText);

	if (m_pSelectedNPC != nullptr)
	{
		m_pSelectedNPC->GetVoxelCharacter()->PlayFacialExpression("Cry");
		m_pSelectedNPC->GetVoxelCharacter()->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "Cry", 0.5f);
	}
}

void SelectCharacter::_CreateCharacterPressed(void* pData)
{
	SelectCharacter* pSelectCharacter = static_cast<SelectCharacter*>(pData);
	pSelectCharacter->CreateCharacterPressed();
}

void SelectCharacter::CreateCharacterPressed() const
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::CreateCharacter);
}

void SelectCharacter::_BackPressed(void* pData)
{
	SelectCharacter* pSelectCharacter = static_cast<SelectCharacter*>(pData);
	pSelectCharacter->BackPressed();
}

void SelectCharacter::BackPressed() const
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::MainMenu);
}

void SelectCharacter::_PopupConfirmPressed(void* pData)
{
	SelectCharacter* pSelectCharacter = static_cast<SelectCharacter*>(pData);
	pSelectCharacter->PopupConfirmPressed();
}

void SelectCharacter::PopupConfirmPressed()
{
	std::string characterName = m_pSelectedNPC->GetName();

	char characterBaseFolder[128];
	char qbFileName[128];
	char facesFileName[128];
	char characterFileName[128];
	char inventoryFileName[128];
	char statsFileName[128];
	char actionbarFileName[128];

	sprintf(characterBaseFolder, "Saves/characters/%s", characterName.c_str());
	sprintf(qbFileName, "Saves/characters/%s/%s.qb", characterName.c_str(), characterName.c_str());
	sprintf(facesFileName, "Saves/characters/%s/%s.faces", characterName.c_str(), characterName.c_str());
	sprintf(characterFileName, "Saves/characters/%s/%s.character", characterName.c_str(), characterName.c_str());
	sprintf(inventoryFileName, "Saves/characters/%s/%s.inv", characterName.c_str(), characterName.c_str());
	sprintf(statsFileName, "Saves/characters/%s/%s.stats", characterName.c_str(), characterName.c_str());
	sprintf(actionbarFileName, "Saves/characters/%s/%s.actionbar", characterName.c_str(), characterName.c_str());

	remove(qbFileName);
	remove(facesFileName);
	remove(characterFileName);
	remove(inventoryFileName);
	remove(statsFileName);
	remove(actionbarFileName);

	// Remove the directory
#ifdef _WIN32
		int error = _rmdir(characterBaseFolder);
#else
		int error = rmdir(characterBaseFolder);
#endif //_WIN32		

	auto iter = find(m_vpCharacterLineUp.begin(), m_vpCharacterLineUp.end(), m_pSelectedNPC);
	if (iter != m_vpCharacterLineUp.end())
	{
		m_vpCharacterLineUp.erase(iter);
	}

	m_updateCreateButton = true;

	m_pSelectedNPC->Explode();
	CubbyGame::GetInstance()->GetNPCManager()->DeleteNPC(m_pSelectedNPC->GetName());
	m_pSelectedNPC = nullptr;

	HideTooltip();
	m_pGUI->RemoveComponent(m_pSelectCharacterButton);
	m_pGUI->RemoveComponent(m_pDeleteCharacterButton);

	ClosePopup();
}

void SelectCharacter::_PopupCancelPressed(void* pData)
{
	SelectCharacter* pSelectCharacter = static_cast<SelectCharacter*>(pData);
	pSelectCharacter->PopupCancelPressed();
}

void SelectCharacter::PopupCancelPressed()
{
	ClosePopup();

	if (m_pSelectedNPC != nullptr)
	{
		m_pSelectedNPC->GetVoxelCharacter()->PlayFacialExpression("Normal");
		m_pSelectedNPC->GetVoxelCharacter()->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose", 0.25f);
	}
}