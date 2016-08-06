/*************************************************************************
> File Name: Enemy.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Enemy class.
> Created Time: 2016/08/06
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ENEMY_H
#define CUBBY_ENEMY_H

#include <Blocks/ChunkManager.h>
#include <Models/VoxelCharacter.h>
#include <Projectile/ProjectileManager.h>
#include <Renderer/Renderer.h>

// Forward declaration
class LightingManager;
class EnemyManager;
class BlockParticleManager;
class TextEffectsManager;
class ItemManager;
class NPCManager;
class NPC;
class EnemySpawner;
class HUD;

enum class EnemyType
{
	None = 0,

	GreenSlime,
	RedSlime,
	BlueSlime,
	YellowSlime,

	NormalSkeleton,
	RangedSkeleton,
	MeleeSkeleton,
	MageSkeleton,

	IronGiant,

	Mummy,
	WalkingZombie,
	CrawlingZombie,

	Bee,
	Bat,
	Ghost,
	Doppelganger,

	TargetDummy,

	Mimic,

	SnowGolem,
	MagmaGolem,
};

class Enemy
{
public:
	// Constructor, Destructor
	Enemy(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer, LightingManager* pLightingManager, BlockParticleManager* pBlockParticleManager, TextEffectsManager* pTextEffectsManager, ItemManager* pItemManager, ProjectileManager* pProjectileManager, HUD* pHUD, EnemyManager* pEnemyManager, NPCManager* pNPCManager, QubicleBinaryManager* pQubicleBinaryManager, EnemyType enemyType);
	~Enemy();

	// Erase flag
	bool GetErase() const;
	void SetErase(bool erase);

	// Accessors / Setters
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition() const;
	void SetVelocity(glm::vec3 vel);
	glm::vec3 GetVelocity() const;
	void SetRotation(float rot);
	float GetRotation() const;
	float GetFaceLookingRotation() const;
	void SetScale(float scale);
	float GetRadius() const;
	void UpdateRadius();
	glm::vec3 GetCenter() const;
	glm::vec3 GetAboveHeadPosition() const;
	glm::vec3 GetForwardVector() const;
	glm::vec3 GetRightVector() const;
	glm::vec3 GetUpVector() const;
	glm::vec3 GetTargetForward() const;

	// Setup
	void SetupEnemyForType();
	void SetupWeaponsForType();
	void InitEnemyForType();
	void LoadWeapon(bool left, std::string weaponFile);
	void UnloadWeapon(bool left) const;

	// Enemy spawner
	void SetEnemySpawner(EnemySpawner* pSpawner);
	void RemoveEnemySpawner(EnemySpawner* pSpawner);

	// Animation
	void SetAnimationSpeed(float speed, bool onlySetOnCompleteAnimation, AnimationSections section);
	float GetAnimationSpeed(AnimationSections section);
	bool AnimationFinished(AnimationSections section);

	// Looking
	void SetTargetForwardToLookAtPoint(glm::vec3 point);
	void LookAtPoint(glm::vec3 point);
	void LookAtRandomDirection();
	void SetRandomLookMode();
	void SetBodyTurnStopThreshold(float threshold);
	void SetBodyTurnSpeedMultiplier(float multiplier);

	// World
	void UpdateGridPosition();
	Chunk* GetCachedGridChunkOrFromPosition(glm::vec3 pos) const;
	void ClearChunkCacheForChunk(Chunk* pChunk);

	// Collision
	bool CheckCollisions(glm::vec3 positionCheck, glm::vec3 previousPosition, glm::vec3* pNormal, glm::vec3* pMovement) const;
	bool IsBlockInFront() const;

	// Movement
	void MoveAbsolute(glm::vec3 direction, const float speed, bool shouldChangeForward = true);
	void StopMoving();
	void Jump(bool jumpUpBlock);
	bool CanJump() const;
	bool IsMoving() const;
	float GetMovementSpeed() const;
	bool CanMoveTowardsTarget() const;

	// Combat
	void DoDamage(float amount, Color textColor, glm::vec3 knockbackDirection, float knockbackAmount, bool createParticleHit, bool shouldUpdateGUI);
	void CheckPlayerDamageRadius();
	void CheckNPCDamageRadius();
	void CheckProjectileDamageRadius(Projectile* pProjectile);
	void Explode();
	void ConvertIntoOtherEnemyType(EnemyType newEnemyType, float scale);
	void Respawn();
	bool CanCheckAggroRange() const;

	void SetNPCDied(NPC* pNPC);
	void SetTargetNPC(NPC* pTargetNPC);

	// Gameplay
	void SetSapped(bool sapped);
	bool IsSapped() const;

	// Spawning
	void SetSpawningParams(glm::vec3 start, glm::vec3 end, float time);

	// Wander / Random target
	void SetRandomTargetMode(bool randomTargetMode);
	void GetNewRandomTargetPosition();
	void ResetRandomTargetPosition();
	void SetLeashParameters(glm::vec3 leashPosition, float leashRadius);

	// Attacking
	void Attack();
	void ReleaseAttack();
	bool CanAttack() const;
	bool GetAttackEnabled() const;
	float GetAttackRadius() const;
	float GetAttackRotation() const;
	float GetAttackSegmentAngle() const;

	// Projectile hitbox
	ProjectileHitboxType GetProjectileHitboxType() const;
	float GetProjectileHitboxRadius() const;
	float GetProjectileHitboxXLength() const;
	float GetProjectileHitboxYLength() const;
	float GetProjectileHitboxZLength() const;
	glm::vec3 GetProjectileHitboxCenter() const;

	// Spawn loot items
	void SpawnLootItems() const;
	void SpawnGibs() const;

	// Rendering Helpers
	void CalculateWorldTransformMatrix();
	void SetOutlineRender(bool outline);
	bool GetOutlineRender() const;
	void SetWireFrameRender(bool wireframe) const;

	// Updating
	void UpdateWeaponLights() const;
	void UpdateWeaponParticleEffects() const;
	void Update(float dt);
	void UpdatePhysics(float dt);
	void UpdateLookingAndForwardTarget(float dt);
	void UpdateCombat();
	void UpdateGameplay() const;
	void UpdateMeleeCombat();
	void UpdateRangedCombat();
	void UpdateTouchCombat();
	void UpdateMovement(float dt);
	void UpdateSpawning();
	void UpdateTimers(float dt);

	// Rendering
	void Render(bool outline, bool reflection, bool silhouette) const;
	void RenderFace() const;
	void RenderWeaponTrails() const;
	void RenderDebug() const;
	void RenderForwardDebug() const;
	void RenderRandomPositionDebug() const;
	void RenderChargingAttackDebug() const;
	void RenderLookingAreaDebug() const;
	void RenderProjectileHitboxDebug() const;

protected:
	static void _AttackEnabledTimerFinished(void* pData);
	void AttackEnabledTimerFinished();

	static void _AttackEnabledDelayTimerFinished(void* pData);
	void AttackEnabledDelayTimerFinished();

private:
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	LightingManager* m_pLightingManager;
	Player* m_pPlayer;
	EnemyManager* m_pEnemyManager;
	BlockParticleManager* m_pBlockParticleManager;
	TextEffectsManager* m_pTextEffectsManager;
	ItemManager* m_pItemManager;
	ProjectileManager* m_pProjectileManager;
	HUD* m_pHUD;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	NPCManager* m_pNPCManager;

	// Erase flag
	bool m_erase;

	// Enemy radius
	float m_radius;

	// Enemy position and movement variables
	glm::vec3 m_position;
	glm::vec3 m_previousPosition;
	glm::vec3 m_velocity;
	glm::vec3 m_gravityDirection;

	// Forward vector
	glm::vec3 m_forward;

	// Target forward / looking vector
	glm::vec3 m_targetForward;

	// Scale
	float m_renderScale;

	// Animation params
	float m_animationSpeed[static_cast<int>(AnimationSections::NumSections)];
	bool m_animationSpeedonlySetOnCompleteAnimation[static_cast<int>(AnimationSections::NumSections)];
	bool m_animationFinished[static_cast<int>(AnimationSections::NumSections)];

	// Enemy name
	std::string m_enemyName;

	// Enemy type
	EnemyType m_enemyType;
	std::string m_typeString;
	std::string m_modelNameString;
	int m_modelVariation;

	// Idle flag
	bool m_isIdle;
	float m_waitBeforeStopMovingAnimationTimer;
	bool m_hasSetIdleAnimation;

	// Movement params
	float m_movementSpeed;
	float m_movementFreezeTimer;

	// Jumping params
	bool m_canJump;
	float m_jumpTime;
	float m_jumpTimer;
	float m_jumpHeight;

	// Look at point
	bool m_lookAtPoint;
	glm::vec3 m_lookPoint;
	bool m_isLookingAtPoint;

	// Turn speed multiplier
	float m_bodyTurnSpeedMultiplier;

	// Turn stop threshold
	float m_bodyTurnStopThreshold;

	// Rendering modes
	bool m_outlineRender;

	// Enemy game properties
	float m_maxHealth;
	float m_health;

	// Damage and knockback timers
	float m_damageTime;
	float m_damageTimer;
	float m_knockbackTime;
	float m_knockbackTimer;

	// Combat
	bool m_canAttack;
	float m_attackDelayTime;
	float m_attackDelayTimer;
	bool m_canInteruptCombatAnim;
	bool m_attackEnabled;
	bool m_insideAttackRadius;
	float m_attackEnabledTimer;
	float m_attackEnabledDelayTimer;
	float m_attackSegmentAngle;
	float m_attackRadius;
	float m_attackRotation;
	float m_movementWaitAfterAttackTimer;

	// Gameplay
	bool m_sapped;
	float m_sappedTimer;
	unsigned int m_sappedParticleEffectID;
	BlockParticleEffect* m_pSappedParticleEffect;

	// Charging attacks
	bool m_isChargingAttack;
	float m_chargeAmount;
	float m_chargeTime;
	glm::vec3 m_chargeSpawnVelocity;
	glm::vec3 m_chargeSpawnPosition;

	// Projectile hitbox
	ProjectileHitboxType m_projectileHitboxType;
	float m_projectileHitboxRadius;
	float m_projectileHitboxXLength;
	float m_projectileHitboxYLength;
	float m_projectileHitboxZLength;
	glm::vec3 m_projectileHitboxCenterOffset;

	// Aggro
	bool m_aggro;
	float m_aggroLookSegmentAngle;
	float m_aggroLookRadius;
	float m_aggroResetTime;
	float m_aggroResetTimer;

	// Spawning
	bool m_spawning;
	glm::vec3 m_spawnStartPosition;
	glm::vec3 m_spawnEndPosition;
	float m_spawnTime;
	float m_spawningTimer;
	float m_spawningT;

	// Wander / random target
	bool m_randomTargetPositionMode;
	float m_getRamdomTargetPositionTimer;
	glm::vec3 m_randomTargetPosition;
	float m_leashRadius;
	glm::vec3 m_leashPosition;

	// Stuck
	bool m_updateStuckTimer;
	bool m_stuck;
	float m_stuckTimer;

	// Enemy light
	unsigned int m_enemyLightID;

	// Enemy particle effect
	unsigned int m_enemyParticleEffectID;
	BlockParticleEffect* m_pEnemyParticleEffect;

	// Grid position
	int m_gridPositionX;
	int m_gridPositionY;
	int m_gridPositionZ;

	// Cached chunk from grid position
	Chunk* m_pCachedGridChunk;

	// Enemy's world matrix
	Matrix4 m_worldMatrix;

	// Target NPC
	NPC* m_pTargetNPC;

	// Were we created from an enemy spawner
	EnemySpawner* m_pParentEnemySpawner;

	// Voxel character
	VoxelCharacter* m_pVoxelCharacter;
};

#endif