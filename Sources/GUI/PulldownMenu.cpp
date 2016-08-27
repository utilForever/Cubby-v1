/*************************************************************************
> File Name: PulldownMenu.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A pulldown menu GUI component.
> Created Time: 2016/08/27
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "PulldownMenu.h"
#include "DirectDrawRectangle.h"
#include "MultiTextureIcon.h"
#include "GUIWindow.h"

// Constructor, Destructor
PulldownMenu::PulldownMenu(Renderer* pRenderer, unsigned int GUIFont, const std::string& menuTitle) :
	Container(pRenderer), m_pPulldownIcon(nullptr), m_pPulldownSelectedIcon(nullptr),
	m_pPulldownHoverIcon(nullptr), m_pPulldownDisabledIcon(nullptr),
	m_pMenuBackgroundDefault(nullptr), m_pMenuBackgroundHover(nullptr),
	m_pMenuBackgroundSelected(nullptr), m_pMenuBackgroundDisabled(nullptr),
	m_pTitleBackgroundDefault(nullptr),	m_pTitleBackgroundHover(nullptr),
	m_pTitleBackgroundSelected(nullptr), m_pTitleBackgroundDisabled(nullptr),
	m_GUIFont(GUIFont),	m_menuName(pRenderer, GUIFont, menuTitle, Color(0.0f, 0.0f, 0.0f)),
	m_menuTitle(pRenderer, GUIFont, menuTitle, Color(0.0f, 0.0f, 0.0f)),
	m_textColor(0.0f, 0.0f, 0.0f), m_pulldownMenu(pRenderer, GUIFont, menuTitle, false),
	m_scrollBar(pRenderer), m_isSelected(false), m_isHover(false), m_isDisabled(false)
{
	SetDefaultIcons(pRenderer);

	m_scrollBar.SetScrollDirection(ScrollBarDirection::Vertical);
	m_scrollBar.SetScrollPosition(0.75f);

	m_pulldownMenu.SetPullDownMenuParent(this);

	Container::Add(&m_pulldownMenu);

	Container::Add(&m_scrollBar);

	m_menuTitle.SetLocation(0, 0);
	Container::Add(&m_menuTitle);

	m_renderMenuHeader = false;
	m_menuName.SetLocation(0, 0);
	Container::Add(&m_menuName);

	m_pulldownMenu.SetMenuItemSpacer(2);

	//m_pulldownMenu.SetAutoSizeMenuWidth(false);

	m_GUIFont = GUIFont;

	m_outline = false;

	m_numItems = 0;
	m_selectedIndex = -1;
	m_maxDisplayItems = 0;

	m_isDisplayScrollbar = true;

	m_pSelectedMenuItem = nullptr;

	m_menuItemChangedCallback = nullptr;
	m_pMenuItemChangedCallbackData = nullptr;
}

PulldownMenu::PulldownMenu(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& menuTitle, Color fontColor, Color outlineColor) :
	Container(pRenderer), m_pPulldownIcon(nullptr), m_pPulldownSelectedIcon(nullptr),
	m_pPulldownHoverIcon(nullptr), m_pPulldownDisabledIcon(nullptr),
	m_pMenuBackgroundDefault(nullptr), m_pMenuBackgroundHover(nullptr),
	m_pMenuBackgroundSelected(nullptr), m_pMenuBackgroundDisabled(nullptr),
	m_pTitleBackgroundDefault(nullptr), m_pTitleBackgroundHover(nullptr),
	m_pTitleBackgroundSelected(nullptr), m_pTitleBackgroundDisabled(nullptr),
	m_GUIFont(GUIFont), m_menuName(pRenderer, GUIFont, menuTitle, Color(0.0f, 0.0f, 0.0f)),
	m_menuTitle(pRenderer, GUIFont, menuTitle, Color(0.0f, 0.0f, 0.0f)),
	m_textColor(0.0f, 0.0f, 0.0f), m_pulldownMenu(pRenderer, GUIFont, menuTitle, false),
	m_scrollBar(pRenderer),	m_isSelected(false), m_isHover(false), m_isDisabled(false)
{
	SetDefaultIcons(pRenderer);

	m_scrollBar.SetScrollDirection(ScrollBarDirection::Vertical);
	m_scrollBar.SetScrollPosition(0.75f);

	m_pulldownMenu.SetPullDownMenuParent(this);

	Container::Add(&m_pulldownMenu);

	Container::Add(&m_scrollBar);

	m_menuTitle.SetLocation(0, 0);
	m_menuTitle.SetColor(fontColor);
	m_menuTitle.SetOutline(true);
	m_menuTitle.SetOutlineColor(outlineColor);
	m_menuTitle.SetOutlineFont(outlineGUIFont);
	Container::Add(&m_menuTitle);

	m_renderMenuHeader = false;
	m_menuName.SetLocation(0, 0);
	m_menuName.SetColor(fontColor);
	m_menuName.SetOutline(true);
	m_menuName.SetOutlineColor(outlineColor);
	m_menuName.SetOutlineFont(outlineGUIFont);
	Container::Add(&m_menuName);

	//m_pulldownMenu.SetAutoSizeMenuWidth(false);

	m_GUIFont = GUIFont;
	m_outlineGUIFont = outlineGUIFont;

	m_outline = true;
	m_outlineColor = outlineColor;
	m_textColor = fontColor;

	m_numItems = 0;
	m_selectedIndex = -1;
	m_maxDisplayItems = 0;

	m_isDisplayScrollbar = true;

	m_pSelectedMenuItem = nullptr;

	m_menuItemChangedCallback = nullptr;
	m_pMenuItemChangedCallbackData = nullptr;
}

PulldownMenu::PulldownMenu(Renderer* pRenderer, unsigned int GUIFont, const std::string& menuTitle,
	const std::string& defaultPulldownTexture, const std::string& selectedPulldownTexture, const std::string& hoverPulldownTexture, const std::string& disabledPulldownTexture,
	const std::string& defaultTitleTexture, const std::string& selectedTitleTexture, const std::string& hoverTitleTexture, const std::string& disabledTitleTexture,
	const std::string& defaultMenuTexture, const std::string& selectedMenuTexture, const std::string& hoverMenuTexture, const std::string& disabledMenuTexture) :
	Container(pRenderer), m_pPulldownIcon(nullptr), m_pPulldownSelectedIcon(nullptr),
	m_pPulldownHoverIcon(nullptr), m_pPulldownDisabledIcon(nullptr),
	m_pMenuBackgroundDefault(nullptr), m_pMenuBackgroundHover(nullptr),
	m_pMenuBackgroundSelected(nullptr), m_pMenuBackgroundDisabled(nullptr),
	m_pTitleBackgroundDefault(nullptr), m_pTitleBackgroundHover(nullptr),
	m_pTitleBackgroundSelected(nullptr), m_pTitleBackgroundDisabled(nullptr),
	m_GUIFont(GUIFont), m_menuName(pRenderer, GUIFont, menuTitle, Color(0.0f, 0.0f, 0.0f)),
	m_menuTitle(pRenderer, GUIFont, menuTitle, Color(0.0f, 0.0f, 0.0f)),
	m_textColor(0.0f, 0.0f, 0.0f), m_pulldownMenu(pRenderer, GUIFont, menuTitle, false),
	m_scrollBar(pRenderer), m_isSelected(false), m_isHover(false), m_isDisabled(false)
{
	m_pPulldownIcon = new Icon(pRenderer, defaultPulldownTexture);
	m_pPulldownSelectedIcon = new Icon(pRenderer, selectedPulldownTexture);
	m_pPulldownHoverIcon = new Icon(pRenderer, hoverPulldownTexture);
	m_pPulldownDisabledIcon = new Icon(pRenderer, disabledPulldownTexture);

	m_pMenuBackgroundDefault = new Icon(pRenderer, defaultMenuTexture);
	m_pMenuBackgroundHover = new Icon(pRenderer, hoverMenuTexture);
	m_pMenuBackgroundSelected = new Icon(pRenderer, selectedMenuTexture);
	m_pMenuBackgroundDisabled = new Icon(pRenderer, disabledMenuTexture);

	m_pTitleBackgroundDefault = new Icon(pRenderer, defaultTitleTexture);
	m_pTitleBackgroundHover = new Icon(pRenderer, hoverTitleTexture);
	m_pTitleBackgroundSelected = new Icon(pRenderer, selectedTitleTexture);
	m_pTitleBackgroundDisabled = new Icon(pRenderer, disabledTitleTexture);

	m_scrollBar.SetScrollDirection(ScrollBarDirection::Vertical);
	m_scrollBar.SetScrollPosition(0.75f);

	Container::Add(m_pPulldownIcon);
	Container::Add(m_pPulldownSelectedIcon);
	Container::Add(m_pPulldownHoverIcon);
	Container::Add(m_pPulldownDisabledIcon);
	Container::Add(m_pMenuBackgroundDefault);
	Container::Add(m_pMenuBackgroundHover);
	Container::Add(m_pMenuBackgroundSelected);
	Container::Add(m_pMenuBackgroundDisabled);
	Container::Add(m_pTitleBackgroundDefault);
	Container::Add(m_pTitleBackgroundHover);
	Container::Add(m_pTitleBackgroundSelected);
	Container::Add(m_pTitleBackgroundDisabled);

	m_pulldownMenu.SetPullDownMenuParent(this);

	Container::Add(&m_pulldownMenu);

	Container::Add(&m_scrollBar);

	m_menuTitle.SetLocation(0, 0);
	Container::Add(&m_menuTitle);

	m_renderMenuHeader = false;
	m_menuName.SetLocation(0, 0);
	Container::Add(&m_menuName);

	//m_pulldownMenu.SetAutoSizeMenuWidth(false);

	m_GUIFont = GUIFont;

	m_outline = false;

	m_numItems = 0;
	m_selectedIndex = -1;
	m_maxDisplayItems = 0;

	m_isDisplayScrollbar = true;

	m_pSelectedMenuItem = nullptr;

	m_menuItemChangedCallback = nullptr;
	m_pMenuItemChangedCallbackData = nullptr;
}

PulldownMenu::~PulldownMenu()
{
	// Delete the menu items that we have dynamically created
	while (!m_vpMenuItemList.empty())
	{
		MenuItem* pItem = m_vpMenuItemList.back();
		delete pItem;
		m_vpMenuItemList.pop_back();
	}

	// Delete the pull-down icons
	delete m_pPulldownIcon;
	delete m_pPulldownSelectedIcon;
	delete m_pPulldownHoverIcon;
	delete m_pPulldownDisabledIcon;

	delete m_pMenuBackgroundDefault;
	delete m_pMenuBackgroundHover;
	delete m_pMenuBackgroundSelected;
	delete m_pMenuBackgroundDisabled;

	delete m_pTitleBackgroundDefault;
	delete m_pTitleBackgroundHover;
	delete m_pTitleBackgroundSelected;
	delete m_pTitleBackgroundDisabled;
}

void PulldownMenu::RemoveIcon(RenderRectangle* pRemoveIcon)
{
	if (pRemoveIcon)
	{
		Remove(pRemoveIcon);
		delete pRemoveIcon;
	}
}

void PulldownMenu::SetDefaultIcons(Renderer* pRenderer)
{
	RemoveIcon(m_pPulldownIcon);
	RemoveIcon(m_pPulldownHoverIcon);
	RemoveIcon(m_pPulldownSelectedIcon);
	RemoveIcon(m_pPulldownDisabledIcon);

	RemoveIcon(m_pMenuBackgroundDefault);
	RemoveIcon(m_pMenuBackgroundHover);
	RemoveIcon(m_pMenuBackgroundSelected);
	RemoveIcon(m_pMenuBackgroundDisabled);

	RemoveIcon(m_pTitleBackgroundDefault);
	RemoveIcon(m_pTitleBackgroundHover);
	RemoveIcon(m_pTitleBackgroundSelected);
	RemoveIcon(m_pTitleBackgroundDisabled);

	m_pPulldownIcon = new DirectDrawRectangle(pRenderer);
	m_pPulldownHoverIcon = new DirectDrawRectangle(pRenderer);
	m_pPulldownSelectedIcon = new DirectDrawRectangle(pRenderer);
	m_pPulldownDisabledIcon = new DirectDrawRectangle(pRenderer);

	m_pMenuBackgroundDefault = new DirectDrawRectangle(pRenderer);
	m_pMenuBackgroundHover = new DirectDrawRectangle(pRenderer);
	m_pMenuBackgroundSelected = new DirectDrawRectangle(pRenderer);
	m_pMenuBackgroundDisabled = new DirectDrawRectangle(pRenderer);

	m_pTitleBackgroundDefault = new DirectDrawRectangle(pRenderer);
	m_pTitleBackgroundHover = new DirectDrawRectangle(pRenderer);
	m_pTitleBackgroundSelected = new DirectDrawRectangle(pRenderer);
	m_pTitleBackgroundDisabled = new DirectDrawRectangle(pRenderer);

	DirectDrawRectangle* pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pPulldownIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.52f, 0.53f, 0.91f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pPulldownHoverIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pPulldownSelectedIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pPulldownDisabledIcon);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pMenuBackgroundDefault);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pMenuBackgroundHover);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pMenuBackgroundSelected);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pMenuBackgroundDisabled);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pTitleBackgroundDefault);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pTitleBackgroundHover);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.0f, 0.0f, 0.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pTitleBackgroundSelected);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.25f, 0.25f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(1.0f, 1.0f, 1.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.0f, 0.0f, 0.0f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(1.0f, 1.0f, 1.0f, 1.0f));

	pDirectDrawRect = static_cast<DirectDrawRectangle*>(m_pTitleBackgroundDisabled);
	pDirectDrawRect->SetBackgroundColorTopLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorTopRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomLeft(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetBackgroundColorBottomRight(Color(0.75f, 0.75f, 0.75f, 1.0f));
	pDirectDrawRect->SetOutlineColorTop(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorBottom(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorLeft(Color(0.25f, 0.25f, 0.25f, 1.0f));
	pDirectDrawRect->SetOutlineColorRight(Color(0.25f, 0.25f, 0.25f, 1.0f));

	Add(m_pPulldownIcon);
	Add(m_pPulldownSelectedIcon);
	Add(m_pPulldownHoverIcon);
	Add(m_pPulldownDisabledIcon);
	Add(m_pMenuBackgroundDefault);
	Add(m_pMenuBackgroundHover);
	Add(m_pMenuBackgroundSelected);
	Add(m_pMenuBackgroundDisabled);
	Add(m_pTitleBackgroundDefault);
	Add(m_pTitleBackgroundHover);
	Add(m_pTitleBackgroundSelected);
	Add(m_pTitleBackgroundDisabled);

	// Set the dimensions of the icons
	Dimensions d = GetDimensions();
	m_pPulldownIcon->SetDimensions(d.width - d.height, 0, d.height, d.height);
	m_pPulldownSelectedIcon->SetDimensions(d.width - d.height, 0, d.height, d.height);
	m_pPulldownHoverIcon->SetDimensions(d.width - d.height, 0, d.height, d.height);
	m_pPulldownDisabledIcon->SetDimensions(d.width - d.height, 0, d.height, d.height);

	// Also set the dimensions of the title background icons
	m_pTitleBackgroundDefault->SetDimensions(0, 0, d.width - d.height, d.height);
	m_pTitleBackgroundHover->SetDimensions(0, 0, d.width - d.height, d.height);
	m_pTitleBackgroundSelected->SetDimensions(0, 0, d.width - d.height, d.height);
	m_pTitleBackgroundDisabled->SetDimensions(0, 0, d.width - d.height, d.height);

	// Also set the dimensions of the title label
	m_menuTitle.SetLocation(3, 3 /*m_pulldownMenu.GetMenuItemSpacer(), m_pulldownMenu.GetMenuItemSpacer()*/);

	// Also set the dimensions of the header label
	int lHeaderWidth = m_pRenderer->GetFreeTypeTextWidth(m_GUIFont, "%s", m_menuName.GetText().c_str());
	m_menuName.SetLocation(-5 - lHeaderWidth, 3);

	Dimensions d_Scrollbar = m_scrollBar.GetDimensions();
	m_pMenuBackgroundDefault->SetDimensions(0, -d_Scrollbar.height, d.width - d.height, d_Scrollbar.height);
	m_pMenuBackgroundDisabled->SetDimensions(0, -d_Scrollbar.height, d.width - d.height, d_Scrollbar.height);

	m_pulldownMenu.SetDepth(GetDepth() + 1.0f);
	m_menuTitle.SetDepth(GetDepth() + 1.0f);

	// Also set the default icons of the scrollbar
	m_scrollBar.SetDefaultIcons(pRenderer);
}

