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

#include <CubbyGame.h>

#include <Items/ItemManager.h>
#include <Maths/3DMaths.h>
#include <Models/VoxelObject.h>
#include <Sounds/SoundEffects.h>
#include <Utils/Interpolator.h>
#include <Utils/Random.h>

#include "Player.h"

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

void Player::SetPosition(glm::vec3 pos)
{
	m_position = pos;
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

void Player::UnloadWeapon(bool left) const
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

// Equipping items
void Player::EquipItem(InventoryItem* pItem)
{
	if (m_crafting)
	{
		// Don't allow to change the equipped items if we are crafting
		return;
	}

	bool isEquipWeapon = false;
	switch (pItem->m_equipSlot)
	{
	case EquipSlot::LeftHand:
	{
		LoadWeapon(true, pItem->m_fileName);

		switch (pItem->m_itemType)
		{
		case InventoryType::WeaponDagger:
		{
			SetDagger(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("TargetPose", true);
			break;
		}
		case InventoryType::WeaponShield:
		{
			SetShield(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("TargetPose", true);
			break;
		}
		case InventoryType::WeaponBow:
		{
			SetBow(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("BindPose", true);
			break;
		}
		case InventoryType::WeaponTorch:
		{
			SetTorch(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("BindPose", true);
			break;
		}
		case InventoryType::WeaponSpellHands:
		{
			SetSpellHands(true);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "HandSpellCastPose", 0.25f);
			m_pVoxelCharacter->SetQubicleMatrixRender("Left_Hand", false);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("HandSpellCastPose", true);
			break;
		}
		case InventoryType::Nothing:
		case InventoryType::Item:
		case InventoryType::Scenery:
		case InventoryType::Block:
		case InventoryType::Clothing:
		case InventoryType::WeaponSword:
		case InventoryType::WeaponAxe:
		case InventoryType::WeaponHammer:
		case InventoryType::WeaponMace:
		case InventoryType::WeaponSickle:
		case InventoryType::WeaponTwoHandedSword:
		case InventoryType::WeaponBoomerang:
		case InventoryType::WeaponBomb:
		case InventoryType::WeaponStaff:
		case InventoryType::WeaponWand:
		case InventoryType::WeaponPickaxe:
		case InventoryType::Consumable:
		default:
			throw std::logic_error("Invalid inventory type in Player::EquipItem()");
		}

		// Add a quiver to the players back
		if (pItem->m_itemType == InventoryType::WeaponBow)
		{
			VoxelWeapon* pNewEquipment = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
			pNewEquipment->SetVoxelCharacterParent(nullptr);
			pNewEquipment->LoadWeapon("Resources/gamedata/items/Quiver/Quiver.item", false);

			pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Quiver", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
			QubicleMatrix* pQuiverMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Quiver");
			pQuiverMatrix->m_boneIndex = m_pVoxelCharacter->GetBodyBoneIndex();
			m_pVoxelCharacter->AddQubicleMatrix(pQuiverMatrix, false);
		}

		isEquipWeapon = true;
	}
	break;
	case EquipSlot::RightHand:
	{
		LoadWeapon(false, pItem->m_fileName);

		switch (pItem->m_itemType)
		{
		case InventoryType::Item:
		{
			SetItemPlacing(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("BindPose", false);
			break;
		}
		case InventoryType::Scenery:
		{
			SetSceneryPlacing(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("BindPose", false);
			break;
		}
		case InventoryType::Block:
		{
			SetBlockPlacing(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("BindPose", false);
			break;
		}
		case InventoryType::WeaponSword:
		{
			SetSword(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("TargetPose", false);
			break;
		}
		case InventoryType::WeaponDagger:
		{
			SetDagger(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("TargetPose", false);
			break;
		}
		case InventoryType::WeaponAxe:
		{
			SetAxe(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("TargetPose", false);
			break;
		}
		case InventoryType::WeaponHammer:
		{
			SetHammer(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("TargetPose", false);
			break;
		}
		case InventoryType::WeaponMace:
		{
			SetMace(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("TargetPose", false);
			break;
		}
		case InventoryType::WeaponSickle:
		{
			SetSickle(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("TargetPose", false);
			break;
		}
		case InventoryType::WeaponTwoHandedSword:
		{
			SetTwoHandedSword(true);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "2HandedSwordPose", 0.25f);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("2HandedSwordPose", false);
			break;
		}
		case InventoryType::WeaponBoomerang:
		{
			SetBoomerang(true);
			m_canThrowWeapon = true;
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("BindPose", false);
			break;
		}
		case InventoryType::WeaponBomb:
		{
			SetBomb(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("BindPose", false);
			break;
		}
		case InventoryType::WeaponStaff:
		{
			SetStaff(true);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "StaffPose", 0.25f);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("StaffPose", false);
			break;
		}
		case InventoryType::WeaponWand:
		{
			SetWand(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("TargetPose", false);
			break;
		}
		case InventoryType::WeaponPickaxe:
		{
			SetPickaxe(true);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("BindPose", false);
			break;
		}
		case InventoryType::WeaponSpellHands:
		{
			SetSpellHands(true);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "HandSpellCastPose", 0.25f);
			m_pVoxelCharacter->SetQubicleMatrixRender("Right_Hand", false);
			m_pVoxelCharacter->PlayAnimationOnPaperDoll("HandSpellCastPose", false);
			break;
		}
		case InventoryType::Nothing:
		case InventoryType::Clothing:
		case InventoryType::WeaponShield:
		case InventoryType::WeaponBow:
		case InventoryType::WeaponTorch:
		case InventoryType::Consumable:
		default:
			throw std::logic_error("Invalid inventory type in Player::EquipItem()");
		}

		// Set new attack radius
		if (m_pVoxelCharacter->GetRightWeapon() != nullptr)
		{
			m_attackRadius = m_pVoxelCharacter->GetRightWeapon()->GetWeaponRadius();
		}

		isEquipWeapon = true;
	}
	break;
	case EquipSlot::Head:
	{
		if (m_pVoxelCharacter != nullptr)
		{
			VoxelWeapon* pNewEquipment = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
			pNewEquipment->SetVoxelCharacterParent(nullptr);
			pNewEquipment->LoadWeapon(pItem->m_fileName.c_str(), false);

			pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Helm", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
			QubicleMatrix* pHelmMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Helm");
			pHelmMatrix->m_boneIndex = m_pVoxelCharacter->GetHeadBoneIndex();
			m_pVoxelCharacter->AddQubicleMatrix(pHelmMatrix, false);
		}
	}
	break;
	case EquipSlot::Shoulders:
	{
		if (m_pVoxelCharacter != nullptr)
		{
			VoxelWeapon* pNewEquipment = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
			pNewEquipment->SetVoxelCharacterParent(nullptr);
			pNewEquipment->LoadWeapon(pItem->m_fileName.c_str(), false);

			if (pItem->m_right)
			{
				pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Right_Shoulder_Armor", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
				QubicleMatrix* pRightShoulderMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Right_Shoulder_Armor");
				pRightShoulderMatrix->m_boneIndex = m_pVoxelCharacter->GetRightShoulderBoneIndex();
				m_pVoxelCharacter->AddQubicleMatrix(pRightShoulderMatrix, false);
			}

			if (pItem->m_left)
			{
				pNewEquipment->GetAnimatedSection(1)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Left_Shoulder_Armor", pNewEquipment->GetAnimatedSection(1)->renderScale, pNewEquipment->GetAnimatedSection(1)->renderOffset.x, pNewEquipment->GetAnimatedSection(1)->renderOffset.y, pNewEquipment->GetAnimatedSection(1)->renderOffset.z);
				QubicleMatrix* pLeftShoulderMatrix = pNewEquipment->GetAnimatedSection(1)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Left_Shoulder_Armor");
				pLeftShoulderMatrix->m_boneIndex = m_pVoxelCharacter->GetLeftShoulderBoneIndex();
				m_pVoxelCharacter->AddQubicleMatrix(pLeftShoulderMatrix, false);
			}
		}
	}
	break;
	case EquipSlot::Body:
	{
		if (m_pVoxelCharacter != nullptr)
		{
			VoxelWeapon* pNewEquipment = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
			pNewEquipment->SetVoxelCharacterParent(nullptr);
			pNewEquipment->LoadWeapon(pItem->m_fileName.c_str(), false);

			pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Body", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
			QubicleMatrix* pBodyMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Body");
			pBodyMatrix->m_boneIndex = m_pVoxelCharacter->GetBodyBoneIndex();
			m_pVoxelCharacter->AddQubicleMatrix(pBodyMatrix, false);
		}
	}
	break;
	case EquipSlot::Legs:
	{
		if (m_pVoxelCharacter != nullptr)
		{
			VoxelWeapon* pNewEquipment = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
			pNewEquipment->SetVoxelCharacterParent(nullptr);
			pNewEquipment->LoadWeapon(pItem->m_fileName.c_str(), false);

			pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Legs", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
			QubicleMatrix* pLegsMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Legs");
			pLegsMatrix->m_boneIndex = m_pVoxelCharacter->GetLegsBoneIndex();
			m_pVoxelCharacter->AddQubicleMatrix(pLegsMatrix, false);
		}
	}
	break;
	case EquipSlot::Hand:
	{
		if (m_pVoxelCharacter != nullptr)
		{
			VoxelWeapon* pNewEquipment = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
			pNewEquipment->SetVoxelCharacterParent(nullptr);
			pNewEquipment->LoadWeapon(pItem->m_fileName.c_str(), false);

			if (pItem->m_right)
			{
				pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Right_Hand", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
				QubicleMatrix* pRightHandMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Right_Hand");
				pRightHandMatrix->m_boneIndex = m_pVoxelCharacter->GetRightHandBoneIndex();
				m_pVoxelCharacter->AddQubicleMatrix(pRightHandMatrix, false);
			}

			if (pItem->m_left)
			{
				pNewEquipment->GetAnimatedSection(1)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Left_Hand", pNewEquipment->GetAnimatedSection(1)->renderScale, pNewEquipment->GetAnimatedSection(1)->renderOffset.x, pNewEquipment->GetAnimatedSection(1)->renderOffset.y, pNewEquipment->GetAnimatedSection(1)->renderOffset.z);
				QubicleMatrix* pLeftHandMatrix = pNewEquipment->GetAnimatedSection(1)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Left_Hand");
				pLeftHandMatrix->m_boneIndex = m_pVoxelCharacter->GetLeftHandBoneIndex();
				m_pVoxelCharacter->AddQubicleMatrix(pLeftHandMatrix, false);
			}
		}
	}
	break;
	case EquipSlot::Feet:
	{
		if (m_pVoxelCharacter != nullptr)
		{
			VoxelWeapon* pNewEquipment = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
			pNewEquipment->SetVoxelCharacterParent(nullptr);
			pNewEquipment->LoadWeapon(pItem->m_fileName.c_str(), false);

			if (pItem->m_right)
			{
				pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Right_Foot", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
				QubicleMatrix* pRightFootMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Right_Foot");
				pRightFootMatrix->m_boneIndex = m_pVoxelCharacter->GetRightFootBoneIndex();
				m_pVoxelCharacter->AddQubicleMatrix(pRightFootMatrix, false);
			}

			if (pItem->m_left)
			{
				pNewEquipment->GetAnimatedSection(1)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Left_Foot", pNewEquipment->GetAnimatedSection(1)->renderScale, pNewEquipment->GetAnimatedSection(1)->renderOffset.x, pNewEquipment->GetAnimatedSection(1)->renderOffset.y, pNewEquipment->GetAnimatedSection(1)->renderOffset.z);
				QubicleMatrix* pLeftFootMatrix = pNewEquipment->GetAnimatedSection(1)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Left_Foot");
				pLeftFootMatrix->m_boneIndex = m_pVoxelCharacter->GetLeftFootBoneIndex();
				m_pVoxelCharacter->AddQubicleMatrix(pLeftFootMatrix, false);
			}
		}
	}
	break;
	case EquipSlot::Accessory1:
	case EquipSlot::Accessory2:
	break;
	case EquipSlot::NoSlot:
	case EquipSlot::NumSlots:
	default:
		throw std::logic_error("Invalid equip slot in Player::EquipItem()");
	}

	if (isEquipWeapon)
	{
		CubbyGame::GetInstance()->PlaySoundEffect(SoundEffect::EquipSword);
	}
	else
	{
		CubbyGame::GetInstance()->PlaySoundEffect(SoundEffect::EquipCloth);
	}

	if (CubbyGame::GetInstance()->GetCameraMode() == CameraMode::FirstPerson)
	{
		// Make sure we set (force) the initial weapons alpha if we equip while in first person mode.
		m_pVoxelCharacter->SetMeshAlpha(0.45f, true);
	}

	// Refresh stat modifier cache since we have equipped an item
	RefreshStatModifierCacheValues();
}

void Player::UnequipItem(EquipSlot equipSlot, bool left, bool right)
{
	if (m_crafting)
	{
		// Don't allow to change the equipped items if we are crafting
		return;
	}

	switch (equipSlot)
	{
	case EquipSlot::LeftHand:
	{
		UnloadWeapon(true);

		SetBow(false);
		SetShield(false);
		SetTorch(false);

		InventoryItem* pRightHand = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::RightHand);
		if (pRightHand == nullptr || pRightHand->m_itemType != InventoryType::WeaponSpellHands)
		{
			SetSpellHands(false);
		}

		m_pVoxelCharacter->SetQubicleMatrixRender("Left_Hand", true);

		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "BindPose", 0.25f);
		m_pVoxelCharacter->PlayAnimationOnPaperDoll("BindPose", true);

		// Remove quiver
		m_pVoxelCharacter->RemoveQubicleMatrix("Quiver");
	}
	break;
	case EquipSlot::RightHand:
	{
		UnloadWeapon(false);

		SetSword(false);
		SetAxe(false);
		SetHammer(false);
		SetMace(false);
		SetDagger(false);
		SetSickle(false);
		SetTwoHandedSword(false);
		SetBoomerang(false);
		SetBomb(false);
		SetStaff(false);
		SetWand(false);
		SetPickaxe(false);
		SetItemPlacing(false);
		SetSceneryPlacing(false);
		SetBlockPlacing(false);
		SetConsumable(false);

		InventoryItem* pLeftHand = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::LeftHand);
		if (pLeftHand == nullptr || pLeftHand->m_itemType != InventoryType::WeaponSpellHands)
		{
			SetSpellHands(false);
		}

		m_pVoxelCharacter->SetQubicleMatrixRender("Right_Hand", true);

		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "BindPose", 0.25f);
		m_pVoxelCharacter->PlayAnimationOnPaperDoll("BindPose", false);
	}
	break;
	case EquipSlot::Head:
	{
		m_pVoxelCharacter->RemoveQubicleMatrix("Helm");
	}
	break;
	case EquipSlot::Shoulders:
	{
		m_pVoxelCharacter->RemoveQubicleMatrix("Right_Shoulder_Armor");
		m_pVoxelCharacter->RemoveQubicleMatrix("Left_Shoulder_Armor");
	}
	break;
	case EquipSlot::Body:
	{
		QubicleMatrix* pBodyMatrix = m_pCharacterBackup->GetQubicleMatrix("Body");
		pBodyMatrix->m_boneIndex = m_pVoxelCharacter->GetBodyBoneIndex();
		m_pVoxelCharacter->AddQubicleMatrix(pBodyMatrix, false);

		char characterFileName[128];
		sprintf(characterFileName, "Resources/gamedata/characters/%s/%s.character", m_type.c_str(), m_modelName.c_str());
		m_pVoxelCharacter->ResetMatrixParamsFromCharacterFile(characterFileName, "Body");
	}
	break;
	case EquipSlot::Legs:
	{
		QubicleMatrix* pLegsMatrix = m_pCharacterBackup->GetQubicleMatrix("Legs");
		pLegsMatrix->m_boneIndex = m_pVoxelCharacter->GetLegsBoneIndex();
		m_pVoxelCharacter->AddQubicleMatrix(pLegsMatrix, false);

		char characterFileName[128];
		sprintf(characterFileName, "Resources/gamedata/characters/%s/%s.character", m_type.c_str(), m_modelName.c_str());
		m_pVoxelCharacter->ResetMatrixParamsFromCharacterFile(characterFileName, "Legs");
	}
	break;
	case EquipSlot::Hand:
	{
		if (right)
		{
			QubicleMatrix* pRightHandMatrix = m_pCharacterBackup->GetQubicleMatrix("Right_Hand");
			pRightHandMatrix->m_boneIndex = m_pVoxelCharacter->GetRightHandBoneIndex();
			m_pVoxelCharacter->AddQubicleMatrix(pRightHandMatrix, false);
		}

		if (left)
		{
			QubicleMatrix* pLeftHandMatrix = m_pCharacterBackup->GetQubicleMatrix("Left_Hand");
			pLeftHandMatrix->m_boneIndex = m_pVoxelCharacter->GetLeftHandBoneIndex();
			m_pVoxelCharacter->AddQubicleMatrix(pLeftHandMatrix, false);
		}

		char characterFileName[128];
		sprintf(characterFileName, "Resources/gamedata/characters/%s/%s.character", m_type.c_str(), m_modelName.c_str());
		m_pVoxelCharacter->ResetMatrixParamsFromCharacterFile(characterFileName, "Right_Hand");
		m_pVoxelCharacter->ResetMatrixParamsFromCharacterFile(characterFileName, "Left_Hand");
	}
	break;
	case EquipSlot::Feet:
	{
		if (right)
		{
			QubicleMatrix* pRightFootMatrix = m_pCharacterBackup->GetQubicleMatrix("Right_Foot");
			pRightFootMatrix->m_boneIndex = m_pVoxelCharacter->GetRightFootBoneIndex();
			m_pVoxelCharacter->AddQubicleMatrix(pRightFootMatrix, false);
		}

		if (left)
		{
			QubicleMatrix* pLeftFootMatrix = m_pCharacterBackup->GetQubicleMatrix("Left_Foot");
			pLeftFootMatrix->m_boneIndex = m_pVoxelCharacter->GetLeftFootBoneIndex();
			m_pVoxelCharacter->AddQubicleMatrix(pLeftFootMatrix, false);
		}

		char characterFileName[128];
		sprintf(characterFileName, "Resources/gamedata/characters/%s/%s.character", m_type.c_str(), m_modelName.c_str());
		m_pVoxelCharacter->ResetMatrixParamsFromCharacterFile(characterFileName, "Right_Foot");
		m_pVoxelCharacter->ResetMatrixParamsFromCharacterFile(characterFileName, "Left_Foot");
	}
	break;
	case EquipSlot::Accessory1:
	case EquipSlot::Accessory2:
	break;
	case EquipSlot::NoSlot:
	case EquipSlot::NumSlots:
	default:
		throw std::logic_error("Invalid equip slot in Player::UnequipItem()");
	}

	// Refresh stat modifier cache since we have unequipped an item
	RefreshStatModifierCacheValues();
}

void Player::RefreshStatModifierCacheValues()
{
	m_strengthModifier = 0;
	m_dexterityModifier = 0;
	m_intelligenceModifier = 0;
	m_vitalityModifier = 0;
	m_armorModifier = 0;
	m_luckModifier = 0;

	for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
	{
		InventoryItem* pEquippedItem = m_pInventoryManager->GetInventoryItemForEquipSlot(static_cast<EquipSlot>(i));

		if (pEquippedItem != nullptr)
		{
			for (size_t j = 0; j < pEquippedItem->m_vpStatAttributes.size(); ++j)
			{
				switch (pEquippedItem->m_vpStatAttributes[j]->GetType())
				{
				case AttributeType::Strength: { m_strengthModifier += pEquippedItem->m_vpStatAttributes[j]->GetModifyAmount(); break; }
				case AttributeType::Dexterity: { m_dexterityModifier += pEquippedItem->m_vpStatAttributes[j]->GetModifyAmount(); break; }
				case AttributeType::Intelligence: { m_intelligenceModifier += pEquippedItem->m_vpStatAttributes[j]->GetModifyAmount(); break; }
				case AttributeType::Vitality: { m_vitalityModifier += pEquippedItem->m_vpStatAttributes[j]->GetModifyAmount(); break; }
				case AttributeType::Armor: { m_armorModifier += pEquippedItem->m_vpStatAttributes[j]->GetModifyAmount(); break; }
				case AttributeType::Luck: { m_luckModifier += pEquippedItem->m_vpStatAttributes[j]->GetModifyAmount(); break; }
				}
			}
		}
	}

	CubbyGame::GetInstance()->GetCharacterGUI()->UpdatePlayerStats();
}

// Collision
bool Player::CheckCollisions(glm::vec3 positionCheck, glm::vec3 previousPosition, glm::vec3* pNormal, glm::vec3* pMovement, bool *pStepUpBlock)
{
	glm::vec3 movementCache = *pMovement;

	float radius = GetRadius();

	// Item collisions
	bool itemCollision = m_pItemManager->CheckCollisions(positionCheck, previousPosition, radius, pNormal, pMovement);

	// World collision
	bool worldCollision = false;

	glm::vec3 floorPosition;
	if (m_pChunkManager->FindClosestFloor(positionCheck, &floorPosition) == false)
	{
		*pMovement = glm::vec3(0.0f, 0.0f, 0.0f);
		return true;
	}
	
	int blockX, blockY, blockZ;
	glm::vec3 blockPos;
	int blockXAbove, blockYAbove, blockZAbove;
	glm::vec3 blockPosAbove;
	int numChecks = 1 + static_cast<int>(radius / (Chunk::BLOCK_RENDER_SIZE * 2.0f));
	bool canAllStepUp = false;
	bool firstStepUp = true;
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
						worldCollision = true;
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
						if (y == 0) // We only want to check on the same y-level as the players position.
						{
							glm::vec3 posCheck1 = glm::vec3(positionCheck.x + ((Chunk::BLOCK_RENDER_SIZE * 2.0f) * x), positionCheck.y + (Chunk::BLOCK_RENDER_SIZE * 2.0f), positionCheck.z + ((Chunk::BLOCK_RENDER_SIZE * 2.0f) * z));
							glm::vec3 posCheck2 = glm::vec3(positionCheck.x + ((Chunk::BLOCK_RENDER_SIZE * 2.0f) * x), positionCheck.y + (Chunk::BLOCK_RENDER_SIZE * 4.0f), positionCheck.z + ((Chunk::BLOCK_RENDER_SIZE * 2.0f) * z));

							Chunk* pChunkAbove = GetCachedGridChunkOrFromPosition(glm::vec3(posCheck1.x, posCheck1.y, posCheck1.z));
							bool activeAbove = m_pChunkManager->GetBlockActiveFrom3DPosition(posCheck1.x, posCheck1.y, posCheck1.z, &blockPosAbove, &blockXAbove, &blockYAbove, &blockZAbove, &pChunkAbove);
							Chunk* pChunkAbove2 = GetCachedGridChunkOrFromPosition(glm::vec3(posCheck2.x, posCheck2.y, posCheck2.z));
							bool activeAbove2 = m_pChunkManager->GetBlockActiveFrom3DPosition(posCheck2.x, posCheck2.y, posCheck2.z, &blockPosAbove, &blockXAbove, &blockYAbove, &blockZAbove, &pChunkAbove2);

							if ((activeAbove == false) && (activeAbove2 == false))
							{
								if (firstStepUp)
								{
									canAllStepUp = true;
								}
							}
							else
							{
								canAllStepUp = false;
							}

							firstStepUp = false;
						}

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

		*pStepUpBlock = canAllStepUp;
	}

	if (itemCollision)
	{
		return true;
	}

	if (worldCollision)
	{
		return true;
	}

	*pMovement = movementCache;

	return false;
}

// Selection
bool Player::GetSelectionBlock(glm::vec3* blockPos, int* blockX, int* blockY, int* blockZ) const
{
	float distance = 0.0f;
	bool collides = false;
	int interations = 0;
	float increments = 0.025f;

	while (collides == false && interations < 110)
	{
		glm::vec3 testPos = GetCenter() + PLAYER_CENTER_OFFSET + normalize(m_cameraForward) * distance;

		Chunk* pChunk = nullptr;
		bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(testPos.x, testPos.y, testPos.z, blockPos, blockX, blockY, blockZ, &pChunk);
		if (active == true)
		{
			collides = true;
		}

		distance += increments;
		interations++;
	}

	return collides;
}

bool Player::GetPlacementBlock(glm::vec3* blockPos, int* blockX, int* blockY, int* blockZ) const
{
	float distance = 0.0f;
	bool collides = false;
	int interations = 0;
	float increments = 0.025f;

	while (collides == false && interations < 175)
	{
		glm::vec3 testPos = GetCenter() + PLAYER_CENTER_OFFSET + normalize(m_cameraForward) * distance;

		Chunk* pChunk = nullptr;
		bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(testPos.x, testPos.y, testPos.z, blockPos, blockX, blockY, blockZ, &pChunk);
		if (active == true)
		{
			// Get an empty block position
			glm::vec3 emptyPos = testPos - normalize(m_cameraForward) * Chunk::BLOCK_RENDER_SIZE;
			bool active2 = m_pChunkManager->GetBlockActiveFrom3DPosition(emptyPos.x, emptyPos.y, emptyPos.z, blockPos, blockX, blockY, blockZ, &pChunk);
			Chunk* pChunk2 = m_pChunkManager->GetChunkFromPosition(emptyPos.x, emptyPos.y, emptyPos.z);

			if (pChunk2 != nullptr && active2 == false)
			{
				glm::vec3 blockPosTest;
				int blockXTest;
				int blockYTest;
				int blockZTest;
				bool pBlock1 = m_pChunkManager->GetBlockActiveFrom3DPosition(emptyPos.x - (Chunk::BLOCK_RENDER_SIZE*2.0f), emptyPos.y, emptyPos.z, &blockPosTest, &blockXTest, &blockYTest, &blockZTest, &pChunk2);
				bool pBlock2 = m_pChunkManager->GetBlockActiveFrom3DPosition(emptyPos.x + (Chunk::BLOCK_RENDER_SIZE*2.0f), emptyPos.y, emptyPos.z, &blockPosTest, &blockXTest, &blockYTest, &blockZTest, &pChunk2);
				bool pBlock3 = m_pChunkManager->GetBlockActiveFrom3DPosition(emptyPos.x, emptyPos.y - (Chunk::BLOCK_RENDER_SIZE*2.0f), emptyPos.z, &blockPosTest, &blockXTest, &blockYTest, &blockZTest, &pChunk2);
				bool pBlock4 = m_pChunkManager->GetBlockActiveFrom3DPosition(emptyPos.x, emptyPos.y + (Chunk::BLOCK_RENDER_SIZE*2.0f), emptyPos.z, &blockPosTest, &blockXTest, &blockYTest, &blockZTest, &pChunk2);
				bool pBlock5 = m_pChunkManager->GetBlockActiveFrom3DPosition(emptyPos.x, emptyPos.y, emptyPos.z - (Chunk::BLOCK_RENDER_SIZE*2.0f), &blockPosTest, &blockXTest, &blockYTest, &blockZTest, &pChunk2);
				bool pBlock6 = m_pChunkManager->GetBlockActiveFrom3DPosition(emptyPos.x, emptyPos.y, emptyPos.z + (Chunk::BLOCK_RENDER_SIZE*2.0f), &blockPosTest, &blockXTest, &blockYTest, &blockZTest, &pChunk2);

				// ONLY allow non-diagonal block placements
				if (pBlock1 == true || pBlock2 == true || pBlock3 == true || pBlock4 == true || pBlock5 == true || pBlock6 == true)
				{
					glm::vec3 dist = (*blockPos) - m_position + PLAYER_CENTER_OFFSET;
					//dist.y = 0.0f;
					if (length(dist) > m_radius)
					{
						collides = true;
					}
				}
			}
		}

		distance += increments;
		interations++;
	}

	return collides;
}

// World
void Player::UpdateGridPosition()
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

int Player::GetGridX() const
{
	return m_gridPositionX;
}

int Player::GetGridY() const
{
	return m_gridPositionY;
}

int Player::GetGridZ() const
{
	return m_gridPositionZ;
}

Chunk* Player::GetCachedGridChunkOrFromPosition(glm::vec3 pos) const
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

void Player::ClearChunkCacheForChunk(Chunk* pChunk)
{
	if (m_pCachedGridChunk == pChunk)
	{
		m_pCachedGridChunk = nullptr;
	}
}

// Camera
void Player::SetCameraPosition(glm::vec3 cameraPos)
{
	m_cameraPosition = cameraPos;
}

void Player::SetCameraForward(glm::vec3 cameraForward)
{
	m_cameraForward = cameraForward;
}

void Player::SetCameraUp(glm::vec3 up)
{
	m_cameraUp = up;
}

void Player::SetCameraRight(glm::vec3 right)
{
	m_cameraRight = right;
}

// Loading configuration and settings for the game
void Player::LoadCharacterSettings()
{
	// Remove supress export
	m_pInventoryManager->SetSupressExport(false);
	m_pActionBar->SetSupressExport(false);
	m_pPlayerStats->SetSupressExport(false);

	m_pInventoryManager->LoadInventory(m_name, m_class, true);
	m_pInventoryManager->ExportInventory(m_name);
	m_pActionBar->ExportActionBar(m_name);

	m_pPlayerStats->ResetStats();
	m_pPlayerStats->ExportStats(m_name);
}

void Player::StartGame()
{
	// Remove supress export
	m_pInventoryManager->SetSupressExport(false);
	m_pActionBar->SetSupressExport(false);
	m_pPlayerStats->SetSupressExport(false);

	// Load inventory from the character's inventory file
	m_pInventoryManager->ImportInventory(m_name);
	m_pInventoryGUI->DeleteInventoryItems();
	m_pInventoryGUI->CreateInventoryItems();
	m_pInventoryManager->SetInventoryGUINeedsUpdate(true);
	m_pInventoryManager->SetCharacterGUINeedsUpdate(true);

	// Load the actionbar
	m_pActionBar->ImportActionBar(m_name);

	// Import the player stats
	m_pPlayerStats->ImportStats(m_name);
	SetClass(m_pPlayerStats->GetClass());

	// Make sure to set the chunk loading from the player position
	int gridX;
	int gridY;
	int gridZ;
	m_pChunkManager->GetGridFromPosition(m_position, &gridX, &gridY, &gridZ);

	// Update initial HUD for player
	CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();
}

// Movement
glm::vec3 Player::GetPositionMovementAmount() const
{
	return m_positionMovementAmount;
}

glm::vec3 Player::MoveAbsolute(glm::vec3 direction, const float speed, bool shouldChangeForward)
{
	if (shouldChangeForward)
	{
		m_forward = normalize(direction);
	}

	m_targetForward = m_forward;
	m_targetForward.y = 0.0f;
	m_targetForward = normalize(m_targetForward);

	glm::vec3 totalAmountMoved;

	glm::vec3 movement = direction;
	glm::vec3 movementAmount = direction * speed;
	glm::vec3 pNormal;
	int numberDivision = 1;
	while (length(movementAmount) >= 1.0f)
	{
		numberDivision++;
		movementAmount = direction * (speed / numberDivision);
	}
	for (int i = 0; i < numberDivision; ++i)
	{
		float speedToUse = (speed / numberDivision) + ((speed / numberDivision) * i);
		glm::vec3 posToCheck = GetCenter() + movement * speedToUse;

		if (m_doStepUpAnimation == false)
		{
			bool stepUp = false;
			if (CheckCollisions(posToCheck, m_previousPosition, &pNormal, &movement, &stepUp))
			{
				if (m_isOnGround == true && m_canJump == true)  // Only allow step up block animations when we are on the ground and also when we can jump. i.e not in air.
				{
					if (stepUp)
					{
						if (m_doStepUpAnimation == false)
						{
							m_doStepUpAnimation = true;

							m_stepUpAnimationYAmount = 0.0f;
							m_stepUpAnimationPrevious = 0.0f;
							m_stepUpAnimationYOffset = 0.0f;
							Interpolator::GetInstance()->AddFloatInterpolation(&m_stepUpAnimationYAmount, 0.0f, (Chunk::BLOCK_RENDER_SIZE * 2.2f), 0.1f, 0.0f, nullptr, _StepUpAnimationFinished, this);
							Interpolator::GetInstance()->AddFloatInterpolation(&m_stepUpAnimationYOffset, (Chunk::BLOCK_RENDER_SIZE * 2.2f), 0.0f, 0.125f, -100.0f);
						}
					}
				}
			}
		}

		m_position += (movement * speedToUse);

		totalAmountMoved += (movement * speedToUse);
	}

	// Change to run animation
	if (m_isChargingAttack == false)
	{
		if (m_pTargetEnemy == nullptr)
		{
			if (IsStaff())
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::LegsFeet, "StaffRun", 0.01f);
			}
			else
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::LegsFeet, "Run", 0.01f);
			}
		}
		else
		{
			if (m_isIdle)
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "Run", 0.01f);
			}
			else
			{
				if (m_animationFinished[static_cast<int>(AnimationSections::HeadBody)])
				{
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "TargetPose", 0.1f);
				}
				if (m_animationFinished[static_cast<int>(AnimationSections::LeftArmHand)] && m_canAttackLeft)
				{
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "TargetPose", 0.1f);
				}
				if (m_animationFinished[static_cast<int>(AnimationSections::RightArmHand)] && m_canAttackRight)
				{
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "TargetPose", 0.1f);
				}

				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, true, AnimationSections::LegsFeet, "Run", 0.01f);
			}
		}
	}
	else
	{
		if (m_isIdle)
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "Run", 0.01f);
		}
	}

	// Create some floor 'dust' particles as we move
	CreateFloorParticles();

	// Footstep sounds
	m_footstepSoundDistance -= fabs(speed);
	if (m_footstepSoundTimer <= 0.0f && m_footstepSoundDistance <= 0.0f && m_canJump)
	{
		int footStepSound = static_cast<int>(SoundEffect::FootStep01) + m_footstepSoundIndex;
		CubbyGame::GetInstance()->PlaySoundEffect(static_cast<SoundEffect>(footStepSound));
		m_footstepSoundIndex = GetRandomNumber(0, 3);

		m_footstepSoundTimer = 0.3f + GetRandomNumber(-10, 10, 1)*0.002f;
		m_footstepSoundDistance = 1.75f;
	}

	m_isIdle = false;

	return totalAmountMoved;
}

