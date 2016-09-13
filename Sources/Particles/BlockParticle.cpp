/*************************************************************************
> File Name: BlockParticle.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Block particle.
> Created Time: 2016/07/11
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Blocks/Chunk.h>
#include <Blocks/ChunkManager.h>
#include <Maths/3DMaths.h>
#include <Utils/Random.h>

#include "BlockParticle.h"
#include "BlockParticleEmitter.h"
#include "BlockParticleEffect.h"

// Constructor, Destructor
BlockParticle::BlockParticle() :
	m_isErase(false), m_isAllowFloorSliding(false),
	m_isRandomStartRotation(false), m_gravityMultiplier(0.0f),
	m_currentRed(0.0f), m_startRed(0.0f), m_startRedVariance(0.0f), m_endRed(0.0f), m_endRedVariance(0.0f),
	m_currentGreen(0.0f), m_startGreen(0.0f), m_startGreenVariance(0.0f), m_endGreen(0.0f), m_endGreenVariance(0.0f),
	m_currentBlue(0.0f), m_startBlue(0.0f), m_startBlueVariance(0.0f), m_endBlue(0.0f), m_endBlueVariance(0.0f),
	m_currentAlpha(0.0f), m_startAlpha(0.0f), m_startAlphaVariance(0.0f), m_endAlpha(0.0f), m_endAlphaVariance(0.0f),
	m_currentScale(0.0f), m_startScale(0.0f), m_startScaleVariance(0.0f), m_endScale(0.0f), m_endScaleVariance(0.0f),
	m_velocityTowardsPoint(0.0f), m_accelerationTowardsPoint(0.0f),
	m_tangentialVelocityXY(0.0f), m_tangentialAccelerationXY(0.0f),
	m_tangentialVelocityXZ(0.0f), m_tangentialAccelerationXZ(0.0f),
	m_tangentialVelocityYZ(0.0f), m_tangentialAccelerationYZ(0.0f),
	m_lifeTime(0.0f), m_maxLifeTime(0.0f), m_lifeTimeVariance(0.0f),
	m_freezeUpdateTimer(0.0f), m_waitAfterUpdateCompleteTimer(0.0f),
	m_isCheckWorldCollisions(false), m_isDestoryOnCollision(false),
	m_isStartLifeDecayOnCollision(false), m_hasCollided(false),
	m_gridPositionX(0), m_gridPositionY(0), m_gridPositionZ(0),
	m_pCachedGridChunk(nullptr), m_pParent(nullptr),
	m_isCreateEmitters(false), m_pCreatedEmitter(nullptr),
	m_pChunkManager(nullptr)
{

}

BlockParticle::~BlockParticle()
{
	if (m_isCreateEmitters == true && m_pCreatedEmitter != nullptr)
	{
		m_pCreatedEmitter->m_pParentParticle = nullptr;
		m_pCreatedEmitter->m_isErase = true;
	}
}

void BlockParticle::SetChunkManager(ChunkManager* pChunkManager)
{
	m_pChunkManager = pChunkManager;
}

void BlockParticle::ClearParticleChunkCacheForChunk(Chunk* pChunk)
{
	if (m_pCachedGridChunk == pChunk)
	{
		m_pCachedGridChunk = nullptr;
	}
}

void BlockParticle::CreateStartingParams()
{
	m_startScale = m_startScale + ((GetRandomNumber(-1, 1, 2) * m_startScaleVariance) * m_startScale);
	m_endScale = m_endScale + ((GetRandomNumber(-1, 1, 2) * m_endScaleVariance) * m_endScale);
	m_currentScale = m_startScale;

	m_startRed = m_startRed + (GetRandomNumber(-1, 1, 2) * m_startRedVariance);
	m_endRed = m_endRed + (GetRandomNumber(-1, 1, 2) * m_endRedVariance);
	m_currentRed = m_startRed;

	m_startGreen = m_startGreen + (GetRandomNumber(-1, 1, 2) * m_startGreenVariance);
	m_endGreen = m_endGreen + (GetRandomNumber(-1, 1, 2) * m_endGreenVariance);
	m_currentGreen = m_startGreen;

	m_startBlue = m_startBlue + (GetRandomNumber(-1, 1, 2) * m_startBlueVariance);
	m_endBlue = m_endBlue + (GetRandomNumber(-1, 1, 2) * m_endBlueVariance);
	m_currentBlue = m_startBlue;

	m_startAlpha = m_startAlpha + (GetRandomNumber(-1, 1, 2) * m_startAlphaVariance);
	m_endAlpha = m_endAlpha + (GetRandomNumber(-1, 1, 2) * m_endAlphaVariance);
	m_currentAlpha = m_startAlpha;

	m_lifeTime = m_lifeTime + ((GetRandomNumber(-1, 1, 2) * m_lifeTimeVariance) * m_lifeTime);
	m_maxLifeTime = m_lifeTime;

	m_velocity = m_velocity + glm::vec3(GetRandomNumber(-100, 100, 2) * 0.01f * m_startVelocityVariance.x, GetRandomNumber(-100, 100, 2) * 0.01f * m_startVelocityVariance.y, GetRandomNumber(-100, 100, 2) * 0.01f * m_startVelocityVariance.z);
	m_angularVelocity = m_angularVelocity + glm::vec3(GetRandomNumber(-100, 100, 2) * 0.01f * m_startAngularVelocityVariance.x, GetRandomNumber(-100, 100, 2) * 0.01f * m_startAngularVelocityVariance.y, GetRandomNumber(-100, 100, 2) * 0.01f * m_startAngularVelocityVariance.z);

	if (m_isRandomStartRotation)
	{
		m_rotation = glm::vec3(GetRandomNumber(-360, 360, 2), GetRandomNumber(-360, 360, 2), GetRandomNumber(-360, 360, 2));
	}
	else
	{
		m_rotation = m_startRotation;
	}
}

void BlockParticle::CalculateWorldTransformMatrix()
{
	// Full world positional matrix
	m_worldMatrix.LoadIdentity();
	m_worldMatrix.SetRotation(DegreeToRadian(m_rotation.x), DegreeToRadian(m_rotation.y), DegreeToRadian(m_rotation.z));

	glm::vec3 pos = m_position;

	if (m_pParent != nullptr && m_pParent->m_shouldParticlesFollowEmitter)
	{
		// If we have a parent and we are locked to their position
		pos += m_pParent->m_position;

		if (m_pParent->m_pParent != nullptr)
		{
			// If our emitter's parent effect has a position offset
			pos += m_pParent->m_pParent->m_position;
		}
	}

	m_worldMatrix.SetTranslation(glm::vec3(pos.x, pos.y, pos.z));

	Matrix4 scaleMat;
	scaleMat.SetScale(glm::vec3(m_currentScale, m_currentScale, m_currentScale));

	m_worldMatrix = scaleMat * m_worldMatrix;

	// Non-world matrix that doesn't contain the world positional offset, i.e only local to the particle's emitter and effect
	m_worldMatrixNoPositionOffset.LoadIdentity();
	m_worldMatrixNoPositionOffset.SetRotation(DegreeToRadian(m_rotation.x), DegreeToRadian(m_rotation.y), DegreeToRadian(m_rotation.z));

	pos = m_positionNoWorldOffset;

	if (m_pParent != nullptr && m_pParent->m_shouldParticlesFollowEmitter)
	{
		// If we have a parent and we are locked to their position
		pos += m_pParent->m_position;

		if (m_pParent->m_pParent != nullptr)
		{
			// If our emitter's parent effect has a position offset
			pos += m_pParent->m_pParent->m_positionNoWorldOffset;
		}
	}

	m_worldMatrixNoPositionOffset.SetTranslation(glm::vec3(pos.x, pos.y, pos.z));

	scaleMat.SetScale(glm::vec3(m_currentScale, m_currentScale, m_currentScale));

	m_worldMatrixNoPositionOffset = scaleMat * m_worldMatrixNoPositionOffset;
}

void BlockParticle::UpdateGridPosition()
{
	int gridPositionX = static_cast<int>((m_position.x + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridPositionY = static_cast<int>((m_position.y + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridPositionZ = static_cast<int>((m_position.z + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);

	if (m_position.x <= -0.5f)
	{
		gridPositionX -= 1;
	}
	if (m_position.y <= -0.5f)
	{
		gridPositionY -= 1;
	}
	if (m_position.z <= -0.5f)
	{
		gridPositionZ -= 1;
	}

	if (gridPositionX != m_gridPositionX || gridPositionY != m_gridPositionY || gridPositionZ != m_gridPositionZ || m_pCachedGridChunk == nullptr)
	{
		m_gridPositionX = gridPositionX;
		m_gridPositionY = gridPositionY;
		m_gridPositionZ = gridPositionZ;

		m_pCachedGridChunk = m_pChunkManager->GetChunk(m_gridPositionX, m_gridPositionY, m_gridPositionZ);
	}
}

Chunk* BlockParticle::GetCachedGridChunkOrFromPosition(glm::vec3 pos) const
{
	// First check if the position is in the same grid as the cached chunk
	int gridPositionX = static_cast<int>((pos.x + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridPositionY = static_cast<int>((pos.y + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);
	int gridPositionZ = static_cast<int>((pos.z + Chunk::BLOCK_RENDER_SIZE) / Chunk::CHUNK_SIZE);

	if (pos.x <= -0.5f)
	{
		gridPositionX -= 1;
	}
	if (pos.y <= -0.5f)
	{
		gridPositionY -= 1;
	}
	if (pos.z <= -0.5f)
	{
		gridPositionZ -= 1;
	}

	if (gridPositionX != m_gridPositionX || gridPositionY != m_gridPositionY || gridPositionZ != m_gridPositionZ)
	{
		return nullptr;
	}
	
	return m_pCachedGridChunk;
}

void BlockParticle::SetFreezeDelayTimer(float timer)
{
	m_freezeUpdateTimer = timer;
}

void BlockParticle::SetWaitAfterUpdateCompleteTimer(float timer)
{
	m_waitAfterUpdateCompleteTimer = timer;
}

// Update
void BlockParticle::Update(float dt)
{
	if (m_isErase)
	{
		return;
	}

	if (m_pParent != nullptr && m_pParent->m_isPaused == true)
	{
		// If our parent emitter is paused, return
		return;
	}

	// Update particle life
	if (m_isStartLifeDecayOnCollision == false || m_hasCollided == true)
	{
		m_lifeTime -= dt;

		if (m_lifeTime < 0.0f)
		{
			m_lifeTime = 0.0f;
		}
	}

	// Update grid position and cached chunk pointer
	if (m_isCheckWorldCollisions)
	{
		UpdateGridPosition();

		if (m_pCachedGridChunk == nullptr)
		{
			m_hasCollided = true;

			if (m_isDestoryOnCollision)
			{
				m_isErase = true;
				return;
			}

			return;
		}
	}

	// If we are frozen, return and don't do any physics updating
	if (m_freezeUpdateTimer >= 0.0f)
	{
		m_freezeUpdateTimer -= dt;

		return;
	}

	if (m_lifeTime > 0.0f)
	{
		if (m_pParent != nullptr)
		{
			// Velocity towards point origin
			glm::vec3 pointOrigin = m_pointOrigin;

			if (m_pParent != nullptr && m_pParent->m_shouldParticlesFollowEmitter == false)
			{
				if (m_pParent->m_pParent != nullptr)
				{
					// Add on parent's particle effect position
					pointOrigin += m_pParent->m_pParent->m_position;
				}
			}

			glm::vec3 toPoint = pointOrigin - m_position;

			if (length(toPoint) > 0.001f)
			{
				m_velocityTowardsPoint += m_accelerationTowardsPoint * dt;
				glm::vec3 velocityToPoint = toPoint * m_velocityTowardsPoint;
				m_pointVelocity += velocityToPoint * dt;

				// Tangential velocity
				glm::vec3 xAxis = glm::vec3(m_velocity.y < 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f);
				glm::vec3 crossX = cross(toPoint, xAxis);
				glm::vec3 yAxis = glm::vec3(0.0f, m_velocity.z < 0.0f ? -1.0f : 1.0f, 0.0f);
				glm::vec3 crossY = cross(toPoint, yAxis);
				glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, m_velocity.y < 0.0f ? -1.0f : 1.0f);
				glm::vec3 crossZ = cross(toPoint, zAxis);

				m_tangentialVelocityXY += m_tangentialAccelerationXY * dt;
				m_tangentialVelocityXZ += m_tangentialAccelerationXZ * dt;
				m_tangentialVelocityYZ += m_tangentialAccelerationYZ * dt;
				glm::vec3 velTangentXY = crossZ * m_tangentialVelocityXY;
				glm::vec3 velTangentXZ = crossY * m_tangentialVelocityXZ;
				glm::vec3 velTangentYZ = crossX * m_tangentialVelocityYZ;

				m_tangentialVelocity = velTangentXY + velTangentXZ + velTangentYZ;
			}
		}

		// Position integration
		glm::vec3 acceleration = (m_gravityDirection * 9.81f) * m_gravityMultiplier;
		m_velocity += acceleration * dt;
		m_position += m_velocity * dt;
		m_positionNoWorldOffset += m_velocity * dt;

		// Point origin and tangential velocity
		m_position += m_tangentialVelocity * dt;
		m_position += m_pointVelocity * dt;
		m_positionNoWorldOffset += m_tangentialVelocity * dt;
		m_positionNoWorldOffset += m_pointVelocity * dt;

		// Rotation integration
		glm::vec3 angularAcceleration(0.0f, 0.0f, 0.0f);
		m_angularVelocity += angularAcceleration * dt;
		m_rotation += m_angularVelocity * dt;

		if (m_isCheckWorldCollisions)
		{
			int blockX, blockY, blockZ;
			glm::vec3 blockPos;
			glm::vec3 particlePos = m_position;

			if (m_pParent != nullptr && m_pParent->m_shouldParticlesFollowEmitter)
			{
				particlePos += m_pParent->m_position;
			}

			Chunk* pChunk = GetCachedGridChunkOrFromPosition(particlePos);
			bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(particlePos.x, particlePos.y, particlePos.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);

			if (active == true)
			{
				m_hasCollided = true;

				if (m_isDestoryOnCollision)
				{
					m_isErase = true;
					return;
				}

				if (m_isAllowFloorSliding)
				{
					// Roll back the integration, since we will intersect the block otherwise
					m_position -= glm::vec3(0.0f, m_velocity.y, 0.0f) * dt;
					m_position -= m_tangentialVelocity * dt;
					m_position -= m_pointVelocity * dt;
					m_velocity -= acceleration * dt;

					// Apply some damping to the rotation and velocity
					m_angularVelocity *= 0.96f;
					m_velocity *= 0.96f;

					if (m_velocity.y <= 0.05f)
					{
						m_velocity = glm::vec3(m_velocity.x, 0.0f, m_velocity.z);
					}
				}
				else
				{
					// Roll back the integration, since we will intersect the block otherwise
					m_position -= m_velocity * dt;
					m_position -= m_tangentialVelocity * dt;
					m_position -= m_pointVelocity * dt;

					// Apply some damping to the rotation and velocity
					m_angularVelocity *= 0.96f;
					m_velocity *= 0.96f;

					if (m_velocity.y <= 0.05f)
					{
						m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
					}
				}
			}
		}
	}
	else
	{
		if (m_waitAfterUpdateCompleteTimer > 0.0f)
		{
			m_waitAfterUpdateCompleteTimer -= dt;
		}
	}

	float timeRatio = (m_lifeTime + m_freezeUpdateTimer) / (m_maxLifeTime + m_freezeUpdateTimer);

	// Update color
	m_currentRed = m_startRed + ((m_endRed - m_startRed) * (1.0f - timeRatio));
	m_currentGreen = m_startGreen + ((m_endGreen - m_startGreen) * (1.0f - timeRatio));
	m_currentBlue = m_startBlue + ((m_endBlue - m_startBlue) * (1.0f - timeRatio));
	m_currentAlpha = m_startAlpha + ((m_endAlpha - m_startAlpha) * (1.0f - timeRatio));

	// Update scale
	m_currentScale = m_startScale + ((m_endScale - m_startScale) * (1.0f - timeRatio));

	if (m_isCreateEmitters == true && m_pCreatedEmitter != nullptr)
	{
		m_pCreatedEmitter->m_pParentParticle = this;
		m_pCreatedEmitter->m_position = m_position;
	}
}