void PulldownMenu::SetPulldownIconDefault(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pPulldownIcon)
	{
		// If we already own a default icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pPulldownIcon);

		delete m_pPulldownIcon;
		m_pPulldownIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pPulldownIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pPulldownIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pPulldownIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pPulldownIcon);

	Dimensions dimensions = GetDimensions();
	int lWidth = m_pulldownMenu.GetDimensions().width;
	m_pPulldownIcon->SetDimensions(lWidth, 0, dimensions.height, dimensions.height);
}

void PulldownMenu::SetPulldownIconHover(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pPulldownHoverIcon)
	{
		// If we already own a default icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pPulldownHoverIcon);

		delete m_pPulldownHoverIcon;
		m_pPulldownHoverIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pPulldownHoverIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pPulldownHoverIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pPulldownHoverIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pPulldownHoverIcon);

	Dimensions dimensions = GetDimensions();
	int width = m_pulldownMenu.GetDimensions().width;
	m_pPulldownHoverIcon->SetDimensions(width, 0, dimensions.height, dimensions.height);
}

void PulldownMenu::SetPulldownIconPressed(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pPulldownSelectedIcon)
	{
		// If we already own a default icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pPulldownSelectedIcon);

		delete m_pPulldownSelectedIcon;
		m_pPulldownSelectedIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pPulldownSelectedIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pPulldownSelectedIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pPulldownSelectedIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pPulldownSelectedIcon);

	Dimensions dimensions = GetDimensions();
	int width = m_pulldownMenu.GetDimensions().width;
	m_pPulldownSelectedIcon->SetDimensions(width, 0, dimensions.height, dimensions.height);
}

