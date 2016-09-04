/*************************************************************************
> File Name: FrontendManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A manager class to contain all the common functionality for front-end pages
>    and GUI graphics.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <inih/INIReader.h>

#include <CubbyGame.h>

#include "Pages/CreateCharacter.h"

#include "FrontEndManager.h"
#include "Pages/MainMenu.h"
#include "Pages/SelectCharacter.h"
#include "Pages/QuitPopup.h"
#include "Pages/PauseMenu.h"
#include "Pages/OptionsMenu.h"
#include "Pages/Credits.h"
#include "Pages/ModMenu.h"

// Constructor, Destructor
FrontendManager::FrontendManager(Renderer* pRenderer, OpenGLGUI* pGUI)
{
	m_pRenderer = pRenderer;
	m_pGUI = pGUI;

	m_pCamera = nullptr;

	// Fonts
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/pf_ronda_seven.ttf", 26, &m_frontendFontLarge, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/pf_ronda_seven.ttf", 18, &m_frontendFontMedium, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/pf_ronda_seven.ttf", 12, &m_frontendFontSmall, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 14, &m_frontendFont14, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 14, &m_frontendFont14Outline, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 18, &m_frontendFont18, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 18, &m_frontendFont18Outline, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 20, &m_frontendFont20, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 20, &m_frontendFont20Outline, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 22, &m_frontendFont22, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 22, &m_frontendFont22Outline, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 25, &m_frontendFont25, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 25, &m_frontendFont25Outline, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 30, &m_frontendFont30, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 30, &m_frontendFont30Outline, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 35, &m_frontendFont35, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 35, &m_frontendFont35Outline, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 40, &m_frontendFont40, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 40, &m_frontendFont40Outline, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 50, &m_frontendFont50, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 50, &m_frontendFont50Outline, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 80, &m_frontendFont80, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 80, &m_frontendFont80Outline, true);

	// Text effects fonts
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolbackot.ttf", 34, &m_textEffectFontNormal, true);
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/screenloggercoolot.ttf", 34, &m_textEffectFontOutline, true);

	// Label colours
	m_normalColor = Color(1.0f, 1.0f, 1.0f);
	m_hoverColor = Color(0.74f, 0.49f, 0.26f);
	m_pressedColor = Color(0.5f, 0.3f, 0.01f);
	m_disabledColor = Color(0.25f, 0.25f, 0.25f);

	// Checkbox
	m_pCheckboxIcon = new Icon(m_pRenderer, "", 16, 16);
	m_pCheckboxIcon->SetDepth(2.0f);
	m_pCheckboxIconHover = new Icon(m_pRenderer, "", 16, 16);
	m_pCheckboxIconHover->SetDepth(2.0f);
	m_pCheckboxIconPressed = new Icon(m_pRenderer, "", 16, 16);
	m_pCheckboxIconPressed->SetDepth(2.0f);
	m_pCheckboxIconDisabled = new Icon(m_pRenderer, "", 16, 16);
	m_pCheckboxIconDisabled->SetDepth(2.0f);
	m_pCheckboxIconToggled = new Icon(m_pRenderer, "", 16, 16);
	m_pCheckboxIconToggled->SetDepth(2.0f);
	m_pCheckboxIconToggledHover = new Icon(m_pRenderer, "", 16, 16);
	m_pCheckboxIconToggledHover->SetDepth(2.0f);
	m_pCheckboxIconToggledPressed = new Icon(m_pRenderer, "", 16, 16);
	m_pCheckboxIconToggledPressed->SetDepth(2.0f);
	m_pCheckboxIconToggledDisabled = new Icon(m_pRenderer, "", 16, 16);
	m_pCheckboxIconToggledDisabled->SetDepth(2.0f);

	// Optionbox
	m_pOptionboxIcon = new Icon(m_pRenderer, "", 16, 16);
	m_pOptionboxIcon->SetDepth(2.0f);
	m_pOptionboxIconHover = new Icon(m_pRenderer, "", 16, 16);
	m_pOptionboxIconHover->SetDepth(2.0f);
	m_pOptionboxIconPressed = new Icon(m_pRenderer, "", 16, 16);
	m_pOptionboxIconPressed->SetDepth(2.0f);
	m_pOptionboxIconDisabled = new Icon(m_pRenderer, "", 16, 16);
	m_pOptionboxIconDisabled->SetDepth(2.0f);
	m_pOptionboxIconToggled = new Icon(m_pRenderer, "", 16, 16);
	m_pOptionboxIconToggled->SetDepth(2.0f);
	m_pOptionboxIconToggledHover = new Icon(m_pRenderer, "", 16, 16);
	m_pOptionboxIconToggledHover->SetDepth(2.0f);
	m_pOptionboxIconToggledPressed = new Icon(m_pRenderer, "", 16, 16);
	m_pOptionboxIconToggledPressed->SetDepth(2.0f);
	m_pOptionboxIconToggledDisabled = new Icon(m_pRenderer, "", 16, 16);
	m_pOptionboxIconToggledDisabled->SetDepth(2.0f);

	// Scrollbar
	m_pScrollbarArrowDownIcon = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarArrowDownIcon->SetDepth(2.0f);
	m_pScrollbarArrowDownIconHover = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarArrowDownIconHover->SetDepth(2.0f);
	m_pScrollbarArrowDownIconPressed = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarArrowDownIconPressed->SetDepth(2.0f);
	m_pScrollbarArrowDownIconDisabled = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarArrowDownIconDisabled->SetDepth(2.0f);
	m_pScrollbarArrowUpIcon = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarArrowUpIcon->SetDepth(2.0f);
	m_pScrollbarArrowUpIconHover = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarArrowUpIconHover->SetDepth(2.0f);
	m_pScrollbarArrowUpIconPressed = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarArrowUpIconPressed->SetDepth(2.0f);
	m_pScrollbarArrowUpIconDisabled = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarArrowUpIconDisabled->SetDepth(2.0f);
	m_pScrollbarBackgroundIcon = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarBackgroundIcon->SetDepth(2.0f);
	m_pScrollbarBackgroundIconDisabled = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarBackgroundIconDisabled->SetDepth(2.0f);
	m_pScrollbarIcon = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarIcon->SetDepth(4.0f);
	m_pScrollbarIconHover = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarIconHover->SetDepth(4.0f);
	m_pScrollbarIconPressed = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarIconPressed->SetDepth(4.0f);
	m_pScrollbarIconDisabled = new Icon(m_pRenderer, "", 18, 18);
	m_pScrollbarIconDisabled->SetDepth(4.0f);

	// Slider
	m_pSliderIconDefault = new Icon(m_pRenderer, "", 12, 24);
	m_pSliderIconDefault->SetDepth(3.0f);
	m_pSliderIconHover = new Icon(m_pRenderer, "", 12, 24);
	m_pSliderIconHover->SetDepth(3.0f);
	m_pSliderIconDrag = new Icon(m_pRenderer, "", 12, 24);
	m_pSliderIconDrag->SetDepth(3.0f);
	m_pSliderIconDefaultBlank = new Icon(m_pRenderer, "", 12, 24);
	m_pSliderIconDefaultBlank->SetDepth(3.0f);
	m_pSliderIconHoverBlank = new Icon(m_pRenderer, "", 12, 24);
	m_pSliderIconHoverBlank->SetDepth(3.0f);
	m_pSliderIconDragBlank = new Icon(m_pRenderer, "", 12, 24);
	m_pSliderIconDragBlank->SetDepth(3.0f);
	m_pSliderIncrementIcon = new Icon(m_pRenderer, "", 12, 12);
	m_pSliderIncrementIcon->SetDepth(3.0f);
	m_pSliderIncrementIconHover = new Icon(m_pRenderer, "", 12, 12);
	m_pSliderIncrementIconHover->SetDepth(3.0f);
	m_pSliderIncrementIconPressed = new Icon(m_pRenderer, "", 12, 12);
	m_pSliderIncrementIconPressed->SetDepth(3.0f);
	m_pSliderIncrementIconDisabled = new Icon(m_pRenderer, "", 12, 12);
	m_pSliderIncrementIconDisabled->SetDepth(3.0f);
	m_pSliderDecrementIcon = new Icon(m_pRenderer, "", 12, 12);
	m_pSliderDecrementIcon->SetDepth(3.0f);
	m_pSliderDecrementIconHover = new Icon(m_pRenderer, "", 12, 12);
	m_pSliderDecrementIconHover->SetDepth(3.0f);
	m_pSliderDecrementIconPressed = new Icon(m_pRenderer, "", 12, 12);
	m_pSliderDecrementIconPressed->SetDepth(3.0f);
	m_pSliderDecrementIconDisabled = new Icon(m_pRenderer, "", 12, 12);
	m_pSliderDecrementIconDisabled->SetDepth(3.0f);

	// Buttons
	m_pButtonIcon = new Icon*[static_cast<int>(ButtonSize::NUM)];
	m_pButtonIconHover = new Icon*[static_cast<int>(ButtonSize::NUM)];
	m_pButtonIconPressed = new Icon*[static_cast<int>(ButtonSize::NUM)];
	m_pButtonIconDisabled = new Icon*[static_cast<int>(ButtonSize::NUM)];
	for (int i = 0; i < static_cast<int>(ButtonSize::NUM); ++i)
	{
		int width = 0;
		int height = 0;
		if (ButtonSize(i) == ButtonSize::Size85x25) { width = 85; height = 25; }
		if (ButtonSize(i) == ButtonSize::Size65x25) { width = 65; height = 25; }
		if (ButtonSize(i) == ButtonSize::Size110x47) { width = 110; height = 47; }
		if (ButtonSize(i) == ButtonSize::Size225x75) { width = 225; height = 75; }

		m_pButtonIcon[static_cast<int>(ButtonSize(i))] = new Icon(m_pRenderer, "", width, height);
		m_pButtonIcon[static_cast<int>(ButtonSize(i))]->SetDepth(2.0f);
		m_pButtonIconHover[static_cast<int>(ButtonSize(i))] = new Icon(m_pRenderer, "", width, height);
		m_pButtonIconHover[static_cast<int>(ButtonSize(i))]->SetDepth(2.0f);
		m_pButtonIconPressed[static_cast<int>(ButtonSize(i))] = new Icon(m_pRenderer, "", width, height);
		m_pButtonIconPressed[static_cast<int>(ButtonSize(i))]->SetDepth(2.0f);
		m_pButtonIconDisabled[static_cast<int>(ButtonSize(i))] = new Icon(m_pRenderer, "", width, height);
		m_pButtonIconDisabled[static_cast<int>(ButtonSize(i))]->SetDepth(2.0f);
	}

	// Close button
	m_pCloseExitButtonIcon = new Icon(m_pRenderer, "", 32, 32);
	m_pCloseExitButtonIcon->SetDepth(1.0f);
	m_pCloseExitButtonIconHover = new Icon(m_pRenderer, "", 32, 32);
	m_pCloseExitButtonIconHover->SetDepth(1.0f);
	m_pCloseExitButtonIconPressed = new Icon(m_pRenderer, "", 32, 32);
	m_pCloseExitButtonIconPressed->SetDepth(1.0f);

	// Rotate buttons
	m_pArrowLeftIcon = new Icon(m_pRenderer, "", 32, 32);
	m_pArrowLeftIcon->SetDepth(2.0f);
	m_pArrowLeftIconHover = new Icon(m_pRenderer, "", 32, 32);
	m_pArrowLeftIconHover->SetDepth(2.0f);
	m_pArrowLeftIconPressed = new Icon(m_pRenderer, "", 32, 32);
	m_pArrowLeftIconPressed->SetDepth(2.0f);
	m_pArrowRightIcon = new Icon(m_pRenderer, "", 32, 32);
	m_pArrowRightIcon->SetDepth(2.0f);
	m_pArrowRightIconHover = new Icon(m_pRenderer, "", 32, 32);
	m_pArrowRightIconHover->SetDepth(2.0f);
	m_pArrowRightIconPressed = new Icon(m_pRenderer, "", 32, 32);
	m_pArrowRightIconPressed->SetDepth(2.0f);

	// Tab options
	m_pTab75OptionIcon = new Icon(m_pRenderer, "", 75, 44);
	m_pTab75OptionIcon->SetDepth(2.0f);
	m_pTab75OptionIconHover = new Icon(m_pRenderer, "", 75, 44);
	m_pTab75OptionIconHover->SetDepth(2.0f);
	m_pTab75OptionIconPressed = new Icon(m_pRenderer, "", 75, 44);
	m_pTab75OptionIconPressed->SetDepth(2.0f);
	m_pTab75OptionIconToggled = new Icon(m_pRenderer, "", 75, 44);
	m_pTab75OptionIconToggled->SetDepth(2.0f);
	m_pTab75OptionIconToggledHover = new Icon(m_pRenderer, "", 75, 44);
	m_pTab75OptionIconToggledHover->SetDepth(2.0f);
	m_pTab75OptionIconToggledPressed = new Icon(m_pRenderer, "", 75, 44);
	m_pTab75OptionIconToggledPressed->SetDepth(2.0f);

	// Common, shared frontend page params
	m_cameraOrbitTimer = 15.0f;

	// Constants
	m_tooltipAppearDelay = 0.25f;

	// Load the common graphics that are set as the currently loaded mod
	LoadCommonGraphics(CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme());

	// Pages
	int width = 800;
	int height = 800;
	FrontendPage* pMainMenu = new MainMenu(m_pRenderer, m_pGUI, this, width, height);
	FrontendPage* pSelectCharacter = new SelectCharacter(m_pRenderer, m_pGUI, this, width, height);
	FrontendPage* pCreateCharacter = new CreateCharacter(m_pRenderer, m_pGUI, this, width, height);
	FrontendPage* pQuitPopup = new QuitPopup(m_pRenderer, m_pGUI, this, width, height);
	FrontendPage* pPauseMenu = new PauseMenu(m_pRenderer, m_pGUI, this, width, height);
	FrontendPage* pOptionsMenu = new OptionsMenu(m_pRenderer, m_pGUI, this, width, height);
	FrontendPage* pCredits = new Credits(m_pRenderer, m_pGUI, this, width, height);
	FrontendPage* pModMenu = new ModMenu(m_pRenderer, m_pGUI, this, width, height);

	m_vpFrontendPages.push_back(pMainMenu);
	m_vpFrontendPages.push_back(pSelectCharacter);
	m_vpFrontendPages.push_back(pCreateCharacter);
	m_vpFrontendPages.push_back(pQuitPopup);
	m_vpFrontendPages.push_back(pPauseMenu);
	m_vpFrontendPages.push_back(pOptionsMenu);
	m_vpFrontendPages.push_back(pCredits);
	m_vpFrontendPages.push_back(pModMenu);

	// Initial page
	m_currentScreen = FrontendScreen::None;
	m_currentPage = nullptr;
	SetFrontendScreen(FrontendScreen::None);
}

FrontendManager::~FrontendManager()
{
	// Frontend pages
	for (size_t i = 0; i < m_vpFrontendPages.size(); ++i)
	{
		delete m_vpFrontendPages[i];
		m_vpFrontendPages[i] = nullptr;
	}
	m_vpFrontendPages.clear();

	// Checkbox
	delete m_pCheckboxIcon;
	delete m_pCheckboxIconHover;
	delete m_pCheckboxIconPressed;
	delete m_pCheckboxIconDisabled;
	delete m_pCheckboxIconToggled;
	delete m_pCheckboxIconToggledHover;
	delete m_pCheckboxIconToggledPressed;
	delete m_pCheckboxIconToggledDisabled;

	// Optionbox
	delete m_pOptionboxIcon;
	delete m_pOptionboxIconHover;
	delete m_pOptionboxIconPressed;
	delete m_pOptionboxIconDisabled;
	delete m_pOptionboxIconToggled;
	delete m_pOptionboxIconToggledHover;
	delete m_pOptionboxIconToggledPressed;
	delete m_pOptionboxIconToggledDisabled;

	// Scrollbar
	delete m_pScrollbarArrowDownIcon;
	delete m_pScrollbarArrowDownIconHover;
	delete m_pScrollbarArrowDownIconPressed;
	delete m_pScrollbarArrowDownIconDisabled;
	delete m_pScrollbarArrowUpIcon;
	delete m_pScrollbarArrowUpIconHover;
	delete m_pScrollbarArrowUpIconPressed;
	delete m_pScrollbarArrowUpIconDisabled;
	delete m_pScrollbarBackgroundIcon;
	delete m_pScrollbarBackgroundIconDisabled;
	delete m_pScrollbarIcon;
	delete m_pScrollbarIconHover;
	delete m_pScrollbarIconPressed;
	delete m_pScrollbarIconDisabled;

	// Slider
	delete m_pSliderIconDefault;
	delete m_pSliderIconHover;
	delete m_pSliderIconDrag;
	delete m_pSliderIconDefaultBlank;
	delete m_pSliderIconHoverBlank;
	delete m_pSliderIconDragBlank;
	delete m_pSliderIncrementIcon;
	delete m_pSliderIncrementIconHover;
	delete m_pSliderIncrementIconPressed;
	delete m_pSliderIncrementIconDisabled;
	delete m_pSliderDecrementIcon;
	delete m_pSliderDecrementIconHover;
	delete m_pSliderDecrementIconPressed;
	delete m_pSliderDecrementIconDisabled;

	// Buttons
	for (int i = 0; i < static_cast<int>(ButtonSize::NUM); i++)
	{
		delete m_pButtonIcon[i];
		delete m_pButtonIconHover[i];
		delete m_pButtonIconPressed[i];
		delete m_pButtonIconDisabled[i];
	}

	// Close button
	delete m_pCloseExitButtonIcon;
	delete m_pCloseExitButtonIconHover;
	delete m_pCloseExitButtonIconPressed;

	// Rotate buttons
	delete m_pArrowLeftIcon;
	delete m_pArrowLeftIconHover;
	delete m_pArrowLeftIconPressed;
	delete m_pArrowRightIcon;
	delete m_pArrowRightIconHover;
	delete m_pArrowRightIconPressed;

	// Tab options
	delete m_pTab75OptionIcon;
	delete m_pTab75OptionIconHover;
	delete m_pTab75OptionIconPressed;
	delete m_pTab75OptionIconToggled;
	delete m_pTab75OptionIconToggledHover;
	delete m_pTab75OptionIconToggledPressed;
}

// Windows dimensions
void FrontendManager::SetWindowDimensions(int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;

	for (size_t i = 0; i < m_vpFrontendPages.size(); ++i)
	{
		m_vpFrontendPages[i]->SetWindowDimensions(m_windowWidth, m_windowHeight);
	}
}

int FrontendManager::GetWindowWidth() const
{
	return m_windowWidth;
}

int FrontendManager::GetWindowHeight() const
{
	return m_windowHeight;
}

// Camera
void FrontendManager::SetCamera(Camera* pCamera)
{
	m_pCamera = pCamera;
}

// Skinning the GUI
void FrontendManager::SkinGUI()
{
	for (size_t i = 0; i < m_vpFrontendPages.size(); ++i)
	{
		m_vpFrontendPages[i]->SkinGUI();
	}
}

void FrontendManager::UnSkinGUI()
{
	for (size_t i = 0; i < m_vpFrontendPages.size(); ++i)
	{
		m_vpFrontendPages[i]->UnSkinGUI();
	}
}

// Frontend screen
FrontendScreen FrontendManager::GetFrontendScreen() const
{
	return m_currentScreen;
}

void FrontendManager::SetFrontendScreen(FrontendScreen screen)
{
	m_currentScreen = screen;

	// Unload current page
	if (m_currentPage != nullptr)
	{
		m_currentPage->Unload();
		m_currentPage = nullptr;
	}

	// Find new page
	for (size_t i = 0; i < m_vpFrontendPages.size(); ++i)
	{
		if (m_vpFrontendPages[i]->GetPageType() == screen)
		{
			m_currentPage = m_vpFrontendPages[i];
			break;
		}
	}

	// Load new page
	if (m_currentPage != nullptr)
	{
		m_currentPage->Load();
	}
}

// Load the icon graphics based on a theme
void FrontendManager::LoadCommonGraphics(std::string themeName)
{
	std::string iconName;

	// Checkbox
	iconName = "Resources/textures/gui/" + themeName + "/common/checkbox/checkDefault.tga";
	m_pCheckboxIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/checkbox/checkHover.tga";
	m_pCheckboxIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/checkbox/checkPressed.tga";
	m_pCheckboxIconPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/checkbox/checkDisabled.tga";
	m_pCheckboxIconDisabled->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/checkbox/checkToggled.tga";
	m_pCheckboxIconToggled->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/checkbox/checkToggledHover.tga";
	m_pCheckboxIconToggledHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/checkbox/checkToggledPressed.tga";
	m_pCheckboxIconToggledPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/checkbox/checkToggledDisabled.tga";
	m_pCheckboxIconToggledDisabled->SetIcon(iconName);

	// Optionbox
	iconName = "Resources/textures/gui/" + themeName + "/common/optionbox/optionDefault.tga";
	m_pOptionboxIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/optionbox/optionHover.tga";
	m_pOptionboxIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/optionbox/optionPressed.tga";
	m_pOptionboxIconPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/optionbox/optionDisabled.tga";
	m_pOptionboxIconDisabled->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/optionbox/optionToggled.tga";
	m_pOptionboxIconToggled->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/optionbox/optionToggledHover.tga";
	m_pOptionboxIconToggledHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/optionbox/optionToggledPressed.tga";
	m_pOptionboxIconToggledPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/optionbox/optionToggledDisabled.tga";
	m_pOptionboxIconToggledDisabled->SetIcon(iconName);

	// Scrollbar
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/arrowDownDefault.tga";
	m_pScrollbarArrowDownIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/arrowDownHover.tga";
	m_pScrollbarArrowDownIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/arrowDownPressed.tga";
	m_pScrollbarArrowDownIconPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/arrowDownDisabled.tga";
	m_pScrollbarArrowDownIconDisabled->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/arrowUpDefault.tga";
	m_pScrollbarArrowUpIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/arrowUpHover.tga";
	m_pScrollbarArrowUpIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/arrowUpPressed.tga";
	m_pScrollbarArrowUpIconPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/arrowUpDisabled.tga";
	m_pScrollbarArrowUpIconDisabled->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/backgroundDefault.tga";
	m_pScrollbarBackgroundIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/backgroundDisabled.tga";
	m_pScrollbarBackgroundIconDisabled->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/scrollbarDefault.tga";
	m_pScrollbarIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/scrollbarHover.tga";
	m_pScrollbarIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/scrollbarPressed.tga";
	m_pScrollbarIconPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/scrollbar/scrollbarDisabled.tga";
	m_pScrollbarIconDisabled->SetIcon(iconName);

	// Slider
	iconName = "Resources/textures/gui/" + themeName + "/common/slider/slider_slide.tga";
	m_pSliderIconDefault->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/slider/slider_slide_hover.tga";
	m_pSliderIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/slider/slider_slide_pressed.tga";
	m_pSliderIconDrag->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/slider/slider_slide_blank.tga";
	m_pSliderIconDefaultBlank->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/slider/slider_slide_hover_blank.tga";
	m_pSliderIconHoverBlank->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/slider/slider_slide_pressed_blank.tga";
	m_pSliderIconDragBlank->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/increment_buttons/plus_default.tga";
	m_pSliderIncrementIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/increment_buttons/plus_hover.tga";
	m_pSliderIncrementIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/increment_buttons/plus_pressed.tga";
	m_pSliderIncrementIconPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/increment_buttons/plus_disabled.tga";
	m_pSliderIncrementIconDisabled->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/increment_buttons/minus_default.tga";
	m_pSliderDecrementIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/increment_buttons/minus_hover.tga";
	m_pSliderDecrementIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/increment_buttons/minus_pressed.tga";
	m_pSliderDecrementIconPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/increment_buttons/minus_disabled.tga";
	m_pSliderDecrementIconDisabled->SetIcon(iconName);

	// Buttons
	for (int i = 0; i < static_cast<int>(ButtonSize::NUM); ++i)
	{
		std::string sizeFolder = "";
		if (ButtonSize(i) == ButtonSize::Size85x25) { sizeFolder = "85x25"; }
		if (ButtonSize(i) == ButtonSize::Size65x25) { sizeFolder = "65x25"; }
		if (ButtonSize(i) == ButtonSize::Size110x47) { sizeFolder = "110x47"; }
		if (ButtonSize(i) == ButtonSize::Size225x75) { sizeFolder = "225x75"; }

		iconName = "Resources/textures/gui/" + themeName + "/common/buttons/" + sizeFolder + "/buttonDefault.tga";
		m_pButtonIcon[i]->SetIcon(iconName);
		iconName = "Resources/textures/gui/" + themeName + "/common/buttons/" + sizeFolder + "/buttonHover.tga";
		m_pButtonIconHover[i]->SetIcon(iconName);
		iconName = "Resources/textures/gui/" + themeName + "/common/buttons/" + sizeFolder + "/buttonPressed.tga";
		m_pButtonIconPressed[i]->SetIcon(iconName);
		iconName = "Resources/textures/gui/" + themeName + "/common/buttons/" + sizeFolder + "/buttonDisabled.tga";
		m_pButtonIconDisabled[i]->SetIcon(iconName);
	}

	// Close button
	iconName = "Resources/textures/gui/" + themeName + "/common/close_button/close_exit.tga";
	m_pCloseExitButtonIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/close_button/close_exit_hover.tga";
	m_pCloseExitButtonIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/close_button/close_exit_pressed.tga";
	m_pCloseExitButtonIconPressed->SetIcon(iconName);

	// Rotate buttons
	iconName = "Resources/textures/gui/" + themeName + "/common/rotate/arrow_left.tga";
	m_pArrowLeftIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/rotate/arrow_left_hover.tga";
	m_pArrowLeftIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/rotate/arrow_left_pressed.tga";
	m_pArrowLeftIconPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/rotate/arrow_right.tga";
	m_pArrowRightIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/rotate/arrow_right_hover.tga";
	m_pArrowRightIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/rotate/arrow_right_pressed.tga";
	m_pArrowRightIconPressed->SetIcon(iconName);

	// Tab options
	iconName = "Resources/textures/gui/" + themeName + "/common/tab_option/tab.tga";
	m_pTab75OptionIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/tab_option/tab_hover.tga";
	m_pTab75OptionIconHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/tab_option/tab_pressed.tga";
	m_pTab75OptionIconPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/tab_option/tab_toggled.tga";
	m_pTab75OptionIconToggled->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/tab_option/tab_toggled_hover.tga";
	m_pTab75OptionIconToggledHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/common/tab_option/tab_toggled_pressed.tga";
	m_pTab75OptionIconToggledPressed->SetIcon(iconName);

	// Label colors
	std::string settingsIniFile = "Resources/textures/gui/" + themeName + "/theme.ini";
	INIReader reader(settingsIniFile);

	if (reader.ParseError() >= 0)
	{
		float r = static_cast<float>(reader.GetReal("NormalLabel", "Red", 1.0f));
		float g = static_cast<float>(reader.GetReal("NormalLabel", "Green", 1.0f));
		float b = static_cast<float>(reader.GetReal("NormalLabel", "Blue", 1.0f));
		m_normalColor = Color(r, g, b);

		r = static_cast<float>(reader.GetReal("HoverLabel", "Red", 1.0f));
		g = static_cast<float>(reader.GetReal("HoverLabel", "Green", 1.0f));
		b = static_cast<float>(reader.GetReal("HoverLabel", "Blue", 1.0f));
		m_hoverColor = Color(r, g, b);

		r = static_cast<float>(reader.GetReal("PressedLabel", "Red", 1.0f));
		g = static_cast<float>(reader.GetReal("PressedLabel", "Green", 1.0f));
		b = static_cast<float>(reader.GetReal("PressedLabel", "Blue", 1.0f));
		m_pressedColor = Color(r, g, b);

		r = static_cast<float>(reader.GetReal("DisabledLabel", "Red", 1.0f));
		g = static_cast<float>(reader.GetReal("DisabledLabel", "Green", 1.0f));
		b = static_cast<float>(reader.GetReal("DisabledLabel", "Blue", 1.0f));
		m_disabledColor = Color(r, g, b);
	}
}

// Setup icons for components
void FrontendManager::SetCheckboxIcons(CheckBox* pCheckBox) const
{
	pCheckBox->SetDefaultIcon(GetCheckboxIcon());
	pCheckBox->SetHoverIcon(GetCheckboxIconHover());
	pCheckBox->SetSelectedIcon(GetCheckboxIconPressed());
	pCheckBox->SetDisabledIcon(GetCheckboxIconDisabled());
	pCheckBox->SetToggledIcon(GetCheckboxIconToggled());
	pCheckBox->SetToggledHoverIcon(GetCheckboxIconToggledHover());
	pCheckBox->SetToggledSelectedIcon(GetCheckboxIconToggledPressed());
	pCheckBox->SetToggledDisabledIcon(GetCheckboxIconToggledDisabled());
}

void FrontendManager::SetOptionboxIcons(OptionBox* pOptionBox) const
{
	pOptionBox->SetDefaultIcon(GetOptionboxIcon());
	pOptionBox->SetHoverIcon(GetOptionboxIconHover());
	pOptionBox->SetSelectedIcon(GetOptionboxIconPressed());
	pOptionBox->SetDisabledIcon(GetOptionboxIconDisabled());
	pOptionBox->SetToggledIcon(GetOptionboxIconToggled());
	pOptionBox->SetToggledHoverIcon(GetOptionboxIconToggledHover());
	pOptionBox->SetToggledSelectedIcon(GetOptionboxIconToggledPressed());
	pOptionBox->SetToggledDisabledIcon(GetOptionboxIconToggledDisabled());
}

void FrontendManager::SetScrollbarIcons(ScrollBar* pScrollbar) const
{
	pScrollbar->SetRightArrowDefaultIcon(GetScrollbarArrowDownIcon());
	pScrollbar->SetRightArrowHoverIcon(GetScrollbarArrowDownHoverIcon());
	pScrollbar->SetRightArrowSelectedIcon(GetScrollbarArrowDownPressedIcon());
	pScrollbar->SetRightArrowDisabledIcon(GetScrollbarArrowDownDisabledIcon());
	pScrollbar->SetLeftArrowDefaultIcon(GetScrollbarArrowUpIcon());
	pScrollbar->SetLeftArrowHoverIcon(GetScrollbarArrowUpHoverIcon());
	pScrollbar->SetLeftArrowSelectedIcon(GetScrollbarArrowUpPressedIcon());
	pScrollbar->SetLeftArrowDisabledIcon(GetScrollbarArrowUpDisabledIcon());
	pScrollbar->SetBackbarIcon(GetScrollbarBackgroundIcon());
	pScrollbar->SetScrollbarDefaultIcon(GetScrollbarIcon());
	pScrollbar->SetScrollbarHoverIcon(GetScrollbarHoverIcon());
	pScrollbar->SetScrollbarSelectedIcon(GetScrollbarPressedIcon());
	pScrollbar->SetScrollbarDisabledIcon(GetScrollbarDisabledIcon());
}

void FrontendManager::SetSliderIcons(Slider* pSlider) const
{
	pSlider->SetScrollerDefaultIcon(GetSliderIconDefault());
	pSlider->SetScrollerHoverIcon(GetSliderIconHover());
	pSlider->SetScrollerDraggingIcon(GetSliderIconDrag());
	pSlider->SetScrollBackbarIncrementIconDefault(GetSliderIconIncrementButtonDefault());
	pSlider->SetScrollBackbarIncrementIconHover(GetSliderIconIncrementButtonHover());
	pSlider->SetScrollBackbarIncrementIconPressed(GetSliderIconIncrementButtonPressed());
	pSlider->SetScrollBackbarIncrementIconDisabled(GetSliderIconIncrementButtonDisabled());
	pSlider->SetScrollBackbarDecrementIconDefault(GetSliderIconDecrementButtonDefault());
	pSlider->SetScrollBackbarDecrementIconHover(GetSliderIconDecrementButtonHover());
	pSlider->SetScrollBackbarDecrementIconPressed(GetSliderIconDecrementButtonPressed());
	pSlider->SetScrollBackbarDecrementIconDisabled(GetSliderIconDecrementButtonDisabled());
}

void FrontendManager::SetPulldownMenuIcons(PulldownMenu* pPulldownMenu) const
{
	SetScrollbarIcons(pPulldownMenu->GetPulldownScrollBar());
	pPulldownMenu->SetPulldownIconDefault(GetScrollbarArrowDownIcon());
	pPulldownMenu->SetPulldownIconHover(GetScrollbarArrowDownHoverIcon());
	pPulldownMenu->SetPulldownIconPressed(GetScrollbarArrowDownPressedIcon());
	pPulldownMenu->SetPulldownIconDisabled(GetScrollbarArrowDownDisabledIcon());
}

void FrontendManager::SetButtonIcons(Button* pButton, ButtonSize size) const
{
	pButton->SetDefaultIcon(GetButtonIcon(size));
	pButton->SetHoverIcon(GetButtonIconHover(size));
	pButton->SetSelectedIcon(GetButtonIconPressed(size));
	pButton->SetDisabledIcon(GetButtonIconDisabled(size));
}

void FrontendManager::SetTabIcons(OptionBox* pTab) const
{
	pTab->SetDefaultIcon(GetTab75OptionIcon());
	pTab->SetHoverIcon(GetTab75OptionIconHover());
	pTab->SetSelectedIcon(GetTab75OptionIconPressed());
	pTab->SetDisabledIcon(GetTab75OptionIcon());  // HACK: Missing disabled graphic
	pTab->SetToggledIcon(GetTab75OptionIconToggled());
	pTab->SetToggledHoverIcon(GetTab75OptionIconToggledHover());
	pTab->SetToggledSelectedIcon(GetTab75OptionIconToggledPressed());
	pTab->SetToggledDisabledIcon(GetTab75OptionIconToggled());  // HACK: Missing disabled graphic
}

// Common, shared front-end page params
float FrontendManager::GetCameraOrbitTimer() const
{
	return m_cameraOrbitTimer;
}

void FrontendManager::SetCameraOrbitTimer(float orbit)
{
	m_cameraOrbitTimer = orbit;
}

// Frontend functionality
void FrontendManager::SetOptionsReturnToMainMenu(bool mainMenu)
{
	for (size_t i = 0; i < m_vpFrontendPages.size(); ++i)
	{
		if (m_vpFrontendPages[i]->GetPageType() == FrontendScreen::OptionsMenu)
		{
			static_cast<OptionsMenu*>(m_vpFrontendPages[i])->SetReturnToMainMenu(mainMenu);
		}
	}
}

void FrontendManager::SetModsMenuReturnToMainMenu(bool mainMenu)
{
	for (size_t i = 0; i < m_vpFrontendPages.size(); ++i)
	{
		if (m_vpFrontendPages[i]->GetPageType() == FrontendScreen::ModMenu)
		{
			static_cast<ModMenu*>(m_vpFrontendPages[i])->SetReturnToMainMenu(mainMenu);
		}
	}
}

void FrontendManager::SetHoverNPC(NPC* pHoverNPC) const
{
	if (m_currentScreen == FrontendScreen::SelectCharacter)
	{
		static_cast<SelectCharacter*>(m_currentPage)->SetHoverNPC(pHoverNPC);
	}
	else if (m_currentScreen == FrontendScreen::CreateCharacter)
	{
		static_cast<CreateCharacter*>(m_currentPage)->SetHoverNPC(pHoverNPC);
	}
}

void FrontendManager::SetSelectedNPC(NPC* pSelectedNPC) const
{
	if (m_currentScreen == FrontendScreen::SelectCharacter)
	{
		static_cast<SelectCharacter*>(m_currentPage)->SetSelectedNPC(pSelectedNPC);
	}
	else if (m_currentScreen == FrontendScreen::CreateCharacter)
	{
		static_cast<CreateCharacter*>(m_currentPage)->SetSelectedNPC(pSelectedNPC);
	}
}

void FrontendManager::SetCharacterSubSelection(std::string subSelection) const
{
	if (m_currentScreen == FrontendScreen::CreateCharacter)
	{
		static_cast<CreateCharacter*>(m_currentPage)->DeletePresetButtons();

		PresetSection section = PresetSection::None;

		if (subSelection == "Head") { section = PresetSection::Head; }
		else if (subSelection == "Body") { section = PresetSection::Body; }
		else if (subSelection == "Legs") { section = PresetSection::Legs; }
		else if (subSelection == "Right_Shoulder") { section = PresetSection::RightShoulder; }
		else if (subSelection == "Left_Shoulder") { section = PresetSection::LeftShoulder; }
		else if (subSelection == "Right_Hand") { section = PresetSection::RightHand; }
		else if (subSelection == "Left_Hand") { section = PresetSection::LeftHand; }
		else if (subSelection == "Right_Foot") { section = PresetSection::RightFoot; }
		else if (subSelection == "Left_Foot") { section = PresetSection::LeftFoot; }

		if (section != PresetSection::None)
		{
			static_cast<CreateCharacter*>(m_currentPage)->CreatePresetButtons(section, true);
		}
	}
}

void FrontendManager::GotoNextCreditScreen() const
{
	if (m_currentScreen == FrontendScreen::Credits)
	{
		static_cast<Credits*>(m_currentPage)->GotoNextCreditPage();
	}
}

void FrontendManager::DisableShadowOption()
{
	for (size_t i = 0; i < m_vpFrontendPages.size(); ++i)
	{
		if (m_vpFrontendPages[i]->GetPageType() == FrontendScreen::OptionsMenu)
		{
			static_cast<OptionsMenu*>(m_vpFrontendPages[i])->DisableShadowOption();
		}
	}
}

// Constants
float FrontendManager::GetToolTipAppearDelay() const
{
	if (CubbyGame::GetInstance()->GetCubbySettings()->m_tooltipDelay)
	{
		return m_tooltipAppearDelay;
	}

	return 0.0f;
}

// Updating
void FrontendManager::Update(float dt) const
{
	if (m_currentPage != nullptr)
	{
		m_currentPage->Update(dt);
	}

	if (m_currentScreen != FrontendScreen::None && m_currentScreen != FrontendScreen::PauseMenu && m_currentScreen != FrontendScreen::QuitPopup)
	{
		if (CubbyGame::GetInstance()->GetGameMode() == GameMode::FrontEnd && CubbyGame::GetInstance()->GetCameraMode() == CameraMode::Frontend)
		{
			UpdateFrontEndCamera(dt);
		}
	}
}

// Updating
void FrontendManager::UpdateFrontEndCamera(float dt) const
{
	if (m_currentPage != nullptr)
	{
		if (m_pCamera != nullptr)
		{
			// Position
			m_pCamera->SetFakePosition(m_currentPage->GetCameraPosition());

			// Forward
			glm::vec3 cameraLookAt = m_currentPage->GetCameraView();
			glm::vec3 cameraForward = normalize(cameraLookAt - m_pCamera->GetFakePosition());
			m_pCamera->SetFacing(cameraForward);

			// Right
			glm::vec3 cameraRight = normalize(cross(cameraForward, glm::vec3(0.0f, 1.0f, 0.0f)));
			m_pCamera->SetRight(cameraRight);

			// Up
			glm::vec3 cameraUp = normalize(cross(cameraRight, cameraForward));
			m_pCamera->SetUp(cameraUp);
		}
	}
}

// Rendering
void FrontendManager::Render() const
{
	if (m_currentPage != nullptr)
	{
		m_currentPage->Render();
	}
}

void FrontendManager::Render2D() const
{
	if (m_currentPage != nullptr)
	{
		m_currentPage->Render2D();
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void FrontendManager::RenderDebug() const
{
	// Do nothing
}
