/*************************************************************************
> File Name: QuestGUI.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Quest GUI class.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "QuestGUI.h"
#include "CubbyGame.h"

// Constructor, Destructor
QuestGUI::QuestGUI(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight)
{
	m_pRenderer = pRenderer;
	m_pGUI = pGUI;
	m_pFrontendManager = pFrontendManager;
	m_pChunkManager = pChunkManager;
	m_pPlayer = pPlayer;
	m_pInventoryManager = pInventoryManager;

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	// Quest Window
	m_pQuestLogWindow = new GUIWindow(m_pRenderer, m_pFrontendManager->GetFrontendFontMedium(), "Quest Log");
	m_pQuestLogWindow->AllowMoving(true);
	m_pQuestLogWindow->AllowClosing(false);
	m_pQuestLogWindow->AllowMinimizing(false);
	m_pQuestLogWindow->AllowScrolling(false);
	m_pQuestLogWindow->SetRenderTitleBar(true);
	m_pQuestLogWindow->SetRenderWindowBackground(true);
	m_pQuestLogWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pQuestLogWindow->Hide();

	m_pTitleBarIcon = new Icon(m_pRenderer, "", 44, 44);
	m_pTitleBarIcon->SetDepth(4.0f);

	m_pQuestLogWindowBackgroundIcon = new Icon(m_pRenderer, "", 292, 364);
	m_pQuestLogWindowBackgroundIcon->SetDepth(1.0f);

	m_pTitleBarBackgroundIcon = new Icon(m_pRenderer, "", 133, 35);
	m_pTitleBarBackgroundIcon->SetDepth(1.0f);

	m_pCloseExitButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCloseExitButton->SetLabelOffset(0, 5);
	m_pCloseExitButton->SetCallBackFunction(_CloseExitPressed);
	m_pCloseExitButton->SetCallBackData(this);

	char titleText[] = "";
	m_pQuestTitleHeaderLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont40(), titleText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pQuestTitleHeaderLabel->SetOutline(true);
	m_pQuestTitleHeaderLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pQuestTitleHeaderLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont40Outline());
	m_pQuestTitleHeaderLabel->SetDepth(4.0f);

	char questLongText[] = "";
	m_pQuestTextLabel = new FormattedLabel(m_pRenderer, m_pFrontendManager->GetFrontendFont22(), m_pFrontendManager->GetFrontendFont22(), m_pFrontendManager->GetFrontendFont22(), questLongText);
	m_pQuestTextLabel->SetOutline(true);
	m_pQuestTextLabel->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pQuestTextLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pQuestTextLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont22Outline());
	m_pQuestTextLabel->SetDepth(3.0f);
	m_pQuestTextLabel->SetWordWrap(true);

	m_pTabOpenCloseButtonIconRight = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconRight->SetDepth(1.0f);

	m_pTabOpenCloseButtonIconRightHover = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconRightHover->SetDepth(1.0f);

	m_pTabOpenCloseButtonIconRightPressed = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconRightPressed->SetDepth(1.0f);

	m_pTabOpenCloseButtonIconLeft = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconLeft->SetDepth(1.0f);

	m_pTabOpenCloseButtonIconLeftHover = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconLeftHover->SetDepth(1.0f);

	m_pTabOpenCloseButtonIconLeftPressed = new Icon(m_pRenderer, "", 12, 80);
	m_pTabOpenCloseButtonIconLeftPressed->SetDepth(1.0f);

	m_pQuestsTabIconRight = new Icon(m_pRenderer, "", 164, 276);
	m_pQuestsTabIconRight->SetDepth(0.5f);
	m_pQuestsTabIconLeft = new Icon(m_pRenderer, "", 164, 276);
	m_pQuestsTabIconLeft->SetDepth(0.5f);

	m_pTabOpenCloseButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), m_pFrontendManager->GetFrontendFont30Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pTabOpenCloseButton->SetDefaultIcon(m_pTabOpenCloseButtonIconRight);
	m_pTabOpenCloseButton->SetHoverIcon(m_pTabOpenCloseButtonIconRightHover);
	m_pTabOpenCloseButton->SetSelectedIcon(m_pTabOpenCloseButtonIconRightPressed);
	m_pTabOpenCloseButton->SetDisabledIcon(m_pTabOpenCloseButtonIconRight);
	m_pTabOpenCloseButton->SetLabelOffset(0, 5);
	m_pTabOpenCloseButton->SetCallBackFunction(_OpenCloseTabPressed);
	m_pTabOpenCloseButton->SetCallBackData(this);

	m_pQuestListScrollbar = new ScrollBar(m_pRenderer);
	m_pQuestListScrollbar->SetScrollDirection(ScrollBarDirection::Vertical);
	m_pQuestListScrollbar->SetScrollSize(0.25f);
	m_pQuestListScrollbar->SetScrollPosition(1.0f);
	m_pQuestListScrollbar->SetDepth(4.0f);
	m_pQuestListScrollbar->SetScissorEnabled(true);

	char currentQuestsText[] = "Current Quests";
	m_pCurrentQuestsHeaderLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont22(), currentQuestsText, Color(0.89f, 0.90f, 0.1f, 1.0f));
	m_pCurrentQuestsHeaderLabel->SetOutline(true);
	m_pCurrentQuestsHeaderLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCurrentQuestsHeaderLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont22Outline());
	m_pCurrentQuestsHeaderLabel->SetDepth(4.0f);

	char completedQuestsText[] = "Completed Quests";
	m_pCompletedQuestsHeaderLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont22(), completedQuestsText, Color(0.89f, 0.90f, 0.1f, 1.0f));
	m_pCompletedQuestsHeaderLabel->SetOutline(true);
	m_pCompletedQuestsHeaderLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pCompletedQuestsHeaderLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont22Outline());
	m_pCompletedQuestsHeaderLabel->SetDepth(4.0f);

	m_pAlphaButton = new Icon(m_pRenderer, "", 4, 4);
	m_pAlphaButton->SetDepth(0.5f);

	m_pQuestLogWindow->SetBackgroundIcon(m_pQuestLogWindowBackgroundIcon);
	m_pQuestLogWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);
	m_pQuestLogWindow->AddComponent(m_pCloseExitButton);
	m_pQuestLogWindow->AddComponent(m_pTitleBarIcon);
	m_pQuestLogWindow->AddComponent(m_pQuestTitleHeaderLabel);
	m_pQuestLogWindow->AddComponent(m_pQuestTextLabel);
	m_pQuestLogWindow->AddComponent(m_pQuestsTabIconRight);
	m_pQuestLogWindow->AddComponent(m_pQuestsTabIconLeft);
	m_pQuestLogWindow->AddComponent(m_pTabOpenCloseButton);

	SetWindowDimensions(m_windowWidth, m_windowHeight);

	m_questTabsOpen = false;
	m_questTabLeftSide = false;
	m_addedQuestListScrollbar = false;

	// Load delay
	m_loadDelay = false;
	m_loadDelayTime = 0.0f;

	m_loaded = false;
}

QuestGUI::~QuestGUI()
{
	delete m_pQuestLogWindow;

	DeleteQuestButtons();

	delete m_pTitleBarIcon;
	delete m_pTitleBarBackgroundIcon;
	delete m_pQuestLogWindowBackgroundIcon;
	delete m_pCloseExitButton;
	delete m_pQuestTitleHeaderLabel;
	delete m_pQuestTextLabel;
	delete m_pTabOpenCloseButtonIconLeft;
	delete m_pTabOpenCloseButtonIconLeftHover;
	delete m_pTabOpenCloseButtonIconLeftPressed;
	delete m_pTabOpenCloseButtonIconRight;
	delete m_pTabOpenCloseButtonIconRightHover;
	delete m_pTabOpenCloseButtonIconRightPressed;
	delete m_pTabOpenCloseButton;
	delete m_pQuestsTabIconRight;
	delete m_pQuestsTabIconLeft;
	delete m_pCurrentQuestsHeaderLabel;
	delete m_pCompletedQuestsHeaderLabel;
	delete m_pQuestListScrollbar;
	delete m_pAlphaButton;
}

void QuestGUI::SetQuestJournal(QuestJournal* pQuestJournal)
{
	m_pQuestJournal = pQuestJournal;
}

void QuestGUI::SetWindowDimensions(int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	m_questWindowWidth = 292;
	m_questWindowHeight = 364;
	m_titlebarWidth = 153;
	m_titlebarHeight = 35;
	m_questTabOpenButtonWidth = 12;
	m_questTabOpenButtonHeight = 80;
	m_questTabWidth = 164;
	m_questTabHeight = 276;

	m_pQuestLogWindow->SetDimensions(50, 250, m_questWindowWidth, m_questWindowHeight);
	m_pQuestLogWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);
	m_pQuestLogWindow->SetTitleOffset(50, 5);
	m_pQuestLogWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	m_pQuestLogWindow->SetApplicationBorder(25, 15, 10, 10);

	m_pTitleBarIcon->SetDimensions(0, m_questWindowHeight, 44, 44);

	m_pCloseExitButton->SetDimensions(m_questWindowWidth - 32, m_questWindowHeight, 32, 32);

	m_pQuestTitleHeaderLabel->SetLocation(20, 315);
	m_pQuestTextLabel->SetDimensions(20, 150, 250, 160);

	m_pTabOpenCloseButton->SetDimensions(-m_questTabOpenButtonWidth, (m_questWindowHeight / 2) - (m_questTabOpenButtonHeight / 2), m_questTabOpenButtonWidth, m_questTabOpenButtonHeight);
	m_pQuestsTabIconLeft->SetDimensions(-m_questTabWidth, (m_questWindowHeight / 2) - (m_questTabHeight / 2), m_questTabWidth, m_questTabHeight);
	m_pQuestsTabIconRight->SetDimensions(0, (m_questWindowHeight / 2) - (m_questTabHeight / 2), m_questTabWidth, m_questTabHeight);

	// Scrollbar
	int scrollbarWidth = 16;
	int scrollbarHeight = m_questTabHeight - 12;
	m_pQuestListScrollbar->SetDimensions(m_questWindowWidth + m_questTabWidth - scrollbarWidth - 4, (m_questWindowHeight / 2) - (m_questTabHeight / 2) + 8, scrollbarWidth, scrollbarHeight);

	// Quest headers
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_pFrontendManager->GetFrontendFont22(), "%s", m_pCurrentQuestsHeaderLabel->GetText().c_str());
	m_pCurrentQuestsHeaderLabel->SetLocation(m_questWindowWidth + 8, (m_questWindowHeight / 2) + (m_questTabHeight / 2) - textHeight - 4);

	int numActiveQuests = static_cast<int>(m_vpCurrentQuestButtons.size()) + 1;
	m_pCompletedQuestsHeaderLabel->SetLocation(m_questWindowWidth + 8, (m_questWindowHeight / 2) + (m_questTabHeight / 2) - textHeight - (numActiveQuests*textHeight) - 4);
}

// Skinning the GUI
void QuestGUI::SkinGUI()
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/quest_title_icon.tga";
	m_pTitleBarIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/quest_window_background.tga";
	m_pQuestLogWindowBackgroundIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/titlebar_background.tga";
	m_pTitleBarBackgroundIcon->SetIcon(iconName);

	m_pQuestLogWindow->SetBackgroundIcon(m_pQuestLogWindowBackgroundIcon);
	m_pQuestLogWindow->SetTitlebarBackgroundIcon(m_pTitleBarBackgroundIcon);

	Point location = m_pQuestLogWindow->GetLocation();
	m_pQuestLogWindow->SetDimensions(location.x, location.y, m_questWindowWidth, m_questWindowHeight);
	m_pQuestLogWindow->SetTitleBarDimensions(0, 0, m_titlebarWidth, m_titlebarHeight);

	iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/tab_openclose_right.tga";
	m_pTabOpenCloseButtonIconRight->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/tab_openclose_right_hover.tga";
	m_pTabOpenCloseButtonIconRightHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/tab_openclose_right_pressed.tga";
	m_pTabOpenCloseButtonIconRightPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/tab_openclose_left.tga";
	m_pTabOpenCloseButtonIconLeft->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/tab_openclose_left_hover.tga";
	m_pTabOpenCloseButtonIconLeftHover->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/tab_openclose_left_pressed.tga";
	m_pTabOpenCloseButtonIconLeftPressed->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/quests_tab_right.tga";
	m_pQuestsTabIconRight->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/QuestGUI/quests_tab_left.tga";
	m_pQuestsTabIconLeft->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/Common/alpha_button.tga";
	m_pAlphaButton->SetIcon(iconName);

	m_pFrontendManager->SetScrollbarIcons(m_pQuestListScrollbar);

	m_pCloseExitButton->SetDefaultIcon(m_pFrontendManager->GetCloseExitButtonIcon());
	m_pCloseExitButton->SetHoverIcon(m_pFrontendManager->GetCloseExitButtonIconHover());
	m_pCloseExitButton->SetSelectedIcon(m_pFrontendManager->GetCloseExitButtonIconPressed());
	m_pCloseExitButton->SetDisabledIcon(m_pFrontendManager->GetCloseExitButtonIcon());
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void QuestGUI::UnSkinGUI() const
{
	// Do nothing
}

// Loading
void QuestGUI::Load(bool loadDelay, float loadDelayTime)
{
	m_loadDelay = loadDelay;
	m_loadDelayTime = loadDelayTime;

	m_pQuestLogWindow->DepthSortComponentChildren();
	if (m_loadDelay == false)
	{
		m_pGUI->AddWindow(m_pQuestLogWindow);
		m_pQuestLogWindow->Show();
	}

	CreateQuestButtons();

	m_questTabsOpen = false;
	//m_addedQuestListScrollbar = false;
	Update(0.01f);

	m_loaded = true;
}

void QuestGUI::Unload()
{
	m_loadDelay = false;
	m_loadDelayTime = 0.0f;

	m_pGUI->RemoveWindow(m_pQuestLogWindow);

	DeleteQuestButtons();

	m_loaded = false;

	if (CubbyGame::GetInstance()->IsGUIWindowStillDisplayed() == false)
	{
		CubbyGame::GetInstance()->TurnCursorOff(false);
		if (CubbyGame::GetInstance()->ShouldRestorePreviousCameraMode())
		{
			CubbyGame::GetInstance()->RestorePreviousCameraMode();
			CubbyGame::GetInstance()->InitializeCameraRotation();
		}
	}
}

bool QuestGUI::IsLoadDelayed() const
{
	return (m_loadDelay == true && m_loadDelayTime > 0.0f);
}

bool QuestGUI::IsLoaded() const
{
	return m_loaded;
}

void QuestGUI::CreateQuestButtons()
{
	int numCurrentQuests = m_pQuestJournal->GetNumCurrentQuests();
	for (int i = 0; i < numCurrentQuests; ++i)
	{
		Quest* pQuest = m_pQuestJournal->GetCurrentQuest(i);

		Button* pNewButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
		pNewButton->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), pQuest->GetName().c_str(), Color(1.0f, 1.0f, 1.0f, 1.0f), 2, 2, true, Color(0.0f, 0.0f, 0.0f, 1.0f));

		pNewButton->SetDefaultIcon(m_pAlphaButton);
		pNewButton->SetHoverIcon(m_pAlphaButton);
		pNewButton->SetSelectedIcon(m_pAlphaButton);
		pNewButton->SetDisabledIcon(m_pAlphaButton);
		pNewButton->SetPressedOffset(0, -4);
		pNewButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
		pNewButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
		pNewButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
		pNewButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

		int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_pFrontendManager->GetFrontendFont20(), "%s", pQuest->GetName().c_str());
		pNewButton->SetDimensions(0, 0, m_questTabWidth - 32, textHeight);

		QuestButton* pQuestButton = new QuestButton();
		pQuestButton->m_pButton = pNewButton;
		pQuestButton->m_pQuest = pQuest;
		pQuestButton->m_pQuestGUI = this;

		pNewButton->SetCallBackFunction(_QuestButtonPressed);
		pNewButton->SetCallBackData(pQuestButton);

		m_vpCurrentQuestButtons.push_back(pQuestButton);
	}

	int numCompletedQuests = m_pQuestJournal->GetNumCompletedQuests();
	for (int i = 0; i < numCompletedQuests; ++i)
	{
		Quest* pQuest = m_pQuestJournal->GetCompletedQuest(i);

		Button* pNewButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), "", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
		pNewButton->AddText(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), m_pFrontendManager->GetFrontendFont20Outline(), pQuest->GetName().c_str(), Color(0.5f, 0.5f, 0.5f, 1.0f), 2, 2, true, Color(0.0f, 0.0f, 0.0f, 1.0f));

		pNewButton->SetDefaultIcon(m_pAlphaButton);
		pNewButton->SetHoverIcon(m_pAlphaButton);
		pNewButton->SetSelectedIcon(m_pAlphaButton);
		pNewButton->SetDisabledIcon(m_pAlphaButton);
		pNewButton->SetPressedOffset(0, -4);
		pNewButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
		pNewButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
		pNewButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
		pNewButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());

		int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_pFrontendManager->GetFrontendFont20(), "%s", pQuest->GetName().c_str());
		pNewButton->SetDimensions(0, 0, m_questTabWidth - 32, textHeight);

		QuestButton* pQuestButton = new QuestButton();
		pQuestButton->m_pButton = pNewButton;
		pQuestButton->m_pQuest = pQuest;
		pQuestButton->m_pQuestGUI = this;

		pNewButton->SetCallBackFunction(_QuestButtonPressed);
		pNewButton->SetCallBackData(pQuestButton);

		m_vpCompletedQuestButtons.push_back(pQuestButton);
	}
}

void QuestGUI::DeleteQuestButtons()
{
	m_pQuestListScrollbar->ClearScrollAreaItems();

	for (size_t i = 0; i < m_vpCurrentQuestButtons.size(); ++i)
	{
		delete m_vpCurrentQuestButtons[i]->m_pButton;

		delete m_vpCurrentQuestButtons[i];
		m_vpCurrentQuestButtons[i] = nullptr;
	}
	m_vpCurrentQuestButtons.clear();

	for (size_t i = 0; i < m_vpCompletedQuestButtons.size(); ++i)
	{
		delete m_vpCompletedQuestButtons[i]->m_pButton;

		delete m_vpCompletedQuestButtons[i];
		m_vpCompletedQuestButtons[i] = nullptr;
	}
	m_vpCompletedQuestButtons.clear();
}

void QuestGUI::RefreshQuestButtons()
{
	DeleteQuestButtons();
	CreateQuestButtons();
	m_addedQuestListScrollbar = false;
}

void QuestGUI::SetQuestData(std::string questTitle, std::string questText) const
{
	m_pQuestTitleHeaderLabel->SetText(questTitle.c_str());
	m_pQuestTextLabel->SetText(questText.c_str());
}

// Updating
void QuestGUI::Update(float dt)
{
	if (m_loadDelay == true)
	{
		if (m_loadDelayTime <= 0.0f)
		{
			m_loadDelay = false;
			m_pGUI->AddWindow(m_pQuestLogWindow);
			m_pQuestLogWindow->Show();
		}
		else
		{
			m_loadDelayTime -= dt;
		}
	}

	// Update the quest tab left and right sides
	if (m_questTabLeftSide)
	{
		m_pTabOpenCloseButton->SetDefaultIcon(m_pTabOpenCloseButtonIconLeft);
		m_pTabOpenCloseButton->SetHoverIcon(m_pTabOpenCloseButtonIconLeftHover);
		m_pTabOpenCloseButton->SetSelectedIcon(m_pTabOpenCloseButtonIconLeftPressed);
		m_pTabOpenCloseButton->SetDisabledIcon(m_pTabOpenCloseButtonIconLeft);

		if (m_questTabsOpen)
		{
			if (m_addedQuestListScrollbar == false)
			{
				m_pGUI->RemoveWindow(m_pQuestLogWindow);

				m_pQuestLogWindow->AddComponent(m_pQuestListScrollbar);
				m_pQuestListScrollbar->AddScrollAreaItem(m_pCurrentQuestsHeaderLabel);
				m_pQuestListScrollbar->AddScrollAreaItem(m_pCompletedQuestsHeaderLabel);

				for (size_t i = 0; i < m_vpCurrentQuestButtons.size(); ++i)
				{
					m_pQuestListScrollbar->AddScrollAreaItem(m_vpCurrentQuestButtons[i]->m_pButton);
				}
				for (size_t i = 0; i < m_vpCompletedQuestButtons.size(); ++i)
				{
					m_pQuestListScrollbar->AddScrollAreaItem(m_vpCompletedQuestButtons[i]->m_pButton);
				}

				m_addedQuestListScrollbar = true;

				m_pGUI->AddWindow(m_pQuestLogWindow);
				m_pQuestLogWindow->Show();
			}

			m_pTabOpenCloseButton->SetDimensions(-m_questTabWidth - m_questTabOpenButtonWidth, (m_questWindowHeight / 2) - (m_questTabOpenButtonHeight / 2), m_questTabOpenButtonWidth, m_questTabOpenButtonHeight);
			m_pQuestsTabIconLeft->SetDimensions(-m_questTabWidth, (m_questWindowHeight / 2) - (m_questTabHeight / 2), m_questTabWidth, m_questTabHeight);
			m_pQuestsTabIconRight->SetDimensions(0, (m_questWindowHeight / 2) - (m_questTabHeight / 2), m_questTabWidth, m_questTabHeight);
		}
		else
		{
			if (m_addedQuestListScrollbar == true)
			{
				m_pQuestLogWindow->RemoveComponent(m_pQuestListScrollbar);
				m_pQuestLogWindow->RemoveComponent(m_pCurrentQuestsHeaderLabel);
				m_pQuestLogWindow->RemoveComponent(m_pCompletedQuestsHeaderLabel);

				for (size_t i = 0; i < m_vpCurrentQuestButtons.size(); ++i)
				{
					m_pQuestLogWindow->RemoveComponent(m_vpCurrentQuestButtons[i]->m_pButton);
				}
				for (size_t i = 0; i < m_vpCompletedQuestButtons.size(); ++i)
				{
					m_pQuestLogWindow->RemoveComponent(m_vpCompletedQuestButtons[i]->m_pButton);
				}

				m_addedQuestListScrollbar = false;
			}

			m_pTabOpenCloseButton->SetDimensions(-m_questTabOpenButtonWidth, (m_questWindowHeight / 2) - (m_questTabOpenButtonHeight / 2), m_questTabOpenButtonWidth, m_questTabOpenButtonHeight);
			m_pQuestsTabIconLeft->SetDimensions(0, (m_questWindowHeight / 2) - (m_questTabHeight / 2), m_questTabWidth, m_questTabHeight);
			m_pQuestsTabIconRight->SetDimensions(0, (m_questWindowHeight / 2) - (m_questTabHeight / 2), m_questTabWidth, m_questTabHeight);
		}

		// Scrollbar
		int scrollbarWidth = 16;
		int scrollbarHeight = m_questTabHeight - 12;
		m_pQuestListScrollbar->SetDimensions(-scrollbarWidth, (m_questWindowHeight / 2) - (m_questTabHeight / 2) + 8, scrollbarWidth, scrollbarHeight);
		m_pQuestListScrollbar->SetScrollArea(-m_questTabWidth + 20, 0, m_questTabWidth - 20, scrollbarHeight);

		// Quest headers
		int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_pFrontendManager->GetFrontendFont22(), "%s", m_pCurrentQuestsHeaderLabel->GetText().c_str());
		m_pCurrentQuestsHeaderLabel->SetLocation(-(m_questTabWidth) + 24, m_questTabHeight - textHeight - 12);

		int numActiveQuests = static_cast<int>(m_vpCurrentQuestButtons.size()) + 1;
		m_pCompletedQuestsHeaderLabel->SetLocation(-(m_questTabWidth) + 24, m_questTabHeight - textHeight - (numActiveQuests * textHeight) - 12);

		// Quest buttons
		for (size_t i = 0; i < m_vpCurrentQuestButtons.size(); ++i)
		{
			m_vpCurrentQuestButtons[i]->m_pButton->SetLocation(-(m_questTabWidth) + 28, m_questTabHeight - textHeight - ((i + 1) * textHeight) - 12);
		}
		for (size_t i = 0; i < m_vpCompletedQuestButtons.size(); ++i)
		{
			m_vpCompletedQuestButtons[i]->m_pButton->SetLocation(-(m_questTabWidth) + 28, m_questTabHeight - textHeight - ((numActiveQuests + (i + 1)) * textHeight) - 12);
		}
	}
	else
	{
		m_pTabOpenCloseButton->SetDefaultIcon(m_pTabOpenCloseButtonIconRight);
		m_pTabOpenCloseButton->SetHoverIcon(m_pTabOpenCloseButtonIconRightHover);
		m_pTabOpenCloseButton->SetSelectedIcon(m_pTabOpenCloseButtonIconRightPressed);
		m_pTabOpenCloseButton->SetDisabledIcon(m_pTabOpenCloseButtonIconRight);

		if (m_questTabsOpen)
		{
			if (m_addedQuestListScrollbar == false)
			{
				m_pGUI->RemoveWindow(m_pQuestLogWindow);

				m_pQuestLogWindow->AddComponent(m_pQuestListScrollbar);
				m_pQuestListScrollbar->AddScrollAreaItem(m_pCurrentQuestsHeaderLabel);
				m_pQuestListScrollbar->AddScrollAreaItem(m_pCompletedQuestsHeaderLabel);

				for (size_t i = 0; i < m_vpCurrentQuestButtons.size(); ++i)
				{
					m_pQuestListScrollbar->AddScrollAreaItem(m_vpCurrentQuestButtons[i]->m_pButton);
				}
				for (size_t i = 0; i < m_vpCompletedQuestButtons.size(); ++i)
				{
					m_pQuestListScrollbar->AddScrollAreaItem(m_vpCompletedQuestButtons[i]->m_pButton);
				}

				m_addedQuestListScrollbar = true;

				m_pGUI->AddWindow(m_pQuestLogWindow);
				m_pQuestLogWindow->Show();
			}

			m_pTabOpenCloseButton->SetDimensions(m_questWindowWidth + m_questTabWidth, (m_questWindowHeight / 2) - (m_questTabOpenButtonHeight / 2), m_questTabOpenButtonWidth, m_questTabOpenButtonHeight);
			m_pQuestsTabIconLeft->SetDimensions(0, (m_questWindowHeight / 2) - (m_questTabHeight / 2), m_questTabWidth, m_questTabHeight);
			m_pQuestsTabIconRight->SetDimensions(m_questWindowWidth, (m_questWindowHeight / 2) - (m_questTabHeight / 2), m_questTabWidth, m_questTabHeight);
		}
		else
		{
			if (m_addedQuestListScrollbar == true)
			{
				m_pQuestLogWindow->RemoveComponent(m_pQuestListScrollbar);
				m_pQuestLogWindow->RemoveComponent(m_pCurrentQuestsHeaderLabel);
				m_pQuestLogWindow->RemoveComponent(m_pCompletedQuestsHeaderLabel);

				for (size_t i = 0; i < m_vpCurrentQuestButtons.size(); ++i)
				{
					m_pQuestLogWindow->RemoveComponent(m_vpCurrentQuestButtons[i]->m_pButton);
				}
				for (size_t i = 0; i < m_vpCurrentQuestButtons.size(); ++i)
				{
					m_pQuestLogWindow->RemoveComponent(m_vpCompletedQuestButtons[i]->m_pButton);
				}

				m_addedQuestListScrollbar = false;
			}

			m_pTabOpenCloseButton->SetDimensions(m_questWindowWidth, (m_questWindowHeight / 2) - (m_questTabOpenButtonHeight / 2), m_questTabOpenButtonWidth, m_questTabOpenButtonHeight);
			m_pQuestsTabIconLeft->SetDimensions(0, (m_questWindowHeight / 2) - (m_questTabHeight / 2), m_questTabWidth, m_questTabHeight);
			m_pQuestsTabIconRight->SetDimensions(0, (m_questWindowHeight / 2) - (m_questTabHeight / 2), m_questTabWidth, m_questTabHeight);
		}

		// Scrollbar
		int scrollbarWidth = 16;
		int scrollbarHeight = m_questTabHeight - 12;
		m_pQuestListScrollbar->SetDimensions(m_questWindowWidth + m_questTabWidth - scrollbarWidth - 4, (m_questWindowHeight / 2) - (m_questTabHeight / 2) + 8, scrollbarWidth, scrollbarHeight);
		m_pQuestListScrollbar->SetScrollArea(-m_questTabWidth + 20, 0, m_questTabWidth - 20, scrollbarHeight);

		// Quest headers
		int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_pFrontendManager->GetFrontendFont22(), "%s", m_pCurrentQuestsHeaderLabel->GetText().c_str());
		m_pCurrentQuestsHeaderLabel->SetLocation(-(m_questTabWidth) + 24, m_questTabHeight - textHeight - 12);

		int numActiveQuests = static_cast<int>(m_vpCurrentQuestButtons.size()) + 1;
		m_pCompletedQuestsHeaderLabel->SetLocation(-(m_questTabWidth) + 24, m_questTabHeight - textHeight - (numActiveQuests * textHeight) - 12);

		// Quest buttons
		for (int i = 0; i < m_vpCurrentQuestButtons.size(); i++)
		{
			m_vpCurrentQuestButtons[i]->m_pButton->SetLocation(-(m_questTabWidth) + 28, m_questTabHeight - textHeight - ((i + 1) * textHeight) - 12);
		}
		for (int i = 0; i < m_vpCompletedQuestButtons.size(); i++)
		{
			m_vpCompletedQuestButtons[i]->m_pButton->SetLocation(-(m_questTabWidth) + 28, m_questTabHeight - textHeight - ((numActiveQuests + (i + 1)) * textHeight) - 12);
		}
	}

	if (m_pQuestLogWindow->GetDimensions().x < 200)
	{
		m_questTabLeftSide = false;
	}
	else if (m_pQuestLogWindow->GetDimensions().x + m_pQuestLogWindow->GetDimensions().width > m_windowWidth - 200)
	{
		m_questTabLeftSide = true;
	}

	// Scrollbar
	int visibleSize = m_pQuestListScrollbar->GetScrollArea().height;
	int textHeight = m_pRenderer->GetFreeTypeTextHeight(m_pFrontendManager->GetFrontendFont22(), "%s", m_pCurrentQuestsHeaderLabel->GetText().c_str());
	int neededHeight = ((static_cast<int>(m_vpCurrentQuestButtons.size()) + static_cast<int>(m_vpCompletedQuestButtons.size()) + 2) * textHeight);
	int heightDiff = neededHeight - visibleSize;

	if (heightDiff > 0)
	{
		m_pQuestListScrollbar->SetScrollSize(static_cast<float>(visibleSize) / static_cast<float>(neededHeight));
		m_pQuestListScrollbar->SetDisabled(false);

		float scrollRatio = m_pQuestListScrollbar->GetScrollRatio();
		int offsetButtonY = static_cast<int>(heightDiff * scrollRatio);

		// Quest headers
		int yPos = 4 + m_pQuestListScrollbar->GetScrollArea().y + m_pQuestListScrollbar->GetScrollArea().height - 2 - (textHeight);
		m_pCurrentQuestsHeaderLabel->SetLocation(-(m_questTabWidth) + 24, yPos + offsetButtonY);
		int numActiveQuests = static_cast<int>(m_vpCurrentQuestButtons.size()) + 1;
		yPos = 4 + m_pQuestListScrollbar->GetScrollArea().y + m_pQuestListScrollbar->GetScrollArea().height - 2 - ((numActiveQuests + 1) * textHeight);
		m_pCompletedQuestsHeaderLabel->SetLocation(-(m_questTabWidth) + 24, yPos + offsetButtonY);

		// Quest buttons
		for (int i = 0; i < m_vpCurrentQuestButtons.size(); i++)
		{
			int yPos2 = 4 + m_pQuestListScrollbar->GetScrollArea().y + m_pQuestListScrollbar->GetScrollArea().height - 2 - ((i + 2) * textHeight);
			m_vpCurrentQuestButtons[i]->m_pButton->SetLocation(-(m_questTabWidth) + 28, yPos2 + offsetButtonY);
		}
		for (int i = 0; i < m_vpCompletedQuestButtons.size(); i++)
		{
			int yPos2 = 4 + m_pQuestListScrollbar->GetScrollArea().y + m_pQuestListScrollbar->GetScrollArea().height - 2 - ((numActiveQuests + (i + 2)) * textHeight);
			m_vpCompletedQuestButtons[i]->m_pButton->SetLocation(-(m_questTabWidth) + 28, yPos2 + offsetButtonY);
		}
	}
	else
	{
		m_pQuestListScrollbar->SetScrollSize(0.0f);
		m_pQuestListScrollbar->SetDisabled(true);
	}
}

// Rendering
// ReSharper disable once CppMemberFunctionMayBeStatic
void QuestGUI::Render() const
{
	// Do nothing
}

void QuestGUI::_CloseExitPressed(void* pData)
{
	QuestGUI* pQuestGUI = static_cast<QuestGUI*>(pData);
	pQuestGUI->CloseExitPressed();
}

void QuestGUI::CloseExitPressed()
{
	Unload();
}

void QuestGUI::_OpenCloseTabPressed(void* pData)
{
	QuestGUI* pQuestGUI = static_cast<QuestGUI*>(pData);
	pQuestGUI->OpenCloseTabPressed();
}

void QuestGUI::OpenCloseTabPressed()
{
	m_questTabsOpen = !m_questTabsOpen;
}

void QuestGUI::_QuestButtonPressed(void* pData)
{
	QuestButton* pQuestButton = static_cast<QuestButton*>(pData);
	pQuestButton->m_pQuestGUI->QuestButtonPressed(pQuestButton);
}

void QuestGUI::QuestButtonPressed(QuestButton* pQuestButton) const
{
	SetQuestData(pQuestButton->m_pQuest->GetName(), pQuestButton->m_pQuest->GetStartText());
}