void PulldownMenu::SetPulldownIconDisabled(RenderRectangle *icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pPulldownDisabledIcon)
	{
		// If we already own a default icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pPulldownDisabledIcon);

		delete m_pPulldownDisabledIcon;
		m_pPulldownDisabledIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pPulldownDisabledIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pPulldownDisabledIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pPulldownDisabledIcon = pNewDirectDraw;
	}

	// Re-add this icon to the component list
	Add(m_pPulldownDisabledIcon);

	Dimensions dimensions = GetDimensions();
	int lWidth = m_pulldownMenu.GetDimensions().width;
	m_pPulldownDisabledIcon->SetDimensions(lWidth, 0, dimensions.height, dimensions.height);
}

unsigned int PulldownMenu::GetGUIFont() const
{
	return m_GUIFont;
}

void PulldownMenu::SetDimensions(int x, int y, int width, int height)
{
	// Set the dimensions of the actual menu
	m_pulldownMenu.SetDimensions(0, 0, width, height);

	width += m_pulldownMenu.GetMenuItemSpacer() * 2;
	height += m_pulldownMenu.GetMenuItemSpacer() * 2;

	Component::SetDimensions(x, y, width + height, height);

	// Also set the dimensions of the pull-down icons
	m_pPulldownIcon->SetDimensions(width, 0, height, height);
	m_pPulldownSelectedIcon->SetDimensions(width, 0, height, height);
	m_pPulldownHoverIcon->SetDimensions(width, 0, height, height);
	m_pPulldownDisabledIcon->SetDimensions(width, 0, height, height);

	// Also set the dimensions of the title background icons
	m_pTitleBackgroundDefault->SetDimensions(0, 0, width, height);
	m_pTitleBackgroundHover->SetDimensions(0, 0, width, height);
	m_pTitleBackgroundSelected->SetDimensions(0, 0, width, height);
	m_pTitleBackgroundDisabled->SetDimensions(0, 0, width, height);

	// Also set the dimensions of the menu background icons

	// Also set the dimensions of the title label
	//m_menuTitle.SetLocation(m_pulldownMenu.GetMenuItemSpacer(), m_pulldownMenu.GetMenuItemSpacer());
	m_menuTitle.SetLocation(3, 3);

	// Also set the dimensions of the header label
	int lHeaderWidth = m_pRenderer->GetFreeTypeTextWidth(m_GUIFont, "%s", m_menuName.GetText().c_str());
	m_menuName.SetLocation(-5 - lHeaderWidth, 3);
}

