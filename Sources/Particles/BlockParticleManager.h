/*************************************************************************
> File Name: BlockParticleManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A manager class for the voxel particle system. This class manages
>    particle effects, particle emitters and the individual particle objects.
> Created Time: 2016/07/12
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BLOCK_PARTICLE_MANAGER_H
#define CUBBY_BLOCK_PARTICLE_MANAGER_H

#include <Models/QubicleBinary.h>
#include <Renderer/Renderer.h>

#include "BlockParticle.h"
#include "BlockParticleEmitter.h"
#include "BlockParticleEffect.h"

using BlockParticlesList = std::vector<BlockParticle*>;
using BlockParticlesEmitterList = std::vector<BlockParticleEmitter*>;
using BlockParticlesEffectList = std::vector<BlockParticleEffect*>;

class BlockParticleManager
{
public:
	// Constructor, Destructor
	BlockParticleManager(Renderer* pRenderer, ChunkManager* pChunkManager);
	~BlockParticleManager();

	// Clearing
	void ClearBlockParticles();
	void ClearBlockParticleEmitters();
	void ClearBlockParticleEffects();

	void RemoveEmitterLinkage(BlockParticleEmitter* pEmitter);

	void ClearParticleChunkCacheForChunk(Chunk* pChunk);

	unsigned int GetInstanceShaderIndex() const;

	void SetupGLBuffers();

	// Accessors
	int GetNumBlockParticleEffects() const;
	int GetNumBlockParticleEmitters() const;
	int GetNumBlockParticles() const;
	int GetNumRenderableParticles(bool noWorldOffset);

	// Creation
	BlockParticle* CreateBlockParticleFromEmitterParams(BlockParticleEmitter* pEmitter);
	BlockParticle* CreateBlockParticle(glm::vec3 pos, glm::vec3 posNoWorldOffset, glm::vec3 gravityDir, float gravityMultiplier, glm::vec3 pointOrigin,
		float startScale, float startScaleVariance, float endScale, float endScaleVariance,
		float startR, float startG, float startB, float startA,
		float startRVariance, float startGVariance, float startBVariance, float startAVariance,
		float endR, float endG, float endB, float endA,
		float endRVariance, float endGVariance, float endBVariance, float endAVariance,
		float lifetime, float lifetimeVariance,
		float velocityTowardPoint, float accelerationTowardsPoint,
		glm::vec3 startVelocity, glm::vec3 startVelocityVariance,
		glm::vec3 startAngularVelocity, glm::vec3 startAngularVelocityVariance,
		float tangentialVelocityXY, float tangentialAccelerationXY, float tangentialVelocityXZ, float tangentialAccelerationXZ, float tangentialVelocityYZ, float tangentialAccelerationYZ,
		bool randomStartRotation, glm::vec3 startRotation, bool worldCollision, bool destoryOnCollision, bool startLifeDecayOnCollision,
		bool createEmitters, BlockParticleEmitter* pCreatedEmitter);
	BlockParticleEmitter* CreateBlockParticleEmitter(std::string name, glm::vec3 pos);

	BlockParticleEffect* ImportParticleEffect(std::string fileName, glm::vec3 pos, unsigned int* particleEffectID);
	void DestroyParticleEffect(unsigned int particleEffectID);
	void UpdateParticleEffectPosition(unsigned int particleEffectID, glm::vec3 position, glm::vec3 positionNoWorldOffset);
	void SetRenderNoWoldOffsetViewport(unsigned int particleEffectID, bool renderNoWoldOffsetViewport);

	void ExplodeQubicleBinary(QubicleBinary* pQubicleBinary, float scale, int particleSpawnChance);
	void ExplodeQubicleMatrix(QubicleMatrix* pMatrix, float scale, int particleSpawnChance, bool allSameColor, float sameR, float sameG, float sameB, float sameA);

	// Rendering modes
	void SetWireFrameRender(bool wireframe);
	void SetInstancedRendering(bool instance);

	// Update
	void Update(float dt);

	// Rendering
	void Render(bool noWorldOffset);
	void RenderInstanced(bool noWorldOffset);
	void RenderDefault(bool noWorldOffset);
	void RenderBlockParticle(BlockParticle* pBlockParticle, bool noWorldOffset) const;
	void RenderDebug();
	void RenderEmitters();
	void RenderEffects();

private:
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;

	// Particle effect counter
	int m_particleEffectCounter;

	// Render modes
	bool m_renderWireFrame;
	bool m_instanceRendering;

	// Array buffers
	GLuint m_vertexArray;
	GLuint m_positionBuffer;
	GLuint m_normalBuffer;
	GLuint m_colorBuffer;
	GLuint m_matrixBuffer;

	// Shader
	unsigned int m_instanceShader;

	// Non-instanced rendering
	unsigned int m_blockMaterialID;
	PositionNormalColorVertex m_vertexBuffer[24];

	// Block particles list
	BlockParticlesList m_vpBlockParticlesList;

	// Block particle emitters list
	BlockParticlesEmitterList m_vpBlockParticleEmittersList;
	BlockParticlesEmitterList m_vpBlockParticleEmittersAddList;

	// Block particle effects list
	BlockParticlesEffectList m_vpBlockParticleEffectsList;
};

#endif