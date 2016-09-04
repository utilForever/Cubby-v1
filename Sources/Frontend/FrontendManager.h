/*************************************************************************
> File Name: FrontendManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A manager class to contain all the common functionality for front-end pages
>    and GUI graphics.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_FRONTEND_MANAGER_H
#define CUBBY_FRONTEND_MANAGER_H

#include <GUI/Checkbox.h>
#include <GUI/Icon.h>
#include <GUI/OptionBox.h>
#include <GUI/PulldownMenu.h>
#include <GUI/ScrollBar.h>
#include <GUI/Slider.h>
#include <NPC/NPC.h>
#include <Renderer/Camera.h>

#include "FrontendPage.h"
#include "FrontendScreens.h"

enum class ButtonSize
{
	Size85x25 = 0,
	Size65x25,
	Size110x47,
	Size225x75,

	NUM,
};

using FrontendPageList = std::vector<FrontendPage*>;

class FrontendManager
{
public:
	// Constructor, Destructor
	FrontendManager(Renderer* pRenderer, OpenGLGUI* pGUI);
	~FrontendManager();

	// Windows dimensions
	void SetWindowDimensions(int width, int height);
	int GetWindowWidth() const;
	int GetWindowHeight() const;

	// Camera
	void SetCamera(Camera* pCamera);

	// Skinning the GUI
	void SkinGUI();
	void UnSkinGUI();

	// Frontend screen
	FrontendScreen GetFrontendScreen() const;
	void SetFrontendScreen(FrontendScreen screen);

	// Load the icon graphics based on a theme
	void LoadCommonGraphics(std::string themeName);

	// Setup icons for components
	void SetCheckboxIcons(CheckBox* pCheckBox) const;
	void SetOptionboxIcons(OptionBox* pOptionBox) const;
	void SetScrollbarIcons(ScrollBar* pScrollbar) const;
	void SetSliderIcons(Slider* pSlider) const;
	void SetPulldownMenuIcons(PulldownMenu* pPulldownMenu) const;
	void SetButtonIcons(Button* pButton, ButtonSize size) const;
	void SetTabIcons(OptionBox* pTab) const;

	// Fonts
	unsigned int GetFrontendFontLarge() const { return m_frontendFontLarge; }
	unsigned int GetFrontendFontMedium() const { return m_frontendFontMedium; }
	unsigned int GetFrontendFontSmall() const { return m_frontendFontSmall; }
	unsigned int GetFrontendFont14() const { return m_frontendFont14; }
	unsigned int GetFrontendFont14Outline() const { return m_frontendFont14Outline; }
	unsigned int GetFrontendFont18() const { return m_frontendFont18; }
	unsigned int GetFrontendFont18Outline() const { return m_frontendFont18Outline; }
	unsigned int GetFrontendFont20() const { return m_frontendFont20; }
	unsigned int GetFrontendFont20Outline() const { return m_frontendFont20Outline; }
	unsigned int GetFrontendFont22() const { return m_frontendFont22; }
	unsigned int GetFrontendFont22Outline() const { return m_frontendFont22Outline; }
	unsigned int GetFrontendFont25() const { return m_frontendFont25; }
	unsigned int GetFrontendFont25Outline() const { return m_frontendFont25Outline; }
	unsigned int GetFrontendFont30() const { return m_frontendFont30; }
	unsigned int GetFrontendFont30Outline() const { return m_frontendFont30Outline; }
	unsigned int GetFrontendFont35() const { return m_frontendFont35; }
	unsigned int GetFrontendFont35Outline() const { return m_frontendFont35Outline; }
	unsigned int GetFrontendFont40() const { return m_frontendFont40; }
	unsigned int GetFrontendFont40Outline() const { return m_frontendFont40Outline; }
	unsigned int GetFrontendFont50() const { return m_frontendFont50; }
	unsigned int GetFrontendFont50Outline() const { return m_frontendFont50Outline; }
	unsigned int GetFrontendFont80() const { return m_frontendFont80; }
	unsigned int GetFrontendFont80Outline() const { return m_frontendFont80Outline; }

	// Text effects fonts
	unsigned int GetTextEffectFont() const { return m_textEffectFontNormal; }
	unsigned int GetTextEffectOutlineFont() const { return m_textEffectFontOutline; }

	// Label colours
	Color GetNormalFontColor() const { return m_normalColor; }
	Color GetHoverFontColor() const { return m_hoverColor; }
	Color GetPressedFontColor() const { return m_pressedColor; }
	Color GetDisabledFontColor() const { return m_disabledColor; }

	// Checkbox
	Icon* GetCheckboxIcon() const { return m_pCheckboxIcon; };
	Icon* GetCheckboxIconHover() const { return m_pCheckboxIconHover; };
	Icon* GetCheckboxIconPressed() const { return m_pCheckboxIconPressed; };
	Icon* GetCheckboxIconDisabled() const { return m_pCheckboxIconDisabled; };
	Icon* GetCheckboxIconToggled() const { return m_pCheckboxIconToggled; };
	Icon* GetCheckboxIconToggledHover() const { return m_pCheckboxIconToggledHover; };
	Icon* GetCheckboxIconToggledPressed() const { return m_pCheckboxIconToggledPressed; };
	Icon* GetCheckboxIconToggledDisabled() const { return m_pCheckboxIconToggledDisabled; };

	// Optionbox
	Icon* GetOptionboxIcon() const { return m_pOptionboxIcon; };
	Icon* GetOptionboxIconHover() const { return m_pOptionboxIconHover; };
	Icon* GetOptionboxIconPressed() const { return m_pOptionboxIconPressed; };
	Icon* GetOptionboxIconDisabled() const { return m_pOptionboxIconDisabled; };
	Icon* GetOptionboxIconToggled() const { return m_pOptionboxIconToggled; };
	Icon* GetOptionboxIconToggledHover() const { return m_pOptionboxIconToggledHover; };
	Icon* GetOptionboxIconToggledPressed() const { return m_pOptionboxIconToggledPressed; };
	Icon* GetOptionboxIconToggledDisabled() const { return m_pOptionboxIconToggledDisabled; };

	// Scrollbar
	Icon* GetScrollbarArrowDownIcon() const { return m_pScrollbarArrowDownIcon; };
	Icon* GetScrollbarArrowDownHoverIcon() const { return m_pScrollbarArrowDownIconHover; };
	Icon* GetScrollbarArrowDownPressedIcon() const { return m_pScrollbarArrowDownIconPressed; };
	Icon* GetScrollbarArrowDownDisabledIcon() const { return m_pScrollbarArrowDownIconDisabled; };
	Icon* GetScrollbarArrowUpIcon() const { return m_pScrollbarArrowUpIcon; };
	Icon* GetScrollbarArrowUpHoverIcon() const { return m_pScrollbarArrowUpIconHover; };
	Icon* GetScrollbarArrowUpPressedIcon() const { return m_pScrollbarArrowUpIconPressed; };
	Icon* GetScrollbarArrowUpDisabledIcon() const { return m_pScrollbarArrowUpIconDisabled; };
	Icon* GetScrollbarBackgroundIcon() const { return m_pScrollbarBackgroundIcon; };
	Icon* GetScrollbarBackgroundIconDisabled() const { return m_pScrollbarBackgroundIconDisabled; };
	Icon* GetScrollbarIcon() const { return m_pScrollbarIcon; };
	Icon* GetScrollbarHoverIcon() const { return m_pScrollbarIconHover; };
	Icon* GetScrollbarPressedIcon() const { return m_pScrollbarIconPressed; };
	Icon* GetScrollbarDisabledIcon() const { return m_pScrollbarIconDisabled; };

	// Slider
	Icon* GetSliderIconDefault() const { return m_pSliderIconDefault; };
	Icon* GetSliderIconHover() const { return m_pSliderIconHover; };
	Icon* GetSliderIconDrag() const { return m_pSliderIconDrag; };
	Icon* GetSliderIconDefaultBlank() const { return m_pSliderIconDefaultBlank; };
	Icon* GetSliderIconHoverBlank() const { return m_pSliderIconHoverBlank; };
	Icon* GetSliderIconDragBlank() const { return m_pSliderIconDragBlank; };
	Icon* GetSliderIconIncrementButtonDefault() const { return m_pSliderIncrementIcon; };
	Icon* GetSliderIconIncrementButtonHover() const { return m_pSliderIncrementIconHover; };
	Icon* GetSliderIconIncrementButtonPressed() const { return m_pSliderIncrementIconPressed; };
	Icon* GetSliderIconIncrementButtonDisabled() const { return m_pSliderIncrementIconDisabled; };
	Icon* GetSliderIconDecrementButtonDefault() const { return m_pSliderDecrementIcon; };
	Icon* GetSliderIconDecrementButtonHover() const { return m_pSliderDecrementIconHover; };
	Icon* GetSliderIconDecrementButtonPressed() const { return m_pSliderDecrementIconPressed; };
	Icon* GetSliderIconDecrementButtonDisabled() const { return m_pSliderDecrementIconDisabled; };

	// Button
	Icon* GetButtonIcon(ButtonSize size) const { return m_pButtonIcon[static_cast<int>(size)]; };
	Icon* GetButtonIconHover(ButtonSize size) const { return m_pButtonIconHover[static_cast<int>(size)]; };
	Icon* GetButtonIconPressed(ButtonSize size) const { return m_pButtonIconPressed[static_cast<int>(size)]; };
	Icon* GetButtonIconDisabled(ButtonSize size) const { return m_pButtonIconDisabled[static_cast<int>(size)]; };

	// Close button
	Icon* GetCloseExitButtonIcon() const { return m_pCloseExitButtonIcon; }
	Icon* GetCloseExitButtonIconHover() const { return m_pCloseExitButtonIconHover; }
	Icon* GetCloseExitButtonIconPressed() const { return m_pCloseExitButtonIconPressed; }

	// Rotate buttons
	Icon* GetRightRotateArrow() const { return m_pArrowRightIcon; }
	Icon* GetRightRotateArrowHover() const { return m_pArrowRightIconHover; }
	Icon* GetRightRotateArrowPressed() const { return m_pArrowRightIconPressed; }
	Icon* GetLeftRotateArrow() const { return m_pArrowLeftIcon; }
	Icon* GetLeftRotateArrowHover() const { return m_pArrowLeftIconHover; }
	Icon* GetLeftRotateArrowPressed() const { return m_pArrowLeftIconPressed; }

	// Tab Options
	Icon* GetTab75OptionIcon() const { return m_pTab75OptionIcon; }
	Icon* GetTab75OptionIconHover() const { return m_pTab75OptionIconHover; }
	Icon* GetTab75OptionIconPressed() const { return m_pTab75OptionIconPressed; }
	Icon* GetTab75OptionIconToggled() const { return m_pTab75OptionIconToggled; }
	Icon* GetTab75OptionIconToggledHover() const { return m_pTab75OptionIconToggledHover; }
	Icon* GetTab75OptionIconToggledPressed() const { return m_pTab75OptionIconToggledPressed; }

	// Common, shared frontend page params
	float GetCameraOrbitTimer() const;
	void SetCameraOrbitTimer(float orbit);

	// Frontend functionality
	void SetOptionsReturnToMainMenu(bool mainMenu);
	void SetModsMenuReturnToMainMenu(bool mainMenu);
	void SetHoverNPC(NPC* pHoverNPC) const;
	void SetSelectedNPC(NPC* pSelectedNPC) const;
	void SetCharacterSubSelection(std::string subSelection) const;
	void GotoNextCreditScreen() const;
	void DisableShadowOption();

	// Constants
	float GetToolTipAppearDelay() const;

	// Updating
	void Update(float dt) const;
	void UpdateFrontEndCamera(float dt) const;

	// Rendering
	void Render() const;
	void Render2D() const;
	void RenderDebug() const;

private:
	Renderer* m_pRenderer;
	OpenGLGUI* m_pGUI;

	// Window params
	int m_windowWidth;
	int m_windowHeight;

	// Camera
	Camera* m_pCamera;

	// Current frontend screen
	FrontendScreen m_currentScreen;
	FrontendPage* m_currentPage;

	// Pages
	FrontendPageList m_vpFrontendPages;

	// Fonts
	unsigned int m_frontendFontLarge;
	unsigned int m_frontendFontMedium;
	unsigned int m_frontendFontSmall;
	unsigned int m_frontendFont14;
	unsigned int m_frontendFont14Outline;
	unsigned int m_frontendFont18;
	unsigned int m_frontendFont18Outline;
	unsigned int m_frontendFont20;
	unsigned int m_frontendFont20Outline;
	unsigned int m_frontendFont22;
	unsigned int m_frontendFont22Outline;
	unsigned int m_frontendFont25;
	unsigned int m_frontendFont25Outline;
	unsigned int m_frontendFont30;
	unsigned int m_frontendFont30Outline;
	unsigned int m_frontendFont35;
	unsigned int m_frontendFont35Outline;
	unsigned int m_frontendFont40;
	unsigned int m_frontendFont40Outline;
	unsigned int m_frontendFont50;
	unsigned int m_frontendFont50Outline;
	unsigned int m_frontendFont80;
	unsigned int m_frontendFont80Outline;

	// Text effects fonts
	unsigned int m_textEffectFontNormal;
	unsigned int m_textEffectFontOutline;

	// Label colours
	Color m_normalColor;
	Color m_hoverColor;
	Color m_pressedColor;
	Color m_disabledColor;

	// Checkbox
	Icon* m_pCheckboxIcon;
	Icon* m_pCheckboxIconHover;
	Icon* m_pCheckboxIconPressed;
	Icon* m_pCheckboxIconDisabled;
	Icon* m_pCheckboxIconToggled;
	Icon* m_pCheckboxIconToggledHover;
	Icon* m_pCheckboxIconToggledPressed;
	Icon* m_pCheckboxIconToggledDisabled;

	// Optionbox
	Icon* m_pOptionboxIcon;
	Icon* m_pOptionboxIconHover;
	Icon* m_pOptionboxIconPressed;
	Icon* m_pOptionboxIconDisabled;
	Icon* m_pOptionboxIconToggled;
	Icon* m_pOptionboxIconToggledHover;
	Icon* m_pOptionboxIconToggledPressed;
	Icon* m_pOptionboxIconToggledDisabled;

	// Scrollbar
	Icon* m_pScrollbarArrowDownIcon;
	Icon* m_pScrollbarArrowDownIconHover;
	Icon* m_pScrollbarArrowDownIconPressed;
	Icon* m_pScrollbarArrowDownIconDisabled;
	Icon* m_pScrollbarArrowUpIcon;
	Icon* m_pScrollbarArrowUpIconHover;
	Icon* m_pScrollbarArrowUpIconPressed;
	Icon* m_pScrollbarArrowUpIconDisabled;
	Icon* m_pScrollbarBackgroundIcon;
	Icon* m_pScrollbarBackgroundIconDisabled;
	Icon* m_pScrollbarIcon;
	Icon* m_pScrollbarIconHover;
	Icon* m_pScrollbarIconPressed;
	Icon* m_pScrollbarIconDisabled;

	// Slider
	Icon* m_pSliderIconDefault;
	Icon* m_pSliderIconHover;
	Icon* m_pSliderIconDrag;
	Icon* m_pSliderIconDefaultBlank;
	Icon* m_pSliderIconHoverBlank;
	Icon* m_pSliderIconDragBlank;
	Icon* m_pSliderIncrementIcon;
	Icon* m_pSliderIncrementIconHover;
	Icon* m_pSliderIncrementIconPressed;
	Icon* m_pSliderIncrementIconDisabled;
	Icon* m_pSliderDecrementIcon;
	Icon* m_pSliderDecrementIconHover;
	Icon* m_pSliderDecrementIconPressed;
	Icon* m_pSliderDecrementIconDisabled;

	// Buttons
	Icon** m_pButtonIcon;
	Icon** m_pButtonIconHover;
	Icon** m_pButtonIconPressed;
	Icon** m_pButtonIconDisabled;

	// Close button
	Icon* m_pCloseExitButtonIcon;
	Icon* m_pCloseExitButtonIconHover;
	Icon* m_pCloseExitButtonIconPressed;

	// Rotate buttons
	Icon* m_pArrowLeftIcon;
	Icon* m_pArrowLeftIconHover;
	Icon* m_pArrowLeftIconPressed;
	Icon* m_pArrowRightIcon;
	Icon* m_pArrowRightIconHover;
	Icon* m_pArrowRightIconPressed;

	// Tab Options
	Icon* m_pTab75OptionIcon;
	Icon* m_pTab75OptionIconHover;
	Icon* m_pTab75OptionIconPressed;
	Icon* m_pTab75OptionIconToggled;
	Icon* m_pTab75OptionIconToggledHover;
	Icon* m_pTab75OptionIconToggledPressed;

	// Common, shared frontend page params
	float m_cameraOrbitTimer;

	// Constants
	float m_tooltipAppearDelay;
};

#endif