void PulldownMenu::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void PulldownMenu::AddEventListeners()
{
	AddMouseListener(this);
	AddFocusListener(this);

	// Also add all children's event listeners
	Container::AddEventListeners();

	//m_scrollBar.AddEventListeners();
}

void PulldownMenu::RemoveEventListeners()
{
	RemoveMouseListener(this);
	RemoveFocusListener(this);

	// Also remove all children's event listeners
	Container::RemoveEventListeners();

	//m_scrollBar.RemoveEventListeners();

	// Remove hover and select properties, since we have made this component un-selectable by the mouse
	SetHover(false);
	SetSelected(false);
}

void PulldownMenu::AddPulldownItem(const std::string& label)
{
	MenuItem* pNewItem;

	if (m_outline)
	{
		pNewItem = new MenuItem(m_pRenderer, m_GUIFont, m_outlineGUIFont, label, m_textColor, m_outlineColor);
	}
	else
	{
		pNewItem = new MenuItem(m_pRenderer, m_GUIFont, label, m_textColor);
	}

	// Set the new item dimensions
	int newItemWidth = m_pRenderer->GetFreeTypeTextWidth(m_GUIFont, "%s", label.c_str());
	int newItemHeight = m_pRenderer->GetFreeTypeTextHeight(m_GUIFont, "%s", label.c_str());
	pNewItem->SetWidth(newItemWidth);
	pNewItem->SetHeight(newItemHeight);

	// Set the new item to have a function callback that occurs when we select this menu item
	pNewItem->SetCallBackFunction(_MenuItemSelected);
	pNewItem->SetCallBackData(pNewItem);
	// Don't use the menu items own background icons
	pNewItem->SetOnlyShowSelectedIcons(true); 
	pNewItem->SetDepth(GetDepth() - 1.0f);

	// TODO: Get this working with depth!!
	//pNewItem->SetHoverIcon(m_pMenuBackgroundHover);
	//pNewItem->SetSelectedIcon(m_pMenuBackgroundSelected);

	m_pulldownMenu.Add(pNewItem, m_isDisplayScrollbar);

	// Increase our num items counter
	m_numItems++;

	// Reset the position of the pull down icons, since we might have increase the width of the initial menu
	int width = m_pulldownMenu.GetDimensions().width;
	int height = m_pulldownMenu.GetDimensions().height;
	m_pPulldownIcon->SetDimensions(width, 0, height, height);
	m_pPulldownSelectedIcon->SetDimensions(width, 0, height, height);
	m_pPulldownHoverIcon->SetDimensions(width, 0, height, height);
	m_pPulldownDisabledIcon->SetDimensions(width, 0, height, height);

	// Reset the dimensions of the title background icons
	m_pTitleBackgroundDefault->SetDimensions(0, 0, width, height);
	m_pTitleBackgroundHover->SetDimensions(0, 0, width, height);
	m_pTitleBackgroundSelected->SetDimensions(0, 0, width, height);
	m_pTitleBackgroundDisabled->SetDimensions(0, 0, width, height);

	// Reset the initial component dimensions
	Component::SetDimensions(GetDimensions().x, GetDimensions().y, width + height, height);

	// Also set the dimensions of the scrollbar, since adding new items increases the height of the pulldown menu
	int pullDownHeight = m_pulldownMenu.GetCurrentMenuheight();
	int pullDownHeightModifiedToMaxDisplay = m_maxDisplayItems * (newItemHeight + (m_pulldownMenu.GetMenuItemSpacer() * 2));
	int heightToUseForScrollbar = pullDownHeight;
	if (m_numItems > m_maxDisplayItems)
	{
		heightToUseForScrollbar = pullDownHeightModifiedToMaxDisplay;
	}
	m_scrollBar.SetDimensions(width, -heightToUseForScrollbar, height, heightToUseForScrollbar);

	// Reset the dimensions of the menu background icons
	int menuItemWidth = width;
	if (!m_isDisplayScrollbar)
	{
		menuItemWidth += height;
	}
	m_pMenuBackgroundDefault->SetDimensions(0, -heightToUseForScrollbar, menuItemWidth, heightToUseForScrollbar);
	// NOT NEEDED: This is the size of a menu item... m_pMenuBackgroundHover->SetDimensions(0, -lHeightToUseForScrollbar, lMenuItemWidth, lHeightToUseForScrollbar);
	// NOT NEEDED: This is the size of a menu item... m_pMenuBackgroundSelected->SetDimensions(0, -lHeightToUseForScrollbar, lMenuItemWidth, lHeightToUseForScrollbar);
	m_pMenuBackgroundDisabled->SetDimensions(0, -heightToUseForScrollbar, menuItemWidth, heightToUseForScrollbar);

	UpdateScrollbar();

	// Check a vector list of all the added items, so we can delete them later, since they are created dynamically
	m_vpMenuItemList.push_back(pNewItem);
}

