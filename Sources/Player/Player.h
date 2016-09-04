/*************************************************************************
> File Name: Player.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The player class contains all the functionality of a player, an entity that
>    is controlled and can act within the world and game.
> Created Time: 2016/08/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_PLAYER_H
#define CUBBY_PLAYER_H

#include <Blocks/ChunkManager.h>
#include <Inventory/InventoryManager.h>
#include <Lighting/LightingManager.h>
#include <Models/VoxelCharacter.h>
#include <Projectile/ProjectileManager.h>
#include <Renderer/Renderer.h>
#include <TextEffects/TextEffectsManager.h>

#include "PlayerClass.h"
#include "PlayerStats.h"

// Forward declaration
class InventoryGUI;
class CharacterGUI;
class CraftingGUI;
class LootGUI;
class ActionBar;

// Global constants
namespace PlayerEquippedProperties
{
	static const int None = 0;
	static const int Sword = 1;
	static const int Axe = 2;
	static const int Hammer = 4;
	static const int Mace = 8;
	static const int Dagger = 16;
	static const int Sickle = 32;
	static const int TwoHandedSword = 64;
	static const int Shield = 128;
	static const int Boomerang = 256;
	static const int Bow = 512;
	static const int Staff = 1024;
	static const int Wand = 2048;
	static const int Pickaxe = 4096;
	static const int Torch = 8192;
	static const int PlacementItem = 16384;
	static const int PlacementScenery = 32768;
	static const int PlacementBlock = 65536;
	static const int Consumable = 131072;
	static const int Bomb = 262144;
	static const int SpellHands = 524288;
}

class Player
{
public:
	// Constructor, Destructor
	Player(Renderer* pRenderer, ChunkManager* pChunkManager, QubicleBinaryManager* pQubicleBinaryManager, LightingManager* pLightingManager, BlockParticleManager* pBlockParticleManager);
	~Player();

	// Linkage
	void SetInventoryManager(InventoryManager* pInventoryManager);
	void SetItemManager(ItemManager* pItemManager);
	void SetProjectileManager(ProjectileManager* pProjectileManager);
	void SetTextEffectsManager(TextEffectsManager* pTextEffectsManager);
	void SetEnemyManager(EnemyManager* pEnemyManager);
	void SetInventoryGUI(InventoryGUI* pInventoryGUI);
	void SetCharacterGUI(CharacterGUI* pCharacterGUI);
	void SetCraftingGUI(CraftingGUI* pCraftingGUI);
	void SetLootGUI(LootGUI* pLootGUI);
	void SetActionBar(ActionBar* pActionBar);

	// Get voxel character pointer
	VoxelCharacter* GetVoxelCharacter() const;

	// Player reset
	void ResetPlayer();

	// Accessors / Setters
	void SetClass(PlayerClass ePlayerClass);
	PlayerClass GetClass() const;
	void SetName(std::string name);
	std::string GetName() const;
	void SetType(std::string typeName);
	std::string GetType() const;
	void SetModelname(std::string modelName);
	std::string GetModelName() const;
	void SetPosition(glm::vec3 pos);
	void SetRespawnPosition(glm::vec3 pos);
	glm::vec3 GetRespawnPosition() const;
	glm::vec3 GetCenter() const;
	glm::vec3 GetForwardVector() const;
	glm::vec3 GetRightVector() const;
	glm::vec3 GetUpVector() const;
	float GetRadius() const;
	void UpdateRadius();
	void SetForwardVector(glm::vec3 forward);
	void SetRotation(float rot);
	float GetRotation() const;

	// Loading
	void LoadCharacter(std::string characterName, bool fromCharacterSelectScreen);

	// Unloading
	void LoadWeapon(bool left, std::string weaponFile) const;
	void UnloadWeapon(bool left) const;

	// Equipping items
	void EquipItem(InventoryItem* pItem);
	void UnequipItem(EquipSlot equipSlot, bool left, bool right);

	// Stat modifier values
	void RefreshStatModifierCacheValues();

	// Collision
	bool CheckCollisions(glm::vec3 positionCheck, glm::vec3 previousPosition, glm::vec3 *pNormal, glm::vec3 *pMovement, bool *pStepUpBlock);

	// Selection
	bool GetSelectionBlock(glm::vec3* blockPos, int* blockX, int* blockY, int* blockZ) const;
	bool GetPlacementBlock(glm::vec3* blockPos, int* blockX, int* blockY, int* blockZ) const;

	// World
	void UpdateGridPosition();
	int GetGridX() const;
	int GetGridY() const;
	int GetGridZ() const;
	Chunk* GetCachedGridChunkOrFromPosition(glm::vec3 pos) const;
	void ClearChunkCacheForChunk(Chunk* pChunk);

	// Camera
	void SetCameraPosition(glm::vec3 cameraPos);
	void SetCameraForward(glm::vec3 cameraForward);
	void SetCameraUp(glm::vec3 up);
	void SetCameraRight(glm::vec3 right);

	// Loading configuration and settings for the game
	void LoadCharacterSettings();
	void StartGame();

	// Movement
	glm::vec3 GetPositionMovementAmount() const;
	glm::vec3 MoveAbsolute(glm::vec3 direction, const float speed, bool shouldChangeForward = true);
	void Move(const float speed);
	void Strafe(const float speed);
	void Levitate(const float speed);
	void StopMoving();
	void Jump();
	bool CanJump() const;
	void SetMoveToTargetPosition(glm::vec3 pos);
	void DisableMoveToTargetPosition();
	void SetLookAtTargetAfterMoveToPosition(glm::vec3 lookAt);
	void CreateFloorParticles();

	// Dead
	bool IsDead() const;

	// Level up
	void LevelUp() const;

	// Game play
	float GetHealth() const;
	float GetMaxHealth() const;
	float GetMagic() const;
	float GetMaxMagic() const;
	void GiveHealth(float amount);
	void GiveCoins(int amount) const;

	// Player stats
	void SetSupressStatsExport(bool supress) const;
	PlayerStats* GetPlayerStats() const;
	std::string GetStrengthModifierString() const;
	std::string GetDexterityModifierString() const;
	std::string GetIntelligenceModifierString() const;
	std::string GetVitalityModifierString() const;
	std::string GetArmorModifierString() const;
	std::string GetLuckModifierString() const;

	// Combat
	void PressAttack();
	void ReleaseAttack();
	bool CanAttackLeft() const;
	bool CanAttackRight() const;
	bool GetAttackEnabled() const;
	float GetAttackRadius() const;
	float GetAttackRotation() const;
	float GetAttackSegmentAngle() const;
	void CheckEnemyDamageRadius(Enemy* pEnemy);
	void CheckProjectileDamageRadius(Projectile* pProjectile);
	void DoDamage(float amount, Color textColor, glm::vec3 knockbackDirection, float knockbackAmount, bool createParticleHit);
	void Explode();
	void Respawn();

	// World editing
	void DestroyBlock();

	// Enemy target
	void SetEnemyDied(Enemy* pEnemy);
	void SetTargetEnemy(Enemy* pTargetEnemy);
	Enemy* GetTargetEnemy() const;

	// Crafting
	void SetCrafting(bool crafting);
	void SetCraftingItem(bool crafting);
	bool IsCrafting() const;

	// Looking
	void LookAtPoint(glm::vec3 point);
	void SetRandomLookMode();
	void SetBodyTurnStopThreshold(float threshold);
	void SetBodyTurnSpeedMultiplier(float multiplier);

	// Projectile hitbox
	ProjectileHitboxType GetProjectileHitboxType() const;
	float GetProjectileHitboxRadius() const;
	float GetProjectileHitboxXLength() const;
	float GetProjectileHitboxYLength() const;
	float GetProjectileHitboxZLength() const;
	glm::vec3 GetProjectileHitboxCenter() const;

	// Player equipped attributes
	void SetNormal();
	void SetSword(bool s);
	void SetAxe(bool s);
	void SetHammer(bool s);
	void SetMace(bool s);
	void SetDagger(bool s);
	void SetSickle(bool s);
	void SetTwoHandedSword(bool s);
	void SetShield(bool s);
	void SetBoomerang(bool s);
	void SetBow(bool s);
	void SetStaff(bool s);
	void SetWand(bool s);
	void SetPickaxe(bool s);
	void SetTorch(bool s);
	void SetItemPlacing(bool s);
	void SetSceneryPlacing(bool s);
	void SetBlockPlacing(bool s);
	void SetConsumable(bool s);
	void SetBomb(bool s);
	void SetSpellHands(bool s);

	bool IsNormal() const;
	bool IsSword() const;
	bool IsAxe() const;
	bool IsHammer() const;
	bool IsMace() const;
	bool IsDagger() const;
	bool IsSickle() const;
	bool IsTwoHandedSword() const;
	bool IsShield() const;
	bool IsBoomerang() const;
	bool IsBow() const;
	bool IsStaff() const;
	bool IsWand() const;
	bool IsPickaxe() const;
	bool IsTorch() const;
	bool IsItemPlacing() const;
	bool IsSceneryPlacing() const;
	bool IsBlockPlacing() const;
	bool IsConsumable() const;
	bool IsBomb() const;
	bool IsSpellHands() const;

	// Rendering modes
	void SetWireFrameRender(bool wireframe) const;
	void SetPlayerAlpha(float alpha) const;
	void SetFirstPersonMode() const;
	void SetThirdPersonMode() const;

	// Rendering Helpers
	void CalculateWorldTransformMatrix();
	void RebuildVoxelCharacter(bool faceMerge) const;

	// Updating
	void Update(float dt);
	void UpdateAnimations();
	void UpdatePhysics(float dt);
	void UpdateMovement(float dt);
	void UpdateWorking(float dt);
	void UpdateLookingAndForwardTarget(float dt);
	void UpdateMagic(float dt);
	void UpdateTimers(float dt);
	void UpdateWeaponLights() const;
	void UpdateWeaponParticleEffects() const;
	void UpdateChargingAttack(float dt);
	void UpdateCombat();
	void UpdateBlockSelection();

	// Rendering
	void Render() const;
	void RenderFirstPerson() const;
	void RenderWeaponTrails() const;
	void RenderFace() const;
	void RenderPaperdoll() const;
	void RenderPaperdollFace() const;
	void RenderPortrait() const;
	void RenderPortraitFace() const;
	void RenderSelectionBlock() const;
	void RenderDebug();
	void RenderProjectileHitboxDebug();

	// Constants
	static const glm::vec3 PLAYER_CENTER_OFFSET;
	static const int PLAYER_NAME_PICKING = 1;

protected:
	static void _StepUpAnimationFinished(void *apData);
	void StepUpAnimationFinished();

	static void _AttackEnabledTimerFinished(void *apData);
	void AttackEnabledTimerFinished();

	static void _AttackEnabledDelayTimerFinished(void *apData);
	void AttackEnabledDelayTimerFinished();

	static void _AttackAnimationTimerFinished(void *apData);
	void AttackAnimationTimerFinished();

	static void _AttackAnimationTimerFinished_Alternative(void *apData);
	void AttackAnimationTimerFinished_Alternative();

private:
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	LightingManager* m_pLightingManager;
	BlockParticleManager* m_pBlockParticleManager;
	InventoryManager* m_pInventoryManager;
	ItemManager* m_pItemManager;
	ProjectileManager* m_pProjectileManager;
	TextEffectsManager* m_pTextEffectsManager;
	EnemyManager* m_pEnemyManager;
	InventoryGUI* m_pInventoryGUI;
	CharacterGUI* m_pCharacterGUI;
	CraftingGUI* m_pCraftingGUI;
	LootGUI* m_pLootGUI;
	ActionBar* m_pActionBar;

	// Player position and movement variables
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_force;

	// Store previous position each frame after we have worked out the new position
	// Used for collision and other movement calculations
	glm::vec3 m_previousPosition;

	// Players respawn position
	glm::vec3 m_respawnPosition;

	// The direction of gravity for the player
	glm::vec3 m_gravityDirection;

	// Local axis
	glm::vec3 m_forward;
	glm::vec3 m_right;
	glm::vec3 m_up;

	// Target forward / looking vector
	glm::vec3 m_targetForward;

	// Keep track of how much we have changed position in the update, based on physics, etc.
	// So that the fake camera position can be updated, if we are in some kind of follow camera mode.
	glm::vec3 m_positionMovementAmount;

	// Player name
	std::string m_name;

	// Player class
	PlayerClass m_class;

	// Player type and model name
	std::string m_type;
	std::string m_modelName;

	// Stepping up single world blocks by walking into them
	bool m_doStepUpAnimation;
	float m_stepUpAnimationYAmount;
	float m_stepUpAnimationPrevious;
	float m_stepUpAnimationYOffset;
	float m_stepUpAdditionYAmountChangedCache;

	// Grid position
	int m_gridPositionX;
	int m_gridPositionY;
	int m_gridPositionZ;

	// Cached chunk from grid position
	Chunk* m_pCachedGridChunk;

	// Camera variables
	glm::vec3 m_cameraPosition;
	glm::vec3 m_cameraForward;
	glm::vec3 m_cameraUp;
	glm::vec3 m_cameraRight;

	// Ground flag
	bool m_isOnGround;
	float m_groundCheckTimer;

	// Floor particles
	float m_floorParticleTimer;

	// Flag to control if we are allowed to jump or not, reset when landing
	bool m_canJump;
	float m_jumpTimer;

	// Idle flag
	bool m_isIdle;

	// Dead flag
	bool m_dead;

	// Create a ghost after we have died.
	bool m_createGhost;
	float m_createGhostTimer;

	// Game properties
	float m_maxHealth;
	float m_health;
	float m_maxMagic;
	float m_magic;

	// Damage and knockback timers
	float m_damageTime;
	float m_damageTimer;
	float m_knockbackTime;
	float m_knockbackTimer;
	float m_hitFacialExpressionTime;
	float m_hitFacialExpressionTimer;
	bool m_returnToNormalFacialExpressionAfterHit;

	// Crafting
	bool m_crafting;
	float m_workingAnimationWaitTimer;
	float m_workingAnimationDelay;
	bool m_createdAnvilHitParticleEffect;

	// Moving to target position, for item interaction points, and NPC dialog
	bool m_moveToTargetPosition;
	glm::vec3 m_targetPosition;
	glm::vec3 m_lookAtPositionAfterMoveToTarget;

	// Look at point
	bool m_lookAtPoint;
	glm::vec3 m_lookPoint;

	// Turn speed multiplier
	float m_bodyTurnSpeedMultiplier;

	// Turn stop threshold
	float m_bodyTurnStopThreshold;

	// Player stats
	PlayerStats* m_pPlayerStats;

	// Combat
	bool m_canAttackLeft;
	bool m_canAttackRight;
	bool m_canInteruptCombatAnim;
	bool m_canThrowWeapon;
	float m_bowAttackDelay;
	bool m_attackEnabled;
	float m_attackEnabledTimer;
	float m_attackEnabledDelayTimer;
	float m_attackSegmentAngle;
	float m_attackRadius;
	float m_attackRotation;

	// Target enemy pointer, used for target camera
	Enemy* m_pTargetEnemy;

	// Projectile hitbox
	ProjectileHitboxType m_projectileHitboxType;
	float m_projectileHitboxRadius;
	float m_projectileHitboxXLength;
	float m_projectileHitboxYLength;
	float m_projectileHitboxZLength;
	glm::vec3 m_projectileHitboxCenterOffset;

	// Charging attacks
	bool m_isChargingAttack;
	float m_chargeAmount;
	float m_chargeTime;
	glm::vec3 m_chargeSpawnVelocity;
	glm::vec3 m_chargeSpawnPosition;

	// Bitfield flag to hold the equipped properties for a player
	unsigned int m_equippedProperties;

	// Block selection
	bool m_blockSelection;
	glm::vec3 m_blockSelectionPos;

	// Cached values for stat modifications, cache is refreshed whenever we equip or unequip a new item
	int m_strengthModifier;
	int m_dexterityModifier;
	int m_intelligenceModifier;
	int m_vitalityModifier;
	int m_armorModifier;
	int m_luckModifier;

	// Player radius
	float m_radius;

	// Footstep sounds
	int m_footstepSoundIndex;
	float m_footstepSoundTimer;
	float m_footstepSoundDistance;

	// Animation params
	bool m_animationFinished[static_cast<int>(AnimationSections::NumSections)];
	float m_animationTimer;

	// Players world matrix
	Matrix4 m_worldMatrix;

	// Voxel character
	VoxelCharacter* m_pVoxelCharacter;
	QubicleBinary* m_pCharacterBackup;
};


#endif