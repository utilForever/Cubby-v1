/*************************************************************************
> File Name: BlockParticleEffect.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Block particle effect.
> Created Time: 2016/07/11
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BLOCK_PARTICLE_EFFECT_H
#define CUBBY_BLOCK_PARTICLE_EFFECT_H

#include <Renderer/Renderer.h>

#include <vector>

// Forward declaration
class BlockParticleManager;
class BlockParticleEmitter;

using BlockParticlesEmitterList = std::vector<BlockParticleEmitter*>;

class BlockParticleEffect
{
public:
	// Constructor, Destructor
	BlockParticleEffect(Renderer* pRenderer, BlockParticleManager* pBlockParticleManager);
	~BlockParticleEffect();

	void ClearEmitters();

	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition() const;

	void Import(const char* fileName);
	void Export(const char* fileName);

	void PlayEffect();
	void StopEffect();
	void PauseEffect();

	bool IsStarted() const;
	bool IsPaused() const;

	void AddEmitterToParticleEffect(BlockParticleEmitter* pEmitter);
	void RemoveEmitterFromParticleEffect(BlockParticleEmitter* pEmitter);

	BlockParticleEmitter* GetEmitter(std::string emitterName);

	// Update
	void Update(float dt);

	// Rendering
	void Render() const;

	bool m_isErase;

	// Effect ID
	unsigned int m_particleEffectID;

	// Name
	std::string m_effectName;

	// Position
	glm::vec3 m_position;

	// Position with no world offset
	glm::vec3 m_positionNoWorldOffset;

	// Flag to indicate if we should render particles created from this particle effect in the no world offset viewports (characterGUI)
	bool m_isRenderNoWoldOffsetViewport;

	// Life
	bool m_isEndless;
	float m_lifeTime;
	float m_lifeTimer;

	// Playback controls
	bool m_isStarted;
	bool m_isPaused;

private:
	Renderer* m_pRenderer;
	BlockParticleManager* m_pBlockParticleManager;

	BlockParticlesEmitterList m_vpBlockParticleEmittersList;
};


#endif