/*************************************************************************
> File Name: CubbyControls.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Implement CubbyGame's methods related to control.
> Created Time: 2016/09/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "CubbyGame.h"

// Controls
void CubbyGame::UpdateControls(float dt)
{
	if (m_gamepadMovement == false)
	{
		UpdateKeyboardControls(dt);
		UpdateMouseControls(dt);
	}

	if (m_keyboardMovement == false)
	{
		if (m_pCubbyWindow->IsJoyStickConnected(0))
		{
			UpdateGamePadControls(dt);
		}
	}
}

void CubbyGame::UpdateKeyboardControls(float dt)
{
	GameMode gameMode = GetGameMode();

	if (gameMode == GameMode::Debug || m_cameraMode == CameraMode::Debug)
	{
		// Keyboard camera movements
		if (m_isKeyboardForward)
		{
			m_pGameCamera->Fly(20.0f * dt, true);
		}

		if (m_isKeyboardBackward)
		{
			m_pGameCamera->Fly(-20.0f * dt, true);
		}

		if (m_isKeyboardStrafeLeft)
		{
			m_pGameCamera->Strafe(-20.0f * dt, true);
		}

		if (m_isKeyboardStrafeRight)
		{
			m_pGameCamera->Strafe(20.0f * dt, true);
		}

		if (m_isKeyboardUp)
		{
			m_pGameCamera->Levitate(20.0f * dt, true);
		}

		if (m_isKeyboardDown)
		{
			m_pGameCamera->Levitate(-20.0f * dt, true);
		}
	}
	else if (gameMode == GameMode::Game)
	{
		// Pause menu
		if (m_isKeyboardMenu)
		{
			m_isKeyboardMenu = false;

			if (IsGUIWindowStillDisplayed())
			{
				CloseAllGUIWindows();

				if (IsGUIWindowStillDisplayed() == false)
				{
					TurnCursorOff(false);
				}
			}
			else
			{
				if (m_pFrontendManager->GetFrontendScreen() == FrontendScreen::None)
				{
					SetPauseMenu();
				}
				else if (m_pFrontendManager->GetFrontendScreen() == FrontendScreen::PauseMenu)
				{
					UnsetPauseMenu();
				}
				else if (m_pFrontendManager->GetFrontendScreen() == FrontendScreen::OptionsMenu)
				{
					m_pFrontendManager->SetFrontendScreen(FrontendScreen::PauseMenu);
				}
				else if (m_pFrontendManager->GetFrontendScreen() == FrontendScreen::ModMenu)
				{
					m_pFrontendManager->SetFrontendScreen(FrontendScreen::PauseMenu);
				}
			}

			// Cancel quit popup menu with escape button
			if (m_pFrontendManager->GetFrontendScreen() == FrontendScreen::QuitPopup)
			{
				GetInstance()->CancelQuitPopup();
			}
		}

		if (m_pPlayer->IsDead() == false)
		{
			// If we are paused, don't allow gameplay control movements
			if (m_isPaused == false)
			{
				if (IsGUIWindowStillDisplayed() == true)
				{
					// Reset movement speed since we have a GUI window open
					m_movementSpeed = 0.0f;
				}
				else
				{
					// Jumping
					if (m_isKeyboardSpace)
					{
						m_pPlayer->Jump();
					}

					// Attacking
					if (m_isAttackPressedMouse && m_canDoAttackMouse)
					{
						// Check interactions
						bool interaction = CheckInteractions();
						if (interaction == false)
						{
							m_pPlayer->PressAttack();
						}

						m_canDoAttackMouse = false;
					}
					if (m_isAttackReleasedMouse)
					{
						m_pPlayer->ReleaseAttack();
						m_isAttackReleasedMouse = false;
					}

					// Player movements
					bool resetMovementVector = false;
					if (m_isKeyboardForward == false && m_isKeyboardBackward == false && m_isKeyboardStrafeLeft == false && m_isKeyboardStrafeRight == false)
					{
						// Reduce the movement speed (drag)
						m_movementSpeed -= m_maxMovementSpeed / (m_movementDragTime / dt);

						if (m_movementSpeed <= 0.0f)
						{
							m_movementSpeed = 0.0f;
							m_keyboardMovement = false;
							m_pPlayer->StopMoving();
						}
					}
					else
					{
						m_keyboardMovement = true;

						// Increase the movement speed since we are pressing a movement key
						m_movementSpeed += m_maxMovementSpeed / (m_movementIncreaseTime / dt);

						// Don't allow faster than max movement
						if (m_movementSpeed > m_maxMovementSpeed)
						{
							m_movementSpeed = m_maxMovementSpeed;
						}
					}

					float targetModeMovementRatio = 0.65f;

					// Forwards, backwards, strafe, left, right directional movement
					if (m_isKeyboardForward)
					{
						if (resetMovementVector == false)
						{
							m_movementDirection = glm::vec3(0.0f, 0.0f, 0.0f);
							resetMovementVector = true;
						}

						if (m_pPlayer->GetTargetEnemy() == nullptr)
						{
							glm::vec3 cameraRight = m_pGameCamera->GetRight();
							glm::vec3 playerUp = m_pPlayer->GetUpVector();
							glm::vec3 moveDirection = normalize(cross(cameraRight, playerUp));		
							m_movementDirection -= moveDirection;
						}
						else
						{
							m_pPlayer->Move(m_movementSpeed * targetModeMovementRatio * dt);
						}
					}

					if (m_isKeyboardBackward)
					{
						if (resetMovementVector == false)
						{
							m_movementDirection = glm::vec3(0.0f, 0.0f, 0.0f);
							resetMovementVector = true;
						}

						if (m_pPlayer->GetTargetEnemy() == nullptr)
						{
							glm::vec3 cameraRight = m_pGameCamera->GetRight();
							glm::vec3 playerUp = m_pPlayer->GetUpVector();
							glm::vec3 moveDirection = normalize(cross(cameraRight, playerUp));
							m_movementDirection += moveDirection;
						}
						else
						{
							m_pPlayer->Move(-m_movementSpeed * targetModeMovementRatio * dt);
						}
					}

					if (m_isKeyboardStrafeLeft)
					{
						if (resetMovementVector == false)
						{
							m_movementDirection = glm::vec3(0.0f, 0.0f, 0.0f);
							resetMovementVector = true;
						}

						if (m_pPlayer->GetTargetEnemy() == nullptr)
						{
							glm::vec3 cameraRight = m_pGameCamera->GetRight();
							glm::vec3 moveDirection = -cameraRight;
							m_movementDirection += moveDirection;
						}
						else
						{
							m_targetCameraXAxisAmountTarget += (0.75f * dt);
							if (m_targetCameraXAxisAmountTarget > 1.0f)
							{
								m_targetCameraXAxisAmountTarget = 1.0f;
							}

							m_pPlayer->Strafe(m_movementSpeed * targetModeMovementRatio * dt);
						}
					}

					if (m_isKeyboardStrafeRight)
					{
						if (resetMovementVector == false)
						{
							m_movementDirection = glm::vec3(0.0f, 0.0f, 0.0f);
						}

						if (m_pPlayer->GetTargetEnemy() == nullptr)
						{
							glm::vec3 cameraRight = m_pGameCamera->GetRight();
							glm::vec3 moveDirection = -cameraRight;
							m_movementDirection -= moveDirection;
						}
						else
						{
							m_targetCameraXAxisAmountTarget -= (0.75f * dt);
							if (m_targetCameraXAxisAmountTarget < -1.0f)
							{
								m_targetCameraXAxisAmountTarget = -1.0f;
							}

							m_pPlayer->Strafe(-m_movementSpeed * targetModeMovementRatio * dt);
						}
					}

					if (length(m_movementDirection) > 0.001f && m_movementSpeed > m_movementStopThreshold)
					{
						bool shouldChangePlayerFacing = (m_cameraMode != CameraMode::FirstPerson);

						m_movementDirection = normalize(m_movementDirection);
						glm::vec3 amountMoved = m_pPlayer->MoveAbsolute(m_movementDirection, m_movementSpeed * dt, shouldChangePlayerFacing);
						m_pGameCamera->SetFakePosition(m_pGameCamera->GetFakePosition() + amountMoved);
					}
				}
			}
		}
	}
}

void CubbyGame::UpdateMouseControls(float dt)
{
	GameMode gameMode = GetGameMode();
	
	if (gameMode == GameMode::Debug || m_cameraMode == CameraMode::Debug)
	{
		if (m_isCameraRotate)
		{
			MouseCameraRotate();
		}
	}
	else if (gameMode == GameMode::Game)
	{
		if (m_pPlayer->IsDead() == false)
		{
			// If we are paused, don't allow gameplay control movements
			if (m_isPaused == false)
			{
				if (IsGUIWindowStillDisplayed() == false)
				{
					if (m_cameraMode == CameraMode::MouseRotate)
					{
						MouseCameraRotate();
					}
					else if (m_cameraMode == CameraMode::AutoCamera)
					{
						// Do nothing, camera rotation is not controlled by player.
					}
					else if (m_cameraMode == CameraMode::FirstPerson)
					{
						MouseCameraRotate();
					}
				}
			}
		}
	}
}

void CubbyGame::UpdateGamePadControls(float dt)
{
	JoystickCameraZoom(dt);

	m_isJoystickJump = m_pCubbyWindow->GetJoystickButton(0, 0);
	if (m_isAttackPressedJoystick)
	{
		m_canDoAttackJoystick = m_pCubbyWindow->GetJoystickAxisValue(0, 2) >= -0.75f;
		m_isAttackReleasedJoystick = m_pCubbyWindow->GetJoystickAxisValue(0, 2) >= -0.75f;
	}
	m_isAttackPressedJoystick = m_pCubbyWindow->GetJoystickAxisValue(0, 2) <= -0.75f;

	// TODO: Need to get newer version of glfw to support different triggers for LT, RT
	m_isTargetEnemyPressedJoystick = false; 
	// TODO: Need to get newer version of glfw to support different triggers for LT, RT
	m_isTargetEnemyReleasedJoystick = false; 

	GameMode gameMode = GetGameMode();

	if (gameMode == GameMode::Debug || m_cameraMode == CameraMode::Debug)
	{
		JoystickCameraMove(dt);
		JoystickCameraRotate(dt);
	}
	else if (gameMode == GameMode::Game)
	{
		if (m_pPlayer->IsDead() == false)
		{
			// If we are paused, don't allow gameplay control movements
			if (m_isPaused == false)
			{
				if (IsGUIWindowStillDisplayed() == true)
				{
					// Reset movement speed since we have a GUI window open
					m_movementSpeed = 0.0f;
				}
				else
				{
					if (m_cameraMode == CameraMode::MouseRotate)
					{
						JoystickCameraRotate(dt);
					}
					else if (m_cameraMode == CameraMode::AutoCamera)
					{
						// Do nothing, camera rotation is not controlled by player.
					}
					else if (m_cameraMode == CameraMode::FirstPerson)
					{
						JoystickCameraRotate(dt);
					}

					// Jumping
					if (m_isJoystickJump)
					{
						m_pPlayer->Jump();
					}

					// Enemy targeting
					if (m_cameraMode != CameraMode::FirstPerson)
					{
						if (m_isTargetEnemyPressedJoystick)
						{
							if (m_pPlayer->GetTargetEnemy() == nullptr)
							{
								SetEnemyTarget();
							}
						}
						if (m_isTargetEnemyReleasedJoystick)
						{
							if (m_pPlayer->GetTargetEnemy() != nullptr)
							{
								ReleaseEnemyTarget();
							}
						}
					}

					// Attacking
					if (m_isAttackPressedJoystick && m_canDoAttackJoystick)
					{
						// Check interactions
						bool interaction = CheckInteractions();
						
						if (interaction == false)
						{
							m_pPlayer->PressAttack();
						}
						
						m_canDoAttackJoystick = false;
					}
					if (m_isAttackReleasedJoystick)
					{
						m_pPlayer->ReleaseAttack();
						m_isAttackReleasedJoystick = false;
					}

					// Player movements
					float axisX = m_pCubbyWindow->GetJoystickAxisValue(0, 0);
					float axisY = m_pCubbyWindow->GetJoystickAxisValue(0, 1);

					// Dead zones
					if (fabs(axisX) < m_pCubbyWindow->GetJoystickAnalogDeadZone())
					{
						axisX = 0.0f;
					}
					if (fabs(axisY) < m_pCubbyWindow->GetJoystickAnalogDeadZone())
					{
						axisY = 0.0f;
					}

					if (fabs(axisX) <= 0.0f && fabs(axisY) <= 0.0f)
					{
						// Reduce the movement speed (drag)
						m_movementSpeed -= m_maxMovementSpeed / (m_movementDragTime / dt);

						if (m_movementSpeed <= 0.0f)
						{
							m_movementSpeed = 0.0f;
							m_gamepadMovement = false;
							m_pPlayer->StopMoving();
						}
					}
					else
					{
						m_gamepadMovement = true;

						// Increase the movement speed since we are pressing a movement key
						m_movementSpeed += m_maxMovementSpeed / (m_movementIncreaseTime / dt);

						// Don't allow faster than max movement
						if (m_movementSpeed > m_maxMovementSpeed)
						{
							m_movementSpeed = m_maxMovementSpeed;
						}
					}

					if (m_pPlayer->GetTargetEnemy() == nullptr)
					{
						glm::vec3 cameraRight = m_pGameCamera->GetRight();
						glm::vec3 playerUp = m_pPlayer->GetUpVector();
						glm::vec3 moveDirection = normalize(cross(cameraRight, playerUp));
						
						m_movementDirection += moveDirection * axisY;
						m_movementDirection += cameraRight * axisX;

						if (length(m_movementDirection) > 0.001f && m_movementSpeed > m_movementStopThreshold)
						{
							bool shouldChangePlayerFacing = (m_cameraMode != CameraMode::FirstPerson);

							m_movementDirection = normalize(m_movementDirection);
							glm::vec3 amountMoved = m_pPlayer->MoveAbsolute(m_movementDirection, m_movementSpeed * dt, shouldChangePlayerFacing);
							m_pGameCamera->SetFakePosition(m_pGameCamera->GetFakePosition() + amountMoved);
						}
					}
					else
					{
						m_targetCameraXAxisAmountTarget -= (axisX * dt);
						
						if (m_targetCameraXAxisAmountTarget > 1.0f)
						{
							m_targetCameraXAxisAmountTarget = 1.0f;
						}
						
						if (m_targetCameraXAxisAmountTarget < -1.0f)
						{
							m_targetCameraXAxisAmountTarget = -1.0f;
						}

						// TODO: Is this a good value for gamepad movement speed?
						m_pPlayer->Move(-axisY * 10.0f * dt);  
						// TODO: Is this a good value for gamepad movement speed?
						m_pPlayer->Strafe(-axisX * 10.0f * dt);
					}
				}
			}
		}
	}
}