void Player::Move(const float speed)
{
	glm::vec3 movement = m_worldMatrix.GetForwardVector();

	float movementSpeed = speed;

	if (speed < 0.0f)
	{
		movement = -movement;
		movementSpeed = -movementSpeed;
	}

	MoveAbsolute(movement, movementSpeed, false);
}

void Player::Strafe(const float speed)
{
	glm::vec3 crossResult;
	crossResult = cross(m_worldMatrix.GetUpVector(), m_worldMatrix.GetForwardVector());
	crossResult = normalize(crossResult);

	float movementSpeed = speed;

	if (movementSpeed < 0.0f)
	{
		crossResult = -crossResult;
		movementSpeed = -movementSpeed;
	}

	MoveAbsolute(crossResult, movementSpeed, false);
}

void Player::Levitate(const float speed)
{
	m_force += glm::vec3(0.0f, 60.0f, 0.0f);
}

void Player::StopMoving()
{
	if (m_canJump)
	{
		if (m_isIdle == false)
		{
			m_isIdle = true;

			if (m_isChargingAttack == false)
			{
				if (m_pTargetEnemy == nullptr)
				{
					if (CanAttackLeft() && CanAttackRight())
					{
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose", 0.15f);
					}
					else if (CanAttackLeft())
					{
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "BindPose", 0.15f);
					}
					else if (CanAttackRight())
					{
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "BindPose", 0.15f);
					}

					if (m_canInteruptCombatAnim)
					{
						if (IsTwoHandedSword())
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "2HandedSwordPose", 0.15f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "2HandedSwordPose", 0.15f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "2HandedSwordPose", 0.15f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "2HandedSwordPose", 0.15f);
						}
						else if (IsStaff())
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "StaffPose", 0.15f);
						}
						else if (IsSpellHands())
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "BindPose", 0.15f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "BindPose", 0.15f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "HandSpellCastPose", 0.15f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "HandSpellCastPose", 0.15f);
						}
						else
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "BindPose", 0.15f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "BindPose", 0.15f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "BindPose", 0.15f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "BindPose", 0.15f);
						}
					}
					else
					{
						if (IsTwoHandedSword())
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "2HandedSwordPose", 0.15f);
						}
						else if (IsSword())
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "2HandedSwordPose", 0.15f);
						}
						else if (IsStaff())
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "BindPose", 0.15f);
						}
						else if (IsPickaxe())
						{
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "BindPose", 0.15f);
							m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "BindPose", 0.15f);
						}
					}
				}
				else
				{
					if (CanAttackLeft() && CanAttackRight())
					{
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose", 0.15f);
					}
					else if (CanAttackLeft())
					{
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "BindPose", 0.15f);
					}
					else if (CanAttackRight())
					{
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "BindPose", 0.15f);
					}

					if (m_canInteruptCombatAnim)
					{
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "TargetPose", 0.15f);
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "TargetPose", 0.15f);
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "TargetPose", 0.15f);
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "TargetPose", 0.15f);
					}
				}
			}
			else
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "BowStance", 0.15f);
			}
		}
	}
}

