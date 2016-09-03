/*************************************************************************
> File Name: FrontendPage.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The abstract base class for front-end screens. Contains all the common
>    functionality and data that all derived front-end pages must override.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_FRONTEND_PAGE_H
#define CUBBY_FRONTEND_PAGE_H

#include <GUI/Label.h>

#include "FrontendScreens.h"

// Forward declaration
class Renderer;
class OpenGLGUI;
class FrontendManager;

class FrontendPage
{
public:
	// Constructor, Destructor
	FrontendPage(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, FrontendScreen pageType, int windowWidth, int windowHeight);
	virtual ~FrontendPage();

	virtual void SetWindowDimensions(int width, int height);

	FrontendScreen GetPageType() const;

	// Page title and sub-title
	void SetTitleAndSubtitle(std::string title, std::string subtitle);
	void RemoveTitlesAndSubTitle();

	// Reset
	virtual void Reset() = 0;

	// Skinning
	virtual void SkinGUI() = 0;
	virtual void UnSkinGUI() = 0;

	// Loading
	virtual void Load() = 0;
	virtual void Unload() = 0;

	// Camera
	glm::vec3 GetCameraPosition() const;
	glm::vec3 GetCameraView() const;

	// Update
	virtual void Update(float dt);

	// Render
	virtual void Render();
	virtual void Render2D();
	virtual void RenderDebug();

protected:
	/* Protected members */
	Renderer* m_pRenderer;
	OpenGLGUI* m_pGUI;
	FrontendManager* m_pFrontendManager;

	// Window params
	int m_windowWidth;
	int m_windowHeight;

	// Camera params
	glm::vec3 m_cameraPosition;
	glm::vec3 m_cameraView;

private:
	FrontendScreen m_pageType;

	// Page headers and description text
	Label* m_pPageTitle;
	Label* m_pPageSubtitle;
	bool m_addedTitles;
	bool m_updateTitleTextCenterLocation;
};

#endif