int PulldownMenu::GetNumItems() const
{
	return m_numItems;
}

int PulldownMenu::GetSelectedItem() const
{
	return m_selectedIndex;
}

void PulldownMenu::RemoveAllPullDownMenuItems()
{
	m_pulldownMenu.RemoveAll();
	m_pulldownMenu.ResetMenu();

	while (!m_vpMenuItemList.empty())
	{
		MenuItem* pItem = m_vpMenuItemList.back();

		delete pItem;
		m_vpMenuItemList.pop_back();
	}

	m_numItems = 0;
	m_selectedIndex = -1;

	m_isDisplayScrollbar = true;

	m_pSelectedMenuItem = nullptr;
}

void PulldownMenu::ResetPullDownMenu()
{
	m_selectedIndex = -1;
	m_pSelectedMenuItem = nullptr;

	m_menuTitle.SetText(m_menuName.GetText());

	// Reset the dimensions of the scrollbar - since this won't get updated until we add a new menu item later
	m_scrollBar.SetDimensions(0, 0, 0, 0);
	m_pMenuBackgroundDefault->SetDimensions(0, 0, 0, 0);
	m_pMenuBackgroundDisabled->SetDimensions(0, 0, 0, 0);
}

void PulldownMenu::SetSelectedItem(const std::string& label)
{
	m_pSelectedMenuItem = m_pulldownMenu.FindItem(label);

	if (m_pSelectedMenuItem != nullptr)
	{
		SetTitleString(m_pSelectedMenuItem->GetLabel().GetText());
	}
}

