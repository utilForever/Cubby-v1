/*************************************************************************
> File Name: SelectCharacter.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A front-end page to allow the player to select which character they want
>    to play as, this page shows a line up of characters (NPCs) that the player
>    can select from. Also includes character customization and items equipped.
> Created Time: 2016/09/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_SELECT_CHARACTER_H
#define CUBBY_SELECT_CHARACTER_H

#include <GUI/Button.h>
#include <GUI/FormattedLabel.h>
#include <GUI/Icon.h>
#include <NPC/NPC.h>
#include <NPC/NPCManager.h>

#include "../FrontendPage.h"

class SelectCharacter : public FrontendPage
{
public:
	// Constructor, Destructor
	SelectCharacter(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontPageManager, int windowWidth, int windowHeight);
	~SelectCharacter();

	void Reset() override;

	void SetWindowDimensions(int width, int height) override;

	void SkinGUI() override;
	void UnSkinGUI() override;

	void Load() override;
	void Unload() override;

	void LoadSelectionCharacters();

	int GetNumCreatedCharacters() const;

	void SetHoverNPC(NPC* pHoverNPC);
	void SetSelectedNPC(NPC* pSelectedNPC);

	void ShowTooltip(int x, int y);
	void HideTooltip();

	void OpenPopup(std::string popupTitle, std::string popupText);
	void ClosePopup();

	void Update(float dt) override;

	void Render() override;
	void Render2D() override;

protected:
	static void _SelectCharacterPressed(void* pData);
	void SelectCharacterPressed();

	static void _DeleteCharacterPressed(void* pData);
	void DeleteCharacterPressed();

	static void _CreateCharacterPressed(void* pData);
	void CreateCharacterPressed() const;

	static void _BackPressed(void* pData);
	void BackPressed() const;

	static void _PopupConfirmPressed(void* pData);
	void PopupConfirmPressed();

	static void _PopupCancelPressed(void* pData);
	void PopupCancelPressed();

private:
	NPCList m_vpCharacterLineUp;

	bool m_addedCreateButton;
	bool m_updateCreateButton;

	// Hover and selection NPC pointers
	NPC* m_pHoverSelectionNPC;
	NPC* m_pSelectedNPC;

	// Text alpha animation
	float m_nameTextFadeInAlpha;
	float m_nameTextFadeInFadeInTime;
	float m_nameTextFadeInFadeInTimer;

	// Tooltip
	bool m_toolTipVisible;
	int m_tooltipWidth;
	int m_tooltipHeight;
	int m_tooltipDescBorder;
	Icon* m_pTooltipBackground;
	Label* m_pTooltipNameLabel;
	FormattedLabel* m_pTooltipDescriptionLabel;

	// Popup
	int m_popupWidth;
	int m_popupHeight;
	int m_popupBorderSpacer;
	int m_popupTitleSpacer;
	int m_popupIconSize;
	int m_popupIconSpacer;
	Label* m_popupTitle;
	FormattedLabel* m_popupText;
	Button* m_pPopupConfirmButton;
	Button* m_pPopupCancelButton;
	Icon* m_pPopupBackgroundIcon;

	Button* m_pSelectCharacterButton;
	Button* m_pDeleteCharacterButton;
	Button* m_pCreateCharacterButton;
	Button* m_pBackButton;
};

#endif