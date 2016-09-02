/*************************************************************************
> File Name: QuestGUI.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Quest GUI class.
> Created Time: 2016/09/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_QUEST_GUI_H
#define CUBBY_QUEST_GUI_H

#include <Blocks/ChunkManager.h>
#include <GUI/Button.h>
#include <GUI/FormattedLabel.h>
#include <GUI/Icon.h>
#include <GUI/OpenGLGUI.h>
#include <GUI/ScrollBar.h>
#include <Inventory/InventoryManager.h>

// Forward declaration
class GameWindow;
class FrontendManager;
class QuestJournal;
class Quest;
class QuestGUI;

struct QuestButton
{
	Button* m_pButton;
	Quest* m_pQuest;
	QuestGUI* m_pQuestGUI;
};

class QuestGUI
{
public:
	// Constructor, Destructor
	QuestGUI(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight);
	~QuestGUI();

	void SetQuestJournal(QuestJournal* pQuestJournal);

	void SetWindowDimensions(int windowWidth, int windowHeight);

	// Skinning the GUI
	void SkinGUI();
	void UnSkinGUI() const;

	// Loading
	void Load(bool loadDelay = false, float loadDelayTime = 0.0f);
	void Unload();
	bool IsLoadDelayed() const;
	bool IsLoaded() const;

	void CreateQuestButtons();
	void DeleteQuestButtons();

	void RefreshQuestButtons();

	void SetQuestData(std::string questTitle, std::string questText) const;

	// Updating
	void Update(float dt);

	// Rendering
	void Render() const;

protected:
	static void _CloseExitPressed(void* pData);
	void CloseExitPressed();

	static void _OpenCloseTabPressed(void* pData);
	void OpenCloseTabPressed();

	static void _QuestButtonPressed(void* pData);
	void QuestButtonPressed(QuestButton* pQuestButtonData) const;

private:
	Renderer* m_pRenderer;
	OpenGLGUI* m_pGUI;
	GameWindow* m_pGameWindow;
	FrontendManager* m_pFrontendManager;
	ChunkManager* m_pChunkManager;
	Player* m_pPlayer;
	InventoryManager* m_pInventoryManager;
	QuestJournal* m_pQuestJournal;

	int m_windowWidth;
	int m_windowHeight;

	bool m_loaded;

	// Load delay
	bool m_loadDelay;
	float m_loadDelayTime;

	int m_questWindowWidth;
	int m_questWindowHeight;
	int m_titlebarWidth;
	int m_titlebarHeight;
	int m_questTabOpenButtonWidth;
	int m_questTabOpenButtonHeight;
	int m_questTabWidth;
	int m_questTabHeight;
	bool m_questTabsOpen;
	bool m_questTabLeftSide;
	bool m_addedQuestListScrollbar;

	// GUI components
	GUIWindow* m_pQuestLogWindow;
	Icon* m_pTitleBarIcon;
	Icon* m_pTitleBarBackgroundIcon;
	Icon* m_pQuestLogWindowBackgroundIcon;
	Button* m_pCloseExitButton;
	Label* m_pQuestTitleHeaderLabel;
	FormattedLabel* m_pQuestTextLabel;
	Icon* m_pTabOpenCloseButtonIconRight;
	Icon* m_pTabOpenCloseButtonIconRightHover;
	Icon* m_pTabOpenCloseButtonIconRightPressed;
	Icon* m_pTabOpenCloseButtonIconLeft;
	Icon* m_pTabOpenCloseButtonIconLeftHover;
	Icon* m_pTabOpenCloseButtonIconLeftPressed;
	Button* m_pTabOpenCloseButton;
	Icon* m_pQuestsTabIconRight;
	Icon* m_pQuestsTabIconLeft;
	ScrollBar* m_pQuestListScrollbar;
	Label* m_pCurrentQuestsHeaderLabel;
	Label* m_pCompletedQuestsHeaderLabel;
	Icon* m_pAlphaButton;
	std::vector<QuestButton*> m_vpCurrentQuestButtons;
	std::vector<QuestButton*> m_vpCompletedQuestButtons;
};

#endif