void Player::Jump()
{
	if (m_canJump == false && m_pChunkManager->IsUnderWater(GetCenter()) == false)
	{
		return;
	}

	if (m_jumpTimer >= 0.0f)
	{
		return;
	}

	if (m_isOnGround == false && m_pChunkManager->IsUnderWater(GetCenter()) == false)
	{
		return;
	}

	// Don't allow jumping if we are doing a step up animation
	if (m_doStepUpAnimation == true)
	{
		return;
	}

	m_canJump = false;
	m_jumpTimer = 0.3f;

	float jumpMultiplier = 14.0f;
	if (m_pChunkManager->IsUnderWater(GetCenter()))
	{
		jumpMultiplier = 5.0f;
	}

	m_velocity += m_up * jumpMultiplier;

	// Change to jump animation
	if (m_isChargingAttack == false)
	{
		if (CanAttackLeft() && CanAttackRight())
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "Jump", 0.01f);
		}
	}
	else
	{
		if (m_canInteruptCombatAnim)
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "Jump", 0.01f);
		}
	}
}

bool Player::CanJump() const
{
	return m_canJump;
}

void Player::SetMoveToTargetPosition(glm::vec3 pos)
{
	m_moveToTargetPosition = true;
	m_targetPosition = pos;
}

void Player::DisableMoveToTargetPosition()
{
	m_moveToTargetPosition = false;
}

