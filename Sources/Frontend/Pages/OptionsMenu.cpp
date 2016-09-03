/*************************************************************************
> File Name: OptionsMenu.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A front-end page that displays the options menu for the game.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <CubbyGame.h>

#include <Maths/3DMaths.h>

#include "../FrontendManager.h"

#include "OptionsMenu.h"

// Constructor, Destructor
OptionsMenu::OptionsMenu(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontPageManager, int windowWidth, int windowHeight) :
	FrontendPage(pRenderer, pGUI, pFrontPageManager, FrontendScreen::OptionsMenu, windowWidth, windowHeight)
{
	// Options Window
	m_pOptionsWindow = new GUIWindow(m_pRenderer, m_pFrontendManager->GetFrontendFontMedium(), "Options");
	m_pOptionsWindow->AllowMoving(true);
	m_pOptionsWindow->AllowClosing(false);
	m_pOptionsWindow->AllowMinimizing(false);
	m_pOptionsWindow->AllowScrolling(false);
	m_pOptionsWindow->SetRenderTitleBar(true);
	m_pOptionsWindow->SetRenderWindowBackground(true);
	m_pOptionsWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pOptionsWindow->Hide();

	m_pTitleBarIcon = new Icon(m_pRenderer, "", 44, 44);
	m_pTitleBarIcon->SetDepth(4.0f);

	m_pOptionsWindowBackgroundIcon = new Icon(m_pRenderer, "", 600, 475);
	m_pOptionsWindowBackgroundIcon->SetDepth(1.0f);

	m_pTitleBarBackgroundIcon = new Icon(m_pRenderer, "", 86, 35);
	m_pTitleBarBackgroundIcon->SetDepth(1.0f);

	m_pCloseExitButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCloseExitButton->SetLabelOffset(0, 5);
	m_pCloseExitButton->SetCallBackFunction(_CloseExitPressed);
	m_pCloseExitButton->SetCallBackData(this);

	m_pOptionsWindow->SetBackgroundIcon(m_pOptionsWindowBackgroundIcon);
	m_pOptionsWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	m_pOptionsWindow->AddComponent(m_pTitleBarIcon);
	m_pOptionsWindow->AddComponent(m_pCloseExitButton);

	// Tabs
	m_pOptionsModeController = new OptionController(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), "Mode");
	m_pOptionsModeController->SetDisplayLabel(false);
	m_pOptionsModeController->SetDisplayBorder(false);

	m_pGameplayMode = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), "Gameplay", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pGameplayMode->SetCallBackFunction(_GameplayTabPressed);
	m_pGameplayMode->SetCallBackData(this);
	m_pGameplayMode->SetDepth(2.0f);
	m_pGameplayMode->SetPressedOffset(0, -2);

	m_pGraphicsMode = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), "Graphics", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pGraphicsMode->SetCallBackFunction(_GraphicsTabPressed);
	m_pGraphicsMode->SetCallBackData(this);
	m_pGraphicsMode->SetDepth(2.1f);
	m_pGraphicsMode->SetPressedOffset(0, -2);

	m_pSoundMode = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), "Sound", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pSoundMode->SetCallBackFunction(_SoundTabPressed);
	m_pSoundMode->SetCallBackData(this);
	m_pSoundMode->SetDepth(2.2f);
	m_pSoundMode->SetPressedOffset(0, -2);

	m_pControlsMode = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), "Controls", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pControlsMode->SetCallBackFunction(_ControlsTabPressed);
	m_pControlsMode->SetCallBackData(this);
	m_pControlsMode->SetDepth(2.3f);
	m_pControlsMode->SetPressedOffset(0, -2);

	m_pInterfaceMode = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), m_pFrontendManager->GetFrontendFont18Outline(), "Interface", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pInterfaceMode->SetCallBackFunction(_InterfaceTabPressed);
	m_pInterfaceMode->SetCallBackData(this);
	m_pInterfaceMode->SetDepth(2.4f);
	m_pInterfaceMode->SetPressedOffset(0, -2);

	m_pOptionsModeController->Add(m_pGameplayMode);
	m_pOptionsModeController->Add(m_pGraphicsMode);
	m_pOptionsModeController->Add(m_pSoundMode);
	m_pOptionsModeController->Add(m_pControlsMode);
	m_pOptionsModeController->Add(m_pInterfaceMode);
	m_pGameplayMode->SetToggled(true);

	m_pOptionsWindow->AddComponent(m_pOptionsModeController);

	// Gameplay
	m_pInvertedMouseMode = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Inverted Mouse", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pInvertedMouseMode->SetDisplayLabel(true);
	m_pInvertedMouseMode->SetDepth(2.0f);
	m_pInvertedMouseMode->SetPressedOffset(0, -2);

	m_pNormalCamOption = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Normal Cam", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pNormalCamOption->SetCallBackFunction(_SwitchToNormalCam);
	//m_pNormalCamOption->SetCallBackData(this);
	m_pNormalCamOption->SetDisplayLabel(true);
	m_pNormalCamOption->SetPressedOffset(0, -2);
	m_pFollowCamOption = new OptionBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Follow Cam", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pFollowCamOption->SetCallBackFunction(_SwitchToFollowCam);
	//m_pFollowCamOption->SetCallBackData(this);
	m_pFollowCamOption->SetDisplayLabel(true);
	m_pFollowCamOption->SetPressedOffset(0, -2);
	m_pCameraSelector = new OptionController(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25Outline(), "Camera Mode", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCameraSelector->SetDisplayLabel(true);
	m_pCameraSelector->SetDisplayBorder(true);
	m_pCameraSelector->SetDepth(2.0f);
	m_pCameraSelector->Add(m_pNormalCamOption);
	m_pCameraSelector->Add(m_pFollowCamOption);
	m_pNormalCamOption->SetToggled(true);

	m_pMouseSensativitySlider = new Slider(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Mouse Sensitivity", 5.0f, 100.0f, 45.0f, Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pMouseSensativitySlider->SetSliderParams(5, 10, SliderDirection::Horizontal);
	m_pMouseSensativitySlider->SetLabelPositionOffset(0, 14);
	m_pMouseSensativitySlider->SetValuePrecision(0);
	m_pMouseSensativitySlider->SetRenderBackgroundBar(false);
	m_pMouseSensativitySlider->SetDepth(2.0f);

	m_pGamepadSensativitySlider = new Slider(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Gamepad Sensitivity", 5.0f, 100.0f, 20.0f, Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pGamepadSensativitySlider->SetSliderParams(5, 10, SliderDirection::Horizontal);
	m_pGamepadSensativitySlider->SetLabelPositionOffset(0, 14);
	m_pGamepadSensativitySlider->SetValuePrecision(0);
	m_pGamepadSensativitySlider->SetRenderBackgroundBar(false);
	m_pGamepadSensativitySlider->SetDepth(2.0f);

	m_pSliderBackgroundIcon_MouseSensativity = new Icon(m_pRenderer, "", 207, 31);
	m_pSliderBackgroundIcon_MouseSensativity->SetDepth(1.5f);
	m_pSliderBackgroundIcon_GamepadSensativity = new Icon(m_pRenderer, "", 207, 31);
	m_pSliderBackgroundIcon_GamepadSensativity->SetDepth(1.5f);

	m_vpGameplayComponents.push_back(m_pInvertedMouseMode);
	m_vpGameplayComponents.push_back(m_pCameraSelector);
	m_vpGameplayComponents.push_back(m_pMouseSensativitySlider);
	m_vpGameplayComponents.push_back(m_pGamepadSensativitySlider);
	m_vpGameplayComponents.push_back(m_pSliderBackgroundIcon_MouseSensativity);
	m_vpGameplayComponents.push_back(m_pSliderBackgroundIcon_GamepadSensativity);

	// Graphics
	m_pShadowsCheckBox = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Shadows", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pShadowsCheckBox->SetDisplayLabel(true);
	m_pShadowsCheckBox->SetDepth(2.0f);
	m_pShadowsCheckBox->SetPressedOffset(0, -2);
	m_pFogRenderCheckBox = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Fog Rendering", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pFogRenderCheckBox->SetDisplayLabel(true);
	m_pFogRenderCheckBox->SetDepth(2.0f);
	m_pFogRenderCheckBox->SetPressedOffset(0, -2);
	m_pWaterRenderCheckBox = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Warter Rendering", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pWaterRenderCheckBox->SetDisplayLabel(true);
	m_pWaterRenderCheckBox->SetDepth(2.0f);
	m_pWaterRenderCheckBox->SetPressedOffset(0, -2);

	m_pFullscreenButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25Outline(), "Fullscreen", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pFullscreenButton->SetCallBackFunction(_ToggleFullScreenPressed);
	m_pFullscreenButton->SetCallBackData(this);
	m_pFullscreenButton->SetPressedOffset(0, -2);

	m_vpGraphicsComponents.push_back(m_pShadowsCheckBox);
	m_vpGraphicsComponents.push_back(m_pFogRenderCheckBox);
	m_vpGraphicsComponents.push_back(m_pWaterRenderCheckBox);
	m_vpGraphicsComponents.push_back(m_pFullscreenButton);

	// Sound
	m_pSoundEffects = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Sound", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pSoundEffects->SetCallBackFunction(_SoundEffectsPressed);
	//m_pSoundEffects->SetCallBackData(this);
	m_pSoundEffects->SetDisplayLabel(true);
	m_pSoundEffects->SetDepth(2.0f);
	m_pSoundEffects->SetPressedOffset(0, -2);

	m_pSoundEffectsVolume = new Slider(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "", 0.0f, 1.0f, 0.5f, Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pSoundEffectsVolume->SetSliderParams(5, 10, SliderDirection::Horizontal);
	//m_pSoundEffectsVolume->SetVaueChangedCallBackFunction(_SoundEffectsChanged);
	//m_pSoundEffectsVolume->SetVaueChangedCallBackData(this);
	m_pSoundEffectsVolume->SetRenderBackgroundBar(false);
	m_pSoundEffectsVolume->SetDepth(2.0f);

	m_pMusic = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Music", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pMusic->SetCallBackFunction(_MusicPressed);
	//m_pMusic->SetCallBackData(this);
	m_pMusic->SetDisplayLabel(true);
	m_pMusic->SetDepth(2.0f);
	m_pMusic->SetPressedOffset(0, -2);

	m_pMusicVolume = new Slider(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "", 0.0f, 1.0f, 0.5f, Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pMusicVolume->SetSliderParams(5, 10, SliderDirection::Horizontal);
	//m_pMusicVolume->SetVaueChangedCallBackFunction(_MusicVolumeChanged);
	//m_pMusicVolume->SetVaueChangedCallBackData(this);
	m_pMusicVolume->SetRenderBackgroundBar(false);
	m_pMusicVolume->SetDepth(2.0f);

	m_pSliderBackgroundIcon_Sound = new Icon(m_pRenderer, "", 207, 31);
	m_pSliderBackgroundIcon_Sound->SetDepth(1.5f);
	m_pSliderBackgroundIcon_Music = new Icon(m_pRenderer, "", 207, 31);
	m_pSliderBackgroundIcon_Music->SetDepth(1.5f);

	m_vpSoundComponents.push_back(m_pSoundEffects);
	m_vpSoundComponents.push_back(m_pSoundEffectsVolume);
	m_vpSoundComponents.push_back(m_pMusic);
	m_vpSoundComponents.push_back(m_pMusicVolume);
	m_vpSoundComponents.push_back(m_pSliderBackgroundIcon_Sound);
	m_vpSoundComponents.push_back(m_pSliderBackgroundIcon_Music);

	// Interface
	m_pRenderGUI = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Render HUD", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pRenderGUI->SetCallBackFunction(_RenderHUDTogglePressed);
	m_pRenderGUI->SetCallBackData(this);
	m_pRenderGUI->SetDisplayLabel(true);
	m_pRenderGUI->SetDepth(2.0f);
	m_pRenderGUI->SetPressedOffset(0, -2);

	m_pRenderCrosshair = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Render Crosshair", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pRenderCrosshair->SetCallBackFunction(_RenderCrosshairPressed);
	//m_pRenderCrosshair->SetCallBackData(this);
	m_pRenderCrosshair->SetDisplayLabel(true);
	m_pRenderCrosshair->SetDepth(2.0f);
	m_pRenderCrosshair->SetPressedOffset(0, -2);

	m_pHUDAnimations = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "HUD Animations", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pHUDAnimations->SetCallBackFunction(_HUDAnimationsPressed);
	//m_pHUDAnimations->SetCallBackData(this);
	m_pHUDAnimations->SetDisplayLabel(true);
	m_pHUDAnimations->SetDepth(2.0f);
	m_pHUDAnimations->SetPressedOffset(0, -2);

	m_pCustomCursors = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Custom Cursors", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCustomCursors->SetCallBackFunction(_CustomCursorsTogglePressed);
	m_pCustomCursors->SetCallBackData(this);
	m_pCustomCursors->SetDisplayLabel(true);
	m_pCustomCursors->SetDepth(2.0f);
	m_pCustomCursors->SetPressedOffset(0, -2);

	m_pEquipHoverIcon = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Equip Helper Cursor", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pEquipHoverIcon->SetCallBackFunction(_HUDAnimationsPressed);
	//m_pEquipHoverIcon->SetCallBackData(this);
	m_pEquipHoverIcon->SetDisplayLabel(true);
	m_pEquipHoverIcon->SetDepth(2.0f);
	m_pEquipHoverIcon->SetPressedOffset(0, -2);

	m_pToolTipAppearDelay = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Tooltip Delay", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pToolTipAppearDelay->SetCallBackFunction(_HUDAnimationsPressed);
	//m_pToolTipAppearDelay->SetCallBackData(this);
	m_pToolTipAppearDelay->SetDisplayLabel(true);
	m_pToolTipAppearDelay->SetDepth(2.0f);
	m_pToolTipAppearDelay->SetPressedOffset(0, -2);

	m_pConfirmDeleteItem = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "Confirm Item Deletion", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pConfirmDeleteItem->SetCallBackFunction(_HUDAnimationsPressed);
	//m_pConfirmDeleteItem->SetCallBackData(this);
	m_pConfirmDeleteItem->SetDisplayLabel(true);
	m_pConfirmDeleteItem->SetDepth(2.0f);
	m_pConfirmDeleteItem->SetPressedOffset(0, -2);

	m_pResetGUIToDefault = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont25(), m_pFrontendManager->GetFrontendFont25Outline(), "Reset GUI", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pResetGUIToDefault->SetLabelOffset(0, 3);
	//m_pResetGUIToDefault->SetCallBackFunction(_ResetGUIPressed);
	//m_pResetGUIToDefault->SetCallBackData(this);
	m_pResetGUIToDefault->SetPressedOffset(0, -2);

	m_vpInterfaceComponents.push_back(m_pRenderGUI);
	m_vpInterfaceComponents.push_back(m_pRenderCrosshair);
	m_vpInterfaceComponents.push_back(m_pHUDAnimations);
	m_vpInterfaceComponents.push_back(m_pCustomCursors);
	m_vpInterfaceComponents.push_back(m_pEquipHoverIcon);
	m_vpInterfaceComponents.push_back(m_pToolTipAppearDelay);
	m_vpInterfaceComponents.push_back(m_pConfirmDeleteItem);
	m_vpInterfaceComponents.push_back(m_pResetGUIToDefault);

	// Controls
	OptionsMenu::SetWindowDimensions(m_windowWidth, m_windowHeight);

	m_initiallyUsingCustomCursors = false;

	m_returnToMainMenu = false;
}

OptionsMenu::~OptionsMenu()
{
	// Window
	delete m_pOptionsWindow;
	delete m_pTitleBarIcon;
	delete m_pTitleBarBackgroundIcon;
	delete m_pOptionsWindowBackgroundIcon;
	delete m_pCloseExitButton;

	// Tabs
	delete m_pOptionsModeController;
	delete m_pGameplayMode;
	delete m_pGraphicsMode;
	delete m_pSoundMode;
	delete m_pInterfaceMode;
	delete m_pControlsMode;

	// Gameplay
	delete m_pInvertedMouseMode;
	delete m_pNormalCamOption;
	delete m_pFollowCamOption;
	delete m_pCameraSelector;
	delete m_pMouseSensativitySlider;
	delete m_pGamepadSensativitySlider;
	delete m_pSliderBackgroundIcon_MouseSensativity;
	delete m_pSliderBackgroundIcon_GamepadSensativity;

	// Graphics
	delete m_pShadowsCheckBox;
	delete m_pFogRenderCheckBox;
	delete m_pWaterRenderCheckBox;
	delete m_pFullscreenButton;

	// Sound
	delete m_pSoundEffects;
	delete m_pSoundEffectsVolume;
	delete m_pMusic;
	delete m_pMusicVolume;
	delete m_pSliderBackgroundIcon_Sound;
	delete m_pSliderBackgroundIcon_Music;

	// Interface
	delete m_pRenderGUI;
	delete m_pRenderCrosshair;
	delete m_pHUDAnimations;
	delete m_pCustomCursors;
	delete m_pEquipHoverIcon;
	delete m_pToolTipAppearDelay;
	delete m_pConfirmDeleteItem;
	delete m_pResetGUIToDefault;
}

void OptionsMenu::Reset()
{
	// Do nothing
}

void OptionsMenu::SetWindowDimensions(int windowWidth, int windowHeight)
{
	FrontendPage::SetWindowDimensions(windowWidth, windowHeight);

	m_optionsWindowWidth = 600;
	m_optionsWindowHeight = 475;
	m_titlebarWidth = 135;
	m_titlebarHeight = 35;

	// Window
	m_pOptionsWindow->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - (m_optionsWindowWidth * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) - (m_optionsWindowHeight * 0.5f)), m_optionsWindowWidth, m_optionsWindowHeight);
	m_pOptionsWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);
	m_pOptionsWindow->SetTitleOffset(50, 5);
	m_pOptionsWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pOptionsWindow->SetApplicationBorder(25, 15, 10, 10);

	m_pTitleBarIcon->SetDimensions(0, m_optionsWindowHeight, 44, 44);
	m_pCloseExitButton->SetDimensions(m_optionsWindowWidth - 32, m_optionsWindowHeight, 32, 32);

	// Tabs
	m_pOptionsModeController->SetDimensions(0, m_optionsWindowHeight, m_optionsWindowWidth, 32);
	m_pGameplayMode->SetDisplayLabel(false);
	m_pGameplayMode->SetDimensions(m_optionsWindowWidth - 32 - 375, 0, 75, 32);
	m_pGameplayMode->SetDisplayLabel(true);
	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pGameplayMode->GetLabel().GetText().c_str());
	m_pGameplayMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pGraphicsMode->SetDisplayLabel(false);
	m_pGraphicsMode->SetDimensions(m_optionsWindowWidth - 32 - 300, 0, 75, 32);
	m_pGraphicsMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pGraphicsMode->GetLabel().GetText().c_str());
	m_pGraphicsMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pSoundMode->SetDisplayLabel(false);
	m_pSoundMode->SetDimensions(m_optionsWindowWidth - 32 - 225, 0, 75, 32);
	m_pSoundMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pSoundMode->GetLabel().GetText().c_str());
	m_pSoundMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pInterfaceMode->SetDisplayLabel(false);
	m_pInterfaceMode->SetDimensions(m_optionsWindowWidth - 32 - 150, 0, 75, 32);
	m_pInterfaceMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pInterfaceMode->GetLabel().GetText().c_str());
	m_pInterfaceMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pControlsMode->SetDisplayLabel(false);
	m_pControlsMode->SetDimensions(m_optionsWindowWidth - 32 - 75, 0, 75, 32);
	m_pControlsMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pControlsMode->GetLabel().GetText().c_str());
	m_pControlsMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	// Gameplay
	m_pInvertedMouseMode->SetDimensions(25, m_optionsWindowHeight - 65, 20, 20);

	m_pNormalCamOption->SetDimensions(5, 43, 20, 20);
	m_pFollowCamOption->SetDimensions(5, 7, 20, 20);
	m_pCameraSelector->SetDimensions(25, m_optionsWindowHeight - 190, 140, 80);

	m_pMouseSensativitySlider->SetDimensions(135, m_optionsWindowHeight - 265, 171, 24);
	m_pMouseSensativitySlider->SetCurrentValue(m_pMouseSensativitySlider->GetCurrentValue(), true);
	m_pMouseSensativitySlider->SetLabelPositionOffset(0, 30);

	m_pGamepadSensativitySlider->SetDimensions(135, m_optionsWindowHeight - 300, 171, 24);
	m_pGamepadSensativitySlider->SetCurrentValue(m_pGamepadSensativitySlider->GetCurrentValue(), true);
	m_pGamepadSensativitySlider->SetLabelPositionOffset(0, 30);

	m_pSliderBackgroundIcon_MouseSensativity->SetDimensions(131, m_optionsWindowHeight - 268, 207, 31);
	m_pSliderBackgroundIcon_GamepadSensativity->SetDimensions(131, m_optionsWindowHeight - 303, 207, 31);

	// Graphics
	m_pShadowsCheckBox->SetDimensions(25, m_optionsWindowHeight - 65, 20, 20);
	m_pFogRenderCheckBox->SetDimensions(25, m_optionsWindowHeight - 90, 20, 20);
	m_pWaterRenderCheckBox->SetDimensions(25, m_optionsWindowHeight - 115, 20, 20);
	m_pFullscreenButton->SetDimensions(230, 10, 110, 47);

	// Sound
	m_pSoundEffects->SetDimensions(25, m_optionsWindowHeight - 65, 20, 20);
	m_pSoundEffectsVolume->SetDimensions(115, m_optionsWindowHeight - 65, 171, 24);
	m_pSoundEffectsVolume->SetCurrentValue(m_pSoundEffectsVolume->GetCurrentValue(), true);

	m_pMusic->SetDimensions(25, m_optionsWindowHeight - 105, 20, 20);
	m_pMusicVolume->SetDimensions(115, m_optionsWindowHeight - 105, 171, 24);
	m_pMusicVolume->SetCurrentValue(m_pMusicVolume->GetCurrentValue(), true);

	m_pSliderBackgroundIcon_Sound->SetDimensions(111, m_optionsWindowHeight - 68, 207, 31);
	m_pSliderBackgroundIcon_Music->SetDimensions(111, m_optionsWindowHeight - 108, 207, 31);

	// Interface
	m_pRenderGUI->SetDimensions(25, m_optionsWindowHeight - 65, 20, 20);
	m_pRenderCrosshair->SetDimensions(25, m_optionsWindowHeight - 105, 20, 20);
	m_pHUDAnimations->SetDimensions(25, m_optionsWindowHeight - 145, 20, 20);
	m_pCustomCursors->SetDimensions(25, m_optionsWindowHeight - 185, 20, 20);
	m_pEquipHoverIcon->SetDimensions(25, m_optionsWindowHeight - 225, 20, 20);
	m_pToolTipAppearDelay->SetDimensions(25, m_optionsWindowHeight - 265, 20, 20);
	m_pConfirmDeleteItem->SetDimensions(25, m_optionsWindowHeight - 305, 20, 20);
	m_pResetGUIToDefault->SetDimensions(25, m_optionsWindowHeight - 375, 110, 47);

	// Controls
}

// Are we returning to the main menu, or are we in the game?
void OptionsMenu::SetReturnToMainMenu(bool mainMenu)
{
	m_returnToMainMenu = mainMenu;
}

// Skinning
void OptionsMenu::SkinGUI()
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/OptionsMenu/slider_background.tga";
	m_pSliderBackgroundIcon_MouseSensativity->SetIcon(iconName);
	m_pSliderBackgroundIcon_GamepadSensativity->SetIcon(iconName);
	m_pSliderBackgroundIcon_Sound->SetIcon(iconName);
	m_pSliderBackgroundIcon_Music->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/OptionsMenu/options_title_icon.tga";
	m_pTitleBarIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/OptionsMenu/options_window_background.tga";
	m_pOptionsWindowBackgroundIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/OptionsMenu/titlebar_background.tga";
	m_pTitleBarBackgroundIcon->SetIcon(iconName);

	m_pOptionsWindow->SetBackgroundIcon(m_pOptionsWindowBackgroundIcon);
	m_pOptionsWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	Point location = m_pOptionsWindow->GetLocation();
	m_pOptionsWindow->SetDimensions(location.x, location.y, m_optionsWindowWidth, m_optionsWindowHeight);
	m_pOptionsWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);

	m_pCloseExitButton->SetDefaultIcon(m_pFrontendManager->GetCloseExitButtonIcon());
	m_pCloseExitButton->SetHoverIcon(m_pFrontendManager->GetCloseExitButtonIconHover());
	m_pCloseExitButton->SetSelectedIcon(m_pFrontendManager->GetCloseExitButtonIconPressed());
	m_pCloseExitButton->SetDisabledIcon(m_pFrontendManager->GetCloseExitButtonIcon());

	m_pFrontendManager->SetTabIcons(m_pGameplayMode);
	m_pFrontendManager->SetTabIcons(m_pGraphicsMode);
	m_pFrontendManager->SetTabIcons(m_pSoundMode);
	m_pFrontendManager->SetTabIcons(m_pControlsMode);
	m_pFrontendManager->SetTabIcons(m_pInterfaceMode);

	m_pGameplayMode->SetDisplayLabel(false);
	m_pGameplayMode->SetDimensions(m_optionsWindowWidth - 32 - 375, 0, 75, 32);
	m_pGameplayMode->SetDisplayLabel(true);
	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pGameplayMode->GetLabel().GetText().c_str());
	m_pGameplayMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pGraphicsMode->SetDisplayLabel(false);
	m_pGraphicsMode->SetDimensions(m_optionsWindowWidth - 32 - 300, 0, 75, 32);
	m_pGraphicsMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pGraphicsMode->GetLabel().GetText().c_str());
	m_pGraphicsMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pSoundMode->SetDisplayLabel(false);
	m_pSoundMode->SetDimensions(m_optionsWindowWidth - 32 - 225, 0, 75, 32);
	m_pSoundMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pSoundMode->GetLabel().GetText().c_str());
	m_pSoundMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pInterfaceMode->SetDisplayLabel(false);
	m_pInterfaceMode->SetDimensions(m_optionsWindowWidth - 32 - 150, 0, 75, 32);
	m_pInterfaceMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pInterfaceMode->GetLabel().GetText().c_str());
	m_pInterfaceMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pControlsMode->SetDisplayLabel(false);
	m_pControlsMode->SetDimensions(m_optionsWindowWidth - 32 - 75, 0, 75, 32);
	m_pControlsMode->SetDisplayLabel(true);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18Outline(), "%s", m_pControlsMode->GetLabel().GetText().c_str());
	m_pControlsMode->SetLabelPosition(static_cast<int>(75 * 0.5f - textWidth * 0.5f), 8);

	m_pFrontendManager->SetCheckboxIcons(m_pInvertedMouseMode);
	Dimensions dimensions = m_pInvertedMouseMode->GetDimensions();
	m_pInvertedMouseMode->SetDimensions(dimensions.x, dimensions.y, 20, 20);

	m_pFrontendManager->SetOptionboxIcons(m_pNormalCamOption);
	dimensions = m_pNormalCamOption->GetDimensions();
	m_pNormalCamOption->SetDimensions(dimensions.x, dimensions.y, 20, 20);
	m_pFrontendManager->SetOptionboxIcons(m_pFollowCamOption);
	dimensions = m_pFollowCamOption->GetDimensions();
	m_pFollowCamOption->SetDimensions(dimensions.x, dimensions.y, 20, 20);

	m_pFrontendManager->SetCheckboxIcons(m_pShadowsCheckBox);
	dimensions = m_pShadowsCheckBox->GetDimensions();
	m_pShadowsCheckBox->SetDimensions(dimensions.x, dimensions.y, 20, 20);

	m_pFrontendManager->SetCheckboxIcons(m_pFogRenderCheckBox);
	dimensions = m_pFogRenderCheckBox->GetDimensions();
	m_pFogRenderCheckBox->SetDimensions(dimensions.x, dimensions.y, 20, 20);

	m_pFrontendManager->SetCheckboxIcons(m_pWaterRenderCheckBox);
	dimensions = m_pWaterRenderCheckBox->GetDimensions();
	m_pWaterRenderCheckBox->SetDimensions(dimensions.x, dimensions.y, 20, 20);

	m_pFrontendManager->SetCheckboxIcons(m_pSoundEffects);
	dimensions = m_pSoundEffects->GetDimensions();
	m_pSoundEffects->SetDimensions(dimensions.x, dimensions.y, 20, 20);
	m_pFrontendManager->SetCheckboxIcons(m_pMusic);
	dimensions = m_pMusic->GetDimensions();
	m_pMusic->SetDimensions(dimensions.x, dimensions.y, 20, 20);

	m_pFrontendManager->SetCheckboxIcons(m_pRenderGUI);
	dimensions = m_pRenderGUI->GetDimensions();
	m_pRenderGUI->SetDimensions(dimensions.x, dimensions.y, 20, 20);
	m_pFrontendManager->SetCheckboxIcons(m_pRenderCrosshair);
	dimensions = m_pRenderCrosshair->GetDimensions();
	m_pRenderCrosshair->SetDimensions(dimensions.x, dimensions.y, 20, 20);
	m_pFrontendManager->SetCheckboxIcons(m_pHUDAnimations);
	dimensions = m_pHUDAnimations->GetDimensions();
	m_pHUDAnimations->SetDimensions(dimensions.x, dimensions.y, 20, 20);
	m_pFrontendManager->SetCheckboxIcons(m_pCustomCursors);
	dimensions = m_pCustomCursors->GetDimensions();
	m_pCustomCursors->SetDimensions(dimensions.x, dimensions.y, 20, 20);
	m_pFrontendManager->SetCheckboxIcons(m_pEquipHoverIcon);
	dimensions = m_pEquipHoverIcon->GetDimensions();
	m_pEquipHoverIcon->SetDimensions(dimensions.x, dimensions.y, 20, 20);
	m_pFrontendManager->SetCheckboxIcons(m_pToolTipAppearDelay);
	dimensions = m_pToolTipAppearDelay->GetDimensions();
	m_pToolTipAppearDelay->SetDimensions(dimensions.x, dimensions.y, 20, 20);
	m_pFrontendManager->SetCheckboxIcons(m_pConfirmDeleteItem);
	dimensions = m_pConfirmDeleteItem->GetDimensions();
	m_pConfirmDeleteItem->SetDimensions(dimensions.x, dimensions.y, 20, 20);
	m_pFrontendManager->SetButtonIcons(m_pResetGUIToDefault, ButtonSize::Size110x47);

	m_pFrontendManager->SetSliderIcons(m_pMouseSensativitySlider);
	m_pFrontendManager->SetSliderIcons(m_pGamepadSensativitySlider);
	m_pFrontendManager->SetSliderIcons(m_pSoundEffectsVolume);
	m_pFrontendManager->SetSliderIcons(m_pMusicVolume);

	m_pGameplayMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pGameplayMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pGameplayMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pGameplayMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pGraphicsMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pGraphicsMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pGraphicsMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pGraphicsMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pSoundMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pSoundMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pSoundMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pSoundMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pControlsMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pControlsMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pControlsMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pControlsMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pInterfaceMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pInterfaceMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pInterfaceMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pInterfaceMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pFrontendManager->SetButtonIcons(m_pFullscreenButton, ButtonSize::Size110x47);
	m_pFullscreenButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pFullscreenButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pFullscreenButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pFullscreenButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

	m_pInvertedMouseMode->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pInvertedMouseMode->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pInvertedMouseMode->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pInvertedMouseMode->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pNormalCamOption->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pNormalCamOption->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pNormalCamOption->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pNormalCamOption->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pFollowCamOption->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pFollowCamOption->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pFollowCamOption->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pFollowCamOption->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pShadowsCheckBox->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pShadowsCheckBox->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pShadowsCheckBox->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pShadowsCheckBox->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pFogRenderCheckBox->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pFogRenderCheckBox->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pFogRenderCheckBox->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pFogRenderCheckBox->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pWaterRenderCheckBox->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pWaterRenderCheckBox->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pWaterRenderCheckBox->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pWaterRenderCheckBox->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pSoundEffects->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pSoundEffects->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pSoundEffects->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pSoundEffects->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pMusic->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pMusic->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pMusic->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pMusic->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pRenderGUI->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pRenderGUI->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pRenderGUI->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pRenderGUI->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pRenderCrosshair->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pRenderCrosshair->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pRenderCrosshair->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pRenderCrosshair->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pHUDAnimations->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pHUDAnimations->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pHUDAnimations->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pHUDAnimations->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pCustomCursors->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pCustomCursors->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pCustomCursors->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pCustomCursors->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pEquipHoverIcon->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pEquipHoverIcon->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pEquipHoverIcon->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pEquipHoverIcon->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pToolTipAppearDelay->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pToolTipAppearDelay->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pToolTipAppearDelay->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pToolTipAppearDelay->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pConfirmDeleteItem->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pConfirmDeleteItem->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pConfirmDeleteItem->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pConfirmDeleteItem->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
	m_pResetGUIToDefault->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pResetGUIToDefault->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pResetGUIToDefault->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pResetGUIToDefault->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
}

void OptionsMenu::UnSkinGUI()
{
	// Do nothing
}

// Loading and saving options to file
void OptionsMenu::LoadOptions() const
{
	CubbySettings* pSettings = CubbyGame::GetInstance()->GetCubbySettings();

	// Gameplay
	m_pInvertedMouseMode->SetToggled(pSettings->m_invertedMouse);
	m_pMouseSensativitySlider->SetCurrentValue(pSettings->m_mouseSensitivity);
	m_pGamepadSensativitySlider->SetCurrentValue(pSettings->m_gamepadSensitivity);

	// Graphics
	m_pShadowsCheckBox->SetToggled(pSettings->m_shadows);
	m_pFogRenderCheckBox->SetToggled(pSettings->m_fogRendering);
	m_pWaterRenderCheckBox->SetToggled(pSettings->m_waterRendering);

	// Sound
	m_pSoundEffects->SetToggled(pSettings->m_audio);
	m_pSoundEffectsVolume->SetCurrentValue(pSettings->m_audioVolume);
	m_pMusic->SetToggled(pSettings->m_music);
	m_pMusicVolume->SetCurrentValue(pSettings->m_musicVolume);

	// Interface
	m_pRenderGUI->SetToggled(pSettings->m_renderGUI);
	m_pRenderCrosshair->SetToggled(pSettings->m_renderCrosshair);
	m_pHUDAnimations->SetToggled(pSettings->m_HUDAnimations);
	m_pCustomCursors->SetToggled(pSettings->m_customCursors);
	m_pEquipHoverIcon->SetToggled(pSettings->m_equipHelper);
	m_pToolTipAppearDelay->SetToggled(pSettings->m_tooltipDelay);
	m_pConfirmDeleteItem->SetToggled(pSettings->m_confirmItemDelete);

	// Controls
}

void OptionsMenu::SaveOptions() const
{
	CubbySettings* pSettings = CubbyGame::GetInstance()->GetCubbySettings();

	// Gameplay
	pSettings->m_invertedMouse = m_pInvertedMouseMode->GetToggled();
	pSettings->m_mouseSensitivity = m_pMouseSensativitySlider->GetCurrentValue();
	pSettings->m_gamepadSensitivity = m_pGamepadSensativitySlider->GetCurrentValue();

	// Graphics
	pSettings->m_shadows = m_pShadowsCheckBox->GetToggled();
	pSettings->m_fogRendering = m_pFogRenderCheckBox->GetToggled();
	pSettings->m_waterRendering = m_pWaterRenderCheckBox->GetToggled();

	// Sound
	pSettings->m_audio = m_pSoundEffects->GetToggled();
	pSettings->m_audioVolume = m_pSoundEffectsVolume->GetCurrentValue();
	pSettings->m_music = m_pMusic->GetToggled();
	pSettings->m_musicVolume = m_pMusicVolume->GetCurrentValue();

	// Interface
	pSettings->m_renderGUI = m_pRenderGUI->GetToggled();
	pSettings->m_renderCrosshair = m_pRenderCrosshair->GetToggled();
	pSettings->m_HUDAnimations = m_pHUDAnimations->GetToggled();
	pSettings->m_customCursors = m_pCustomCursors->GetToggled();
	pSettings->m_equipHelper = m_pEquipHoverIcon->GetToggled();
	pSettings->m_tooltipDelay = m_pToolTipAppearDelay->GetToggled();
	pSettings->m_confirmItemDelete = m_pConfirmDeleteItem->GetToggled();

	// Save the options file
	pSettings->SaveOptions();
}

// Load unload
void OptionsMenu::Load()
{
	// Load options to GUI
	LoadOptions();

	// Cache if we are using custom cursors
	m_initiallyUsingCustomCursors = m_pCustomCursors->GetToggled();

	m_pGameplayMode->SetToggled(true);
	GameplayTabPressed();

	m_loaded = true;
}

void OptionsMenu::Unload()
{
	// Save GUI to options
	SaveOptions();

	// Depending on if we have changed the custom cursors option, we need to force the cursor to appear or disappear.
	if (m_initiallyUsingCustomCursors != m_pCustomCursors->GetToggled())
	{
		if (m_pCustomCursors->GetToggled())
		{
			CubbyGame::GetInstance()->TurnCursorOff(true);
			CubbyGame::GetInstance()->TurnCursorOn(false, false);
		}
		else
		{
			CubbyGame::GetInstance()->TurnCursorOn(false, true);
		}
	}

	// Remove ALL tab sections
	for (size_t i = 0; i < m_vpGameplayComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGameplayComponents[i]);
	}
	for (size_t i = 0; i < m_vpGraphicsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGraphicsComponents[i]);
	}
	for (size_t i = 0; i < m_vpSoundComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpSoundComponents[i]);
	}
	for (size_t i = 0; i < m_vpControlsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpControlsComponents[i]);
	}
	for (size_t i = 0; i < m_vpInterfaceComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpInterfaceComponents[i]);
	}

	m_pGUI->RemoveWindow(m_pOptionsWindow);

	m_loaded = false;
}

// Disable options
void OptionsMenu::DisableShadowOption() const
{
	m_pShadowsCheckBox->SetToggled(false);
	m_pShadowsCheckBox->SetDisabled(true);
	CubbyGame::GetInstance()->GetCubbySettings()->m_shadows = false;
}

// Update
void OptionsMenu::Update(float dt)
{
	FrontendPage::Update(dt);

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
		glm::vec3 position = glm::vec3(cos(angle) * radius, radius*0.75f, sin(angle) * radius);

		m_cameraPosition = position;
		m_cameraView = glm::vec3(0.0f, 8.0f, 0.0f);
	}

	CubbyGame::GetInstance()->SetGlobalBlurAmount(0.00125f);

	// Always update the music and sound settings so that we can dynamically change audio settings in the options menu
	CubbyGame::GetInstance()->GetCubbySettings()->m_music = m_pMusic->GetToggled();
	CubbyGame::GetInstance()->GetCubbySettings()->m_musicVolume = m_pMusicVolume->GetCurrentValue();
	CubbyGame::GetInstance()->GetCubbySettings()->m_audio = m_pSoundEffects->GetToggled();
	CubbyGame::GetInstance()->GetCubbySettings()->m_audioVolume = m_pSoundEffectsVolume->GetCurrentValue();

	// Always update these graphical settings as soon as we change the toggles
	CubbyGame::GetInstance()->GetCubbySettings()->m_shadows = m_pShadowsCheckBox->GetToggled();
	CubbyGame::GetInstance()->GetCubbySettings()->m_fogRendering = m_pFogRenderCheckBox->GetToggled();
	CubbyGame::GetInstance()->GetCubbySettings()->m_waterRendering = m_pWaterRenderCheckBox->GetToggled();
}

// Render
void OptionsMenu::Render()
{
	FrontendPage::Render();
}

void OptionsMenu::Render2D()
{
	FrontendPage::Render2D();
}

// Static functionality
void OptionsMenu::_CloseExitPressed(void* pData)
{
	OptionsMenu* pOptionsMenu = static_cast<OptionsMenu*>(pData);
	pOptionsMenu->CloseExitPressed();
}

void OptionsMenu::CloseExitPressed() const
{
	if (m_returnToMainMenu)
	{
		m_pFrontendManager->SetFrontendScreen(FrontendScreen::MainMenu);
	}
	else
	{
		m_pFrontendManager->SetFrontendScreen(FrontendScreen::PauseMenu);
	}
}

void OptionsMenu::_GameplayTabPressed(void* pData)
{
	OptionsMenu* pOptionsMenu = static_cast<OptionsMenu*>(pData);
	pOptionsMenu->GameplayTabPressed();
}

void OptionsMenu::GameplayTabPressed()
{
	if (m_loaded)
	{
		m_pGUI->RemoveWindow(m_pOptionsWindow);
	}

	// Remove
	for (size_t i = 0; i < m_vpGameplayComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGameplayComponents[i]);
	}
	for (size_t i = 0; i < m_vpGraphicsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGraphicsComponents[i]);
	}
	for (size_t i = 0; i < m_vpSoundComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpSoundComponents[i]);
	}
	for (size_t i = 0; i < m_vpControlsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpControlsComponents[i]);
	}
	for (size_t i = 0; i < m_vpInterfaceComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpInterfaceComponents[i]);
	}

	// Add
	for (size_t i = 0; i < m_vpGameplayComponents.size(); ++i)
	{
		m_pOptionsWindow->AddComponent(m_vpGameplayComponents[i]);
	}

	m_pGUI->AddWindow(m_pOptionsWindow);
	m_pOptionsWindow->Show();
}

void OptionsMenu::_GraphicsTabPressed(void* pData)
{
	OptionsMenu* pOptionsMenu = static_cast<OptionsMenu*>(pData);
	pOptionsMenu->GraphicsTabPressed();
}

void OptionsMenu::GraphicsTabPressed()
{
	if (m_loaded)
	{
		m_pGUI->RemoveWindow(m_pOptionsWindow);
	}

	// Remove
	for (size_t i = 0; i < m_vpGameplayComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGameplayComponents[i]);
	}
	for (size_t i = 0; i < m_vpGraphicsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGraphicsComponents[i]);
	}
	for (size_t i = 0; i < m_vpSoundComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpSoundComponents[i]);
	}
	for (size_t i = 0; i < m_vpControlsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpControlsComponents[i]);
	}
	for (size_t i = 0; i < m_vpInterfaceComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpInterfaceComponents[i]);
	}

	// Add
	for (size_t i = 0; i < m_vpGraphicsComponents.size(); ++i)
	{
		m_pOptionsWindow->AddComponent(m_vpGraphicsComponents[i]);
	}

	m_pGUI->AddWindow(m_pOptionsWindow);
	m_pOptionsWindow->Show();
}

void OptionsMenu::_SoundTabPressed(void* pData)
{
	OptionsMenu* pOptionsMenu = static_cast<OptionsMenu*>(pData);
	pOptionsMenu->SoundTabPressed();
}

void OptionsMenu::SoundTabPressed()
{
	if (m_loaded)
	{
		m_pGUI->RemoveWindow(m_pOptionsWindow);
	}

	// Remove
	for (size_t i = 0; i < m_vpGameplayComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGameplayComponents[i]);
	}
	for (size_t i = 0; i < m_vpGraphicsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGraphicsComponents[i]);
	}
	for (size_t i = 0; i < m_vpSoundComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpSoundComponents[i]);
	}
	for (size_t i = 0; i < m_vpControlsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpControlsComponents[i]);
	}
	for (size_t i = 0; i < m_vpInterfaceComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpInterfaceComponents[i]);
	}

	// Add
	for (size_t i = 0; i < m_vpSoundComponents.size(); ++i)
	{
		m_pOptionsWindow->AddComponent(m_vpSoundComponents[i]);
	}

	m_pGUI->AddWindow(m_pOptionsWindow);
	m_pOptionsWindow->Show();
}

void OptionsMenu::_InterfaceTabPressed(void* pData)
{
	OptionsMenu* lpOptionsMenu = static_cast<OptionsMenu*>(pData);
	lpOptionsMenu->InterfaceTabPressed();
}

void OptionsMenu::InterfaceTabPressed()
{
	if (m_loaded)
	{
		m_pGUI->RemoveWindow(m_pOptionsWindow);
	}

	// Remove
	for (size_t i = 0; i < m_vpGameplayComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGameplayComponents[i]);
	}
	for (size_t i = 0; i < m_vpGraphicsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGraphicsComponents[i]);
	}
	for (size_t i = 0; i < m_vpSoundComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpSoundComponents[i]);
	}
	for (size_t i = 0; i < m_vpControlsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpControlsComponents[i]);
	}
	for (size_t i = 0; i < m_vpInterfaceComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpInterfaceComponents[i]);
	}

	// Add
	for (size_t i = 0; i < m_vpInterfaceComponents.size(); i++)
	{
		m_pOptionsWindow->AddComponent(m_vpInterfaceComponents[i]);
	}

	m_pGUI->AddWindow(m_pOptionsWindow);
	m_pOptionsWindow->Show();
}

void OptionsMenu::_ControlsTabPressed(void* pData)
{
	OptionsMenu* pOptionsMenu = static_cast<OptionsMenu*>(pData);
	pOptionsMenu->ControlsTabPressed();
}

void OptionsMenu::ControlsTabPressed()
{
	if (m_loaded)
	{
		m_pGUI->RemoveWindow(m_pOptionsWindow);
	}

	// Remove
	for (size_t i = 0; i < m_vpGameplayComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGameplayComponents[i]);
	}
	for (size_t i = 0; i < m_vpGraphicsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpGraphicsComponents[i]);
	}
	for (size_t i = 0; i < m_vpSoundComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpSoundComponents[i]);
	}
	for (size_t i = 0; i < m_vpControlsComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpControlsComponents[i]);
	}
	for (size_t i = 0; i < m_vpInterfaceComponents.size(); ++i)
	{
		m_pOptionsWindow->RemoveComponent(m_vpInterfaceComponents[i]);
	}

	// Add
	for (size_t i = 0; i < m_vpControlsComponents.size(); ++i)
	{
		m_pOptionsWindow->AddComponent(m_vpControlsComponents[i]);
	}

	m_pGUI->AddWindow(m_pOptionsWindow);
	m_pOptionsWindow->Show();
}

void OptionsMenu::_ToggleFullScreenPressed(void* pData)
{
	OptionsMenu* pOptionsMenu = static_cast<OptionsMenu*>(pData);
	pOptionsMenu->ToggleFullScreenPressed();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void OptionsMenu::ToggleFullScreenPressed() const
{
	CubbyGame::GetInstance()->ToggleFullScreenPressed();
}

void OptionsMenu::_CustomCursorsTogglePressed(void* pData)
{
	OptionsMenu* pOptionsMenu = static_cast<OptionsMenu*>(pData);
	pOptionsMenu->CustomCursorsTogglePressed();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void OptionsMenu::CustomCursorsTogglePressed() const
{
	// TODO: Make it so that the custom cursors are turned on/off immediately when toggling the option
}

void OptionsMenu::_RenderHUDTogglePressed(void* pData)
{
	OptionsMenu* pOptionsMenu = static_cast<OptionsMenu*>(pData);
	pOptionsMenu->RenderHUDTogglePressed();
}

void OptionsMenu::RenderHUDTogglePressed() const
{
	if (m_pRenderGUI->GetToggled())
	{
		if (CubbyGame::GetInstance()->GetHUD()->IsLoaded() == false)
		{
			CubbyGame::GetInstance()->GetHUD()->Load();
		}
		if (CubbyGame::GetInstance()->GetActionBar()->IsLoaded() == false)
		{
			CubbyGame::GetInstance()->GetActionBar()->Load();
		}
	}
	else
	{
		if (CubbyGame::GetInstance()->GetHUD()->IsLoaded() == true)
		{
			CubbyGame::GetInstance()->GetHUD()->Unload();
		}
		if (CubbyGame::GetInstance()->GetActionBar()->IsLoaded() == true)
		{
			CubbyGame::GetInstance()->GetActionBar()->Unload();
		}
	}
}