void PulldownMenu::SetSelectedMenuItem(MenuItem* lpMenuItem)
{
	m_pSelectedMenuItem = lpMenuItem;
}

MenuItem* PulldownMenu::GetSelectedMenuItem() const
{
	return m_pSelectedMenuItem;
}

void PulldownMenu::SetMaxNumItemsDisplayed(int lMaxDisplay)
{
	m_maxDisplayItems = lMaxDisplay;
}

int PulldownMenu::GetMaxNumItemsDisplayed() const
{
	return m_maxDisplayItems;
}

void PulldownMenu::UpdateScrollbar()
{
	if (m_numItems > m_maxDisplayItems)
	{
		float scrollRatio = static_cast<float>(m_maxDisplayItems) / static_cast<float>(m_numItems);
		m_scrollBar.SetScrollSize(scrollRatio);
		m_scrollBar.SetScrollPosition(1.0f);

		m_scrollBar.SetDisabled(false);
	}
	else
	{
		m_scrollBar.SetDisabled(true);
	}
}

void PulldownMenu::SetTitleString(const std::string& label)
{
	m_menuTitle.SetText(label);
}

void PulldownMenu::SetRenderHeader(bool render)
{
	m_renderMenuHeader = render;
}

void PulldownMenu::SetSelected(bool val)
{
	m_isSelected = val;
	m_pulldownMenu.SetSelected(val);
}

void PulldownMenu::SetHover(bool val)
{
	m_isHover = val;
	m_pulldownMenu.SetHover(val);
}

void PulldownMenu::SetDisabled(bool val)
{
	m_isDisabled = val;
	m_pulldownMenu.SetDisabled(val);

	if (m_isDisabled)
	{
		SetSelected(false);
		SetHover(false);
	}
}

bool PulldownMenu::IsSelected() const
{
	return m_isSelected;
}