void Player::SetLookAtTargetAfterMoveToPosition(glm::vec3 lookAt)
{
	m_lookAtPositionAfterMoveToTarget = lookAt;
}

void Player::CreateFloorParticles()
{
	if (m_isOnGround == false)
	{
		return;
	}

	if (m_floorParticleTimer > 0.0f)
	{
		return;
	}

	glm::vec3 floorPosition;
	if (m_pChunkManager->FindClosestFloor(GetCenter(), &floorPosition))
	{
		glm::vec3 blockPosition = floorPosition - glm::vec3(0.0f, Chunk::BLOCK_RENDER_SIZE, 0.0f);

		int blockX, blockY, blockZ;
		glm::vec3 blockPos;
		Chunk* pChunk = nullptr;
		bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(blockPosition.x, blockPosition.y, blockPosition.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);

		if (active && pChunk != nullptr)
		{
			float r = 1.0f;
			float g = 1.0f;
			float b = 1.0f;
			float a = 1.0f;
			pChunk->GetColor(blockX, blockY, blockZ, &r, &g, &b, &a);

			glm::vec3 spawnPosition = blockPosition + glm::vec3(0.0f, Chunk::BLOCK_RENDER_SIZE, 0.0f);
			float randomNum = GetRandomNumber(0, 100, 0);
			spawnPosition += GetRightVector() * 0.25f * ((randomNum < 50) ? -1.0f : 1.0f);
			m_pBlockParticleManager->CreateBlockParticle(spawnPosition, spawnPosition, glm::vec3(0.0f, -1.0f, 0.0f), 1.0f, spawnPosition, 0.125f, 0.5f, 0.125f, 0.5f,
				r, g, b, a, 0.0f, 0.0f, 0.0f, 0.0f, r, g, b, a, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f, 0.0f,
				glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(2.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				true, glm::vec3(0.0f, 0.0f, 0.0f), true, false, true, false, nullptr);
		}
	}

	float randomNumber = GetRandomNumber(0, 100, 2);
	m_floorParticleTimer = randomNumber * 0.0025f;
}

// Dead
bool Player::IsDead() const
{
	return m_dead;
}

// Level up
// ReSharper disable once CppMemberFunctionMayBeStatic
void Player::LevelUp() const
{
	CubbyGame::GetInstance()->GetHUD()->LevelUp();
}

// Gameplay
float Player::GetHealth() const
{
	return m_health;
}

float Player::GetMaxHealth() const
{
	return m_maxHealth;
}

float Player::GetMagic() const
{
	return m_magic;
}

float Player::GetMaxMagic() const
{
	return m_maxMagic;
}

void Player::GiveHealth(float amount)
{
	m_health += amount;

	if (m_health >= m_maxHealth)
	{
		m_health = m_maxHealth;
	}

	// Do an animated text effect
	glm::vec3 screenposition = GetCenter() + glm::vec3(GetRandomNumber(-1, 1, 2) * 0.25f, 0.0f, GetRandomNumber(-1, 1, 2) * 0.25f);
	char healthText[32];
	sprintf(healthText, "%i hp", static_cast<int>(amount));
	AnimatedText* pTestTextEffect = m_pTextEffectsManager->CreateTextEffect(CubbyGame::GetInstance()->GetFrontendManager()->GetTextEffectFont(), CubbyGame::GetInstance()->GetFrontendManager()->GetTextEffectOutlineFont(), CubbyGame::GetInstance()->GetDefaultViewport(), TextDrawMode::Screen3D, TextEffect::FadeUp, TextDrawStyle::Outline, screenposition, Color(0.0f, 1.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), healthText, 1.0f);
	pTestTextEffect->SetAutoDelete(true);
	pTestTextEffect->StartEffect();

	CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();
}

void Player::GiveCoins(int amount) const
{
	// Do an animated text effect
	glm::vec3 screenposition = GetCenter() + glm::vec3(GetRandomNumber(-1, 1, 2) * 0.25f, 0.0f, GetRandomNumber(-1, 1, 2) * 0.25f);
	char healthText[32];
	sprintf(healthText, "%i coin", static_cast<int>(amount));
	AnimatedText* pTestTextEffect = m_pTextEffectsManager->CreateTextEffect(CubbyGame::GetInstance()->GetFrontendManager()->GetTextEffectFont(), CubbyGame::GetInstance()->GetFrontendManager()->GetTextEffectOutlineFont(), CubbyGame::GetInstance()->GetDefaultViewport(), TextDrawMode::Screen3D, TextEffect::FadeUp, TextDrawStyle::Outline, screenposition, Color(1.0f, 1.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), healthText, 1.0f);
	pTestTextEffect->SetAutoDelete(true);
	pTestTextEffect->StartEffect();

	CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();
}

// Player stats
void Player::SetSupressStatsExport(bool supress) const
{
	m_pPlayerStats->SetSupressExport(supress);
}

PlayerStats* Player::GetPlayerStats() const
{
	return m_pPlayerStats;
}

std::string Player::GetStrengthModifierString() const
{
	char returnString[64];
	char signModifier[2];
	char colorModifier[16];
	
	if (m_strengthModifier == 0)
	{
		return "";
	}
	
	if (m_strengthModifier > 0)
	{
		sprintf(signModifier, "+");
		sprintf(colorModifier, "Green");
	}
	else
	{
		sprintf(signModifier, "-");
		sprintf(colorModifier, "Red");
	}

	sprintf(returnString, "[C=%s]%s %i", colorModifier, signModifier, abs(m_strengthModifier));

	return returnString;
}

std::string Player::GetDexterityModifierString() const
{
	char returnString[64];
	char signModifier[2];
	char colorModifier[16];
	
	if (m_dexterityModifier == 0)
	{
		return "";
	}
	
	if (m_dexterityModifier > 0)
	{
		sprintf(signModifier, "+");
		sprintf(colorModifier, "Green");
	}
	else
	{
		sprintf(signModifier, "-");
		sprintf(colorModifier, "Red");
	}

	sprintf(returnString, "[C=%s]%s %i", colorModifier, signModifier, abs(m_dexterityModifier));

	return returnString;
}

std::string Player::GetIntelligenceModifierString() const
{
	char returnString[64];
	char signModifier[2];
	char colorModifier[16];

	if (m_intelligenceModifier == 0)
	{
		return "";
	}

	if (m_intelligenceModifier > 0)
	{
		sprintf(signModifier, "+");
		sprintf(colorModifier, "Green");
	}
	else
	{
		sprintf(signModifier, "-");
		sprintf(colorModifier, "Red");
	}

	sprintf(returnString, "[C=%s]%s %i", colorModifier, signModifier, abs(m_intelligenceModifier));

	return returnString;
}

std::string Player::GetVitalityModifierString() const
{
	char returnString[64];
	char signModifier[2];
	char colorModifier[16];
	
	if (m_vitalityModifier == 0)
	{
		return "";
	}
	
	if (m_vitalityModifier > 0)
	{
		sprintf(signModifier, "+");
		sprintf(colorModifier, "Green");
	}
	else
	{
		sprintf(signModifier, "-");
		sprintf(colorModifier, "Red");
	}

	sprintf(returnString, "[C=%s]%s %i", colorModifier, signModifier, abs(m_vitalityModifier));

	return returnString;
}

std::string Player::GetArmorModifierString() const
{
	char returnString[64];
	char signModifier[2];
	char colorModifier[16];
	
	if (m_armorModifier == 0)
	{
		return "";
	}
	
	if (m_armorModifier > 0)
	{
		sprintf(signModifier, "+");
		sprintf(colorModifier, "Green");
	}
	else
	{
		sprintf(signModifier, "-");
		sprintf(colorModifier, "Red");
	}

	sprintf(returnString, "[C=%s]%s %i", colorModifier, signModifier, abs(m_armorModifier));

	return returnString;
}

std::string Player::GetLuckModifierString() const
{
	char returnString[64];
	char signModifier[2];
	char colorModifier[16];

	if (m_luckModifier == 0)
	{
		return "";
	}

	if (m_luckModifier > 0)
	{
		sprintf(signModifier, "+");
		sprintf(colorModifier, "Green");
	}
	else
	{
		sprintf(signModifier, "-");
		sprintf(colorModifier, "Red");
	}

	sprintf(returnString, "[C=%s]%s %i", colorModifier, signModifier, abs(m_luckModifier));

	return returnString;
}

// Crafting
void Player::SetCrafting(bool crafting)
{
	m_crafting = crafting;

	if (m_crafting)
	{
		m_createdAnvilHitParticleEffect = true;
	}
}

void Player::SetCraftingItem(bool crafting)
{
	if (crafting)
	{
		UnloadWeapon(false);
		LoadWeapon(false, GetEquipmentFileNameForType(EquipmentType::Hammer));
	}
	else
	{
		m_createdAnvilHitParticleEffect = true;

		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose", 0.1f);

		SetRandomLookMode();

		InventoryItem* pInventoryItem = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::RightHand);
		if (pInventoryItem != nullptr)
		{
			LoadWeapon(false, pInventoryItem->m_fileName);
		}
		else
		{
			UnloadWeapon(false);
		}
	}
}

