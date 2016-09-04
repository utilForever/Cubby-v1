/*************************************************************************
> File Name: Chunk.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Cubby update functions.
> Created Time: 2016/07/09
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Utils/Interpolator.h>
#include <Utils/TimeManager.h>

#include "CubbyGame.h"

#ifdef __linux__
#include <sys/time.h>
#endif //__linux__

// Updating
void CubbyGame::Update()
{
	// FPS
#ifdef _WIN32
	QueryPerformanceCounter(&m_fpsCurrentTicks);
	m_deltaTime = (static_cast<float>(m_fpsCurrentTicks.QuadPart - m_fpsPreviousTicks.QuadPart) / static_cast<float>(m_fpsTicksPerSecond.QuadPart));
#else
	struct timeval tm;
	gettimeofday(&tm, nullptr);
	m_fpsCurrentTicks = static_cast<double>(tm.tv_sec) + static_cast<double>(tm.tv_usec) / 1000000.0;
	m_deltaTime = (m_fpsCurrentTicks - m_fpsPreviousTicks);
#endif //_WIN32
	m_fps = 1.0f / m_deltaTime;
	m_fpsPreviousTicks = m_fpsCurrentTicks;

	float maxDeltaTime = 0.25f;
	if (m_deltaTime > maxDeltaTime)
	{
		std::cout << "Warning: DeltaTime exceeded sensible value, switching dt from " << m_deltaTime << " to " << maxDeltaTime << ".\n";
		m_deltaTime = maxDeltaTime;
	}

	// Update interpolator singleton
	Interpolator::GetInstance()->Update(m_deltaTime);

	// Pause the interpolator we are are paused.
	Interpolator::GetInstance()->SetPaused(m_isPaused);

	// Update the time manager (countdowntimers);
	TimeManager::GetInstance()->Update(m_deltaTime);

	// Update the audio manager
	SoundManager::GetInstance()->Update(m_pGameCamera->GetPosition(), m_pGameCamera->GetFacing(), m_pGameCamera->GetUp());

	// Update the initial wait timer and variables, so we don't do gameplay updates straight away
	if (m_initialStartWait == true)
	{
		if (m_initialWaitTimer > m_initialWaitTime)
		{
			m_initialStartWait = false;
		}
		else
		{
			m_initialWaitTimer += m_deltaTime;
			m_initialStartWait = true;
		}
	}

	// Update the current biome
	Biome currentBiome = m_pBiomeManager->GetBiome(m_pPlayer->GetCenter());
	if (currentBiome != m_currentBiome)
	{
		m_pSkybox->SetCurrentBiome(currentBiome);
		m_currentBiome = currentBiome;
	}

	// Update game music
	if (m_gameMode == GameMode::Game)
	{
		UpdateGameMusic(m_deltaTime);
	}
	UpdateMusicVolume(0.0f);

	// Main components update
	if (m_isPaused == false && m_initialStartWait == false)
	{
		// Update the lighting manager
		m_pLightingManager->Update(m_deltaTime);

		// Block particle manager
		m_pBlockParticleManager->Update(m_deltaTime);

		// Instance manager
		m_pInstanceManager->Update(m_deltaTime);

		// Scenery manager
		//m_pSceneryManager->Update(m_deltaTime);

		// Inventory manager
		m_pInventoryManager->Update();

		// Item manager
		m_pItemManager->Update(m_deltaTime);
		m_pItemManager->UpdateItemLights();
		m_pItemManager->UpdateItemParticleEffects();
		m_interactItemMutex.lock();
		m_pInteractItem = m_pItemManager->CheckItemPlayerInteraction();
		m_interactItemMutex.unlock();

		// Projectile manager
		m_pProjectileManager->Update(m_deltaTime);
		m_pProjectileManager->UpdateProjectileLights();
		m_pProjectileManager->UpdateProjectileParticleEffects();

		// Text effects manager
		m_pTextEffectsManager->Update(m_deltaTime);

		// Update the NPC manager
		m_pNPCManager->Update(m_deltaTime);

		// Update the enemy manager
		m_pEnemyManager->Update(m_deltaTime);

		// Update the biome manager
		//m_pBiomeManager->Update(m_deltaTime);

		// Player
		if (m_animationUpdate)
		{
			m_pPlayer->Update(m_deltaTime);
		}

		// Camera faked position
		if (m_cameraMode == CameraMode::MouseRotate || m_cameraMode == CameraMode::AutoCamera || m_cameraMode == CameraMode::NPCDialog)
		{
			glm::vec3 playerMovementChanged = m_pPlayer->GetPositionMovementAmount();
			m_pGameCamera->SetFakePosition(m_pGameCamera->GetFakePosition() + playerMovementChanged);
		}

		// Water
		m_elapsedWaterTime += m_deltaTime;
	}

	// Update the chunk manager
	m_pChunkManager->Update(m_deltaTime);

	// Update name picking
	if (m_pGUI->IsMouseInteractingWithGUIComponent(false) == false)
	{
		UpdateNamePicking();
	}

	// Update the NPC hover selection based on the mouse name picking
	if (m_gameMode == GameMode::FrontEnd)
	{
		if (m_isNamePickingSelected)
		{
			m_pNPCManager->UpdateHoverNamePickingSelection(m_pickedObject);
		}
		else
		{
			m_pNPCManager->UpdateHoverNamePickingSelection(-1);
		}
	}

	// Update controls
	UpdateControls(m_deltaTime);

	// Update the camera based on movements
	if (m_gameMode == GameMode::Game)
	{
		UpdateCamera(m_deltaTime);
		m_pPlayer->SetCameraPosition(m_pGameCamera->GetPosition());
		m_pPlayer->SetCameraForward(normalize(m_pGameCamera->GetFacing()));
		m_pPlayer->SetCameraUp(normalize(m_pGameCamera->GetUp()));
		m_pPlayer->SetCameraRight(normalize(m_pGameCamera->GetRight()));
	}

	// Update the dynamic camera zoom
	UpdateCameraZoom(m_deltaTime);

	// Update the camera clipping
	m_targetCameraPositionBeforeClipping = m_pGameCamera->GetFakePosition();
	UpdateCameraClipping(m_deltaTime);

	// Update the player's alpha and transparency based on camera distance to player
	if (m_gameMode == GameMode::Game && m_cameraMode != CameraMode::Debug)
	{
		UpdatePlayerAlpha(m_deltaTime);
	}

	// Update the frontend
	m_pFrontendManager->Update(m_deltaTime);

	// Update the GUI
	int x = m_pCubbyWindow->GetCursorX();
	int y = m_pCubbyWindow->GetCursorY();
	
	m_pGUI->Update(m_deltaTime);
	
	if (IsCursorOn())
	{
		m_pGUI->ImportMouseMotion(x, m_windowHeight - y);
	}
	
	UpdateGUI(m_deltaTime);

	if (m_isPaused == false && m_initialStartWait == false)
	{
		// Update game GUI
		UpdateGameGUI(m_deltaTime);

		// Update lights
		UpdateLights(m_deltaTime);
	}

	// Update the application and window
	m_pCubbyWindow->Update(m_deltaTime);
}

void CubbyGame::UpdateNamePicking()
{
	POINT mouse = { GetInstance()->GetWindowCursorX(), (m_windowHeight - GetInstance()->GetWindowCursorY()) };

	// Push attributes
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	// Disabled rendering
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	// Set the projection mode
	m_pRenderer->SetProjectionMode(ProjectionMode::PERSPECTIVE, m_defaultViewport);

	// Init name picking
	m_pRenderer->InitNameStack();

	// Start name picking
	m_pRenderer->StartNamePicking(m_defaultViewport, mouse.x, mouse.y);

	// Set the look at camera
	m_pGameCamera->Look();

	// Different sub-systems render name picking
	{
		m_pNPCManager->RenderNamePicking();
	}

	// End the name picking
	m_pickedObject = m_pRenderer->GetPickedObject();

	// Pop attributes
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);   // Enable rendering
	glPopAttrib();

	if (m_pickedObject != -1)
	{
		m_isNamePickingSelected = true;
	}
	else
	{
		m_isNamePickingSelected = false;
	}
}

void CubbyGame::UpdatePlayerAlpha(float dt) const
{
	glm::vec3 toPlayer = ((m_pPlayer->GetCenter() + Player::PLAYER_CENTER_OFFSET) - m_pGameCamera->GetPosition());
	float distance = length(toPlayer);
	float alpha = (distance) / 7.5f;

	if (m_cameraMode == CameraMode::NPCDialog)
	{
		// Dialog and crafting camera alpha
		alpha = 1.0f;
	}

	if (m_cameraMode == CameraMode::FirstPerson)
	{
		// First person alpha
		alpha = 0.45f;
	}

	m_pPlayer->SetPlayerAlpha(alpha);
}

void CubbyGame::UpdateLights(float dt)
{
	m_pRenderer->EditLightPosition(m_defaultLight, m_defaultLightPosition);
}

void CubbyGame::UpdateGameGUI(float dt) const
{
	if (m_pInventoryGUI->IsLoaded())
	{
		m_pInventoryGUI->Update(dt);
	}

	if (m_pCharacterGUI->IsLoaded())
	{
		m_pCharacterGUI->Update(dt);
	}

	if (m_pLootGUI->IsLoaded())
	{
		m_pLootGUI->Update(dt);
	}

	if (m_pCraftingGUI->IsLoaded())
	{
		m_pCraftingGUI->Update(dt);
	}

	if (m_pQuestGUI->IsLoaded())
	{
		m_pQuestGUI->Update(dt);
	}

	if (m_pHUD->IsLoaded())
	{
		m_pHUD->Update(dt);
	}
}
