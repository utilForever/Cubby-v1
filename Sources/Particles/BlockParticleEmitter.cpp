/*************************************************************************
> File Name: BlockParticleEmitter.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Block particle emitter.
> Created Time: 2016/07/11
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Maths/3DMaths.h>

#include "BlockParticleEffect.h"
#include "BlockParticleEmitter.h"
#include "BlockParticleManager.h"

// Constructor, Destructor
BlockParticleEmitter::BlockParticleEmitter(Renderer* pRenderer, BlockParticleManager* pBlockParticleManager) :
	m_pRenderer(pRenderer), m_pBlockParticleManager(pBlockParticleManager),
	m_isErase(false), m_emitterName(""), m_creationTime(0.0f), m_creationTimer(0.0f),
	m_numParticlesToSpawn(0), m_shouldParticlesFollowEmitter(false),
	m_isOrbit(false), m_isOrbitReverse(false), m_orbitRadius(0.0f), m_orbitTime(0.0f), m_orbitTimer(0.0f),
	m_isStarted(false), m_isPaused(false), m_emitterType(EmitterType::Point),
	m_emitterRadius(0.0f), m_emitterLengthX(0.0f), m_emitterLengthY(0.0f), m_emitterLengthZ(0.0f),
	m_isSpawnOutline(false), m_gravityMultiplier(0.0f),
	m_startScale(0.0f), m_startScaleVariance(0.0f), m_endScale(0.0f), m_endScaleVariance(0.0f),
	m_startRed(0.0f), m_startRedVariance(0.0f), m_endRed(0.0f), m_endRedVariance(0.0f),
	m_startGreen(0.0f), m_startGreenVariance(0.0f), m_endGreen(0.0f), m_endGreenVariance(0.0f),
	m_startBlue(0.0f), m_startBlueVariance(0.0f), m_endBlue(0.0f), m_endBlueVariance(0.0f),
	m_startAlpha(0.0f), m_startAlphaVariance(0.0f), m_endAlpha(0.0f), m_endAlphaVariance(0.0f),
	m_lifeTime(0.0f), m_lifeTimeVariance(0.0f),
	m_velocityTowardsPoint(0.0f), m_accelerationTowardsPoint(0.0f),
	m_isCheckWorldCollisions(false), m_isDestoryOnCollision(false), m_isStartLifeDecayOnCollision(false), m_isRandomStartRotation(false),
	m_tangentialVelocityXY(0.0f), m_tangentialAccelerationXY(0.0f),
	m_tangentialVelocityXZ(0.0f), m_tangentialAccelerationXZ(0.0f),
	m_tangentialVelocityYZ(0.0f), m_tangentialAccelerationYZ(0.0f),
	m_isCreateEmitters(false), m_createEmitterName(""), m_isReferenceEmitter(false),
	m_pParent(nullptr), m_pParentParticle(nullptr)
{

}

BlockParticleEmitter::~BlockParticleEmitter()
{
	if (m_pParent)
	{
		m_pParent->RemoveEmitterFromParticleEffect(this);
	}

	m_pBlockParticleManager->RemoveEmitterLinkage(this);
}

// Getter
bool BlockParticleEmitter::IsErase() const
{
	return m_isErase;
}

void BlockParticleEmitter::SetErase(bool isErase)
{
	m_isErase = isErase;
}

void BlockParticleEmitter::SetParent(BlockParticleEffect* parent)
{
	m_pParent = parent;
}

void BlockParticleEmitter::CopyParams(BlockParticleEmitter* EmitterParams)
{
	m_startingPosition = EmitterParams->m_startingPosition;
	m_position = EmitterParams->m_position;

	// Creation Time
	m_creationTime = EmitterParams->m_creationTime;
	m_creationTimer = EmitterParams->m_creationTimer;

	// Number of particles to spawn
	m_numParticlesToSpawn = EmitterParams->m_numParticlesToSpawn;

	// Should particles follow emitter movement
	m_shouldParticlesFollowEmitter = EmitterParams->m_shouldParticlesFollowEmitter;

	// Emitter orbiting
	m_isOrbit = EmitterParams->m_isOrbit;
	m_isOrbitReverse = EmitterParams->m_isOrbitReverse;
	m_orbitRadius = EmitterParams->m_orbitRadius;
	m_orbitTime = EmitterParams->m_orbitTime;
	m_orbitTimer = EmitterParams->m_orbitTimer;

	// Playback controls
	m_isStarted = EmitterParams->m_isStarted;
	m_isPaused = EmitterParams->m_isPaused;

	// Emitter type
	m_emitterType = EmitterParams->m_emitterType;

	// Emitter radius and lengths
	m_emitterRadius = EmitterParams->m_emitterRadius;
	m_emitterLengthX = EmitterParams->m_emitterLengthX;
	m_emitterLengthY = EmitterParams->m_emitterLengthY;
	m_emitterLengthZ = EmitterParams->m_emitterLengthZ;

	// Are we spawning on the outside of the shape
	m_isSpawnOutline = EmitterParams->m_isSpawnOutline;

	// Particle creation params
	m_gravityDirection = EmitterParams->m_gravityDirection;
	m_gravityMultiplier = EmitterParams->m_gravityMultiplier;
	m_pointOrigin = EmitterParams->m_pointOrigin;
	m_startScale = EmitterParams->m_startScale;
	m_startScaleVariance = EmitterParams->m_startScaleVariance;
	m_endScale = EmitterParams->m_endScale;
	m_endScaleVariance = EmitterParams->m_endScaleVariance;
	m_startRed = EmitterParams->m_startRed;
	m_startRedVariance = EmitterParams->m_startRedVariance;
	m_endRed = EmitterParams->m_endRed;
	m_endRedVariance = EmitterParams->m_endRedVariance;
	m_startGreen = EmitterParams->m_startGreen;
	m_startGreenVariance = EmitterParams->m_startGreenVariance;
	m_endGreen = EmitterParams->m_endGreen;
	m_endGreenVariance = EmitterParams->m_endGreenVariance;
	m_startBlue = EmitterParams->m_startBlue;
	m_startBlueVariance = EmitterParams->m_startBlueVariance;
	m_endBlue = EmitterParams->m_endBlue;
	m_endBlueVariance = EmitterParams->m_endBlueVariance;
	m_startAlpha = EmitterParams->m_startAlpha;
	m_startAlphaVariance = EmitterParams->m_startAlphaVariance;
	m_endAlpha = EmitterParams->m_endAlpha;
	m_endAlphaVariance = EmitterParams->m_endAlphaVariance;
	m_lifeTime = EmitterParams->m_lifeTime;
	m_lifeTimeVariance = EmitterParams->m_lifeTimeVariance;
	m_velocityTowardsPoint = EmitterParams->m_velocityTowardsPoint;
	m_accelerationTowardsPoint = EmitterParams->m_accelerationTowardsPoint;
	m_isCheckWorldCollisions = EmitterParams->m_isCheckWorldCollisions;
	m_isDestoryOnCollision = EmitterParams->m_isDestoryOnCollision;
	m_isStartLifeDecayOnCollision = EmitterParams->m_isStartLifeDecayOnCollision;
	m_startVelocity = EmitterParams->m_startVelocity;
	m_startVelocityVariance = EmitterParams->m_startVelocityVariance;
	m_isRandomStartRotation = EmitterParams->m_isRandomStartRotation;
	m_startRotation = EmitterParams->m_startRotation;
	m_startAngularVelocity = EmitterParams->m_startAngularVelocity;
	m_startAngularVelocityVariance = EmitterParams->m_startAngularVelocityVariance;
	m_tangentialVelocityXY = EmitterParams->m_tangentialVelocityXY;
	m_tangentialAccelerationXY = EmitterParams->m_tangentialAccelerationXY;
	m_tangentialVelocityXZ = EmitterParams->m_tangentialVelocityXZ;
	m_tangentialAccelerationXZ = EmitterParams->m_tangentialAccelerationXZ;
	m_tangentialVelocityYZ = EmitterParams->m_tangentialVelocityYZ;
	m_tangentialAccelerationYZ = EmitterParams->m_tangentialAccelerationYZ;
	m_isCreateEmitters = EmitterParams->m_isCreateEmitters;
	m_createEmitterName = EmitterParams->m_createEmitterName;
	m_isReferenceEmitter = false;

	// Parent effect
	m_pParent = EmitterParams->m_pParent;

	if (m_pParent)
	{
		m_pParent->AddEmitterToParticleEffect(this);
	}
}

// Update
void BlockParticleEmitter::Update(float dt)
{
	if (m_isReferenceEmitter)
	{
		// If we are a reference emitter, don't do anything
		return;
	}

	if (m_isPaused)
	{
		// If we are paused, return
		return;
	}

	if (m_isErase)
	{
		return;
	}

	if (m_creationTimer <= 0.0f)
	{
		m_creationTimer = m_creationTime;

		// Only create particles if we have been started
		if (m_isStarted)
		{
			for (int i = 0; i < m_numParticlesToSpawn; ++i)
			{
				m_pBlockParticleManager->CreateBlockParticleFromEmitterParams(this);
			}
		}
	}
	else
	{
		m_creationTimer -= dt;
	}

	if (m_isOrbit)
	{
		m_orbitTimer += dt;

		if (m_orbitTimer >= m_orbitTime)
		{
			m_orbitTimer = 0.0f;
		}

		float ratio = m_orbitTimer / m_orbitTime;
		float rotationAngle = DegreeToRadian(ratio * 360.0f);

		if (m_isOrbitReverse)
		{
			rotationAngle = -rotationAngle;
		}

		m_position = m_startingPosition + glm::vec3(cos(rotationAngle) * m_orbitRadius, 0.0f, sin(rotationAngle) * m_orbitRadius);
	}
}

// Rendering
void BlockParticleEmitter::Render() const
{
	if (m_isReferenceEmitter)
	{
		// If we are a reference emitter, don't do anything
		return;
	}

	if (m_isErase)
	{
		return;
	}

	// Emitter
	m_pRenderer->PushMatrix();

	if (m_pParent != nullptr && m_pParentParticle == nullptr)
	{
		m_pRenderer->TranslateWorldMatrix(m_pParent->m_position.x, m_pParent->m_position.y, m_pParent->m_position.z);
	}

	m_pRenderer->TranslateWorldMatrix(m_position.x, m_position.y, m_position.z);

	float length = m_emitterLengthX;
	float height = m_emitterLengthY;
	float width = m_emitterLengthZ;

	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->SetCullMode(CullMode::NOCULL);
	m_pRenderer->SetLineWidth(1.0f);

	if (m_emitterType == EmitterType::Point)
	{
		float pointLength = 0.5f;
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateVertex(-pointLength, 0.0f, 0.0f);
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
		m_pRenderer->ImmediateVertex(pointLength, 0.0f, 0.0f);
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
		m_pRenderer->ImmediateVertex(0.0f, -pointLength, 0.0f);
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
		m_pRenderer->ImmediateVertex(0.0f, pointLength, 0.0f);
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
		m_pRenderer->ImmediateVertex(0.0f, 0.0f, -pointLength);
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
		m_pRenderer->ImmediateVertex(0.0f, 0.0f, pointLength);
		
		m_pRenderer->DisableImmediateMode();
	}
	else if (m_emitterType == EmitterType::Square)
	{
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateNormal(0.0f, -1.0f, 0.0f);
		m_pRenderer->ImmediateVertex(-length, 0.0f, -width);
		m_pRenderer->ImmediateVertex(length, 0.0f, -width);
		m_pRenderer->ImmediateVertex(length, 0.0f, width);
		m_pRenderer->ImmediateVertex(-length, 0.0f, width);
		
		m_pRenderer->DisableImmediateMode();
	}
	else if (m_emitterType == EmitterType::Cube)
	{
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateNormal(0.0f, 0.0f, -1.0f);
		m_pRenderer->ImmediateVertex(length, -height, -width);
		m_pRenderer->ImmediateVertex(-length, -height, -width);
		m_pRenderer->ImmediateVertex(-length, height, -width);
		m_pRenderer->ImmediateVertex(length, height, -width);

		m_pRenderer->ImmediateNormal(0.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateVertex(-length, -height, width);
		m_pRenderer->ImmediateVertex(length, -height, width);
		m_pRenderer->ImmediateVertex(length, height, width);
		m_pRenderer->ImmediateVertex(-length, height, width);

		m_pRenderer->ImmediateNormal(1.0f, 0.0f, 0.0f);
		m_pRenderer->ImmediateVertex(length, -height, width);
		m_pRenderer->ImmediateVertex(length, -height, -width);
		m_pRenderer->ImmediateVertex(length, height, -width);
		m_pRenderer->ImmediateVertex(length, height, width);

		m_pRenderer->ImmediateNormal(-1.0f, 0.0f, 0.0f);
		m_pRenderer->ImmediateVertex(-length, -height, -width);
		m_pRenderer->ImmediateVertex(-length, -height, width);
		m_pRenderer->ImmediateVertex(-length, height, width);
		m_pRenderer->ImmediateVertex(-length, height, -width);

		m_pRenderer->ImmediateNormal(0.0f, -1.0f, 0.0f);
		m_pRenderer->ImmediateVertex(-length, -height, -width);
		m_pRenderer->ImmediateVertex(length, -height, -width);
		m_pRenderer->ImmediateVertex(length, -height, width);
		m_pRenderer->ImmediateVertex(-length, -height, width);

		m_pRenderer->ImmediateNormal(0.0f, 1.0f, 0.0f);
		m_pRenderer->ImmediateVertex(length, height, -width);
		m_pRenderer->ImmediateVertex(-length, height, -width);
		m_pRenderer->ImmediateVertex(-length, height, width);
		m_pRenderer->ImmediateVertex(length, height, width);
		
		m_pRenderer->DisableImmediateMode();
	}
	else if (m_emitterType == EmitterType::Circle)
	{
		float radius = m_emitterRadius;

		m_pRenderer->PushMatrix();

		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
		m_pRenderer->DrawLineCircle(radius, 30);

		m_pRenderer->PopMatrix();
	}
	else if (m_emitterType == EmitterType::Sphere)
	{
		float radius = m_emitterRadius;

		m_pRenderer->PushMatrix();

		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
		m_pRenderer->DrawSphere(radius, 30, 30);

		m_pRenderer->PopMatrix();
	}
	else if (m_emitterType == EmitterType::Mesh)
	{
		// Do nothing
	}

	m_pRenderer->SetCullMode(CullMode::BACK);

	m_pRenderer->PopMatrix();

	// Origin Point
	m_pRenderer->PushMatrix();

	if (m_pParent != nullptr && m_pParentParticle == nullptr)
	{
		m_pRenderer->TranslateWorldMatrix(m_pParent->m_position.x, m_pParent->m_position.y, m_pParent->m_position.z);
	}

	if (m_pParentParticle != nullptr)
	{
		m_pRenderer->TranslateWorldMatrix(m_pParentParticle->m_position.x, m_pParentParticle->m_position.y, m_pParentParticle->m_position.z);
	}

	if (m_shouldParticlesFollowEmitter)
	{
		m_pRenderer->TranslateWorldMatrix(m_position.x, m_position.y, m_position.z);
	}

	m_pRenderer->TranslateWorldMatrix(m_pointOrigin.x, m_pointOrigin.y, m_pointOrigin.z);

	length = 0.5f;

	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->SetLineWidth(2.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(-length, 0.0f, 0.0f);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(length, 0.0f, 0.0f);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, -length, 0.0f);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, length, 0.0f);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, -length);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, length);

	m_pRenderer->DisableImmediateMode();

	m_pRenderer->PopMatrix();
}
