/*************************************************************************
> File Name: Player.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The player class contains all the functionality of a player, an entity that
>    is controlled and can act within the world and game.
> Created Time: 2016/08/04
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Player.h"
#include <Maths/3DMaths.h>

// Global constants
const glm::vec3 Player::PLAYER_CENTER_OFFSET = glm::vec3(0.0f, 1.525f, 0.0f);

// Constructor, Destructor
Player::Player(Renderer* pRenderer, ChunkManager* pChunkManager, QubicleBinaryManager* pQubicleBinaryManager, LightingManager* pLightingManager, BlockParticleManager* pBlockParticleManager) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager), m_pQubicleBinaryManager(pQubicleBinaryManager),
	m_pLightingManager(pLightingManager), m_pBlockParticleManager(pBlockParticleManager),
	m_pInventoryManager(nullptr), m_pItemManager(nullptr),
	m_pInventoryGUI(nullptr), m_pCharacterGUI(nullptr), m_pCraftingGUI(nullptr),
	m_pLootGUI(nullptr), m_pActionBar(nullptr)
{
	// Create voxel character
	m_pVoxelCharacter = new VoxelCharacter(m_pRenderer, m_pQubicleBinaryManager);
	m_pCharacterBackup = new QubicleBinary(m_pRenderer);

	// Reset player
	ResetPlayer();

	// Load default character model
	SetName("Steve");
	LoadCharacter("Steve", false);
}

Player::~Player()
{
	ResetPlayer();

	delete m_pPlayerStats;

	if (m_pCharacterBackup != nullptr)
	{
		m_pCharacterBackup->SetNullLinkage(m_pVoxelCharacter->GetQubicleModel());
	}
	
	delete m_pVoxelCharacter;

	if (m_pCharacterBackup != nullptr)
	{
		delete m_pCharacterBackup;
	}
}

// Linkage
void Player::SetInventoryManager(InventoryManager* pInventoryManager)
{
	m_pInventoryManager = pInventoryManager;
}

void Player::SetItemManager(ItemManager* pItemManager)
{
	m_pItemManager = pItemManager;
}

void Player::SetProjectileManager(ProjectileManager* pProjectileManager)
{
	m_pProjectileManager = pProjectileManager;
}

void Player::SetTextEffectsManager(TextEffectsManager* pTextEffectsManager)
{
	m_pTextEffectsManager = pTextEffectsManager;
}

void Player::SetEnemyManager(EnemyManager* pEnemyManager)
{
	m_pEnemyManager = pEnemyManager;
}

void Player::SetInventoryGUI(InventoryGUI* pInventoryGUI)
{
	m_pInventoryGUI = pInventoryGUI;
}

void Player::SetCharacterGUI(CharacterGUI* pCharacterGUI)
{
	m_pCharacterGUI = pCharacterGUI;
}

void Player::SetCraftingGUI(CraftingGUI* pCraftingGUI)
{
	m_pCraftingGUI = pCraftingGUI;
}

void Player::SetLootGUI(LootGUI* pLootGUI)
{
	m_pLootGUI = pLootGUI;
}

void Player::SetActionBar(ActionBar* pActionBar)
{
	m_pActionBar = pActionBar;
}

// Get voxel character pointer
VoxelCharacter* Player::GetVoxelCharacter() const
{
	return m_pVoxelCharacter;
}

// Player reset
void Player::ResetPlayer()
{
	m_class = PlayerClass::Debug;

	m_forward = glm::vec3(0.0f, 0.0f, 1.0f);
	m_right = glm::vec3(1.0f, 0.0f, 0.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	m_targetForward = m_forward;

	m_position = glm::vec3(8.0f, 8.0f, 8.0f);
	m_respawnPosition = m_position + glm::vec3(0.0f, 0.1f, 0.0f);
	m_gravityDirection = glm::vec3(0.0f, -1.0f, 0.0f);

	// Stepping up single world blocks by walking into them
	m_doStepUpAnimation = false;
	m_stepUpAnimationYAmount = 0.0f;
	m_stepUpAnimationPrevious = 0.0f;
	m_stepUpAnimationYOffset = 0.0f;
	m_stepUpAdditionYAmountChangedCache = 0.0f;

	// Grid positioning
	m_gridPositionX = 0;
	m_gridPositionY = 0;
	m_gridPositionZ = 0;

	m_pCachedGridChunk = nullptr;

	// Ground check
	m_isOnGround = false;
	m_groundCheckTimer = 0.0f;

	// Floor particles
	m_floorParticleTimer = 0.25f;

	// Jumping
	m_canJump = true;
	m_jumpTimer = 0.0f;

	// Dead flag
	m_dead = false;

	// Ghost
	m_createGhost = false;
	m_createGhostTimer = 0.0f;

	// Health
	m_maxHealth = 100.0f;
	m_health = m_maxHealth;
	m_maxMagic = 100.0f;
	m_magic = m_maxMagic;

	// Damage
	m_damageTime = 0.4f;
	m_damageTimer = m_damageTime;
	m_knockbackTime = 0.4f;
	m_knockbackTimer = m_knockbackTime;
	m_hitFacialExpressionTime = 0.4f;
	m_hitFacialExpressionTimer = m_hitFacialExpressionTime;

	// Target enemy
	m_pTargetEnemy = nullptr;

	// Idle flag
	m_isIdle = true;

	// Crafting
	m_crafting = false;
	m_workingAnimationWaitTimer = 0.0f;
	m_workingAnimationDelay = 0.55f;
	m_createdAnvilHitParticleEffect = true;

	// Look at point
	m_lookAtPoint = false;
	m_bodyTurnSpeedMultiplier = 3.5f;
	m_bodyTurnStopThreshold = 0.35f;

	// Moving to target position, for item interaction points, and NPC dialog
	m_moveToTargetPosition = false;

	// Combat
	m_canAttackLeft = true;
	m_canAttackRight = true;
	m_canInteruptCombatAnim = true;
	m_canThrowWeapon = true;
	m_bowAttackDelay = 0.0f;
	m_attackEnabled = false;
	m_attackEnabledTimer = 0.0f;
	m_attackEnabledDelayTimer = 0.0f;
	m_attackSegmentAngle = 0.75f;
	m_attackRadius = 1.75f;
	m_attackRotation = 0.0f;
	m_canThrowWeapon = true;

	// Projectile hitbox
	m_projectileHitboxType = ProjectileHitboxType::Cube;
	m_projectileHitboxXLength = 0.65f;
	m_projectileHitboxYLength = 1.25f;
	m_projectileHitboxZLength = 0.65f;
	m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.75f, 0.0f);

	// Charging attacks
	m_isChargingAttack = false;
	m_chargeAmount = 0.0f;
	m_chargeTime = 1.0f;

	// Block selection
	m_blockSelection = false;

	// Player stats
	m_pPlayerStats = new PlayerStats(this);
	m_strengthModifier = 0;
	m_dexterityModifier = 0;
	m_intelligenceModifier = 0;
	m_vitalityModifier = 0;
	m_armorModifier = 0;
	m_luckModifier = 0;

	// Initial equipped state
	m_equippedProperties = 0;
	SetNormal();

	// Animation params
	for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
	{
		m_animationFinished[i] = false;
	}
	m_animationTimer = 0.0f;

	m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose");

	UnloadWeapon(true);
	UnloadWeapon(false);
}

// Accessors / Setters
void Player::SetClass(PlayerClass ePlayerClass)
{
	m_class = ePlayerClass;

	m_pPlayerStats->SetClass(m_class);
}

PlayerClass Player::GetClass() const
{
	return m_class;
}

void Player::SetName(std::string name)
{
	m_name = name;
}

std::string Player::GetName() const
{
	return m_name;
}

void Player::SetType(std::string typeName)
{
	m_type = typeName;
}

std::string Player::GetType() const
{
	return m_type;
}

void Player::SetModelname(std::string modelName)
{
	m_modelName = modelName;
}

std::string Player::GetModelName() const
{
	return m_modelName;
}

void Player::SetRespawnPosition(glm::vec3 pos)
{
	m_respawnPosition = pos;
}

glm::vec3 Player::GetRespawnPosition() const
{
	return m_respawnPosition;
}

glm::vec3 Player::GetCenter() const
{
	glm::vec3 center = m_position + m_up * m_radius;
	center -= glm::vec3(0.0f, (m_doStepUpAnimation ? 0.0f : m_stepUpAnimationYOffset), 0.0f);

	return center;
}

glm::vec3 Player::GetForwardVector() const
{
	return m_forward;
}

glm::vec3 Player::GetRightVector() const
{
	return m_right;
}

glm::vec3 Player::GetUpVector() const
{
	return m_up;
}

float Player::GetRadius() const
{
	return m_radius;
}

void Player::UpdateRadius()
{
	m_radius = m_pVoxelCharacter->GetCharacterScale() / 0.14f;
}

void Player::SetForwardVector(glm::vec3 forward)
{
	m_forward = normalize(forward);
	m_right = normalize(cross(m_forward, m_up));

	m_targetForward = m_forward;
	m_targetForward.y = 0.0f;
	m_targetForward = normalize(m_targetForward);
}

void Player::SetRotation(float rot)
{
	float angle = DegreeToRadian(rot);
	m_forward = glm::vec3(sin(angle), 0.0f, cos(angle));

	m_targetForward = m_forward;
}

float Player::GetRotation() const
{
	float rotationAngle = acos(dot(glm::vec3(0.0f, 0.0f, 1.0f), m_forward));
	rotationAngle = RadianToDegree(rotationAngle);
	if (m_forward.x < 0.0f)
	{
		rotationAngle = (360.0f - rotationAngle);
	}

	return rotationAngle;
}

// Loading
void Player::LoadCharacter(std::string characterName, bool fromCharacterSelectScreen)
{
	m_pVoxelCharacter->UnloadCharacter();
	m_pVoxelCharacter->Reset();

	m_type = "Human";
	m_modelName = characterName;

	char characterBaseFolder[128];
	char qbFileName[128];
	char ms3dFileName[128];
	char animListFileName[128];
	char facesFileName[128];
	char characterFileName[128];

	if (fromCharacterSelectScreen)
	{
		sprintf(characterBaseFolder, "Resources/gamedata/models");
		sprintf(qbFileName, "Saves/characters/%s/%s.qb", characterName.c_str(), characterName.c_str());
		sprintf(ms3dFileName, "Resources/gamedata/models/%s/%s.ms3d", m_type.c_str(), m_type.c_str());
		sprintf(animListFileName, "Resources/gamedata/models/%s/%s.animlist", m_type.c_str(), m_type.c_str());
		sprintf(facesFileName, "Saves/characters/%s/%s.faces", characterName.c_str(), characterName.c_str());
		sprintf(characterFileName, "Saves/characters/%s/%s.character", characterName.c_str(), characterName.c_str());
	}
	else
	{
		sprintf(characterBaseFolder, "Resources/gamedata/models");
		sprintf(qbFileName, "Resources/gamedata/models/%s/%s.qb", m_type.c_str(), m_modelName.c_str());
		sprintf(ms3dFileName, "Resources/gamedata/models/%s/%s.ms3d", m_type.c_str(), m_type.c_str());
		sprintf(animListFileName, "Resources/gamedata/models/%s/%s.animlist", m_type.c_str(), m_type.c_str());
		sprintf(facesFileName, "Resources/gamedata/models/%s/%s.faces", m_type.c_str(), m_modelName.c_str());
		sprintf(characterFileName, "Resources/gamedata/models/%s/%s.character", m_type.c_str(), m_modelName.c_str());
	}

	m_pVoxelCharacter->LoadVoxelCharacter(m_type.c_str(), qbFileName, ms3dFileName, animListFileName, facesFileName, characterFileName, characterBaseFolder, true);

	m_pVoxelCharacter->SetBreathingAnimationEnabled(true);
	m_pVoxelCharacter->SetWinkAnimationEnabled(true);
	m_pVoxelCharacter->SetTalkingAnimationEnabled(false);
	m_pVoxelCharacter->SetRandomMouthSelection(true);
	m_pVoxelCharacter->SetRandomLookDirection(true);
	m_pVoxelCharacter->SetWireFrameRender(false);
	m_pVoxelCharacter->SetCharacterScale(0.08f);

	if (m_pCharacterBackup != nullptr)
	{
		delete m_pCharacterBackup;
		m_pCharacterBackup = new QubicleBinary(m_pRenderer);
	}

	m_pCharacterBackup->Import(qbFileName, true);

	UpdateRadius();
}

// Unloading
void Player::LoadWeapon(bool left, std::string weaponFile) const
{
	if (left)
	{
		if (m_pVoxelCharacter->GetLeftWeapon() != nullptr)
		{
			// Load the weapon file
			m_pVoxelCharacter->LoadLeftWeapon(weaponFile.c_str());
		}
	}
	else
	{
		if (m_pVoxelCharacter->GetRightWeapon() != nullptr)
		{
			// Load the weapon file
			m_pVoxelCharacter->LoadRightWeapon(weaponFile.c_str());
		}
	}
}

void Player::UnloadWeapon(bool left)
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
					unsigned int lId;
					m_pLightingManager->AddDyingLight(glm::vec3(lightPos.x, lightPos.y, lightPos.z), lightRadius * scale, lightDiffuseMultiplier, lightColor, 2.0f, &lId);
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