bool Player::IsCrafting() const
{
	return m_crafting;
}

// Looking
void Player::LookAtPoint(glm::vec3 point)
{
	m_lookAtPoint = true;
	m_lookPoint = point;
}

void Player::SetRandomLookMode()
{
	m_pVoxelCharacter->SetFaceTargetDirection(m_pVoxelCharacter->GetFaceLookingDirection());
	m_pVoxelCharacter->SetRandomLookDirection(true);
	m_pVoxelCharacter->SetFaceLookToTargetSpeedMultiplier(1.0f);
	m_lookAtPoint = false;
}

void Player::SetBodyTurnStopThreshold(float threshold)
{
	m_bodyTurnStopThreshold = threshold;
}

void Player::SetBodyTurnSpeedMultiplier(float multiplier)
{
	m_bodyTurnSpeedMultiplier = multiplier;
}

// Player equipped attributes
void Player::SetNormal()
{
	m_equippedProperties = PlayerEquippedProperties::None;
}

void Player::SetSword(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Sword;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Sword;
	}
}

void Player::SetAxe(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Axe;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Axe;
	}
}

void Player::SetHammer(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Hammer;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Hammer;
	}
}

void Player::SetMace(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Mace;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Mace;
	}
}

void Player::SetDagger(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Dagger;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Dagger;
	}
}

