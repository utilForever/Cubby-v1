/*************************************************************************
> File Name: Enemy.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Enemy class.
> Created Time: 2016/08/06
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <CubbyGame.h>

#include <Items/ItemManager.h>
#include <Lighting/LightingManager.h>
#include <Maths/3DMaths.h>
#include <Models/VoxelObject.h>
#include <NPC/NPCManager.h>
#include <Player/Player.h>
#include <Sounds/SoundEffects.h>
#include <Utils/Interpolator.h>
#include <Utils/Random.h>

#include "EnemyManager.h"
#include "EnemySpawner.h"
#include "Enemy.h"

// Constructor, Destructor
Enemy::Enemy(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer, LightingManager* pLightingManager, BlockParticleManager* pBlockParticleManager, TextEffectsManager* pTextEffectsManager, ItemManager* pItemManager, ProjectileManager* pProjectileManager, HUD* pHUD, EnemyManager* pEnemyManager, NPCManager* pNPCManager, QubicleBinaryManager* pQubicleBinaryManager, EnemyType enemyType) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager), m_pLightingManager(pLightingManager), m_pPlayer(pPlayer),
	m_pEnemyManager(pEnemyManager), m_pBlockParticleManager(pBlockParticleManager), m_pTextEffectsManager(pTextEffectsManager),
	m_pItemManager(pItemManager), m_pProjectileManager(pProjectileManager), m_pHUD(pHUD),
	m_pQubicleBinaryManager(pQubicleBinaryManager), m_pNPCManager(pNPCManager), m_enemyType(enemyType)
{
	m_radius = 1.0f;

	m_gravityDirection = glm::vec3(0.0f, -1.0f, 0.0f);

	m_forward = glm::vec3(0.0f, 0.0f, 1.0f);
	m_targetForward = glm::vec3(0.0f, 0.0f, 1.0f);

	for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
	{
		m_animationSpeed[i] = 1.0f;
		m_animationSpeedonlySetOnCompleteAnimation[i] = false;
		m_animationFinished[i] = false;
	}

	// Parent spawner
	m_pParentEnemySpawner = nullptr;

	// Enemy name
	m_enemyName = "Enemy";

	// Idle
	m_isIdle = true;
	m_waitBeforeStopMovingAnimationTimer = 0.0f;
	m_hasSetIdleAnimation = true;

	// Rendering modes
	m_outlineRender = false;

	// Movement
	m_movementSpeed = 6.0f;
	m_movementFreezeTimer = 0.0f;

	// Jumping
	m_canJump = true;
	m_jumpTime = 1.5f;
	m_jumpTimer = 0.0f;
	m_jumpHeight = 14.5f;

	// Combat
	m_canAttack = true;
	m_attackDelayTime = 0.25f;
	m_attackDelayTimer = m_attackDelayTime*2.0f;
	m_attackEnabledDelayTimer = 0.0f;
	m_canInteruptCombatAnim = true;
	m_attackEnabled = false;
	m_insideAttackRadius = false;
	m_attackEnabledTimer = 0.0f;
	m_attackSegmentAngle = 0.75f;
	m_attackRadius = 1.75f;
	m_attackRotation = 0.0f;
	m_movementWaitAfterAttackTimer = 0.0f;

	// Gameplay
	m_sapped = false;
	m_sappedTimer = 0.0f;
	m_sappedParticleEffectID = -1;
	m_pSappedParticleEffect = nullptr;

	// Spawning
	m_spawning = false;
	m_spawnTime = 0.0f;
	m_spawningTimer = 0.0f;
	m_spawningT = 0.0f;

	// Wander / random target
	m_randomTargetPositionMode = false;
	m_getRamdomTargetPositionTimer = 3.0f;
	m_leashRadius = 10.0f;

	// Stuck
	m_updateStuckTimer = false;
	m_stuck = false;
	m_stuckTimer = 5.0f;

	// Enemy light
	m_enemyLightID = -1;

	// Enemy particle effect
	m_pEnemyParticleEffect = nullptr;

	// Charging attacks
	m_isChargingAttack = false;
	m_chargeAmount = 0.0f;
	m_chargeTime = 1.0f;

	// Projectile hitbox
	m_projectileHitboxType = ProjectileHitboxType::Sphere;
	m_projectileHitboxRadius = 0.5f;
	m_projectileHitboxXLength = 0.5f;
	m_projectileHitboxYLength = 0.5f;
	m_projectileHitboxZLength = 0.5f;

	// Aggro
	m_aggro = false;
	m_aggroLookSegmentAngle = 0.7f;
	m_aggroLookRadius = 10.0f;
	m_aggroResetTime = 5.0f;
	m_aggroResetTimer = m_aggroResetTime;

	// Look at point
	m_lookAtPoint = false;
	m_bodyTurnSpeedMultiplier = 3.5f;
	m_bodyTurnStopThreshold = 0.35f;
	m_isLookingAtPoint = false;

	m_maxHealth = 100.0f;
	m_health = m_maxHealth;

	m_damageTime = 0.4f;
	m_damageTimer = m_damageTime;
	m_knockbackTime = 0.4f;
	m_knockbackTimer = m_knockbackTime;

	m_pTargetNPC = nullptr;

	m_gridPositionX = 0;
	m_gridPositionY = 0;
	m_gridPositionZ = 0;

	m_pCachedGridChunk = nullptr;

	m_modelVariation = 1;

	char characterBaseFolder[128];
	char qbFileName[128];
	char ms3dFileName[128];
	char animListFileName[128];
	char facesFileName[128];
	char characterFileName[128];

	SetupEnemyForType();

	sprintf(characterBaseFolder, "Resources/gamedata/models");
	sprintf(qbFileName, "Resources/gamedata/models/%s/%s.qb", m_typeString.c_str(), m_modelNameString.c_str());
	sprintf(ms3dFileName, "Resources/gamedata/models/%s/%s.ms3d", m_typeString.c_str(), m_typeString.c_str());
	sprintf(animListFileName, "Resources/gamedata/models/%s/%s.animlist", m_typeString.c_str(), m_typeString.c_str());
	sprintf(facesFileName, "Resources/gamedata/models/%s/%s.faces", m_typeString.c_str(), m_modelNameString.c_str());
	sprintf(characterFileName, "Resources/gamedata/models/%s/%s.character", m_typeString.c_str(), m_modelNameString.c_str());

	if (m_enemyType == EnemyType::Doppelganger)
	{
		char playerFolder[128];
		sprintf(playerFolder, "Saves/characters/%s", m_modelNameString.c_str());
		sprintf(qbFileName, "Saves/characters/%s/%s.qb", m_modelNameString.c_str(), m_modelNameString.c_str());
		sprintf(facesFileName, "Saves/characters/%s/%s.faces", m_modelNameString.c_str(), m_modelNameString.c_str());
		sprintf(characterFileName, "Saves/characters/%s/%s.character", m_modelNameString.c_str(), m_modelNameString.c_str());

		m_pLightingManager->AddLight(glm::vec3(0.0f, 0.0f, 0.0f), 10.0f, 1.0f, Color(0.0f, 0.0f, 1.0f, 1.0f), &m_enemyLightID);

		m_pEnemyParticleEffect = m_pBlockParticleManager->ImportParticleEffect("Resources/gamedata/particles/ghost_rain.effect", m_position, &m_enemyParticleEffectID);
		m_pEnemyParticleEffect->PlayEffect();
	}

	m_pVoxelCharacter = new VoxelCharacter(m_pRenderer, m_pQubicleBinaryManager);
	m_pVoxelCharacter->UnloadCharacter();
	m_pVoxelCharacter->Reset();

	bool useQubicleManager = false;
	m_pVoxelCharacter->LoadVoxelCharacter(m_typeString.c_str(), qbFileName, ms3dFileName, animListFileName, facesFileName, characterFileName, characterBaseFolder, useQubicleManager);

	m_pVoxelCharacter->SetBreathingAnimationEnabled(true);
	m_pVoxelCharacter->SetWinkAnimationEnabled(true);
	m_pVoxelCharacter->SetTalkingAnimationEnabled(false);
	m_pVoxelCharacter->SetRandomMouthSelection(false);
	m_pVoxelCharacter->SetRandomLookDirection(true);
	m_pVoxelCharacter->SetWireFrameRender(false);

	m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose");

	SetupWeaponsForType();

	InitEnemyForType();

	m_erase = false;
}

Enemy::~Enemy()
{
	// If we belong to a spawner, make sure we indicate that we were killed
	if (m_pParentEnemySpawner != nullptr)
	{
		m_pParentEnemySpawner->RemoveEnemyFromThisSpawner();
	}

	UnloadWeapon(true);
	UnloadWeapon(false);

	m_pVoxelCharacter->RemoveQubicleMatrix("Quiver");

	if (m_enemyType == EnemyType::Doppelganger)
	{
		// Remove enemy light
		m_pLightingManager->RemoveLight(m_enemyLightID);

		// Create dying light
		unsigned int ID;
		m_pLightingManager->AddDyingLight(GetCenter(), 10.0f, 1.0f, Color(0.0f, 0.0f, 1.0f, 1.0f), 2.0f, &ID);

		m_pEnemyParticleEffect->StopEffect();
		m_pEnemyParticleEffect->m_isErase = true;
	}

	delete m_pVoxelCharacter;
}

// Erase flag
bool Enemy::GetErase() const
{
	return m_erase;
}

void Enemy::SetErase(bool erase)
{
	m_erase = erase;
}

// Accessors / Setters
void Enemy::SetPosition(glm::vec3 pos)
{
	m_position = pos;
}

glm::vec3 Enemy::GetPosition() const
{
	return m_position;
}

void Enemy::SetVelocity(glm::vec3 vel)
{
	m_velocity = vel;
}

glm::vec3 Enemy::GetVelocity() const
{
	return m_velocity;
}

void Enemy::SetRotation(float rot)
{
	float angle = DegreeToRadian(rot);
	m_forward = glm::vec3(sin(angle), 0.0f, cos(angle));
	m_targetForward = m_forward;
}

float Enemy::GetRotation() const
{
	float rotationAngle = acos(dot(glm::vec3(0.0f, 0.0f, 1.0f), m_forward));
	rotationAngle = RadianToDegree(rotationAngle);
	
	if (m_forward.x < 0.0f)
	{
		rotationAngle = (360.0f - rotationAngle);
	}

	return rotationAngle;
}

float Enemy::GetFaceLookingRotation() const
{
	glm::vec3 faceLooking = m_pVoxelCharacter->GetFaceLookingDirection();
	faceLooking.y = 0.0f;
	faceLooking = normalize(faceLooking);

	float rotationAngle = acos(dot(glm::vec3(0.0f, 0.0f, 1.0f), faceLooking));
	rotationAngle = RadianToDegree(rotationAngle);

	if (faceLooking.x < 0.0f)
	{
		rotationAngle = (360.0f - rotationAngle);
	}

	return rotationAngle;
}

void Enemy::SetScale(float scale)
{
	m_renderScale = scale;

	m_pVoxelCharacter->SetCharacterScale(scale);

	UpdateRadius();
}

float Enemy::GetRadius() const
{
	return m_radius;
}

void Enemy::UpdateRadius()
{
	m_radius = m_pVoxelCharacter->GetCharacterScale() / 0.14f;
}

glm::vec3 Enemy::GetCenter() const
{
	glm::vec3 center = m_position + glm::vec3(0.0f, m_radius, 0.0f);

	return center;
}

glm::vec3 Enemy::GetAboveHeadPosition() const
{
	glm::vec3 abovePosition;

	if (m_projectileHitboxType == ProjectileHitboxType::Sphere)
	{
		abovePosition.y += m_projectileHitboxRadius * 1.1f;
	}
	else if (m_projectileHitboxType == ProjectileHitboxType::Cube)
	{
		abovePosition.y += m_projectileHitboxYLength * 1.75f;
	}

	return GetCenter() + abovePosition;
}

glm::vec3 Enemy::GetForwardVector() const
{
	return m_worldMatrix.GetForwardVector();
}

glm::vec3 Enemy::GetRightVector() const
{
	return m_worldMatrix.GetRightVector();
}

glm::vec3 Enemy::GetUpVector() const
{
	return m_worldMatrix.GetUpVector();
}

glm::vec3 Enemy::GetTargetForward() const
{
	return m_targetForward;
}

// Setup
void Enemy::SetupEnemyForType()
{
	switch (m_enemyType)
	{
	case EnemyType::None:
	{
	}
	break;
	case EnemyType::GreenSlime:
	{
		m_typeString = "Jumping";
		m_modelNameString = "GreenSlime";
	}
	break;
	case EnemyType::RedSlime:
	{
		m_typeString = "Jumping";
		m_modelNameString = "RedSlime";
	}
	break;
	case EnemyType::BlueSlime:
	{
		m_typeString = "Jumping";
		m_modelNameString = "BlueSlime";
	}
	break;
	case EnemyType::YellowSlime:
	{
		m_typeString = "Jumping";
		m_modelNameString = "YellowSlime";
	}
	break;

	case EnemyType::NormalSkeleton:
	{
		m_typeString = "Human";
		m_modelNameString = "Skeleton1";
	}
	break;
	case EnemyType::RangedSkeleton:
	{
		m_typeString = "Human";
		m_modelNameString = "Skeleton2";
	}
	break;
	case EnemyType::MeleeSkeleton:
	{
		m_typeString = "Human";
		m_modelNameString = "Skeleton3";
	}
	break;
	case EnemyType::MageSkeleton:
	{
		m_typeString = "Human";
		m_modelNameString = "Skeleton4";
	}
	break;

	case EnemyType::IronGiant:
	{
		m_typeString = "Human";
		m_modelNameString = "IronGiant";
	}
	break;

	case EnemyType::Mummy:
	{
		m_typeString = "Human";
		m_modelNameString = "Mummy";
	}
	break;

	case EnemyType::WalkingZombie:
	{
		if (GetRandomNumber(0, 100) > 50.0f)
		{
			m_modelVariation = 1;
		}
		else
		{
			m_modelVariation = 2;
		}

		char variationString[64];
		sprintf(variationString, "WalkingZombie%i", m_modelVariation);

		m_typeString = "Human";
		m_modelNameString = variationString;
	}
	break;
	case EnemyType::CrawlingZombie:
	{
		char variationString[64];
		sprintf(variationString, "CrawlingZombie%i", m_modelVariation);

		m_typeString = "Human";
		m_modelNameString = variationString;
	}
	break;

	case EnemyType::Bee:
	{
		m_typeString = "Flying";
		m_modelNameString = "Bee";
	}
	break;
	case EnemyType::Bat:
	{
		m_typeString = "Flying";
		m_modelNameString = "Bat";
	}
	break;
	case EnemyType::Ghost:
	{
		m_typeString = "Flying";
		m_modelNameString = "Ghost";
	}
	break;
	case EnemyType::Doppelganger:
	{
		m_typeString = m_pPlayer->GetType();
		m_modelNameString = m_pPlayer->GetModelName();
	}
	break;

	case EnemyType::TargetDummy:
	{
		m_typeString = "Dummy";
		m_modelNameString = "TargetDummy";
	}
	break;

	case EnemyType::Mimic:
	{
		m_typeString = "Jumping";
		m_modelNameString = "Mimic";
	}
	break;

	case EnemyType::SnowGolem:
	{
	}
	break;
	case EnemyType::MagmaGolem:
	{
	}
	break;
	}
}

void Enemy::SetupWeaponsForType()
{
	switch (m_enemyType)
	{
	case EnemyType::None:
	{
	}
	break;

	case EnemyType::GreenSlime:
	{
	}
	break;
	case EnemyType::RedSlime:
	{
	}
	break;
	case EnemyType::BlueSlime:
	{
	}
	break;
	case EnemyType::YellowSlime:
	{
	}
	break;

	case EnemyType::NormalSkeleton:
	{
	}
	break;
	case EnemyType::RangedSkeleton:
	{
		LoadWeapon(true, GetEquipmentFileNameForType(EquipmentType::WoodenBow));
	}
	break;
	case EnemyType::MeleeSkeleton:
	{
		LoadWeapon(false, GetEquipmentFileNameForType(EquipmentType::IronSword));
		LoadWeapon(true, GetEquipmentFileNameForType(EquipmentType::IronShield));
	}
	break;
	case EnemyType::MageSkeleton:
	{
		LoadWeapon(false, GetEquipmentFileNameForType(EquipmentType::NecroStaff));
	}
	break;

	case EnemyType::IronGiant:
	{
	}
	break;

	case EnemyType::Mummy:
	{
	}
	break;

	case EnemyType::WalkingZombie:
	{
	}
	break;

	case EnemyType::CrawlingZombie:
	{
	}
	break;

	case EnemyType::Bee:
	{
	}
	break;
	case EnemyType::Bat:
	{
	}
	break;
	case EnemyType::Ghost:
	{
	}
	break;
	case EnemyType::Doppelganger:
	{
	}
	break;

	case EnemyType::TargetDummy:
	{
	}
	break;

	case EnemyType::Mimic:
	{
	}
	break;

	case EnemyType::SnowGolem:
	{
	}
	break;
	case EnemyType::MagmaGolem:
	{
	}
	break;
	}
}

void Enemy::InitEnemyForType()
{
	switch (m_enemyType)
	{
	case EnemyType::None:
	{
	}
	break;

	case EnemyType::GreenSlime:
	{
		m_enemyName = "Green Slime";

		m_pVoxelCharacter->SetBreathingAnimationEnabled(false);
		m_pVoxelCharacter->SetWinkAnimationEnabled(false);
		m_pVoxelCharacter->SetRandomLookDirection(false);

		m_pVoxelCharacter->SetMeshAlpha(0.65f);

		m_projectileHitboxType = ProjectileHitboxType::Sphere;
		m_projectileHitboxRadius = 0.75f;

		m_jumpTime = 2.5f;
		m_attackRadius = 1.15f;
		m_bodyTurnStopThreshold = 0.025f;
	}
	break;
	case EnemyType::RedSlime:
	{
		m_enemyName = "Red Slime";

		m_pVoxelCharacter->SetBreathingAnimationEnabled(false);
		m_pVoxelCharacter->SetWinkAnimationEnabled(false);
		m_pVoxelCharacter->SetRandomLookDirection(false);

		m_pVoxelCharacter->SetMeshAlpha(0.65f);

		m_projectileHitboxType = ProjectileHitboxType::Sphere;
		m_projectileHitboxRadius = 0.75f;

		m_jumpTime = 2.5f;
		m_attackRadius = 1.15f;
		m_bodyTurnStopThreshold = 0.025f;
	}
	break;
	case EnemyType::BlueSlime:
	{
		m_enemyName = "Blue Slime";

		m_pVoxelCharacter->SetBreathingAnimationEnabled(false);
		m_pVoxelCharacter->SetWinkAnimationEnabled(false);
		m_pVoxelCharacter->SetRandomLookDirection(false);

		m_pVoxelCharacter->SetMeshAlpha(0.65f);

		m_projectileHitboxType = ProjectileHitboxType::Sphere;
		m_projectileHitboxRadius = 0.75f;

		m_jumpTime = 0.5f;
		m_jumpHeight = 10.0f;
		m_attackRadius = 1.15f;
		m_bodyTurnStopThreshold = 0.025f;
	}
	break;
	case EnemyType::YellowSlime:
	{
		m_enemyName = "Yellow Slime";

		m_pVoxelCharacter->SetBreathingAnimationEnabled(false);
		m_pVoxelCharacter->SetWinkAnimationEnabled(false);
		m_pVoxelCharacter->SetRandomLookDirection(false);

		m_pVoxelCharacter->SetMeshAlpha(0.65f);

		m_projectileHitboxType = ProjectileHitboxType::Sphere;
		m_projectileHitboxRadius = 0.75f;

		m_jumpTime = 0.5f;
		m_jumpHeight = 10.0f;
		m_attackRadius = 1.215f;
		m_bodyTurnStopThreshold = 0.025f;
	}
	break;

	case EnemyType::NormalSkeleton:
	{
		m_enemyName = "Skeleton";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.65f;
		m_projectileHitboxYLength = 1.25f;
		m_projectileHitboxZLength = 0.65f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.75f, 0.0f);

		m_bodyTurnStopThreshold = 0.025f;
		m_attackRadius = 0.25f;

		m_aggroResetTime = 15.0f;
	}
	break;
	case EnemyType::RangedSkeleton:
	{
		m_enemyName = "Skeleton Archer";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.65f;
		m_projectileHitboxYLength = 1.25f;
		m_projectileHitboxZLength = 0.65f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.75f, 0.0f);

		m_bodyTurnStopThreshold = 0.025f;
		m_attackRadius = 0.25f;

		m_aggroResetTime = 15.0f;

		// Add a quiver item to the enemy ranger
		VoxelWeapon* pNewEquipment = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
		pNewEquipment->SetVoxelCharacterParent(nullptr);
		pNewEquipment->LoadWeapon("Resources/gamedata/items/Quiver/Quiver.item", false);

		pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Quiver", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
		QubicleMatrix* pQuiverMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Quiver");
		pQuiverMatrix->m_boneIndex = m_pVoxelCharacter->GetBodyBoneIndex();
		m_pVoxelCharacter->AddQubicleMatrix(pQuiverMatrix, false);
	}
	break;
	case EnemyType::MeleeSkeleton:
	{
		m_enemyName = "Skeleton Knight";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.65f;
		m_projectileHitboxYLength = 1.25f;
		m_projectileHitboxZLength = 0.65f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.75f, 0.0f);
	}
	break;
	case EnemyType::MageSkeleton:
	{
		m_enemyName = "Skeleton Mage";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.65f;
		m_projectileHitboxYLength = 1.25f;
		m_projectileHitboxZLength = 0.65f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.75f, 0.0f);

		m_bodyTurnStopThreshold = 0.025f;
		m_attackRadius = 0.25f;

		m_aggroResetTime = 15.0f;

		m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "StaffPose");
	}
	break;

	case EnemyType::IronGiant:
	{
		m_enemyName = "Iron Giant";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 1.65f;
		m_projectileHitboxYLength = 3.25f;
		m_projectileHitboxZLength = 1.65f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 3.0f, 0.0f);

		m_bodyTurnSpeedMultiplier = 1.0f;
		m_bodyTurnStopThreshold = 0.005f;
		m_attackRadius = 0.0f;
		m_movementSpeed = 3.0f;
		m_aggroResetTime = 15.0f;

		m_radius = 3.0f;
	}
	break;

	case EnemyType::Mummy:
	{
		m_enemyName = "Mummy";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.65f;
		m_projectileHitboxYLength = 1.25f;
		m_projectileHitboxZLength = 0.65f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.75f, 0.0f);

		m_attackRadius = 0.0f;
		m_movementSpeed = 3.0f;
		m_aggroResetTime = 15.0f;
	}
	break;

	case EnemyType::WalkingZombie:
	{
		m_enemyName = "Zombie";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.65f;
		m_projectileHitboxYLength = 1.25f;
		m_projectileHitboxZLength = 0.65f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.75f, 0.0f);

		m_attackRadius = 0.0f;
		m_movementSpeed = 3.0f;
		m_aggroResetTime = 15.0f;
	}
	break;

	case EnemyType::CrawlingZombie:
	{
		m_enemyName = "Zombie";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.65f;
		m_projectileHitboxYLength = 0.8f;
		m_projectileHitboxZLength = 0.65f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.0f, 0.0f);

		m_attackRadius = 0.0f;
		m_movementSpeed = 1.0f;
		m_aggroResetTime = 15.0f;

		m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "ZombieCrawl");
	}
	break;

	case EnemyType::Bee:
	{
		m_enemyName = "Bee";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.6f;
		m_projectileHitboxYLength = 0.6f;
		m_projectileHitboxZLength = 0.95f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.0f, 0.0f);

		m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "Fly");
		m_attackRadius = 1.0f;
	}
	break;
	case EnemyType::Bat:
	{
		m_enemyName = "Bat";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.75f;
		m_projectileHitboxYLength = 0.5f;
		m_projectileHitboxZLength = 0.5f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.25f, 0.0f);

		m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "Fly");

		m_aggroResetTime = 15.0f;
	}
	break;
	case EnemyType::Ghost:
	{
		m_enemyName = "Ghost";

		m_pVoxelCharacter->SetMeshAlpha(0.65f);
		m_attackRadius = 0.65f;
	}
	break;
	case EnemyType::Doppelganger:
	{
		m_enemyName = "Ghostly Doppelganger";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.65f;
		m_projectileHitboxYLength = 1.25f;
		m_projectileHitboxZLength = 0.65f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.75f, 0.0f);

		m_pVoxelCharacter->SetMeshAlpha(0.65f);
		m_pVoxelCharacter->SetMeshSingleColor(1.0f, 1.0f, 1.0f);
		m_attackRadius = 0.65f;
	}
	break;

	case EnemyType::TargetDummy:
	{
		m_enemyName = "Target Dummy";

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.65f;
		m_projectileHitboxYLength = 1.6f;
		m_projectileHitboxZLength = 0.65f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 1.0f, 0.0f);

		m_pVoxelCharacter->SetBreathingAnimationEnabled(false);
		m_pVoxelCharacter->SetWinkAnimationEnabled(false);
		m_pVoxelCharacter->SetRandomLookDirection(false);
	}
	break;

	case EnemyType::Mimic:
	{
		m_enemyName = "Mimic";

		m_pVoxelCharacter->SetBreathingAnimationEnabled(false);
		m_pVoxelCharacter->SetWinkAnimationEnabled(false);
		m_pVoxelCharacter->SetRandomLookDirection(false);

		m_projectileHitboxType = ProjectileHitboxType::Cube;
		m_projectileHitboxXLength = 0.8f;
		m_projectileHitboxYLength = 0.5f;
		m_projectileHitboxZLength = 0.5f;
		m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.0f, 0.0f);

		m_bodyTurnSpeedMultiplier = 10.0f;
		m_bodyTurnStopThreshold = 0.005f;
		m_movementSpeed = 8.0f;
		m_jumpTime = 0.5f;
		m_jumpHeight = 10.0f;
		m_attackRadius = 0.5f;
	}
	break;

	case EnemyType::SnowGolem:
	{
		m_enemyName = "Snow Golem";

	}
	break;
	case EnemyType::MagmaGolem:
	{
		m_enemyName = "Magma Golem";

	}
	break;
	}
}

void Enemy::LoadWeapon(bool left, std::string weaponFile)
{
	if (left)
	{
		if (m_pVoxelCharacter->GetLeftWeapon() != nullptr)
		{
			if (m_pVoxelCharacter->IsLeftWeaponLoaded())
			{
				// Lights
				for (int i = 0; i < m_pVoxelCharacter->GetLeftWeapon()->GetNumLights(); ++i)
				{
					unsigned int lightID;
					glm::vec3 lightPos;
					float lightRadius;
					float lightDiffuseMultiplier;
					Color lightColor;
					bool connectedToSegment;

					m_pVoxelCharacter->GetLeftWeapon()->GetLightParams(i, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

					if (lightID != -1)
					{
						m_pLightingManager->RemoveLight(lightID);
						m_pVoxelCharacter->GetLeftWeapon()->SetLightingID(i, -1);
					}
				}

				// Particle effects
				for (int i = 0; i < m_pVoxelCharacter->GetLeftWeapon()->GetNumParticleEffects(); ++i)
				{
					unsigned int particleEffectID;
					glm::vec3 particleEffectPos;
					std::string effectName;
					bool connectedToSegment;

					m_pVoxelCharacter->GetLeftWeapon()->GetParticleEffectParams(i, &particleEffectID, &particleEffectPos, &effectName, &connectedToSegment);

					if (particleEffectID != -1)
					{
						m_pBlockParticleManager->DestroyParticleEffect(particleEffectID);
						m_pVoxelCharacter->GetLeftWeapon()->SetParticleEffectID(i, -1);
					}
				}
			}

			// Load the weapon file
			m_pVoxelCharacter->LoadLeftWeapon(weaponFile.c_str());
		}
	}
	else
	{
		if (m_pVoxelCharacter->GetRightWeapon() != nullptr)
		{
			if (m_pVoxelCharacter->IsRightWeaponLoaded())
			{
				// Lights
				for (int i = 0; i < m_pVoxelCharacter->GetRightWeapon()->GetNumLights(); ++i)
				{
					unsigned int lightID;
					glm::vec3 lightPos;
					float lightRadius;
					float lightDiffuseMultiplier;
					Color lightColor;
					bool connectedToSegment;

					m_pVoxelCharacter->GetRightWeapon()->GetLightParams(i, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

					if (lightID != -1)
					{
						m_pLightingManager->RemoveLight(lightID);
						m_pVoxelCharacter->GetRightWeapon()->SetLightingID(i, -1);
					}
				}

				// Particle effects
				for (int i = 0; i < m_pVoxelCharacter->GetRightWeapon()->GetNumParticleEffects(); ++i)
				{
					unsigned int particleEffectID;
					glm::vec3 particleEffectPos;
					std::string effectName;
					bool connectedToSegment;

					m_pVoxelCharacter->GetRightWeapon()->GetParticleEffectParams(i, &particleEffectID, &particleEffectPos, &effectName, &connectedToSegment);

					if (particleEffectID != -1)
					{
						m_pBlockParticleManager->DestroyParticleEffect(particleEffectID);
						m_pVoxelCharacter->GetRightWeapon()->SetParticleEffectID(i, -1);
					}
				}
			}

			// Load the weapon file
			m_pVoxelCharacter->LoadRightWeapon(weaponFile.c_str());

			// Weapon attack radius
			if (m_pVoxelCharacter->GetRightWeapon() != nullptr)
			{
				if (m_pVoxelCharacter->IsRightWeaponLoaded())
				{
					m_attackRadius = m_pVoxelCharacter->GetRightWeapon()->GetWeaponRadius();
				}
			}
		}
	}
}

void Enemy::UnloadWeapon(bool left) const
{
	VoxelWeapon* pWeapon;
	bool isWeaponLoaded;

	if (left)  // Left side
	{
		pWeapon = m_pVoxelCharacter->GetLeftWeapon();
		isWeaponLoaded = m_pVoxelCharacter->IsLeftWeaponLoaded();
	}
	else  // Right side
	{
		pWeapon = m_pVoxelCharacter->GetRightWeapon();
		isWeaponLoaded = m_pVoxelCharacter->IsRightWeaponLoaded();
	}

	if (pWeapon != nullptr)
	{
		if (isWeaponLoaded)
		{
			// Lights
			for (int i = 0; i < pWeapon->GetNumLights(); ++i)
			{
				unsigned int lightID;
				glm::vec3 lightPos;
				float lightRadius;
				float lightDiffuseMultiplier;
				Color lightColor;
				bool connectedToSegment;

				pWeapon->GetLightParams(i, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

				if (lightID != -1)
				{
					m_pLightingManager->RemoveLight(lightID);
					pWeapon->SetLightingID(i, -1);

					if (connectedToSegment == false)
					{
						// Rotate due to characters forward vector
						float rotationAngle = acos(dot(glm::vec3(0.0f, 0.0f, 1.0f), m_forward));
						if (m_forward.x < 0.0f)
						{
							rotationAngle = -rotationAngle;
						}

						Matrix4 rotationMatrix;
						rotationMatrix.SetRotation(0.0f, rotationAngle, 0.0f);
						lightPos = rotationMatrix * lightPos;

						// Translate to position
						lightPos += m_position;
					}

					float scale = m_pVoxelCharacter->GetCharacterScale();
					unsigned int ID;
					m_pLightingManager->AddDyingLight(glm::vec3(lightPos.x, lightPos.y, lightPos.z), lightRadius * scale, lightDiffuseMultiplier, lightColor, 2.0f, &ID);
				}
			}

			// Particle Effects
			for (int i = 0; i < pWeapon->GetNumParticleEffects(); ++i)
			{
				unsigned int particleEffectID;
				glm::vec3 particleEffectPos;
				std::string effectName;
				bool connectedToSegment;

				pWeapon->GetParticleEffectParams(i, &particleEffectID, &particleEffectPos, &effectName, &connectedToSegment);

				if (particleEffectID != -1)
				{
					m_pBlockParticleManager->DestroyParticleEffect(particleEffectID);
					pWeapon->SetParticleEffectID(i, -1);
				}
			}
		}

		pWeapon->UnloadWeapon();

		if (left)  // Left side
		{
			m_pVoxelCharacter->UnloadLeftWeapon();
		}
		else  // Right side
		{
			m_pVoxelCharacter->UnloadRightWeapon();
		}
	}
}

// Enemy spawner
void Enemy::SetEnemySpawner(EnemySpawner* pSpawner)
{
	m_pParentEnemySpawner = pSpawner;
}

void Enemy::RemoveEnemySpawner(EnemySpawner* pSpawner)
{
	if (m_pParentEnemySpawner == pSpawner)
	{
		m_pParentEnemySpawner = nullptr;
	}
}

// Animation
void Enemy::SetAnimationSpeed(float speed, bool onlySetOnCompleteAnimation, AnimationSections section)
{
	if (m_canAttack == false)
	{
		return;
	}

	if (section == AnimationSections::FullBody)
	{
		for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
		{
			if (m_animationSpeedonlySetOnCompleteAnimation[i] == false || m_animationFinished[i] == true)
			{
				m_animationSpeed[i] = speed;
				m_animationSpeedonlySetOnCompleteAnimation[i] = false;
			}
		}
	}
	else
	{
		m_animationSpeed[static_cast<int>(section)] = speed;
		m_animationSpeedonlySetOnCompleteAnimation[static_cast<int>(section)] = onlySetOnCompleteAnimation;
	}
}

float Enemy::GetAnimationSpeed(AnimationSections section)
{
	return m_animationSpeed[static_cast<int>(section)];
}

bool Enemy::AnimationFinished(AnimationSections section)
{
	return m_animationFinished[static_cast<int>(section)];
}

// Looking
void Enemy::SetTargetForwardToLookAtPoint(glm::vec3 point)
{
	glm::vec3 toPoint = point - GetCenter();
	toPoint.y = 0.0f;
	toPoint = normalize(toPoint);

	m_targetForward = toPoint;
}

void Enemy::LookAtPoint(glm::vec3 point)
{
	m_lookAtPoint = true;
	m_lookPoint = point;
}

void Enemy::LookAtRandomDirection()
{
	m_forward = glm::vec3(GetRandomNumber(-1, 1, 2), 0.0f, GetRandomNumber(-1, 1, 2));
	m_forward = normalize(m_forward);

	m_lookAtPoint = false;
	m_targetForward = m_forward;
}

void Enemy::SetRandomLookMode()
{
	m_pVoxelCharacter->SetFaceTargetDirection(m_pVoxelCharacter->GetFaceLookingDirection());
	m_pVoxelCharacter->SetRandomLookDirection(true);
	m_pVoxelCharacter->SetFaceLookToTargetSpeedMultiplier(1.0f);
	m_lookAtPoint = false;
}

void Enemy::SetBodyTurnStopThreshold(float threshold)
{
	m_bodyTurnStopThreshold = threshold;
}

void Enemy::SetBodyTurnSpeedMultiplier(float multiplier)
{
	m_bodyTurnSpeedMultiplier = multiplier;
}

// World
void Enemy::UpdateGridPosition()
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

Chunk* Enemy::GetCachedGridChunkOrFromPosition(glm::vec3 pos) const
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

void Enemy::ClearChunkCacheForChunk(Chunk* pChunk)
{
	if (m_pCachedGridChunk == pChunk)
	{
		m_pCachedGridChunk = nullptr;

		// Erase if the chunk we belonged to is unloaded
		SetErase(true);
	}
}

// Collision
bool Enemy::CheckCollisions(glm::vec3 positionCheck, glm::vec3 previousPosition, glm::vec3 *pNormal, glm::vec3 *pMovement) const
{
	float radius = GetRadius();

	glm::vec3 movementCache = *pMovement;

	// World collisions
	bool worldCollision = false;

	glm::vec3 floorPosition;
	if (m_pChunkManager->FindClosestFloor(positionCheck, &floorPosition) == false)
	{
		*pMovement = glm::vec3(0.0f, 0.0f, 0.0f);
		return true;
	}
	
	int blockX, blockY, blockZ;
	glm::vec3 blockPos;
	int numChecks = 1 + static_cast<int>(radius / (Chunk::BLOCK_RENDER_SIZE * 2.0f));

	for (int x = -numChecks; x <= numChecks; ++x)
	{
		for (int y = -numChecks; y <= numChecks; ++y)
		{
			for (int z = -numChecks; z <= numChecks; ++z)
			{
				*pNormal = glm::vec3(0.0f, 0.0f, 0.0f);

				Chunk* pChunk = GetCachedGridChunkOrFromPosition(positionCheck + glm::vec3((Chunk::BLOCK_RENDER_SIZE * 2.0f) * x, (Chunk::BLOCK_RENDER_SIZE * 2.0f) * y, (Chunk::BLOCK_RENDER_SIZE * 2.0f) * z));
				bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(positionCheck.x + ((Chunk::BLOCK_RENDER_SIZE * 2.0f) * x), positionCheck.y + ((Chunk::BLOCK_RENDER_SIZE * 2.0f) * y), positionCheck.z + ((Chunk::BLOCK_RENDER_SIZE * 2.0f) * z), &blockPos, &blockX, &blockY, &blockZ, &pChunk);

				if (active == false)
				{
					if (pChunk == nullptr || pChunk->IsSetup() == false)
					{
						*pMovement = glm::vec3(0.0f, 0.0f, 0.0f);
						worldCollision = false;
					}
				}
				else if (active == true)
				{
					Plane3D planes[6];
					planes[0] = Plane3D(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(Chunk::BLOCK_RENDER_SIZE, 0.0f, 0.0f));
					planes[1] = Plane3D(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-Chunk::BLOCK_RENDER_SIZE, 0.0f, 0.0f));
					planes[2] = Plane3D(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, Chunk::BLOCK_RENDER_SIZE, 0.0f));
					planes[3] = Plane3D(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -Chunk::BLOCK_RENDER_SIZE, 0.0f));
					planes[4] = Plane3D(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, Chunk::BLOCK_RENDER_SIZE));
					planes[5] = Plane3D(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -Chunk::BLOCK_RENDER_SIZE));

					float distance;
					int inside = 0;
					bool insideCache[6];

					for (int i = 0; i < 6; ++i)
					{
						glm::vec3 pointToCheck = blockPos - previousPosition;
						distance = planes[i].GetPointDistance(pointToCheck);

						if (distance < -radius)
						{
							// Outside...
							insideCache[i] = false;
						}
						else if (distance < radius)
						{
							// Intersecting..
							insideCache[i] = true;
						}
						else
						{
							// Inside...
							insideCache[i] = true;
						}
					}

					for (int i = 0; i < 6; ++i)
					{
						glm::vec3 pointToCheck = blockPos - positionCheck;
						distance = planes[i].GetPointDistance(pointToCheck);

						if (distance < -radius)
						{
							// Outside...
						}
						else if (distance < radius)
						{
							// Intersecting..
							inside++;
							if (insideCache[i] == false)
							{
								*pNormal += planes[i].m_normal;
							}
						}
						else
						{
							// Inside...
							inside++;
							if (insideCache[i] == false)
							{
								*pNormal += planes[i].m_normal;
							}
						}
					}

					if (inside == 6)
					{
						if (length(*pNormal) <= 1.0f)
						{
							if (length(*pNormal) > 0.0f)
							{
								*pNormal = normalize(*pNormal);
							}

							float dotResult = dot(*pNormal, *pMovement);
							*pNormal *= dotResult;

							*pMovement -= *pNormal;

							worldCollision = true;
						}
					}
				}
			}
		}
	}

	if (worldCollision)
	{
		return true;
	}

	*pMovement = movementCache;

	return false;
}

bool Enemy::IsBlockInFront() const
{
	glm::vec3 blockPosCheck;
	int blockXCheck, blockYCheck, blockZCheck;

	// Straight in front
	glm::vec3 checkPos = GetCenter() + normalize(m_forward) * (m_radius*2.0f);
	Chunk* pChunk = GetCachedGridChunkOrFromPosition(checkPos);
	bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(checkPos.x, checkPos.y, checkPos.z, &blockPosCheck, &blockXCheck, &blockYCheck, &blockZCheck, &pChunk);
	if (active == true)
	{
		return true;
	}

	// To the left side
	checkPos = GetCenter() + normalize(m_forward - GetRightVector()) * (m_radius*2.0f);
	pChunk = GetCachedGridChunkOrFromPosition(checkPos);
	active = m_pChunkManager->GetBlockActiveFrom3DPosition(checkPos.x, checkPos.y, checkPos.z, &blockPosCheck, &blockXCheck, &blockYCheck, &blockZCheck, &pChunk);
	if (active == true)
	{
		return true;
	}

	// To the right side
	checkPos = GetCenter() + normalize(m_forward + GetRightVector()) * (m_radius*2.0f);
	pChunk = GetCachedGridChunkOrFromPosition(checkPos);
	active = m_pChunkManager->GetBlockActiveFrom3DPosition(checkPos.x, checkPos.y, checkPos.z, &blockPosCheck, &blockXCheck, &blockYCheck, &blockZCheck, &pChunk);
	if (active == true)
	{
		return true;
	}

	return false;
}

// Movement
void Enemy::MoveAbsolute(glm::vec3 direction, const float speed, bool shouldChangeForward)
{
	if (shouldChangeForward)
	{
		float speedToUse = speed;
		glm::vec3 diff = normalize(m_forward) - normalize(direction);
		if (dot(normalize(m_forward), normalize(direction)) < 0.1f)
		{
			speedToUse *= 1.75f;
		}
		m_forward -= (diff * speedToUse);
		m_forward = normalize(m_forward);
	}

	glm::vec3 movement = direction;
	glm::vec3 movementAmount = direction*speed;
	int numberDivision = 1;

	while (length(movementAmount) >= Chunk::BLOCK_RENDER_SIZE)
	{
		numberDivision++;
		movementAmount = direction*(speed / numberDivision);
	}

	for (int i = 0; i < numberDivision; ++i)
	{
		float speedToUse = (speed / numberDivision) + ((speed / numberDivision) * i);
		m_position += (movement * speedToUse) * 0.95f;
	}

	if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat)
	{
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "Fly", 0.01f);
	}
	else if (m_enemyType == EnemyType::GreenSlime || m_enemyType == EnemyType::RedSlime || m_enemyType == EnemyType::BlueSlime || m_enemyType == EnemyType::YellowSlime)
	{
		// TODO: Blend animation
	}
	else if (m_enemyType == EnemyType::Mimic)
	{
		float RandomChompTime = 0.025f + (GetRandomNumber(0, 3, 2) * 0.05f);
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "MimicChomp", RandomChompTime);
	}

	if (m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
	{
		SetAnimationSpeed(0.35f, true, AnimationSections::FullBody);
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "GhostPose", 0.01f);
	}
	else if (m_enemyType == EnemyType::CrawlingZombie)
	{
		SetAnimationSpeed(0.35f, true, AnimationSections::FullBody);
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "ZombieCrawl", 0.015f);
	}
	else if (m_enemyType == EnemyType::Mummy || m_enemyType == EnemyType::WalkingZombie)
	{
		if (m_canJump)
		{
			SetAnimationSpeed(0.5f, true, AnimationSections::FullBody);
		}

		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "MummyWalk", 0.015f);
	}
	else if (m_enemyType == EnemyType::MageSkeleton)
	{
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::LegsFeet, "StaffRun", 0.01f);
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, true, AnimationSections::RightArmHand, "StaffRun", 0.01f);
	}
	else
	{
		if (CanAttack())
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::LegsFeet, "Run", 0.01f);
		}
		else if (m_isIdle)
		{
			if (m_canInteruptCombatAnim)
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "Run", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "Run", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "Run", 0.01f);
			}
		}
	}


	m_isIdle = false;
}

void Enemy::StopMoving()
{
	if (m_canJump)
	{
		if (m_isIdle == false)
		{
			m_isIdle = true;

			if (m_enemyType != EnemyType::Mimic)
			{
				SetRandomLookMode();
			}
		}

		if (m_hasSetIdleAnimation == false)
		{
			if (m_enemyType != EnemyType::Bee && m_enemyType != EnemyType::Bat && m_enemyType != EnemyType::Ghost && m_enemyType != EnemyType::Doppelganger && m_enemyType != EnemyType::CrawlingZombie)
			{
				if (m_waitBeforeStopMovingAnimationTimer <= 0.0f)
				{
					if (m_enemyType != EnemyType::Mimic)
					{
						if (m_enemyType == EnemyType::MageSkeleton)
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "StaffPose", 0.15f);
						}
						else
						{
							if (CanAttack())
							{
								m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "BindPose", 0.15f);
							}
							if (m_canInteruptCombatAnim)
							{
								m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "BindPose", 0.15f);
								m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "BindPose", 0.15f);
								m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "BindPose", 0.15f);
							}
						}
					}

					m_hasSetIdleAnimation = true;
				}
			}
		}
	}
}

void Enemy::Jump(bool jumpUpBlock)
{
	if (m_canJump == false)
	{
		return;
	}

	if (m_jumpTimer >= 0.0f)
	{
		return;
	}

	if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
	{
		return; // Flying enemies can't jump
	}

	m_canJump = false;
	m_jumpTimer = m_jumpTime;

	m_velocity += glm::vec3(0.0f, m_jumpHeight, 0.0f);

	if (m_enemyType == EnemyType::GreenSlime || m_enemyType == EnemyType::RedSlime || m_enemyType == EnemyType::BlueSlime || m_enemyType == EnemyType::YellowSlime || m_enemyType == EnemyType::Mimic)
	{
		if (jumpUpBlock)
		{
			// Add a little extra bounce for jumping enemies to get up blocks
			m_velocity += glm::vec3(0.0f, 4.5f, 0.0f);
		}
	}

	if (m_enemyType != EnemyType::Bee && m_enemyType != EnemyType::Bat && m_enemyType != EnemyType::Ghost && m_enemyType != EnemyType::Doppelganger && m_enemyType != EnemyType::Mimic && m_enemyType != EnemyType::GreenSlime && m_enemyType != EnemyType::RedSlime && m_enemyType != EnemyType::BlueSlime && m_enemyType != EnemyType::YellowSlime)
	{
		if (m_enemyType == EnemyType::Mummy || m_enemyType == EnemyType::WalkingZombie || m_enemyType == EnemyType::CrawlingZombie)
		{
			SetAnimationSpeed(1.0f, false, AnimationSections::LegsFeet);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "Jump", 0.01f);
		}
		else
		{
			if (CanAttack())
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "Jump", 0.01f);
			}

			if (m_canInteruptCombatAnim)
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "Jump", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "Jump", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "Jump", 0.01f);
			}
		}
	}

	if (m_enemyType == EnemyType::Mimic)
	{
		CubbyGame::GetInstance()->PlaySoundEffect3D(SoundEffect::MimicJump, GetCenter());
	}
}

bool Enemy::CanJump() const
{
	return m_canJump;
}

bool Enemy::IsMoving() const
{
	if (m_enemyType == EnemyType::GreenSlime || m_enemyType == EnemyType::RedSlime)
	{
		return false;
	}

	return m_isIdle == false;
}

float Enemy::GetMovementSpeed() const
{
	return m_movementSpeed;
}

bool Enemy::CanMoveTowardsTarget() const
{
	if (m_enemyType == EnemyType::TargetDummy)
	{
		return false;
	}

	if (m_enemyType == EnemyType::Mimic)
	{
		// Mimic can only move towards the target position if it is in aggro, else stay still!
		if (m_aggro)
		{
			return true;
		}

		return false;
	}

	if (m_enemyType == EnemyType::NormalSkeleton || m_enemyType == EnemyType::RangedSkeleton || m_enemyType == EnemyType::MageSkeleton || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::IronGiant)
	{
		if (m_aggro)
		{
			// Stand still when attacking for ranged characters
			return false;
		}

		if (m_randomTargetPositionMode == false)
		{
			return false;
		}
	}

	return true;
}

// Combat
void Enemy::DoDamage(float amount, Color textColor, glm::vec3 knockbackDirection, float knockbackAmount, bool createParticleHit, bool shouldUpdateGUI)
{
	if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
	{
		knockbackAmount *= 0.5f;
	}

	if (m_enemyType == EnemyType::TargetDummy)
	{
		knockbackAmount = 0.0f;
	}

	if (m_enemyType == EnemyType::IronGiant)
	{
		knockbackAmount = 0.0f;
	}

	bool finalDeathHit = false;

	// Remove sapped
	SetSapped(false);

	if (m_damageTimer <= 0.0f)
	{
		if (m_aggro == false)
		{
			// If we are setting aggro for initial setting, reset and extend the attack delay timer
			m_attackDelayTimer = m_attackDelayTime*2.0f;
		}

		if (m_enemyType != EnemyType::TargetDummy)
		{
			m_aggro = true;
			m_aggroResetTimer = m_aggroResetTime;
		}

		float healthBefore = m_health;
		float damageDone = healthBefore - m_health;

		if (m_enemyType != EnemyType::TargetDummy)
		{
			m_health -= amount;
		}

		// Figure out if we are dead yet
		if (m_health <= 0.0f)
		{
			m_health = 0.0f;

			finalDeathHit = true;

			if (m_enemyType != EnemyType::WalkingZombie)
			{
				// Spawn loot items
				SpawnLootItems();

				// If we are the players target enemy, reset players target
				if (m_pPlayer->GetTargetEnemy() == this)
				{
					CubbyGame::GetInstance()->ReleaseEnemyTarget();
				}
			}
			else
			{
				// Spawn gibs
				SpawnGibs();
			}

			// Explode the voxel model
			Explode();
		}

		// Update the HUD
		if (shouldUpdateGUI)
		{
			m_pHUD->SetEnemyHealth(m_health, m_maxHealth, damageDone);
			m_pHUD->SetEnemyName(m_enemyName.c_str());
		}

		if (finalDeathHit == false)
		{
			// Play a hit response animation
			if (m_enemyType == EnemyType::NormalSkeleton || m_enemyType == EnemyType::RangedSkeleton || m_enemyType == EnemyType::MeleeSkeleton || m_enemyType == EnemyType::MageSkeleton || m_enemyType == EnemyType::Mummy || m_enemyType == EnemyType::WalkingZombie)
			{
				if (m_isChargingAttack == false)
				{
					SetAnimationSpeed(1.0f, false, AnimationSections::FullBody);
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "HitResponse", 0.01f);
				}
			}
		}

		// Do an animated text effect
		glm::vec3 screenPosition = GetCenter() + glm::vec3(GetRandomNumber(-1, 1, 2) * 0.25f, 0.0f, GetRandomNumber(-1, 1, 2) * 0.25f);
		char damageText[32];
		sprintf(damageText, "%i", static_cast<int>(amount));
		AnimatedText* pTestTextEffect = m_pTextEffectsManager->CreateTextEffect(CubbyGame::GetInstance()->GetFrontendManager()->GetTextEffectFont(), CubbyGame::GetInstance()->GetFrontendManager()->GetTextEffectOutlineFont(), CubbyGame::GetInstance()->GetDefaultViewport(), TextDrawMode::Screen3D, TextEffect::FadeUp, TextDrawStyle::Outline, screenPosition, textColor, Color(0.0f, 0.0f, 0.0f), damageText, 1.0f);
		pTestTextEffect->SetAutoDelete(true);
		pTestTextEffect->StartEffect();

		if (createParticleHit && m_health > 0.0f)
		{
			// Do a hit particle effect
			glm::vec3 hitParticlePos = GetCenter() - (normalize(knockbackDirection) * m_radius);
			unsigned int effectID = -1;
			BlockParticleEffect* pBlockParticleEffect = CubbyGame::GetInstance()->GetBlockParticleManager()->ImportParticleEffect("Resources/gamedata/particles/combat_hit.effect", hitParticlePos, &effectID);
			pBlockParticleEffect->PlayEffect();
		}

		m_damageTimer = m_damageTime;

		m_movementFreezeTimer = 0.35f;
	}

	if (m_knockbackTimer <= 0.0f)
	{
		if (m_spawning == false)
		{
			m_velocity += knockbackDirection * knockbackAmount;
		}

		m_knockbackTimer = m_knockbackTime;
	}
}

void Enemy::CheckPlayerDamageRadius()
{
	if (m_pPlayer->GetAttackEnabled() == false)
	{
		// If attack is not enabled, just return
		return;
	}

	glm::vec3 distance = GetCenter() - m_pPlayer->GetCenter();
	float lengthToPlayer = length(distance);

	if (lengthToPlayer <= m_radius + m_pPlayer->GetAttackRadius())
	{
		glm::vec3 distanceMinusY = distance;
		distanceMinusY.y = 0.0f;
		glm::vec3 direction = normalize(distanceMinusY);
		direction = normalize(direction);

		// Figure out the attack vector, based on the attack rotation
		float playerRotation = m_pPlayer->GetRotation();
		float attackRotation = m_pPlayer->GetAttackRotation();
		float angle = DegreeToRadian(playerRotation + attackRotation);
		glm::vec3 attackDirection = glm::vec3(sin(angle), 0.0f, cos(angle));
		float dotProduct = dot(direction, attackDirection);

		if (dotProduct > m_pPlayer->GetAttackSegmentAngle()) // Check if we are within the attack segment
		{
			glm::vec3 knockbackDirection;
			if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
			{
				knockbackDirection = direction * 2.0f;
			}
			else
			{
				knockbackDirection = direction * 2.0f + glm::vec3(0.0f, 1.0f, 0.0f);
			}

			knockbackDirection = normalize(knockbackDirection);
			Color damageColor = Color(1.0f, 1.0f, 1.0f);

			float knockbackAmount = 16.0f;			
			if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
			{
				knockbackAmount = 1.0f;
			}

			DoDamage(20.0f, damageColor, knockbackDirection, knockbackAmount, true, true);
		}
	}
}

void Enemy::CheckNPCDamageRadius()
{
	int numNPCs = m_pNPCManager->GetNumNPCs();

	for (int i = 0; i < numNPCs; ++i)
	{
		NPC *pNPC = m_pNPCManager->GetNPC(i);

		if (pNPC->NeedErase() == true)
		{
			continue;
		}

		if (pNPC->GetAttackEnabled() == false)
		{
			continue;
		}

		glm::vec3 distance = GetCenter() - pNPC->GetCenter();
		float lengthToNPC = length(distance);

		if (lengthToNPC <= m_radius + pNPC->GetAttackRadius())
		{
			glm::vec3 distanceMinusY = distance;
			distanceMinusY.y = 0.0f;
			glm::vec3 direction = normalize(distanceMinusY);
			direction = normalize(direction);

			// Figure out the attack vector, based on the attack rotation
			float NPCRotation = pNPC->GetRotation();
			float attackRotation = pNPC->GetAttackRotation();
			float angle = DegreeToRadian(NPCRotation + attackRotation);
			glm::vec3 attackDirection = glm::vec3(sin(angle), 0.0f, cos(angle));
			float dotProduct = dot(direction, attackDirection);

			if (dotProduct > pNPC->GetAttackSegmentAngle()) // Check if we are within the attack segment
			{
				glm::vec3 knockbackDirection;
				if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
				{
					knockbackDirection = (direction*2.0f);
				}
				else
				{
					knockbackDirection = (direction*2.0f) + glm::vec3(0.0f, 1.0f, 0.0f);
				}

				knockbackDirection = normalize(knockbackDirection);
				Color damageColor = Color(1.0f, 1.0f, 1.0f);

				// Set NPC target as attacker
				SetTargetNPC(pNPC);

				float knockbackAmount = 16.0f;
				if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
				{
					knockbackAmount = 1.0f;
				}

				DoDamage(20.0f, damageColor, knockbackDirection, knockbackAmount, true, false);
			}
		}
	}
}

void Enemy::CheckProjectileDamageRadius(Projectile* pProjectile)
{
	if (pProjectile->CanAttackEnemies() == false)
	{
		return;
	}

	glm::vec3 projectileHitboxCenter = GetProjectileHitboxCenter();

	glm::vec3 toProjectile = projectileHitboxCenter - pProjectile->GetCenter();

	bool hitByProjectile = false;
	if (m_projectileHitboxType == ProjectileHitboxType::Sphere)
	{
		if (length(toProjectile) < GetRadius() + pProjectile->GetRadius())
		{
			hitByProjectile = true;
		}
	}
	else if (m_projectileHitboxType == ProjectileHitboxType::Cube)
	{
		Plane3D planes[6];
		Matrix4 rotationMatrix;
		rotationMatrix.SetYRotation(DegreeToRadian(GetRotation()));

		planes[0] = Plane3D(rotationMatrix * glm::vec3(-1.0f, 0.0f, 0.0f), projectileHitboxCenter + (rotationMatrix * glm::vec3(m_projectileHitboxXLength, 0.0f, 0.0f)));
		planes[1] = Plane3D(rotationMatrix * glm::vec3(1.0f, 0.0f, 0.0f), projectileHitboxCenter + (rotationMatrix * glm::vec3(-m_projectileHitboxXLength, 0.0f, 0.0f)));
		planes[2] = Plane3D(rotationMatrix * glm::vec3(0.0f, -1.0f, 0.0f), projectileHitboxCenter + (rotationMatrix * glm::vec3(0.0f, m_projectileHitboxYLength, 0.0f)));
		planes[3] = Plane3D(rotationMatrix * glm::vec3(0.0f, 1.0f, 0.0f), projectileHitboxCenter + (rotationMatrix * glm::vec3(0.0f, -m_projectileHitboxYLength, 0.0f)));
		planes[4] = Plane3D(rotationMatrix * glm::vec3(0.0f, 0.0f, -1.0f), projectileHitboxCenter + (rotationMatrix * glm::vec3(0.0f, 0.0f, m_projectileHitboxZLength)));
		planes[5] = Plane3D(rotationMatrix * glm::vec3(0.0f, 0.0f, 1.0f), projectileHitboxCenter + (rotationMatrix * glm::vec3(0.0f, 0.0f, -m_projectileHitboxZLength)));

		float distance;
		int inside = 0;

		for (int i = 0; i < 6; ++i)
		{
			distance = planes[i].GetPointDistance(pProjectile->GetCenter());

			if (distance < -pProjectile->GetRadius())
			{
				// Outside...
			}
			else if (distance < pProjectile->GetRadius())
			{
				// Intersecting..
				inside++;
			}
			else
			{
				// Inside...
				inside++;
			}
		}

		if (inside == 6)
		{
			hitByProjectile = true;
		}
	}

	if (hitByProjectile)
	{
		glm::vec3 knockbackDirection;
		if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
		{
			knockbackDirection = (normalize(pProjectile->GetVelocity()) * 2.0f);
		}
		else
		{
			knockbackDirection = (normalize(pProjectile->GetVelocity()) * 2.0f) + glm::vec3(0.0f, 1.0f, 0.0f);
		}

		knockbackDirection = normalize(knockbackDirection);
		Color damageColor = Color(1.0f, 1.0f, 1.0f);

		// Set NPC target as attacker, if they owned the projectile
		bool playerDamage = (pProjectile->GetPlayerOwner() != nullptr);
		if (pProjectile->GetNPCOwner() != nullptr)
		{
			SetTargetNPC(pProjectile->GetNPCOwner());
		}

		float knockbackAmount = 16.0f;
		if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
		{
			knockbackAmount = 1.0f;
		}

		DoDamage(15.0f, damageColor, knockbackDirection, knockbackAmount, false, playerDamage);

		if (pProjectile->IsReturnToPlayer() == false)
		{
			pProjectile->Explode();
		}
	}
}

void Enemy::Explode()
{
	if (m_erase == true)
	{
		return;
	}

	CalculateWorldTransformMatrix();

	if (m_enemyType == EnemyType::WalkingZombie)
	{
		QubicleBinary* pQubicleModel = m_pVoxelCharacter->GetQubicleModel();

		int legsIndex = pQubicleModel->GetMatrixIndexForName("Legs");
		QubicleMatrix* pLegsMatrix = pQubicleModel->GetQubicleMatrix(legsIndex);
		m_pBlockParticleManager->ExplodeQubicleMatrix(pLegsMatrix, m_pVoxelCharacter->GetCharacterScale(), 100, false, 1.0f, 1.0f, 1.0f, 1.0f);

		int leftFootIndex = pQubicleModel->GetMatrixIndexForName("Left_Foot");
		QubicleMatrix* pLeftFootMatrix = pQubicleModel->GetQubicleMatrix(leftFootIndex);
		m_pBlockParticleManager->ExplodeQubicleMatrix(pLeftFootMatrix, m_pVoxelCharacter->GetCharacterScale(), 100, false, 1.0f, 1.0f, 1.0f, 1.0f);

		int rightFootIndex = pQubicleModel->GetMatrixIndexForName("Right_Foot");
		QubicleMatrix* pRightFootMatrix = pQubicleModel->GetQubicleMatrix(rightFootIndex);
		m_pBlockParticleManager->ExplodeQubicleMatrix(pRightFootMatrix, m_pVoxelCharacter->GetCharacterScale(), 100, false, 1.0f, 1.0f, 1.0f, 1.0f);

		ConvertIntoOtherEnemyType(EnemyType::CrawlingZombie, 0.08f);

		m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose");
		SetAnimationSpeed(0.35f, true, AnimationSections::FullBody);
		m_pVoxelCharacter->Update(0.01f, m_animationSpeed);
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "ZombieCrawl", 0.15f);

		m_movementFreezeTimer = 0.5f;

		return;
	}

	for (int explodeCounter = 0; explodeCounter < 3; ++explodeCounter)
	{
		QubicleBinary* pQubicleModel = nullptr;

		if (explodeCounter == 0)
		{
			pQubicleModel = m_pVoxelCharacter->GetQubicleModel();
		}
		else if (explodeCounter == 1)
		{
			if (m_pVoxelCharacter->GetRightWeapon() != nullptr)
			{
				if (m_pVoxelCharacter->IsRightWeaponLoaded())
				{
					for (int animatedSectionsIndex = 0; animatedSectionsIndex < m_pVoxelCharacter->GetRightWeapon()->GetNumAimatedSections(); ++animatedSectionsIndex)
					{
						AnimatedSection* pAnimatedSection = m_pVoxelCharacter->GetRightWeapon()->GetAnimatedSection(animatedSectionsIndex);
						pQubicleModel = pAnimatedSection->pVoxelObject->GetQubicleModel();
					}
				}
			}
		}
		else if (explodeCounter == 2)
		{
			if (m_pVoxelCharacter->GetLeftWeapon() != nullptr)
			{
				if (m_pVoxelCharacter->IsLeftWeaponLoaded())
				{
					for (int animatedSectionsIndex = 0; animatedSectionsIndex < m_pVoxelCharacter->GetLeftWeapon()->GetNumAimatedSections(); ++animatedSectionsIndex)
					{
						AnimatedSection* pAnimatedSection = m_pVoxelCharacter->GetLeftWeapon()->GetAnimatedSection(animatedSectionsIndex);
						pQubicleModel = pAnimatedSection->pVoxelObject->GetQubicleModel();
					}
				}
			}
		}

		if (pQubicleModel != nullptr)
		{
			m_pBlockParticleManager->ExplodeQubicleBinary(pQubicleModel, m_pVoxelCharacter->GetCharacterScale(), 50);
		}
	}

	// Signal to the player and NPC manager that this enemy died, clear linkage
	m_pPlayer->SetEnemyDied(this);
	m_pNPCManager->SetEnemyDied(this);

	// Signal to the projectile manager that this enemy died, clear linkage
	m_pProjectileManager->ClearProjectileOwnerForEnemy(this);

	// Give the player experience
	m_pPlayer->GetPlayerStats()->GiveExperience(10);

	if (m_enemyType == EnemyType::Mimic)
	{
		CubbyGame::GetInstance()->PlaySoundEffect3D(SoundEffect::MimicDie, GetCenter());
	}

	m_erase = true;
}

void Enemy::ConvertIntoOtherEnemyType(EnemyType newEnemyType, float scale)
{
	UnloadWeapon(true);
	UnloadWeapon(false);

	m_enemyType = newEnemyType;

	char characterBaseFolder[128];
	char qbFileName[128];
	char ms3dFileName[128];
	char animListFileName[128];
	char facesFileName[128];
	char characterFileName[128];

	SetupEnemyForType();

	sprintf(characterBaseFolder, "Resources/gamedata/models");
	sprintf(qbFileName, "Resources/gamedata/models/%s/%s.qb", m_typeString.c_str(), m_modelNameString.c_str());
	sprintf(ms3dFileName, "Resources/gamedata/models/%s/%s.ms3d", m_typeString.c_str(), m_typeString.c_str());
	sprintf(animListFileName, "Resources/gamedata/models/%s/%s.animlist", m_typeString.c_str(), m_typeString.c_str());
	sprintf(facesFileName, "Resources/gamedata/models/%s/%s.faces", m_typeString.c_str(), m_modelNameString.c_str());
	sprintf(characterFileName, "Resources/gamedata/models/%s/%s.character", m_typeString.c_str(), m_modelNameString.c_str());

	if (m_enemyType == EnemyType::Doppelganger)
	{
		char playerFolder[128];
		sprintf(playerFolder, "Saves/characters/%s", m_modelNameString.c_str());
		sprintf(qbFileName, "Saves/characters/%s/%s.qb", m_modelNameString.c_str(), m_modelNameString.c_str());
		sprintf(facesFileName, "Saves/characters/%s/%s.faces", m_modelNameString.c_str(), m_modelNameString.c_str());
		sprintf(characterFileName, "Saves/characters/%s/%s.character", m_modelNameString.c_str(), m_modelNameString.c_str());
	}

	m_pVoxelCharacter->UnloadCharacter();
	m_pVoxelCharacter->Reset();

	bool useQubicleManager = (m_enemyType != EnemyType::Doppelganger);
	m_pVoxelCharacter->LoadVoxelCharacter(m_typeString.c_str(), qbFileName, ms3dFileName, animListFileName, facesFileName, characterFileName, characterBaseFolder, useQubicleManager);

	m_pVoxelCharacter->SetBreathingAnimationEnabled(true);
	m_pVoxelCharacter->SetWinkAnimationEnabled(true);
	m_pVoxelCharacter->SetTalkingAnimationEnabled(false);
	m_pVoxelCharacter->SetRandomMouthSelection(false);
	m_pVoxelCharacter->SetRandomLookDirection(true);
	m_pVoxelCharacter->SetWireFrameRender(false);

	m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose");

	SetupWeaponsForType();

	InitEnemyForType();

	SetScale(scale);

	SetLeashParameters(GetCenter(), m_leashRadius);

	m_health = m_maxHealth;
}

void Enemy::Respawn()
{
	m_health = m_maxHealth;
}

bool Enemy::CanCheckAggroRange() const
{
	if (m_enemyType == EnemyType::Mimic)
	{
		if (m_aggro == false)
		{
			return false;
		}
	}

	return true;
}

void Enemy::SetNPCDied(NPC* pNPC)
{
	if (m_pTargetNPC == pNPC)
	{
		SetTargetNPC(nullptr);
	}
}

void Enemy::SetTargetNPC(NPC* pTargetNPC)
{
	m_pTargetNPC = pTargetNPC;
}

// Gameplay
void Enemy::SetSapped(bool sapped)
{
	if (m_sapped == false && sapped)
	{
		// Turn on
		m_pSappedParticleEffect = m_pBlockParticleManager->ImportParticleEffect("Resources/gamedata/particles/sapped.effect", GetAboveHeadPosition(), &m_sappedParticleEffectID);
		m_pSappedParticleEffect->PlayEffect();

		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose", 0.1f);
	}
	else if (m_sapped && sapped == false)
	{
		// Turn off
		m_pSappedParticleEffect->StopEffect();
		m_pSappedParticleEffect->m_isErase = true;
		m_pSappedParticleEffect = nullptr;
		m_sappedParticleEffectID = -1;
		m_sappedTimer = 0.0f;
	}

	m_sapped = sapped;

	if (m_sapped)
	{
		// Refresh sapped timer
		m_sappedTimer = 10.0f;
	}
}

bool Enemy::IsSapped() const
{
	return m_sapped;
}

// Spawning
void Enemy::SetSpawningParams(glm::vec3 start, glm::vec3 end, float time)
{
	m_spawning = true;
	m_spawnStartPosition = start;
	m_spawnEndPosition = end;
	m_spawnTime = time;
	m_spawningTimer = time;

	if (m_enemyType == EnemyType::WalkingZombie || m_enemyType == EnemyType::NormalSkeleton || m_enemyType == EnemyType::MeleeSkeleton || m_enemyType == EnemyType::RangedSkeleton || m_enemyType == EnemyType::MageSkeleton)
	{
		m_pVoxelCharacter->Update(0.01f, m_animationSpeed);
		SetAnimationSpeed(0.75f, true, AnimationSections::FullBody);
		m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "ZombieSpawn");
	}
	else if (m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
	{
		m_pVoxelCharacter->Update(0.01f, m_animationSpeed);
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "GhostPose", time);
	}

	Interpolator::GetInstance()->AddFloatInterpolation(&m_spawningT, 0.0f, 1.0f, time, -100.0f);
}

// Wander / Random target
void Enemy::SetRandomTargetMode(bool randomTargetMode)
{
	m_randomTargetPositionMode = randomTargetMode;
}

void Enemy::GetNewRandomTargetPosition()
{
	m_getRamdomTargetPositionTimer = 3.5f;
	float randomDistance = (GetRandomNumber(30, 100, 1) * 0.01f) * m_leashRadius;
	m_randomTargetPosition = m_leashPosition + (normalize(glm::vec3(GetRandomNumber(-1, 1, 2), 0.0f, GetRandomNumber(-1, 1, 2))) * randomDistance);

	m_stuck = false;
	m_updateStuckTimer = false;
	m_stuckTimer = 5.0f;
}

void Enemy::ResetRandomTargetPosition()
{
	m_randomTargetPosition = m_position;
}

void Enemy::SetLeashParameters(glm::vec3 leashPosition, float leashRadius)
{
	m_leashPosition = leashPosition;
	m_leashRadius = leashRadius;
}

// Attacking
void Enemy::Attack()
{
	bool doAttack = false;

	if (m_attackDelayTimer <= 0.0f)
	{
		if (m_enemyType == EnemyType::Bee)
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "StingAttack", 0.01f);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "StingAttack", 0.01f);

			m_attackSegmentAngle = 0.75f;
			m_attackDelayTime = 1.0f;

			float attackTime = 0.60f;
			float startRotation = 0.0f;
			float endRotation = 0.0f;
			float easingRotation = 0.0f;

			m_attackEnabled = true;
			m_attackEnabledTimer = 0.0f;
			m_attackEnabledDelayTimer = 0.3f;
			m_attackRotation = startRotation;

			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledTimer, 0.0f, attackTime, attackTime, 0.0f, nullptr, _AttackEnabledTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackRotation, startRotation, endRotation, attackTime, easingRotation);

			doAttack = true;
		}
		else if (m_enemyType == EnemyType::Bat)
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "StingAttack", 0.1f);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "StingAttack", 0.1f);

			m_attackDelayTime = 1.75f + GetRandomNumber(-100, 50, 2) * 0.005f;

			m_attackEnabledDelayTimer = 0.35f;
			float attackTime = 0.60f;

			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledTimer, 0.0f, attackTime, attackTime, 0.0f, nullptr, _AttackEnabledTimerFinished, this);

			doAttack = true;
		}
		else if (m_enemyType == EnemyType::GreenSlime || m_enemyType == EnemyType::RedSlime || m_enemyType == EnemyType::BlueSlime || m_enemyType == EnemyType::YellowSlime)
		{
			// TODO: Implement attack
		}
		else if (m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
		{
			// TODO: Implement attack
		}
		else if (m_enemyType == EnemyType::Mimic)
		{
			// TODO: Implement attack
		}
		else if (m_enemyType == EnemyType::IronGiant)
		{
			m_attackDelayTime = 1.0f;

			m_attackEnabledDelayTimer = 0.15f;

			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);

			doAttack = true;
		}
		else if (m_enemyType == EnemyType::NormalSkeleton)
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "SwordAttack2", 0.01f);

			m_attackDelayTime = 1.75f + GetRandomNumber(-50, 25, 2) * 0.005f;

			m_attackEnabledDelayTimer = 0.15f;

			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);

			doAttack = true;
		}
		else if (m_enemyType == EnemyType::MageSkeleton)
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "StaffAttack", 0.01f);

			m_attackDelayTime = 1.0f + GetRandomNumber(-100, 50, 2) * 0.005f;

			m_attackEnabledDelayTimer = 0.15f;

			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);

			doAttack = true;
		}
		else if (m_enemyType == EnemyType::RangedSkeleton)
		{
			if (m_isChargingAttack == false)
			{
				m_isChargingAttack = true;
				m_chargeAmount = 0.0f;

				LoadWeapon(false, "Resources/gamedata/items/Arrow/ArrowHold.item");

				if (m_isIdle)
				{
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BowCharge", 0.2f);
				}
				else
				{
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "BowCharge", 0.2f);
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "BowCharge", 0.2f);
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "BowCharge", 0.2f);
				}

				doAttack = true;
			}
		}
		else if (m_enemyType == EnemyType::MeleeSkeleton)
		{
			static int counter = 0;

			float attackTime;
			float startRotation;
			float endRotation;
			float easingRotation;

			if (counter == 2)
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "SwordAttack3", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "SwordAttack3", 0.01f);
				counter = 0;
				m_canInteruptCombatAnim = false;

				m_attackSegmentAngle = 0.60f;
				m_attackEnabledDelayTimer = 0.0f;
				attackTime = 0.6f;
				startRotation = 360.0f;
				endRotation = -45.0f;
				easingRotation = -75.0f;
			}
			else
			{
				if (counter == 0)
				{
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "SwordAttack1", 0.01f);
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "SwordAttack1", 0.01f);

					m_attackSegmentAngle = 0.75f;
					m_attackEnabledDelayTimer = 0.1f;
					attackTime = 0.4f;
					startRotation = -20.0f;
					endRotation = -20.0f;
					easingRotation = 0.0f;
				}
				else
				{
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "SwordAttack2", 0.01f);
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "SwordAttack2", 0.01f);

					m_attackSegmentAngle = 0.75f;
					m_attackEnabledDelayTimer = 0.2f;
					attackTime = 0.4f;
					startRotation = -10.0f;
					endRotation = -10.0f;
					easingRotation = 0.0f;
				}
				counter++;
			}

			m_movementWaitAfterAttackTimer = 0.5f;
			m_attackDelayTime = 0.35f;
			m_attackEnabled = true;
			m_attackEnabledTimer = 0.0f;
			m_attackRotation = startRotation;

			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledTimer, 0.0f, attackTime, attackTime, 0.0f, nullptr, _AttackEnabledTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackRotation, startRotation, endRotation, attackTime, easingRotation);

			// Start weapon trails
			if (m_pVoxelCharacter->GetRightWeapon())
			{
				m_pVoxelCharacter->GetRightWeapon()->StartWeaponTrails();
			}

			doAttack = true;
		}
	}

	// Only setup the delay timer and animation speed change if we are actually going to attack
	if (doAttack)
	{
		m_animationFinished[static_cast<int>(AnimationSections::RightArmHand)] = true;
		SetAnimationSpeed(1.0f, true, AnimationSections::FullBody);

		m_attackDelayTimer = m_attackDelayTime;

		m_canAttack = false;
	}
}

void Enemy::ReleaseAttack()
{
	if (m_isChargingAttack == true)
	{
		UnloadWeapon(false);

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(m_chargeSpawnPosition, m_chargeSpawnVelocity, 0.0f, "Resources/gamedata/items/Arrow/arrow.item", 0.08f);
		pProjectile->SetProjectileType(false, true, true);
		pProjectile->SetOwner(nullptr, nullptr, this);

		m_isChargingAttack = false;
		m_chargeAmount = 0.0f;

		m_attackDelayTimer = m_attackDelayTime*2.0f;

		if (m_isIdle)
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose", 0.2f);
		}
		else
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "BindPose", 0.2f);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "BindPose", 0.2f);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "BindPose", 0.2f);
		}
	}
}

bool Enemy::CanAttack() const
{
	return m_canAttack;
}

bool Enemy::GetAttackEnabled() const
{
	return m_attackEnabled && (m_attackEnabledDelayTimer <= 0.0f);
}

float Enemy::GetAttackRadius() const
{
	return m_attackRadius;
}

float Enemy::GetAttackRotation() const
{
	return m_attackRotation;
}

float Enemy::GetAttackSegmentAngle() const
{
	return m_attackSegmentAngle;
}

// Projectile hitbox
ProjectileHitboxType Enemy::GetProjectileHitboxType() const
{
	return m_projectileHitboxType;
}

float Enemy::GetProjectileHitboxRadius() const
{
	return m_projectileHitboxRadius;
}

float Enemy::GetProjectileHitboxXLength() const
{
	return m_projectileHitboxXLength;
}

float Enemy::GetProjectileHitboxYLength() const
{
	return m_projectileHitboxYLength;
}

float Enemy::GetProjectileHitboxZLength() const
{
	return m_projectileHitboxZLength;
}

glm::vec3 Enemy::GetProjectileHitboxCenter() const
{
	return GetCenter() + m_projectileHitboxCenterOffset;
}

// Spawn loot items
void Enemy::SpawnLootItems() const
{
	// Create the random ingredients drops
	ItemType item;
	int quantity;
	CubbyGame::GetInstance()->GetRandomLootManager()->GetSpawnedIngredientItemForEnemy(m_enemyType, &item, &quantity);

	if (quantity != 0 && item != ItemType::None)
	{
		for (int i = 0; i < quantity; ++i)
		{
			float radius = GetRadius();
			float angle = DegreeToRadian(GetRandomNumber(0, 360, 1));
			glm::vec3 ItemPosition = GetCenter() + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);
			glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0f);
			gravity = normalize(gravity);

			Item* pItem = m_pItemManager->CreateItem(GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), GetItemFileNameForType(item).c_str(), ItemType::DroppedItem, GetItemTitleForType(item).c_str(), true, false, 0.08f);

			if (pItem != nullptr)
			{
				pItem->SetGravityDirection(gravity);
				glm::vec3 vel = ItemPosition - GetCenter();
				pItem->SetVelocity(normalize(vel) * static_cast<float>(GetRandomNumber(2, 4, 2)) + glm::vec3(0.0f, 9.5f + GetRandomNumber(-2, 4, 2), 0.0f));
				pItem->SetRotation(glm::vec3(0.0f, GetRandomNumber(0, 360, 2), 0.0f));
				pItem->SetAngularVelocity(glm::vec3(0.0f, 90.0f, 0.0f));
				pItem->SetCollisionEnabled(false);

				InventoryItem* pInventoryItem = CubbyGame::GetInstance()->GetInventoryManager()->CreateInventoryItemForCrafting(item, 1, ItemQuality::Common);
				pItem->SetDroppedItem(pInventoryItem);

				int numY = pItem->GetVoxelItem()->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix(0)->m_matrixSizeY;
				pItem->GetVoxelItem()->SetRenderOffset(glm::vec3(0.0f, numY * 0.5f, 0.0f));
			}
		}
	}

	// Create random loot
	if (m_enemyType == EnemyType::Mimic)
	{
		EquipmentType equipment = EquipmentType::None;
		InventoryItem* pRandomLoot = CubbyGame::GetInstance()->GetRandomLootManager()->GetRandomLootItem(&equipment);

		if (pRandomLoot != nullptr && equipment != EquipmentType::None)
		{
			float radius = GetRadius();
			float angle = DegreeToRadian(GetRandomNumber(0, 360, 1));
			glm::vec3 ItemPosition = GetCenter() + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);
			glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0f);
			gravity = normalize(gravity);

			Item* pItem = m_pItemManager->CreateItem(GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), GetEquipmentFileNameForType(equipment).c_str(), ItemType::DroppedItem, GetEquipmentTitleForType(equipment).c_str(), true, false, 0.08f);

			if (pItem != nullptr)
			{
				pItem->SetGravityDirection(gravity);
				glm::vec3 vel = ItemPosition - GetCenter();
				pItem->SetVelocity(normalize(vel) * static_cast<float>(GetRandomNumber(2, 4, 2)) + glm::vec3(0.0f, 9.5f + GetRandomNumber(-2, 4, 2), 0.0f));
				pItem->SetRotation(glm::vec3(0.0f, GetRandomNumber(0, 360, 2), 0.0f));
				pItem->SetAngularVelocity(glm::vec3(0.0f, 90.0f, 0.0f));
				pItem->SetCollisionEnabled(false);

				pItem->SetDroppedItem(pRandomLoot);

				int numY = pItem->GetVoxelItem()->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix(0)->m_matrixSizeY;
				pItem->GetVoxelItem()->SetRenderOffset(glm::vec3(0.0f, numY * 0.5f, 0.0f));
			}
		}
	}

	// Create items from the killed enemy (coins, hearts)
	int numItems = GetRandomNumber(3, 8);

	for (int i = 0; i < numItems; ++i)
	{
		float radius = GetRadius();
		float angle = DegreeToRadian(GetRandomNumber(0, 360, 1));
		glm::vec3 ItemPosition = GetCenter() + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);

		glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0f);
		gravity = normalize(gravity);
		
		Item* pItem;
		if (GetRandomNumber(0, 100) > 90)
		{
			pItem = m_pItemManager->CreateItem(GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), GetItemFileNameForType(ItemType::Heart).c_str(), ItemType::Heart, GetItemTitleForType(ItemType::Heart).c_str(), false, true, 0.03f);
		}
		else
		{
			pItem = m_pItemManager->CreateItem(GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), GetItemFileNameForType(ItemType::Coin).c_str(), ItemType::Coin, GetItemTitleForType(ItemType::Coin).c_str(), false, true, 0.0225f);
		}

		if (pItem != nullptr)
		{
			pItem->SetGravityDirection(gravity);
			glm::vec3 vel = ItemPosition - GetCenter();
			pItem->SetVelocity(normalize(vel) * static_cast<float>(GetRandomNumber(2, 4, 2)) + glm::vec3(0.0f, 9.5f + GetRandomNumber(-2, 4, 2), 0.0f));
			pItem->SetRotation(glm::vec3(0.0f, GetRandomNumber(0, 360, 2), 0.0f));
			pItem->SetAngularVelocity(glm::vec3(0.0f, 90.0f, 0.0f));

			pItem->SetAutoDisappear(20.0f + (GetRandomNumber(-20, 20, 1) * 0.2f));
			pItem->SetCollisionEnabled(false);
		}
	}
}

void Enemy::SpawnGibs() const
{
	int numGibs = 4;

	for (int i = 0; i < numGibs; ++i)
	{
		float radius = 0.5f;
		float angle = DegreeToRadian(GetRandomNumber(0, 360, 1));
		glm::vec3 ItemPosition = GetCenter() + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);

		glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0f);
		gravity = normalize(gravity);
		Item* pItem = nullptr;

		if (i == 0)
		{
			pItem = m_pItemManager->CreateItem(GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), "Resources/gamedata/items/ZombieGibs/SpineGib.item", ItemType::Gib, "Gib1", false, false, 0.08f);
		}
		else if (i == 1)
		{
			pItem = m_pItemManager->CreateItem(GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), "Resources/gamedata/items/ZombieGibs/FootGib.item", ItemType::Gib, "Gib2", false, false, 0.08f);
		}
		else if (i == 2)
		{
			pItem = m_pItemManager->CreateItem(GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), "Resources/gamedata/items/ZombieGibs/Hand2Gib.item", ItemType::Gib, "Gib3", false, false, 0.08f);
		}
		else if (i == 3)
		{
			pItem = m_pItemManager->CreateItem(GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), "Resources/gamedata/items/ZombieGibs/LegsGib.item", ItemType::Gib, "Gib4", false, false, 0.08f);
		}

		if (pItem != nullptr)
		{
			pItem->SetGravityDirection(gravity);
			glm::vec3 vel = ItemPosition - GetCenter();
			pItem->SetVelocity(normalize(vel) * static_cast<float>(GetRandomNumber(3, 7, 2)) + glm::vec3(0.0f, 5.0f + GetRandomNumber(-2, 5, 2), 0.0f));
			pItem->SetRotation(glm::vec3(0.0f, GetRandomNumber(0, 360, 2), 0.0f));
			
			pItem->SetAutoDisappear(10.0f + (GetRandomNumber(-20, 20, 1) * 0.2f));
			pItem->SetCollisionEnabled(false);
		}
	}
}

// Rendering Helpers
void Enemy::CalculateWorldTransformMatrix()
{
	glm::vec3 forward = m_forward;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	if (forward.x == 0.0f && forward.z == 0.0f)
	{
		if (forward.y < 0.0f)
		{
			up = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		else
		{
			up = glm::vec3(-1.0f, 0.0f, 0.0f);
		}
	}

	glm::vec3 right = normalize(cross(up, forward));

	up = normalize(cross(forward, right));

	float matrix[16] =
	{
		right.x, right.y, right.z, 0.0f,
		up.x, up.y, up.z, 0.0f,
		forward.x, forward.y, forward.z, 0.0f,
		m_position.x, m_position.y, m_position.z, 1.0f
	};

	m_worldMatrix.SetValues(matrix);
}

void Enemy::SetOutlineRender(bool outline)
{
	m_outlineRender = outline;
}

bool Enemy::GetOutlineRender() const
{
	return m_outlineRender;
}

void Enemy::SetWireFrameRender(bool wireframe) const
{
	if (m_pVoxelCharacter != nullptr)
	{
		m_pVoxelCharacter->SetWireFrameRender(wireframe);
	}
}

// Updating
void Enemy::UpdateWeaponLights() const
{
	for (int i = 0; i < 2; ++i)
	{
		VoxelWeapon* pWeapon;
		bool isWeaponLoaded;
		
		if (i == 0)  // Right side
		{
			pWeapon = m_pVoxelCharacter->GetRightWeapon();
			isWeaponLoaded = m_pVoxelCharacter->IsRightWeaponLoaded();
		}
		else  // Left side
		{
			pWeapon = m_pVoxelCharacter->GetLeftWeapon();
			isWeaponLoaded = m_pVoxelCharacter->IsLeftWeaponLoaded();
		}

		if (pWeapon != nullptr)
		{
			if (isWeaponLoaded)
			{
				for (int j = 0; j < pWeapon->GetNumLights(); ++j)
				{
					unsigned int lightID;
					glm::vec3 lightPos;
					float lightRadius;
					float lightDiffuseMultiplier;
					Color lightColor;
					bool connectedToSegment;

					pWeapon->GetLightParams(j, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

					if (lightID == -1)
					{
						m_pLightingManager->AddLight(glm::vec3(0.0f), 0.0f, 1.0f, Color(1.0f, 1.0f, 1.0f, 1.0f), &lightID);
						pWeapon->SetLightingID(j, lightID);
					}

					if (connectedToSegment == false)
					{
						// Rotate due to characters forward vector
						float rotationAngle = acos(dot(glm::vec3(0.0f, 0.0f, 1.0f), m_forward));
						if (m_forward.x < 0.0f)
						{
							rotationAngle = -rotationAngle;
						}
						Matrix4 rotationMatrix;
						rotationMatrix.SetRotation(0.0f, rotationAngle, 0.0f);
						lightPos = rotationMatrix * lightPos;

						// Translate to position
						lightPos += m_position;
					}

					float scale = m_pVoxelCharacter->GetCharacterScale();

					m_pLightingManager->UpdateLightPosition(lightID, glm::vec3(lightPos.x, lightPos.y, lightPos.z));
					m_pLightingManager->UpdateLightRadius(lightID, lightRadius * scale);
					m_pLightingManager->UpdateLightDiffuseMultiplier(lightID, lightDiffuseMultiplier);
					m_pLightingManager->UpdateLightColor(lightID, lightColor);
				}
			}
		}
	}
}

void Enemy::UpdateWeaponParticleEffects() const
{
	for (int i = 0; i < 2; ++i)
	{
		VoxelWeapon* pWeapon;
		bool isWeaponLoaded;

		if (i == 0)  // Right side
		{
			pWeapon = m_pVoxelCharacter->GetRightWeapon();
			isWeaponLoaded = m_pVoxelCharacter->IsRightWeaponLoaded();
		}
		else  // Left side
		{
			pWeapon = m_pVoxelCharacter->GetLeftWeapon();
			isWeaponLoaded = m_pVoxelCharacter->IsLeftWeaponLoaded();
		}

		if (pWeapon != nullptr)
		{
			if (isWeaponLoaded)
			{
				for (int j = 0; j < pWeapon->GetNumParticleEffects(); ++j)
				{
					unsigned int particleEffectID;
					glm::vec3 particleEffectPos;
					glm::vec3 particleEffectPosNoWorldOffset;
					std::string effectName;
					bool connectedToSegment;

					pWeapon->GetParticleEffectParams(j, &particleEffectID, &particleEffectPos, &effectName, &connectedToSegment);

					if (particleEffectID == -1)
					{
						m_pBlockParticleManager->ImportParticleEffect(effectName, glm::vec3(particleEffectPos.x, particleEffectPos.y, particleEffectPos.z), &particleEffectID);
						pWeapon->SetParticleEffectID(j, particleEffectID);
						m_pBlockParticleManager->SetRenderNoWoldOffsetViewport(particleEffectID, true);
					}

					particleEffectPosNoWorldOffset = particleEffectPos;
					
					if (connectedToSegment == false)
					{
						// Rotate due to characters forward vector
						float rotationAngle = acos(dot(glm::vec3(0.0f, 0.0f, 1.0f), m_forward));
						if (m_forward.x < 0.0f)
						{
							rotationAngle = -rotationAngle;
						}
						Matrix4 rotationMatrix;
						rotationMatrix.SetRotation(0.0f, rotationAngle, 0.0f);
						particleEffectPos = rotationMatrix * particleEffectPos;

						// Translate to position
						particleEffectPos += m_position;
					}

					m_pBlockParticleManager->UpdateParticleEffectPosition(particleEffectID, particleEffectPos, particleEffectPosNoWorldOffset);
				}
			}
		}
	}
}

void Enemy::Update(float dt)
{
	// Update grid position
	UpdateGridPosition();

	// Update timers
	UpdateTimers(dt);

	// Update spawning
	UpdateSpawning();

	// Update look at point
	UpdateLookingAndForwardTarget(dt);

	// Update movement
	UpdateMovement(dt);

	// Update combat
	UpdateCombat();

	// Update gameplay
	UpdateGameplay();

	// Check for player attack damage
	CheckPlayerDamageRadius();

	// Check for NPC attack damage
	CheckNPCDamageRadius();

	if (m_pVoxelCharacter != nullptr)
	{
		m_pVoxelCharacter->Update(dt, m_animationSpeed);
		m_pVoxelCharacter->SetWeaponTrailsOriginMatrix(m_worldMatrix);

		for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
		{
			m_animationFinished[i] = m_pVoxelCharacter->HasAnimationFinished(static_cast<AnimationSections>(i));
		}
	}

	// Reset the canAttack flag if our weapon arm animation is completed
	if (m_canAttack == false && (m_animationFinished[static_cast<int>(AnimationSections::RightArmHand)] == true) || (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger))
	{
		// Stop weapon trails
		if (m_pVoxelCharacter != nullptr)
		{
			if (m_pVoxelCharacter->GetRightWeapon())
			{
				if (m_pVoxelCharacter->IsRightWeaponLoaded())
				{
					m_pVoxelCharacter->GetRightWeapon()->StopWeaponTrails();
				}
			}
		}

		m_canAttack = true;
		m_canInteruptCombatAnim = true;
	}

	// Charging - figure out trajectory and velocity for projectile
	if (m_isChargingAttack)
	{
		m_chargeAmount += dt / m_chargeTime;

		if (m_chargeAmount > 1.0f)
		{
			m_chargeAmount = 1.0f;
		}
	}

	if (m_enemyType == EnemyType::RangedSkeleton)
	{
		m_chargeSpawnPosition = GetCenter() + (m_forward * 0.75f) + (GetUpVector() * 0.5f);

		float liftAmount = 1.75f * m_chargeAmount;
		float powerAmount = 45.0f * m_chargeAmount;

		// Player target
		glm::vec3 toTarget;
		if (m_pTargetNPC != nullptr)
		{
			toTarget = m_pTargetNPC->GetCenter() - GetCenter();
		}
		else
		{
			toTarget = m_pPlayer->GetCenter() - GetCenter();
		}

		float toTargetDistance = length(toTarget);
		liftAmount += toTargetDistance * 0.20f;
		m_chargeSpawnVelocity = (normalize(toTarget) * powerAmount) + glm::vec3(0.0f, liftAmount, 0.0f);
	}

	// Update physics
	UpdatePhysics(dt);

	// Update the enemy light
	if (m_enemyType == EnemyType::Doppelganger)
	{
		m_pLightingManager->UpdateLightPosition(m_enemyLightID, GetCenter());

		m_pEnemyParticleEffect->SetPosition(GetCenter() + glm::vec3(0.0f, 0.75f, 0.0f));
	}
}

void Enemy::UpdatePhysics(float dt)
{
	// Gravity modifications for flying creatures
	glm::vec3 acceleration;
	if (m_enemyType != EnemyType::Bee && m_enemyType != EnemyType::Bat && m_enemyType != EnemyType::Ghost && m_enemyType != EnemyType::Doppelganger)
	{
		if (m_spawning == false)
		{
			acceleration += (m_gravityDirection * 9.81f) * 5.0f;
		}
	}

	// Integrate velocity
	m_velocity += acceleration * dt;

	if (m_enemyType != EnemyType::Ghost && m_enemyType != EnemyType::Doppelganger)
	{
		if (m_spawning == false)
		{
			// Check collision
			glm::vec3 velocityToUse = m_velocity;
			glm::vec3 velAmount = velocityToUse*dt;
			glm::vec3 pNormal;
			int numberDivision = 1;

			while (length(velAmount) >= 1.0f)
			{
				numberDivision++;
				velAmount = velocityToUse*(dt / numberDivision);
			}

			for (int i = 0; i < numberDivision; ++i)
			{
				float dtToUse = (dt / numberDivision) + ((dt / numberDivision) * i);
				glm::vec3 posToCheck = GetCenter() + velocityToUse * dtToUse;

				if (CheckCollisions(posToCheck, m_previousPosition, &pNormal, &velAmount))
				{
					// Reset velocity, we don't have any bounce
					m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
					velocityToUse = glm::vec3(0.0f, 0.0f, 0.0f);

					if (velocityToUse.y <= 0.0f)
					{
						if (m_canJump == false)
						{
							m_canJump = true;
						}
					}
				}
			}

			// Integrate position
			m_position += velocityToUse * dt;
		}
	}

	// Drag for flying enemies
	if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
	{
		m_velocity *= 0.9975f;
	}

	if (m_enemyType != EnemyType::TargetDummy && m_enemyType != EnemyType::Mimic)
	{
		glm::vec3 currentPos = m_position;
		glm::vec3 difference = (currentPos - m_previousPosition);
		difference.y = 0.0f;

		float lengthResult = length(difference);
		if (lengthResult <= 0.025f)
		{
			m_updateStuckTimer = true;
		}
		else
		{
			m_updateStuckTimer = false;
			m_stuckTimer = 5.0f;
		}
	}

	m_previousPosition = GetCenter();
}

void Enemy::UpdateLookingAndForwardTarget(float dt)
{
	if (m_sapped)
	{
		return;
	}

	glm::vec3 lootAt = m_lookPoint;
	if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
	{
		lootAt += glm::vec3(0.0f, 0.5f, 0.0f);
	}

	glm::vec3 toPoint = lootAt - GetCenter();
	toPoint = normalize(toPoint);

	if (m_lookAtPoint)
	{
		m_targetForward = toPoint;

		if (m_enemyType != EnemyType::Bee && m_enemyType != EnemyType::Bat && m_enemyType != EnemyType::Ghost && m_enemyType != EnemyType::Doppelganger)
		{
			m_targetForward.y = 0.0f;
		}
	}

	m_targetForward = normalize(m_targetForward);

	if (length(m_forward - m_targetForward) <= m_bodyTurnStopThreshold)
	{
		m_isLookingAtPoint = true;
	}
	else
	{
		m_isLookingAtPoint = false;

		glm::vec3 targetForwardToUse = m_targetForward;
		float dotToTargetForward = dot(m_targetForward, m_forward);
		if (dotToTargetForward <= -1.0f || dotToTargetForward >= 1.0f)
		{
			targetForwardToUse = cross(m_targetForward, glm::vec3(0.0f, 1.0f, 0.0f));
		}

		glm::vec3 toTarget = targetForwardToUse - m_forward;
		m_forward += (toTarget * dt) * m_bodyTurnSpeedMultiplier;
		m_forward = normalize(m_forward);
	}

	if (m_pVoxelCharacter != nullptr)
	{
		if (m_lookAtPoint)
		{
			float angleMissing = dot(m_targetForward, m_forward);
			float angle = acos(angleMissing);

			if (angle < -1.0f)
			{
				angle = -1.0f;
			}
			if (angle > 1.0f)
			{
				angle = 1.0f;
			}

			if (RadianToDegree(angle) > 80.0f)
			{
				angle = DegreeToRadian(80.0f);
			}

			float tempX = static_cast<float>(sin((angle)));
			float tempZ = static_cast<float>(cos((angle)));

			glm::vec3 crossProduct = cross(m_targetForward, m_forward);

			if (crossProduct.y > 0.0f)
				tempX = -tempX;

			if ((m_randomTargetPositionMode && m_aggro == false) || m_enemyType == EnemyType::Doppelganger)
			{
				toPoint.y = 0.0f;
			}

			glm::vec3 lookDirection = glm::vec3(tempX, toPoint.y, tempZ);

			m_pVoxelCharacter->SetFaceTargetDirection(lookDirection);
			m_pVoxelCharacter->SetRandomLookDirection(false);
			m_pVoxelCharacter->SetFaceLookToTargetSpeedMultiplier(4.0f);
		}
	}
}

void Enemy::UpdateCombat()
{
	if (m_sapped)
	{
		return;
	}

	if (m_enemyType == EnemyType::TargetDummy)
	{
		return;
	}

	bool aggroCancel;
	float radius;
	glm::vec3 center;

	if (m_pTargetNPC != nullptr)
	{
		aggroCancel = m_pTargetNPC->IsDead();
		radius = m_pTargetNPC->GetRadius();
		center = m_pTargetNPC->GetCenter();
	}
	else
	{
		aggroCancel = m_pPlayer->IsDead();
		radius = m_pPlayer->GetRadius();
		center = m_pPlayer->GetCenter();
	}

	if (aggroCancel)
	{
		if (m_aggro == true)
		{
			m_aggro = false;

			if (m_enemyType != EnemyType::Mimic)
			{
				SetRandomLookMode();
			}
		}
	}
	else
	{
		// Check the aggro area and radius
		if (CanCheckAggroRange())
		{
			glm::vec3 distance = center - GetCenter();
			float lengthToPlayer = length(distance);

			if (lengthToPlayer <= m_aggroLookRadius + radius)
			{
				glm::vec3 distanceMinusY = distance;
				distanceMinusY.y = 0.0f;
				glm::vec3 direction = normalize(distanceMinusY);
				direction = normalize(direction);

				// Figure out the look vector, based on the look rotation
				float rotation = GetRotation();
				float faceLookingAngle = GetFaceLookingRotation();
				float angle = DegreeToRadian(rotation + faceLookingAngle);
				glm::vec3 attackDirection = glm::vec3(sin(angle), 0.0f, cos(angle));
				float dotProduct = dot(direction, attackDirection);

				if (dotProduct > m_aggroLookSegmentAngle) // Check if we are within the look segment
				{
					if (m_aggro == false)
					{
						// If we are setting aggro for initial setting, reset and extend the attack delay timer
						m_attackDelayTimer = m_attackDelayTime*2.0f;
					}

					m_aggro = true;
					m_aggroResetTimer = m_aggroResetTime;

					if (m_enemyType == EnemyType::IronGiant || m_enemyType == EnemyType::RangedSkeleton || m_enemyType == EnemyType::MageSkeleton || m_enemyType == EnemyType::Bat)
					{
						StopMoving();
					}
				}
			}
		}
	}

	if (m_enemyType == EnemyType::RangedSkeleton || m_enemyType == EnemyType::NormalSkeleton || m_enemyType == EnemyType::MageSkeleton || m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::IronGiant)
	{
		// Update ranged combat
		UpdateRangedCombat();
	}
	else if (m_enemyType == EnemyType::GreenSlime || m_enemyType == EnemyType::RedSlime || m_enemyType == EnemyType::BlueSlime || m_enemyType == EnemyType::YellowSlime || m_enemyType == EnemyType::Mummy || m_enemyType == EnemyType::WalkingZombie || m_enemyType == EnemyType::CrawlingZombie || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger || m_enemyType == EnemyType::Mimic)
	{
		// Update touch combat
		UpdateTouchCombat();
	}
	else if (m_enemyType == EnemyType::MeleeSkeleton || m_enemyType == EnemyType::Bee)
	{
		// Update melee combat
		UpdateMeleeCombat();
	}
}

void Enemy::UpdateGameplay() const
{
	if (m_sapped)
	{
		if (m_pSappedParticleEffect != nullptr)
		{
			m_pSappedParticleEffect->SetPosition(GetAboveHeadPosition());
		}
	}
}

void Enemy::UpdateMeleeCombat()
{
	if (m_pTargetNPC != nullptr)
	{
		if (m_pTargetNPC->IsDead())
		{
			return;
		}
	}
	else
	{
		if (m_pPlayer->IsDead())
		{
			return;
		}
	}

	if (m_aggro)
	{
		glm::vec3 targetPos;
		float radius;

		if (m_pTargetNPC != nullptr)
		{
			targetPos = m_pTargetNPC->GetCenter();
			radius = m_pTargetNPC->GetRadius();
		}
		else
		{
			targetPos = m_pPlayer->GetCenter();
			radius = m_pPlayer->GetRadius();
		}

		glm::vec3 toTarget = targetPos - GetCenter();
		toTarget.y = 0.0f;
		float lengthToTarget = length(toTarget);

		m_lookPoint = targetPos;
		m_lookAtPoint = true;

		if (lengthToTarget <= m_attackRadius + radius)
		{
			m_insideAttackRadius = true;

			if (m_canAttack)
			{
				Attack();
			}
		}
		else
		{
			m_insideAttackRadius = false;
		}
	}
}

void Enemy::UpdateRangedCombat()
{
	if (m_pTargetNPC != nullptr)
	{
		if (m_pTargetNPC->IsDead())
		{
			return;
		}
	}
	else
	{
		if (m_pPlayer->IsDead())
		{
			return;
		}
	}

	if (m_aggro)
	{
		m_insideAttackRadius = true;

		if (m_canAttack)
		{
			Attack();
		}

		if (m_isChargingAttack)
		{
			if (m_chargeAmount >= 1.0f)
			{
				ReleaseAttack();
			}
		}
	}
}

void Enemy::UpdateTouchCombat()
{
	if (m_pTargetNPC != nullptr)
	{
		if (m_pTargetNPC->IsDead())
		{
			return;
		}
	}
	else
	{
		if (m_pPlayer->IsDead())
		{
			return;
		}
	}

	glm::vec3 targetPos;
	float radius;
	ProjectileHitboxType hitBoxType;
	glm::vec3 hitBoxCenter;
	float targetRotation;
	float hitboxX;
	float hitboxY;
	float hitboxZ;

	if (m_pTargetNPC != nullptr)
	{
		targetPos = m_pTargetNPC->GetCenter();
		radius = m_pTargetNPC->GetRadius();
		hitBoxType = m_pTargetNPC->GetProjectileHitboxType();
		hitBoxCenter = m_pTargetNPC->GetProjectileHitboxCenter();
		targetRotation = m_pTargetNPC->GetRotation();
		hitboxX = m_pTargetNPC->GetProjectileHitboxXLength();
		hitboxY = m_pTargetNPC->GetProjectileHitboxYLength();
		hitboxZ = m_pTargetNPC->GetProjectileHitboxZLength();
	}
	else
	{
		targetPos = m_pPlayer->GetCenter();
		radius = m_pPlayer->GetRadius();
		hitBoxType = m_pPlayer->GetProjectileHitboxType();
		hitBoxCenter = m_pPlayer->GetProjectileHitboxCenter();
		targetRotation = m_pPlayer->GetRotation();
		hitboxX = m_pPlayer->GetProjectileHitboxXLength();
		hitboxY = m_pPlayer->GetProjectileHitboxYLength();
		hitboxZ = m_pPlayer->GetProjectileHitboxZLength();
	}

	glm::vec3 toTarget = targetPos - GetCenter();
	float lengthToTarget = length(toTarget);

	if (m_aggro)
	{
		m_lookPoint = targetPos;
		m_lookAtPoint = true;
	}

	bool touchTarget = false;
	if (hitBoxType == ProjectileHitboxType::Sphere)
	{
		if (lengthToTarget <= m_attackRadius + radius)
		{
			touchTarget = true;
		}
	}
	else if (hitBoxType == ProjectileHitboxType::Cube)
	{
		Plane3D planes[6];
		Matrix4 rotationMatrix;
		rotationMatrix.SetYRotation(DegreeToRadian(targetRotation));

		planes[0] = Plane3D(rotationMatrix * glm::vec3(-1.0f, 0.0f, 0.0f), hitBoxCenter + (rotationMatrix * glm::vec3(hitboxX, 0.0f, 0.0f)));
		planes[1] = Plane3D(rotationMatrix * glm::vec3(1.0f, 0.0f, 0.0f), hitBoxCenter + (rotationMatrix * glm::vec3(-hitboxX, 0.0f, 0.0f)));
		planes[2] = Plane3D(rotationMatrix * glm::vec3(0.0f, -1.0f, 0.0f), hitBoxCenter + (rotationMatrix * glm::vec3(0.0f, hitboxY, 0.0f)));
		planes[3] = Plane3D(rotationMatrix * glm::vec3(0.0f, 1.0f, 0.0f), hitBoxCenter + (rotationMatrix * glm::vec3(0.0f, -hitboxY, 0.0f)));
		planes[4] = Plane3D(rotationMatrix * glm::vec3(0.0f, 0.0f, -1.0f), hitBoxCenter + (rotationMatrix * glm::vec3(0.0f, 0.0f, hitboxZ)));
		planes[5] = Plane3D(rotationMatrix * glm::vec3(0.0f, 0.0f, 1.0f), hitBoxCenter + (rotationMatrix * glm::vec3(0.0f, 0.0f, -hitboxZ)));

		float distance;
		int inside = 0;

		for (int i = 0; i < 6; ++i)
		{
			distance = planes[i].GetPointDistance(GetCenter());

			if (distance < -GetRadius())
			{
				// Outside...
			}
			else if (distance < GetRadius())
			{
				// Intersecting..
				inside++;
			}
			else
			{
				// Inside...
				inside++;
			}
		}

		if (inside == 6)
		{
			touchTarget = true;
		}
	}

	if (touchTarget)
	{
		glm::vec3 knockbackDirection = glm::vec3(toTarget.x, 0.0f, toTarget.z) * 2.0f + glm::vec3(0.0f, 1.0f, 0.0f);
		knockbackDirection = normalize(knockbackDirection);
		Color damageColor = Color(1.0f, 1.0f, 1.0f);

		float knockbackAmount = 16.0f;
		if (m_pTargetNPC != nullptr)
		{
			// Collision damage for NPC
			m_pTargetNPC->SetTargetEnemy(this);
			m_pTargetNPC->DoDamage(15.0f, damageColor, knockbackDirection, knockbackAmount, false);
		}
		else
		{
			// Collision damage for player
			m_pPlayer->DoDamage(15.0f, damageColor, knockbackDirection, knockbackAmount, false);
		}

		m_velocity.x = 0.0f;
		m_velocity.z = 0.0f;
		m_aggro = true;
		m_aggroResetTimer = m_aggroResetTime;
	}
}

void Enemy::UpdateMovement(float dt)
{
	if (m_spawning)
	{
		// Can't move if spawning
		return;
	}

	if (m_sapped)
	{
		// Can't move if sapped
		return;
	}

	if (m_movementFreezeTimer >= 0.0f)
	{
		// Can't move if movement freeze time is set
		return;
	}

	glm::vec3 targetPos;

	if (m_aggro)
	{
		if (m_pTargetNPC != nullptr)
		{
			targetPos = m_pTargetNPC->GetCenter();
		}
		else
		{
			targetPos = m_pPlayer->GetCenter();
		}

		if (m_enemyType == EnemyType::Bat || m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
		{
			targetPos += glm::vec3(0.0f, 0.5f, 0.0f);
		}
	}
	else
	{
		if (m_randomTargetPositionMode)
		{
			targetPos = m_randomTargetPosition;
		}
		else
		{
			targetPos = GetCenter();
		}
	}

	if (m_enemyType == EnemyType::TargetDummy)
	{
		targetPos = GetCenter();
	}

	glm::vec3 toTarget = targetPos - GetCenter();
	if (m_enemyType != EnemyType::Bee && m_enemyType != EnemyType::Bat && m_enemyType != EnemyType::Ghost && m_enemyType != EnemyType::Doppelganger)
	{
		toTarget.y = 0.0f;
	}

	float lengthToTarget = length(toTarget);
	bool reachedTarget;
	if (m_pTargetNPC != nullptr)
	{
		reachedTarget = (lengthToTarget < (m_attackRadius*0.99f) + m_pTargetNPC->GetRadius());
	}
	else
	{
		reachedTarget = (lengthToTarget < (m_attackRadius*0.99f) + m_pPlayer->GetRadius());
	}

	if (m_isIdle && reachedTarget == false)
	{
		m_hasSetIdleAnimation = false;
		m_waitBeforeStopMovingAnimationTimer = 0.25f;
	}

	if (reachedTarget && m_canJump == true)
	{
		StopMoving();

		// Random wander and getting new target timer
		if (m_randomTargetPositionMode && m_aggro == false)
		{
			if (m_getRamdomTargetPositionTimer <= 0.0f)
			{
				GetNewRandomTargetPosition();
			}
			else
			{
				m_getRamdomTargetPositionTimer -= dt;
			}
		}
	}
	else
	{
		LookAtPoint(targetPos);

		bool shouldStopMovingUntilJump = false;
		if (IsBlockInFront())
		{
			if (m_enemyType != EnemyType::CrawlingZombie)
			{
				if (m_jumpTimer > 0.0f)
				{
					shouldStopMovingUntilJump = true;
					StopMoving();
				}

				Jump(true);
			}
		}

		if (m_enemyType == EnemyType::GreenSlime || m_enemyType == EnemyType::RedSlime || m_enemyType == EnemyType::BlueSlime || m_enemyType == EnemyType::YellowSlime || m_enemyType == EnemyType::Mimic)
		{
			if (m_aggro || m_enemyType != EnemyType::Mimic)
			{
				Jump(false);
			}
		}
		else
		{
			if (CanMoveTowardsTarget())
			{
				if (shouldStopMovingUntilJump == false)
				{
					if (m_movementWaitAfterAttackTimer <= 0.0f)
					{
						glm::vec3 toTarget2 = targetPos - m_position;
						glm::vec3 movementDirection = toTarget2;
						if (m_enemyType != EnemyType::Bee && m_enemyType != EnemyType::Bat && m_enemyType != EnemyType::Ghost && m_enemyType != EnemyType::Doppelganger)
						{
							movementDirection.y = 0.0f;
						}
						movementDirection = normalize(movementDirection);

						float movementSpeed = (m_movementSpeed * dt);
						if (movementSpeed > 0.5f)
						{
							movementSpeed = 0.5f;
						}

						float animationSpeed = 0.75f;

						for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
						{
							SetAnimationSpeed(animationSpeed, false, static_cast<AnimationSections>(i));
						}

						// Iron giants walk slow
						if (m_enemyType == EnemyType::IronGiant)
						{
							for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
							{
								SetAnimationSpeed(0.5f, false, static_cast<AnimationSections>(i));
							}
						}

						MoveAbsolute(movementDirection, movementSpeed);
					}
				}
			}
		}
	}

	if (m_enemyType == EnemyType::GreenSlime || m_enemyType == EnemyType::RedSlime || m_enemyType == EnemyType::BlueSlime || m_enemyType == EnemyType::YellowSlime || m_enemyType == EnemyType::Mimic)
	{
		if (m_canJump == false)
		{
			float movementSpeed = m_movementSpeed * dt;
			if (movementSpeed > 0.5f)
			{
				movementSpeed = 0.5f;
			}

			MoveAbsolute(m_forward, movementSpeed);
		}
	}
}

void Enemy::UpdateSpawning()
{
	if (m_spawning)
	{
		if (m_enemyType == EnemyType::Ghost || m_enemyType == EnemyType::Doppelganger)
		{
			glm::vec3 diff = m_spawnEndPosition - m_spawnStartPosition;

			m_position = m_spawnStartPosition + (diff * m_spawningT);

			// Spawning scale
			m_pVoxelCharacter->SetCharacterScale(m_renderScale * m_spawningT);

			SetRotation(720.0f * m_spawningT);
		}
	}
}

void Enemy::UpdateTimers(float dt)
{
	// Sapped
	if (m_sapped)
	{
		if (m_sappedTimer <= 0.0f)
		{
			SetSapped(false);
		}
		else
		{
			m_sappedTimer -= dt;
		}
	}
	else
	{
		// Aggro reset timer
		if (m_aggro)
		{
			if (m_aggroResetTimer <= 0.0f)
			{
				if (m_canJump == true)
				{
					m_aggro = false;

					m_isChargingAttack = false;
					m_chargeAmount = 0.0f;

					if (m_enemyType != EnemyType::GreenSlime && m_enemyType != EnemyType::RedSlime && m_enemyType != EnemyType::BlueSlime && m_enemyType != EnemyType::YellowSlime && m_enemyType != EnemyType::Mimic)
					{
						SetRandomLookMode();
					}

					if (m_enemyType == EnemyType::RangedSkeleton)
					{
						m_isChargingAttack = false;
						m_chargeAmount = 0.0f;

						UnloadWeapon(false);

						if (m_isIdle)
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose", 0.2f);
						}
						else
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "BindPose", 0.2f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "BindPose", 0.2f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "BindPose", 0.2f);
						}
					}
				}
			}
			else
			{
				m_aggroResetTimer -= dt;
			}
		}

		// Attack delay
		if (m_insideAttackRadius)
		{
			if (m_attackDelayTimer > 0.0f)
			{
				m_attackDelayTimer -= dt;
			}
		}

		// Movement wait after attack
		if (m_movementWaitAfterAttackTimer > 0.0f)
		{
			m_movementWaitAfterAttackTimer -= dt;
		}

		// Jump timer
		if (m_jumpTimer >= 0.0f)
		{
			m_jumpTimer -= dt;
		}

		// Movement freeze
		if (m_movementFreezeTimer >= 0.0f)
		{
			m_movementFreezeTimer -= dt;
		}

		// Spawning
		if (m_spawning)
		{
			if (m_spawningTimer >= 0.0f)
			{
				m_spawningTimer -= dt;
			}
			else
			{
				if (m_enemyType == EnemyType::Doppelganger)
				{
					SetAnimationSpeed(0.35f, true, AnimationSections::FullBody);
				}

				m_pVoxelCharacter->SetCharacterScale(m_renderScale);

				SetLeashParameters(GetCenter(), m_leashRadius);
				ResetRandomTargetPosition();

				m_spawning = false;
			}
		}

		// Idle animation wait timer
		if (m_waitBeforeStopMovingAnimationTimer >= 0.0f)
		{
			m_waitBeforeStopMovingAnimationTimer -= dt;
		}

		// Stuck timer
		if (m_updateStuckTimer)
		{
			if (m_stuckTimer > 0.0f)
			{
				m_stuckTimer -= dt;
			}
			else
			{
				if (m_aggro == false)
				{
					m_stuck = true;
					GetNewRandomTargetPosition();
				}
			}
		}
	}

	// Damage and knockback timers
	if (m_damageTimer > 0.0f)
	{
		m_damageTimer -= dt;
	}
	if (m_knockbackTimer > 0.0f)
	{
		m_knockbackTimer -= dt;
	}
}

// Rendering
void Enemy::Render(bool outline, bool reflection, bool silhouette) const
{
	if (m_pVoxelCharacter != nullptr)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);

		Color outlineColor(1.0f, 0.0f, 0.0f, 1.0f);

		m_pVoxelCharacter->Render(outline, reflection, silhouette, outlineColor, false);
		m_pVoxelCharacter->RenderWeapons(outline, reflection, silhouette, outlineColor);

		m_pRenderer->PopMatrix();
	}
}

void Enemy::RenderFace() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);

	m_pVoxelCharacter->RenderFace();
	
	m_pRenderer->PopMatrix();
}

void Enemy::RenderWeaponTrails() const
{
	if (m_pVoxelCharacter != nullptr)
	{
		m_pRenderer->PushMatrix();
		
		m_pVoxelCharacter->RenderWeaponTrails();
	
		m_pRenderer->PopMatrix();
	}
}

void Enemy::RenderDebug() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);

	m_pRenderer->PushMatrix();

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);

	m_pRenderer->RotateWorldMatrix(0.0f, GetRotation(), 0.0f);

	m_pRenderer->DrawSphere(m_radius, 20, 20);

	m_pRenderer->PopMatrix();

	// Attack segment
	if (m_attackEnabled && (m_attackEnabledDelayTimer <= 0.0f))
	{
		m_pRenderer->PushMatrix();

		float angle = acos(m_attackSegmentAngle);

		m_pRenderer->ImmediateColorAlpha(1.0f, 0.15f, 0.05f, 1.0f);
		m_pRenderer->RotateWorldMatrix(0.0f, GetRotation() - 90.0f + m_attackRotation, 0.0f);
		m_pRenderer->TranslateWorldMatrix(0.0f, 0.25f, 0.0f);
		m_pRenderer->DrawCircleSector(m_attackRadius, angle, 10);

		m_pRenderer->PopMatrix();
	}

	m_pRenderer->PopMatrix();

	// Bones
	if (m_pVoxelCharacter != nullptr)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);

		m_pVoxelCharacter->RenderBones();

		m_pRenderer->PopMatrix();
	}

	// Forwards
	RenderForwardDebug();

	// Random position
	RenderRandomPositionDebug();

	// Charging attack
	RenderChargingAttackDebug();

	// Looking segment for aggro checking
	RenderLookingAreaDebug();

	// Projectile hit box
	RenderProjectileHitboxDebug();
}

void Enemy::RenderForwardDebug() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);

	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->SetLineWidth(3.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(m_forward.x * 1.5f, m_forward.y * 1.5f, m_forward.z * 1.5f);

	m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 0.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(m_targetForward.x * 1.5f, m_targetForward.y * 1.5f, m_targetForward.z * 1.5f);
	
	m_pRenderer->DisableImmediateMode();
	
	m_pRenderer->PopMatrix();

	if (m_pVoxelCharacter != nullptr)
	{
		m_pRenderer->PushMatrix();
		
		m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);
		m_pVoxelCharacter->RenderFacingDebug();
		
		m_pRenderer->PopMatrix();
	}
}

void Enemy::RenderRandomPositionDebug() const
{
	if (m_randomTargetPositionMode == false)
	{
		return;
	}

	m_pRenderer->PushMatrix();

	m_pRenderer->SetRenderMode(RenderMode::SOLID);

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(GetCenter().x, GetCenter().y, GetCenter().z);
	m_pRenderer->ImmediateVertex(m_randomTargetPosition.x, m_randomTargetPosition.y, m_randomTargetPosition.z);

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
	m_pRenderer->ImmediateVertex(m_randomTargetPosition.x, m_randomTargetPosition.y - 0.5f, m_randomTargetPosition.z);
	m_pRenderer->ImmediateVertex(m_randomTargetPosition.x, m_randomTargetPosition.y + 0.5f, m_randomTargetPosition.z);
	m_pRenderer->DisableImmediateMode();

	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(m_leashPosition.x, m_leashPosition.y, m_leashPosition.z);
	m_pRenderer->DrawLineCircle(m_leashRadius, 20);
	
	m_pRenderer->PopMatrix();
	
	m_pRenderer->PopMatrix();
}

void Enemy::RenderChargingAttackDebug() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);
	m_pRenderer->TranslateWorldMatrix(0.0f, 0.5f, 0.0f);

	m_pRenderer->ScaleWorldMatrix(m_pVoxelCharacter->GetCharacterScale(), m_pVoxelCharacter->GetCharacterScale(), m_pVoxelCharacter->GetCharacterScale());

	float chargingMultiply = m_chargeAmount * 30.0f;
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->SetLineWidth(3.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);
	
	m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(m_forward.x * chargingMultiply, m_forward.y * chargingMultiply, m_forward.z * chargingMultiply);
	
	m_pRenderer->DisableImmediateMode();
	
	m_pRenderer->PopMatrix();

	m_pRenderer->PushMatrix();
	
	glm::vec3 startPos = m_chargeSpawnPosition;
	glm::vec3 accel = (m_gravityDirection * 9.81f) * 2.5f;
	glm::vec3 vel = m_chargeSpawnVelocity;
	glm::vec3 pos1 = startPos;
	glm::vec3 pos2 = pos1;
	float deltaTime = 0.04f;

	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);
	m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 1.0f, 1.0f);
	
	for (int i = 0; i < 50; ++i)
	{
		vel += accel * deltaTime;
		pos2 += vel * deltaTime;

		m_pRenderer->ImmediateVertex(pos1.x, pos1.y, pos1.z);
		m_pRenderer->ImmediateVertex(pos2.x, pos2.y, pos2.z);

		pos1 = pos2;
	}

	m_pRenderer->DisableImmediateMode();
	
	m_pRenderer->PopMatrix();
}

void Enemy::RenderLookingAreaDebug() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);

	if (m_aggro)
	{
		m_pRenderer->ImmediateColorAlpha(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	}

	float facelookingAngle = GetFaceLookingRotation();
	float angle = acos(m_aggroLookSegmentAngle);

	m_pRenderer->RotateWorldMatrix(0.0f, GetRotation() - 90.0f + facelookingAngle, 0.0f);
	m_pRenderer->DrawCircleSector(m_aggroLookRadius, angle, 10);
	
	m_pRenderer->PopMatrix();
}

void Enemy::RenderProjectileHitboxDebug() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetProjectileHitboxCenter().x, GetProjectileHitboxCenter().y, GetProjectileHitboxCenter().z);

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 1.0f, 1.0f);

	if (m_projectileHitboxType == ProjectileHitboxType::Sphere)
	{
		m_pRenderer->DrawSphere(m_projectileHitboxRadius, 20, 20);
	}
	else if (m_projectileHitboxType == ProjectileHitboxType::Cube)
	{
		float length = m_projectileHitboxXLength;
		float height = m_projectileHitboxYLength;
		float width = m_projectileHitboxZLength;

		m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
		m_pRenderer->SetCullMode(CullMode::NOCULL);
		m_pRenderer->SetLineWidth(1.0f);

		m_pRenderer->RotateWorldMatrix(0.0f, GetRotation(), 0.0f);

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

		m_pRenderer->SetCullMode(CullMode::BACK);
	}

	m_pRenderer->PopMatrix();
}

// Callback functions
void Enemy::_AttackEnabledTimerFinished(void* pData)
{
	Enemy* pEnemy = static_cast<Enemy*>(pData);
	pEnemy->AttackEnabledTimerFinished();
}

void Enemy::AttackEnabledTimerFinished()
{
	m_attackEnabled = false;

	if (m_enemyType == EnemyType::Bee || m_enemyType == EnemyType::Bat)
	{
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "Fly", 0.05f);
	}
}

void Enemy::_AttackEnabledDelayTimerFinished(void* pData)
{
	Enemy* pEnemy = static_cast<Enemy*>(pData);
	pEnemy->AttackEnabledDelayTimerFinished();
}

void Enemy::AttackEnabledDelayTimerFinished()
{
	if (m_erase)
	{
		return;
	}

	m_attackEnabledDelayTimer = 0.0f;

	if (m_enemyType == EnemyType::NormalSkeleton)
	{
		glm::vec3 boneSpawnPosition = GetCenter() + (m_forward * 0.75f) + (GetRightVector() * -0.4f) + (GetUpVector() * 0.5f);

		glm::vec3 toTarget;
		if (m_pTargetNPC != nullptr)
		{
			toTarget = m_pTargetNPC->GetCenter() - GetCenter();
		}
		else
		{
			toTarget = m_pPlayer->GetCenter() - GetCenter();
		}
		float liftAmount = 1.75f * (length(toTarget) * 0.065f);
		glm::vec3 boneSpawnVelocity = (normalize(toTarget) * 15.0f) + glm::vec3(0.0f, liftAmount, 0.0f);

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(boneSpawnPosition, boneSpawnVelocity, 0.0f, "Resources/gamedata/items/Bone/Bone.item", 0.04f);
		pProjectile->SetProjectileType(false, true, true);
		pProjectile->SetOwner(nullptr, nullptr, this);
		pProjectile->SetGravityMultiplier(0.35f);
	}
	else if (m_enemyType == EnemyType::MageSkeleton)
	{
		glm::vec3 fireballSpawnPosition = GetCenter() + (m_forward * 0.75f) + (GetRightVector() * -0.4f) + (GetUpVector() * 0.25f);

		float powerAmount = 25.0f;
		glm::vec3 toTarget;
		if (m_pTargetNPC != nullptr)
		{
			toTarget = m_pTargetNPC->GetCenter() - GetCenter();
		}
		else
		{
			toTarget = m_pPlayer->GetCenter() - GetCenter();
		}

		glm::vec3 fireballSpawnVelocity = normalize(toTarget) * powerAmount;

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(fireballSpawnPosition, fireballSpawnVelocity, 0.0f, "Resources/gamedata/items/Fireball/Fireball.item", 0.04f);
		pProjectile->SetProjectileType(false, true, true);
		pProjectile->SetOwner(nullptr, nullptr, this);
		pProjectile->SetGravityMultiplier(0.0f);
	}
	else if (m_enemyType == EnemyType::IronGiant)
	{
		glm::vec3 fireballSpawnPosition = GetCenter() + (m_forward * 0.75f) + (GetUpVector() * 5.0f);

		float powerAmount = 25.0f;
		glm::vec3 toTarget;
		if (m_pTargetNPC != nullptr)
		{
			toTarget = m_pTargetNPC->GetCenter() - fireballSpawnPosition;
		}
		else
		{
			toTarget = m_pPlayer->GetCenter() - fireballSpawnPosition;
		}
		glm::vec3 fireballSpawnVelocity = normalize(toTarget) * powerAmount;

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(fireballSpawnPosition, fireballSpawnVelocity, 0.0f, "Resources/gamedata/items/Fireball/Fireball.item", 0.04f);
		pProjectile->SetProjectileType(false, true, true);
		pProjectile->SetOwner(nullptr, nullptr, this);
		pProjectile->SetGravityMultiplier(0.0f);
	}
	else if (m_enemyType == EnemyType::Bat)
	{
		glm::vec3 boneSpawnPosition = GetCenter() + (m_forward * 1.25f);

		glm::vec3 toTarget;
		if (m_pTargetNPC != nullptr)
		{
			toTarget = m_pTargetNPC->GetCenter() - GetCenter();
		}
		else
		{
			toTarget = m_pPlayer->GetCenter() - GetCenter();
		}
		glm::vec3 boneSpawnVelocity = normalize(toTarget) * 15.0f;

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(boneSpawnPosition, boneSpawnVelocity, 0.0f, "Resources/gamedata/items/Batshot/Batshot.item", 0.06f);
		pProjectile->SetProjectileType(false, true, true);
		pProjectile->SetOwner(nullptr, nullptr, this);
		pProjectile->SetGravityMultiplier(0.0f);
	}
}