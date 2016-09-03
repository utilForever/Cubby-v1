/*************************************************************************
> File Name: Credits.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A front-end page that displays the credits for the game.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_CREDITS_H
#define CUBBY_CREDITS_H

#include <NPC/NPC.h>

#include "../FrontendPage.h"

struct CreditTextLine
{
	std::string m_text;
	float m_screenWidthRatio;
	float m_screenHeightDifference;
	unsigned int m_fontId;
	unsigned int m_outlineFontId;
};

using CreditTextLineList = std::vector<CreditTextLine*>;

class Credits : public FrontendPage
{
public:
	// Constructor, Destructor
	Credits(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, int windowWidth, int windowHeight);
	~Credits();

	void Reset() override;

	void CreateCreditTextLinesForPage(int pageIndex);
	void ClearCreditTextLines();

	void CreateCreditsNPCForPage(int pageIndex);
	void ClearCreditsNPC();

	void SetWindowDimensions(int windowWidth, int windowHeight) override;

	// Skinning the GUI
	void SkinGUI() override;
	void UnSkinGUI() override;

	// Loading
	void Load() override;
	void Unload() override;

	void GotoNextCreditPage();

	void Update(float dt) override;

	void Render() override;
	void Render2D() override;

private:
	int m_numCreditPages;
	int m_currentCreditPage;

	NPC* m_pCreditsNPC;

	float m_clickToNextTimer;

	CreditTextLineList m_vpCreditLineList;
};

#endif