/*************************************************************************
> File Name: NPC.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    An NPC is like a player, but not human controlled. Can be set to have
>    different behaviors and different sub-types of NPCs, interacts with
>    the player and also attacks enemies.
> Created Time: 2016/08/06
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>

#include <CubbyGame.h>

#include <Items/ItemManager.h>
#include <Lighting/LightingManager.h>
#include <Maths/3DMaths.h>
#include <Models/VoxelObject.h>
#include <Models/VoxelWeapon.h>
#include <TextEffects/AnimatedText.h>
#include <Utils/Interpolator.h>
#include <Utils/Random.h>

#include "NPC.h"

// Constructor, Destructor
NPC::NPC(Renderer* pRenderer, ChunkManager* pChunkManager, Player* pPlayer, LightingManager* pLightingManager, BlockParticleManager* pBlockParticleManager, TextEffectsManager* pTextEffectsManager, ItemManager* pItemManager, ProjectileManager* pProjectileManager, EnemyManager* pEnemyManager, QubicleBinaryManager* pQubicleBinaryManager, std::string name, std::string typeName, std::string modelName, bool characterSelectScreen, bool useQubicleManager) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager), m_pLightingManager(pLightingManager), m_pPlayer(pPlayer),
	m_pBlockParticleManager(pBlockParticleManager), m_pTextEffectsManager(pTextEffectsManager), m_pItemManager(pItemManager),
	m_pQubicleBinaryManager(pQubicleBinaryManager), m_pProjectileManager(pProjectileManager), m_pEnemyManager(pEnemyManager)
{
	m_erase = false;

	m_name = name;

	m_pVoxelCharacter = new VoxelCharacter(m_pRenderer, m_pQubicleBinaryManager);
	m_pCharacterBackup = new QubicleBinary(m_pRenderer);

	m_radius = 1.0f;

	m_gravityDirection = glm::vec3(0.0f, -1.0f, 0.0f);

	m_forward = glm::vec3(0.0f, 0.0f, 1.0f);
	m_targetForward = glm::vec3(0.0f, 0.0f, 1.0f);

	m_gridPositionX = 0;
	m_gridPositionY = 0;
	m_gridPositionZ = 0;

	m_pCachedGridChunk = nullptr;

	m_outlineRender = false;
	m_hoverRender = false;
	m_subSelectionRender = false;

	// Animation params
	for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
	{
		m_animationSpeed[i] = 1.0f;
		m_animationSpeedonlySetOnCompleteAnimation[i] = false;
		m_animationFinished[i] = false;
	}
	m_animationTimer = 0.0f;

	// Loading character files
	m_type = typeName;
	m_modelName = modelName;

	// NPC State
	m_NPCState = NPCState::Idle;
	m_previousNPCState = NPCState::Idle;

	// Combat type
	m_NPCCombatType = NPCCombatType::None;

	// Frontend NPC
	m_frontEndNPC = false;

	// Player class for frontend lineup
	m_selectedClass = PlayerClass::Debug;

	// Movement gameplay params
	m_moveToPlayer = false;
	m_hasReachedTargetPosition = false;
	m_lookAtPositionWhenReachTarget = false;

	// Projectile hitbox
	m_projectileHitboxType = ProjectileHitboxType::Cube;
	m_projectileHitboxXLength = 0.65f;
	m_projectileHitboxYLength = 1.25f;
	m_projectileHitboxZLength = 0.65f;
	m_projectileHitboxCenterOffset = glm::vec3(0.0f, 0.75f, 0.0f);

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
	m_aggroRadius = 12.0f;
	m_updateAggroRadiusTimer = 0.0f;
	m_attackCounter = 0;

	// Health
	m_maxHealth = 100.0f;
	m_health = m_maxHealth;
	m_maxMagic = 100.0f;
	m_magic = m_maxMagic;

	m_damageTime = 0.4f;
	m_damageTimer = m_damageTime;
	m_knockbackTime = 0.4f;
	m_knockbackTimer = m_knockbackTime;

	// Charging attacks
	m_isChargingAttack = false;
	m_chargeAmount = 0.0f;
	m_chargeTime = 1.0f;

	// Idle
	m_isIdle = true;
	m_waitBeforeStopMovingAnimationTimer = 0.0f;
	m_hasSetIdleAnimation = true;

	m_movementSpeed = 3.75f;
	m_maxMovementSpeed = 7.0f;
	m_minMovementSpeed = 1.0f;

	m_currentWaypointIndex = 0;

	// Jumping
	m_canJump = true;
	m_jumpTime = 1.0f;
	m_jumpTimer = 0.0f;
	m_jumpHeight = 14.5f;

	// Look at point
	m_bLookAtPoint = false;
	m_bodyTurnSpeedMultiplier = 3.5f;
	m_bodyTurnStopThreshold = 0.35f;
	m_bIsLookingAtPoint = false;

	// Talking
	m_talkMode = false;
	m_talkDelay = 0.0f;
	m_talkTime = 0.0f;
	m_talkTimer = 0.0f;

	// Working
	m_isWorking = false;
	m_workingAnimationWaitTimer = 0.0f;
	m_workingAnimationDelay = 0.45f;
	m_workingRepetitionBeforeLongDelay = 3;
	m_workingRepetitionCounter = 0;
	m_createdAnvilHitParticleEffect = false;

	// Push collision enabled
	m_pushingCollisionEnabled = true;

	// Is this NPC a credits NPC, if so, behave differently
	m_isCreditsNPC = false;

	m_pTargetEnemy = nullptr;

	char characterBaseFolder[128];
	char qbFilename[128];
	char ms3dFilename[128];
	char animListFilename[128];
	char facesFilename[128];
	char characterFilename[128];

	if (characterSelectScreen)
	{
		sprintf(characterBaseFolder, "Resources/gamedata/models");
		sprintf(qbFilename, "saves/characters/%s/%s.qb", modelName.c_str(), modelName.c_str());
		sprintf(ms3dFilename, "Resources/gamedata/models/%s/%s.ms3d", m_type.c_str(), m_type.c_str());
		sprintf(animListFilename, "Resources/gamedata/models/%s/%s.animlist", m_type.c_str(), m_type.c_str());
		sprintf(facesFilename, "saves/characters/%s/%s.faces", modelName.c_str(), modelName.c_str());
		sprintf(characterFilename, "saves/characters/%s/%s.character", modelName.c_str(), modelName.c_str());
	}
	else
	{
		sprintf(characterBaseFolder, "Resources/gamedata/models");
		sprintf(qbFilename, "Resources/gamedata/models/%s/%s.qb", m_type.c_str(), modelName.c_str());
		sprintf(ms3dFilename, "Resources/gamedata/models/%s/%s.ms3d", m_type.c_str(), m_type.c_str());
		sprintf(animListFilename, "Resources/gamedata/models/%s/%s.animlist", m_type.c_str(), m_type.c_str());
		sprintf(facesFilename, "Resources/gamedata/models/%s/%s.faces", m_type.c_str(), modelName.c_str());
		sprintf(characterFilename, "Resources/gamedata/models/%s/%s.character", m_type.c_str(), modelName.c_str());
	}

	m_pVoxelCharacter->UnloadCharacter();
	m_pVoxelCharacter->Reset();
	m_pVoxelCharacter->LoadVoxelCharacter(m_type.c_str(), qbFilename, ms3dFilename, animListFilename, facesFilename, characterFilename, characterBaseFolder, useQubicleManager);

	m_pVoxelCharacter->SetBreathingAnimationEnabled(true);
	m_pVoxelCharacter->SetWinkAnimationEnabled(true);
	m_pVoxelCharacter->SetTalkingAnimationEnabled(false);
	m_pVoxelCharacter->SetRandomMouthSelection(true);
	m_pVoxelCharacter->SetRandomLookDirection(true);
	m_pVoxelCharacter->SetWireFrameRender(false);

	m_pVoxelCharacter->PlayAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose");

	UnloadWeapon(true);
	UnloadWeapon(false);

	if (m_pCharacterBackup != nullptr)
	{
		delete m_pCharacterBackup;
		m_pCharacterBackup = new QubicleBinary(m_pRenderer);
	}

	m_pCharacterBackup->Import(qbFilename, true);

	UpdateRadius();
}

NPC::~NPC()
{
	ClearWaypoints();

	UnloadWeapon(true);
	UnloadWeapon(false);

	m_pVoxelCharacter->RemoveQubicleMatrix("Quiver");

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

void NPC::SetLightingManager(LightingManager* pLightingManager)
{
	m_pLightingManager = pLightingManager;
}

void NPC::SetErase(bool erase)
{
	m_erase = erase;
}

bool NPC::NeedErase() const
{
	return m_erase;
}

VoxelCharacter* NPC::GetVoxelCharacter() const
{
	return m_pVoxelCharacter;
}

void NPC::SetName(std::string name)
{
	m_name = name;
}

std::string NPC::GetName() const
{
	return m_name;
}

void NPC::SetType(std::string typeName)
{
	m_type = typeName;
}

std::string NPC::GetType() const
{
	return m_type;
}

void NPC::SetModelName(std::string modelName)
{
	m_modelName = modelName;
}

std::string NPC::GetModelName() const
{
	return m_modelName;
}

void NPC::SetPosition(glm::vec3 pos)
{
	m_position = pos;
}

glm::vec3 NPC::GetPosition() const
{
	return m_position;
}

void NPC::SetRotation(float rot)
{
	float angle = DegreeToRadian(rot);
	m_forward = glm::vec3(sin(angle), 0.0f, cos(angle));
}

float NPC::GetRotation() const
{
	float rotationAngle = acos(dot(glm::vec3(0.0f, 0.0f, 1.0f), m_forward));
	rotationAngle = RadianToDegree(rotationAngle);
	if (m_forward.x < 0.0f)
	{
		rotationAngle = (360.0f - rotationAngle);
	}

	return rotationAngle;
}

void NPC::SetScale(float scale)
{
	m_pVoxelCharacter->SetCharacterScale(scale);

	UpdateRadius();
}

float NPC::GetRadius() const
{
	return m_radius;
}

void NPC::UpdateRadius()
{
	m_radius = m_pVoxelCharacter->GetCharacterScale() / 0.14f;
}

glm::vec3 NPC::GetCenter() const
{
	glm::vec3 center = m_position + glm::vec3(0.0f, m_radius, 0.0f);

	return center;
}

glm::vec3 NPC::GetForwardVector() const
{
	return m_worldMatrix.GetForwardVector();
}

glm::vec3 NPC::GetRightVector() const
{
	return m_worldMatrix.GetRightVector();
}

glm::vec3 NPC::GetUpVector() const
{
	return m_worldMatrix.GetUpVector();
}

// Front-end NPCs
void NPC::SetFrontEndNPC(bool frontEnd)
{
	m_frontEndNPC = frontEnd;
}

bool NPC::IsFrontEndNPC() const
{
	return m_frontEndNPC;
}

void NPC::SetPlayerClass(PlayerClass selectedClass)
{
	m_selectedClass = selectedClass;
}

PlayerClass NPC::GetPlayerClass() const
{
	return m_selectedClass;
}

// Combat type
void NPC::SetNPCCombatType(NPCCombatType combatType, bool setWeaponModel)
{
	m_NPCCombatType = combatType;

	if (setWeaponModel)
	{
		UnloadWeapon(false);
		UnloadWeapon(true);

		SetBodyTurnStopThreshold(0.01f);
		SetBodyTurnSpeedMultiplier(6.0f);

		switch (m_NPCCombatType)
		{
		case NPCCombatType::None: { break; }
		case NPCCombatType::MeleeSword:
		{
			LoadWeapon(false, GetEquipmentFileNameForType(EquipmentType::IronSword));
			break;
		}
		case NPCCombatType::Archer:
		{
			LoadWeapon(true, GetEquipmentFileNameForType(EquipmentType::WoodenBow));

			// Add a quiver item to the enemy ranger
			VoxelWeapon* pNewEquipment = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
			pNewEquipment->SetVoxelCharacterParent(nullptr);
			pNewEquipment->LoadWeapon("Resources/gamedata/items/Quiver/Quiver.item", false);

			pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Quiver", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
			QubicleMatrix* pQuiverMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Quiver");
			pQuiverMatrix->m_boneIndex = m_pVoxelCharacter->GetBodyBoneIndex();
			m_pVoxelCharacter->AddQubicleMatrix(pQuiverMatrix, false);

			break;
		}
		case NPCCombatType::Staff:
		{
			LoadWeapon(false, GetEquipmentFileNameForType(EquipmentType::MageStaff));
			break;
		}
		case NPCCombatType::FireballHands:
		{
			LoadWeapon(false, GetEquipmentFileNameForType(EquipmentType::FireballHandRight));
			LoadWeapon(true, GetEquipmentFileNameForType(EquipmentType::FireballHandLeft));
			break;
		}
		}
	}
}

void NPC::LoadWeapon(bool left, std::string weaponFile)
{
	if (left)
	{
		if (m_pVoxelCharacter->GetLeftWeapon() != nullptr)
		{
			if (m_pVoxelCharacter->IsLeftWeaponLoaded())
			{
				// Lights
				for (int i = 0; i < m_pVoxelCharacter->GetLeftWeapon()->GetNumLights(); i++)
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
				for (int i = 0; i < m_pVoxelCharacter->GetLeftWeapon()->GetNumParticleEffects(); i++)
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
				for (int i = 0; i < m_pVoxelCharacter->GetRightWeapon()->GetNumLights(); i++)
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

void NPC::UnloadWeapon(bool left) const
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

					if (m_frontEndNPC == false)
					{
						float scale = m_pVoxelCharacter->GetCharacterScale();
						unsigned int id;
						m_pLightingManager->AddDyingLight(glm::vec3(lightPos.x, lightPos.y, lightPos.z), lightRadius * scale, lightDiffuseMultiplier, lightColor, 2.0f, &id);
					}
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
void NPC::ImportEquippedItems(std::string inventoryFile)
{
	std::ifstream importFile;
	importFile.open(inventoryFile.c_str(), std::ios_base::binary);

	if (importFile.is_open())
	{
		char* input = new char[128];
		char* pEnd;

		// Import the stored inventory items
		importFile.getline(input, 128, '|');
		int numInventory = strtol(input, &pEnd, 10);

		for (int i = 0; i < numInventory; ++i)
		{
			importFile.getline(input, 128, '|');
			int itemValid = strtol(input, &pEnd, 10);

			if (itemValid == 1)
			{
				char fileName[128];
				importFile.getline(fileName, 128, '|');

				char iconFileName[128];
				importFile.getline(iconFileName, 128, '|');

				// Item type
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				// Item
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				// Status
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				// Equip slot
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				// Item quality
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				// Left
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				// Right
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				char title[128];
				importFile.getline(title, 128, '|');

				char description[128];
				importFile.getline(description, 128, '|');

				// Red
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);
				
				// Green
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);
				
				// Blue
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);

				// Scale
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);

				// Offset X
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);
				
				// Offset Y
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);
				
				// Offset Z
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);

				// Quantity
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				// Also import the stats attributes
				int numStatAttributes;
				importFile.getline(input, 128, '|');
				numStatAttributes = strtol(input, &pEnd, 10);
				for (int k = 0; k < numStatAttributes; ++k)
				{
					// Type
					importFile.getline(input, 128, '|');
					strtol(input, &pEnd, 10);

					// Amount
					importFile.getline(input, 128, '|');
					strtol(input, &pEnd, 10);
				}
			}
		}

		// Import the equipped items
		for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
		{
			importFile.getline(input, 128, '|');
			int itemValid = strtol(input, &pEnd, 10);

			if (itemValid == 1)
			{
				char fileName[128];
				importFile.getline(fileName, 128, '|');

				char iconFileName[128];
				importFile.getline(iconFileName, 128, '|');

				// Item type
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				// Item
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				// Status
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int equipSlot = strtol(input, &pEnd, 10);

				// Item quality
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int left = strtol(input, &pEnd, 10);

				importFile.getline(input, 128, '|');
				int right = strtol(input, &pEnd, 10);

				char title[128];
				importFile.getline(title, 128, '|');

				char description[128];
				importFile.getline(description, 128, '|');

				// Red
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);

				// Green
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);
				
				// Blue
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);

				// Scale
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);

				// Offset X
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);
				
				// Offset Y
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);
				
				// Offset Z
				importFile.getline(input, 128, '|');
				strtod(input, &pEnd);

				// Quantity
				importFile.getline(input, 128, '|');
				strtol(input, &pEnd, 10);

				// Also import the stats attributes
				int numStatAttributes;
				importFile.getline(input, 128, '|');
				numStatAttributes = strtol(input, &pEnd, 10);
				for (int k = 0; k < numStatAttributes; ++k)
				{
					// Type
					importFile.getline(input, 128, '|');
					strtol(input, &pEnd, 10);

					// Amount
					importFile.getline(input, 128, '|');
					strtol(input, &pEnd, 10);
				}

				EquipItem(static_cast<EquipSlot>(equipSlot), fileName, static_cast<bool>(left), static_cast<bool>(right));
			}
		}

		// Import the number of coins
		importFile.getline(input, 128, '|');

		delete[] input;
	}
}

void NPC::EquipItem(EquipSlot equipSlot, const char* itemFileName, bool left, bool right)
{
	switch (equipSlot)
	{
	case EquipSlot::LeftHand:
	{
		LoadWeapon(true, itemFileName);
	}
	break;
	case EquipSlot::RightHand:
	{
		LoadWeapon(false, itemFileName);
	}
	break;
	case EquipSlot::Head:
	{
		if (m_pVoxelCharacter != nullptr)
		{
			VoxelWeapon* pNewEquipment = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
			pNewEquipment->SetVoxelCharacterParent(nullptr);
			pNewEquipment->LoadWeapon(itemFileName, false);

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
			pNewEquipment->LoadWeapon(itemFileName, false);

			if (right)
			{
				pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Right_Shoulder_Armor", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
				QubicleMatrix* pRightShoulderMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Right_Shoulder_Armor");
				pRightShoulderMatrix->m_boneIndex = m_pVoxelCharacter->GetRightShoulderBoneIndex();
				m_pVoxelCharacter->AddQubicleMatrix(pRightShoulderMatrix, false);
			}

			if (left)
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
			pNewEquipment->LoadWeapon(itemFileName, false);

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
			pNewEquipment->LoadWeapon(itemFileName, false);

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
			pNewEquipment->LoadWeapon(itemFileName, false);

			if (right)
			{
				pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Right_Hand", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
				QubicleMatrix* pRightHandMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Right_Hand");
				pRightHandMatrix->m_boneIndex = m_pVoxelCharacter->GetRightHandBoneIndex();
				m_pVoxelCharacter->AddQubicleMatrix(pRightHandMatrix, false);
			}

			if (left)
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
			pNewEquipment->LoadWeapon(itemFileName, false);

			if (right)
			{
				pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->SetScaleAndOffsetForMatrix("Right_Foot", pNewEquipment->GetAnimatedSection(0)->renderScale, pNewEquipment->GetAnimatedSection(0)->renderOffset.x, pNewEquipment->GetAnimatedSection(0)->renderOffset.y, pNewEquipment->GetAnimatedSection(0)->renderOffset.z);
				QubicleMatrix* pRightFootMatrix = pNewEquipment->GetAnimatedSection(0)->pVoxelObject->GetQubicleModel()->GetQubicleMatrix("Right_Foot");
				pRightFootMatrix->m_boneIndex = m_pVoxelCharacter->GetRightFootBoneIndex();
				m_pVoxelCharacter->AddQubicleMatrix(pRightFootMatrix, false);
			}

			if (left)
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
	{
	}
	break;
	case EquipSlot::Accessory2:
	{
	}
	break;
	case EquipSlot::NoSlot:
	case EquipSlot::NumSlots:
	default:
		throw std::logic_error("Invalid equip slot at NPC::EquipItem()");
	}
}

void NPC::UnequipItem(EquipSlot equipSlot, bool left, bool right) const
{
	switch (equipSlot)
	{
	case EquipSlot::LeftHand:
	{
		UnloadWeapon(true);

		m_pVoxelCharacter->RemoveQubicleMatrix("Quiver");
	}
	break;
	case EquipSlot::RightHand:
	{
		UnloadWeapon(false);
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
	{
	}
	break;
	case EquipSlot::Accessory2:
	{
	}
	break;
	case EquipSlot::NoSlot:
	case EquipSlot::NumSlots:
	default:
		throw std::logic_error("Invalid equip slot at NPC::UnequipItem()");
	}
}

// Animation
void NPC::SetAnimationSpeed(float speed, bool onlySetOnCompleteAnimation, AnimationSections section)
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

float NPC::GetAnimationSpeed(AnimationSections section)
{
	return m_animationSpeed[static_cast<int>(section)];
}

bool NPC::AnimationFinished(AnimationSections section)
{
	return m_animationFinished[static_cast<int>(section)];
}

// Skeleton bone scale
void NPC::SetSkeletonBoneScale(float scale) const
{
	if (m_pVoxelCharacter)
	{
		m_pVoxelCharacter->SetBoneScale(scale);
	}
}

float NPC::GetSkeletonBoneScale() const
{
	if (m_pVoxelCharacter)
	{
		return m_pVoxelCharacter->GetBoneScale().x; // x,y,z all the same
	}

	return 1.0f;
}

// NPC State
NPCState NPC::GetState() const
{
	return m_NPCState;
}

// Movement gameplay
void NPC::SetMoveToPlayer(bool moveToPlayer)
{
	m_moveToPlayer = moveToPlayer;
}

void NPC::SetLookAtPositionWhenReachedTarget(bool enabled, glm::vec3 lookAtPosition)
{
	m_lookAtPositionWhenReachTarget = enabled;
	m_lookAtPositionWhenReachedTargetPosition = lookAtPosition;
}

// Dead
// ReSharper disable once CppMemberFunctionMayBeStatic
bool NPC::IsDead() const
{
	// TODO: IsDead()
	return false;
}

// Projectile hitbox
ProjectileHitboxType NPC::GetProjectileHitboxType() const
{
	return m_projectileHitboxType;
}

float NPC::GetProjectileHitboxRadius() const
{
	return m_projectileHitboxRadius;
}

float NPC::GetProjectileHitboxXLength() const
{
	return m_projectileHitboxXLength;
}

float NPC::GetProjectileHitboxYLength() const
{
	return m_projectileHitboxYLength;
}

float NPC::GetProjectileHitboxZLength() const
{
	return m_projectileHitboxZLength;
}

glm::vec3 NPC::GetProjectileHitboxCenter() const
{
	return GetCenter() + m_projectileHitboxCenterOffset;
}

// Movement
void NPC::MoveAbsolute(glm::vec3 direction, const float speed, bool shouldChangeForward)
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

	glm::vec3 pNormal;
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
		glm::vec3 posToCheck = GetCenter() + movement * speedToUse;

		if (CheckCollisions(posToCheck, m_previousPosition, &pNormal, &movement))
		{
		}

		m_position += (movement * speedToUse) * 0.95f;
	}

	if (CanAttack())
	{
		if (m_NPCCombatType == NPCCombatType::Staff)
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::LegsFeet, "StaffRun", 0.01f);
		}
		else
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::LegsFeet, "Run", 0.01f);
		}
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

	m_isIdle = false;
}

void NPC::StopMoving()
{
	if (m_isCreditsNPC)
	{
		return;
	}

	if (m_canJump)
	{
		if (m_isIdle == false)
		{
			m_isIdle = true;

			SetRandomLookMode();
		}

		if (m_hasSetIdleAnimation == false)
		{
			if (m_waitBeforeStopMovingAnimationTimer <= 0.0f)
			{
				if (CanAttack())
				{
					if (m_NPCCombatType == NPCCombatType::Staff)
					{
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "StaffPose", 0.15f);
					}
					else
					{
						m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "BindPose", 0.15f);
					}
				}
				if (m_canInteruptCombatAnim)
				{
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "BindPose", 0.15f);
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "BindPose", 0.15f);
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "BindPose", 0.15f);
				}

				m_hasSetIdleAnimation = true;
			}
		}
	}
}

void NPC::Jump()
{
	if (m_canJump == false)
	{
		return;
	}

	if (m_jumpTimer >= 0.0f)
	{
		return;
	}

	m_canJump = false;
	m_jumpTimer = m_jumpTime;

	m_velocity += glm::vec3(0.0f, m_jumpHeight, 0.0f);

	if (CanAttack())
	{
		SetAnimationSpeed(1.0f, false, AnimationSections::RightArmHand);
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "Jump", 0.01f);
	}

	if (m_canInteruptCombatAnim)
	{
		SetAnimationSpeed(1.0f, false, AnimationSections::HeadBody);
		SetAnimationSpeed(1.0f, false, AnimationSections::LegsFeet);
		SetAnimationSpeed(1.0f, false, AnimationSections::LeftArmHand);
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "Jump", 0.01f);
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LegsFeet, false, AnimationSections::LegsFeet, "Jump", 0.01f);
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "Jump", 0.01f);
	}
}

void NPC::SetTargetPosition(glm::vec3 pos)
{
	m_targetPosition = pos;
}

WayPoint* NPC::AddWaypoint(glm::vec3 pos, float xLength, float yLength, float zLength)
{
	WayPoint* pNewWayPoint = new WayPoint();
	pNewWayPoint->m_position = pos;
	pNewWayPoint->m_xLength = xLength;
	pNewWayPoint->m_yLength = yLength;
	pNewWayPoint->m_zLength = zLength;
	pNewWayPoint->m_pNextWaypoint = nullptr;

	// Connect to previous waypoint
	if (m_vpWayPointList.size() > 0)
	{
		m_vpWayPointList[m_vpWayPointList.size() - 1]->m_pNextWaypoint = pNewWayPoint;
	}

	m_vpWayPointList.push_back(pNewWayPoint);

	return pNewWayPoint;
}

void NPC::ClearWaypoints()
{
	for (size_t i = 0; i < m_vpWayPointList.size(); ++i)
	{
		delete m_vpWayPointList[i];
		m_vpWayPointList[i] = nullptr;
	}

	m_vpWayPointList.clear();
}

void NPC::StartWaypointNavigation(int waypointIndex)
{
	if (m_vpWayPointList.size() > 0 && waypointIndex < m_vpWayPointList.size())
	{
		m_targetPosition = m_vpWayPointList[waypointIndex]->m_position;
		m_currentWaypointIndex = waypointIndex;
		m_NPCState = NPCState::MovingToWayPoint;
	}
}

bool NPC::IsInsideWaypoint(int waypointIndex)
{
	if (waypointIndex < m_vpWayPointList.size())
	{
		WayPoint* pWaypoint = m_vpWayPointList[waypointIndex];

		float lowX = pWaypoint->m_position.x - pWaypoint->m_xLength;
		float lowY = pWaypoint->m_position.y - pWaypoint->m_yLength;
		float lowZ = pWaypoint->m_position.z - pWaypoint->m_zLength;
		float highX = pWaypoint->m_position.x + pWaypoint->m_xLength;
		float highY = pWaypoint->m_position.y + pWaypoint->m_yLength;
		float highZ = pWaypoint->m_position.z + pWaypoint->m_zLength;

		glm::vec3 pos = GetCenter();
		if (pos.x < lowX || pos.x > highX || pos.y < lowY || pos.y > highY || pos.z < lowZ || pos.z > highZ)
		{
			return false;
		}

		return true;
	}

	return false;
}

void NPC::SetMoveBackToPosition(glm::vec3 pos)
{
	m_moveBackToPosition = pos;
}

void NPC::SetForwards(glm::vec3 dir)
{
	m_forward = normalize(dir);
}

void NPC::SetTargetForwards(glm::vec3 dir)
{
	m_targetForward = normalize(dir);
}

bool NPC::HasReachedTargetPosition() const
{
	return m_hasReachedTargetPosition;
}

// Combat
void NPC::DoDamage(float amount, Color textColor, glm::vec3 knockbackDirection, float knockbackAmount, bool createParticleHit)
{
	if (m_damageTimer <= 0.0f)
	{
		m_health -= amount;

		// Figure out if we are dead yet
		if (m_health <= 0.0f)
		{
			m_health = 0.0f;

			Explode();
		}

		// Update HUD player data
		CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();

		// Play a hit response animation
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "HitResponse", 0.01f);

		// Do an animated text effect
		glm::vec3 screenposition = GetCenter() + glm::vec3(GetRandomNumber(-1, 1, 2) * 0.25f, 0.0f, GetRandomNumber(-1, 1, 2) * 0.25f);
		char damageText[32];
		sprintf(damageText, "%i", static_cast<int>(amount));
		AnimatedText* pTestTextEffect = m_pTextEffectsManager->CreateTextEffect(CubbyGame::GetInstance()->GetFrontendManager()->GetTextEffectFont(), CubbyGame::GetInstance()->GetFrontendManager()->GetTextEffectOutlineFont(), CubbyGame::GetInstance()->GetDefaultViewport(), TextDrawMode::Screen3D, TextEffect::FadeUp, TextDrawStyle::Outline, screenposition, textColor, Color(0.0f, 0.0f, 0.0f), damageText, 1.0f);
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
	}

	if (m_knockbackTimer <= 0.0f)
	{
		m_velocity += knockbackDirection * knockbackAmount;

		m_knockbackTimer = m_knockbackTime;
	}
}

void NPC::CheckEnemyDamageRadius()
{
	int numEnemies = m_pEnemyManager->GetNumEnemies();

	for (int i = 0; i < numEnemies; i++)
	{
		Enemy *pEnemy = m_pEnemyManager->GetEnemy(i);

		if (pEnemy->GetErase() == true)
		{
			continue;
		}

		if (pEnemy->GetAttackEnabled() == false)
		{
			continue;
		}

		glm::vec3 distance = GetCenter() - pEnemy->GetCenter();
		float lengthToEnemy = length(distance);
		if (lengthToEnemy <= m_radius + pEnemy->GetAttackRadius())
		{
			glm::vec3 distanceMinusY = distance;
			distanceMinusY.y = 0.0f;
			glm::vec3 direction = normalize(distanceMinusY);
			direction = normalize(direction);

			// Figure out the attack vector, based on the attack rotation
			float enemyRotation = pEnemy->GetRotation();
			float attackRotation = pEnemy->GetAttackRotation();
			float angle = DegreeToRadian(enemyRotation + attackRotation);
			glm::vec3 attackDirection = glm::vec3(sin(angle), 0.0f, cos(angle));
			float dotProduct = dot(direction, attackDirection);

			if (dotProduct > pEnemy->GetAttackSegmentAngle()) // Check if we are within the attack segment
			{
				glm::vec3 knockbackDirection = (direction * 2.0f) + glm::vec3(0.0f, 1.0f, 0.0f);

				knockbackDirection = normalize(knockbackDirection);
				Color damageColor = Color(1.0f, 1.0f, 1.0f);

				// Set NPC target as attacker
				SetTargetEnemy(pEnemy);

				float knockbackAmount = 16.0f;
				DoDamage(20.0f, damageColor, knockbackDirection, knockbackAmount, true);
			}
		}
	}
}

void NPC::CheckProjectileDamageRadius(Projectile* pProjectile)
{
	if (pProjectile->CanAttackNPCs() == false)
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
		glm::vec3 knockbackDirection = (normalize(pProjectile->GetVelocity())*2.0f) + glm::vec3(0.0f, 1.0f, 0.0f);
		knockbackDirection = normalize(knockbackDirection);
		Color damageColor = Color(1.0f, 1.0f, 1.0f);

		if (pProjectile->GetEnemyOwner() != nullptr)
		{
			// Set NPC target as attacker
			SetTargetEnemy(pProjectile->GetEnemyOwner());
		}

		float knockbackAmount = 16.0f;
		DoDamage(15.0f, damageColor, knockbackDirection, knockbackAmount, false);

		pProjectile->Explode();
	}
}

void NPC::Explode()
{
	if (IsDead())
	{
		return;
	}

	// TODO: NPC Dead
	//m_dead = true;

	m_isChargingAttack = false;
	m_chargeAmount = 0.0f;

	CalculateWorldTransformMatrix();

	// Explode the qubicle binary voxel file
	for (int explodeCounter = 0; explodeCounter < 3; ++explodeCounter)
	{
		QubicleBinary* pQubicleModel = nullptr;
		int spawnChance = 100;
		if (explodeCounter == 0)
		{
			pQubicleModel = m_pVoxelCharacter->GetQubicleModel();
			spawnChance = 50;
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
						spawnChance = 100;
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
						spawnChance = 100;
					}
				}
			}
		}

		if (pQubicleModel != nullptr)
		{
			m_pBlockParticleManager->ExplodeQubicleBinary(pQubicleModel, m_pVoxelCharacter->GetCharacterScale(), spawnChance);
		}
	}

	// Unload weapons
	UnloadWeapon(true);
	UnloadWeapon(false);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void NPC::Respawn() const
{
	// TODO : NPC Respawn()
}

// Attacking
void NPC::Attack()
{
	bool doAttack = false;

	if (m_attackDelayTimer <= 0.0f)
	{
		if (m_NPCCombatType == NPCCombatType::MeleeSword)
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

			m_movementWaitAfterAttackTimer = 0.75f;
			m_attackDelayTime = 0.15f;
			m_attackEnabled = true;
			m_attackEnabledTimer = 0.0f;
			m_attackRotation = startRotation;
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledTimer, 0.0f, attackTime, attackTime, 0.0f, nullptr, _AttackEnabledTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackRotation, startRotation, endRotation, attackTime, easingRotation);

			// Start weapon trails
			if (m_pVoxelCharacter->GetRightWeapon())
			{
				if (m_pVoxelCharacter->IsRightWeaponLoaded())
				{
					m_pVoxelCharacter->GetRightWeapon()->StartWeaponTrails();
				}
			}

			doAttack = true;
		}
		else if (m_NPCCombatType == NPCCombatType::Archer)
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
			}

			doAttack = true;
		}
		else if (m_NPCCombatType == NPCCombatType::Staff)
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "StaffAttack", 0.01f);

			m_attackDelayTime = 1.35f + GetRandomNumber(-100, 50, 2) * 0.005f;

			m_attackEnabledDelayTimer = 0.15f;
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);

			doAttack = true;
		}
		else if (m_NPCCombatType == NPCCombatType::FireballHands)
		{
			m_attackCounter++;

			if (m_attackCounter == 2)
			{
				m_attackCounter = 0;
			}

			if (m_attackCounter == 1)
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "HandSpellCastRight", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "HandSpellCastRight", 0.01f);
			}
			else
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "HandSpellCastLeft", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "HandSpellCastLeft", 0.01f);
			}

			m_animationFinished[static_cast<int>(AnimationSections::RightArmHand)] = true;
			SetAnimationSpeed(1.5f, true, AnimationSections::FullBody);

			m_attackDelayTime = 0.75f + GetRandomNumber(-50, 50, 2) * 0.005f;
			Interpolator::GetInstance()->AddFloatInterpolation(&m_animationTimer, 0.0f, 0.3f, 0.3f, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);

			m_canAttack = false;

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

void NPC::ReleaseAttack()
{
	if (m_isChargingAttack == true)
	{
		UnloadWeapon(false);

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(m_chargeSpawnPosition, m_chargeSpawnVelocity, 0.0f, "Resources/gamedata/items/Arrow/arrow.item", 0.08f);
		pProjectile->SetProjectileType(true, false, false);
		pProjectile->SetOwner(nullptr, this, nullptr);

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

bool NPC::CanAttack() const
{
	return m_canAttack;
}

bool NPC::GetAttackEnabled() const
{
	return m_attackEnabled && (m_attackEnabledDelayTimer <= 0.0f);
}

float NPC::GetAttackRadius() const
{
	return m_attackRadius;
}

void NPC::SetAttackRadius(float attackRadius)
{
	m_attackRadius = attackRadius;
}

float NPC::GetAttackRotation() const
{
	return m_attackRotation;
}

float NPC::GetAttackSegmentAngle() const
{
	return m_attackSegmentAngle;
}

void NPC::SetEnemyDied(Enemy* pEnemy)
{
	if (m_pTargetEnemy == pEnemy)
	{
		SetTargetEnemy(nullptr);

		SetRandomLookMode();
	}
}

void NPC::SetTargetEnemy(Enemy* pEnemy)
{
	m_pTargetEnemy = pEnemy;

	if (m_pTargetEnemy != nullptr)
	{
		if (m_NPCCombatType == NPCCombatType::Archer || m_NPCCombatType == NPCCombatType::Staff || m_NPCCombatType == NPCCombatType::FireballHands)
		{
			StopMoving();
		}

		if (m_NPCState != NPCState::Combat)
		{
			m_previousNPCState = m_NPCState;
			m_NPCState = NPCState::Combat;
		}
	}
	else
	{
		m_NPCState = m_previousNPCState;

		if (m_NPCCombatType == NPCCombatType::Archer)
		{
			m_isChargingAttack = false;
			m_chargeAmount = 0.0f;

			UnloadWeapon(false);
		}

		if (m_NPCState == NPCState::MovingToWayPoint)
		{
			StartWaypointNavigation(m_currentWaypointIndex);
		}
		else if (m_moveToPlayer == false)
		{
			SetTargetPosition(m_moveBackToPosition);
		}
	}
}

// Looking
void NPC::SetTargetForwardToLookAtPoint(glm::vec3 point)
{
	glm::vec3 toPoint = point - GetCenter();
	toPoint.y = 0.0f;
	toPoint = normalize(toPoint);

	m_targetForward = toPoint;
}

void NPC::LookAtPoint(glm::vec3 point)
{
	m_bLookAtPoint = true;
	m_lookPoint = point;
}

void NPC::SetRandomLookMode()
{
	m_pVoxelCharacter->SetFaceTargetDirection(m_pVoxelCharacter->GetFaceLookingDirection());
	m_pVoxelCharacter->SetRandomLookDirection(true);
	m_pVoxelCharacter->SetFaceLookToTargetSpeedMultiplier(1.0f);
	m_bLookAtPoint = false;
}

void NPC::SetBodyTurnStopThreshold(float threshold)
{
	m_bodyTurnStopThreshold = threshold;
}

void NPC::SetBodyTurnSpeedMultiplier(float multiplier)
{
	m_bodyTurnSpeedMultiplier = multiplier;
}

bool NPC::IsLookAtPointMode() const
{
	return m_bLookAtPoint;
}

bool NPC::IsLookingAtPoint() const
{
	return m_bIsLookingAtPoint;
}

// Talking
void NPC::SetNPCTalkMode(float talkDelay, float talkTime)
{
	m_talkMode = true;
	m_talkDelay = talkDelay;
	m_talkTime = talkTime;
	m_talkTimer = 0.0f;

	if (m_NPCState != NPCState::Talking)
	{
		m_previousNPCState = m_NPCState;
		m_NPCState = NPCState::Talking;
	}

	m_createdAnvilHitParticleEffect = true;

	m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose", 0.15f);
}

void NPC::StopNPCTalkMode()
{
	m_talkMode = false;
	m_pVoxelCharacter->SetTalkingAnimationEnabled(false);

	m_NPCState = m_previousNPCState;
}

// Working
void NPC::SetWorking(glm::vec3 workPointOfInterest)
{
	m_isWorking = true;

	m_lookAtWorkPoint = workPointOfInterest;

	if (m_NPCState != NPCState::Working)
	{
		m_previousNPCState = m_NPCState;
		m_NPCState = NPCState::Working;
	}
}

void NPC::StopWorking()
{
	m_isWorking = false;

	m_NPCState = m_previousNPCState;
}

// World
void NPC::UpdateGridPosition()
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

Chunk* NPC::GetCachedGridChunkOrFromPosition(glm::vec3 pos) const
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

void NPC::ClearChunkCacheForChunk(Chunk* pChunk)
{
	if (m_pCachedGridChunk == pChunk)
	{
		m_pCachedGridChunk = nullptr;
	}
}

// Collision
bool NPC::CheckCollisions(glm::vec3 positionCheck, glm::vec3 previousPosition, glm::vec3 *pNormal, glm::vec3 *pMovement) const
{
	float radius = GetRadius();

	glm::vec3 movementCache = *pMovement;

	// World collisions
	bool worldCollision = false;

	int blockX, blockY, blockZ;
	glm::vec3 blockPos;

	glm::vec3 floorPosition;
	if (m_pChunkManager->FindClosestFloor(positionCheck, &floorPosition) == false)
	{
		*pMovement = glm::vec3(0.0f, 0.0f, 0.0f);
		return true;
	}

	int numChecks = 1 + static_cast<int>(radius / (Chunk::BLOCK_RENDER_SIZE* 2.0f));
		
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

bool NPC::IsBlockInFront() const
{
	glm::vec3 blockPosCheck;
	int blockXCheck, blockYCheck, blockZCheck;

	// Straight in front
	glm::vec3 checkPos = GetCenter() + normalize(m_forward) * (m_radius * 2.0f);
	Chunk* pChunk = GetCachedGridChunkOrFromPosition(checkPos);
	bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(checkPos.x, checkPos.y, checkPos.z, &blockPosCheck, &blockXCheck, &blockYCheck, &blockZCheck, &pChunk);
	if (active == true)
	{
		return true;
	}

	// To the left side
	checkPos = GetCenter() + normalize(m_forward - GetRightVector()) * (m_radius * 2.0f);
	pChunk = GetCachedGridChunkOrFromPosition(checkPos);
	active = m_pChunkManager->GetBlockActiveFrom3DPosition(checkPos.x, checkPos.y, checkPos.z, &blockPosCheck, &blockXCheck, &blockYCheck, &blockZCheck, &pChunk);
	if (active == true)
	{
		return true;
	}

	// To the right side
	checkPos = GetCenter() + normalize(m_forward + GetRightVector()) * (m_radius * 2.0f);
	pChunk = GetCachedGridChunkOrFromPosition(checkPos);
	active = m_pChunkManager->GetBlockActiveFrom3DPosition(checkPos.x, checkPos.y, checkPos.z, &blockPosCheck, &blockXCheck, &blockYCheck, &blockZCheck, &pChunk);
	if (active == true)
	{
		return true;
	}

	return false;
}

bool NPC::IsPushingCollisionEnabled() const
{
	return  m_pushingCollisionEnabled;
}

void NPC::SetPushingCollisionEnabled(bool pushEnabled)
{
	m_pushingCollisionEnabled = pushEnabled;
}

void NPC::SetCreditsNPC(bool creditsNPC)
{
	m_isCreditsNPC = creditsNPC;
}

bool NPC::IsCreditsNPC() const
{
	return m_isCreditsNPC;
}

void NPC::CalculateWorldTransformMatrix()
{
	m_forward = normalize(m_forward);

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

void NPC::SetOutlineRender(bool outline)
{
	m_outlineRender = outline;
}

bool NPC::GetOutlineRender() const
{
	return m_outlineRender;
}

void NPC::SetHoverRender(bool hover)
{
	m_hoverRender = hover;
}

bool NPC::GetHoverRender() const
{
	return m_hoverRender;
}

void NPC::SetWireFrameRender(bool wireframe) const
{
	if (m_pVoxelCharacter != nullptr)
	{
		m_pVoxelCharacter->SetWireFrameRender(wireframe);
	}
}

void NPC::SetSubSelectionRender(bool subSelection)
{
	m_subSelectionRender = subSelection;
}

bool NPC::GetSubSelectionRender() const
{
	return m_subSelectionRender;
}

glm::vec2 NPC::GetScreenPosition() const
{
	return m_screenPosition;
}

void NPC::UpdateWeaponLights() const
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

void NPC::UpdateWeaponParticleEffects() const
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

void NPC::UpdateLookingAndForwardTarget(float dt)
{
	glm::vec3 toPoint = m_lookPoint - GetCenter();
	toPoint = normalize(toPoint);

	if (m_bLookAtPoint)
	{
		m_targetForward = toPoint;
		m_targetForward.y = 0.0f;
	}

	m_targetForward = normalize(m_targetForward);

	if (length(m_forward - m_targetForward) <= m_bodyTurnStopThreshold)
	{
		m_bIsLookingAtPoint = true;
	}
	else
	{
		m_bIsLookingAtPoint = false;

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
		if (m_bLookAtPoint)
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
			m_pVoxelCharacter->SetRandomLookDirection(true);
			m_pVoxelCharacter->SetFaceLookToTargetSpeedMultiplier(1.0f);
		}
	}
}

void NPC::UpdateTalkMode(float dt)
{
	if (m_talkMode)
	{
		m_talkTimer += dt;

		if (m_talkTimer >= m_talkDelay)
		{
			m_pVoxelCharacter->SetTalkingAnimationEnabled(true);
		}

		if (m_talkTimer >= m_talkDelay + m_talkTime)
		{
			m_talkMode = false;
			m_pVoxelCharacter->SetTalkingAnimationEnabled(false);
		}
	}
}

void NPC::UpdateWorking(float dt)
{
	if (m_isWorking)
	{
		LookAtPoint(m_lookAtWorkPoint);

		if (m_pVoxelCharacter->HasAnimationFinished(AnimationSections::FullBody))
		{
			if (m_createdAnvilHitParticleEffect == false)
			{
				glm::vec3 anvilHitPos = m_lookPoint + glm::vec3(0.0f, 0.5f, 0.0f);

				unsigned int effectID = -1;
				BlockParticleEffect* pBlockParticleEffect = CubbyGame::GetInstance()->GetBlockParticleManager()->ImportParticleEffect("Resources/gamedata/particles/anvil_hit.effect", anvilHitPos, &effectID);
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

			if (m_workingAnimationWaitTimer <= 0.0f)
			{
				m_workingRepetitionCounter++;
				m_workingAnimationWaitTimer = m_workingAnimationDelay;

				if (m_workingRepetitionCounter == m_workingRepetitionBeforeLongDelay)
				{
					m_workingRepetitionCounter = 0;
					m_workingAnimationWaitTimer += (m_workingAnimationDelay * 2.0f); // 3 times as long delay (2 + 1)
				}

				if (m_pVoxelCharacter->HasAnimationFinished(AnimationSections::FullBody))
				{
					for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
					{
						SetAnimationSpeed(1.0f, false, static_cast<AnimationSections>(i));
					}

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

void NPC::UpdateCombat(float dt)
{
	if (m_NPCCombatType == NPCCombatType::MeleeSword)
	{
		UpdateMeleeCombat();
	}
	else if (m_NPCCombatType == NPCCombatType::Archer)
	{
		UpdateRangedCombat(dt);
	}
	else if (m_NPCCombatType == NPCCombatType::Staff)
	{
		UpdateRangedCombat(dt);
	}
	else if (m_NPCCombatType == NPCCombatType::FireballHands)
	{
		UpdateRangedCombat(dt);
	}

	// Reset the canAttack flag if our weapon arm animation is completed
	if (m_canAttack == false && (m_animationFinished[static_cast<int>(AnimationSections::RightArmHand)] == true))
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
}

void NPC::UpdateMeleeCombat()
{
	glm::vec3 toTarget = m_targetPosition - GetCenter();
	toTarget.y = 0.0f;
	float lengthToTarget = length(toTarget);

	m_lookPoint = m_targetPosition;
	m_bLookAtPoint = true;

	if (lengthToTarget <= m_attackRadius + m_pTargetEnemy->GetRadius())
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

void NPC::UpdateRangedCombat(float dt)
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

	// Charging - figure out trajectory and velocity for projectile
	if (m_isChargingAttack)
	{
		m_chargeAmount += dt / m_chargeTime;

		if (m_chargeAmount > 1.0f)
		{
			m_chargeAmount = 1.0f;
		}
	}

	if (m_NPCCombatType == NPCCombatType::Archer && m_pTargetEnemy != nullptr)
	{
		m_chargeSpawnPosition = GetCenter() + (m_forward * 0.75f) + (GetUpVector() * 0.5f);

		float liftAmount = 1.75f * m_chargeAmount;
		float powerAmount = 45.0f * m_chargeAmount;

		// Player target
		glm::vec3 toTarget = m_pTargetEnemy->GetCenter() - GetCenter();
		float toTargetDistance = length(toTarget);
		liftAmount += toTargetDistance * 0.20f;
		m_chargeSpawnVelocity = (normalize(toTarget) * powerAmount) + glm::vec3(0.0f, liftAmount, 0.0f);
	}
}

void NPC::UpdateMovement(float dt)
{
	if (m_NPCState == NPCState::Talking || m_NPCState == NPCState::Working)
	{
		return;
	}

	bool reachedTarget;

	if (m_NPCState == NPCState::MovingToWayPoint)
	{
		reachedTarget = IsInsideWaypoint(m_currentWaypointIndex);
		m_hasReachedTargetPosition = reachedTarget;
	}
	else
	{
		float radius;

		if (m_pTargetEnemy != nullptr)
		{
			SetTargetPosition(m_pTargetEnemy->GetCenter());
			radius = m_pTargetEnemy->GetRadius();
		}
		else
		{
			if (m_moveToPlayer)
			{
				SetTargetPosition(m_pPlayer->GetCenter());
				radius = m_pPlayer->GetRadius();
			}
			else
			{
				SetTargetPosition(m_moveBackToPosition);
				radius = GetRadius();
			}
		}

		glm::vec3 toTarget = m_targetPosition - GetCenter();
		toTarget.y = 0;
		float lengthToTarget = length(toTarget);

		reachedTarget = (lengthToTarget < m_attackRadius * 0.99f + radius);
		m_hasReachedTargetPosition = reachedTarget;
	}

	if (m_isIdle && reachedTarget == false)
	{
		m_hasSetIdleAnimation = false;
		m_waitBeforeStopMovingAnimationTimer = 0.25f;
	}

	if (reachedTarget && m_canJump == true)
	{
		if (m_NPCState == NPCState::MovingToWayPoint)
		{
			m_currentWaypointIndex++;
			if (m_currentWaypointIndex >= m_vpWayPointList.size())
			{
				m_currentWaypointIndex = 0;
				StartWaypointNavigation(m_currentWaypointIndex);
			}
			else
			{
				StartWaypointNavigation(m_currentWaypointIndex);
			}
		}
		else
		{
			StopMoving();
		}

		if (m_lookAtPositionWhenReachTarget)
		{
			LookAtPoint(m_lookAtPositionWhenReachedTargetPosition);
			m_lookAtPositionWhenReachTarget = false;
		}
	}
	else
	{
		LookAtPoint(m_targetPosition);

		bool shouldStopMovingUntilJump = false;
		if (IsBlockInFront())
		{
			if (m_jumpTimer > 0.0f)
			{
				shouldStopMovingUntilJump = true;
				StopMoving();
			}

			Jump();
		}

		if (shouldStopMovingUntilJump == false)
		{
			if (m_movementWaitAfterAttackTimer <= 0.0f)
			{
				if ((m_NPCCombatType != NPCCombatType::Archer && m_NPCCombatType != NPCCombatType::Staff && m_NPCCombatType != NPCCombatType::FireballHands) || m_pTargetEnemy == nullptr)
				{
					glm::vec3 toTarget = m_targetPosition - m_position;
					glm::vec3 movementDirection = toTarget;
					movementDirection.y = 0.0f;
					movementDirection = normalize(movementDirection);

					if (m_NPCState == NPCState::MovingToWayPoint)
					{
						m_movementSpeed = 3.75f;
					}
					else
					{
						glm::vec3 movementDistance = toTarget;
						movementDistance.y = 0.0f;
						m_movementSpeed = ((length(movementDistance) + 1.5f) / 8.0f) * m_maxMovementSpeed;

						if (m_movementSpeed > m_maxMovementSpeed)
							m_movementSpeed = m_maxMovementSpeed;
						if (m_movementSpeed < m_minMovementSpeed)
							m_movementSpeed = m_minMovementSpeed;
					}

					float movementSpeed = m_movementSpeed * dt;
					
					if (movementSpeed > 0.5f)
					{
						movementSpeed = 0.5f;
					}
					
					float animationSpeed = (m_movementSpeed / m_maxMovementSpeed);

					if (m_canJump && m_canInteruptCombatAnim == false)
					{
						for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
						{
							SetAnimationSpeed(animationSpeed, true, static_cast<AnimationSections>(i));
						}
					}

					MoveAbsolute(movementDirection, movementSpeed);
				}
			}
		}
	}
}

void NPC::UpdateNPCState(float dt)
{
	switch (m_NPCState)
	{
	case NPCState::Idle:
	{
	}
	break;
	case NPCState::MovingToWayPoint:
	{
	}
	break;
	case NPCState::Talking:
	{
		UpdateTalkMode(dt);
	}
	break;
	case NPCState::Working:
	{
		UpdateWorking(dt);
	}
	break;
	case NPCState::Combat:
	{
		UpdateCombat(dt);
	}
	break;
	}
}

void NPC::UpdatePhysics(float dt)
{
	glm::vec3 acceleration;
	if (IsFrontEndNPC() == false && IsCreditsNPC() == false) // Don't do gravity in front-end or credits
	{
		acceleration = (m_gravityDirection * 9.81f) * 5.0f;
	}

	// Integrate velocity
	m_velocity += acceleration * dt;

	// Check collision
	{
		glm::vec3 velocityToUse = m_velocity;
		glm::vec3 velAmount = velocityToUse*dt;
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
			glm::vec3 posToCheck = GetCenter() + velocityToUse*dtToUse;

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

	m_previousPosition = GetCenter();
}

void NPC::Update(float dt)
{
	// Update grid position
	UpdateGridPosition();

	// Update normal movement
	UpdateMovement(dt);

	// Update look at point
	UpdateLookingAndForwardTarget(dt);

	// Update NPC state
	UpdateNPCState(dt);

	// Update aggro distance check for enemies to attack
	if (m_updateAggroRadiusTimer <= 0.0f)
	{
		UpdateAggroRadius();
		m_updateAggroRadiusTimer = 1.0f + GetRandomNumber(0, 100, 2) * 0.01f;
	}

	// Check for enemy attack damage
	CheckEnemyDamageRadius();

	// Update timers
	UpdateTimers(dt);

	if (m_pVoxelCharacter != nullptr)
	{
		m_pVoxelCharacter->Update(dt, m_animationSpeed);
		m_pVoxelCharacter->SetWeaponTrailsOriginMatrix(m_worldMatrix);

		for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
		{
			m_animationFinished[i] = m_pVoxelCharacter->HasAnimationFinished(static_cast<AnimationSections>(i));
		}
	}

	// Update physics
	UpdatePhysics(dt);
}

void NPC::UpdateScreenCoordinates2d(Camera* pCamera)
{
	// Get projection co-ordinates
	m_pRenderer->PushMatrix();
	m_pRenderer->SetProjectionMode(ProjectionMode::PERSPECTIVE, CubbyGame::GetInstance()->GetDefaultViewport());
	pCamera->Look();

	int winX, winY;
	m_pRenderer->GetScreenCoordinatesFromWorldPosition(GetCenter(), &winX, &winY);

	m_screenPosition.x = static_cast<float>(winX);
	m_screenPosition.y = static_cast<float>(winY);
	m_pRenderer->PopMatrix();
}

void NPC::UpdateSubSelectionNamePicking(int pickingID, bool mousePressed)
{
	if (m_pVoxelCharacter != nullptr)
	{
		if (mousePressed)
		{
			m_subSelectionSelected = m_pVoxelCharacter->GetSubSelectionName(pickingID);
			CubbyGame::GetInstance()->GetFrontendManager()->SetCharacterSubSelection(m_subSelectionSelected);
		}
		else
		{
			m_subSelectionHover = m_pVoxelCharacter->GetSubSelectionName(pickingID);
		}
	}
}

void NPC::UpdateAggroRadius()
{
	if (m_pTargetEnemy != nullptr)
	{
		return;
	}

	Enemy* pClosestEnemy = nullptr;
	float closestDistance = 999.9f;

	int numEnemies = m_pEnemyManager->GetNumEnemies();
	for (int i = 0; i < numEnemies; ++i)
	{
		Enemy* pEnemy = m_pEnemyManager->GetEnemy(i);

		if (pEnemy->GetErase())
		{
			continue;
		}

		float distance = length(GetCenter() - pEnemy->GetCenter());

		if (distance < m_aggroRadius)
		{
			if (distance < closestDistance)
			{
				closestDistance = distance;
				pClosestEnemy = pEnemy;
			}
		}
	}

	if (pClosestEnemy != nullptr)
	{
		SetTargetEnemy(pClosestEnemy);
	}
}

void NPC::UpdateTimers(float dt)
{
	// Attack delay
	if (m_insideAttackRadius)
	{
		if (m_attackDelayTimer > 0.0f)
		{
			m_attackDelayTimer -= dt;
		}
	}

	// Jump timer
	if (m_jumpTimer >= 0.0f)
	{
		m_jumpTimer -= dt;
	}

	// Movement wait after attack
	if (m_movementWaitAfterAttackTimer > 0.0f)
	{
		m_movementWaitAfterAttackTimer -= dt;
	}

	// Idle animation wait timer
	if (m_waitBeforeStopMovingAnimationTimer >= 0.0f)
	{
		m_waitBeforeStopMovingAnimationTimer -= dt;
	}

	// Update aggro radius check timer
	if (m_updateAggroRadiusTimer > 0.0f)
	{
		m_updateAggroRadiusTimer -= dt;
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

void NPC::Render(bool outline, bool reflection, bool silhouette) const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);

	Color outlineColor(1.0f, 1.0f, 0.0f, 1.0f);
	if (m_hoverRender && m_outlineRender == false)
	{
		outlineColor = Color(1.0f, 0.0f, 1.0f, 1.0f);
	}

	m_pVoxelCharacter->Render(outline, reflection, silhouette, outlineColor, false);
	m_pVoxelCharacter->RenderWeapons(outline, reflection, silhouette, outlineColor);

	m_pRenderer->PopMatrix();
}

void NPC::RenderFace() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);

	m_pVoxelCharacter->RenderFace();

	m_pRenderer->PopMatrix();
}

void NPC::RenderWeaponTrails() const
{
	m_pRenderer->PushMatrix();
	m_pVoxelCharacter->RenderWeaponTrails();
	m_pRenderer->PopMatrix();
}

void NPC::RenderDebug()
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

	if (m_pVoxelCharacter != nullptr)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);
		//m_pRenderer->TranslateWorldMatrix(m_position.x, m_position.y, m_position.z);
		m_pVoxelCharacter->RenderFacingDebug();

		m_pRenderer->PopMatrix();
	}

	// Bones
	if (m_pVoxelCharacter != nullptr)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);

		m_pVoxelCharacter->RenderBones();

		m_pRenderer->PopMatrix();
	}

	// Forwards debug
	RenderForwardDebug();

	// Waypoints
	RenderWaypointsDebug();

	// Projectile hitbox
	RenderProjectileHitboxDebug();

	// Aggro radius
	RenderAggroRadiusDebug();

	// Movement position
	RenderMovementPositionDebug();
}

void NPC::RenderForwardDebug() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);

	m_pRenderer->ScaleWorldMatrix(m_pVoxelCharacter->GetCharacterScale(), m_pVoxelCharacter->GetCharacterScale(), m_pVoxelCharacter->GetCharacterScale());

	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->SetLineWidth(3.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);
	
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(m_forward.x*15.0f, m_forward.y*15.0f, m_forward.z*15.0f);

	m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 0.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(m_targetForward.x*15.0f, m_targetForward.y*15.0f, m_targetForward.z*15.0f);
	
	m_pRenderer->DisableImmediateMode();
	
	m_pRenderer->PopMatrix();
}

void NPC::RenderWaypointsDebug()
{
	m_pRenderer->PushMatrix();

	if (m_NPCState == NPCState::MovingToWayPoint)
	{
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->SetLineWidth(1.0f);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

		m_pRenderer->ImmediateColorAlpha(1.0f, 0.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateVertex(GetCenter().x, GetCenter().y + 0.15f, GetCenter().z);
		m_pRenderer->ImmediateVertex(m_targetPosition.x, m_targetPosition.y + 0.65f, m_targetPosition.z);
	
		m_pRenderer->DisableImmediateMode();

		m_pRenderer->SetLineWidth(3.0f);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);
		
		m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 1.0f, 1.0f);
		m_pRenderer->ImmediateVertex(m_targetPosition.x, m_targetPosition.y + 0.1f, m_targetPosition.z);
		m_pRenderer->ImmediateVertex(m_targetPosition.x, m_targetPosition.y + 0.8f, m_targetPosition.z);
		
		m_pRenderer->DisableImmediateMode();
	}

	for (size_t i = 0; i < m_vpWayPointList.size(); ++i)
	{
		float length = m_vpWayPointList[i]->m_xLength;
		float height = m_vpWayPointList[i]->m_yLength;
		float width = m_vpWayPointList[i]->m_zLength;

		m_pRenderer->PushMatrix();

		m_pRenderer->TranslateWorldMatrix(m_vpWayPointList[i]->m_position.x, m_vpWayPointList[i]->m_position.y, m_vpWayPointList[i]->m_position.z);

		m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
		m_pRenderer->SetCullMode(CullMode::NOCULL);
		m_pRenderer->SetLineWidth(1.0f);
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
		
		m_pRenderer->PopMatrix();

		// Connecting waypoints
		if (m_vpWayPointList[i]->m_pNextWaypoint != nullptr)
		{
			m_pRenderer->SetRenderMode(RenderMode::SOLID);
			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

			m_pRenderer->ImmediateColorAlpha(0.75f, 1.0f, 0.5f, 1.0f);
			m_pRenderer->ImmediateVertex(m_vpWayPointList[i]->m_position.x, m_vpWayPointList[i]->m_position.y, m_vpWayPointList[i]->m_position.z);
			m_pRenderer->ImmediateVertex(m_vpWayPointList[i]->m_pNextWaypoint->m_position.x, m_vpWayPointList[i]->m_pNextWaypoint->m_position.y, m_vpWayPointList[i]->m_pNextWaypoint->m_position.z);
			
			m_pRenderer->DisableImmediateMode();
		}
	}
	
	m_pRenderer->SetCullMode(CullMode::BACK);
	
	m_pRenderer->PopMatrix();
}

void NPC::RenderSubSelection(bool outline, bool silhouette) const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);

	if (m_subSelectionSelected != "")
	{
		Color outlineColor(1.0f, 1.0f, 0.0f, 1.0f);
		m_pVoxelCharacter->RenderSubSelection(m_subSelectionSelected, outline, silhouette, outlineColor);
	}

	if (m_subSelectionHover != "" && m_subSelectionHover != m_subSelectionSelected)
	{
		Color outlineColor(1.0f, 0.0f, 1.0f, 1.0f);
		m_pVoxelCharacter->RenderSubSelection(m_subSelectionHover, outline, silhouette, outlineColor);
	}

	m_pRenderer->PopMatrix();
}

void NPC::RenderSubSelectionNormal() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);

	Color outlineColor(1.0f, 1.0f, 0.0f, 1.0f);

	if (m_subSelectionSelected != "Head" && m_subSelectionHover != "Head")
	{
		m_pVoxelCharacter->RenderSubSelection("Head", false, false, outlineColor);
	}

	if (m_subSelectionSelected != "Body" && m_subSelectionHover != "Body")
	{
		m_pVoxelCharacter->RenderSubSelection("Body", false, false, outlineColor);
	}

	if (m_subSelectionSelected != "Legs" && m_subSelectionHover != "Legs")
	{
		m_pVoxelCharacter->RenderSubSelection("Legs", false, false, outlineColor);
	}

	if (m_subSelectionSelected != "Right_Shoulder" && m_subSelectionHover != "Right_Shoulder")
	{
		m_pVoxelCharacter->RenderSubSelection("Right_Shoulder", false, false, outlineColor);
	}

	if (m_subSelectionSelected != "Left_Shoulder" && m_subSelectionHover != "Left_Shoulder")
	{
		m_pVoxelCharacter->RenderSubSelection("Left_Shoulder", false, false, outlineColor);
	}

	if (m_subSelectionSelected != "Right_Hand" && m_subSelectionHover != "Right_Hand")
	{
		m_pVoxelCharacter->RenderSubSelection("Right_Hand", false, false, outlineColor);
	}

	if (m_subSelectionSelected != "Left_Hand" && m_subSelectionHover != "Left_Hand")
	{
		m_pVoxelCharacter->RenderSubSelection("Left_Hand", false, false, outlineColor);
	}

	if (m_subSelectionSelected != "Right_Foot" && m_subSelectionHover != "Right_Foot")
	{
		m_pVoxelCharacter->RenderSubSelection("Right_Foot", false, false, outlineColor);
	}

	if (m_subSelectionSelected != "Left_Foot" && m_subSelectionHover != "Left_Foot")
	{
		m_pVoxelCharacter->RenderSubSelection("Left_Foot", false, false, outlineColor);
	}

	m_pVoxelCharacter->RenderWeapons(false, false, false, outlineColor);
	
	m_pRenderer->PopMatrix();
}

void NPC::RenderSubSelectionNamePicking() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);

	Color outlineColor(1.0f, 1.0f, 0.0f, 1.0f);

	m_pVoxelCharacter->Render(false, false, false, outlineColor, true);

	m_pRenderer->PopMatrix();
}

void NPC::RenderProjectileHitboxDebug() const
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

void NPC::RenderAggroRadiusDebug() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);
	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->ImmediateColorAlpha(0.0f, 0.0f, 1.0f, 1.0f);

	m_pRenderer->DrawLineCircle(m_aggroRadius, 20);

	m_pRenderer->PopMatrix();
}

void NPC::RenderMovementPositionDebug() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(m_moveBackToPosition.x, m_moveBackToPosition.y, m_moveBackToPosition.z);
	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 1.0f, 1.0f);

	float length = 0.5f;

	m_pRenderer->SetLineWidth(2.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);
	
	m_pRenderer->ImmediateVertex(-length, 0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(length, 0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(0.0f, -length, 0.0f);
	m_pRenderer->ImmediateVertex(0.0f, length, 0.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, -length);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, length);
	
	m_pRenderer->DisableImmediateMode();
	
	m_pRenderer->PopMatrix();
}

// Callback functions
void NPC::_AttackEnabledTimerFinished(void* pData)
{
	NPC* pNPC = static_cast<NPC*>(pData);
	pNPC->AttackEnabledTimerFinished();
}

void NPC::AttackEnabledTimerFinished()
{
	m_attackEnabled = false;
}

void NPC::_AttackEnabledDelayTimerFinished(void* pData)
{
	NPC* pNPC = static_cast<NPC*>(pData);
	pNPC->AttackEnabledDelayTimerFinished();
}

void NPC::AttackEnabledDelayTimerFinished()
{
	if (m_erase)
	{
		return;
	}

	if (m_pTargetEnemy == nullptr)
	{
		return;
	}

	if (m_NPCCombatType == NPCCombatType::Staff)
	{
		glm::vec3 fireballSpawnPosition = GetCenter() + (m_forward * 0.75f) + (GetRightVector() * -0.4f) + (GetUpVector() * 0.25f);

		float powerAmount = 25.0f;
		glm::vec3 toTarget = m_pTargetEnemy->GetCenter() - GetCenter();

		glm::vec3 fireballSpawnVelocity = (normalize(toTarget) * powerAmount);

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(fireballSpawnPosition, fireballSpawnVelocity, 0.0f, "Resources/gamedata/items/Fireball/Fireball.item", 0.04f);
		pProjectile->SetProjectileType(true, false, false);
		pProjectile->SetOwner(nullptr, this, nullptr);
		pProjectile->SetGravityMultiplier(0.0f);
	}
	else if (m_NPCCombatType == NPCCombatType::FireballHands)
	{
		float powerAmount = 25.0f;
		
		glm::vec3 spellSpawnPosition = GetCenter() + (m_forward * 0.5f) + (GetUpVector() * 0.0f);

		if (m_attackCounter == 1)
		{
			spellSpawnPosition += -(GetRightVector() * 0.4f);
		}
		else
		{
			spellSpawnPosition += (GetRightVector() * 0.4f);
		}

		glm::vec3 toTarget = m_pTargetEnemy->GetProjectileHitboxCenter() - GetCenter();
		glm::vec3 spellSpawnVelocity = (normalize(toTarget) * powerAmount);

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(spellSpawnPosition, spellSpawnVelocity, 0.0f, "Resources/gamedata/items/Fireball/FireballBlue.item", 0.05f);
		pProjectile->SetProjectileType(true, false, false);
		pProjectile->SetOwner(nullptr, this, nullptr);
		pProjectile->SetGravityMultiplier(0.0f);
	}
}