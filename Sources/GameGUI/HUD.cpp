/*************************************************************************
> File Name: HUD.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The HUD is the on-screen display of components and interface objects that
>    display certain information for the player. For example the character's
>    health and mana, or the current enemy name and health points.
>    Also functionality for finite life HUD elements, such as level up text, or
>    'death' text and respawn button.
> Created Time: 2016/09/01
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "HUD.h"
#include "CubbyGame.h"
#include "Utils/Interpolator.h"
#include "../../Libraries/glew/include/gl/glew.h"

// Constructor, Destructor
HUD::HUD(Renderer* pRenderer, OpenGLGUI* pGUI, FrontendManager* pFrontendManager, ChunkManager* pChunkManager, Player* pPlayer, InventoryManager* pInventoryManager, int windowWidth, int windowHeight)
{
	m_pRenderer = pRenderer;
	m_pGUI = pGUI;
	m_pFrontendManager = pFrontendManager;
	m_pChunkManager = pChunkManager;
	m_pPlayer = pPlayer;
	m_pInventoryManager = pInventoryManager;

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	// Player death
	char deathHeaderText[] = "You Have Died.";
	m_pDeathHeaderLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont80(), deathHeaderText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pDeathHeaderLabel->SetOutline(true);
	m_pDeathHeaderLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pDeathHeaderLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont80Outline());

	m_pRespawnButton = new Button(m_pRenderer, m_pFrontendManager->GetFrontendFont50(), m_pFrontendManager->GetFrontendFont50Outline(), "Respawn", Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pRespawnButton->SetLabelOffset(0, 3);
	m_pRespawnButton->SetPressedOffset(0, -4);
	m_pRespawnButton->SetCallBackFunction(_RespawnPressed);
	m_pRespawnButton->SetCallBackData(this);

	// Level up
	char levelupHeaderText[] = "Level Up!";
	m_pLevelUpLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont80(), levelupHeaderText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pLevelUpLabel->SetOutline(true);
	m_pLevelUpLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pLevelUpLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont80Outline());

	// Player portrait
	m_pPortraitBackgroundIcon = new Icon(m_pRenderer, "", 72, 72);
	m_pPortraitBackgroundIcon->SetDepth(2.0f);

	// Player health
	char healthText[] = "100/100";
	m_pHealthLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), healthText, Color(1.0f, 1.0f, 1.0f, 0.75f));
	m_pHealthLabel->SetOutline(true);
	m_pHealthLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 0.95f));
	m_pHealthLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont20Outline());
	m_pHealthLabel->SetDepth(2.0f);

	m_pHealthContainerIcon = new Icon(m_pRenderer, "", 72, 64);
	m_pHealthContainerIcon->SetDepth(2.0f);

	m_pHealthFillerIcon = new Icon(m_pRenderer, "", 103, 21);
	m_pHealthFillerIcon->SetDepth(1.75f);

	m_pHealthFillerBackgroundIcon = new Icon(m_pRenderer, "", 53, 21);
	m_pHealthFillerBackgroundIcon->SetDepth(1.7f);

	// Player magic
	char magicText[] = "100/100";
	m_pMagicLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont14(), magicText, Color(1.0f, 1.0f, 1.0f, 0.75f));
	m_pMagicLabel->SetOutline(true);
	m_pMagicLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 0.95f));
	m_pMagicLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont14Outline());
	m_pMagicLabel->SetDepth(2.0f);

	m_pManaFillerIcon = new Icon(m_pRenderer, "", 84, 11);
	m_pManaFillerIcon->SetDepth(1.75f);

	m_pManaFillerBackgroundIcon = new Icon(m_pRenderer, "", 53, 11);
	m_pManaFillerBackgroundIcon->SetDepth(1.7f);

	// Player experience
	char experienceText[] = "0/100";
	m_pExperienceLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont18(), experienceText, Color(1.0f, 1.0f, 1.0f, 0.75f));
	m_pExperienceLabel->SetOutline(true);
	m_pExperienceLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 0.95f));
	m_pExperienceLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont18Outline());
	m_pExperienceLabel->SetDepth(2.0f);

	m_pExperienceContainerIcon = new Icon(m_pRenderer, "", 328, 26);
	m_pExperienceContainerIcon->SetDepth(2.0f);

	m_pExperienceFillerIcon = new Icon(m_pRenderer, "", 320, 14);
	m_pExperienceFillerIcon->SetDepth(1.75f);

	m_pExperienceFillerBackgroundIcon = new Icon(m_pRenderer, "", 53, 14);
	m_pExperienceFillerBackgroundIcon->SetDepth(1.7f);

	// Enemy health
	char enemyHealthText[] = "100/100";
	m_pEnemyHealthLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont20(), enemyHealthText, Color(1.0f, 1.0f, 1.0f, 0.75f));
	m_pEnemyHealthLabel->SetOutline(true);
	m_pEnemyHealthLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 0.95f));
	m_pEnemyHealthLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont20Outline());
	m_pEnemyHealthLabel->SetDepth(2.0f);


	m_pEnemyHealthContainerIcon = new Icon(m_pRenderer, "", 300, 33);
	m_pEnemyHealthContainerIcon->SetDepth(2.0f);

	m_pEnemyHealthFillerIcon = new Icon(m_pRenderer, "", 103, 21);
	m_pEnemyHealthFillerIcon->SetDepth(1.75f);

	m_pEnemyHealthFillerBackgroundIcon = new Icon(m_pRenderer, "", 53, 21);
	m_pEnemyHealthFillerBackgroundIcon->SetDepth(1.7f);

	// Enemy name
	char enemyNameText[] = "Enemy";
	m_pEnemyNameLabel = new Label(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), enemyNameText, Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_pEnemyNameLabel->SetOutline(true);
	m_pEnemyNameLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	m_pEnemyNameLabel->SetOutlineFont(m_pFrontendManager->GetFrontendFont30Outline());

	// Portrait
	m_pDynamicPortraitIcon = new Icon(m_pRenderer);
	m_pDynamicPortraitIcon->SetDepth(2.5f);

	// TODO: Checkboxes for GUI panels on the HUD
	// Checkbox icons
	//m_pInventoryCheckboxIcon = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/inventory_normal.tga", 44, 44);
	//m_pInventoryCheckboxIcon->SetDepth(2.0f);
	//m_pInventoryCheckboxIconHover = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/inventory_hover.tga", 44, 44);
	//m_pInventoryCheckboxIconHover->SetDepth(2.0f);
	//m_pInventoryCheckboxIconPressed = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/inventory_pressed.tga", 44, 44);
	//m_pInventoryCheckboxIconPressed->SetDepth(2.0f);
	//m_pInventoryCheckboxIconToggled = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/inventory_toggled.tga", 44, 44);
	//m_pInventoryCheckboxIconToggled->SetDepth(2.0f);
	//m_pInventoryCheckboxIconToggledHover = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/inventory_toggled_hover.tga", 44, 44);
	//m_pInventoryCheckboxIconToggledHover->SetDepth(2.0f);
	//m_pInventoryCheckboxIconToggledPressed = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/inventory_toggled_pressed.tga", 44, 44);
	//m_pInventoryCheckboxIconToggledPressed->SetDepth(2.0f);
	//m_pCharacterCheckboxIcon = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/character_normal.tga", 44, 44);
	//m_pCharacterCheckboxIcon->SetDepth(2.0f);
	//m_pCharacterCheckboxIconHover = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/character_hover.tga", 44, 44);
	//m_pCharacterCheckboxIconHover->SetDepth(2.0f);
	//m_pCharacterCheckboxIconPressed = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/character_pressed.tga", 44, 44);
	//m_pCharacterCheckboxIconPressed->SetDepth(2.0f);
	//m_pCharacterCheckboxIconToggled = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/character_toggled.tga", 44, 44);
	//m_pCharacterCheckboxIconToggled->SetDepth(2.0f);
	//m_pCharacterCheckboxIconToggledHover = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/character_toggled_hover.tga", 44, 44);
	//m_pCharacterCheckboxIconToggledHover->SetDepth(2.0f);
	//m_pCharacterCheckboxIconToggledPressed = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/character_toggled_pressed.tga", 44, 44);
	//m_pCharacterCheckboxIconToggledPressed->SetDepth(2.0f);
	//m_pQuestCheckboxIcon = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/quest_normal.tga", 44, 44);
	//m_pQuestCheckboxIcon->SetDepth(2.0f);
	//m_pQuestCheckboxIconHover = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/quest_hover.tga", 44, 44);
	//m_pQuestCheckboxIconHover->SetDepth(2.0f);
	//m_pQuestCheckboxIconPressed = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/quest_pressed.tga", 44, 44);
	//m_pQuestCheckboxIconPressed->SetDepth(2.0f);
	//m_pQuestCheckboxIconToggled = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/quest_toggled.tga", 44, 44);
	//m_pQuestCheckboxIconToggled->SetDepth(2.0f);
	//m_pQuestCheckboxIconToggledHover = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/quest_toggled_hover.tga", 44, 44);
	//m_pQuestCheckboxIconToggledHover->SetDepth(2.0f);
	//m_pQuestCheckboxIconToggledPressed = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/quest_toggled_pressed.tga", 44, 44);
	//m_pQuestCheckboxIconToggledPressed->SetDepth(2.0f);
	//m_pCraftingCheckboxIcon = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/crafting_normal.tga", 44, 44);
	//m_pCraftingCheckboxIcon->SetDepth(2.0f);
	//m_pCraftingCheckboxIconHover = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/crafting_hover.tga", 44, 44);
	//m_pCraftingCheckboxIconHover->SetDepth(2.0f);
	//m_pCraftingCheckboxIconPressed = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/crafting_pressed.tga", 44, 44);
	//m_pCraftingCheckboxIconPressed->SetDepth(2.0f);
	//m_pCraftingCheckboxIconToggled = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/crafting_toggled.tga", 44, 44);
	//m_pCraftingCheckboxIconToggled->SetDepth(2.0f);
	//m_pCraftingCheckboxIconToggledHover = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/crafting_toggled_hover.tga", 44, 44);
	//m_pCraftingCheckboxIconToggledHover->SetDepth(2.0f);
	//m_pCraftingCheckboxIconToggledPressed = new Icon(m_pRenderer, "Resources/textures/gui/Stonewash/HUD/Buttons/crafting_toggled_pressed.tga", 44, 44);
	//m_pCraftingCheckboxIconToggledPressed->SetDepth(2.0f);

	// GUI panel checkboxes
	//m_pInventoryCheckBox = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), "");
	//m_pInventoryCheckBox->SetCallBackFunction(_InventoryCheckboxPressed);
	//m_pInventoryCheckBox->SetCallBackData(this);
	//m_pInventoryCheckBox->SetDefaultIcon(m_pInventoryCheckboxIcon);
	//m_pInventoryCheckBox->SetHoverIcon(m_pInventoryCheckboxIconHover);
	//m_pInventoryCheckBox->SetSelectedIcon(m_pInventoryCheckboxIconPressed);
	//m_pInventoryCheckBox->SetToggledIcon(m_pInventoryCheckboxIconToggled);
	//m_pInventoryCheckBox->SetToggledHoverIcon(m_pInventoryCheckboxIconToggledHover);
	//m_pInventoryCheckBox->SetToggledSelectedIcon(m_pInventoryCheckboxIconToggledPressed);

	//m_pCharacterCheckBox = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), "");
	//m_pCharacterCheckBox->SetCallBackFunction(_CharacterCheckboxPressed);
	//m_pCharacterCheckBox->SetCallBackData(this);
	//m_pCharacterCheckBox->SetDefaultIcon(m_pCharacterCheckboxIcon);
	//m_pCharacterCheckBox->SetHoverIcon(m_pCharacterCheckboxIconHover);
	//m_pCharacterCheckBox->SetSelectedIcon(m_pCharacterCheckboxIconPressed);
	//m_pCharacterCheckBox->SetToggledIcon(m_pCharacterCheckboxIconToggled);
	//m_pCharacterCheckBox->SetToggledHoverIcon(m_pCharacterCheckboxIconToggledHover);
	//m_pCharacterCheckBox->SetToggledSelectedIcon(m_pCharacterCheckboxIconToggledPressed);

	//m_pQuestCheckBox = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), "");
	//m_pQuestCheckBox->SetCallBackFunction(_QuestCheckboxPressed);
	//m_pQuestCheckBox->SetCallBackData(this);
	//m_pQuestCheckBox->SetDefaultIcon(m_pQuestCheckboxIcon);
	//m_pQuestCheckBox->SetHoverIcon(m_pQuestCheckboxIconHover);
	//m_pQuestCheckBox->SetSelectedIcon(m_pQuestCheckboxIconPressed);
	//m_pQuestCheckBox->SetToggledIcon(m_pQuestCheckboxIconToggled);
	//m_pQuestCheckBox->SetToggledHoverIcon(m_pQuestCheckboxIconToggledHover);
	//m_pQuestCheckBox->SetToggledSelectedIcon(m_pQuestCheckboxIconToggledPressed);

	//m_pCraftingCheckBox = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), "");
	//m_pCraftingCheckBox->SetCallBackFunction(_CraftingCheckboxPressed);
	//m_pCraftingCheckBox->SetCallBackData(this);
	//m_pCraftingCheckBox->SetDefaultIcon(m_pCraftingCheckboxIcon);
	//m_pCraftingCheckBox->SetHoverIcon(m_pCraftingCheckboxIconHover);
	//m_pCraftingCheckBox->SetSelectedIcon(m_pCraftingCheckboxIconPressed);
	//m_pCraftingCheckBox->SetToggledIcon(m_pCraftingCheckboxIconToggled);
	//m_pCraftingCheckBox->SetToggledHoverIcon(m_pCraftingCheckboxIconToggledHover);
	//m_pCraftingCheckBox->SetToggledSelectedIcon(m_pCraftingCheckboxIconToggledPressed);

	//m_pSkillsCheckBox = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), "");
	//m_pSkillsCheckBox->SetCallBackFunction(_SkillsCheckboxPressed);
	//m_pSkillsCheckBox->SetCallBackData(this);

	//m_pPetCheckBox = new CheckBox(m_pRenderer, m_pFrontendManager->GetFrontendFont30(), "");
	//m_pPetCheckBox->SetCallBackFunction(_PetCheckboxPressed);
	//m_pPetCheckBox->SetCallBackData(this);

	SetWindowDimensions(m_windowWidth, m_windowHeight);

	m_enemyInfoDisappearTime = 3.0f;
	m_enemyInfoDisappearTimer = 0.0f;
	m_removedEnemyInfo = true;
	m_updatePlayerHealthTextLocation = false;
	m_updatePlayerMagicTextLocation = false;
	m_updatePlayerExperienceTextLocation = false;
	m_updateEnemyTextLocation = false;

	m_playerDied = false;
	m_levelUp = false;

	m_loaded = false;
}

HUD::~HUD()
{
	// Player death
	delete m_pDeathHeaderLabel;
	delete m_pRespawnButton;

	// Level up
	delete m_pLevelUpLabel;

	// Player portrait
	delete m_pPortraitBackgroundIcon;

	// Player health
	delete m_pHealthLabel;
	delete m_pHealthContainerIcon;
	delete m_pHealthFillerIcon;
	delete m_pHealthFillerBackgroundIcon;

	// Player magic
	delete m_pMagicLabel;

	// Player experience
	delete m_pExperienceLabel;
	delete m_pExperienceContainerIcon;
	delete m_pExperienceFillerIcon;
	delete m_pExperienceFillerBackgroundIcon;
	delete m_pManaFillerIcon;
	delete m_pManaFillerBackgroundIcon;

	// Enemy health
	delete m_pEnemyHealthLabel;
	delete m_pEnemyNameLabel;
	delete m_pEnemyHealthContainerIcon;
	delete m_pEnemyHealthFillerIcon;
	delete m_pEnemyHealthFillerBackgroundIcon;

	// Portrait
	delete m_pDynamicPortraitIcon;

	// TODO: Checkboxes for GUI panels on the HUD
	// GUI panel checkboxes
	//delete m_pInventoryCheckBox;
	//delete m_pCharacterCheckBox;
	//delete m_pQuestCheckBox;
	//delete m_pCraftingCheckBox;
	//delete m_pSkillsCheckBox;
	//delete m_pPetCheckBox;

	// Checkbox icons
	//delete m_pInventoryCheckboxIcon;
	//delete m_pInventoryCheckboxIconHover;
	//delete m_pInventoryCheckboxIconPressed;
	//delete m_pInventoryCheckboxIconToggled;
	//delete m_pInventoryCheckboxIconToggledHover;
	//delete m_pInventoryCheckboxIconToggledPressed;
	//delete m_pCharacterCheckboxIcon;
	//delete m_pCharacterCheckboxIconHover;
	//delete m_pCharacterCheckboxIconPressed;
	//delete m_pCharacterCheckboxIconToggled;
	//delete m_pCharacterCheckboxIconToggledHover;
	//delete m_pCharacterCheckboxIconToggledPressed;
	//delete m_pQuestCheckboxIcon;
	//delete m_pQuestCheckboxIconHover;
	//delete m_pQuestCheckboxIconPressed;
	//delete m_pQuestCheckboxIconToggled;
	//delete m_pQuestCheckboxIconToggledHover;
	//delete m_pQuestCheckboxIconToggledPressed;
	//delete m_pCraftingCheckboxIcon;
	//delete m_pCraftingCheckboxIconHover;
	//delete m_pCraftingCheckboxIconPressed;
	//delete m_pCraftingCheckboxIconToggled;
	//delete m_pCraftingCheckboxIconToggledHover;
	//delete m_pCraftingCheckboxIconToggledPressed;
}

void HUD::SetInventoryGUI(InventoryGUI* pInventoryGUI)
{
	m_pInventoryGUI = pInventoryGUI;
}

void HUD::SetCharacterGUI(CharacterGUI* pCharacterGUI)
{
	m_pCharacterGUI = pCharacterGUI;
}

void HUD::SetQuestGUI(QuestGUI* pQuestGUI)
{
	m_pQuestGUI = pQuestGUI;
}

void HUD::SetCraftingGUI(CraftingGUI* pCraftingGUI)
{
	m_pCraftingGUI = pCraftingGUI;
}

void HUD::SetWindowDimensions(int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	// Player death
	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont80(), "%s", m_pDeathHeaderLabel->GetText().c_str());
	m_pDeathHeaderLabel->SetLocation(static_cast<int>((m_windowWidth * 0.5f) - static_cast<int>(textWidth * 0.5f)), static_cast<int>(m_windowHeight * 0.5f + 150));
	m_respawnButtonWidth = 200;
	m_respawnButtonHeight = 75;
	m_pRespawnButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - static_cast<int>(m_respawnButtonWidth*0.5f)), static_cast<int>((m_windowHeight * 0.5f) - (m_respawnButtonHeight * 0.5f) + 75), static_cast<int>(m_respawnButtonWidth), static_cast<int>(m_respawnButtonHeight));

	// Level up
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont80(), "%s", m_pLevelUpLabel->GetText().c_str());
	m_pLevelUpLabel->SetLocation(static_cast<int>((m_windowWidth * 0.5f) - static_cast<int>(textWidth * 0.5f)), static_cast<int>(m_windowHeight * 0.5f - 25));

	// Player portrait
	m_pPortraitBackgroundIcon->SetDimensions(20, m_windowHeight - 88, 72, 72);

	// Player health
	m_playerHealthX = 92.0f;
	m_playerHealthY = m_windowHeight - 49.0f;
	m_playerHealthWidth = 160.0f;
	m_playerHealthHeight = 17.0f;
	m_playerHealthTitleOffset = 18.0f;
	m_pHealthContainerIcon->SetDimensions(92, m_windowHeight - 80, 164, 56);
	m_pHealthFillerIcon->SetDimensions(24, m_windowHeight - 84, 103, 21);
	m_pHealthFillerBackgroundIcon->SetDimensions(24, m_windowHeight - 84, 53, 21);

	// Player magic
	m_playerMagicX = 92.0f;
	m_playerMagicY = m_windowHeight - 72.0f;
	m_playerMagicWidth = 160.0f;
	m_playerMagicHeight = 11.0f;
	m_playerMagicTitleOffset = 18.0f;
	m_pManaFillerIcon->SetDimensions(24, m_windowHeight - 84, 84, 11);
	m_pManaFillerBackgroundIcon->SetDimensions(24, m_windowHeight - 84, 53, 11);

	// Player experience
	m_playerExperienceX = m_windowWidth / 2 - 328 / 2 + 4.0f;
	m_playerExperienceY = 92;
	m_playerExperienceWidth = 321.0f;
	m_playerExperienceHeight = 14.0f;
	m_playerExperienceTitleOffset = 18.0f;
	m_pExperienceContainerIcon->SetDimensions(m_windowWidth / 2 - 328 / 2, 88, 328, 26);
	m_pExperienceFillerIcon->SetDimensions(24, m_windowHeight - 84, 84, 11);
	m_pExperienceFillerBackgroundIcon->SetDimensions(24, m_windowHeight - 84, 53, 11);

	// Enemy health
	m_enemyHealthWidth = 300.0f;
	m_enemyHealthHeight = 33.0f;
	m_enemyHealthX = (m_windowWidth * 0.5f) - (m_enemyHealthWidth * 0.5f);
	m_enemyHealthY = static_cast<float>(m_windowHeight - 75);
	textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont20(), "%s", m_pEnemyHealthLabel->GetText().c_str());
	m_pEnemyHealthLabel->SetLocation(static_cast<int>((m_windowWidth * 0.5f) - static_cast<int>(textWidth * 0.5f)), static_cast<int>(m_windowHeight) + 7);
	m_pEnemyNameLabel->SetLocation(static_cast<int>((m_windowWidth * 0.5f) - static_cast<int>(textWidth * 0.5f)), static_cast<int>(m_windowHeight) + 35);
	m_pEnemyHealthContainerIcon->SetDimensions(static_cast<int>(m_enemyHealthX), static_cast<int>(m_enemyHealthY), static_cast<int>(m_enemyHealthWidth), static_cast<int>(m_enemyHealthHeight));
	m_pEnemyHealthFillerIcon->SetDimensions(static_cast<int>(m_enemyHealthX), static_cast<int>(m_enemyHealthY), static_cast<int>(m_enemyHealthWidth), static_cast<int>(m_enemyHealthHeight));
	m_pEnemyHealthFillerBackgroundIcon->SetDimensions(static_cast<int>(m_enemyHealthX), static_cast<int>(m_enemyHealthY), static_cast<int>(m_enemyHealthWidth), static_cast<int>(m_enemyHealthHeight));

	// Portrait icon
	m_pDynamicPortraitIcon->SetDimensions(24, m_windowHeight - 84, 64, 64);

	// GUI panel checkboxes
	//int checkboxSize = 32;
	//int windowBorder = 10;
	//int spacer = 10;
	// TODO: Checkboxes for GUI panels on the HUD
	//m_pInventoryCheckBox->SetDimensions(m_windowWidth - windowBorder - checkboxSize * 1 - spacer * 0, 10, checkboxSize, checkboxSize);
	//m_pCharacterCheckBox->SetDimensions(m_windowWidth - windowBorder - checkboxSize * 2 - spacer * 1, 10, checkboxSize, checkboxSize);
	//m_pQuestCheckBox->SetDimensions(m_windowWidth - windowBorder - checkboxSize * 3 - spacer * 2, 10, checkboxSize, checkboxSize);
	//m_pCraftingCheckBox->SetDimensions(m_windowWidth - windowBorder - checkboxSize * 4 - spacer * 3, 10, checkboxSize, checkboxSize);
	//m_pSkillsCheckBox->SetDimensions(m_windowWidth - windowBorder - checkboxSize * 5 - spacer * 4, 10, checkboxSize, checkboxSize);
	//m_pPetCheckBox->SetDimensions(m_windowWidth - windowBorder - checkboxSize * 6 - spacer * 5, 10, checkboxSize, checkboxSize);

	// Make sure to update the text locations for various HUD text
	m_updatePlayerHealthTextLocation = true;
	m_updatePlayerMagicTextLocation = true;
	m_updatePlayerExperienceTextLocation = true;
	m_updateEnemyTextLocation = true;
}

// Skinning the GUI
void HUD::SkinGUI()
{
	std::string themeName = CubbyGame::GetInstance()->GetModsManager()->GetHUDTextureTheme();

	std::string iconName = "Resources/textures/gui/" + themeName + "/HUD/portrait_background.tga";
	m_pPortraitBackgroundIcon->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/HUD/health_container.tga";
	m_pHealthContainerIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/HUD/health_filler.tga";
	m_pHealthFillerIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/HUD/health_filler_background.tga";
	m_pHealthFillerBackgroundIcon->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/HUD/mana_filler.tga";
	m_pManaFillerIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/HUD/mana_filler_background.tga";
	m_pManaFillerBackgroundIcon->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/HUD/experience_container.tga";
	m_pExperienceContainerIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/HUD/experience_filler.tga";
	m_pExperienceFillerIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/HUD/experience_filler_background.tga";
	m_pExperienceFillerBackgroundIcon->SetIcon(iconName);

	iconName = "Resources/textures/gui/" + themeName + "/HUD/enemy_health_container.tga";
	m_pEnemyHealthContainerIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/HUD/enemy_health_filler.tga";
	m_pEnemyHealthFillerIcon->SetIcon(iconName);
	iconName = "Resources/textures/gui/" + themeName + "/HUD/enemy_health_filler_background.tga";
	m_pEnemyHealthFillerBackgroundIcon->SetIcon(iconName);

	m_pFrontendManager->SetButtonIcons(m_pRespawnButton, ButtonSize::Size225x75);
	m_pRespawnButton->SetDimensions(static_cast<int>((m_windowWidth * 0.5f) - static_cast<int>(m_respawnButtonWidth * 0.5f)), static_cast<int>((m_windowHeight * 0.5f) - (m_respawnButtonHeight * 0.5f) + 75), static_cast<int>(m_respawnButtonWidth), static_cast<int>(m_respawnButtonHeight));

	m_pRespawnButton->SetNormalLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pRespawnButton->SetHoverLabelColor(m_pFrontendManager->GetHoverFontColor());
	m_pRespawnButton->SetPressedLabelColor(m_pFrontendManager->GetPressedFontColor());
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void HUD::UnSkinGUI() const
{
	// Do nothing
}

// Loading
void HUD::Load()
{
	m_loaded = true;

	if (m_playerDied)
	{
		// Player death
		m_pGUI->AddComponent(m_pDeathHeaderLabel);
		m_pGUI->AddComponent(m_pRespawnButton);
	}

	// Player portrait
	m_pGUI->AddComponent(m_pPortraitBackgroundIcon);

	// Player health
	m_pGUI->AddComponent(m_pHealthLabel);
	m_pGUI->AddComponent(m_pHealthContainerIcon);
	m_pGUI->AddComponent(m_pHealthFillerIcon);
	m_pGUI->AddComponent(m_pHealthFillerBackgroundIcon);

	// Player magic
	m_pGUI->AddComponent(m_pMagicLabel);
	m_pGUI->AddComponent(m_pManaFillerIcon);
	m_pGUI->AddComponent(m_pManaFillerBackgroundIcon);

	// Player experience
	m_pGUI->AddComponent(m_pExperienceLabel);
	m_pGUI->AddComponent(m_pExperienceContainerIcon);
	m_pGUI->AddComponent(m_pExperienceFillerIcon);
	m_pGUI->AddComponent(m_pExperienceFillerBackgroundIcon);

	// Portrait icon
	m_pGUI->AddComponent(m_pDynamicPortraitIcon);

	// TODO: Checkboxes for GUI panels on the HUD
	// GUI panel checkboxes
	//m_pGUI->AddComponent(m_pInventoryCheckBox);
	//m_pGUI->AddComponent(m_pCharacterCheckBox);
	//m_pGUI->AddComponent(m_pQuestCheckBox);
	//m_pGUI->AddComponent(m_pCraftingCheckBox);
	//m_pGUI->AddComponent(m_pSkillsCheckBox);
	//m_pGUI->AddComponent(m_pPetCheckBox);

	UpdatePlayerData();
}

void HUD::Unload()
{
	// Player death
	m_pGUI->RemoveComponent(m_pDeathHeaderLabel);
	m_pGUI->RemoveComponent(m_pRespawnButton);

	// Level up
	m_pGUI->RemoveComponent(m_pLevelUpLabel);

	// Player portrait
	m_pGUI->RemoveComponent(m_pPortraitBackgroundIcon);

	// Player health
	m_pGUI->RemoveComponent(m_pHealthLabel);
	m_pGUI->RemoveComponent(m_pHealthContainerIcon);
	m_pGUI->RemoveComponent(m_pHealthFillerIcon);
	m_pGUI->RemoveComponent(m_pHealthFillerBackgroundIcon);

	// Player magic
	m_pGUI->RemoveComponent(m_pMagicLabel);
	m_pGUI->RemoveComponent(m_pManaFillerIcon);
	m_pGUI->RemoveComponent(m_pManaFillerBackgroundIcon);

	// Player experience
	m_pGUI->RemoveComponent(m_pExperienceLabel);
	m_pGUI->RemoveComponent(m_pExperienceContainerIcon);
	m_pGUI->RemoveComponent(m_pExperienceFillerIcon);
	m_pGUI->RemoveComponent(m_pExperienceFillerBackgroundIcon);

	// Enemy health
	m_pGUI->RemoveComponent(m_pEnemyHealthLabel);
	m_pGUI->RemoveComponent(m_pEnemyNameLabel);
	m_pGUI->RemoveComponent(m_pEnemyHealthContainerIcon);
	m_pGUI->RemoveComponent(m_pEnemyHealthFillerIcon);
	m_pGUI->RemoveComponent(m_pEnemyHealthFillerBackgroundIcon);

	// Portrait icon
	m_pGUI->RemoveComponent(m_pDynamicPortraitIcon);

	// TODO: Checkboxes for GUI panels on the HUD
	// GUI panel checkboxes
	//m_pGUI->RemoveComponent(m_pInventoryCheckBox);
	//m_pGUI->RemoveComponent(m_pCharacterCheckBox);
	//m_pGUI->RemoveComponent(m_pQuestCheckBox);
	//m_pGUI->RemoveComponent(m_pCraftingCheckBox);
	//m_pGUI->RemoveComponent(m_pSkillsCheckBox);
	//m_pGUI->RemoveComponent(m_pPetCheckBox);

	m_loaded = false;
}

bool HUD::IsLoaded() const
{
	return m_loaded;
}

// Player death
void HUD::PlayerDied()
{
	m_playerDied = true;

	m_pGUI->AddComponent(m_pDeathHeaderLabel);

	float deathHeaderDelay = 3.5f;
	float deathHeaderTimeIn = 0.5f;
	float deathHeaderWait = 0.5f;
	FloatInterpolation* deathAlpha1 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_deathHeaderAlpha, 0.0f, 0.0f, deathHeaderDelay, 100.0f);
	FloatInterpolation* deathAlpha2 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_deathHeaderAlpha, 0.0f, 1.0f, deathHeaderTimeIn, 100.0f);
	FloatInterpolation* deathAlpha3 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_deathHeaderAlpha, 1.0f, 1.0f, deathHeaderWait, 100.0f, nullptr, _DeathTextFinished, this);
	Interpolator::GetInstance()->LinkFloatInterpolation(deathAlpha1, deathAlpha2);
	Interpolator::GetInstance()->LinkFloatInterpolation(deathAlpha2, deathAlpha3);
	Interpolator::GetInstance()->AddFloatInterpolation(deathAlpha1);

	m_pDeathHeaderLabel->SetColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
	m_pDeathHeaderLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 0.0f));
}

void HUD::PlayerRespawn()
{
	m_playerDied = false;

	m_pGUI->RemoveComponent(m_pDeathHeaderLabel);
	m_pGUI->RemoveComponent(m_pRespawnButton);
}

// Level up
void HUD::LevelUp()
{
	m_levelUp = true;

	m_pGUI->AddComponent(m_pLevelUpLabel);

	float levelUpDelay = 0.0f;
	float levelUpTimeIn = 0.5f;
	float levelUpWait = 1.5f;
	float levelUpTimeOut = 0.25f;
	FloatInterpolation* levelUpAlpha1 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_levelUpAlpha, 0.0f, 0.0f, levelUpDelay, 100.0f);
	FloatInterpolation* levelUpAlpha2 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_levelUpAlpha, 0.0f, 1.0f, levelUpTimeIn, 100.0f);
	FloatInterpolation* levelUpAlpha3 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_levelUpAlpha, 1.0f, 1.0f, levelUpWait, 100.0f);
	FloatInterpolation* levelUpAlpha4 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_levelUpAlpha, 1.0f, 0.0f, levelUpTimeOut, 100.0f, nullptr, _LevelUpTextFinished, this);
	Interpolator::GetInstance()->LinkFloatInterpolation(levelUpAlpha1, levelUpAlpha2);
	Interpolator::GetInstance()->LinkFloatInterpolation(levelUpAlpha2, levelUpAlpha3);
	Interpolator::GetInstance()->LinkFloatInterpolation(levelUpAlpha3, levelUpAlpha4);
	Interpolator::GetInstance()->AddFloatInterpolation(levelUpAlpha1);

	// Do a movement up animation
	int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont80(), "%s", m_pLevelUpLabel->GetText().c_str());
	m_pLevelUpLabel->SetLocation(static_cast<int>((m_windowWidth * 0.5f) - static_cast<int>(textWidth * 0.5f)), static_cast<int>(m_windowHeight * 0.5f - 50));

	float totalTime = levelUpDelay + levelUpTimeIn + levelUpWait + levelUpTimeOut;
	Interpolator::GetInstance()->AddIntInterpolation(&m_pLevelUpLabel->GetDimensionsPointer()->y, m_pLevelUpLabel->GetDimensions().y, m_pLevelUpLabel->GetDimensions().y + 150, totalTime, -100.0f);

	m_pLevelUpLabel->SetColor(Color(1.0f, 1.0f, 0.0f, 0.0f));
	m_pLevelUpLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, 0.0f));
}

// Enemy health
void HUD::SetEnemyHealth(float health, float healthMax, float damagedone)
{
	m_enemyHealth = health;
	m_enemyHealthMax = healthMax;

	// Update the HP label
	char healthText[32];
	sprintf(healthText, "%i/%i", static_cast<int>(m_enemyHealth), static_cast<int>(m_enemyHealthMax));

	m_updateEnemyTextLocation = true;

	m_pEnemyHealthLabel->SetText(healthText);

	// Make the enemy health bar appear, or refresh its timer
	m_enemyInfoDisappearTimer = m_enemyInfoDisappearTime;
	if (m_removedEnemyInfo == true && m_loaded)
	{
		m_pGUI->AddComponent(m_pEnemyHealthLabel);
		m_pGUI->AddComponent(m_pEnemyNameLabel);
		m_pGUI->AddComponent(m_pEnemyHealthContainerIcon);
		m_pGUI->AddComponent(m_pEnemyHealthFillerIcon);
		m_pGUI->AddComponent(m_pEnemyHealthFillerBackgroundIcon);
	}
	m_removedEnemyInfo = false;

	// Set the enemy health width decreaser
	m_enemyHealthWidthDecreaser = damagedone;

	// Create an enemy health dropoff
	//EnemyHealthDropOff* pNewDropOff = new EnemyHealthDropOff();

	//Vector3d translation = m_pEnemyHealthContainerIcon->GetAnimationUpdater()->GetAnimatedTranslation();
	//float enemyHealthBarRatio = m_enemyHealth / m_enemyHealthMax;
	//float enemyWidthModified = (m_enemyHealthWidth - 16.0f) * enemyHealthBarRatio;
	//float enemyHealthXModified = m_enemyHealthX + 8.0f + translation.x;
	//float enemyHealthYModified = m_enemyHealthY + 8.0f + translation.y;

	//pNewDropOff->m_x = static_cast<int>(enemyHealthXModified + enemyWidthModified);
	//pNewDropOff->m_y = static_cast<int>(enemyHealthYModified);
	//pNewDropOff->m_width = static_cast<int>(damagedone * (m_enemyHealthWidth / m_enemyHealthMax));
	//pNewDropOff->m_height = static_cast<int>(m_enemyHealthHeight - 12);

	//pNewDropOff->m_maxLife = 0.75f + GetRandomNumber(-1, 1, 1) * 0.1f;
	//pNewDropOff->m_life = pNewDropOff->m_maxLife;

	//float randomNum = GetRandomNumber(-5, 5) * 10.0f;
	//pNewDropOff->m_velocity = Vector3d(randomNum, -50.0f + GetRandomNumber(-20, 30, 1), 0.0f);
	//pNewDropOff->m_rotationVelocity = Vector3d(0.0f, 0.0f, -randomNum);

	//pNewDropOff->m_erase = false;

	//m_vpEnemyHealthDropOffList.push_back(pNewDropOff);
}

void HUD::SetEnemyName(const char* name) const
{
	m_pEnemyNameLabel->SetText(name);

	char nameText[MAX_ENEMY_NAME_SIZE];
	sprintf(nameText, "%s", name);
}

// Updating
void HUD::Update(float dt)
{
	if (CubbyGame::GetInstance()->IsPaused() == false)
	{
		// Update the player death components
		if (m_playerDied)
		{
			m_pDeathHeaderLabel->SetColor(Color(1.0f, 1.0f, 1.0f, m_deathHeaderAlpha));
			m_pDeathHeaderLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, m_deathHeaderAlpha));
		}

		// Update the level up text
		if (m_levelUp)
		{
			m_pLevelUpLabel->SetColor(Color(1.0f, 1.0f, 0.0f, m_levelUpAlpha));
			m_pLevelUpLabel->SetOutlineColor(Color(0.0f, 0.0f, 0.0f, m_levelUpAlpha));
		}

		// Update the enemy health disappear timer
		if (m_enemyInfoDisappearTimer > 0.0f)
		{
			m_enemyInfoDisappearTimer -= dt;
		}
		else
		{
			if (m_removedEnemyInfo == false)
			{
				m_pGUI->RemoveComponent(m_pEnemyHealthLabel);
				m_pGUI->RemoveComponent(m_pEnemyNameLabel);
				m_pGUI->RemoveComponent(m_pEnemyHealthContainerIcon);
				m_pGUI->RemoveComponent(m_pEnemyHealthFillerIcon);
				m_pGUI->RemoveComponent(m_pEnemyHealthFillerBackgroundIcon);

				m_removedEnemyInfo = true;
			}
		}
	}

	// Portrait dynamic icon
	// Set the dynamic icon
	m_pDynamicPortraitIcon->SetDynamicTexture(CubbyGame::GetInstance()->GetDynamicPortraitTexture());

	// TODO: Checkboxes for GUI panels on the HUD
	// Update the checkboxes for the GUI panels if open or not
	//m_pInventoryCheckBox->SetToggled(m_pInventoryGUI->IsLoaded());
	//m_pCharacterCheckBox->SetToggled(m_pCharacterGUI->IsLoaded());
	//m_pQuestCheckBox->SetToggled(m_pQuestGUI->IsLoaded());
	//m_pCraftingCheckBox->SetToggled(m_pCraftingGUI->IsLoaded());
	//m_pSkillsCheckBox->SetToggled(m_pSkillsGUI->IsLoaded());
	//m_pPetCheckBox->SetToggled(m_pCompanionPet->IsSummoned());

	// TODO : Checkboxes for GUI panels on the HUD
	//if(m_pGameWindow->IsPaused())
	//{
	//	m_pInventoryCheckBox->SetEnabled(false);
	//	m_pCharacterCheckBox->SetEnabled(false);
	//	m_pQuestCheckBox->SetEnabled(false);
	//	m_pCraftingCheckBox->SetEnabled(false);
	//	m_pSkillsCheckBox->SetEnabled(false);
	//	m_pPetCheckBox->SetEnabled(false);
	//}
	//else
	//{
	//	m_pInventoryCheckBox->SetEnabled(true);
	//	m_pCharacterCheckBox->SetEnabled(true);
	//	m_pQuestCheckBox->SetEnabled(true);
	//	m_pCraftingCheckBox->SetEnabled(true);
	//	m_pSkillsCheckBox->SetEnabled(true);
	//	m_pPetCheckBox->SetEnabled(true);
	//}
}

void HUD::UpdatePlayerData()
{
	// Update the player health
	int health = static_cast<int>(m_pPlayer->GetHealth());
	int maxHealth = static_cast<int>(m_pPlayer->GetMaxHealth());
	char healthText[32];
	sprintf(healthText, "%i/%i", health, maxHealth);
	m_pHealthLabel->SetText(healthText);

	// Update the player magic
	int magic = static_cast<int>(m_pPlayer->GetMagic());
	int maxMagic = static_cast<int>(m_pPlayer->GetMaxMagic());
	char magicText[32];
	sprintf(magicText, "%i/%i", magic, maxMagic);
	m_pMagicLabel->SetText(magicText);

	// Update the player experience
	int experience = static_cast<int>(m_pPlayer->GetPlayerStats()->GetExperience());
	int maxExperience = static_cast<int>(m_pPlayer->GetPlayerStats()->GetMaxExperience());
	char experienceText[32];
	sprintf(experienceText, "%i/%i", experience, maxExperience);
	m_pExperienceLabel->SetText(experienceText);

	m_updatePlayerHealthTextLocation = true;
	m_updatePlayerMagicTextLocation = true;
	m_updatePlayerExperienceTextLocation = true;
}

// Rendering
void HUD::Render()
{
	if (m_loaded == false)
	{
		return;
	}

	RenderPlayerHealth();
	RenderPlayerMagic();
	RenderPlayerExperience();
	RenderEnemyHealth();
}

void HUD::RenderPlayerHealth()
{
	if (m_updatePlayerHealthTextLocation)
	{
		int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont20(), "%s", m_pHealthLabel->GetText().c_str());
		m_pHealthLabel->SetLocation(static_cast<int>(m_playerHealthX + (m_playerHealthWidth * 0.5f) - static_cast<int>(textWidth * 0.5f)), static_cast<int>(m_playerHealthY));

		m_updatePlayerHealthTextLocation = false;
	}

	// Player health bars
	float playerHealthBarRatio = m_pPlayer->GetHealth() / m_pPlayer->GetMaxHealth();
	if (playerHealthBarRatio >= 1.0f)
	{
		playerHealthBarRatio = 1.0f;
	}

	float playerWidthModified = (m_playerHealthWidth - 0.0f) * playerHealthBarRatio;
	float playerHeightModified = m_playerHealthHeight;

	float playerHealthXModified = m_playerHealthX + 0.0f;
	float playerHealthYModified = m_playerHealthY + 0.0f;

	m_pHealthFillerIcon->SetDimensions(static_cast<int>(playerHealthXModified), static_cast<int>(playerHealthYModified), static_cast<int>(playerWidthModified), static_cast<int>(playerHeightModified));
	m_pHealthFillerBackgroundIcon->SetDimensions(static_cast<int>(playerHealthXModified + playerWidthModified), static_cast<int>(playerHealthYModified), static_cast<int>(m_playerHealthWidth - playerWidthModified + ((m_pPlayer->GetHealth() <= 0.0f || m_pPlayer->GetHealth() >= m_pPlayer->GetMaxHealth()) ? 0 : 1)), static_cast<int>(playerHeightModified));

	// Render the health bar
	m_pRenderer->PushMatrix();
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	m_pRenderer->SetProjectionMode(ProjectionMode::TWO_DIMENSION, CubbyGame::GetInstance()->GetDefaultViewport());

	m_pRenderer->SetLookAtCamera(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_pRenderer->SetRenderMode(RenderMode::SOLID);

	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
	
	//m_pRenderer->ImmediateColorAlpha(0.94f, 0.15f, 0.04f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerHealthXModified, playerHealthYModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerHealthXModified + playerWidthModified, playerHealthYModified, 2.0f);
	//m_pRenderer->ImmediateColorAlpha(0.94f, 0.45f, 0.29f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerHealthXModified + playerWidthModified, playerHealthYModified + playerHeightModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerHealthXModified, playerHealthYModified + playerHeightModified, 2.0f);

	// Background
	//m_pRenderer->ImmediateColorAlpha(0.59f, 0.24f, 0.02f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerHealthXModified, playerHealthYModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerHealthXModified + m_playerHealthWidth, playerHealthYModified, 2.0f);
	//m_pRenderer->ImmediateColorAlpha(1.0f, 0.81f, 0.36f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerHealthXModified + m_playerHealthWidth, playerHealthYModified + playerHeightModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerHealthXModified, playerHealthYModified + playerHeightModified, 2.0f);
	
	m_pRenderer->DisableImmediateMode();

	m_pRenderer->DisableTransparency();
	m_pRenderer->PopMatrix();
}

void HUD::RenderPlayerMagic()
{
	if (m_updatePlayerMagicTextLocation)
	{
		int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont14(), "%s", m_pMagicLabel->GetText().c_str());
		m_pMagicLabel->SetLocation(static_cast<int>(m_playerMagicX + (m_playerMagicWidth * 0.5f) - static_cast<int>(textWidth * 0.5f)), static_cast<int>(m_playerMagicY));

		m_updatePlayerMagicTextLocation = false;
	}

	// Player health bars
	float playerMagicBarRatio = m_pPlayer->GetMagic() / m_pPlayer->GetMaxMagic();
	if (playerMagicBarRatio >= 1.0f)
	{
		playerMagicBarRatio = 1.0f;
	}

	float playerWidthModified = (m_playerMagicWidth - 0.0f) * playerMagicBarRatio;
	float playerHeightModified = m_playerMagicHeight;

	float playerMagicXModified = m_playerMagicX + 0.0f;
	float playerMagicYModified = m_playerMagicY + 0.0f;

	m_pManaFillerIcon->SetDimensions(static_cast<int>(playerMagicXModified), static_cast<int>(playerMagicYModified), static_cast<int>(playerWidthModified), static_cast<int>(playerHeightModified));
	m_pManaFillerBackgroundIcon->SetDimensions(static_cast<int>(playerMagicXModified + playerWidthModified), static_cast<int>(playerMagicYModified), static_cast<int>(m_playerMagicWidth - playerWidthModified + ((m_pPlayer->GetMagic() <= 0.0f || m_pPlayer->GetMagic() >= m_pPlayer->GetMaxMagic()) ? 0 : 1)), static_cast<int>(playerHeightModified));

	// Render the magic bar
	m_pRenderer->PushMatrix();
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	m_pRenderer->SetProjectionMode(ProjectionMode::TWO_DIMENSION, CubbyGame::GetInstance()->GetDefaultViewport());

	m_pRenderer->SetLookAtCamera(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_pRenderer->SetRenderMode(RenderMode::SOLID);

	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
	
	//m_pRenderer->ImmediateColorAlpha(0.137f, 0.419f, 0.517f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerMagicXModified, playerMagicYModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerMagicXModified + playerWidthModified, playerMagicYModified, 2.0f);
	//m_pRenderer->ImmediateColorAlpha(0.298f, 0.674f, 0.803f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerMagicXModified + playerWidthModified, playerMagicYModified + playerHeightModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerMagicXModified, playerMagicYModified + playerHeightModified, 2.0f);

	// Background
	//m_pRenderer->ImmediateColorAlpha(0.59f, 0.24f, 0.02f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerMagicXModified, playerMagicYModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerMagicXModified + m_playerMagicWidth, playerMagicYModified, 2.0f);
	//m_pRenderer->ImmediateColorAlpha(1.0f, 0.81f, 0.36f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerMagicXModified + m_playerMagicWidth, playerMagicYModified + playerHeightModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerMagicXModified, playerMagicYModified + playerHeightModified, 2.0f);
	
	m_pRenderer->DisableImmediateMode();

	m_pRenderer->DisableTransparency();
	m_pRenderer->PopMatrix();
}

void HUD::RenderPlayerExperience()
{
	if (m_updatePlayerExperienceTextLocation)
	{
		int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont18(), "%s", m_pExperienceLabel->GetText().c_str());
		m_pExperienceLabel->SetLocation(static_cast<int>(m_playerExperienceX + (m_playerExperienceWidth * 0.5f) - static_cast<int>(textWidth * 0.5f)), static_cast<int>(m_playerExperienceY));

		m_updatePlayerExperienceTextLocation = false;
	}

	// Player experience bars
	float playerExperienceBarRatio = static_cast<float>(m_pPlayer->GetPlayerStats()->GetExperience()) / static_cast<float>(m_pPlayer->GetPlayerStats()->GetMaxExperience());
	if (playerExperienceBarRatio >= 1.0f)
	{
		playerExperienceBarRatio = 1.0f;
	}

	float playerWidthModified = (m_playerExperienceWidth - 0.0f) * playerExperienceBarRatio;
	float playerHeightModified = m_playerExperienceHeight;

	float playerExperienceXModified = m_playerExperienceX + 0.0f;
	float playerExperienceYModified = m_playerExperienceY + 0.0f;

	m_pExperienceFillerIcon->SetDimensions(static_cast<int>(playerExperienceXModified), static_cast<int>(playerExperienceYModified), static_cast<int>(playerWidthModified), static_cast<int>(playerHeightModified));
	m_pExperienceFillerBackgroundIcon->SetDimensions(static_cast<int>(playerExperienceXModified + playerWidthModified), static_cast<int>(playerExperienceYModified), static_cast<int>(m_playerExperienceWidth - playerWidthModified + ((m_pPlayer->GetPlayerStats()->GetExperience() <= 0.0f || m_pPlayer->GetPlayerStats()->GetExperience() >= m_pPlayer->GetPlayerStats()->GetMaxExperience()) ? 0 : 1)), static_cast<int>(playerHeightModified));

	// Render the experience bar
	m_pRenderer->PushMatrix();
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	m_pRenderer->SetProjectionMode(ProjectionMode::TWO_DIMENSION, CubbyGame::GetInstance()->GetDefaultViewport());

	m_pRenderer->SetLookAtCamera(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_pRenderer->SetRenderMode(RenderMode::SOLID);

	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
	
	//m_pRenderer->ImmediateColorAlpha(0.16f, 0.09f, 0.24f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerExperienceXModified, playerExperienceYModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerExperienceXModified + playerWidthModified, playerExperienceYModified, 2.0f);
	//m_pRenderer->ImmediateColorAlpha(0.67f, 0.37f, 0.95f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerExperienceXModified + playerWidthModified, playerExperienceYModified + playerHeightModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerExperienceXModified, playerExperienceYModified + playerHeightModified, 2.0f);

	// Background
	//m_pRenderer->ImmediateColorAlpha(0.59f, 0.24f, 0.02f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerExperienceXModified, playerExperienceYModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerExperienceXModified + m_playerExperienceWidth, playerExperienceYModified, 2.0f);
	//m_pRenderer->ImmediateColorAlpha(1.0f, 0.81f, 0.36f, 1.0f);
	//m_pRenderer->ImmediateVertex(playerExperienceXModified + m_playerExperienceWidth, playerExperienceYModified + playerHeightModified, 2.0f);
	//m_pRenderer->ImmediateVertex(playerExperienceXModified, playerExperienceYModified + playerHeightModified, 2.0f);
	
	m_pRenderer->DisableImmediateMode();
	m_pRenderer->DisableTransparency();

	m_pRenderer->PopMatrix();
}

void HUD::RenderEnemyHealth()
{
	if (m_updateEnemyTextLocation)
	{
		int textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont20(), "%s", m_pEnemyHealthLabel->GetText().c_str());
		m_pEnemyHealthLabel->SetLocation(static_cast<int>((m_windowWidth * 0.5f) - static_cast<int>(textWidth * 0.5f)), static_cast<int>(m_enemyHealthY) + 7);

		textWidth = m_pRenderer->GetFreeTypeTextWidth(m_pFrontendManager->GetFrontendFont30(), "%s", m_pEnemyNameLabel->GetText().c_str());
		m_pEnemyNameLabel->SetLocation(static_cast<int>((m_windowWidth * 0.5f) - static_cast<int>(textWidth * 0.5f)), static_cast<int>(m_enemyHealthY) + 35);

		m_updateEnemyTextLocation = false;
	}

	bool componentStillRender = false;
	if (m_pGUI->GetComponent(m_pEnemyHealthContainerIcon) == m_pEnemyHealthContainerIcon)
	{
		componentStillRender = true;
	}

	if (componentStillRender || (m_enemyInfoDisappearTimer > 0.0f && m_removedEnemyInfo == false))
	{
	    // Enemy health bar
		float enemyHealthBarRatio = m_enemyHealth / m_enemyHealthMax;
		if (enemyHealthBarRatio >= 1.0f)
		{
			enemyHealthBarRatio = 1.0f;
		}

		float enemyWidthModified = (m_enemyHealthWidth - 0.0f) * enemyHealthBarRatio;
		float enemyHeightModified = m_enemyHealthHeight;

		float enemyHealthXModified = m_enemyHealthX + 4.0f;
		float enemyHealthYModified = m_enemyHealthY + 4.0f;

		enemyWidthModified -= 8.0f;
		enemyHeightModified -= 8.0f;

		//float enemyYellowX = enemyHealthXModified + enemyWidthModified;
		//float enemyYellowY = enemyHealthYModified;
		//float enemyYellowWidth = (m_enemyHealthWidthDecreaser * (m_enemyHealthWidth / m_enemyHealthMax));

		m_pEnemyHealthFillerIcon->SetDimensions(static_cast<int>(enemyHealthXModified), static_cast<int>(enemyHealthYModified), static_cast<int>(enemyWidthModified), static_cast<int>(enemyHeightModified));
		m_pEnemyHealthFillerBackgroundIcon->SetDimensions(static_cast<int>(enemyHealthXModified + enemyWidthModified + (m_enemyHealth <= 0.0f ? 8.0f : 0.0f)), static_cast<int>(enemyHealthYModified), static_cast<int>(m_enemyHealthWidth - enemyWidthModified - (m_enemyHealth > 0.0f ? 8.0f : 16.0f)), static_cast<int>(enemyHeightModified));

		// Render the health bar
		m_pRenderer->PushMatrix();
		m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		glLoadIdentity();
		m_pRenderer->SetProjectionMode(ProjectionMode::TWO_DIMENSION, CubbyGame::GetInstance()->GetDefaultViewport());

		m_pRenderer->SetLookAtCamera(glm::vec3(0.0f, 0.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_pRenderer->SetRenderMode(RenderMode::SOLID);

		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
		
		//m_pRenderer->ImmediateColorAlpha(0.94f, 0.15f, 0.04f, 1.0f);
		//m_pRenderer->ImmediateVertex(enemyHealthXModified, enemyHealthYModified, 2.0f);
		//m_pRenderer->ImmediateVertex(enemyHealthXModified + enemyWidthModified, enemyHealthYModified, 2.0f);
		//m_pRenderer->ImmediateColorAlpha(0.94f, 0.45f, 0.29f, 1.0f);
		//m_pRenderer->ImmediateVertex(enemyHealthXModified + enemyWidthModified, enemyHealthYModified + enemyHeightModified, 2.0f);
		//m_pRenderer->ImmediateVertex(enemyHealthXModified, enemyHealthYModified + enemyHeightModified, 2.0f);

		// Width decreaser
		//m_pRenderer->ImmediateColorAlpha(1.0f, 0.75f, 0.0f, 1.0f);
		//m_pRenderer->ImmediateVertex(enemyYellowX, enemyYellowY, 2.0f);
		//m_pRenderer->ImmediateVertex(enemyYellowX + enemyYellowWidth, enemyYellowY, 2.0f);
		//m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.5f, 1.0f);
		//m_pRenderer->ImmediateVertex(enemyYellowX + enemyYellowWidth, enemyYellowY + enemyHeightModified, 2.0f);
		//m_pRenderer->ImmediateVertex(enemyYellowX, enemyYellowY + enemyHeightModified, 2.0f);

		// Background
		//m_pRenderer->ImmediateColorAlpha(0.59f, 0.24f, 0.02f, 1.0f);
		//m_pRenderer->ImmediateVertex(enemyHealthXModified, enemyHealthYModified, 2.0f);
		//m_pRenderer->ImmediateVertex(enemyHealthXModified + m_enemyHealthWidth, enemyHealthYModified, 2.0f);
		//m_pRenderer->ImmediateColorAlpha(1.0f, 0.81f, 0.36f, 1.0f);
		//m_pRenderer->ImmediateVertex(enemyHealthXModified + m_enemyHealthWidth, enemyHealthYModified + enemyHeightModified, 2.0f);
		//m_pRenderer->ImmediateVertex(enemyHealthXModified, enemyHealthYModified + enemyHeightModified, 2.0f);
		
		m_pRenderer->DisableImmediateMode();
		m_pRenderer->DisableTransparency();
		
		m_pRenderer->PopMatrix();
	}
}

void HUD::_DeathTextFinished(void* pData)
{
	HUD* pHUD = static_cast<HUD*>(pData);
	pHUD->DeathTextFinished();
}

void HUD::DeathTextFinished() const
{
	m_pRespawnButton->SetLabelColor(m_pFrontendManager->GetNormalFontColor());
	m_pGUI->AddComponent(m_pRespawnButton);

	CubbyGame::GetInstance()->TurnCursorOn(true, false);
}

void HUD::_RespawnPressed(void* pData)
{
	HUD* pHUD = static_cast<HUD*>(pData);
	pHUD->RespawnPressed();
}

void HUD::RespawnPressed()
{
	m_pPlayer->Respawn();

	PlayerRespawn();

	CubbyGame::GetInstance()->PlayerRespawned();
	CubbyGame::GetInstance()->InitializeCameraRotation();

	CubbyGame::GetInstance()->TurnCursorOff(false);
}

void HUD::_LevelUpTextFinished(void* pData)
{
	HUD* pHUD = static_cast<HUD*>(pData);
	pHUD->LevelUpTextFinished();
}

void HUD::LevelUpTextFinished()
{
	m_levelUp = false;

	m_pGUI->RemoveComponent(m_pLevelUpLabel);
}

// TODO : Checkboxes for GUI panels on the HUD
//void HUD::_InventoryCheckboxPressed(void* pData)
//{
//	HUD* pHUD = static_cast<HUD*>(pData);
//	pHUD->InventoryCheckboxPressed();
//}
//
//void HUD::InventoryCheckboxPressed()
//{
//	if(m_pGameWindow->IsPaused())
//	{
//		return;
//	}
//
//	if(m_pInventoryGUI->IsLoaded())
//	{
//		m_pInventoryGUI->Unload();
//
//		if(m_pGameWindow->IsGUIWindowStillDisplayed() == false)
//		{
//			m_pGameWindow->TurnCursorOff();
//		}
//	}
//	else
//	{
//		m_pInventoryGUI->Load();
//	}
//}
//
//void HUD::_CharacterCheckboxPressed(void* pData)
//{
//	HUD* pHUD = static_cast<HUD*>(pData);
//	pHUD->CharacterCheckboxPressed();
//}
//
//void HUD::CharacterCheckboxPressed()
//{
//	if(m_pGameWindow->IsPaused())
//	{
//		return;
//	}
//
//	if(m_pCharacterGUI->IsLoaded())
//	{
//		m_pCharacterGUI->Unload();
//
//		if(m_pGameWindow->IsGUIWindowStillDisplayed() == false)
//		{
//			m_pGameWindow->TurnCursorOff();
//		}
//	}
//	else
//	{
//		m_pCharacterGUI->Load();
//	}
//}
//
//void HUD::_QuestCheckboxPressed(void* pData)
//{
//	HUD* pHUD = static_cast<HUD*>(pData);
//	pHUD->QuestCheckboxPressed();
//}
//
//void HUD::QuestCheckboxPressed()
//{
//	if(m_pGameWindow->IsPaused())
//	{
//		return;
//	}
//
//	if(m_pQuestGUI->IsLoaded())
//	{
//		m_pQuestGUI->Unload();
//
//		if(m_pGameWindow->IsGUIWindowStillDisplayed() == false)
//		{
//			m_pGameWindow->TurnCursorOff();
//		}
//	}
//	else
//	{
//		m_pQuestGUI->Load();
//	}
//}
//
//void HUD::_CraftingCheckboxPressed(void* pData)
//{
//	HUD* pHUD = static_cast<HUD*>(pData);
//	pHUD->CraftingCheckboxPressed();
//}
//
//void HUD::CraftingCheckboxPressed()
//{
//	if(m_pGameWindow->IsPaused())
//	{
//		return;
//	}
//
//	if(m_pCraftingGUI->IsLoaded())
//	{
//		m_pCraftingGUI->Unload();
//
//		if(m_pGameWindow->IsGUIWindowStillDisplayed() == false)
//		{
//			m_pGameWindow->TurnCursorOff();
//		}
//	}
//	else
//	{
//		m_pCraftingGUI->SetCraftingRecipesForItem(ItemType::None);
//		m_pCraftingGUI->Load();
//	}
//}
//
//void HUD::_SkillsCheckboxPressed(void* pData)
//{
//	HUD* pHUD = static_cast<HUD*>(pData);
//	pHUD->SkillsCheckboxPressed();
//}
//
//void HUD::SkillsCheckboxPressed()
//{
//	if(m_pGameWindow->IsPaused())
//	{
//		return;
//	}
//
//	if(m_pSkillsGUI->IsLoaded())
//	{
//		m_pSkillsGUI->Unload();
//
//		if(m_pGameWindow->IsGUIWindowStillDisplayed() == false)
//		{
//			m_pGameWindow->TurnCursorOff();
//		}
//	}
//	else
//	{
//		m_pSkillsGUI->Load();
//	}
//}
//
//void HUD::_PetCheckboxPressed(void* pData)
//{
//	HUD* pHUD = static_cast<HUD*>(pData);
//	pHUD->PetCheckboxPressed();
//}
//
//void HUD::PetCheckboxPressed()
//{
//	if(m_pGameWindow->IsPaused())
//	{
//		return;
//	}
//
//	if(m_pCompanionPet->IsSummoned())
//	{
//		m_pCompanionPet->UnsummonPet();
//	}
//	else
//	{
//		m_pCompanionPet->SummonPet();
//	}
//}