void Player::SetSickle(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Sickle;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Sickle;
	}
}

void Player::SetTwoHandedSword(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::TwoHandedSword;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::TwoHandedSword;
	}
}

void Player::SetShield(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Shield;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Shield;
	}
}

void Player::SetBoomerang(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Boomerang;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Boomerang;
	}
}

void Player::SetBow(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Bow;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Bow;
	}
}

void Player::SetStaff(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Staff;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Staff;
	}
}

void Player::SetWand(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Wand;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Wand;
	}
}

void Player::SetPickaxe(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Pickaxe;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Pickaxe;
	}
}

void Player::SetTorch(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Torch;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Torch;
	}
}

void Player::SetItemPlacing(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::PlacementItem;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::PlacementItem;
	}
}

void Player::SetSceneryPlacing(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::PlacementScenery;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::PlacementScenery;
	}
}

void Player::SetBlockPlacing(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::PlacementBlock;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::PlacementBlock;
	}
}

void Player::SetConsumable(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Consumable;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Consumable;
	}
}

void Player::SetBomb(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::Bomb;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::Bomb;
	}
}

void Player::SetSpellHands(bool s)
{
	if (s)
	{
		m_equippedProperties |= PlayerEquippedProperties::SpellHands;
	}
	else
	{
		m_equippedProperties &= ~PlayerEquippedProperties::SpellHands;
	}
}

bool Player::IsNormal() const
{
	return (m_equippedProperties & PlayerEquippedProperties::None) == PlayerEquippedProperties::None;
}

bool Player::IsSword() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Sword) == PlayerEquippedProperties::Sword;
}

bool Player::IsAxe() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Axe) == PlayerEquippedProperties::Axe;
}

bool Player::IsHammer() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Hammer) == PlayerEquippedProperties::Hammer;
}

bool Player::IsMace() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Mace) == PlayerEquippedProperties::Mace;
}

bool Player::IsDagger() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Dagger) == PlayerEquippedProperties::Dagger;
}

bool Player::IsSickle() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Sickle) == PlayerEquippedProperties::Sickle;
}

bool Player::IsTwoHandedSword() const
{
	return (m_equippedProperties & PlayerEquippedProperties::TwoHandedSword) == PlayerEquippedProperties::TwoHandedSword;
}

bool Player::IsShield() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Shield) == PlayerEquippedProperties::Shield;
}

bool Player::IsBoomerang() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Boomerang) == PlayerEquippedProperties::Boomerang;
}

bool Player::IsBow() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Bow) == PlayerEquippedProperties::Bow;
}

bool Player::IsStaff() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Staff) == PlayerEquippedProperties::Staff;
}

bool Player::IsWand() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Wand) == PlayerEquippedProperties::Wand;
}

bool Player::IsPickaxe() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Pickaxe) == PlayerEquippedProperties::Pickaxe;
}

bool Player::IsTorch() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Torch) == PlayerEquippedProperties::Torch;
}

bool Player::IsItemPlacing() const
{
	return (m_equippedProperties & PlayerEquippedProperties::PlacementItem) == PlayerEquippedProperties::PlacementItem;
}

bool Player::IsSceneryPlacing() const
{
	return (m_equippedProperties & PlayerEquippedProperties::PlacementScenery) == PlayerEquippedProperties::PlacementScenery;
}

bool Player::IsBlockPlacing() const
{
	return (m_equippedProperties & PlayerEquippedProperties::PlacementBlock) == PlayerEquippedProperties::PlacementBlock;
}

bool Player::IsConsumable() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Consumable) == PlayerEquippedProperties::Consumable;
}

bool Player::IsBomb() const
{
	return (m_equippedProperties & PlayerEquippedProperties::Bomb) == PlayerEquippedProperties::Bomb;
}

bool Player::IsSpellHands() const
{
	return (m_equippedProperties & PlayerEquippedProperties::SpellHands) == PlayerEquippedProperties::SpellHands;
}

// Rendering modes
void Player::SetWireFrameRender(bool wireframe) const
{
	m_pVoxelCharacter->SetWireFrameRender(wireframe);
}

void Player::SetPlayerAlpha(float alpha) const
{
	m_pVoxelCharacter->SetMeshAlpha(alpha);
}

void Player::SetFirstPersonMode() const
{
	m_pVoxelCharacter->SetBreathingAnimationEnabled(false);
	m_pVoxelCharacter->SetRandomLookDirection(false);

	m_pVoxelCharacter->SetFaceLookingDirection(glm::vec3(0.0f, 0.0f, 1.0f));
	m_pVoxelCharacter->SetFaceTargetDirection(m_pVoxelCharacter->GetFaceLookingDirection());
}

void Player::SetThirdPersonMode() const
{
	m_pVoxelCharacter->SetBreathingAnimationEnabled(true);
	m_pVoxelCharacter->SetRandomLookDirection(true);
}

// Rendering Helpers
void Player::CalculateWorldTransformMatrix()
{
	m_right = normalize(cross(m_up, m_forward));
	m_forward = normalize(cross(m_right, m_up));

	float matrix[16] =
	{
		m_right.x, m_right.y, m_right.z, 0.0f,
		m_up.x, m_up.y, m_up.z, 0.0f,
		m_forward.x, m_forward.y, m_forward.z, 0.0f,
		m_position.x, m_position.y - (m_doStepUpAnimation ? 0.0f : m_stepUpAnimationYOffset), m_position.z, 1.0f
	};

	m_worldMatrix.SetValues(matrix);
}

void Player::RebuildVoxelCharacter(bool faceMerge) const
{
	m_pVoxelCharacter->RebuildVoxelModel(faceMerge);
}

// Updating
void Player::Update(float dt)
{
	// Update grid position
	UpdateGridPosition();

	// Update the voxel model
	float animationSpeeds[static_cast<int>(AnimationSections::NumSections)] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	m_pVoxelCharacter->Update(dt, animationSpeeds);
	m_pVoxelCharacter->SetWeaponTrailsOriginMatrix(m_worldMatrix);

	// Update animations
	UpdateAnimations();

	// Update charging attack
	UpdateChargingAttack(dt);

	// Update combat
	UpdateCombat();

	// Update block selection
	UpdateBlockSelection();

	// Update movement
	UpdateMovement(dt);

	// Update working
	UpdateWorking(dt);

	// Update look at point
	UpdateLookingAndForwardTarget(dt);

	// Update / Create weapon lights and particle effects
	UpdateWeaponLights();
	UpdateWeaponParticleEffects();

	// Update magic
	UpdateMagic(dt);

	// Update timers
	UpdateTimers(dt);

	// Physics update
	UpdatePhysics(dt);
}

void Player::UpdateAnimations()
{
	if (m_pVoxelCharacter != nullptr)
	{
		for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
		{
			m_animationFinished[i] = m_pVoxelCharacter->HasAnimationFinished(static_cast<AnimationSections>(i));
		}
	}

	if (m_canInteruptCombatAnim == false && m_animationFinished[static_cast<int>(AnimationSections::FullBody)] == true)
	{
		m_canInteruptCombatAnim = true;
	}
	if (m_canAttackLeft == false && m_animationFinished[static_cast<int>(AnimationSections::LeftArmHand)] == true)
	{
		m_canAttackLeft = true;
	}
	if (m_canAttackRight == false && m_animationFinished[static_cast<int>(AnimationSections::RightArmHand)] == true)
	{
		m_canAttackRight = true;
	}
}

void Player::UpdatePhysics(float dt)
{
	m_positionMovementAmount = glm::vec3(0.0f, 0.0f, 0.0f);

	if (m_groundCheckTimer <= 0.0f)
	{
		m_isOnGround = false;
	}

	// Step up animation
	float stepUpAddition = 0.0f;
	if (m_doStepUpAnimation)
	{
		stepUpAddition = m_stepUpAnimationYAmount - m_stepUpAnimationPrevious;
		m_position.y += stepUpAddition;

		m_stepUpAnimationPrevious = m_stepUpAnimationYAmount;
	}

	// Gravity multiplier
	float gravityMultiplier = 4.0f;
	if (m_pChunkManager->IsUnderWater(GetCenter()))
	{
		gravityMultiplier = 0.5f;
	}

	// Integrate velocity
	glm::vec3 acceleration = m_force + (m_gravityDirection * 9.81f) * gravityMultiplier;
	m_velocity += acceleration * dt;

	// Check collision
	glm::vec3 velocityToUse = m_velocity;
	glm::vec3 velAmount = velocityToUse * dt;
	glm::vec3 pNormal;
	int numberDivision = 1;

	while (length(velAmount) >= 1.0f)
	{
		numberDivision++;
		velAmount = velocityToUse * (dt / numberDivision);
	}

	for (int i = 0; i < numberDivision; ++i)
	{
		float dtToUse = (dt / numberDivision) + ((dt / numberDivision) * i);
		glm::vec3 posToCheck = GetCenter() + velocityToUse * dtToUse;
		bool stepUp = false;

		if (CheckCollisions(posToCheck, m_previousPosition, &pNormal, &velAmount, &stepUp))
		{
			// Reset velocity, we don't have any bounce
			m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			velocityToUse = glm::vec3(0.0f, 0.0f, 0.0f);

			if (velocityToUse.y <= 0.0f)
			{
				m_isOnGround = true;
				m_groundCheckTimer = 0.1f;

				if (m_canJump == false)
				{
					m_canJump = true;

					CubbyGame::GetInstance()->PlaySoundEffect(SoundEffect::JumpLand);
				}
			}
		}
	}

	// Integrate position
	m_position += velocityToUse * dt;

	m_positionMovementAmount += glm::vec3(0.0f, stepUpAddition + m_stepUpAdditionYAmountChangedCache, 0.0f);
	m_positionMovementAmount += velocityToUse * dt;

	m_stepUpAdditionYAmountChangedCache = 0.0f;

	// Store previous position
	m_previousPosition = GetCenter();
}

