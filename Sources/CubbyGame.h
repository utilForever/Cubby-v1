/*************************************************************************
> File Name: CubbyGame.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The Cubby game class houses all the game functionality and logic that directly
>    interfaces with the game subsystems. Also this game class is the container
>    for all the renderer objects that are required to draw the scene, such as
>    shaders, viewports, frame buffers, etc. Finally this class also owns all
>    the GUI components that are created to handle user input.
> Created Time: 2016/07/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_GAME_H
#define CUBBY_GAME_H

#include <Blocks/BiomeManager.h>
#include <Blocks/ChunkManager.h>
#include <Enemy/EnemyManager.h>
#include <Frontend/FrontendManager.h>
#include <GameGUI/ActionBar.h>
#include <GameGUI/CharacterGUI.h>
#include <GameGUI/CraftingGUI.h>
#include <GameGUI/HUD.h>
#include <GameGUI/InventoryGUI.h>
#include <GameGUI/LootGUI.h>
#include <GameGUI/QuestGUI.h>
#include <GUI/OpenGLGUI.h>
#include <GUI/OptionController.h>
#include <Instance/InstanceManager.h>
#include <Inventory/InventoryManager.h>
#include <Items/ItemManager.h>
#include <Items/RandomLootManager.h>
#include <Lighting/LightingManager.h>
#include <Mods/ModsManager.h>
#include <NPC/NPCManager.h>
#include <Particles/BlockParticleManager.h>
#include <Player/Player.h>
#include <Projectile/ProjectileManager.h>
#include <Quests/QuestJournal.h>
#include <Quests/QuestManager.h>
#include <Renderer/Camera.h>
#include <Renderer/Renderer.h>
#include <Scenery/SceneryManager.h>
#include <Skybox/Skybox.h>
#include <Sounds/SoundManager.h>
#include <Sounds/SoundEffects.h>
#include <TextEffects/TextEffectsManager.h>

#include "CubbySettings.h"
#include "CubbyWindow.h"

#ifdef __linux__
typedef struct POINT {
	float x;
	float y;
} POINT;
#endif //__linux__

// Game modes
enum class GameMode
{
	Debug = 0,
	Loading,
	FrontEnd,
	Game,
};

// Camera modes
enum class CameraMode
{
	Debug = 0,
	Frontend,
	MouseRotate,
	AutoCamera,
	FirstPerson,
	NPCDialog,
	EnemyTarget,
};

class CubbyGame
{
public:
	static CubbyGame* GetInstance();

	// Creation
	void Create(CubbySettings* pCubbySettings);

	// Destruction
	void Destroy();
	void DestroyGUI();

	// Quitting
	void CancelQuitPopup();
	void ShowQuitPopup();
	void SetGameQuit(bool quit);

	// Pause
	bool IsPaused() const;
	void SetPaused(bool pause);
	void SetPauseMenu();
	void UnsetPauseMenu();

	// Blur
	void SetGlobalBlurAmount(float blurAmount);

	// Cinematic letterbox
	void OpenLetterBox();
	void CloseLetterBox();

	// Paperdoll rendering
	void SetPaperdollRotation(float rotation);
	void RotatePaperdollModel(float rot);
	unsigned int GetDynamicPaperdollTexture() const;

	// Portrait
	unsigned int GetDynamicPortraitTexture() const;

	// Events
	void PollEvents() const;
	bool ShouldClose() const;

	// Window functionality
	int GetWindowCursorX() const;
	int GetWindowCursorY() const;
	void TurnCursorOn(bool resetCursorPosition, bool forceOn);
	void TurnCursorOff(bool forceOff);
	bool IsCursorOn() const;
	void ResizeWindow(int width, int height);
	void CloseWindow();
	void UpdateJoySticks() const;

	// Music
	void StartFrontEndMusic();
	void StartGameMusic();
	void StopMusic();
	void UpdateGameMusic(float dt);
	void UpdateMusicVolume(float dt) const;

	// Sounds
	void PlaySoundEffect(SoundEffect soundEffect, float soundEnhanceMultiplier = 1.0f) const;
	void PlaySoundEffect3D(SoundEffect soundEffect, glm::vec3 soundPosition, float soundEnhanceMultiplier = 1.0f) const;

	// Controls
	void UpdateControls(float dt);
	void UpdateKeyboardControls(float dt);
	void UpdateMouseControls(float dt);
	void UpdateGamePadControls(float dt);

	// Camera controls
	void UpdateCamera(float dt);
	void UpdateCameraModeSwitching();
	void InitializeCameraRotation();
	void UpdateCameraAutoCamera(float dt, bool updateCameraPosition);
	void UpdateCameraFirstPerson(float dt) const;
	void UpdateCameraNPCDialog(float dt) const;
	void UpdateCameraEnemyTarget(float dt);
	void UpdateCameraClipping(float dt);
	void UpdateCameraZoom(float dt);
	bool ShouldRestorePreviousCameraMode() const;
	void SavePreviousCameraMode();
	void RestorePreviousCameraMode();

	// Input
	void KeyPressed(int key, int scancode, int mods);
	void KeyReleased(int key, int scancode, int mods);
	void CharacterEntered(int keyCode) const;
	void MouseLeftPressed();
	void MouseLeftReleased();
	void MouseRightPressed();
	void MouseRightReleased();
	void MouseMiddlePressed() const;
	void MouseMiddleReleased() const;
	void MouseScroll(double x, double y);
	void WrapCameraZoomValue();

	// Mouse controls
	void MouseCameraRotate();

	// Joystick controls
	void JoystickCameraMove(float dt) const;
	void JoystickCameraRotate(float dt) const;
	void JoystickCameraZoom(float dt);

	// Game functions
	void QuitToFrontEnd();
	void SetupDataForGame() const;
	void SetupDataForFrontEnd() const;
	void StartGameFromFrontEnd();
	void PlayerRespawned();
	void SetGameMode(GameMode mode);
	GameMode GetGameMode() const;
	void SetCameraMode(CameraMode mode);
	CameraMode GetCameraMode() const;

	// Interactions
	bool CheckInteractions();
	Item* GetInteractItem() const;

	// Enemy Targeting
	void SetEnemyTarget();
	void ReleaseEnemyTarget();

	// Updating
	void Update();
	void UpdateNamePicking();
	void UpdatePlayerAlpha(float dt) const;
	void UpdateLights(float dt);
	void UpdateGUI(float dt);
	void UpdateGameGUI(float dt) const;

	// Rendering
	void PreRender() const;
	void BeginShaderRender() const;
	void EndShaderRender() const;
	void Render();
	void RenderSkybox() const;
	void RenderShadows() const;
	void RenderWaterReflections() const;
	void RenderWater() const;
	void RenderDeferredLighting() const;
	void RenderTransparency() const;
	void RenderSSAOTexture() const;
	void RenderFXAATexture() const;
	void RenderFirstPassFullScreen() const;
	void RenderSecondPassFullScreen() const;
	void RenderGUI() const;
	void RenderHUD() const;
	void RenderCinematicLetterBox() const;
	void RenderCrosshair() const;
	void RenderCustomCursor() const;
	void RenderPaperdollViewport() const;
	void RenderPortraitViewport() const;
	void RenderFirstPersonViewport() const;
	void RenderDeferredRenderingPaperDoll() const;
	void RenderDeferredRenderingPortrait() const;
	void RenderDebugInformation();

	// GUI Helper functions
	bool IsGUIWindowStillDisplayed() const;
	void CloseAllGUIWindows() const;
	void CloseInteractionGUI();

	// GUI
	void CreateGUI();
	void SetupGUI();
	void SkinGUI() const;
	void UnSkinGUI() const;
	void GUITurnOffCursor() const;
	void ShowGUI() const;
	void HideGUI() const;
	void UpdateCharactersPulldown() const;
	void UpdateWeaponsPulldown() const;
	void UpdateAnimationsPulldown() const;
	void AddConsoleLabel(std::string message);
	void ClearConsoleLabels();
	void UpdateConsoleLabels();
	void ToggleFullScreenPressed();
	
	// Accessors
	unsigned int GetDefaultViewport() const;
	Camera* GetGameCamera() const;
	Player* GetPlayer() const;
	ChunkManager* GetChunkManager() const;
	BiomeManager* GetBiomeManager() const;
	FrontendManager* GetFrontendManager() const;
	BlockParticleManager* GetBlockParticleManager() const;
	NPCManager* GetNPCManager() const;
	ItemManager* GetItemManager() const;
	InventoryManager* GetInventoryManager() const;
	RandomLootManager* GetRandomLootManager() const;
	ModsManager* GetModsManager() const;
	CharacterGUI* GetCharacterGUI() const;
	QuestGUI* GetQuestGUI() const;
	HUD* GetHUD() const;
	ActionBar* GetActionBar() const;
	CubbySettings* GetCubbySettings() const;

protected:
	CubbyGame() = default;
	CubbyGame(const CubbyGame&) = delete;
	CubbyGame(CubbyGame&&) = delete;
	CubbyGame& operator=(const CubbyGame&) = delete;
	CubbyGame& operator=(CubbyGame&&) = delete;

	// GUI callbacks
	static void _PlayAnimationPressed(void* pData);
	void PlayAnimationPressed() const;

	static void _AnimationPullDownChanged(void* pData);
	void AnimationPullDownChanged() const;

	static void _CharacterPullDownChanged(void* pData);
	void CharacterPullDownChanged() const;

	static void _GameModeChanged(void* pData);
	void GameModeChanged();

	static void _CameraModeChanged(void* pData);
	void CameraModeChanged();

	static void _FaceMergeCheckboxChanged(void* pData);
	void FaceMergeCheckboxChanged() const;

	static void _StepUpdatePressed(void* pData);
	void StepUpdatePressed() const;

	static void _ConsoleReturnPressed(void* pData);
	void ConsoleReturnPressed();

private:
	CubbyWindow* m_pCubbyWindow;
	CubbySettings* m_pCubbySettings;

	// Renderer
	Renderer* m_pRenderer;

	// GUI
	OpenGLGUI* m_pGUI;

	// Game camera
	Camera* m_pGameCamera;

	// Qubicle binary manager
	QubicleBinaryManager* m_pQubicleBinaryManager;

	// Player
	Player* m_pPlayer;

	// NPC Manager
	NPCManager* m_pNPCManager;

	// Enemy manager
	EnemyManager* m_pEnemyManager;

	// Inventory
	InventoryManager* m_pInventoryManager;

	// Items
	ItemManager* m_pItemManager;

	// Random loot
	RandomLootManager* m_pRandomLootManager;

	// Projectile manager
	ProjectileManager* m_pProjectileManager;

	// Quest manager
	QuestManager* m_pQuestManager;

	// Quest journal
	QuestJournal* m_pQuestJournal;

	// Chunk manager
	ChunkManager* m_pChunkManager;

	// Biome manager
	BiomeManager* m_pBiomeManager;

	// Scenery
	SceneryManager* m_pSceneryManager;

	// Lighting manager
	LightingManager* m_pLightingManager;

	// Skybox
	Skybox* m_pSkybox;

	// Block particle manager
	BlockParticleManager* m_pBlockParticleManager;

	// Text effects
	TextEffectsManager* m_pTextEffectsManager;

	// Instance manager
	InstanceManager* m_pInstanceManager;

	// Frontend manager
	FrontendManager* m_pFrontendManager;

	// Mods manager
	ModsManager* m_pModsManager;

	// Mouse picking
	int m_pickedObject;
	bool m_isNamePickingSelected;

	// Game mode
	GameMode m_gameMode;
	bool m_allowToChangeToGame;
	bool m_allowToChangeToFrontend;

	// Camera mode
	CameraMode m_cameraMode;
	CameraMode m_previousCameraMode;
	bool m_shouldRestorePreviousCameraMode;

	// Biome
	Biome m_currentBiome;

	// Interacting item
	tthread::mutex m_interactItemMutex;
	Item* m_pInteractItem;

	// Window width and height
	int m_windowWidth;
	int m_windowHeight;

	// Quit message
	bool m_isGameQuit;

	// Paused
	bool m_isPaused;

	// View ports
	unsigned int m_defaultViewport;
	unsigned int m_firstpersonViewport;
	unsigned int m_paperdollViewport;
	unsigned int m_portraitViewport;

	// Fonts
	unsigned int m_defaultFont;

	// Lights
	unsigned int m_defaultLight;
	glm::vec3 m_defaultLightPosition;
	glm::vec3 m_defaultLightView;

	// Materials
	unsigned int m_defaultMaterial;

	// Frame buffers
	unsigned int m_SSAOFrameBuffer;
	unsigned int m_shadowFrameBuffer;
	unsigned int m_lightingFrameBuffer;
	unsigned int m_transparencyFrameBuffer;
	unsigned int m_waterReflectionFrameBuffer;
	unsigned int m_FXAAFrameBuffer;
	unsigned int m_firstPassFullscreenBuffer;
	unsigned int m_secondPassFullscreenBuffer;
	unsigned int m_paperdollBuffer;
	unsigned int m_paperdollSSAOTextureBuffer;
	unsigned int m_portraitBuffer;
	unsigned int m_portraitSSAOTextureBuffer;

	// Shaders
	unsigned int m_defaultShader;
	unsigned int m_phongShader;
	unsigned int m_SSAOShader;
	unsigned int m_shadowShader;
	unsigned int m_waterShader;
	unsigned int m_lightingShader;
	unsigned int m_cubeMapShader;
	unsigned int m_textureShader;
	unsigned int m_fxaaShader;
	unsigned int m_blurVerticalShader;
	unsigned int m_blurHorizontalShader;
	unsigned int m_paperdollShader;

	// Custom cursor textures
	unsigned int m_customCursorNormalBuffer;
	unsigned int m_customCursorClickedBuffer;
	unsigned int m_customCursorRotateBuffer;
	unsigned int m_customCursorZoomBuffer;

	// Paperdoll viewport
	int m_paperdollViewportX;
	int m_paperdollViewportY;
	int m_paperdollViewportWidth;
	int m_paperdollViewportHeight;

	// Portrait viewport
	int m_portraitViewportX;
	int m_portraitViewportY;
	int m_portraitViewportWidth;
	int m_portraitViewportHeight;

	// FPS and deltatime
#ifdef _WIN32
	LARGE_INTEGER m_fpsPreviousTicks;
	LARGE_INTEGER m_fpsCurrentTicks;
	LARGE_INTEGER m_fpsTicksPerSecond;
#else
	double m_fpsPreviousTicks;
	double m_fpsCurrentTicks;
#endif //_WIN32
	float m_deltaTime;
	float m_fps;

	// Initial starting wait timer
	float m_initialWaitTimer;
	float m_initialWaitTime;
	bool m_initialStartWait;

	// Keyboard flags
	bool m_isKeyboardForward;
	bool m_isKeyboardBackward;
	bool m_isKeyboardStrafeLeft;
	bool m_isKeyboardStrafeRight;
	bool m_isKeyboardLeft;
	bool m_isKeyboardRight;
	bool m_isKeyboardUp;
	bool m_isKeyboardDown;
	bool m_isKeyboardSpace;
	bool m_isKeyboardMenu;

	// Joystick flags
	bool m_isJoystickJump;

	// Custom cursor
	bool m_isPressedCursorDown;
	bool m_isCustomCursorOn;

	// Combat flags
	bool m_isAttackPressedMouse;
	bool m_isAttackReleasedMouse;
	bool m_canDoAttackMouse;
	bool m_isAttackPressedJoystick;
	bool m_isAttackReleasedJoystick;
	bool m_canDoAttackJoystick;
	bool m_isTargetEnemyPressedJoystick;
	bool m_isTargetEnemyReleasedJoystick;

	// Camera movement
	bool m_isCameraRotate;
	int m_pressedX;
	int m_pressedY;
	int m_currentX;
	int m_currentY;
	float m_cameraDistance;
	float m_maxCameraDistance;

	// Auto camera mode
	glm::vec3 m_cameraPositionAutoModeCached;
	glm::vec3 m_cameraPositionAutoMode;
	glm::vec3 m_cameraBehindPlayerPosition;
	float m_autoCameraMovingModifier;

	// NPC dialog camera mode
	glm::vec3 m_targetCameraViewNPCDialog;
	glm::vec3 m_targetCameraPositionNPCDialog;

	// Enemy target camera mode
	float m_targetCameraXAxisAmount;
	float m_targetCameraXAxisAmountTarget;
	float m_targetCameraYRatio;
	float m_targetCameraForwardRatio;

	// Camera clipping
	glm::vec3 m_targetCameraPositionBeforeClipping;
	glm::vec3 m_cameraPositionAfterClipping;

	// Player movement
	bool m_keyboardMovement;
	bool m_gamepadMovement;
	glm::vec3 m_movementDirection;
	float m_movementSpeed;
	float m_movementDragTime;
	float m_movementIncreaseTime;
	float m_maxMovementSpeed;
	float m_movementStopThreshold;

	// Blur
	float m_globalBlurAmount;

	// Cinematic letterbox mode
	float m_letterBoxRatio;

	// Water
	float m_elapsedWaterTime;

	// Paperdoll rendering
	float m_paperdollRenderRotation;

	// Game GUI
	InventoryGUI* m_pInventoryGUI;
	CharacterGUI* m_pCharacterGUI;
	LootGUI* m_pLootGUI;
	CraftingGUI* m_pCraftingGUI;
	QuestGUI* m_pQuestGUI;
	ActionBar* m_pActionBar;
	HUD* m_pHUD;

	// Music and sound
	FMOD::Channel* m_pMusicChannel;
	FMOD::Sound* m_pMusicSound;
	Biome m_currentBiomeMusic;

	// GUI Components
	bool m_GUICreated;
	GUIWindow* m_pMainWindow;
	CheckBox* m_pSSAOCheckBox;
	CheckBox* m_pDynamicLightingCheckBox;
	CheckBox* m_pWireframeCheckBox;
	CheckBox* m_pMSAACheckBox;
	CheckBox* m_pDeferredCheckBox;
	CheckBox* m_pUpdateCheckBox;
	CheckBox* m_pBlurCheckBox;
	CheckBox* m_pDebugRenderCheckBox;
	CheckBox* m_pInstanceRenderCheckBox;
	Button* m_pPlayAnimationButton;
	PulldownMenu* m_pAnimationsPulldown;
	PulldownMenu* m_pCharacterPulldown;
	GUIWindow* m_pGameWindow;
	OptionBox* m_pGameOptionBox;
	OptionBox* m_pDebugOptionBox;
	OptionBox* m_pFrontEndOptionBox;
	OptionController* m_pGameModeOptionController;
	OptionBox* m_pDebugCameraOptionBox;
	OptionBox* m_pMouseRotateCameraOptionBox;
	OptionBox* m_pAutoCameraOptionBox;
	OptionBox* m_pFrontendCameraOptionBox;
	OptionController* m_pCameraModeOptionController;
	CheckBox* m_pFaceMergingCheckbox;
	CheckBox* m_pStepUpdateCheckbox;
	Button* m_pStepUpdateButton;
	GUIWindow* m_pConsoleWindow;
	TextBox* m_pConsoleTextbox;
	ScrollBar* m_pConsoleScrollbar;
	std::vector<Label*> m_vpConsoleLabels;
	std::vector<Label*> m_vpConsoleLabels_Add;
	std::vector<std::string> m_vStringCache;

	// Toggle flags
	bool m_deferredRendering;
	bool m_modelWireframe;
	int m_modelAnimationIndex;
	bool m_multiSampling;
	bool m_ssao;
	bool m_blur;
	bool m_dynamicLighting;
	bool m_animationUpdate;
	bool m_fullscreen;
	bool m_debugRender;
	bool m_instanceRender;

	// Singleton instance
	static CubbyGame* m_instance;
};

#endif