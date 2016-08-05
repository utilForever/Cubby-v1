/*************************************************************************
> File Name: NPC.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    An NPC is like a player, but not human controlled. Can be set to have
>    different behaviors and different sub-types of NPCs, interacts with
>    the player and also attacks enemies.
> Created Time: 2016/08/06
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_NPC_H
#define CUBBY_NPC_H

#include <Inventory/InventoryManager.h>
#include <Models/VoxelCharacter.h>
#include <Player/PlayerClass.h>
#include <Projectile/ProjectileManager.h>

// Forward declaration
class LightingManager;
class BlockParticleManager;
class TextEffectsManager;
class ItemManager;
class Enemy;
class EnemyManager;

enum class NPCState
{
	Idle = 0,
	MovingToWayPoint,
	Talking,
	Working,
	Combat,
};

enum class NPCCombatType
{
	None = 0,
	MeleeSword,
	Archer,
	Staff,
	FireballHands,
};

struct WayPoint
{
	glm::vec3 m_position;
	float m_xLength;
	float m_yLength;
	float m_zLength;
	WayPoint* m_pNextWaypoint;
};

using WayPointList = std::vector<WayPoint*>;

class NPC
{
public:
	// Constructor, Destructor
	NPC(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer, LightingManager* pLightingManager, BlockParticleManager* pBlockParticleManager, TextEffectsManager* pTextEffectsManager, ItemManager* pItemManager, ProjectileManager* pProjectileManager, EnemyManager* pEnemyManager, QubicleBinaryManager* pQubicleBinaryManager, std::string name, std::string typeName, std::string modelName, bool characterSelectScreen, bool useQubicleManager);
	~NPC();

	void SetLightingManager(LightingManager* pLightingManager);

	void SetErase(bool erase);
	bool NeedErase() const;

	// Accessors / Setters
	VoxelCharacter* GetVoxelCharacter() const;
	void SetName(std::string name);
	std::string GetName() const;
	void SetType(std::string typeName);
	std::string GetType() const;
	void SetModelName(std::string modelName);
	std::string GetModelName() const;
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition() const;
	void SetRotation(float rot);
	float GetRotation() const;
	void SetScale(float scale);
	float GetRadius() const;
	void UpdateRadius();
	glm::vec3 GetCenter() const;
	glm::vec3 GetForwardVector() const;
	glm::vec3 GetRightVector() const;
	glm::vec3 GetUpVector() const;

	// Front-end NPCs
	void SetFrontEndNPC(bool frontEnd);
	bool IsFrontEndNPC() const;
	void SetPlayerClass(PlayerClass selectedClass);
	PlayerClass GetPlayerClass() const;

	// Combat type
	void SetNPCCombatType(NPCCombatType combatType, bool setWeaponModel);

	// NPC setup
	void LoadWeapon(bool left, std::string weaponFile);
	void UnloadWeapon(bool left) const;

	// Equipping items
	void ImportEquippedItems(std::string inventoryFile);
	void EquipItem(EquipSlot equipSlot, const char* itemFileName, bool left, bool right);
	void UnequipItem(EquipSlot equipSlot, bool left, bool right) const;

	// Animation
	void SetAnimationSpeed(float speed, bool onlySetOnCompleteAnimation, AnimationSections section);
	float GetAnimationSpeed(AnimationSections section);
	bool AnimationFinished(AnimationSections section);

	// Skeleton bone scale
	void SetSkeletonBoneScale(float scale) const;
	float GetSkeletonBoneScale() const;

	// NPC State
	NPCState GetState() const;

	// Movement gameplay
	void SetMoveToPlayer(bool moveToPlayer);
	void SetLookAtPositionWhenReachedTarget(bool enabled, glm::vec3 lookAtPosition);

	// Dead
	bool IsDead() const;

	// Projectile hitbox
	ProjectileHitboxType GetProjectileHitboxType() const;
	float GetProjectileHitboxRadius() const;
	float GetProjectileHitboxXLength() const;
	float GetProjectileHitboxYLength() const;
	float GetProjectileHitboxZLength() const;
	glm::vec3 GetProjectileHitboxCenter() const;

	// Movement
	void MoveAbsolute(glm::vec3 direction, const float speed, bool shouldChangeForward = true);
	void StopMoving();
	void Jump();
	void SetTargetPosition(glm::vec3 pos);
	WayPoint* AddWaypoint(glm::vec3 pos, float xLength, float yLength, float zLength);
	void ClearWaypoints();
	void StartWaypointNavigation(int waypointIndex);
	bool IsInsideWaypoint(int waypointIndex);
	void SetMoveBackToPosition(glm::vec3 pos);
	void SetForwards(glm::vec3 dir);
	void SetTargetForwards(glm::vec3 dir);
	bool HasReachedTargetPosition() const;

	// Combat
	void DoDamage(float amount, Color textColor, glm::vec3 knockbackDirection, float knockbackAmount, bool createParticleHit);
	void CheckEnemyDamageRadius();
	void CheckProjectileDamageRadius(Projectile* pProjectile);
	void Explode();
	void Respawn() const;

	// Attacking
	void Attack();
	void ReleaseAttack();
	bool CanAttack() const;
	bool GetAttackEnabled() const;
	float GetAttackRadius() const;
	void SetAttackRadius(float attackRadius);
	float GetAttackRotation() const;
	float GetAttackSegmentAngle() const;
	void SetEnemyDied(Enemy* pEnemy);
	void SetTargetEnemy(Enemy* pEnemy);

	// Looking
	void SetTargetForwardToLookAtPoint(glm::vec3 point);
	void LookAtPoint(glm::vec3 point);
	void SetRandomLookMode();
	void SetBodyTurnStopThreshold(float threshold);
	void SetBodyTurnSpeedMultiplier(float multiplier);
	bool IsLookAtPointMode() const;
	bool IsLookingAtPoint() const;

	// Talking
	void SetNPCTalkMode(float talkDelay, float talkTime);
	void StopNPCTalkMode();

	// Working
	void SetWorking(glm::vec3 workPointOfInterest);
	void StopWorking();

	// World
	void UpdateGridPosition();
	Chunk* GetCachedGridChunkOrFromPosition(glm::vec3 pos) const;
	void ClearChunkCacheForChunk(Chunk* pChunk);

	// Collision
	bool CheckCollisions(glm::vec3 positionCheck, glm::vec3 previousPosition, glm::vec3 *pNormal, glm::vec3 *pMovement) const;
	bool IsBlockInFront() const;
	bool IsPushingCollisionEnabled() const;
	void SetPushingCollisionEnabled(bool pushEnabled);

	void SetCreditsNPC(bool creditsNPC);
	bool IsCreditsNPC() const;

	// Rendering Helpers
	void CalculateWorldTransformMatrix();
	void SetOutlineRender(bool outline);
	bool GetOutlineRender() const;
	void SetHoverRender(bool hover);
	bool GetHoverRender() const;
	void SetWireFrameRender(bool wireframe) const;
	void SetSubSelectionRender(bool subSelection);
	bool GetSubSelectionRender() const;
	glm::vec2 GetScreenPosition() const;

	// Updating
	void UpdateWeaponLights() const;
	void UpdateWeaponParticleEffects() const;
	void UpdateLookingAndForwardTarget(float dt);
	void UpdateTalkMode(float dt);
	void UpdateWorking(float dt);
	void UpdateCombat(float dt);
	void UpdateMeleeCombat();
	void UpdateRangedCombat(float dt);
	void UpdateMovement(float dt);
	void UpdateNPCState(float dt);
	void UpdatePhysics(float dt);
	void Update(float dt);
	void UpdateScreenCoordinates2d(Camera* pCamera);
	void UpdateSubSelectionNamePicking(int pickingId, bool mousePressed);
	void UpdateAggroRadius();
	void UpdateTimers(float dt);

	// Rendering
	void Render(bool outline, bool reflection, bool silhouette) const;
	void RenderFace() const;
	void RenderWeaponTrails() const;
	void RenderDebug();
	void RenderForwardDebug() const;
	void RenderWaypointsDebug();
	void RenderSubSelection(bool outline, bool silhouette) const;
	void RenderSubSelectionNormal() const;
	void RenderSubSelectionNamePicking() const;
	void RenderProjectileHitboxDebug() const;
	void RenderAggroRadiusDebug() const;
	void RenderMovementPositionDebug() const;

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
	BlockParticleManager* m_pBlockParticleManager;
	TextEffectsManager* m_pTextEffectsManager;
	ItemManager* m_pItemManager;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	ProjectileManager* m_pProjectileManager;
	EnemyManager* m_pEnemyManager;

	// Erase flag
	bool m_erase;

	// NPC name
	std::string m_name;
	std::string m_type;
	std::string m_modelName;

	// Player radius
	float m_radius;

	// Forward vector
	glm::vec3 m_forward;

	// Target forward / looking vector
	glm::vec3 m_targetForward;

	// Player position and movement variables
	glm::vec3 m_position;
	glm::vec3 m_previousPosition;
	glm::vec3 m_velocity;
	glm::vec3 m_gravityDirection;

	// NPC's world matrix
	Matrix4 m_worldMatrix;

	// NPC State
	NPCState m_NPCState;
	NPCState m_previousNPCState;

	// Combat type
	NPCCombatType m_NPCCombatType;

	// Frontend NPC flag
	bool m_frontEndNPC;

	// What class are we for the front-end character lineup
	PlayerClass m_selectedClass;

	// Idle flag
	bool m_isIdle;
	float m_waitBeforeStopMovingAnimationTimer;
	bool m_hasSetIdleAnimation;

	// Movement gameplay params
	bool m_moveToPlayer;
	glm::vec3 m_moveBackToPosition;
	glm::vec3 m_lookAtPositionWhenReachedTargetPosition;
	bool m_lookAtPositionWhenReachTarget;
	bool m_hasReachedTargetPosition;

	// Projectile hitbox
	ProjectileHitboxType m_projectileHitboxType;
	float m_projectileHitboxRadius;
	float m_projectileHitboxXLength;
	float m_projectileHitboxYLength;
	float m_projectileHitboxZLength;
	glm::vec3 m_projectileHitboxCenterOffset;

	// Movement
	glm::vec3 m_targetPosition;
	WayPointList m_vpWayPointList;
	int m_currentWaypointIndex;

	// Movement params
	float m_movementSpeed;
	float m_maxMovementSpeed;
	float m_minMovementSpeed;

	// Jumping params
	bool m_canJump;
	float m_jumpTime;
	float m_jumpTimer;
	float m_jumpHeight;

	// Damage and knockback timers
	float m_damageTime;
	float m_damageTimer;
	float m_knockbackTime;
	float m_knockbackTimer;

	// Game properties
	float m_maxHealth;
	float m_health;
	float m_maxMagic;
	float m_magic;

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
	float m_aggroRadius;
	float m_updateAggroRadiusTimer;
	int m_attackCounter;

	// Charging attacks
	bool m_isChargingAttack;
	float m_chargeAmount;
	float m_chargeTime;
	glm::vec3 m_chargeSpawnVelocity;
	glm::vec3 m_chargeSpawnPosition;

	// Look at point
	bool m_bLookAtPoint;
	glm::vec3 m_lookPoint;
	bool m_bIsLookingAtPoint;

	// Turn speed multiplier
	float m_bodyTurnSpeedMultiplier;

	// Turn stop threshold
	float m_bodyTurnStopThreshold;

	// Talking
	bool m_talkMode;
	float m_talkDelay;
	float m_talkTime;
	float m_talkTimer;

	// Working
	bool m_isWorking;
	float m_workingAnimationWaitTimer;
	float m_workingAnimationDelay;
	int m_workingRepetitionBeforeLongDelay;
	int m_workingRepetitionCounter;
	glm::vec3 m_lookAtWorkPoint;
	bool m_createdAnvilHitParticleEffect;

	// Push collision enabled
	bool m_pushingCollisionEnabled;

	// Is this NPC a credits NPC, if so, behave differently
	bool m_isCreditsNPC;

	// Grid position
	int m_gridPositionX;
	int m_gridPositionY;
	int m_gridPositionZ;

	// Cached chunk from grid position
	Chunk* m_pCachedGridChunk;

	// Rendering modes
	bool m_outlineRender;
	bool m_hoverRender;
	bool m_subSelectionRender;

	// 2d screen position, for billboard rendering
	glm::vec2 m_screenPosition;

	// Sub selection of body parts - During character creation screen
	std::string m_subSelectionSelected;
	std::string m_subSelectionHover;

	// Animation params
	float m_animationSpeed[static_cast<int>(AnimationSections::NumSections)];
	bool m_animationSpeedonlySetOnCompleteAnimation[static_cast<int>(AnimationSections::NumSections)];
	bool m_animationFinished[static_cast<int>(AnimationSections::NumSections)];
	float m_animationTimer;

	// Target enemy
	Enemy* m_pTargetEnemy;

	// Voxel character
	VoxelCharacter* m_pVoxelCharacter;
	QubicleBinary* m_pCharacterBackup;
};

#endif