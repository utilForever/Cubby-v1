/*************************************************************************
> File Name: BlockParticleEmitter.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Block particle emitter.
> Created Time: 2016/07/11
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BLOCK_PARTICLE_EMITTER_H
#define CUBBY_BLOCK_PARTICLE_EMITTER_H

#include <Renderer/Renderer.h>

// Forward declaration
class BlockParticleManager;
class BlockParticleEffect;
class BlockParticle;

enum class EmitterType
{
	Point = 0,
	Square,
	Cube,
	Circle,
	Sphere,
	Mesh,
};

class BlockParticleEmitter
{
public:
	// Constructor, Destructor
	BlockParticleEmitter(Renderer* pRenderer, BlockParticleManager* pBlockParticleManager);
	~BlockParticleEmitter();

	// Getter
	bool IsErase() const;
	void SetErase(bool isErase);
	void SetParent(BlockParticleEffect* parent);

	void CopyParams(BlockParticleEmitter* EmitterParams);

	// Update
	void Update(float dt);

	// Rendering
	void Render() const;

	bool m_isErase;

	// Name
	std::string m_emitterName;

	// Emitter position
	glm::vec3 m_startingPosition;
	glm::vec3 m_position;

	// Creation Time
	float m_creationTime;
	float m_creationTimer;

	// Number of particles to spawn
	int m_numParticlesToSpawn;

	// Should particles follow emitter movement
	bool m_shouldParticlesFollowEmitter;

	// Emitter orbiting
	bool m_isOrbit;
	bool m_isOrbitReverse;
	float m_orbitRadius;
	float m_orbitTime;
	float m_orbitTimer;

	// Playback controls
	bool m_isStarted;
	bool m_isPaused;

	// Emitter type
	EmitterType m_emitterType;

	// Emitter radius and lengths
	float m_emitterRadius;
	float m_emitterLengthX;
	float m_emitterLengthY;
	float m_emitterLengthZ;

	// Are we spawning on the outside of the shape
	bool m_isSpawnOutline;

	// Particle creation params
	glm::vec3 m_gravityDirection;
	float m_gravityMultiplier;
	glm::vec3 m_pointOrigin;
	float m_startScale;
	float m_startScaleVariance;
	float m_endScale;
	float m_endScaleVariance;
	float m_startRed;
	float m_startRedVariance;
	float m_endRed;
	float m_endRedVariance;
	float m_startGreen;
	float m_startGreenVariance;
	float m_endGreen;
	float m_endGreenVariance;
	float m_startBlue;
	float m_startBlueVariance;
	float m_endBlue;
	float m_endBlueVariance;
	float m_startAlpha;
	float m_startAlphaVariance;
	float m_endAlpha;
	float m_endAlphaVariance;
	float m_lifeTime;
	float m_lifeTimeVariance;
	float m_velocityTowardsPoint;
	float m_accelerationTowardsPoint;
	bool m_isCheckWorldCollisions;
	bool m_isDestoryOnCollision;
	bool m_isStartLifeDecayOnCollision;
	glm::vec3 m_startVelocity;
	glm::vec3 m_startVelocityVariance;
	bool m_isRandomStartRotation;
	glm::vec3 m_startRotation;
	glm::vec3 m_startAngularVelocity;
	glm::vec3 m_startAngularVelocityVariance;
	float m_tangentialVelocityXY;
	float m_tangentialAccelerationXY;
	float m_tangentialVelocityXZ;
	float m_tangentialAccelerationXZ;
	float m_tangentialVelocityYZ;
	float m_tangentialAccelerationYZ;
	bool m_isCreateEmitters;
	std::string m_createEmitterName;
	bool m_isReferenceEmitter;

	// Parent effect
	BlockParticleEffect* m_pParent;

	// Parent particle
	BlockParticle* m_pParentParticle;

private:
	Renderer* m_pRenderer;
	BlockParticleManager* m_pBlockParticleManager;
};

#endif