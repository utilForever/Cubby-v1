/*************************************************************************
> File Name: CubbyCamera.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Implement CubbyGame's methods related to camera.
> Created Time: 2016/09/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Maths/3DMaths.h>

#include "CubbyGame.h"

// Camera controls
void CubbyGame::UpdateCamera(float dt)
{
	if (m_pPlayer->IsDead() == false)
	{
		if (m_isPaused == false)
		{
			if (m_cameraMode == CameraMode::MouseRotate)
			{
				UpdateCameraAutoCamera(dt, false);
			}
			else if (m_cameraMode == CameraMode::AutoCamera)
			{
				UpdateCameraAutoCamera(dt, true);
			}
			else if (m_cameraMode == CameraMode::FirstPerson)
			{
				UpdateCameraFirstPerson(dt);
			}
			else if (m_cameraMode == CameraMode::NPCDialog)
			{
				UpdateCameraNPCDialog(dt);
			}
			else if (m_cameraMode == CameraMode::EnemyTarget)
			{
				UpdateCameraEnemyTarget(dt);
			}
		}

		if (m_gameMode == GameMode::Game && m_cameraMode != CameraMode::Debug)
		{
			UpdateCameraModeSwitching();
		}
	}
}

void CubbyGame::UpdateCameraModeSwitching()
{
	if (m_cameraDistance < 1.5f)
	{
		if (m_cameraMode != CameraMode::FirstPerson)
		{
			SavePreviousCameraMode();
			SetCameraMode(CameraMode::FirstPerson);
			
			// Enter first person mode
			m_pPlayer->SetFirstPersonMode();
			
			m_pGameCamera->SetZoomAmount(1.5f);
		}
	}
	else
	{
		if (m_cameraMode == CameraMode::FirstPerson)
		{
			if (m_previousCameraMode == CameraMode::MouseRotate)
			{
				m_pMouseRotateCameraOptionBox->SetToggled(true);
			}
			else if (m_previousCameraMode == CameraMode::AutoCamera)
			{
				m_pAutoCameraOptionBox->SetToggled(true);
			}

			m_cameraDistance = 1.5f;
			m_maxCameraDistance = 1.5f;

			// Enter third person mode
			m_pPlayer->SetThirdPersonMode();

			RestorePreviousCameraMode();
			InitializeCameraRotation();
		}
	}
}

void CubbyGame::InitializeCameraRotation()
{
	m_currentX = m_pCubbyWindow->GetCursorX();
	m_currentY = m_pCubbyWindow->GetCursorY();

	glm::vec3 ratios = normalize(glm::vec3(2.5f, 1.0f, 0.0f));

	m_cameraBehindPlayerPosition = m_pPlayer->GetCenter() + Player::PLAYER_CENTER_OFFSET;
	m_cameraBehindPlayerPosition += m_pPlayer->GetForwardVector() * -(m_cameraDistance * ratios.x);
	m_cameraBehindPlayerPosition += m_pPlayer->GetUpVector() * (m_cameraDistance * ratios.y);

	// Only set the position, since the player will be controlling the rotation of the camera
	m_pGameCamera->SetFakePosition(m_cameraBehindPlayerPosition);
}

void CubbyGame::UpdateCameraAutoCamera(float dt, bool updateCameraPosition)
{
	if (updateCameraPosition)
	{
		float movementModifierNormal = 10.0f;
		float movementModifierMoving = 0.125f;
		float movementModifierChangeAmount = 2.0f;
		if (m_keyboardMovement || m_gamepadMovement)
		{
			m_autoCameraMovingModifier = movementModifierMoving;
		}
		else
		{
			if (m_autoCameraMovingModifier < movementModifierNormal)
			{
				m_autoCameraMovingModifier += movementModifierChangeAmount * dt;
			}
			else
			{
				m_autoCameraMovingModifier = movementModifierNormal;
			}
		}

		glm::vec3 ratios = normalize(glm::vec3(2.5f, 1.0f, 0.0f));
		float catchupSpeed = 1.0f * (1.0f - (m_cameraDistance / 20.0f)) * m_autoCameraMovingModifier;

		m_cameraBehindPlayerPosition = m_pPlayer->GetCenter() + Player::PLAYER_CENTER_OFFSET;
		m_cameraBehindPlayerPosition += m_pPlayer->GetForwardVector() * -(m_cameraDistance * ratios.x);
		m_cameraBehindPlayerPosition += m_pPlayer->GetUpVector() * (m_cameraDistance * ratios.y);

		m_cameraPositionAutoModeCached = m_cameraBehindPlayerPosition;

		// Update the target vectors based on the cached and calculated values
		{
			glm::vec3 newPos = m_cameraPositionAutoModeCached;
			glm::vec3 toPos = newPos - m_cameraPositionAutoMode;
			m_cameraPositionAutoMode += toPos * (catchupSpeed * 2.0f) * dt;
		}

		// Position
		glm::vec3 cameraLookAt = m_pPlayer->GetCenter() + Player::PLAYER_CENTER_OFFSET;
		glm::vec3 targetFacing = normalize(cameraLookAt - m_cameraPositionAutoMode);
		
		targetFacing.y = 0.0f;
		targetFacing = normalize(targetFacing);
		
		glm::vec3 camFacing = m_pGameCamera->GetFacing();
		
		camFacing.y = 0.0f;
		camFacing = normalize(camFacing);
		
		glm::vec3 crossResult = cross(targetFacing, camFacing);
		float dotResult = dot(targetFacing, camFacing);
		float rotationDegrees = RadianToDegree(acos(dotResult));
		
		if (rotationDegrees > 1.0f)
		{
			if (crossResult.y > 0.0f)
			{
				rotationDegrees = -rotationDegrees;
			}
			
			float changeAmount = rotationDegrees * 1.0f * m_autoCameraMovingModifier;
			
			m_pGameCamera->RotateAroundPointY(changeAmount * dt, true);
		}
	}

	// Forward
	glm::vec3 cameraLookAt = m_pPlayer->GetCenter() + Player::PLAYER_CENTER_OFFSET;
	glm::vec3 cameraForward = normalize(cameraLookAt - m_pGameCamera->GetFakePosition());
	m_pGameCamera->SetFacing(cameraForward);

	// Right
	glm::vec3 cameraRight = normalize(cross(cameraForward, m_pPlayer->GetUpVector()));
	m_pGameCamera->SetRight(cameraRight);

	// Up
	glm::vec3 cameraUp = normalize(cross(cameraRight, cameraForward));
	m_pGameCamera->SetUp(cameraUp);
}

void CubbyGame::UpdateCameraFirstPerson(float dt) const
{
	m_pGameCamera->SetFakePosition(m_pPlayer->GetCenter() + Player::PLAYER_CENTER_OFFSET);
	m_pPlayer->SetForwardVector(m_pGameCamera->GetFacing());
}

void CubbyGame::UpdateCameraNPCDialog(float dt) const
{
	// Position
	glm::vec3 diff = m_targetCameraPositionNPCDialog - m_pGameCamera->GetPosition();
	m_pGameCamera->SetFakePosition(m_pGameCamera->GetPosition() + ((diff * 5.0f) * dt));

	// Forward
	diff = m_targetCameraViewNPCDialog - m_pGameCamera->GetView();
	glm::vec3 cameraLookAt = m_pGameCamera->GetView() + ((diff * 5.0f) * dt);
	glm::vec3 cameraForward = normalize(cameraLookAt - m_pGameCamera->GetFakePosition());
	m_pGameCamera->SetFacing(cameraForward);

	// Right
	glm::vec3 cameraRight = normalize(cross(cameraForward, m_pPlayer->GetUpVector()));
	m_pGameCamera->SetRight(cameraRight);

	// Up
	glm::vec3 cameraUp = normalize(cross(cameraRight, cameraForward));
	m_pGameCamera->SetUp(cameraUp);
}

void CubbyGame::UpdateCameraEnemyTarget(float dt)
{
	// Target camera position
	glm::vec3 targetCameraPosition = m_pPlayer->GetCenter();
	targetCameraPosition += m_pPlayer->GetRightVector() * (-2.0f * m_targetCameraXAxisAmount);
	targetCameraPosition += m_pPlayer->GetForwardVector() * (-1.0f * m_targetCameraForwardRatio);
	targetCameraPosition += m_pPlayer->GetUpVector() * (0.75f * m_targetCameraYRatio);

	// Target camera view
	glm::vec3 toTarget = m_pPlayer->GetTargetEnemy()->GetProjectileHitboxCenter() - m_pPlayer->GetCenter();
	glm::vec3 TargetCameraView = m_pPlayer->GetCenter() + toTarget * 0.65f;

	float toTargetDistance = length(toTarget);
	float ratio = (toTargetDistance - 5.0f) / 15.0f;
	if (ratio < 0.0f)
	{
		ratio = 0.0f;
	}
	if (ratio > 1.0f)
	{
		ratio = 1.0f;
	}

	float upVectorRatioTarget;
	if (toTarget.y > 2.0f && toTarget.y > toTargetDistance * 0.25f)
	{
		upVectorRatioTarget = -0.1f * ratio;
	}
	else
	{
		upVectorRatioTarget = 2.0f + (1.5f * (1.0f - ratio));
	}

	m_targetCameraYRatio = upVectorRatioTarget;

	// Lag on the y Ratio updating
	float yRatioDiff = fabs(upVectorRatioTarget - m_targetCameraYRatio);
	if (m_targetCameraYRatio > upVectorRatioTarget)
	{
		m_targetCameraYRatio -= (yRatioDiff * 3.0f) * dt;
	}
	else if (m_targetCameraYRatio < upVectorRatioTarget)
	{
		m_targetCameraYRatio += (yRatioDiff * 3.0f) * dt;
	}

	// Lag the forward vector ratio
	float forwardVectorRatioTarget = (2.0f + (1.5f * (1.0f - ratio))) * 0.75f;
	float forwardRatioDiff = fabs(forwardVectorRatioTarget - m_targetCameraForwardRatio);
	if (m_targetCameraForwardRatio > forwardVectorRatioTarget)
	{
		m_targetCameraForwardRatio -= (forwardRatioDiff * 3.0f) * dt;
	}
	else if (m_targetCameraForwardRatio < forwardVectorRatioTarget)
	{
		m_targetCameraForwardRatio += (forwardRatioDiff * 3.0f) * dt;
	}

	// Lag on the X amount
	float XAxisDiff = fabs(m_targetCameraXAxisAmountTarget - m_targetCameraXAxisAmount);
	if (m_targetCameraXAxisAmount > m_targetCameraXAxisAmountTarget)
	{
		m_targetCameraXAxisAmount -= (XAxisDiff * 3.0f) * dt;
	}
	else if (m_targetCameraXAxisAmount < m_targetCameraXAxisAmountTarget)
	{
		m_targetCameraXAxisAmount += (XAxisDiff * 3.0f) * dt;
	}
	
	glm::vec3 targetPosition = targetCameraPosition;
	glm::vec3 targetView = TargetCameraView;

	glm::vec3 diffPos = targetPosition - m_pGameCamera->GetPosition();
	glm::vec3 diffView = targetView - m_pGameCamera->GetView();

	// Position
	m_pGameCamera->SetFakePosition(m_pGameCamera->GetPosition() + ((diffPos * 3.0f) * dt));

	// Forward
	glm::vec3 cameraLookAt = m_pGameCamera->GetView() + ((diffView * 3.0f) * dt);
	glm::vec3 cameraForward = normalize(cameraLookAt - m_pGameCamera->GetFakePosition());
	m_pGameCamera->SetFacing(cameraForward);

	// Right
	glm::vec3 cameraRight = normalize(cross(cameraForward, m_pPlayer->GetUpVector()));
	m_pGameCamera->SetRight(cameraRight);

	// Up
	glm::vec3 cameraUp = normalize(cross(cameraRight, cameraForward));
	m_pGameCamera->SetUp(cameraUp);
}

void CubbyGame::UpdateCameraClipping(float dt)
{
	glm::vec3 cameraPosition = m_targetCameraPositionBeforeClipping;

	if (m_gameMode == GameMode::Game && m_cameraMode != CameraMode::Debug)
	{
		int numIterations = 0;
		int maxNumIterations = 100;
		glm::vec3 basePos;
		glm::vec3 testPos;
		bool collides = true;
		int blockX, blockY, blockZ;
		glm::vec3 blockPos;
		bool active;
		glm::vec3 toPlayer = ((m_pPlayer->GetCenter() + Player::PLAYER_CENTER_OFFSET) - m_pGameCamera->GetFakePosition());
		float distance = length(toPlayer);
		float incrementAmount = distance / maxNumIterations;
		
		while (collides == true && numIterations < maxNumIterations)
		{
			collides = false;
			basePos = cameraPosition;

			glm::vec3 playerRight = m_pPlayer->GetRightVector() * 0.25f;
			testPos = basePos + playerRight;
			Chunk* pChunk = nullptr;
			active = m_pChunkManager->GetBlockActiveFrom3DPosition(testPos.x, testPos.y, testPos.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);
			if (active)
			{
				collides = true;
			}

			playerRight = m_pPlayer->GetRightVector() * -0.25f;
			testPos = basePos + playerRight;
			pChunk = nullptr;
			active = m_pChunkManager->GetBlockActiveFrom3DPosition(testPos.x, testPos.y, testPos.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);
			if (active)
			{
				collides = true;
			}

			glm::vec3 playerUp = m_pPlayer->GetUpVector() * -0.25f;
			testPos = basePos + playerUp;
			pChunk = nullptr;
			active = m_pChunkManager->GetBlockActiveFrom3DPosition(testPos.x, testPos.y, testPos.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);
			if (active)
			{
				collides = true;
			}

			playerUp = m_pPlayer->GetUpVector() * 0.25f;
			testPos = basePos + playerUp;
			pChunk = nullptr;
			active = m_pChunkManager->GetBlockActiveFrom3DPosition(testPos.x, testPos.y, testPos.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);
			if (active)
			{
				collides = true;
			}

			if (collides == true)
			{
				cameraPosition += m_pGameCamera->GetFacing() * incrementAmount;
			}

			numIterations++;
		}
	}

	m_cameraPositionAfterClipping = cameraPosition;
	m_pGameCamera->SetPosition(m_cameraPositionAfterClipping);
}

void CubbyGame::UpdateCameraZoom(float dt)
{
	// Make sure we gradually move inwards/outwards
	if (m_cameraMode != CameraMode::FirstPerson)
	{
		float camDiff = fabs(m_cameraDistance - m_maxCameraDistance);
		float changeAmount = 0.0f;
		if (m_cameraDistance < m_maxCameraDistance)
		{
			changeAmount = camDiff * dt;
		}
		else if (m_cameraDistance >= m_maxCameraDistance)
		{
			changeAmount = -camDiff * dt;
		}

		m_cameraDistance += changeAmount;
		m_pGameCamera->Zoom(changeAmount, true);
	}
}

bool CubbyGame::ShouldRestorePreviousCameraMode() const
{
	return m_shouldRestorePreviousCameraMode;
}

void CubbyGame::SavePreviousCameraMode()
{
	m_previousCameraMode = m_cameraMode;
}

void CubbyGame::RestorePreviousCameraMode()
{
	SetCameraMode(m_previousCameraMode);
	m_shouldRestorePreviousCameraMode = false;
}