bool PulldownMenu::IsHover() const
{
	return m_isHover;
}

bool PulldownMenu::IsDisabled() const
{
	return m_isDisabled;
}

void PulldownMenu::SetDisplayScrollBar(bool show)
{
	m_isDisplayScrollbar = show;

	if (m_isDisplayScrollbar)
	{
		Add(&m_scrollBar);

		//m_scrollBar.SetVisible(true);
		//m_scrollBar.SetEnabled(true);
	}
	else
	{
		Remove(&m_scrollBar);

		//m_scrollBar.SetVisible(false);
		//m_scrollBar.SetEnabled(false);
	}
}

void PulldownMenu::OpenMenu()
{
	m_pulldownMenu.OpenMenu();
}

void PulldownMenu::CloseMenu()
{
	m_pulldownMenu.CloseMenu();
}

bool PulldownMenu::IsMenuOpen() const
{
	return m_pulldownMenu.IsMenuOpen();
}

bool PulldownMenu::ChildHasFocus()
{
	if (m_pulldownMenu.ChildHasFocus() || m_scrollBar.HasFocus())
	{
		return true;
	}

	return false;
}

bool PulldownMenu::MenuChildHasFocus()
{
	if (m_pulldownMenu.ChildHasFocus())
	{
		return true;
	}

	return false;
}

ComponentType PulldownMenu::GetComponentType() const
{
	return ComponentType::PulldownMenu;
}

ScrollBar* PulldownMenu::GetPulldownScrollBar()
{
	return &m_scrollBar;
}

void PulldownMenu::MenuItemChanged() const
{
	if (m_menuItemChangedCallback)
	{
		m_menuItemChangedCallback(m_pMenuItemChangedCallbackData);
	}
}

void PulldownMenu::SetMenuItemChangedCallBackFunction(FunctionCallback func)
{
	m_menuItemChangedCallback = func;
}

void PulldownMenu::SetMenuItemChangedCallBackData(void* pData)
{
	m_pMenuItemChangedCallbackData = pData;
}

void PulldownMenu::MouseEntered(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetHover(true);

	OnMouseEnter();
}

void PulldownMenu::MouseExited(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetHover(false);

	// Also removed the button selection if we exit it's dimensions
	SetSelected(false);

	OnMouseExit();
}

void PulldownMenu::MousePressed(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	if (m_pulldownMenu.IsMenuOpen())
	{
		SetSelected(false);

		m_pulldownMenu.CloseMenu();
	}
	else
	{
		SetSelected(true);

		m_pulldownMenu.OpenMenu();
	}

	// If our parent is a GUIWindow, then makew this window have focus in the GUI, used to make it's depth the highest
	if (GetParent() != nullptr && GetParent()->GetComponentType() == ComponentType::GUIWindow)
	{
		GUIWindow* pParentWindow = static_cast<GUIWindow*>(GetParent());
		pParentWindow->SetFocusWindow();
	}

	OnMousePressed();
}

void PulldownMenu::MouseReleased(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	SetSelected(false);

	OnMouseReleased();
}

void PulldownMenu::MouseReleasedOutside(const MouseEvent& event)
{
	// Do nothing
}

void PulldownMenu::MouseClicked(const MouseEvent& event)
{
	if (IsDisabled())
	{
		return;
	}

	OnMouseClicked();
}

void PulldownMenu::FocusLost(const FocusEvent& event)
{
	// Do nothing
}

void PulldownMenu::FocusGained(const FocusEvent& event)
{
	// Do nothing
}

void PulldownMenu::OnMouseEnter()
{
	// Do nothing
}

void PulldownMenu::OnMouseExit()
{
	// Do nothing
}

void PulldownMenu::OnMousePressed()
{
	// Do nothing
}

void PulldownMenu::OnMouseReleased()
{
	// Do nothing
}

void PulldownMenu::OnMouseClicked()
{
	// Do nothing
}

