/*************************************************************************
> File Name: CubbyGUI.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Implement CubbyGame's methods related to GUI.
> Created Time: 2016/09/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Utils/FileUtils.h>

#include "CubbyGame.h"

void CubbyGame::CreateGUI()
{
	m_pMainWindow = new GUIWindow(m_pRenderer, m_defaultFont, "Main");
	m_pMainWindow->AllowMoving(true);
	m_pMainWindow->AllowClosing(false);
	m_pMainWindow->AllowMinimizing(true);
	m_pMainWindow->AllowScrolling(true);
	m_pMainWindow->SetRenderTitleBar(true);
	m_pMainWindow->SetRenderWindowBackground(true);
	m_pMainWindow->SetOutlineRender(true);
	m_pMainWindow->SetDimensions(15, 35, 320, 140);
	m_pMainWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);

	m_pWireframeCheckBox = new CheckBox(m_pRenderer, m_defaultFont, "Wireframe");
	m_pWireframeCheckBox->SetDimensions(10, 10, 14, 14);
	m_pMSAACheckBox = new CheckBox(m_pRenderer, m_defaultFont, "Anti-Aliasing");
	m_pMSAACheckBox->SetDimensions(10, 46, 14, 14);
	m_pDynamicLightingCheckBox = new CheckBox(m_pRenderer, m_defaultFont, "Lighting");
	m_pDynamicLightingCheckBox->SetDimensions(10, 64, 14, 14);
	m_pSSAOCheckBox = new CheckBox(m_pRenderer, m_defaultFont, "SSAO");
	m_pSSAOCheckBox->SetDimensions(10, 82, 14, 14);
	m_pBlurCheckBox = new CheckBox(m_pRenderer, m_defaultFont, "Blur");
	m_pBlurCheckBox->SetDimensions(10, 100, 14, 14);
	m_pDeferredCheckBox = new CheckBox(m_pRenderer, m_defaultFont, "Deferred");
	m_pDeferredCheckBox->SetDimensions(10, 118, 14, 14);
	m_pUpdateCheckBox = new CheckBox(m_pRenderer, m_defaultFont, "Update");
	m_pUpdateCheckBox->SetDimensions(110, 28, 14, 14);
	m_pUpdateCheckBox->SetToggled(true);
	m_pDebugRenderCheckBox = new CheckBox(m_pRenderer, m_defaultFont, "DebugRender");
	m_pDebugRenderCheckBox->SetDimensions(110, 10, 14, 14);
	m_pInstanceRenderCheckBox = new CheckBox(m_pRenderer, m_defaultFont, "Instance Particles");
	m_pInstanceRenderCheckBox->SetDimensions(110, 46, 14, 14);

	m_pPlayAnimationButton = new Button(m_pRenderer, m_defaultFont, "Play Anim");
	m_pPlayAnimationButton->SetDimensions(230, 40, 85, 25);
	m_pPlayAnimationButton->SetCallBackFunction(_PlayAnimationPressed);
	m_pPlayAnimationButton->SetCallBackData(this);

	m_pAnimationsPulldown = new PulldownMenu(m_pRenderer, m_defaultFont, "Animation");
	m_pAnimationsPulldown->SetDimensions(150, 70, 140, 14);
	m_pAnimationsPulldown->SetMaxNumItemsDisplayed(5);
	m_pAnimationsPulldown->SetDepth(2.0f);
	m_pAnimationsPulldown->SetRenderHeader(true);
	m_pAnimationsPulldown->SetMenuItemChangedCallBackFunction(_AnimationPullDownChanged);
	m_pAnimationsPulldown->SetMenuItemChangedCallBackData(this);

	m_pCharacterPulldown = new PulldownMenu(m_pRenderer, m_defaultFont, "Character");
	m_pCharacterPulldown->SetDimensions(150, 115, 140, 14);
	m_pCharacterPulldown->SetMaxNumItemsDisplayed(5);
	m_pCharacterPulldown->SetDepth(4.0f);
	m_pCharacterPulldown->SetRenderHeader(true);
	m_pCharacterPulldown->SetMenuItemChangedCallBackFunction(_CharacterPullDownChanged);
	m_pCharacterPulldown->SetMenuItemChangedCallBackData(this);

	m_pMainWindow->AddComponent(m_pSSAOCheckBox);
	m_pMainWindow->AddComponent(m_pBlurCheckBox);
	m_pMainWindow->AddComponent(m_pDynamicLightingCheckBox);
	m_pMainWindow->AddComponent(m_pWireframeCheckBox);
	m_pMainWindow->AddComponent(m_pMSAACheckBox);
	m_pMainWindow->AddComponent(m_pDeferredCheckBox);
	m_pMainWindow->AddComponent(m_pUpdateCheckBox);
	m_pMainWindow->AddComponent(m_pDebugRenderCheckBox);
	m_pMainWindow->AddComponent(m_pInstanceRenderCheckBox);
	m_pMainWindow->AddComponent(m_pPlayAnimationButton);
	m_pMainWindow->AddComponent(m_pAnimationsPulldown);
	m_pMainWindow->AddComponent(m_pCharacterPulldown);

	m_pGameWindow = new GUIWindow(m_pRenderer, m_defaultFont, "Game");
	m_pGameWindow->AllowMoving(true);
	m_pGameWindow->AllowClosing(false);
	m_pGameWindow->AllowMinimizing(true);
	m_pGameWindow->AllowScrolling(true);
	m_pGameWindow->SetRenderTitleBar(true);
	m_pGameWindow->SetRenderWindowBackground(true);
	m_pGameWindow->SetOutlineRender(true);
	m_pGameWindow->SetDimensions(350, 35, 270, 140);
	m_pGameWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);

	m_pGameOptionBox = new OptionBox(m_pRenderer, m_defaultFont, "Game");
	m_pGameOptionBox->SetDimensions(10, 50, 14, 14);
	m_pGameOptionBox->SetCallBackFunction(_GameModeChanged);
	m_pGameOptionBox->SetCallBackData(this);
	m_pFrontEndOptionBox = new OptionBox(m_pRenderer, m_defaultFont, "Frontend");
	m_pFrontEndOptionBox->SetDimensions(10, 30, 14, 14);
	m_pFrontEndOptionBox->SetCallBackFunction(_GameModeChanged);
	m_pFrontEndOptionBox->SetCallBackData(this);
	m_pDebugOptionBox = new OptionBox(m_pRenderer, m_defaultFont, "Debug");
	m_pDebugOptionBox->SetDimensions(10, 10, 14, 14);
	m_pDebugOptionBox->SetCallBackFunction(_GameModeChanged);
	m_pDebugOptionBox->SetCallBackData(this);
	m_pGameModeOptionController = new OptionController(m_pRenderer, m_defaultFont, "Mode");
	m_pGameModeOptionController->SetDisplayLabel(true);
	m_pGameModeOptionController->SetDisplayBorder(true);
	m_pGameModeOptionController->SetDimensions(10, 55, 85, 70);
	m_pGameModeOptionController->Add(m_pGameOptionBox);
	m_pGameModeOptionController->Add(m_pDebugOptionBox);
	m_pGameModeOptionController->Add(m_pFrontEndOptionBox);
	m_pDebugOptionBox->SetToggled(true);

	m_pFaceMergingCheckbox = new CheckBox(m_pRenderer, m_defaultFont, "Face Merging");
	m_pFaceMergingCheckbox->SetDimensions(10, 10, 14, 14);
	m_pFaceMergingCheckbox->SetCallBackFunction(_FaceMergeCheckboxChanged);
	m_pFaceMergingCheckbox->SetCallBackData(this);

	m_pStepUpdateCheckbox = new CheckBox(m_pRenderer, m_defaultFont, "Step Update");
	m_pStepUpdateCheckbox->SetDimensions(110, 10, 14, 14);

	m_pStepUpdateButton = new Button(m_pRenderer, m_defaultFont, "Step");
	m_pStepUpdateButton->SetDimensions(200, 5, 65, 25);
	m_pStepUpdateButton->SetCallBackFunction(_StepUpdatePressed);
	m_pStepUpdateButton->SetCallBackData(this);

	m_pDebugCameraOptionBox = new OptionBox(m_pRenderer, m_defaultFont, "Debug");
	m_pDebugCameraOptionBox->SetDimensions(10, 70, 14, 14);
	m_pDebugCameraOptionBox->SetCallBackFunction(_CameraModeChanged);
	m_pDebugCameraOptionBox->SetCallBackData(this);
	m_pMouseRotateCameraOptionBox = new OptionBox(m_pRenderer, m_defaultFont, "Player Rotate");
	m_pMouseRotateCameraOptionBox->SetDimensions(10, 50, 14, 14);
	m_pMouseRotateCameraOptionBox->SetCallBackFunction(_CameraModeChanged);
	m_pMouseRotateCameraOptionBox->SetCallBackData(this);
	m_pAutoCameraOptionBox = new OptionBox(m_pRenderer, m_defaultFont, "Auto");
	m_pAutoCameraOptionBox->SetDimensions(10, 30, 14, 14);
	m_pAutoCameraOptionBox->SetCallBackFunction(_CameraModeChanged);
	m_pAutoCameraOptionBox->SetCallBackData(this);
	m_pFrontendCameraOptionBox = new OptionBox(m_pRenderer, m_defaultFont, "Frontend");
	m_pFrontendCameraOptionBox->SetDimensions(10, 10, 14, 14);
	m_pFrontendCameraOptionBox->SetCallBackFunction(_CameraModeChanged);
	m_pFrontendCameraOptionBox->SetCallBackData(this);
	m_pCameraModeOptionController = new OptionController(m_pRenderer, m_defaultFont, "Camera");
	m_pCameraModeOptionController->SetDisplayLabel(true);
	m_pCameraModeOptionController->SetDisplayBorder(true);
	m_pCameraModeOptionController->SetDimensions(160, 35, 105, 90);
	m_pCameraModeOptionController->Add(m_pDebugCameraOptionBox);
	m_pCameraModeOptionController->Add(m_pMouseRotateCameraOptionBox);
	m_pCameraModeOptionController->Add(m_pAutoCameraOptionBox);
	m_pCameraModeOptionController->Add(m_pFrontendCameraOptionBox);
	m_pDebugCameraOptionBox->SetToggled(true);
	m_pMouseRotateCameraOptionBox->SetDisabled(true);
	m_pAutoCameraOptionBox->SetDisabled(true);
	m_pFrontendCameraOptionBox->SetDisabled(true);

	m_pGameWindow->AddComponent(m_pGameModeOptionController);
	m_pGameWindow->AddComponent(m_pCameraModeOptionController);
	m_pGameWindow->AddComponent(m_pFaceMergingCheckbox);
	m_pGameWindow->AddComponent(m_pStepUpdateCheckbox);
	m_pGameWindow->AddComponent(m_pStepUpdateButton);

	// Console window
	m_pConsoleWindow = new GUIWindow(m_pRenderer, m_defaultFont, "Console");
	m_pConsoleWindow->AllowMoving(true);
	m_pConsoleWindow->AllowClosing(false);
	m_pConsoleWindow->AllowMinimizing(true);
	m_pConsoleWindow->AllowScrolling(true);
	m_pConsoleWindow->SetRenderTitleBar(true);
	m_pConsoleWindow->SetRenderWindowBackground(true);
	m_pConsoleWindow->SetOutlineRender(true);
	m_pConsoleWindow->SetDimensions(635, 35, 500, 140);
	m_pConsoleWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);

	m_pConsoleTextbox = new TextBox(m_pRenderer, m_defaultFont, "", "");
	m_pConsoleTextbox->SetDimensions(1, 1, 498, 16);
	m_pConsoleTextbox->SetDontLoseFocus(false);
	m_pConsoleTextbox->SetCallBackFunction_OnReturnPressed(_ConsoleReturnPressed);
	m_pConsoleTextbox->SetCallBackData_OnReturnPressed(this);
	m_pConsoleTextbox->SetPipeColor(Color(0.0f, 0.0f, 0.0f));
	m_pConsoleTextbox->SetDontLoseFocus(true);

	m_pConsoleScrollbar = new ScrollBar(m_pRenderer);
	m_pConsoleScrollbar->SetScrollDirection(ScrollBarDirection::Vertical);
	m_pConsoleScrollbar->SetScrollSize(1.0f);
	m_pConsoleScrollbar->SetScrollPosition(1.0f);
	m_pConsoleScrollbar->SetScissorEnabled(true);
	m_pConsoleScrollbar->SetScrollArea(-484, 0, 490, 122);
	m_pConsoleScrollbar->SetDepth(2.0f);
	m_pConsoleScrollbar->SetDimensions(486, 18, 14, 122);
	m_pConsoleScrollbar->SetScissorEnabled(true);

	m_pConsoleWindow->AddComponent(m_pConsoleTextbox);
	m_pConsoleWindow->AddComponent(m_pConsoleScrollbar);

	m_pGUI->AddWindow(m_pMainWindow);
	m_pGUI->AddWindow(m_pGameWindow);
	m_pGUI->AddWindow(m_pConsoleWindow);

	UpdateCharactersPulldown();
	UpdateWeaponsPulldown();
	UpdateAnimationsPulldown();

	m_pCharacterPulldown->SetSelectedItem("Steve");
	m_pAnimationsPulldown->SetSelectedItem("BindPose");

	m_GUICreated = true;
}

void CubbyGame::SetupGUI()
{
	m_pDeferredCheckBox->SetToggled(m_pCubbySettings->m_deferredRendering);
	m_pSSAOCheckBox->SetToggled(m_pCubbySettings->m_ssao);
	m_pBlurCheckBox->SetToggled(m_pCubbySettings->m_blur);
	m_pDynamicLightingCheckBox->SetToggled(m_pCubbySettings->m_dynamicLighting);
	m_pMSAACheckBox->SetToggled(m_pCubbySettings->m_msaa);
	m_pInstanceRenderCheckBox->SetToggled(m_pCubbySettings->m_instancedParticles);
	m_pWireframeCheckBox->SetToggled(m_pCubbySettings->m_wireframeRendering);
	m_pDebugRenderCheckBox->SetToggled(m_pCubbySettings->m_debugRendering);
	m_pFaceMergingCheckbox->SetToggled(m_pCubbySettings->m_faceMerging);
	m_pStepUpdateCheckbox->SetToggled(m_pCubbySettings->m_stepUpdating);

	// Debug GUI
	if (m_pCubbySettings->m_showDebugGUI)
	{
		ShowGUI();
	}
	else
	{
		HideGUI();
	}

	// Game mode
	if (strcmp(m_pCubbySettings->m_gameMode.c_str(), "Debug") == 0)
	{
		m_pDebugOptionBox->SetToggled(true);
	}
	else if (strcmp(m_pCubbySettings->m_gameMode.c_str(), "Game") == 0)
	{
		m_previousCameraMode = CameraMode::MouseRotate;
		m_pGameOptionBox->SetToggled(true);
		m_pMouseRotateCameraOptionBox->SetToggled(true);

		// Supress exporting of files, since we are loading into a DEBUG configuration direct from bootstrap
		m_pInventoryManager->SetSupressExport(true);
		m_pActionBar->SetSupressExport(true);
		m_pPlayer->SetSupressStatsExport(true);
		// Load default inventory since we are loading directly into the game
		m_pInventoryManager->LoadInventory("", PlayerClass::Debug, false);
	}
	else if (strcmp(m_pCubbySettings->m_gameMode.c_str(), "FrontEnd") == 0)
	{
		m_pFrontEndOptionBox->SetToggled(true);
		m_pFrontendCameraOptionBox->SetToggled(true);
	}
	m_pCubbyWindow->Update(m_deltaTime);
	GameModeChanged();
	CameraModeChanged();
}

void CubbyGame::SkinGUI() const
{
	m_pFrontendManager->SetCheckboxIcons(m_pMSAACheckBox);
	m_pFrontendManager->SetCheckboxIcons(m_pDynamicLightingCheckBox);
	m_pFrontendManager->SetCheckboxIcons(m_pSSAOCheckBox);
	m_pFrontendManager->SetCheckboxIcons(m_pBlurCheckBox);
	m_pFrontendManager->SetCheckboxIcons(m_pDeferredCheckBox);
	m_pFrontendManager->SetCheckboxIcons(m_pWireframeCheckBox);
	m_pFrontendManager->SetCheckboxIcons(m_pFaceMergingCheckbox);
	m_pFrontendManager->SetCheckboxIcons(m_pStepUpdateCheckbox);
	m_pFrontendManager->SetCheckboxIcons(m_pUpdateCheckBox);
	m_pFrontendManager->SetCheckboxIcons(m_pDebugRenderCheckBox);
	m_pFrontendManager->SetCheckboxIcons(m_pInstanceRenderCheckBox);

	m_pFrontendManager->SetOptionboxIcons(m_pGameOptionBox);
	m_pFrontendManager->SetOptionboxIcons(m_pDebugOptionBox);
	m_pFrontendManager->SetOptionboxIcons(m_pFrontEndOptionBox);

	m_pFrontendManager->SetOptionboxIcons(m_pDebugCameraOptionBox);
	m_pFrontendManager->SetOptionboxIcons(m_pMouseRotateCameraOptionBox);
	m_pFrontendManager->SetOptionboxIcons(m_pAutoCameraOptionBox);
	m_pFrontendManager->SetOptionboxIcons(m_pFrontendCameraOptionBox);

	m_pFrontendManager->SetPulldownMenuIcons(m_pAnimationsPulldown);
	m_pFrontendManager->SetPulldownMenuIcons(m_pCharacterPulldown);

	m_pFrontendManager->SetScrollbarIcons(m_pConsoleScrollbar);

	m_pFrontendManager->SetButtonIcons(m_pPlayAnimationButton, ButtonSize::Size85x25);
	m_pFrontendManager->SetButtonIcons(m_pStepUpdateButton, ButtonSize::Size65x25);

	// Also skin the frontend pages
	m_pFrontendManager->SkinGUI();

	// Also skin the gameGUI pages
	m_pInventoryGUI->SkinGUI();
	m_pCharacterGUI->SkinGUI();
	m_pCraftingGUI->SkinGUI();
	m_pQuestGUI->SkinGUI();
	m_pLootGUI->SkinGUI();
	m_pActionBar->SkinGUI();
	m_pHUD->SkinGUI();
}

void CubbyGame::UnSkinGUI() const
{
	m_pMSAACheckBox->SetDefaultIcons(m_pRenderer);
	m_pDynamicLightingCheckBox->SetDefaultIcons(m_pRenderer);
	m_pSSAOCheckBox->SetDefaultIcons(m_pRenderer);
	m_pBlurCheckBox->SetDefaultIcons(m_pRenderer);
	m_pDeferredCheckBox->SetDefaultIcons(m_pRenderer);
	m_pWireframeCheckBox->SetDefaultIcons(m_pRenderer);
	m_pFaceMergingCheckbox->SetDefaultIcons(m_pRenderer);
	m_pStepUpdateCheckbox->SetDefaultIcons(m_pRenderer);
	m_pUpdateCheckBox->SetDefaultIcons(m_pRenderer);
	m_pDebugRenderCheckBox->SetDefaultIcons(m_pRenderer);
	m_pInstanceRenderCheckBox->SetDefaultIcons(m_pRenderer);

	m_pGameOptionBox->SetDefaultIcons(m_pRenderer);
	m_pDebugOptionBox->SetDefaultIcons(m_pRenderer);
	m_pFrontEndOptionBox->SetDefaultIcons(m_pRenderer);

	m_pAnimationsPulldown->SetDefaultIcons(m_pRenderer);
	m_pCharacterPulldown->SetDefaultIcons(m_pRenderer);

	m_pDebugCameraOptionBox->SetDefaultIcons(m_pRenderer);
	m_pMouseRotateCameraOptionBox->SetDefaultIcons(m_pRenderer);
	m_pAutoCameraOptionBox->SetDefaultIcons(m_pRenderer);
	m_pFrontendCameraOptionBox->SetDefaultIcons(m_pRenderer);

	m_pPlayAnimationButton->SetDefaultIcons(m_pRenderer);
	m_pStepUpdateButton->SetDefaultIcons(m_pRenderer);

	m_pConsoleScrollbar->SetDefaultIcons(m_pRenderer);

	// Also unskin the front-end pages
	m_pFrontendManager->UnSkinGUI();

	// Also unskin the gameGUI pages
	m_pInventoryGUI->UnSkinGUI();
	m_pCharacterGUI->UnSkinGUI();
	m_pCraftingGUI->UnSkinGUI();
	m_pQuestGUI->UnSkinGUI();
	m_pLootGUI->UnSkinGUI();
	m_pActionBar->UnSkinGUI();
	m_pHUD->UnSkinGUI();
}

void CubbyGame::DestroyGUI()
{
	delete m_pMainWindow;
	delete m_pSSAOCheckBox;
	delete m_pBlurCheckBox;
	delete m_pDynamicLightingCheckBox;
	delete m_pWireframeCheckBox;
	delete m_pMSAACheckBox;
	delete m_pDeferredCheckBox;
	delete m_pUpdateCheckBox;
	delete m_pDebugRenderCheckBox;
	delete m_pInstanceRenderCheckBox;
	delete m_pPlayAnimationButton;
	delete m_pAnimationsPulldown;
	delete m_pCharacterPulldown;
	delete m_pGameWindow;
	delete m_pGameOptionBox;
	delete m_pDebugOptionBox;
	delete m_pFrontEndOptionBox;
	delete m_pGameModeOptionController;
	delete m_pFaceMergingCheckbox;
	delete m_pStepUpdateCheckbox;
	delete m_pStepUpdateButton;
	delete m_pDebugCameraOptionBox;
	delete m_pMouseRotateCameraOptionBox;
	delete m_pAutoCameraOptionBox;
	delete m_pFrontendCameraOptionBox;
	delete m_pCameraModeOptionController;
	delete m_pConsoleWindow;
	delete m_pConsoleTextbox;
	ClearConsoleLabels();
	delete m_pConsoleScrollbar;
}

void CubbyGame::UpdateGUI(float dt)
{
	// Depending on if deferred rendering is enabled, allow or disallow certain other graphic features
	if (m_deferredRendering)
	{
		m_pSSAOCheckBox->SetDisabled(false);
		m_pDynamicLightingCheckBox->SetDisabled(false);
		m_pBlurCheckBox->SetDisabled(false);
	}
	else
	{
		m_pSSAOCheckBox->SetDisabled(true);
		m_pDynamicLightingCheckBox->SetDisabled(true);
		m_pBlurCheckBox->SetDisabled(true);
		m_pMSAACheckBox->SetDisabled(false);
	}

	// If shaders are not loaded, then disable some graphical features
	if (m_SSAOShader == -1)
	{
		m_pSSAOCheckBox->SetToggled(false);
		m_pSSAOCheckBox->SetDisabled(true);
		m_pDeferredCheckBox->SetToggled(false);
		m_pDeferredCheckBox->SetDisabled(true);
		m_pDynamicLightingCheckBox->SetToggled(false);
		m_pDynamicLightingCheckBox->SetDisabled(true);
	}

	if (m_lightingShader == -1)
	{
		m_pDynamicLightingCheckBox->SetToggled(false);
		m_pDynamicLightingCheckBox->SetDisabled(true);
	}

	if (m_shadowShader == -1)
	{
		m_pFrontendManager->DisableShadowOption();
	}

	if (m_pBlockParticleManager->GetInstanceShaderIndex() == -1)
	{
		m_pInstanceRenderCheckBox->SetToggled(false);
		m_pInstanceRenderCheckBox->SetDisabled(true);
	}

	if (m_fxaaShader == -1 && m_deferredRendering)
	{
		m_pMSAACheckBox->SetToggled(false);
		m_pMSAACheckBox->SetDisabled(true);
	}

	if (m_blurVerticalShader == -1 || m_blurHorizontalShader == -1)
	{
		m_pBlurCheckBox->SetToggled(false);
		m_pBlurCheckBox->SetDisabled(true);
	}

	m_ssao = m_pSSAOCheckBox->GetToggled();
	m_blur = m_pBlurCheckBox->GetToggled();
	m_dynamicLighting = m_pDynamicLightingCheckBox->GetToggled();
	m_modelWireframe = m_pWireframeCheckBox->GetToggled();
	m_multiSampling = m_pMSAACheckBox->GetToggled();
	m_deferredRendering = m_pDeferredCheckBox->GetToggled();
	m_animationUpdate = m_pUpdateCheckBox->GetToggled();
	m_debugRender = m_pDebugRenderCheckBox->GetToggled();
	m_instanceRender = m_pInstanceRenderCheckBox->GetToggled();

	m_pPlayer->SetWireFrameRender(m_modelWireframe);
	m_pChunkManager->SetWireframeRender(m_modelWireframe);
	m_pChunkManager->SetFaceMerging(m_pFaceMergingCheckbox->GetToggled());
	m_pChunkManager->SetStepLockEnabled(m_pStepUpdateCheckbox->GetToggled());
	m_pBlockParticleManager->SetWireFrameRender(m_modelWireframe);
	m_pBlockParticleManager->SetInstancedRendering(m_instanceRender);
	m_pItemManager->SetWireFrameRender(m_modelWireframe);
	m_pNPCManager->SetWireFrameRender(m_modelWireframe);
	m_pEnemyManager->SetWireFrameRender(m_modelWireframe);
	m_pInstanceManager->SetWireFrameRender(m_modelWireframe);

	// Update console
	UpdateConsoleLabels();
}

void CubbyGame::GUITurnOffCursor() const
{
	m_pGUI->ResetSelectionManager();
}

void CubbyGame::ShowGUI() const
{
	if (m_pMainWindow->IsVisible() == false)
	{
		m_pMainWindow->Show();
	}
	if (m_pGameWindow->IsVisible() == false)
	{
		m_pGameWindow->Show();
	}
	if (m_pConsoleWindow->IsVisible() == false)
	{
		m_pConsoleWindow->Show();
	}
}

void CubbyGame::HideGUI() const
{
	if (m_pMainWindow->IsVisible() == true)
	{
		m_pMainWindow->Hide();
	}
	if (m_pGameWindow->IsVisible() == true)
	{
		m_pGameWindow->Hide();
	}
	if (m_pConsoleWindow->IsVisible() == true)
	{
		m_pConsoleWindow->Hide();
	}
}

void CubbyGame::UpdateCharactersPulldown() const
{
	m_pCharacterPulldown->RemoveAllPullDownMenuItems();
	m_pCharacterPulldown->ResetPullDownMenu();
	m_pMainWindow->RemoveComponent(m_pCharacterPulldown);

	char importDirectory[128];
	sprintf(importDirectory, "Resources/gamedata/models/Human/*.*");

	std::vector<std::string> listFiles;
	listFiles = ListFilesInDirectory(importDirectory);
	
	for (size_t i = 0; i < listFiles.size(); ++i)
	{
		if (strcmp(listFiles[i].c_str(), ".") == 0 || strcmp(listFiles[i].c_str(), "..") == 0)
		{
			continue;
		}

		int lastIndex = static_cast<int>(listFiles[i].find_last_of("."));
		if (lastIndex != -1)
		{
			std::string characterName = listFiles[i].substr(0, lastIndex);
			std::string extension = listFiles[i].substr(lastIndex);
			if (strcmp(extension.c_str(), ".qb") == 0)
			{
				m_pCharacterPulldown->AddPulldownItem(characterName);
			}
		}
	}

	m_pMainWindow->AddComponent(m_pCharacterPulldown);
	m_pCharacterPulldown->AddEventListeners();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CubbyGame::UpdateWeaponsPulldown() const
{
	// Do nothing
}

void CubbyGame::UpdateAnimationsPulldown() const
{
	m_pAnimationsPulldown->RemoveAllPullDownMenuItems();
	m_pAnimationsPulldown->ResetPullDownMenu();
	m_pMainWindow->RemoveComponent(m_pAnimationsPulldown);

	for (int i = 0; i < m_pPlayer->GetVoxelCharacter()->GetNumAnimations(); i++)
	{
		m_pAnimationsPulldown->AddPulldownItem(m_pPlayer->GetVoxelCharacter()->GetAnimationName(i));
	}

	m_pMainWindow->AddComponent(m_pAnimationsPulldown);
	m_pAnimationsPulldown->AddEventListeners();
}

void CubbyGame::AddConsoleLabel(std::string message)
{
	if (m_GUICreated == false)
	{
		m_vStringCache.push_back(message);

		return;
	}

	char chatChar[8192];
	sprintf(chatChar, "%s", message.c_str());

	std::string chatString = chatChar;

	int charIndex = 0;
	int startLineIndex = 0;
	int previousSpaceIndex = 0;

	// Our position
	float currentTextX = 0.0f;
	int newLineIndex = 1;

	int indexToUse = static_cast<int>(m_vpConsoleLabels.size()) + static_cast<int>(m_vpConsoleLabels_Add.size());

	while (chatChar[charIndex] != 0)
	{
		char pChar = chatChar[charIndex];
		char pNextChar = chatChar[charIndex + 1];

		// Check for spaces
		if (pChar == ' ')
		{
			std::string str(chatString.substr(startLineIndex, charIndex - startLineIndex));
			int textLineWidth = m_pRenderer->GetFreeTypeTextWidth(m_defaultFont, "%s", str.c_str());

			// If the current X position, plus our new text length is greater than the width, then we know we will go out of bounds
			if (currentTextX + textLineWidth > m_pConsoleScrollbar->GetScrollArea().width)
			{
				std::string str2(chatString.substr(startLineIndex, previousSpaceIndex - startLineIndex));

				Label* pNewLabel = new Label(m_pRenderer, m_defaultFont, str2.c_str(), Color(1.0f, 1.0f, 1.0f));
				int xPos = m_pConsoleScrollbar->GetScrollArea().x;
				int yPos = m_pConsoleScrollbar->GetScrollArea().y + m_pConsoleScrollbar->GetScrollArea().height - (indexToUse + newLineIndex) * 14;
				pNewLabel->SetLocation(xPos, yPos);

				m_vpConsoleLabels_Add.push_back(pNewLabel);

				// Skip over the new line, else we will detect it on the next loop
				startLineIndex = previousSpaceIndex + 1;
				newLineIndex++;
			}

			previousSpaceIndex = charIndex;
		}

		// Check for the end of the string
		if (pNextChar == 0)
		{
			std::string str(chatString.substr(startLineIndex, charIndex + 1 - startLineIndex));
			
			Label* pNewLabel = new Label(m_pRenderer, m_defaultFont, str.c_str(), Color(1.0f, 1.0f, 1.0f));
			int xPos = m_pConsoleScrollbar->GetScrollArea().x;
			int yPos = m_pConsoleScrollbar->GetScrollArea().y + m_pConsoleScrollbar->GetScrollArea().height - (indexToUse + newLineIndex) * 14;
			pNewLabel->SetLocation(xPos, yPos);

			m_vpConsoleLabels_Add.push_back(pNewLabel);
		}

		charIndex++;
	}
}

void CubbyGame::ClearConsoleLabels()
{
	m_pConsoleScrollbar->ClearScrollAreaItems();

	for (size_t i = 0; i < m_vpConsoleLabels.size(); ++i)
	{
		delete m_vpConsoleLabels[i];
		m_vpConsoleLabels[i] = nullptr;
	}

	m_vpConsoleLabels.clear();
}

void CubbyGame::UpdateConsoleLabels()
{
	if (m_GUICreated == false)
	{
		return;
	}

	for (size_t i = 0; i < m_vStringCache.size(); ++i)
	{
		AddConsoleLabel(m_vStringCache[i]);
	}
	
	m_vStringCache.clear();
	
	// Add to GUI
	for (size_t i = 0; i < m_vpConsoleLabels_Add.size(); ++i)
	{
		m_vpConsoleLabels.push_back(m_vpConsoleLabels_Add[i]);
		m_pConsoleScrollbar->AddScrollAreaItem(m_vpConsoleLabels_Add[i]);
		m_pConsoleScrollbar->SetScrollPosition(0.0f);
	}
	m_vpConsoleLabels_Add.clear();

	// Update scrollbar and scroll items
	int visibleSize = m_pConsoleScrollbar->GetScrollArea().height;
	int neededHeight = static_cast<int>(m_vpConsoleLabels.size()) * 14 + 5;
	
	int heightDiff = neededHeight - visibleSize;
	if (heightDiff > 0)
	{
		m_pConsoleScrollbar->SetScrollSize(static_cast<float>(visibleSize) / static_cast<float>(neededHeight));
		m_pConsoleScrollbar->SetDisabled(false);

		float scrollRatio = m_pConsoleScrollbar->GetScrollRatio();
		int offsetButtonY = static_cast<int>(heightDiff * scrollRatio);

		for (size_t i = 0; i < m_vpConsoleLabels.size(); ++i)
		{
			int xPos = m_pConsoleScrollbar->GetScrollArea().x;
			int yPos = visibleSize - 14 - (i * 14);

			m_vpConsoleLabels[i]->SetLocation(xPos, yPos + offsetButtonY);
		}
	}
	else
	{
		m_pConsoleScrollbar->SetScrollSize(0.0f);
		m_pConsoleScrollbar->SetDisabled(true);
	}
}

void CubbyGame::ToggleFullScreenPressed()
{
	m_fullscreen = !m_fullscreen;

	m_pCubbyWindow->ToggleFullScreen(m_fullscreen);
	m_pBlockParticleManager->SetupGLBuffers();
}

// GUI callbacks
void CubbyGame::_PlayAnimationPressed(void* pData)
{
	CubbyGame* pCubbyGame = static_cast<CubbyGame*>(pData);
	pCubbyGame->PlayAnimationPressed();
}

void CubbyGame::PlayAnimationPressed() const
{
	AnimationPullDownChanged();
}

void CubbyGame::_AnimationPullDownChanged(void* pData)
{
	CubbyGame* pCubbyGame = static_cast<CubbyGame*>(pData);
	pCubbyGame->AnimationPullDownChanged();
}

void CubbyGame::AnimationPullDownChanged() const
{
	MenuItem* pMenuItem = m_pAnimationsPulldown->GetSelectedMenuItem();
	
	if (pMenuItem != nullptr)
	{
		m_pPlayer->GetVoxelCharacter()->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, pMenuItem->GetLabel().GetText().c_str());
	}
}

void CubbyGame::_CharacterPullDownChanged(void* pData)
{
	CubbyGame* pCubbyGame = static_cast<CubbyGame*>(pData);
	pCubbyGame->CharacterPullDownChanged();
}

void CubbyGame::CharacterPullDownChanged() const
{
	MenuItem* pMenuItem = m_pCharacterPulldown->GetSelectedMenuItem();
	
	if (pMenuItem != nullptr)
	{
		m_pPlayer->UnloadWeapon(false);
		m_pPlayer->UnloadWeapon(true);

		m_pPlayer->LoadCharacter(pMenuItem->GetLabel().GetText().c_str(), false);

		AnimationPullDownChanged();
	}
}

void CubbyGame::_GameModeChanged(void* pData)
{
	CubbyGame* pCubbyGame = static_cast<CubbyGame*>(pData);
	pCubbyGame->GameModeChanged();
}

void CubbyGame::GameModeChanged()
{
	GameMode gameMode = GetGameMode();

	if (m_pGameOptionBox->GetToggled() && gameMode != GameMode::Game)
	{
		SetGameMode(GameMode::Game);

		m_pMouseRotateCameraOptionBox->SetDisabled(false);
		m_pAutoCameraOptionBox->SetDisabled(false);

		// Allow switching to front-end mode
		m_allowToChangeToFrontend = false;
		m_allowToChangeToGame = true;
		m_pGameOptionBox->SetDisabled(false);
		m_pFrontEndOptionBox->SetDisabled(false);
	}
	else if (m_pFrontEndOptionBox->GetToggled() && gameMode != GameMode::FrontEnd)
	{
		SetGameMode(GameMode::FrontEnd);

		// Set front-end page to intro
		m_pFrontendManager->SetFrontendScreen(FrontendScreen::MainMenu);

		m_pMouseRotateCameraOptionBox->SetDisabled(true);
		m_pAutoCameraOptionBox->SetDisabled(true);

		// Allow switching to game mode
		m_allowToChangeToFrontend = true;
		m_allowToChangeToGame = false;
		m_pGameOptionBox->SetDisabled(false);
		m_pFrontEndOptionBox->SetDisabled(false);
	}
	else if (m_pDebugOptionBox->GetToggled() && gameMode != GameMode::Debug)
	{
		SetGameMode(GameMode::Debug);

		m_cameraMode = CameraMode::Debug;
		m_pDebugCameraOptionBox->SetToggled(true);

		m_pMouseRotateCameraOptionBox->SetDisabled(true);
		m_pAutoCameraOptionBox->SetDisabled(true);
		m_pFrontendCameraOptionBox->SetDisabled(true);

		// Decide if we came into debug mode from either game or front-end and then only allow to go back to that previous mode
		// This prevents us getting into a weird state were we go something like: game -> debug -> frontend.
		m_pGameOptionBox->SetDisabled(true);
		if (m_allowToChangeToGame == true)
		{
			m_pGameOptionBox->SetDisabled(false);
		}
		m_pFrontEndOptionBox->SetDisabled(true);
		if (m_allowToChangeToFrontend == true)
		{
			m_pFrontEndOptionBox->SetDisabled(false);
		}
	}
}

void CubbyGame::_CameraModeChanged(void* pData)
{
	CubbyGame* pCubbyGame = static_cast<CubbyGame*>(pData);
	pCubbyGame->CameraModeChanged();
}

void CubbyGame::CameraModeChanged()
{
	if (m_pDebugCameraOptionBox->GetToggled())
	{
		SetCameraMode(CameraMode::Debug);

		TurnCursorOn(true, false);
	}
	else if (m_pMouseRotateCameraOptionBox->GetToggled())
	{
		SetCameraMode(CameraMode::MouseRotate);
		InitializeCameraRotation();

		TurnCursorOff(false);
	}
	else if (m_pAutoCameraOptionBox->GetToggled())
	{
		SetCameraMode(CameraMode::AutoCamera);
		InitializeCameraRotation();

		TurnCursorOff(false);
	}
	else if (m_pFrontendCameraOptionBox->GetToggled())
	{
		SetCameraMode(CameraMode::Frontend);

		TurnCursorOn(true, false);
	}
}

void CubbyGame::_FaceMergeCheckboxChanged(void* pData)
{
	CubbyGame* pCubbyGame = static_cast<CubbyGame*>(pData);
	pCubbyGame->FaceMergeCheckboxChanged();
}

void CubbyGame::FaceMergeCheckboxChanged() const
{
	bool faceMerging = m_pFaceMergingCheckbox->GetToggled();

	m_pPlayer->RebuildVoxelCharacter(faceMerging);
}

void CubbyGame::_StepUpdatePressed(void* pData)
{
	CubbyGame* pCubbyGame = static_cast<CubbyGame*>(pData);
	pCubbyGame->StepUpdatePressed();
}

void CubbyGame::StepUpdatePressed() const
{
	m_pChunkManager->StepNextUpdate();
}

void CubbyGame::_ConsoleReturnPressed(void* pData)
{
	CubbyGame* pCubbyGame = static_cast<CubbyGame*>(pData);
	pCubbyGame->ConsoleReturnPressed();
}

void CubbyGame::ConsoleReturnPressed()
{
	if (m_pConsoleTextbox->GetText() == "")
	{
		return;
	}

	char chatChar[8192];
	sprintf(chatChar, "%s", m_pConsoleTextbox->GetText().c_str());
	std::string chatMessage = chatChar;

	AddConsoleLabel(chatMessage);

	m_pConsoleTextbox->SetText("");
}