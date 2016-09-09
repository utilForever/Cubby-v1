/*************************************************************************
> File Name: BlockParticle.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Block particle.
> Created Time: 2016/07/11
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BLOCK_PARTICLE_H
#define CUBBY_BLOCK_PARTICLE_H

#include <Maths/Matrix4.h>

// Forward declaration
class BlockParticleEmitter;
class Chunk;
class ChunkManager;

class BlockParticle
{
public:
	// Constructor, Destructor
	BlockParticle();
	~BlockParticle();

	void SetChunkManager(ChunkManager* pChunkManager);

	void ClearParticleChunkCacheForChunk(Chunk* pChunk);

	void CreateStartingParams();

	void CalculateWorldTransformMatrix();

	void UpdateGridPosition();
	Chunk* GetCachedGridChunkOrFromPosition(glm::vec3 pos) const;

	void SetFreezeDelayTimer(float timer);
	void SetWaitAfterUpdateCompleteTimer(float timer);

	// Update
	void Update(float dt);

	bool m_isErase;

	// Floor Sliding flag (set manually)
	bool m_isAllowFloorSliding;

	// Position
	glm::vec3 m_position;

	// Position with no world offset
	glm::vec3 m_positionNoWorldOffset;

	// Velocity
	glm::vec3 m_velocity;
	glm::vec3 m_startVelocityVariance;

	// Rotation
	glm::vec3 m_rotation;
	glm::vec3 m_startRotation;
	bool m_isRandomStartRotation;

	// Angular velocity
	glm::vec3 m_angularVelocity;
	glm::vec3 m_startAngularVelocityVariance;

	// Gravity
	glm::vec3 m_gravityDirection;
	float m_gravityMultiplier;

	// World matrix
	Matrix4 m_worldMatrix;

	// World matrix with no positional offset, used for viewport rendering without world movements (i.e. characterGUI)
	Matrix4 m_worldMatrixNoPositionOffset;

	// Particle color
	float m_currentRed;
	float m_startRed;
	float m_startRedVariance;
	float m_endRed;
	float m_endRedVariance;
	float m_currentGreen;
	float m_startGreen;
	float m_startGreenVariance;
	float m_endGreen;
	float m_endGreenVariance;
	float m_currentBlue;
	float m_startBlue;
	float m_startBlueVariance;
	float m_endBlue;
	float m_endBlueVariance;
	float m_currentAlpha;
	float m_startAlpha;
	float m_startAlphaVariance;
	float m_endAlpha;
	float m_endAlphaVariance;

	// Particle scale
	float m_currentScale;
	float m_startScale;
	float m_startScaleVariance;
	float m_endScale;
	float m_endScaleVariance;

	// Velocity towards point origin
	glm::vec3 m_pointOrigin;
	glm::vec3 m_pointVelocity;
	float m_velocityTowardsPoint;
	float m_accelerationTowardsPoint;

	// Tangential velocity
	glm::vec3 m_tangentialVelocity;
	float m_tangentialVelocityXY;
	float m_tangentialAccelerationXY;
	float m_tangentialVelocityXZ;
	float m_tangentialAccelerationXZ;
	float m_tangentialVelocityYZ;
	float m_tangentialAccelerationYZ;

	// Lifetime
	float m_lifeTime;
	float m_maxLifeTime;
	float m_lifeTimeVariance;

	// Freeze update / delay
	float m_freezeUpdateTimer;

	// Wait after complete
	float m_waitAfterUpdateCompleteTimer;

	// Does particle collide with the world
	bool m_isCheckWorldCollisions;

	// Do we destroy this particle instantly on collision with world
	bool m_isDestoryOnCollision;

	// Do we only start the life decay after we collide
	bool m_isStartLifeDecayOnCollision;
	bool m_hasCollided;

	// Grid
	int m_gridPositionX;
	int m_gridPositionY;
	int m_gridPositionZ;

	// Cached chunk pointer
	Chunk* m_pCachedGridChunk;

	// Emitter parent
	BlockParticleEmitter* m_pParent;

	// Create Emitters instead of particles
	bool m_isCreateEmitters;
	BlockParticleEmitter* m_pCreatedEmitter;

	ChunkManager* m_pChunkManager;
};

#endif