void Player::UpdateMovement(float dt)
{
	if (m_moveToTargetPosition)
	{
		glm::vec3 targetPos = m_targetPosition;
		glm::vec3 toTarget = targetPos - GetCenter();
		toTarget.y = 0.0f;
		float lengthToTarget = length(toTarget);

		bool reachedTarget = (lengthToTarget < 0.1f);

		if (reachedTarget)
		{
			StopMoving();

			SetBodyTurnStopThreshold(0.01f);
			SetBodyTurnSpeedMultiplier(6.0f);

			if (m_moveToTargetPosition)
			{
				LookAtPoint(m_lookAtPositionAfterMoveToTarget);

				m_moveToTargetPosition = false;
			}
		}
		else
		{
			LookAtPoint(targetPos);

			toTarget = m_targetPosition - m_position;
			glm::vec3 movementDirection = toTarget;
			movementDirection.y = 0.0f;
			movementDirection = normalize(movementDirection);

			float movementSpeed = (4.5f * dt);
			if (movementSpeed > 0.5f)
			{
				movementSpeed = 0.5f;
			}
			
			// TODO: Animation speeds
			// float animationSpeed = 1.0f;

			//for (int i = 0; i < AnimationSections::NumSections; ++i)
			//{
			//	SetAnimationSpeed(animationSpeed, false, static_cast<AnimationSections>(i));
			//}

			MoveAbsolute(movementDirection, movementSpeed);
		}
	}
}

void Player::UpdateWorking(float dt)
{
	if (m_pVoxelCharacter->HasAnimationFinished(AnimationSections::RightArmHand))
	{
		if (m_createdAnvilHitParticleEffect == false)
		{
			glm::vec3 anvilHitPos = m_lookPoint + glm::vec3(0.0f, 0.5f, 0.0f);

			unsigned int effectID = -1;
			BlockParticleEffect* pBlockParticleEffect = m_pBlockParticleManager->ImportParticleEffect("Resources/gamedata/particles/anvil_hit.effect", anvilHitPos, &effectID);
			pBlockParticleEffect->PlayEffect();

			// Stop weapon trails
			if (m_pVoxelCharacter->GetRightWeapon())
			{
				if (m_pVoxelCharacter->IsRightWeaponLoaded())
				{
					m_pVoxelCharacter->GetRightWeapon()->StopWeaponTrails();
				}
			}

			m_createdAnvilHitParticleEffect = true;
		}
	}

	if (m_crafting)
	{
		if (m_pVoxelCharacter->HasAnimationFinished(AnimationSections::RightArmHand))
		{
			if (m_workingAnimationWaitTimer <= 0.0f)
			{
				m_workingAnimationWaitTimer = m_workingAnimationDelay;

				if (m_pVoxelCharacter->HasAnimationFinished(AnimationSections::RightArmHand))
				{
					// TODO: Animation speeds
					//for (int i = 0; i < AnimationSections::NumSections; ++i)
					//{
					//	SetAnimationSpeed(1.0f, false, static_cast<AnimationSections>(i));
					//}

					m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "AnvilWork");

					// Start weapon trails
					if (m_pVoxelCharacter->GetRightWeapon())
					{
						if (m_pVoxelCharacter->IsRightWeaponLoaded())
						{
							m_pVoxelCharacter->GetRightWeapon()->StartWeaponTrails();
						}
					}

					m_createdAnvilHitParticleEffect = false;
				}
			}
			else
			{
				m_workingAnimationWaitTimer -= dt;
			}
		}
	}
}

void Player::UpdateLookingAndForwardTarget(float dt)
{
	if (m_pTargetEnemy != nullptr)
	{
		LookAtPoint(m_pTargetEnemy->GetCenter());

		glm::vec3 toTarget = m_pTargetEnemy->GetCenter() - GetCenter();
		toTarget.y = 0.0f;
		m_forward = normalize(toTarget);
		m_targetForward = m_forward;
	}

	glm::vec3 toPoint = m_lookPoint - GetCenter();
	toPoint = normalize(toPoint);

	if (m_lookAtPoint)
	{
		m_targetForward = toPoint;
		m_targetForward.y = 0.0f;
	}

	m_targetForward = normalize(m_targetForward);

	if (length(m_forward - m_targetForward) <= m_bodyTurnStopThreshold)
	{
	}
	else
	{
		glm::vec3 toTarget = m_targetForward - m_forward;
		m_forward += (toTarget * dt) * m_bodyTurnSpeedMultiplier;
		m_forward = normalize(m_forward);
	}

	if (m_pVoxelCharacter != nullptr)
	{
		if (m_lookAtPoint)
		{
			float angleMissing = dot(m_targetForward, m_forward);
			float angle = acos(angleMissing);

			if (RadianToDegree(angle) > 80.0f)
			{
				angle = DegreeToRadian(80.0f);
			}

			float tempX = static_cast<float>(sin(angle));
			float tempZ = static_cast<float>(cos(angle));

			glm::vec3 crossProduct = cross(m_targetForward, m_forward);

			if (crossProduct.y > 0.0f)
			{
				tempX = -tempX;
			}

			glm::vec3 lookDirection = glm::vec3(tempX, toPoint.y, tempZ);

			m_pVoxelCharacter->SetFaceTargetDirection(lookDirection);
			m_pVoxelCharacter->SetRandomLookDirection(false);
			m_pVoxelCharacter->SetFaceLookToTargetSpeedMultiplier(4.0f);
		}
		else
		{
			if (CubbyGame::GetInstance()->GetCameraMode() != CameraMode::FirstPerson)
			{
				m_pVoxelCharacter->SetRandomLookDirection(true);
			}

			m_pVoxelCharacter->SetFaceLookToTargetSpeedMultiplier(1.0f);
		}
	}
}

void Player::UpdateMagic(float dt)
{
	if (IsDead() == false)
	{
		// Passively add mana back
		m_magic += 5.0f * dt;

		if (m_magic > m_maxMagic)
		{
			m_magic = m_maxMagic;
		}

		CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();
	}
}

