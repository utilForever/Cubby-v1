/*************************************************************************
> File Name: CubbyGame.cpp
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

#ifdef __linux__
#include <sys/time.h>
#endif //__linux__

#include <glew/include/GL/glew.h>
#include <glm/detail/func_geometric.hpp>

#include <Utils/Interpolator.h>

#include "CubbyGame.h"

extern std::string g_soundEffectFileNames[static_cast<int>(SoundEffect::NumSoundEffect)];

// Initialize the singleton instance
CubbyGame* CubbyGame::m_instance = nullptr;

CubbyGame* CubbyGame::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new CubbyGame;
	}

	return m_instance;
}

// Creation
void CubbyGame::Create(CubbySettings* pCubbySettings)
{
	m_pRenderer = nullptr;
	m_pGameCamera = nullptr;
	m_pQubicleBinaryManager = nullptr;
	m_pPlayer = nullptr;
	m_pChunkManager = nullptr;
	m_pFrontendManager = nullptr;

	m_pInventoryGUI = nullptr;
	m_pCharacterGUI = nullptr;
	m_pLootGUI = nullptr;
	m_pCraftingGUI = nullptr;
	m_pQuestGUI = nullptr;
	m_pActionBar = nullptr;
	m_pHUD = nullptr;

	m_GUICreated = false;

	m_pCubbySettings = pCubbySettings;
	m_pCubbyWindow = new CubbyWindow(this, m_pCubbySettings);

	// Create the window
	m_pCubbyWindow->Create();

	// Setup the FPS and delta time counters
#ifdef _WIN32
	QueryPerformanceCounter(&m_fpsPreviousTicks);
	QueryPerformanceCounter(&m_fpsCurrentTicks);
	QueryPerformanceFrequency(&m_fpsTicksPerSecond);
#else
	struct timeval tm;
	gettimeofday(&tm, nullptr);
	m_fpsCurrentTicks = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
	m_fpsPreviousTicks = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;
#endif //_WIN32
	m_deltaTime = 0.0f;
	m_fps = 0.0f;

	// Mouse name picking 
	m_pickedObject = -1;
	m_isNamePickingSelected = false;

	// Custom cursors
	m_isPressedCursorDown = false;
	m_isCustomCursorOn = false;

	// Paper doll viewport dimensions
	m_paperdollViewportX = 0;
	m_paperdollViewportY = 0;
	m_paperdollViewportWidth = 800;
	m_paperdollViewportHeight = 800;

	// Portrait viewport dimensions
	m_portraitViewportX = 0;
	m_portraitViewportY = 0;
	m_portraitViewportWidth = 800;
	m_portraitViewportHeight = 800;

	// Setup the initial starting wait timing
	m_initialWaitTimer = 0.0f;
	m_initialWaitTime = 0.5f;
	m_initialStartWait = true;

	// Create the renderer
	m_windowWidth = m_pCubbyWindow->GetWindowWidth();
	m_windowHeight = m_pCubbyWindow->GetWindowHeight();
	m_pRenderer = new Renderer(m_windowWidth, m_windowHeight, 32, 8);

	// Pause and quit
	m_isGameQuit = false;
	m_isPaused = false;

	// Interactions
	m_pInteractItem = nullptr;

	// Biome
	m_currentBiome = Biome::None;

	// Music and Sound
	m_pMusicChannel = nullptr;
	m_pMusicSound = nullptr;
	m_currentBiomeMusic = Biome::None;

	// Create the GUI
	m_pGUI = new OpenGLGUI(m_pRenderer);

	// Create cameras
	m_pGameCamera = new Camera(m_pRenderer);
	m_pGameCamera->SetPosition(glm::vec3(8.0f, 8.25f, 15.5f));
	m_pGameCamera->SetFakePosition(m_pGameCamera->GetPosition());
	m_pGameCamera->SetFacing(glm::vec3(0.0f, 0.0f, -1.0f));
	m_pGameCamera->SetUp(glm::vec3(0.0f, 1.0f, 0.0f));
	m_pGameCamera->SetRight(glm::vec3(1.0f, 0.0f, 0.0f));

	// Create viewports
	m_pRenderer->CreateViewport(0, 0, m_windowWidth, m_windowHeight, 60.0f, &m_defaultViewport);
	m_pRenderer->CreateViewport(0, 0, m_windowWidth, m_windowHeight, 60.0f, &m_firstpersonViewport);
	m_pRenderer->CreateViewport(m_paperdollViewportY, m_paperdollViewportX, m_paperdollViewportWidth, m_paperdollViewportHeight, 60.0f, &m_paperdollViewport);
	m_pRenderer->CreateViewport(m_portraitViewportY, m_portraitViewportX, m_portraitViewportWidth, m_portraitViewportHeight, 60.0f, &m_portraitViewport);

	// Create fonts
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/arial.ttf", 12, &m_defaultFont);

	// Create the custom cursor textures
	int lTextureWidth, lTextureHeight, lTextureWidth2, lTextureHeight2;
	m_pRenderer->LoadTexture("Resources/textures/cursors/finger_cursor_normal.tga", &lTextureWidth, &lTextureHeight, &lTextureWidth2, &lTextureHeight2, &m_customCursorNormalBuffer);
	m_pRenderer->LoadTexture("Resources/textures/cursors/finger_cursor_clicked.tga", &lTextureWidth, &lTextureHeight, &lTextureWidth2, &lTextureHeight2, &m_customCursorClickedBuffer);
	m_pRenderer->LoadTexture("Resources/textures/cursors/finger_cursor_rotate.tga", &lTextureWidth, &lTextureHeight, &lTextureWidth2, &lTextureHeight2, &m_customCursorRotateBuffer);
	m_pRenderer->LoadTexture("Resources/textures/cursors/finger_cursor_zoom.tga", &lTextureWidth, &lTextureHeight, &lTextureWidth2, &lTextureHeight2, &m_customCursorZoomBuffer);

	// Create lights
	m_defaultLightPosition = glm::vec3(300.0f, 300.0f, 300.0f);
	m_defaultLightView = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lightDirection = m_defaultLightView - m_defaultLightPosition;
	m_pRenderer->CreateLight(Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f),
		m_defaultLightPosition, lightDirection, 0.0f, 0.0f, 2.0f, 0.001f, 0.0f, true, false, &m_defaultLight);

	// Create materials
	m_pRenderer->CreateMaterial(Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.0f, 1.0f), 64, &m_defaultMaterial);

	// Create the frame buffers
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "SSAO", &m_SSAOFrameBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 5.0f, "Shadow", &m_shadowFrameBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "Deferred Lighting", &m_lightingFrameBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "Transparency", &m_transparencyFrameBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "Water Reflection", &m_waterReflectionFrameBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "FXAA", &m_FXAAFrameBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "FullScreen 1st Pass", &m_firstPassFullscreenBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "FullScreen 2nd Pass", &m_secondPassFullscreenBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, 800, 800, 1.0f, "Paperdoll", &m_paperdollBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, 800, 800, 1.0f, "Paperdoll SSAO Texture", &m_paperdollSSAOTextureBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, 800, 800, 1.0f, "Portrait", &m_portraitBuffer);
	m_pRenderer->CreateFrameBuffer(-1, true, true, true, true, 800, 800, 1.0f, "Portrait SSAO Texture", &m_portraitSSAOTextureBuffer);

	// Create the shaders
	m_defaultShader = -1;
	m_phongShader = -1;
	m_SSAOShader = -1;
	m_shadowShader = -1;
	m_waterShader = -1;
	m_lightingShader = -1;
	m_cubeMapShader = -1;
	m_textureShader = -1;
	m_fxaaShader = -1;
	m_blurVerticalShader = -1;
	m_blurHorizontalShader = -1;
	m_paperdollShader = -1;
	m_pRenderer->LoadGLSLShader("Resources/shaders/default.vertex", "Resources/shaders/default.pixel", &m_defaultShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/phong.vertex", "Resources/shaders/phong.pixel", &m_phongShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/shadow.vertex", "Resources/shaders/shadow.pixel", &m_shadowShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/water_still.vertex", "Resources/shaders/water_still.pixel", &m_waterShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/texture.vertex", "Resources/shaders/texture.pixel", &m_textureShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/fullscreen/SSAO.vertex", "Resources/shaders/fullscreen/SSAO.pixel", &m_SSAOShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/fullscreen/fxaa.vertex", "Resources/shaders/fullscreen/fxaa.pixel", &m_fxaaShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/fullscreen/lighting.vertex", "Resources/shaders/fullscreen/lighting.pixel", &m_lightingShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/cube_map.vertex", "Resources/shaders/cube_map.pixel", &m_cubeMapShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/fullscreen/blur_vertical.vertex", "Resources/shaders/fullscreen/blur_vertical.pixel", &m_blurVerticalShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/fullscreen/blur_horizontal.vertex", "Resources/shaders/fullscreen/blur_horizontal.pixel", &m_blurHorizontalShader);
	m_pRenderer->LoadGLSLShader("Resources/shaders/paperdoll.vertex", "Resources/shaders/paperdoll.pixel", &m_paperdollShader);

	// Create the mods manager
	m_pModsManager = new ModsManager();
	m_pModsManager->LoadMods();

	// Create the sound manager
	SoundManager::GetInstance()->Setup();

	// Create the qubicle binary file manager
	m_pQubicleBinaryManager = new QubicleBinaryManager(m_pRenderer);

	// Create the chunk manager
	m_pChunkManager = new ChunkManager(m_pRenderer, m_pCubbySettings, m_pQubicleBinaryManager);
	m_pChunkManager->SetStepLockEnabled(m_pCubbySettings->m_stepUpdating);

	// Create the biome manager
	m_pBiomeManager = new BiomeManager(m_pRenderer);

	// Create the lighting manager
	m_pLightingManager = new LightingManager(m_pRenderer);

	// Create the scenery manager
	m_pSceneryManager = new SceneryManager(m_pRenderer, m_pChunkManager);

	// Create the skybox
	m_pSkybox = new Skybox(m_pRenderer);

	// Create the block particle manager
	m_pBlockParticleManager = new BlockParticleManager(m_pRenderer, m_pChunkManager);

	// Create the text effects manager
	m_pTextEffectsManager = new TextEffectsManager(m_pRenderer);
	m_pTextEffectsManager->SetCamera(m_pGameCamera);

	// Create the instance manager
	m_pInstanceManager = new InstanceManager(m_pRenderer);

	// Create the player
	m_pPlayer = new Player(m_pRenderer, m_pChunkManager, m_pQubicleBinaryManager, m_pLightingManager, m_pBlockParticleManager);

	// Create the NPC manager
	m_pNPCManager = new NPCManager(m_pRenderer, m_pChunkManager);

	// Create the enemy manager
	m_pEnemyManager = new EnemyManager(m_pRenderer, m_pChunkManager, m_pPlayer);

	// Create the quest manager
	m_pQuestManager = new QuestManager();

	// Create the quest journal
	m_pQuestJournal = new QuestJournal(m_pQuestManager);

	// Create the inventory manager
	m_pInventoryManager = new InventoryManager();

	// Create the item manager
	m_pItemManager = new ItemManager(m_pRenderer, m_pChunkManager, m_pPlayer);

	// Create the random loot manager
	m_pRandomLootManager = new RandomLootManager();

	// Create the projectile manager
	m_pProjectileManager = new ProjectileManager(m_pRenderer, m_pChunkManager);

	// Create the front-end manager
	m_pFrontendManager = new FrontendManager(m_pRenderer, m_pGUI);
	m_pFrontendManager->SetWindowDimensions(m_windowWidth, m_windowHeight);
	m_pFrontendManager->SetCamera(m_pGameCamera);

	// Create the game GUI pages
	m_pInventoryGUI = new InventoryGUI(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
	m_pCharacterGUI = new CharacterGUI(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
	m_pLootGUI = new LootGUI(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
	m_pCraftingGUI = new CraftingGUI(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
	m_pQuestGUI = new QuestGUI(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
	m_pActionBar = new ActionBar(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);
	m_pHUD = new HUD(m_pRenderer, m_pGUI, m_pFrontendManager, m_pChunkManager, m_pPlayer, m_pInventoryManager, m_windowWidth, m_windowHeight);

	// Create module and manager linkage
	m_pChunkManager->SetPlayer(m_pPlayer);
	m_pChunkManager->SetSceneryManager(m_pSceneryManager);
	m_pChunkManager->SetBiomeManager(m_pBiomeManager);
	m_pChunkManager->SetEnemyManager(m_pEnemyManager);
	m_pChunkManager->SetNPCManager(m_pNPCManager);
	m_pChunkManager->SetBlockParticleManager(m_pBlockParticleManager);
	m_pChunkManager->SetItemManager(m_pItemManager);
	m_pPlayer->SetInventoryManager(m_pInventoryManager);
	m_pPlayer->SetItemManager(m_pItemManager);
	m_pPlayer->SetProjectileManager(m_pProjectileManager);
	m_pPlayer->SetTextEffectsManager(m_pTextEffectsManager);
	m_pPlayer->SetEnemyManager(m_pEnemyManager);
	m_pPlayer->SetInventoryGUI(m_pInventoryGUI);
	m_pPlayer->SetCharacterGUI(m_pCharacterGUI);
	m_pPlayer->SetCraftingGUI(m_pCraftingGUI);
	m_pPlayer->SetLootGUI(m_pLootGUI);
	m_pPlayer->SetActionBar(m_pActionBar);
	m_pNPCManager->SetPlayer(m_pPlayer);
	m_pNPCManager->SetLightingManager(m_pLightingManager);
	m_pNPCManager->SetBlockParticleManager(m_pBlockParticleManager);
	m_pNPCManager->SetTextEffectsManager(m_pTextEffectsManager);
	m_pNPCManager->SetItemManager(m_pItemManager);
	m_pNPCManager->SetQubicleBinaryManager(m_pQubicleBinaryManager);
	m_pNPCManager->SetProjectileManager(m_pProjectileManager);
	m_pNPCManager->SetEnemyManager(m_pEnemyManager);
	m_pEnemyManager->SetLightingManager(m_pLightingManager);
	m_pEnemyManager->SetBlockParticleManager(m_pBlockParticleManager);
	m_pEnemyManager->SetTextEffectsManager(m_pTextEffectsManager);
	m_pEnemyManager->SetItemManager(m_pItemManager);
	m_pEnemyManager->SetProjectileManager(m_pProjectileManager);
	m_pEnemyManager->SetHUD(m_pHUD);
	m_pEnemyManager->SetQubicleBinaryManager(m_pQubicleBinaryManager);
	m_pEnemyManager->SetNPCManager(m_pNPCManager);
	m_pInventoryManager->SetPlayer(m_pPlayer);
	m_pInventoryManager->SetInventoryGUI(m_pInventoryGUI);
	m_pInventoryManager->SetLootGUI(m_pLootGUI);
	m_pInventoryManager->SetActionBar(m_pActionBar);
	m_pItemManager->SetLightingManager(m_pLightingManager);
	m_pItemManager->SetBlockParticleManager(m_pBlockParticleManager);
	m_pItemManager->SetQubicleBinaryManager(m_pQubicleBinaryManager);
	m_pItemManager->SetInventoryManager(m_pInventoryManager);
	m_pItemManager->SetNPCManager(m_pNPCManager);
	m_pProjectileManager->SetLightingManager(m_pLightingManager);
	m_pProjectileManager->SetBlockParticleManager(m_pBlockParticleManager);
	m_pProjectileManager->SetPlayer(m_pPlayer);
	m_pProjectileManager->SetQubicleBinaryManager(m_pQubicleBinaryManager);
	m_pQuestManager->SetNPCManager(m_pNPCManager);
	m_pQuestManager->SetInventoryManager(m_pInventoryManager);
	m_pQuestManager->SetQuestJournal(m_pQuestJournal);
	m_pQuestJournal->SetPlayer(m_pPlayer);
	m_pInventoryGUI->SetCharacterGUI(m_pCharacterGUI);
	m_pInventoryGUI->SetLootGUI(m_pLootGUI);
	m_pInventoryGUI->SetActionBar(m_pActionBar);
	m_pInventoryGUI->SetItemManager(m_pItemManager);
	m_pCharacterGUI->SetInventoryGUI(m_pInventoryGUI);
	m_pCharacterGUI->SetLootGUI(m_pLootGUI);
	m_pCharacterGUI->SetActionBar(m_pActionBar);
	m_pLootGUI->SetInventoryGUI(m_pInventoryGUI);
	m_pLootGUI->SetCharacterGUI(m_pCharacterGUI);
	m_pLootGUI->SetActionBar(m_pActionBar);
	m_pActionBar->SetInventoryGUI(m_pInventoryGUI);
	m_pActionBar->SetCharacterGUI(m_pCharacterGUI);
	m_pActionBar->SetLootGUI(m_pLootGUI);
	m_pQuestGUI->SetQuestJournal(m_pQuestJournal);
	m_pHUD->SetInventoryGUI(m_pInventoryGUI);
	m_pHUD->SetCharacterGUI(m_pCharacterGUI);
	m_pHUD->SetQuestGUI(m_pQuestGUI);
	m_pHUD->SetCraftingGUI(m_pCraftingGUI);

	// Keyboard movement
	m_isKeyboardForward = false;
	m_isKeyboardBackward = false;
	m_isKeyboardStrafeLeft = false;
	m_isKeyboardStrafeRight = false;
	m_isKeyboardLeft = false;
	m_isKeyboardRight = false;
	m_isKeyboardUp = false;
	m_isKeyboardDown = false;
	m_isKeyboardSpace = false;
	m_isKeyboardMenu = false;

	// Joystick flags
	m_isJoystickJump = false;

	// Combat flags
	m_isAttackPressedMouse = false;
	m_isAttackReleasedMouse = false;
	m_isAttackPressedJoystick = false;
	m_isAttackReleasedJoystick = false;
	m_canDoAttackMouse = true;
	m_canDoAttackJoystick = true;
	m_isTargetEnemyPressedJoystick = false;
	m_isTargetEnemyReleasedJoystick = false;

	// Camera movement
	m_isCameraRotate = false;
	m_pressedX = 0;
	m_pressedY = 0;
	m_currentX = 0;
	m_currentY = 0;
	m_cameraDistance = m_pGameCamera->GetZoomAmount();
	m_maxCameraDistance = m_cameraDistance;

	// Auto camera mode
	m_autoCameraMovingModifier = 1.0f;

	// Enemy target camera mode
	m_targetCameraXAxisAmount = 0.0f;
	m_targetCameraXAxisAmountTarget = 0.0f;
	m_targetCameraYRatio = 0.0f;
	m_targetCameraForwardRatio = 0.0f;

	// Player movement
	m_keyboardMovement = false;
	m_gamepadMovement = false;
	m_movementSpeed = 0.0f;
	m_movementDragTime = 0.45f;
	m_movementIncreaseTime = 0.25f;
	m_maxMovementSpeed = 10.0f;
	m_movementStopThreshold = 0.05f;

	// Blur
	m_globalBlurAmount = 0.0f;

	// Cinematic letterbox mode
	m_letterBoxRatio = 0.0f;

	// Water
	m_elapsedWaterTime = 0.0f;

	// Paperdoll rendering
	m_paperdollRenderRotation = 0.0f;

	// Toggle flags
	m_deferredRendering = true;
	m_modelWireframe = false;
	m_modelAnimationIndex = 0;
	m_multiSampling = true;
	m_ssao = true;
	m_blur = false;
	m_dynamicLighting = true;
	m_animationUpdate = true;
	m_fullscreen = m_pCubbySettings->m_fullscreen;
	m_debugRender = false;
	m_instanceRender = true;

	// Camera mode
	m_cameraMode = CameraMode::Debug;
	m_previousCameraMode = CameraMode::Debug;
	m_shouldRestorePreviousCameraMode = false;

	// Game mode
	m_gameMode = GameMode::Loading;
	m_allowToChangeToGame = true;
	m_allowToChangeToFrontend = true;
	SetGameMode(m_gameMode);

	// Turn the cursor initially off if we have custom cursors enabled
	if (m_pCubbySettings->m_customCursors)
	{
		TurnCursorOff(true);
	}

	// Create, setup and skin the GUI components
	CreateGUI();
	SetupGUI();
	SkinGUI();
	UpdateGUI(0.0f);
}

// Destruction
void CubbyGame::Destroy()
{
	if (m_instance)
	{
		delete m_pSkybox;
		delete m_pChunkManager;
		delete m_pItemManager;
		delete m_pRandomLootManager;
		delete m_pInventoryManager;
		delete m_pFrontendManager;
		delete m_pPlayer;
		delete m_pNPCManager;
		delete m_pEnemyManager;
		delete m_pLightingManager;
		delete m_pSceneryManager;
		delete m_pBlockParticleManager;
		delete m_pTextEffectsManager;
		delete m_pInstanceManager;
		delete m_pBiomeManager;
		delete m_pQubicleBinaryManager;
		delete m_pModsManager;
		delete m_pGameCamera;
		delete m_pQuestManager;
		delete m_pQuestJournal;
		delete m_pInventoryGUI;
		delete m_pCharacterGUI;
		delete m_pLootGUI;
		delete m_pCraftingGUI;
		delete m_pQuestGUI;
		delete m_pActionBar;

		// Destroy the GUI components before we delete the GUI manager object.
		DestroyGUI();
		delete m_pGUI;
		delete m_pRenderer;

		SoundManager::GetInstance()->Shutdown();

		m_pCubbyWindow->Destroy();

		delete m_pCubbyWindow;

		delete m_instance;
	}
}

// Quitting
void CubbyGame::CancelQuitPopup()
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::None);

	SetPaused(false);

	SetGlobalBlurAmount(0.0f);

	TurnCursorOff(false);
}

void CubbyGame::ShowQuitPopup()
{
	if (m_pFrontendManager->GetFrontendScreen() != FrontendScreen::QuitPopup)
	{
		CloseAllGUIWindows();

		m_pFrontendManager->SetFrontendScreen(FrontendScreen::QuitPopup);

		SetPaused(true);

		SetGlobalBlurAmount(0.0015f);

		TurnCursorOn(false, false);
	}
}

void CubbyGame::SetGameQuit(bool quit)
{
	m_isGameQuit = quit;
}

// Pause
bool CubbyGame::IsPaused() const
{
	return m_isPaused;
}

void CubbyGame::SetPaused(bool pause)
{
	m_isPaused = pause;
}

void CubbyGame::SetPauseMenu()
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::PauseMenu);

	SetPaused(true);

	SetGlobalBlurAmount(0.0015f);

	TurnCursorOn(true, false);
}

void CubbyGame::UnsetPauseMenu()
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::None);

	SetPaused(false);

	SetGlobalBlurAmount(0.0f);

	TurnCursorOff(false);
}

// Blur
void CubbyGame::SetGlobalBlurAmount(float blurAmount)
{
	m_globalBlurAmount = blurAmount;
	m_pBlurCheckBox->SetToggled(m_globalBlurAmount > 0.0f);
}

// Cinematic letterbox
void CubbyGame::OpenLetterBox()
{
	Interpolator::GetInstance()->AddFloatInterpolation(&m_letterBoxRatio, m_letterBoxRatio, 1.0f, 0.25f, -100.0f);
}

void CubbyGame::CloseLetterBox()
{
	Interpolator::GetInstance()->AddFloatInterpolation(&m_letterBoxRatio, m_letterBoxRatio, 0.0f, 0.25f, 100.0f);
}

// Paperdoll rendering
void CubbyGame::SetPaperdollRotation(float rotation)
{
	m_paperdollRenderRotation = rotation;
}

void CubbyGame::RotatePaperdollModel(float rot)
{
	m_paperdollRenderRotation += rot;
}

unsigned int CubbyGame::GetDynamicPaperdollTexture() const
{
	return m_pRenderer->GetDiffuseTextureFromFrameBuffer(m_paperdollSSAOTextureBuffer);
}

// Portrait
unsigned int CubbyGame::GetDynamicPortraitTexture() const
{
	return m_pRenderer->GetDiffuseTextureFromFrameBuffer(m_portraitSSAOTextureBuffer);
}

// Events
void CubbyGame::PollEvents() const
{
	m_pCubbyWindow->PollEvents();
}

bool CubbyGame::ShouldClose() const
{
	return m_isGameQuit;
}

// Window functionality
int CubbyGame::GetWindowCursorX() const
{
	return m_pCubbyWindow->GetCursorX();
}

int CubbyGame::GetWindowCursorY() const
{
	return m_pCubbyWindow->GetCursorY();
}

void CubbyGame::TurnCursorOn(bool resetCursorPosition, bool forceOn)
{
	m_pCubbyWindow->TurnCursorOn(resetCursorPosition, forceOn);

	m_isCustomCursorOn = true;
}

void CubbyGame::TurnCursorOff(bool forceOff)
{
	m_pCubbyWindow->TurnCursorOff(forceOff);

	m_isCustomCursorOn = false;

	// Make sure to set the current X and Y when we turn the cursor off, so that camera controls don't glitch.
	m_currentX = m_pCubbyWindow->GetCursorX();
	m_currentY = m_pCubbyWindow->GetCursorY();
}

bool CubbyGame::IsCursorOn() const
{
	if (m_pCubbySettings->m_customCursors)
	{
		return m_isCustomCursorOn;
	}

	return m_pCubbyWindow->IsCursorOn();
}

void CubbyGame::ResizeWindow(int width, int height)
{
	m_windowWidth = width;
	m_windowHeight = height;

	m_pCubbyWindow->ResizeWindow(m_windowWidth, m_windowHeight);

	if (m_pRenderer)
	{
		// Let the renderer know we have resized the window
		m_pRenderer->ResizeWindow(m_windowWidth, m_windowHeight);

		// Resize the main viewport
		m_pRenderer->ResizeViewport(m_defaultViewport, 0, 0, m_windowWidth, m_windowHeight, 60.0f);
		m_pRenderer->ResizeViewport(m_firstpersonViewport, 0, 0, m_windowWidth, m_windowHeight, 60.0f);
		m_pRenderer->ResizeViewport(m_paperdollViewport, m_paperdollViewportY, m_paperdollViewportX, m_paperdollViewportWidth, m_paperdollViewportHeight, 60.0f);
		m_pRenderer->ResizeViewport(m_portraitViewport, m_portraitViewportY, m_portraitViewportX, m_portraitViewportWidth, m_portraitViewportHeight, 60.0f);

		// Resize the frame buffers
		m_pRenderer->CreateFrameBuffer(m_SSAOFrameBuffer, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "SSAO", &m_SSAOFrameBuffer);
		m_pRenderer->CreateFrameBuffer(m_shadowFrameBuffer, true, true, true, true, m_windowWidth, m_windowHeight, 5.0f, "Shadow", &m_shadowFrameBuffer);
		m_pRenderer->CreateFrameBuffer(m_lightingFrameBuffer, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "Deferred Lighting", &m_lightingFrameBuffer);
		m_pRenderer->CreateFrameBuffer(m_transparencyFrameBuffer, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "Transparency", &m_transparencyFrameBuffer);
		m_pRenderer->CreateFrameBuffer(m_waterReflectionFrameBuffer, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "Water Reflection", &m_waterReflectionFrameBuffer);
		m_pRenderer->CreateFrameBuffer(m_FXAAFrameBuffer, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "FXAA", &m_FXAAFrameBuffer);
		m_pRenderer->CreateFrameBuffer(m_firstPassFullscreenBuffer, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "FullScreen 1st Pass", &m_firstPassFullscreenBuffer);
		m_pRenderer->CreateFrameBuffer(m_secondPassFullscreenBuffer, true, true, true, true, m_windowWidth, m_windowHeight, 1.0f, "FullScreen 2nd Pass", &m_secondPassFullscreenBuffer);
		m_pRenderer->CreateFrameBuffer(m_paperdollBuffer, true, true, true, true, 800, 800, 1.0f, "Paperdoll", &m_paperdollBuffer);
		m_pRenderer->CreateFrameBuffer(m_paperdollSSAOTextureBuffer, true, true, true, true, 800, 800, 1.0f, "Paperdoll SSAO Texture", &m_paperdollSSAOTextureBuffer);
		m_pRenderer->CreateFrameBuffer(m_portraitBuffer, true, true, true, true, 800, 800, 1.0f, "Portrait", &m_portraitBuffer);
		m_pRenderer->CreateFrameBuffer(m_portraitSSAOTextureBuffer, true, true, true, true, 800, 800, 1.0f, "Portrait SSAO Texture", &m_portraitSSAOTextureBuffer);

		// Give the new windows dimensions to the GUI components also
		m_pMainWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
		m_pGameWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
		m_pConsoleWindow->SetApplicationDimensions(m_windowWidth, m_windowHeight);
	}

	// Frontend
	if (m_pFrontendManager)
	{
		m_pFrontendManager->SetWindowDimensions(m_windowWidth, m_windowHeight);
	}

	// Resize game GUI
	if (m_pInventoryGUI)
	{
		m_pInventoryGUI->SetWindowDimensions(m_windowWidth, m_windowHeight);
	}
	if (m_pCharacterGUI)
	{
		m_pCharacterGUI->SetWindowDimensions(m_windowWidth, m_windowHeight);
	}
	if (m_pLootGUI)
	{
		m_pLootGUI->SetWindowDimensions(m_windowWidth, m_windowHeight);
	}
	if (m_pCraftingGUI)
	{
		m_pCraftingGUI->SetWindowDimensions(m_windowWidth, m_windowHeight);
	}
	if (m_pActionBar)
	{
		m_pActionBar->SetWindowDimensions(m_windowWidth, m_windowHeight);
	}
	if (m_pHUD)
	{
		m_pHUD->SetWindowDimensions(m_windowWidth, m_windowHeight);
	}
}

void CubbyGame::CloseWindow()
{
	if (m_gameMode == GameMode::Game)
	{
		ShowQuitPopup();
	}
	else
	{
		m_isGameQuit = true;
	}
}

void CubbyGame::UpdateJoySticks() const
{
	m_pCubbyWindow->UpdateJoySticks();
}

// Music
void CubbyGame::StartFrontEndMusic()
{
	std::string musicModName = GetInstance()->GetModsManager()->GetSoundPack();
	std::string musicFileName = "Resources/audio/" + musicModName + "/music/vox_intro.ogg";
	m_pMusicSound = SoundManager::GetInstance()->PlaySound2D(&m_pMusicChannel, musicFileName.c_str(), true, true);

	UpdateMusicVolume(0.0f);
}

void CubbyGame::StartGameMusic()
{
	Biome currentBiome = m_pBiomeManager->GetBiome(m_pPlayer->GetCenter());
	m_currentBiomeMusic = currentBiome;

	std::string biomeFileName = "";
	switch (m_currentBiomeMusic)
	{
	case Biome::None: { break; }
	case Biome::GrassLand: { biomeFileName = "biome_plains.ogg"; break; }
	case Biome::Desert: { biomeFileName = "biome_desert.ogg"; break; }
	//case Biome::Jungle: { biomeFileName = "biome_jungle.ogg"; break; }
	case Biome::Tundra: { biomeFileName = "biome_snow.ogg"; break; }
	//case Biome::Swamp: { biomeFileName = ""; break; }
	case Biome::AshLand: { biomeFileName = "biome_nightmare.ogg"; break; }
	//case Biome::Nightmare: { biomeFileName = ""; break; }
	default: throw std::logic_error("Invalid Biome in StartGameMusic()");
	}

	std::string musicModName = GetInstance()->GetModsManager()->GetSoundPack();
	std::string musicFileName = "Resources/audio/" + musicModName + "/music/" + biomeFileName;
	m_pMusicSound = SoundManager::GetInstance()->PlaySound2D(&m_pMusicChannel, musicFileName.c_str(), true, true);

	UpdateMusicVolume(0.0f);
}

void CubbyGame::StopMusic()
{
	// Stop the music
	SoundManager::GetInstance()->StopSound(m_pMusicChannel);

	m_pMusicChannel = nullptr;
	m_pMusicSound = nullptr;
}

void CubbyGame::UpdateGameMusic(float dt)
{
	Biome currentBiome = m_pBiomeManager->GetBiome(m_pPlayer->GetCenter());

	if (currentBiome != m_currentBiomeMusic)
	{
		StopMusic();
		StartGameMusic();
	}
}

void CubbyGame::UpdateMusicVolume(float dt) const
{
	if (m_pCubbySettings->m_music)
	{
		if (m_pMusicChannel != nullptr)
		{
			m_pMusicChannel->setVolume(0.125f * m_pCubbySettings->m_musicVolume);
		}
	}
	else
	{
		if (m_pMusicChannel != nullptr)
		{
			m_pMusicChannel->setVolume(0.0f);
		}
	}
}

// Sounds
void CubbyGame::PlaySoundEffect(SoundEffect soundEffect, float soundEnhanceMultiplier) const
{
	if (m_pCubbySettings->m_audio)
	{
		std::string soundModName = GetInstance()->GetModsManager()->GetSoundPack();
		std::string soundeffectFileName = g_soundEffectFileNames[static_cast<int>(soundEffect)];
		std::string soundFileName = "Resources/audio/" + soundModName + "/soundeffects/" + soundeffectFileName;

		FMOD::Channel* pSoundChannel;
		SoundManager::GetInstance()->PlaySound2D(&pSoundChannel, soundFileName.c_str(), false);
		pSoundChannel->setVolume(soundEnhanceMultiplier * m_pCubbySettings->m_audioVolume);
	}
}

void CubbyGame::PlaySoundEffect3D(SoundEffect soundEffect, glm::vec3 soundPosition, float soundEnhanceMultiplier) const
{
	if (m_pCubbySettings->m_audio)
	{
		std::string soundModName = GetInstance()->GetModsManager()->GetSoundPack();
		std::string soundeffectFileName = g_soundEffectFileNames[static_cast<int>(soundEffect)];
		std::string soundFileName = "Resources/audio/" + soundModName + "/soundeffects/" + soundeffectFileName;

		FMOD::Channel* pSoundChannel;
		SoundManager::GetInstance()->PlaySound3D(&pSoundChannel, soundFileName.c_str(), soundPosition, false);
		pSoundChannel->setVolume(3.0f * soundEnhanceMultiplier * m_pCubbySettings->m_audioVolume);
	}
}

// Game functions
void CubbyGame::QuitToFrontEnd()
{
	TurnCursorOn(true, false);
	SetGameMode(GameMode::FrontEnd);

	m_pFrontEndOptionBox->SetToggled(true);
	m_pFrontendCameraOptionBox->SetToggled(true);
	m_pMouseRotateCameraOptionBox->SetDisabled(true);
	m_pAutoCameraOptionBox->SetDisabled(true);
	m_pFrontendCameraOptionBox->SetDisabled(false);
	m_pDebugCameraOptionBox->SetDisabled(false);
	m_pCubbyWindow->Update(m_deltaTime);
	GameModeChanged();
	CameraModeChanged();

	// Make sure that movement speed drag is reset, so that we don't carry on moving after quitting and restarting game
	m_movementSpeed = 0.0f;

	// Set front-end page to intro
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::MainMenu);
}

void CubbyGame::SetupDataForGame() const
{
	// Initial player startup position and rotation
	glm::vec3 starPosition = glm::vec3(10.0f, 8.0f, 23.0f);
	m_pPlayer->SetPosition(starPosition);
	m_pPlayer->SetRespawnPosition(starPosition + glm::vec3(0.0f, 0.1f, 0.0f));
	m_pPlayer->SetRotation(90.0f);

	// Items
	Item* pFurnace = m_pItemManager->CreateItem(glm::vec3(25.0f, 10.0f, 29.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), "Resources/gamedata/items/Furnace/Furnace.item", ItemType::Furnace, "Furnace", true, false, 0.16f);
	pFurnace->SetInteractionPositionOffset(glm::vec3(0.0f, 0.0f, -2.0f));
	Item* pAnvil = m_pItemManager->CreateItem(glm::vec3(32.0f, 9.0f, 26.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), "Resources/gamedata/items/Anvil/Anvil.item", ItemType::Anvil, "Anvil", true, false, 0.14f);
	pAnvil->SetInteractionPositionOffset(glm::vec3(0.0f, 0.0f, -1.5f));

	// Chest with random loot item
	Item* pChest = m_pItemManager->CreateItem(glm::vec3(17.0f, 12.0f, 28.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), "Resources/gamedata/items/Chest/Chest.item", ItemType::Chest, "Chest", true, false, 0.08f);
	EquipmentType equipment = EquipmentType::None;
	InventoryItem* pRandomLoot = GetInstance()->GetRandomLootManager()->GetRandomLootItem(&equipment);
	
	if (pRandomLoot != nullptr && equipment != EquipmentType::None)
	{
		InventoryItem* pRandomLootItem = pChest->AddLootItem(pRandomLoot, 0, 2);
		pRandomLootItem->m_scale = pRandomLoot->m_scale;
		pRandomLootItem->m_offsetX = pRandomLoot->m_offsetX;
		pRandomLootItem->m_offsetY = pRandomLoot->m_offsetY;
		pRandomLootItem->m_offsetZ = pRandomLoot->m_offsetZ;
		pRandomLootItem->m_left = pRandomLoot->m_left;
		pRandomLootItem->m_right = pRandomLoot->m_right;
	}

	// Item spawners
	ItemSpawner* pItemSpawner1 = m_pItemManager->CreateItemSpawner(glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	pItemSpawner1->SetSpawningParams(0.0f, 0.0f, 15, glm::vec3(0.0f, 0.0f, 0.0f), true, glm::vec3(0.0f, 0.5f, 0.0f), true, true, 25.0f, Biome::GrassLand, 0.15f);
	pItemSpawner1->AddItemTypeToSpawn(ItemType::CopperVein);
	pItemSpawner1->AddItemTypeToSpawn(ItemType::IronVein);

	ItemSpawner* pItemSpawner2 = m_pItemManager->CreateItemSpawner(glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	pItemSpawner2->SetSpawningParams(0.0f, 0.0f, 15, glm::vec3(0.0f, 0.0f, 0.0f), true, glm::vec3(0.0f, 0.5f, 0.0f), true, true, 25.0f, Biome::AshLand, 0.15f);
	pItemSpawner2->AddItemTypeToSpawn(ItemType::SilverVein);
	pItemSpawner2->AddItemTypeToSpawn(ItemType::GoldVein);

	ItemSpawner* pItemSpawner3 = m_pItemManager->CreateItemSpawner(glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	pItemSpawner3->SetSpawningParams(0.0f, 0.0f, 3, glm::vec3(0.0f, 0.0f, 0.0f), true, glm::vec3(0.0f, 0.5f, 0.0f), true, true, 25.0f, Biome::GrassLand, 0.08f);
	pItemSpawner3->AddItemTypeToSpawn(ItemType::Chest);

	// Npcs
	NPC* pCharacter1 = m_pNPCManager->CreateNPC("Mage", "Human", "Mage", glm::vec3(21.0f, 8.5f, 25.0f), 0.08f, false, true);
	pCharacter1->SetForwards(glm::vec3(0.0f, 0.0f, -1.0f));
	pCharacter1->SetTargetForwards(glm::vec3(0.0f, 0.0f, -1.0f));
	pCharacter1->SetNPCCombatType(NPCCombatType::Staff, true);

	// Safezones (Where we cannot spawn enemies)
	m_pBiomeManager->AddSafeZone(glm::vec3(21.0f, 8.5f, 20.0f), 25.f, 30.0f, 25.0f);
	m_pBiomeManager->AddTown(glm::vec3(8.0f, 8.0f, 8.0f), 75.f, 15.0f, 75.0f);

	// Enemies
	//Enemy* pEnemy0 = m_pEnemyManager->CreateEnemy(glm::vec3(35.5f, 12.0f, 5.5f), EnemyType::RedSlime, 0.08f);

	// Enemy spawners
	// Grassland
	EnemySpawner* pEnemySpawner0 = m_pEnemyManager->CreateEnemySpawner(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 5.0f));
	pEnemySpawner0->SetSpawningParams(3.0f, 3.0f, 8, glm::vec3(0.0f, 0.0f, 0.0f), true, glm::vec3(0.0f, 1.0f, 0.0f), true, true, 25.0f, Biome::GrassLand);
	pEnemySpawner0->AddEnemyTypeToSpawn(EnemyType::GreenSlime);
	pEnemySpawner0->AddEnemyTypeToSpawn(EnemyType::RedSlime);
	//pEnemySpawner0->AddEnemyTypeToSpawn(EnemyType::BlueSlime);
	pEnemySpawner0->AddEnemyTypeToSpawn(EnemyType::YellowSlime);

	EnemySpawner* pEnemySpawner1 = m_pEnemyManager->CreateEnemySpawner(glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	pEnemySpawner1->SetSpawningParams(5.0f, 3.5f, 3, glm::vec3(0.0f, 5.0f, 0.0f), true, glm::vec3(0.0f, 5.0f, 0.0f), true, true, 25.0f, Biome::GrassLand);
	pEnemySpawner1->AddEnemyTypeToSpawn(EnemyType::Bee);

	EnemySpawner* pEnemySpawner2 = m_pEnemyManager->CreateEnemySpawner(glm::vec3(0.0f, 8.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	pEnemySpawner2->SetSpawningParams(0.0f, 1.0f, 3, glm::vec3(0.0f, 5.0f, 0.0f), true, glm::vec3(0.0f, 0.0f, 0.0f), true, true, 25.0f, Biome::GrassLand);
	pEnemySpawner2->AddEnemyTypeToSpawn(EnemyType::Mimic);

	// Desert
	EnemySpawner* pEnemySpawner3 = m_pEnemyManager->CreateEnemySpawner(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 5.0f));
	pEnemySpawner3->SetSpawningParams(3.0f, 3.0f, 8, glm::vec3(0.0f, 0.0f, 0.0f), true, glm::vec3(0.0f, 1.0f, 0.0f), true, true, 25.0f, Biome::Desert);
	pEnemySpawner3->AddEnemyTypeToSpawn(EnemyType::NormalSkeleton);
	pEnemySpawner3->AddEnemyTypeToSpawn(EnemyType::RangedSkeleton);
	pEnemySpawner3->AddEnemyTypeToSpawn(EnemyType::MeleeSkeleton);
	pEnemySpawner3->AddEnemyTypeToSpawn(EnemyType::MageSkeleton);

	EnemySpawner* pEnemySpawner4 = m_pEnemyManager->CreateEnemySpawner(glm::vec3(0.0f, 12.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	pEnemySpawner4->SetSpawningParams(0.0f, 1.0f, 4, glm::vec3(0.0f, 5.0f, 0.0f), true, glm::vec3(0.0f, 0.0f, 0.0f), true, true, 25.0f, Biome::Desert);
	pEnemySpawner4->AddEnemyTypeToSpawn(EnemyType::Mummy);

	// Ashlands
	EnemySpawner* pEnemySpawner5 = m_pEnemyManager->CreateEnemySpawner(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 5.0f));
	pEnemySpawner5->SetSpawningParams(3.0f, 3.0f, 15, glm::vec3(0.0f, 0.0f, 0.0f), true, glm::vec3(0.0f, 1.0f, 0.0f), true, true, 25.0f, Biome::AshLand);
	pEnemySpawner5->AddEnemyTypeToSpawn(EnemyType::WalkingZombie);
	pEnemySpawner5->AddEnemyTypeToSpawn(EnemyType::CrawlingZombie);

	EnemySpawner* pEnemySpawner6 = m_pEnemyManager->CreateEnemySpawner(glm::vec3(0.0f, 12.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	pEnemySpawner6->SetSpawningParams(0.0f, 1.0f, 4, glm::vec3(0.0f, 5.0f, 0.0f), true, glm::vec3(0.0f, 0.0f, 0.0f), true, true, 25.0f, Biome::AshLand);
	pEnemySpawner6->AddEnemyTypeToSpawn(EnemyType::IronGiant);

	EnemySpawner* pEnemySpawner7 = m_pEnemyManager->CreateEnemySpawner(glm::vec3(0.0f, 12.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	pEnemySpawner7->SetSpawningParams(0.0f, 1.0f, 8, glm::vec3(0.0f, 5.0f, 0.0f), true, glm::vec3(0.0f, 5.0f, 0.0f), true, true, 25.0f, Biome::AshLand);
	pEnemySpawner7->AddEnemyTypeToSpawn(EnemyType::Bat);

	// Tundra
	EnemySpawner* pEnemySpawner8 = m_pEnemyManager->CreateEnemySpawner(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 5.0f));
	pEnemySpawner8->SetSpawningParams(3.0f, 3.0f, 8, glm::vec3(0.0f, 0.0f, 0.0f), true, glm::vec3(0.0f, 1.0f, 0.0f), true, true, 25.0f, Biome::Tundra);
	pEnemySpawner8->AddEnemyTypeToSpawn(EnemyType::BlueSlime);

	// Quests
	// Quest 1
	std::string startText1 = "Hello there brave adventurer, my name is [C=Custom(00A2E8)]Melinda The Witch[C=White] and I have stumbled across a 'valuable treasure'. I can give you this treasure if you will do a quest for me...\nI need you to kill these pesky [C=Red]slimes[C=White] that are overrunning this place. If you do this for me, the treasure is yours. What do you say?";
	std::string completedText1 = "You have completed the quest, and destroyed the sliminess of these lands, well done!";
	std::string denyText1 = "You are already on a quest, come back to me once you have finished.";
	Quest* pSlimeQuest = m_pQuestManager->CreateQuest("A Simple Task", startText1, completedText1, denyText1);
	pSlimeQuest->AddQuestObjective("Kill 5 Slimes", QuestType::KillX, 5, EnemyType::GreenSlime, ItemType::None, nullptr, "", nullptr);
	InventoryItem* pQuestReward1 = m_pInventoryManager->CreateEquipmentItemFromType(EquipmentType::IronSword);
	pSlimeQuest->SetQuestReward(pQuestReward1);
	pSlimeQuest->ExportQuest();
	
	// Quest 2
	std::string startText2 = "You look like the type of adventurer who really needs some copper, Please could you be a good fella and collect some copper nuggets for me, they are required for refining into copper bars.";
	std::string completedText2 = "Wow... thank you for collecting the copper nuggets, you can keep it as a reward.";
	std::string denyText2 = "You are already on a quest, come back to me once you have finished.";
	Quest* pCollectQuest = m_pQuestManager->CreateQuest("A Simple Collection", startText2, completedText2, denyText2);
	pCollectQuest->AddQuestObjective("Collect 5 Copper Nuggets", QuestType::CollectX, 5, EnemyType::None, ItemType::CopperOre, nullptr, "", nullptr);
	InventoryItem* pQuestReward2 = m_pInventoryManager->CreateInventoryItemForCrafting(ItemType::CopperOre, 5, ItemQuality::Common);
	pCollectQuest->SetQuestReward(pQuestReward2);
	pCollectQuest->ExportQuest();

	// Quest 3
	std::string startText3 = "My dearest [C=Yellow]Moxie[C=White] is lost and I can't find her. I have looked everywhere, but she is nowhere to be found...\n\nCan you lend me your assistance and help locate my dearest [C=Yellow]Moxie[C=White]? I dont know where she could be, but the last time I saw her she was playing around by [C=Custom(00A2E8)]Joseph Maclure's Barnyard[C=White].\n\nIf you can help find [C=Yellow]Moxie[C=White] for me I will reward you handsomely.";
	std::string completedText3 = "You have found Moxie? Thank you so much kind stranger!";
	std::string denyText3 = "You are already on a quest, come back to me once you have finished.";
	Quest* pFindQuest = m_pQuestManager->CreateQuest("Find Moxie", startText3, completedText3, denyText3);
	pFindQuest->ExportQuest();

	m_pQuestJournal->AddQuestJournalEntry(pSlimeQuest);
	m_pQuestJournal->AddQuestJournalEntry(pCollectQuest);
	m_pQuestJournal->AddQuestJournalEntry(pFindQuest);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void CubbyGame::SetupDataForFrontEnd() const
{
	// Safezones (Where we cannot spawn enemies)
	m_pBiomeManager->AddSafeZone(glm::vec3(21.0f, 8.5f, 20.0f), 25.f, 30.0f, 25.0f);
	m_pBiomeManager->AddTown(glm::vec3(8.0f, 8.0f, 8.0f), 75.f, 15.0f, 75.0f);
}

void CubbyGame::StartGameFromFrontEnd()
{
	m_pFrontendManager->SetFrontendScreen(FrontendScreen::None);

	m_previousCameraMode = CameraMode::MouseRotate;
	m_pGameOptionBox->SetToggled(true);
	m_pMouseRotateCameraOptionBox->SetToggled(true);
	m_pFrontendCameraOptionBox->SetDisabled(true);
	GameModeChanged();
	CameraModeChanged();

	// Start game from player
	m_pPlayer->StartGame();
}

void CubbyGame::PlayerRespawned()
{
	// Stop any movement drag when we re-spawn
	m_movementSpeed = 0.0f;
}

void CubbyGame::SetGameMode(GameMode mode)
{
	GameMode previousGameMode = m_gameMode;
	m_gameMode = mode;

	if (m_gameMode == GameMode::Debug)
	{
		// Do nothing
	}

	if (m_gameMode == GameMode::FrontEnd)
	{
		if (previousGameMode == GameMode::Game || previousGameMode == GameMode::Loading)
		{
			// Close all open GUI windows
			CloseAllGUIWindows();

			// Clear the items
			m_pItemManager->ClearItems();
			m_pItemManager->ClearItemSpawners();

			// Clear the NPCs
			m_pNPCManager->ClearNPCs();

			// Clear the enemies and enemy spawners
			m_pEnemyManager->ClearEnemies();
			m_pEnemyManager->ClearEnemySpawners();

			// Clear all projectiles
			m_pProjectileManager->ClearProjectiles();

			// Reset the inventory manager
			m_pInventoryManager->Reset();

			// Clear the quests
			m_pQuestManager->ClearQuests();

			// Clear the quest journal
			m_pQuestJournal->ClearJournal();

			// Reset the quest GUI text components
			m_pQuestGUI->SetQuestData("", "");

			// Reset the player
			m_pPlayer->ResetPlayer();

			// Set the water level
			m_pChunkManager->SetWaterHeight(1.3f);

			// Unload action bar
			if (m_pActionBar->IsLoaded())
			{
				if (m_pCubbySettings->m_renderGUI)
				{
					m_pActionBar->Unload();
				}
			}

			// Unload the HUD
			if (m_pHUD->IsLoaded())
			{
				if (m_pCubbySettings->m_renderGUI)
				{
					m_pHUD->Unload();
				}
			}

			// Setup the game data since we have just loaded fresh into the front-end.
			SetupDataForFrontEnd();

			// Music
			StopMusic();
			StartFrontEndMusic();

			// Initial chunk creation
			m_pChunkManager->InitializeChunkCreation();
		}
	}

	if (m_gameMode == GameMode::Game)
	{
		if (previousGameMode == GameMode::FrontEnd || previousGameMode == GameMode::Loading)
		{
			// Close all open GUI windows
			CloseAllGUIWindows();

			// Clear the items
			m_pItemManager->ClearItems();
			m_pItemManager->ClearItemSpawners();

			// Clear the NPCs
			m_pNPCManager->ClearNPCs();

			// Clear the enemies and enemy spawners
			m_pEnemyManager->ClearEnemies();
			m_pEnemyManager->ClearEnemySpawners();

			// Clear all projectiles
			m_pProjectileManager->ClearProjectiles();

			// Clear the quests
			m_pQuestManager->ClearQuests();

			// Clear the quest journal
			m_pQuestJournal->ClearJournal();

			// Reset the quest GUI text components
			m_pQuestGUI->SetQuestData("", "");

			// Reset the player
			m_pPlayer->ResetPlayer();

			// Set the water level
			m_pChunkManager->SetWaterHeight(1.3f);

			// Load action bar
			if (m_pActionBar->IsLoaded() == false)
			{
				if (m_pCubbySettings->m_renderGUI)
				{
					m_pActionBar->Load();
				}
			}

			// Load the HUD
			if (m_pHUD->IsLoaded() == false)
			{
				if (m_pCubbySettings->m_renderGUI)
				{
					m_pHUD->Load();
				}
			}

			// Setup the game data since we have just loaded fresh into a game.
			SetupDataForGame();

			// Music
			StopMusic();
			StartGameMusic();

			// Initial chunk creation
			m_pChunkManager->InitializeChunkCreation();
		}
	}
}

GameMode CubbyGame::GetGameMode() const
{
	return m_gameMode;
}

void CubbyGame::SetCameraMode(CameraMode mode)
{
	m_cameraMode = mode;
}

CameraMode CubbyGame::GetCameraMode() const
{
	return m_cameraMode;
}

// Interactions
bool CubbyGame::CheckInteractions()
{
	bool interaction = false;

	if (m_isPaused)
	{
		return false;
	}

	if (m_pPlayer->CanJump() == false)
	{
		// Don't allow interactions if we are jumping
		return false;
	}

	if (m_pPlayer->IsDead())
	{
		// Don't allow interactions if the player is dead.
		return false;
	}

	if (m_pPlayer->GetTargetEnemy() != nullptr)
	{
		// Don't allow interactions while we are in target mode
		return false;
	}

	// Check item interactions
	m_interactItemMutex.lock();
	if (interaction == false && m_pInteractItem != nullptr)
	{
		bool shouldStopMovement = false;

		// Dropped items become collectible by the player and magnet towards him
		if (m_pInteractItem->GetItemType() == ItemType::DroppedItem)
		{
			if (m_pInventoryManager->CanAddInventoryItem(m_pInteractItem->GetDroppedInventoryItem()->m_title.c_str(), m_pInteractItem->GetDroppedInventoryItem()->m_item, m_pInteractItem->GetDroppedInventoryItem()->m_quantity))
			{
				m_pInteractItem->SetIsCollectible(true);
				m_pInteractItem->SetCollectionDelay(0.0f);
			}

			interaction = true;
		}

		// Crafting stations
		if (m_pInteractItem->GetItemType() == ItemType::Anvil || m_pInteractItem->GetItemType() == ItemType::Furnace)
		{
			m_pPlayer->StopMoving();
			shouldStopMovement = true;

			// Load crafting GUI
			if (m_pCraftingGUI->IsLoaded() == false)
			{
				m_pCraftingGUI->SetCraftingRecipesForItem(m_pInteractItem->GetItemType());
				m_pCraftingGUI->Load(true, 0.9f);
				m_pCraftingGUI->SetInteractionitem(m_pInteractItem);

				SavePreviousCameraMode();
				m_shouldRestorePreviousCameraMode = true;
				TurnCursorOn(false, false);
			}

			// Set NPC dialog camera mode
			SetCameraMode(CameraMode::NPCDialog);

			// Figure out which way to position the camera, based on how we are looking at the NPC when interacting
			glm::vec3 toItem = normalize(m_pInteractItem->GetCenter() - m_pInteractItem->GetInteractionPosition());
			glm::vec3 crossResult = cross(glm::vec3(0.0f, 1.0f, 0.0f), toItem);
			float dotAngle = dot(m_pGameCamera->GetFacing(), crossResult);
			
			if (dotAngle > 0.5f)
			{
				glm::vec3 center = (m_pInteractItem->GetCenter() - m_pInteractItem->GetInteractionPosition());
				glm::vec3 crossRight = cross(normalize(center), glm::vec3(0.0f, 1.0f, 0.0f));
				m_targetCameraPositionNPCDialog = (m_pInteractItem->GetInteractionPosition() + center * 2.0f + crossRight * 4.0f + glm::vec3(0.0f, 1.0f, 0.0f) * 2.5f);
				m_targetCameraViewNPCDialog = (m_pInteractItem->GetInteractionPosition() + center * 0.0f - crossRight * 4.0f);
			}
			else
			{
				glm::vec3 center = (m_pInteractItem->GetCenter() - m_pInteractItem->GetInteractionPosition());
				glm::vec3 crossRight = cross(normalize(center), glm::vec3(0.0f, 1.0f, 0.0f));
				m_targetCameraPositionNPCDialog = (m_pInteractItem->GetInteractionPosition() + center * 2.0f - crossRight * 4.0f + glm::vec3(0.0f, 1.0f, 0.0f) * 2.5f);
				m_targetCameraViewNPCDialog = (m_pInteractItem->GetInteractionPosition() + center * 0.0f + crossRight * 4.0f);
			}

			// Player move to interaction point and look at interaction item
			m_pPlayer->SetMoveToTargetPosition(m_pInteractItem->GetInteractionPosition());
			m_pPlayer->SetLookAtTargetAfterMoveToPosition(m_pInteractItem->GetCenter());

			// Set player alpha to full opacity
			m_pPlayer->SetPlayerAlpha(1.0f);

			// Open cinematic letterbox
			OpenLetterBox();

			interaction = true;
		}

		// Chest interaction
		if (m_pInteractItem->GetItemType() == ItemType::Chest)
		{
			if (m_pInteractItem->IsStillAnimating() == false)
			{
				m_pInteractItem->Interact();

				interaction = true;

				if (m_pInteractItem->IsInteracting() == true) // Only open the GUI screens if we are opening a chest
				{
					m_pPlayer->StopMoving();
					shouldStopMovement = true;

					PlaySoundEffect(SoundEffect::ChestOpen);

					if (m_pLootGUI->IsLoaded())
					{
						m_pLootGUI->Unload();

						if (IsGUIWindowStillDisplayed() == false)
						{
							TurnCursorOff(false);
						}
					}
					else if (m_pFrontendManager->GetFrontendScreen() == FrontendScreen::None)
					{
						m_pLootGUI->LoadItems(m_pInteractItem);

						m_pLootGUI->Load(true, 1.0f);

						if (m_pInventoryGUI->IsLoaded() == false)
						{
							m_pInventoryGUI->Load(true, 1.0f);
						}

						m_pPlayer->StopMoving();
						shouldStopMovement = true;

						TurnCursorOn(false, false);
					}
				}
			}
		}

		if (shouldStopMovement == true)
		{
			// Stop any movement drag when we interact with item
			m_movementSpeed = 0.0f;
		}
	}
	m_interactItemMutex.unlock();

	return interaction;
}

Item* CubbyGame::GetInteractItem() const
{
	return m_pInteractItem;
}

// Enemy Targeting
void CubbyGame::SetEnemyTarget()
{
	if (m_pPlayer->IsDead() == false && m_pPlayer->GetTargetEnemy() == nullptr)
	{
		// Get the enemy based on the cursor
		int cursorX = static_cast<int>(m_windowWidth*0.5f);
		int cursorY = static_cast<int>(m_windowHeight*0.5f);

		Enemy* pEnemy = m_pEnemyManager->GetCursorEnemy(m_pGameCamera, cursorX, cursorY);

		if (pEnemy != nullptr && pEnemy->GetErase() == false)
		{
			// Set the player's enemy pointer
			m_pPlayer->SetTargetEnemy(pEnemy);
			pEnemy->SetOutlineRender(true);

			// Set player alpha
			m_pPlayer->SetPlayerAlpha(0.6f);

			// Set target camera
			SavePreviousCameraMode();
			m_shouldRestorePreviousCameraMode = true;
			SetCameraMode(CameraMode::EnemyTarget);

			// Open cinematic letter box
			OpenLetterBox();
		}
	}
}

void CubbyGame::ReleaseEnemyTarget()
{
	if (m_pPlayer->GetTargetEnemy() != nullptr)
	{
		m_pPlayer->GetTargetEnemy()->SetOutlineRender(false);
		m_pPlayer->SetTargetEnemy(nullptr);

		// Reset player alpha
		m_pPlayer->SetPlayerAlpha(1.0f);

		// Return back to the previous camera state
		if (ShouldRestorePreviousCameraMode())
		{
			RestorePreviousCameraMode();
			InitializeCameraRotation();
		}

		// Close cinematic letter box
		CloseLetterBox();
	}
}

// GUI Helper functions
bool CubbyGame::IsGUIWindowStillDisplayed() const
{
	if (m_pInventoryGUI->IsLoaded())
	{
		return true;
	}

	if (m_pCharacterGUI->IsLoaded())
	{
		return true;
	}

	if (m_pLootGUI->IsLoaded())
	{
		return true;
	}

	if (m_pCraftingGUI->IsLoaded())
	{
		return true;
	}

	if (m_pQuestGUI->IsLoaded())
	{
		return true;
	}

	return false;
}

void CubbyGame::CloseAllGUIWindows() const
{
	if (m_pInventoryGUI->IsLoaded() && m_pInventoryGUI->IsLoadDelayed() == false)
	{
		m_pInventoryGUI->Unload();
	}

	if (m_pCharacterGUI->IsLoaded() && m_pCharacterGUI->IsLoadDelayed() == false)
	{
		m_pCharacterGUI->Unload();
	}

	if (m_pLootGUI->IsLoaded() && m_pLootGUI->IsLoadDelayed() == false)
	{
		m_pLootGUI->Unload();
	}

	if (m_pCraftingGUI->IsLoaded() && m_pCraftingGUI->IsLoadDelayed() == false)
	{
		m_pCraftingGUI->Unload();
	}

	if (m_pQuestGUI->IsLoaded() && m_pQuestGUI->IsLoadDelayed() == false)
	{
		m_pQuestGUI->Unload();
	}

	// Reset focus, also resets any text entry that we might have been doing.
	m_pGUI->ResetFocus();
}

void CubbyGame::CloseInteractionGUI()
{
	if (m_pCraftingGUI->IsLoaded())
	{
		m_pCraftingGUI->Unload();

		CloseLetterBox();

		if (IsGUIWindowStillDisplayed() == false)
		{
			TurnCursorOff(false);
		}

		if (ShouldRestorePreviousCameraMode())
		{
			RestorePreviousCameraMode();
			InitializeCameraRotation();
		}
	}
}

// Accessors
unsigned int CubbyGame::GetDefaultViewport() const
{
	return m_defaultViewport;
}

Camera* CubbyGame::GetGameCamera() const
{
	return m_pGameCamera;
}

Player* CubbyGame::GetPlayer() const
{
	return m_pPlayer;
}

ChunkManager* CubbyGame::GetChunkManager() const
{
	return m_pChunkManager;
}

BiomeManager* CubbyGame::GetBiomeManager() const
{
	return m_pBiomeManager;
}

FrontendManager* CubbyGame::GetFrontendManager() const
{
	return m_pFrontendManager;
}

BlockParticleManager* CubbyGame::GetBlockParticleManager() const
{
	return m_pBlockParticleManager;
}

NPCManager* CubbyGame::GetNPCManager() const
{
	return m_pNPCManager;
}

ItemManager* CubbyGame::GetItemManager() const
{
	return m_pItemManager;
}

InventoryManager* CubbyGame::GetInventoryManager() const
{
	return m_pInventoryManager;
}

RandomLootManager* CubbyGame::GetRandomLootManager() const
{
	return m_pRandomLootManager;
}

ModsManager* CubbyGame::GetModsManager() const
{
	return m_pModsManager;
}

CharacterGUI* CubbyGame::GetCharacterGUI() const
{
	return m_pCharacterGUI;
}

QuestGUI* CubbyGame::GetQuestGUI() const
{
	return m_pQuestGUI;
}

HUD* CubbyGame::GetHUD() const
{
	return m_pHUD;
}

ActionBar* CubbyGame::GetActionBar() const
{
	return m_pActionBar;
}

CubbySettings* CubbyGame::GetCubbySettings() const
{
	return m_pCubbySettings;
}