void PulldownMenu::DrawChildren()
{
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_GUIFont, "%s", m_pulldownMenu.GetMenuTitle().c_str());
	int menuHeight = textHeight + (m_pulldownMenu.GetMenuItemSpacer() * 2);
	int fullMenuDisplayHeight = m_maxDisplayItems * menuHeight;
	
	// Do a scissor test here
	Point location = GetLocation();
	for (Component* parent = GetParent(); parent != nullptr;)
	{
		Point parentLocation = parent->GetLocation();

		location.x += parentLocation.x;
		location.y += parentLocation.y;

		parent = parent->GetParent();
	}

	int scissorX = location.x;
	int scissorY = location.y - fullMenuDisplayHeight;
	int scissorWidth = m_pulldownMenu.GetBiggestWidth() + (m_pulldownMenu.GetMenuItemSpacer() * 2);
	int scissorHeight = fullMenuDisplayHeight;
	//int depth = static_cast<int>(GetDepth());
		
	// DEBUG: Draw the scissor region
	//m_pRenderer->PushMatrix();

	//m_pRenderer->SetLineWidth(1.0f);
	//m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);

	//m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINE_LOOP);
	//m_pRenderer->ImmediateVertex((scissorX - GetLocation().x),						(scissorY - GetLocation().y),					depth);
	//m_pRenderer->ImmediateVertex((scissorX - GetLocation().x) + scissorWidth,		(scissorY - GetLocation().y),					depth);
	//m_pRenderer->ImmediateVertex((scissorX - GetLocation().x) + scissorWidth + 1,	(scissorY - GetLocation().y ) + scissorHeight,	depth);
	//m_pRenderer->ImmediateVertex((scissorX - GetLocation().x),						(scissorY - GetLocation().y ) + scissorHeight,	depth);
	//
	//m_pRenderer->DisableImmediateMode();
	//
	//m_pRenderer->PopMatrix();
	
	bool isNeedScissor = false;

	// Modify the menu position, depending on where the scrollbar is
	if (m_numItems > m_maxDisplayItems)
	{
		float missingAmount = 1.0f - m_scrollBar.GetScrollSize();
		float scrollPositionRelative = m_scrollBar.GetScrollPosition() - m_scrollBar.GetScrollSize() * 0.5f;
		float modifiedScrollPosition = 1.0f - scrollPositionRelative / missingAmount;
		int fullHeight = m_numItems * menuHeight;
		int seenHeight = fullHeight - fullMenuDisplayHeight;
		float modifiedY = modifiedScrollPosition * static_cast<float>(seenHeight) - menuHeight;
		m_pulldownMenu.SetLocation(0, static_cast<int>(modifiedY));

		isNeedScissor = true;
	}
	else
	{
		int modifiedY = -menuHeight;
		m_pulldownMenu.SetLocation(0, modifiedY);
	}
	
	//Container::DrawChildren();

	// NOTE: We have to do this manually, since we only want to scissor test the menu child

	// This vector list is depth sorted, so we render in depth order, starting with the lowest depth first
	if (IsContainer() && HasChildren() && IsVisible())
	{
		// Draw all children
		for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
		{
			if ((*iter)->IsVisible())
			{
				if (isNeedScissor && (*iter)->GetComponentType() == ComponentType::Menu)
				{
					m_pRenderer->EnableScissorTest(scissorX, scissorY, scissorWidth, scissorHeight);;
				}

				(*iter)->Draw();

				if (isNeedScissor && (*iter)->GetComponentType() == ComponentType::Menu)
				{
					m_pRenderer->DisableScissorTest();
				}
			}
		}
	}
}

void PulldownMenu::DrawSelf()
{
	//Container::DrawSelf();

	m_menuName.SetVisible(m_renderMenuHeader);

	// Reset visibility
	m_pPulldownIcon->SetVisible(false);
	m_pPulldownHoverIcon->SetVisible(false);
	m_pPulldownSelectedIcon->SetVisible(false);
	m_pPulldownDisabledIcon->SetVisible(false);

	m_pTitleBackgroundDefault->SetVisible(false);
	m_pTitleBackgroundHover->SetVisible(false);
	m_pTitleBackgroundSelected->SetVisible(false);
	m_pTitleBackgroundDisabled->SetVisible(false);

	m_pMenuBackgroundDefault->SetVisible(false);
	m_pMenuBackgroundHover->SetVisible(false);
	m_pMenuBackgroundSelected->SetVisible(false);
	m_pMenuBackgroundDisabled->SetVisible(false);

	if (m_pulldownMenu.IsMenuOpen())
	{
		m_scrollBar.SetVisible(true);
		m_scrollBar.SetEnabled(true);

		if (IsDisabled())
		{
			m_pMenuBackgroundDisabled->SetVisible(true);
		}
		else
		{
			m_pMenuBackgroundDefault->SetVisible(true);
		}
	}
	else
	{
		m_scrollBar.SetVisible(false);
		m_scrollBar.SetEnabled(false);
	}

	if (IsSelected())
	{
		m_pPulldownSelectedIcon->SetVisible(true);
		m_pTitleBackgroundSelected->SetVisible(true);
	}
	else if (IsHover())
	{
		m_pPulldownHoverIcon->SetVisible(true);
		m_pTitleBackgroundHover->SetVisible(true);
	}
	else if (IsDisabled())
	{
		m_pPulldownDisabledIcon->SetVisible(true);
		m_pTitleBackgroundDisabled->SetVisible(true);
	}
	else
	{
		m_pPulldownIcon->SetVisible(true);
		m_pTitleBackgroundDefault->SetVisible(true);
	}
}

void PulldownMenu::_MenuItemSelected(void* pData)
{
	MenuItem* pMenuItem = static_cast<MenuItem*>(pData);

	if (pMenuItem)
	{
		pMenuItem->MenuItemSelectedFromPulldowmMenu();
	}
}