void Player::UpdateTimers(float dt)
{
	// Jump timer
	if (m_jumpTimer >= 0.0f)
	{
		m_jumpTimer -= dt;
	}

	// Ground check timer
	if (m_groundCheckTimer >= 0.0f)
	{
		m_groundCheckTimer -= dt;
	}

	// Floor particle timer
	if (m_floorParticleTimer >= 0.0f)
	{
		m_floorParticleTimer -= dt;
	}

	// Footstep sounds
	if (m_footstepSoundTimer > 0.0f)
	{
		m_footstepSoundTimer -= dt;
	}

	// Bow attack delay
	if (m_bowAttackDelay >= 0.0f)
	{
		m_bowAttackDelay -= dt;
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
	if (m_hitFacialExpressionTimer > 0.0f)
	{
		m_hitFacialExpressionTimer -= dt;
	}
	else
	{
		if (m_returnToNormalFacialExpressionAfterHit == false)
		{
			// Revert back to normal facial expression
			m_pVoxelCharacter->PlayFacialExpression("Normal");
			m_returnToNormalFacialExpressionAfterHit = true;
		}
	}

	// Ghost timer
	if (m_createGhost)
	{
		if (m_createGhostTimer > 0.0f)
		{
			m_createGhostTimer -= dt;
		}
		else
		{
			glm::vec3 ghostSpawn = GetCenter();
			glm::vec3 floorPosition;

			if (m_pChunkManager->FindClosestFloor(ghostSpawn, &floorPosition))
			{
				ghostSpawn = floorPosition + glm::vec3(0.0f, 0.01f, 0.0f);
			}
			Enemy* pGhost = m_pEnemyManager->CreateEnemy(ghostSpawn, EnemyType::Doppelganger, 0.08f);
			pGhost->SetSpawningParams(ghostSpawn - glm::vec3(0.0f, 0.0f, 0.0f), ghostSpawn + glm::vec3(-m_cameraForward.x * 1.5f, 1.5f, -m_cameraForward.z * 1.5f), 1.5f);
			m_createGhost = false;
		}
	}
}

void Player::UpdateWeaponLights() const
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

					pWeapon->GetLightParams(i, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

					if (lightID == -1)
					{
						m_pLightingManager->AddLight(glm::vec3(0.0f), 0.0f, 1.0f, Color(1.0f, 1.0f, 1.0f, 1.0f), &lightID);
						pWeapon->SetLightingID(i, lightID);
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

void Player::UpdateWeaponParticleEffects() const
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

void Player::UpdateChargingAttack(float dt)
{
	// Charging - figure out trajectory and velocity for projectile
	if (m_isChargingAttack)
	{
		m_chargeAmount += dt / m_chargeTime;

		if (m_chargeAmount > 1.0f)
		{
			m_chargeAmount = 1.0f;
		}

		m_chargeSpawnPosition = GetCenter() + (m_forward * 0.75f) + (GetUpVector() * 0.5f);

		float liftAmount = 3.5f * m_chargeAmount;
		float powerAmount = 90.0f * m_chargeAmount;
		float cameraMultiplier = 75.0f;

		if (CubbyGame::GetInstance()->GetCameraMode() == CameraMode::FirstPerson)
		{
			cameraMultiplier = 107.0f;
		}

		if (m_pTargetEnemy)
		{
			// Enemy target
			glm::vec3 toTarget = m_pTargetEnemy->GetCenter() - GetCenter();
			float toTargetDistance = length(toTarget);
			liftAmount += toTargetDistance * 0.04f;
			m_chargeSpawnVelocity = (normalize(toTarget) * powerAmount) + glm::vec3(0.0f, liftAmount, 0.0f);
		}
		else
		{
			m_chargeSpawnVelocity = (m_forward * powerAmount) + (GetUpVector() * liftAmount) + (glm::vec3(0.0f, 1.0f, 0.0f) * (m_cameraForward.y * cameraMultiplier) * m_chargeAmount);
		}

		if (CubbyGame::GetInstance()->GetCameraMode() == CameraMode::FirstPerson)
		{
			m_pVoxelCharacter->SetHeadAndUpperBodyLookRotation(0.0f, 0.0f);
		}
		else
		{
			m_pVoxelCharacter->SetHeadAndUpperBodyLookRotation(-m_cameraForward.y * 20.0f, 0.5f);
		}
	}
	else
	{
		m_pVoxelCharacter->SetHeadAndUpperBodyLookRotation(0.0f, 0.0f);
	}
}

void Player::UpdateCombat()
{
	// Check projectile hits
	for (int i = 0; i < m_pProjectileManager->GetNumProjectiles(); ++i)
	{
		Projectile* pProjectile = m_pProjectileManager->GetProjectile(i);

		if (pProjectile != nullptr && pProjectile->GetErase() == false)
		{
			CheckProjectileDamageRadius(pProjectile);
		}
	}
}

void Player::UpdateBlockSelection()
{
	if (IsPickaxe())
	{
		Item* pInteractItem = CubbyGame::GetInstance()->GetInteractItem();
		// Only show the mining selection block if we are not interacting with an item
		if (pInteractItem == nullptr)
		{
			int blockX, blockY, blockZ;
			glm::vec3 blockPos;
			m_blockSelection = GetSelectionBlock(&blockPos, &blockX, &blockY, &blockZ);

			if (m_blockSelection == true)
			{
				m_blockSelectionPos = blockPos;
			}
		}
		else
		{
			m_blockSelection = false;
		}
	}
}

// Rendering
void Player::Render() const
{
	if (IsDead())
	{
		return;
	}

	Color oulineColor(1.0f, 1.0f, 0.0f, 1.0f);
	
	m_pRenderer->PushMatrix();

	m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);

	m_pRenderer->PushMatrix();

	if (m_isChargingAttack)
	{
		m_pRenderer->RotateWorldMatrix(-m_cameraForward.y * 20.0f, 0.0f, 0.0f);
	}

	m_pVoxelCharacter->RenderWeapons(false, false, false, oulineColor);

	m_pRenderer->PopMatrix();
	
	m_pVoxelCharacter->Render(false, false, false, oulineColor, false);
	
	m_pRenderer->PopMatrix();
}

void Player::RenderFirstPerson() const
{
	if (IsDead())
	{
		return;
	}

	Color oulineColor(1.0f, 1.0f, 0.0f, 1.0f);

	m_pRenderer->PushMatrix();

	m_pVoxelCharacter->RenderWeapons(false, false, false, oulineColor);
	m_pVoxelCharacter->RenderSubSelection("Left_Hand", false, false, oulineColor);
	m_pVoxelCharacter->RenderSubSelection("Right_Hand", false, false, oulineColor);

	m_pRenderer->PopMatrix();
}

void Player::RenderWeaponTrails() const
{
	m_pRenderer->PushMatrix();

	m_pVoxelCharacter->RenderWeaponTrails();

	m_pRenderer->PopMatrix();
}

void Player::RenderFace() const
{
	if (IsDead())
	{
		return;
	}

	m_pRenderer->PushMatrix();

	m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);
	m_pRenderer->EmptyTextureIndex(0);
	m_pVoxelCharacter->RenderFace();

	m_pRenderer->PopMatrix();
}

void Player::RenderPaperdoll() const
{
	m_pRenderer->PushMatrix();

	m_pVoxelCharacter->RenderPaperdoll();
	m_pVoxelCharacter->RenderWeaponsPaperdoll();

	m_pRenderer->PopMatrix();
}

void Player::RenderPaperdollFace() const
{
	m_pRenderer->PushMatrix();

	m_pVoxelCharacter->RenderFacePaperdoll();

	m_pRenderer->PopMatrix();
}

void Player::RenderPortrait() const
{
	m_pRenderer->PushMatrix();

	m_pVoxelCharacter->RenderPortrait();

	m_pRenderer->PopMatrix();
}

void Player::RenderPortraitFace() const
{
	m_pRenderer->PushMatrix();

	m_pVoxelCharacter->RenderFacePortrait();

	m_pRenderer->PopMatrix();
}

void Player::RenderSelectionBlock() const
{
	if (m_dead == true)
	{
		return;
	}

	if (IsPickaxe() == false && IsBlockPlacing() == false && IsItemPlacing() == false)
	{
		return;
	}

	if (m_blockSelection)
	{
		float length = Chunk::BLOCK_RENDER_SIZE * 1.1f;
		float height = Chunk::BLOCK_RENDER_SIZE * 1.1f;
		float width = Chunk::BLOCK_RENDER_SIZE * 1.1f;

		m_pRenderer->PushMatrix();

		m_pRenderer->TranslateWorldMatrix(m_blockSelectionPos.x, m_blockSelectionPos.y, m_blockSelectionPos.z);
		
		for (int i = 0; i < 2; ++i)
		{
			if (i == 0)
			{
				m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
				m_pRenderer->SetCullMode(CullMode::NOCULL);
				m_pRenderer->SetLineWidth(1.0f);
			}
			else
			{
				m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
				m_pRenderer->SetRenderMode(RenderMode::SOLID);
			}

			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

			m_pRenderer->ImmediateColorAlpha(1.0f, 0.9f, 0.25f, 0.25f);
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

			if (i == 1)
			{
				m_pRenderer->DisableTransparency();
			}
		}

		m_pRenderer->SetCullMode(CullMode::BACK);
		
		m_pRenderer->PopMatrix();
	}
}

void Player::RenderDebug()
{
	if (CubbyGame::GetInstance()->GetCameraMode() != CameraMode::FirstPerson)
	{
		m_pRenderer->PushMatrix();
		
		m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);

		// Radius
		m_pRenderer->PushMatrix();
		
		m_pRenderer->SetLineWidth(1.0f);
		m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);

		m_pRenderer->RotateWorldMatrix(90.0f, 0.0f, 0.0f);
		m_pRenderer->DrawSphere(m_radius, 20, 20);
		
		m_pRenderer->PopMatrix();

		// Forwards
		m_pRenderer->PushMatrix();
		
		m_pRenderer->ScaleWorldMatrix(m_pVoxelCharacter->GetCharacterScale(), m_pVoxelCharacter->GetCharacterScale(), m_pVoxelCharacter->GetCharacterScale());

		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->SetLineWidth(3.0f);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);
		
		// Target forwards
		m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 1.0f, 1.0f);
		m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
		m_pRenderer->ImmediateVertex(m_targetForward.x * 15.0f, m_targetForward.y * 15.0f, m_targetForward.z * 15.0f);

		// Right
		m_pRenderer->ImmediateColorAlpha(1.0f, 0.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
		m_pRenderer->ImmediateVertex(m_right.x * 15.0f, m_right.y * 15.0f, m_right.z * 15.0f);

		// Up
		m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
		m_pRenderer->ImmediateVertex(m_up.x * 15.0f, m_up.y * 15.0f, m_up.z * 15.0f);

		// Forwards
		m_pRenderer->ImmediateColorAlpha(0.0f, 0.0f, 1.0f, 1.0f);
		m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
		m_pRenderer->ImmediateVertex(m_forward.x * 15.0f, m_forward.y * 15.0f, m_forward.z * 15.0f);
		
		m_pRenderer->DisableImmediateMode();
		
		m_pRenderer->PopMatrix();

		// NPC interaction sector
		m_pRenderer->PushMatrix();
		
		m_pRenderer->SetLineWidth(1.0f);
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
		float angle = acos(NPCManager::NPC_INTERACTION_RADIUS_CHECK);
		m_pRenderer->RotateWorldMatrix(0.0f, GetRotation() - 90.0f, 0.0f);
		m_pRenderer->DrawCircleSector(NPCManager::NPC_INTERACTION_DISTANCE, angle, 10);
		
		m_pRenderer->PopMatrix();

		// Item interaction sector
		m_pRenderer->PushMatrix();

		m_pRenderer->SetLineWidth(1.0f);
		m_pRenderer->ImmediateColorAlpha(0.0f, 0.5f, 1.0f, 1.0f);
		angle = acos(ItemManager::ITEM_INTERACTION_RADIUS_CHECK);
		m_pRenderer->RotateWorldMatrix(0.0f, GetRotation() - 90.0f, 0.0f);
		m_pRenderer->DrawCircleSector(ItemManager::ITEM_INTERACTION_DISTANCE, angle, 10);
		
		m_pRenderer->PopMatrix();

		// Attack segment
		if (m_attackEnabled && (m_attackEnabledDelayTimer <= 0.0f))
		{
			m_pRenderer->PushMatrix();
			
			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->ImmediateColorAlpha(1.0f, 0.15f, 0.05f, 1.0f);
			angle = acos(m_attackSegmentAngle);
			m_pRenderer->RotateWorldMatrix(0.0f, GetRotation() - 90.0f + m_attackRotation, 0.0f);
			m_pRenderer->TranslateWorldMatrix(0.0f, 0.25f, 0.0f);
			m_pRenderer->DrawCircleSector(m_attackRadius, angle, 10);
			
			m_pRenderer->PopMatrix();
		}

		m_pRenderer->PopMatrix();

		// Projectile hitbox
		RenderProjectileHitboxDebug();
	}
}

void Player::RenderProjectileHitboxDebug()
{
	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetProjectileHitboxCenter().x, GetProjectileHitboxCenter().y, GetProjectileHitboxCenter().z);

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 1.0f, 1.0f);

	if (m_projectileHitboxType == ProjectileHitboxType::Sphere)
	{
		m_pRenderer->DrawSphere(m_radius, 20, 20);
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

void Player::_StepUpAnimationFinished(void *pData)
{
	Player* pPlayer = static_cast<Player*>(pData);
	pPlayer->StepUpAnimationFinished();
}

void Player::StepUpAnimationFinished()
{
	// Final addition
	m_stepUpAdditionYAmountChangedCache = m_stepUpAnimationYAmount - m_stepUpAnimationPrevious;
	m_position.y += m_stepUpAdditionYAmountChangedCache;

	m_doStepUpAnimation = false;
}
