/*************************************************************************
> File Name: CubbyInput.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Implement CubbyGame's methods related to input.
> Created Time: 2016/09/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Maths/3DMaths.h>

#include "CubbyGame.h"

// Input callbacks
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
	{
		CubbyGame::GetInstance()->KeyPressed(key, scancode, mods);

		break;
	}
	case GLFW_RELEASE:
	{
		CubbyGame::GetInstance()->KeyReleased(key, scancode, mods);
		break;
	}
	case GLFW_REPEAT:
	{
		break;
	}
	default:
	{
		
	}
	}
}

void CharacterCallback(GLFWwindow* window, unsigned int keyCode)
{
	CubbyGame::GetInstance()->CharacterEntered(keyCode);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			CubbyGame::GetInstance()->MouseLeftPressed();
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			CubbyGame::GetInstance()->MouseRightPressed();
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			CubbyGame::GetInstance()->MouseMiddlePressed();
		}

		break;
	}
	case GLFW_RELEASE:
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			CubbyGame::GetInstance()->MouseLeftReleased();
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			CubbyGame::GetInstance()->MouseRightReleased();
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			CubbyGame::GetInstance()->MouseMiddleReleased();
		}

		break;
	}
	default:
	{
		throw std::logic_error("Invalid Mouse Button Input!");
	}
	}
}

void MouseScrollCallback(GLFWwindow* window, double x, double y)
{
	CubbyGame::GetInstance()->MouseScroll(x, y);
}

// Input
void CubbyGame::KeyPressed(int key, int scanCode, int mods)
{
	m_pGUI->KeyPressed(key, mods);

	if (m_pGUI->IsKeyboardInteractingWithGUIComponent() && key != GLFW_KEY_ESCAPE)
	{
		// For text box entry
		return;
	}

	switch (key)
	{
		// Player movement
	case GLFW_KEY_W:
	{
		m_isKeyboardForward = true;
		break;
	}
	case GLFW_KEY_S:
	{
		m_isKeyboardBackward = true;
		break;
	}
	case GLFW_KEY_A:
	{
		m_isKeyboardLeft = true;
		m_isKeyboardStrafeLeft = true;
		break;
	}
	case GLFW_KEY_D:
	{
		m_isKeyboardRight = true;
		m_isKeyboardStrafeRight = true;
		break;
	}
	case GLFW_KEY_F:
	{
		m_isKeyboardUp = true;
		break;
	}
	case GLFW_KEY_V:
	{
		m_isKeyboardDown = true;
		break;
	}
	case GLFW_KEY_SPACE:
	{
		m_isKeyboardSpace = true;
		break;
	}
	case GLFW_KEY_ESCAPE:
	{
		m_isKeyboardMenu = true;
		break;
	}
	default:
	{
		
	}
	}
}

void CubbyGame::KeyReleased(int key, int scancode, int mods)
{
	m_pGUI->KeyReleased(key, mods);

	if (m_pGUI->IsKeyboardInteractingWithGUIComponent() && key != GLFW_KEY_ESCAPE)
	{
		// For text box entry
		return; 
	}

	switch (key)
	{
	// Player movement
	case GLFW_KEY_W:
	{
		m_isKeyboardForward = false;
		break;
	}
	case GLFW_KEY_S:
	{
		m_isKeyboardBackward = false;
		break;
	}
	case GLFW_KEY_A:
	{
		m_isKeyboardLeft = false;
		m_isKeyboardStrafeLeft = false;
		break;
	}
	case GLFW_KEY_D:
	{
		m_isKeyboardRight = false;
		m_isKeyboardStrafeRight = false;
		break;
	}
	case GLFW_KEY_F:
	{
		m_isKeyboardUp = false;
		break;
	}
	case GLFW_KEY_V:
	{
		m_isKeyboardDown = false;
		break;
	}
	case GLFW_KEY_SPACE:
	{
		m_isKeyboardSpace = false;
		break;
	}
	case GLFW_KEY_ESCAPE:
	{
		m_isKeyboardMenu = false;
		break;
	}
	case GLFW_KEY_O:
	{
		m_pDebugCameraOptionBox->SetToggled(true);
		CameraModeChanged();
		break;
	}
	case GLFW_KEY_L:
	{
		SetPaused(!IsPaused());
		break;
	}
	case GLFW_KEY_P:
	{
		if (m_pGUI->IsKeyboardInteractingWithGUIComponent() == false)
		{
			if (m_pMainWindow->IsVisible() == false)
			{
				ShowGUI();
			}
			else
			{
				HideGUI();
			}
		}
		break;
	}

	// Game GUI
	case GLFW_KEY_I:
	{
		if (GetGameMode() == GameMode::Game)
		{
			if (m_pPlayer->IsDead() == false)
			{
				if (m_pInventoryGUI->IsLoaded())
				{
					m_pInventoryGUI->Unload();

					if (GetInstance()->IsGUIWindowStillDisplayed() == false)
					{
						TurnCursorOff(false);
					}
				}
				else if (m_pFrontendManager->GetFrontendScreen() == FrontendScreen::None)
				{
					m_pInventoryGUI->Load();

					m_pPlayer->StopMoving();

					TurnCursorOn(false, false);
				}
			}
		}
		break;
	}
	case GLFW_KEY_C:
	{
		if (GetGameMode() == GameMode::Game)
		{
			if (m_pPlayer->IsDead() == false)
			{
				if (m_pCharacterGUI->IsLoaded())
				{
					m_pCharacterGUI->Unload();

					if (GetInstance()->IsGUIWindowStillDisplayed() == false)
					{
						TurnCursorOff(false);
					}
				}
				else if (m_pFrontendManager->GetFrontendScreen() == FrontendScreen::None)
				{
					m_pCharacterGUI->Load();

					m_pPlayer->StopMoving();

					TurnCursorOn(false, false);
				}
			}
		}
		break;
	}
	case GLFW_KEY_K:
	{
		if (GetGameMode() == GameMode::Game)
		{
			if (m_pPlayer->IsDead() == false)
			{
				if (m_pQuestGUI->IsLoaded())
				{
					m_pQuestGUI->Unload();

					if (GetInstance()->IsGUIWindowStillDisplayed() == false)
					{
						TurnCursorOff(false);
					}
				}
				else if (m_pFrontendManager->GetFrontendScreen() == FrontendScreen::None)
				{
					m_pQuestGUI->Load();

					m_pPlayer->StopMoving();

					TurnCursorOn(false, false);
				}
			}
		}
		break;
	}

	// Number keys
	case GLFW_KEY_1:
	{
		m_pActionBar->UseActionBarslot(0);
		break;
	}
	case GLFW_KEY_2:
	{
		m_pActionBar->UseActionBarslot(1);
		break;
	}
	case GLFW_KEY_3:
	{
		m_pActionBar->UseActionBarslot(2);
		break;
	}
	case GLFW_KEY_4:
	{
		m_pActionBar->UseActionBarslot(3);
		break;
	}
	case GLFW_KEY_5:
	{
		m_pActionBar->UseActionBarslot(4);
		break;
	}
	case GLFW_KEY_6:
	{
		m_pActionBar->UseActionBarslot(5);
		break;
	}
	case GLFW_KEY_7:
	{
		m_pActionBar->UseActionBarslot(6);
		break;
	}
	case GLFW_KEY_8:
	{
		m_pActionBar->UseActionBarslot(7);
		break;
	}
	case GLFW_KEY_9:
	{
		m_pActionBar->UseActionBarslot(8);
		break;
	}
	case GLFW_KEY_0:
	{
		m_pActionBar->UseActionBarslot(9);
		break;
	}
	default:
	{
		
	}
	}
}

void CubbyGame::CharacterEntered(int keyCode) const
{
	m_pGUI->CharacterEntered(keyCode);
}

void CubbyGame::MouseLeftPressed()
{
	m_isPressedCursorDown = true;

	if (IsCursorOn())
	{
		m_pGUI->MousePressed(static_cast<int>(MouseButton::BUTTON1));
	}

	if (IsCursorOn() == false || !m_pGUI->IsMouseInteractingWithGUIComponent(false))
	{
		m_currentX = m_pCubbyWindow->GetCursorX();
		m_currentY = m_pCubbyWindow->GetCursorY();
		m_pressedX = m_currentX;
		m_pressedY = m_currentY;

		if (m_gameMode == GameMode::Debug || m_cameraMode == CameraMode::Debug)
		{
			// Turn cursor off
			if (IsCursorOn() == true)
			{
				TurnCursorOff(false);
			}

			m_isCameraRotate = true;
		}
		else if (m_gameMode == GameMode::Game)
		{
			m_isAttackPressedMouse = true;
		}
	}

	// For front-end character selection
	if (m_gameMode == GameMode::FrontEnd && !m_pGUI->IsMouseInteractingWithGUIComponent(false))
	{
		if (m_isNamePickingSelected)
		{
			m_pNPCManager->UpdateNamePickingSelection(m_pickedObject);
		}
		else
		{
			m_pNPCManager->UpdateNamePickingSelection(-1);
		}
	}

	// For front-end credits screen advancement
	if (m_gameMode == GameMode::FrontEnd && m_pFrontendManager->GetFrontendScreen() == FrontendScreen::Credits)
	{
		m_pFrontendManager->GotoNextCreditScreen();
	}
}

void CubbyGame::MouseLeftReleased()
{
	m_isPressedCursorDown = false;

	if (IsCursorOn())
	{
		m_pGUI->MouseReleased(static_cast<int>(MouseButton::BUTTON1));
	}

	if (m_gameMode == GameMode::Debug || m_cameraMode == CameraMode::Debug)
	{
		if (!m_pGUI->IsMouseInteractingWithGUIComponent(false))
		{
			// Turn cursor on
			if (IsCursorOn() == false)
			{
				TurnCursorOn(true, false);
			}
		}

		m_isCameraRotate = false;
	}
	else if (m_gameMode == GameMode::Game)
	{
		m_isAttackReleasedMouse = true;
		m_isAttackPressedMouse = false;
		m_canDoAttackMouse = true;
	}
}

void CubbyGame::MouseRightPressed()
{
	m_isPressedCursorDown = true;

	if (IsCursorOn())
	{
		m_pGUI->MousePressed(static_cast<int>(MouseButton::BUTTON2));
	}

	if (m_gameMode == GameMode::Game && m_cameraMode != CameraMode::FirstPerson)
	{
		SetEnemyTarget();
	}

	if (IsCursorOn() == false || !m_pGUI->IsMouseInteractingWithGUIComponent(false))
	{
		m_currentX = m_pCubbyWindow->GetCursorX();
		m_currentY = m_pCubbyWindow->GetCursorY();
		m_pressedX = m_currentX;
		m_pressedY = m_currentY;
	}
}

void CubbyGame::MouseRightReleased()
{
	m_isPressedCursorDown = false;

	if (IsCursorOn())
	{
		m_pGUI->MouseReleased(static_cast<int>(MouseButton::BUTTON2));
	}

	ReleaseEnemyTarget();
}

void CubbyGame::MouseMiddlePressed() const
{
	if (IsCursorOn())
	{
		m_pGUI->MousePressed(static_cast<int>(MouseButton::BUTTON3));
	}
}

void CubbyGame::MouseMiddleReleased() const
{
	if (IsCursorOn())
	{
		m_pGUI->MouseReleased(static_cast<int>(MouseButton::BUTTON3));
	}
}

void CubbyGame::MouseScroll(double x, double y)
{
	GameMode gameMode = GetGameMode();

	if (m_pPlayer->IsDead() == false || (gameMode == GameMode::Debug || m_cameraMode == CameraMode::Debug))
	{
		if (m_isPaused == false)
		{
			// Don't allow mouse zooming when we are an enemy target.
			if (m_pPlayer->GetTargetEnemy() == nullptr)
			{
				if (IsCursorOn() == false || !m_pGUI->IsMouseInteractingWithGUIComponent(false))
				{
					if (m_cameraMode != CameraMode::FirstPerson)
					{
						m_maxCameraDistance += static_cast<float>(-y * 0.5f);

						WrapCameraZoomValue();
					}
					else
					{
						if (y < 0.0f)
						{
							m_cameraDistance = 2.0f;
							m_maxCameraDistance = 2.0f;
						}
					}
				}
			}
		}
	}
}

void CubbyGame::WrapCameraZoomValue()
{
	float minAmount = 0.5f;
	float maxAmount = 15.0f;

	// Camera rotation modes
	if (m_gameMode == GameMode::Game && (m_cameraMode == CameraMode::AutoCamera || m_cameraMode == CameraMode::MouseRotate || m_cameraMode == CameraMode::NPCDialog))
	{
		minAmount = 1.0f;
		maxAmount = 15.0f;
	}

	// First person mode
	if (m_gameMode == GameMode::Game && m_cameraMode == CameraMode::FirstPerson)
	{
		minAmount = 1.25f;
		maxAmount = 1.75f;
	}

	if (m_maxCameraDistance <= minAmount)
	{
		m_maxCameraDistance = minAmount;
	}

	if (m_maxCameraDistance >= maxAmount)
	{
		m_maxCameraDistance = maxAmount;
	}
}

// Mouse controls
void CubbyGame::MouseCameraRotate()
{
	int x = m_pCubbyWindow->GetCursorX();
	int y = m_pCubbyWindow->GetCursorY();

	float changeX;
	float changeY;

	// The mouse hasn't moved so just return
	if ((m_currentX == x) && (m_currentY == y))
	{
		return;
	}

	// Calculate and scale down the change in position
	changeX = (x - m_currentX) / 5.0f;
	changeY = (y - m_currentY) / 5.0f;

	// Upside down
	if (m_pGameCamera->GetUp().y < 0.0f)
	{
		changeX = -changeX;
	}

	// First person mode
	if (m_cameraMode == CameraMode::FirstPerson)
	{
		changeY = -changeY;
	}

	// Inverted mouse
	if (m_pCubbySettings->m_invertedMouse == true)
	{
		changeY = -changeY;
	}

	// Scale based on mouse sensitivity options
	changeX *= m_pCubbySettings->m_mouseSensitivity * 0.02f;
	changeY *= m_pCubbySettings->m_mouseSensitivity * 0.02f;

	// Limit the rotation, so we can't go 'over' or 'under' the player with out rotations
	glm::vec3 cameraFacing = m_pGameCamera->GetFacing();
	float dotResult = acos(dot(cameraFacing, glm::vec3(0.0f, 1.0f, 0.0f)));
	float rotationDegrees = RadianToDegree(dotResult) - 90.0f;
	float limitAngle = 75.0f;
	if ((rotationDegrees > limitAngle && changeY < 0.0f) || (rotationDegrees < -limitAngle && changeY > 0.0f))
	{
		changeY = 0.0f;
	}

	if (m_cameraMode == CameraMode::FirstPerson)
	{
		m_pGameCamera->Rotate(changeY * 0.75f, 0.0f, 0.0f);
		m_pGameCamera->RotateY(-changeX * 0.75f);
	}
	else
	{
		m_pGameCamera->RotateAroundPoint(changeY * 0.75f, 0.0f, 0.0f, true);
		m_pGameCamera->RotateAroundPointY(-changeX * 0.75f, true);
	}

	m_currentX = x;
	m_currentY = y;
}

// Joystick controls
void CubbyGame::JoystickCameraMove(float dt) const
{
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

	float changeX = axisX * 10.0f * dt;
	float changeY = axisY * 10.0f * dt;

	m_pGameCamera->Fly(-changeY);
	m_pGameCamera->Strafe(changeX);
}

void CubbyGame::JoystickCameraRotate(float dt) const
{
	float axisX = m_pCubbyWindow->GetJoystickAxisValue(0, 4);
	float axisY = m_pCubbyWindow->GetJoystickAxisValue(0, 3);

	// Dead zones
	if (fabs(axisX) < m_pCubbyWindow->GetJoystickAnalogDeadZone())
	{
		axisX = 0.0f;
	}
	if (fabs(axisY) < m_pCubbyWindow->GetJoystickAnalogDeadZone())
	{
		axisY = 0.0f;
	}

	float changeX = axisX * 150.0f * dt;
	float changeY = axisY * 150.0f * dt;

	// Upside down
	if (m_pGameCamera->GetUp().y < 0.0f)
	{
		changeX = -changeX;
	}

	// First person mode
	if (m_cameraMode == CameraMode::FirstPerson)
	{
		changeY = -changeY;
	}

	// Inverted mouse
	if (m_pCubbySettings->m_invertedMouse == true)
	{
		changeY = -changeY;
	}

	// Scale based on gamepad sensitivity options
	changeX *= m_pCubbySettings->m_gamepadSensitivity * 0.02f;
	changeY *= m_pCubbySettings->m_gamepadSensitivity * 0.02f;

	// Limit the rotation, so we can't go 'over' or 'under' the player with out rotations
	glm::vec3 cameraFacing = m_pGameCamera->GetFacing();
	float dotResult = acos(dot(cameraFacing, glm::vec3(0.0f, 1.0f, 0.0f)));
	float rotationDegrees = RadianToDegree(dotResult) - 90.0f;
	float limitAngle = 75.0f;
	if ((rotationDegrees > limitAngle && changeY < 0.0f) || (rotationDegrees < -limitAngle && changeY > 0.0f))
	{
		changeY = 0.0f;
	}

	if (m_cameraMode == CameraMode::FirstPerson)
	{
		m_pGameCamera->Rotate(changeY, 0.0f, 0.0f);
		m_pGameCamera->RotateY(-changeX);
	}
	else
	{
		m_pGameCamera->RotateAroundPoint(changeY, 0.0f, 0.0f, true);
		m_pGameCamera->RotateAroundPointY(-changeX, true);
	}
}

void CubbyGame::JoystickCameraZoom(float dt)
{
	bool zoomOut = m_pCubbyWindow->GetJoystickButton(0, 4);
	bool zoomIn = m_pCubbyWindow->GetJoystickButton(0, 5);

	float zoomAmount = 0.0f;
	if (zoomIn)
	{
		zoomAmount = 10.0f;
	}
	if (zoomOut)
	{
		zoomAmount = -10.0f;
	}

	float changeY = zoomAmount * dt;

	if (m_cameraMode != CameraMode::FirstPerson)
	{
		m_maxCameraDistance += static_cast<float>(-changeY);

		WrapCameraZoomValue();
	}
	else
	{
		if (changeY < 0.0f)
		{
			m_cameraDistance = 2.0f;
			m_maxCameraDistance = 2.0f;
		}
	}

	WrapCameraZoomValue();
}