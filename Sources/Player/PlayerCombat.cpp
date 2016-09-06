/*************************************************************************
> File Name: PlayerCombat.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    The player class contains all the functionality of a player, an entity that
>    is controlled and can act within the world and game.
> Created Time: 2016/08/05
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <CubbyGame.h>

#include <Maths/3DMaths.h>
#include <Models/VoxelObject.h>
#include <Utils/Interpolator.h>
#include <Utils/Random.h>

#include "Player.h"

// Combat
void Player::PressAttack()
{
	if (IsDead())
	{
		return;
	}

	if (IsBow())
	{
		if (m_bowAttackDelay <= 0.0f)
		{
			if (m_isChargingAttack == false)
			{
				m_isChargingAttack = true;
				m_chargeAmount = 0.0f;

				LoadWeapon(false, "Resources/gamedata/items/Arrow/ArrowHold.item");

				if (CubbyGame::GetInstance()->GetCameraMode() == CameraMode::FirstPerson)
				{
					// Make sure we set arrow alpha if we are in first person mode
					m_pVoxelCharacter->SetMeshAlpha(0.45f, true);
				}

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
		}
	}
	else if (IsBoomerang())
	{
		if (CanAttackRight())
		{
			if (m_canThrowWeapon)
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "SwordAttack1", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "SwordAttack1", 0.01f);

				Interpolator::GetInstance()->AddFloatInterpolation(&m_animationTimer, 0.0f, 0.22f, 0.22f, 0.0f, nullptr, _AttackAnimationTimerFinished, this);

				m_canAttackRight = false;
				m_canThrowWeapon = false;

				// Start weapon trails
				if (m_pVoxelCharacter->GetRightWeapon())
				{
					if (m_pVoxelCharacter->IsRightWeaponLoaded())
					{
						m_pVoxelCharacter->GetRightWeapon()->StartWeaponTrails();
					}
				}
			}
		}
	}
	else if (IsStaff())
	{
		if (m_magic >= 10.0f)
		{
			if (CanAttackRight())
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "StaffAttack", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "StaffAttack", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "StaffAttack", 0.01f);

				m_canInteruptCombatAnim = false;

				Interpolator::GetInstance()->AddFloatInterpolation(&m_animationTimer, 0.0f, 0.2f, 0.2f, 0.0f, nullptr, _AttackAnimationTimerFinished, this);

				m_magic -= 10.0f;
				CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();

				m_canAttackRight = false;
			}
		}
	}
	else if (IsWand())
	{
	}
	else if (IsBomb())
	{
		if (CanAttackRight())
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "SwordAttack2", 0.01f);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "SwordAttack2", 0.01f);

			Interpolator::GetInstance()->AddFloatInterpolation(&m_animationTimer, 0.0f, 0.25f, 0.25f, 0.0f, nullptr, _AttackAnimationTimerFinished, this);

			m_canAttackRight = false;
		}
	}
	else if (IsConsumable())
	{
	}
	else if (IsDagger())
	{
		InventoryItem* pRightHand = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::RightHand);
		InventoryItem* pLeftHand = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::LeftHand);

		if (pRightHand != nullptr && pRightHand->m_itemType == InventoryType::WeaponDagger && CanAttackRight())
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "SwordAttack2", 0.01f);

			Interpolator::GetInstance()->AddFloatInterpolation(&m_animationTimer, 0.0f, 0.3f, 0.3f, 0.0f, nullptr, _AttackAnimationTimerFinished, this);

			m_canInteruptCombatAnim = true;

			m_attackEnabledDelayTimer = 0.0f;
			m_attackSegmentAngle = 0.75f;
			float attackTime = 0.4f;
			float startRotation = -10.0f;
			float endRotation = -10.0f;
			float easingRotation = 0.0f;

			m_attackEnabled = true;
			m_attackEnabledTimer = 0.0f;
			m_attackRotation = startRotation;
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledTimer, 0.0f, attackTime, attackTime, 0.0f, nullptr, _AttackEnabledTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackRotation, startRotation, endRotation, attackTime, easingRotation);

			m_canAttackRight = false;
		}
		else if (pLeftHand != nullptr && pLeftHand->m_itemType == InventoryType::WeaponDagger && CanAttackLeft())
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "SwordAttack2", 0.01f);

			Interpolator::GetInstance()->AddFloatInterpolation(&m_animationTimer, 0.0f, 0.3f, 0.3f, 0.0f, nullptr, _AttackAnimationTimerFinished_Alternative, this);

			m_canInteruptCombatAnim = true;

			m_attackEnabledDelayTimer = 0.0f;
			m_attackSegmentAngle = 0.75f;
			float attackTime = 0.4f;
			float startRotation = -10.0f;
			float endRotation = -10.0f;
			float easingRotation = 0.0f;

			m_attackEnabled = true;
			m_attackEnabledTimer = 0.0f;
			m_attackRotation = startRotation;
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledTimer, 0.0f, attackTime, attackTime, 0.0f, nullptr, _AttackEnabledTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackRotation, startRotation, endRotation, attackTime, easingRotation);

			m_canAttackLeft = false;
		}
	}
	else if (IsHammer())
	{

	}
	else if (IsMace())
	{

	}
	else if (IsSickle())
	{

	}
	else if (IsPickaxe())
	{
		if (CanAttackRight())
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "Mine", 0.01f);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "Mine", 0.01f);

			m_canInteruptCombatAnim = false;

			Interpolator::GetInstance()->AddFloatInterpolation(&m_animationTimer, 0.0f, 0.4f, 0.4f, 0.0f, nullptr, _AttackAnimationTimerFinished, this);

			m_canAttackRight = false;
		}
	}
	else if (IsAxe())
	{
	}
	else if (IsTwoHandedSword())
	{
		if (CanAttackRight())
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "2HandedSwordAttack", 0.01f);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "2HandedSwordAttack", 0.01f);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "2HandedSwordAttack", 0.01f);

			m_canInteruptCombatAnim = false;

			m_attackEnabledDelayTimer = 0.175f;
			m_attackSegmentAngle = 0.60f;
			float attackTime = 0.6f;
			float startRotation = 240.0f;
			float endRotation = -60.0f;
			float easingRotation = -50.0f;

			m_attackEnabled = true;
			m_attackEnabledTimer = 0.0f;
			m_attackRotation = startRotation;
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledTimer, 0.0f, attackTime, attackTime, 0.0f, nullptr, _AttackEnabledTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackRotation, startRotation, endRotation, attackTime, easingRotation);

			m_canAttackRight = false;
		}
	}
	else if (IsSword())
	{
		if (CanAttackRight())
		{
			float attackTime = 0.0f;
			float startRotation = 0.0f;
			float endRotation = 0.0f;
			float easingRotation = 0.0f;

			int randomAttack = GetRandomNumber(1, 3);
			if (randomAttack == 1)
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "SwordAttack1", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "SwordAttack1", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "SwordAttack1", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "SwordAttack1", 0.01f);

				m_canInteruptCombatAnim = false;

				m_attackEnabledDelayTimer = 0.1f;
				m_attackSegmentAngle = 0.75f;
				attackTime = 0.4f;
				startRotation = -20.0f;
				endRotation = -20.0f;
				easingRotation = 0.0f;
			}
			else if (randomAttack == 2)
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "SwordAttack2", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::HeadBody, false, AnimationSections::HeadBody, "SwordAttack2", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "SwordAttack2", 0.01f);
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "SwordAttack1", 0.01f);

				m_canInteruptCombatAnim = false;

				m_attackEnabledDelayTimer = 0.2f;
				m_attackSegmentAngle = 0.75f;
				attackTime = 0.4f;
				startRotation = -10.0f;
				endRotation = -10.0f;
				easingRotation = 0.0f;
			}
			else if (randomAttack == 3)
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "SwordAttack3", 0.01f);

				m_canInteruptCombatAnim = false;

				m_attackEnabledDelayTimer = 0.0f;
				m_attackSegmentAngle = 0.60f;
				attackTime = 0.6f;
				startRotation = 360.0f;
				endRotation = -45.0f;
				easingRotation = -75.0f;
			}

			m_canAttackRight = false;

			m_attackEnabled = true;
			m_attackEnabledTimer = 0.0f;
			m_attackRotation = startRotation;
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledTimer, 0.0f, attackTime, attackTime, 0.0f, nullptr, _AttackEnabledTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackEnabledDelayTimer, m_attackEnabledDelayTimer, 0.0f, m_attackEnabledDelayTimer, 0.0f, nullptr, _AttackEnabledDelayTimerFinished, this);
			Interpolator::GetInstance()->AddFloatInterpolation(&m_attackRotation, startRotation, endRotation, attackTime, easingRotation);
		}
	}
	else if (IsBlockPlacing())
	{

	}
	else if (IsItemPlacing())
	{

	}
	else if (IsSceneryPlacing())
	{

	}
	else if (IsSpellHands())
	{
		if (m_magic >= 5.0f)
		{
			InventoryItem* pRightHand = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::RightHand);
			InventoryItem* pLeftHand = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::LeftHand);

			if (pRightHand != nullptr && pRightHand->m_itemType == InventoryType::WeaponSpellHands && CanAttackRight())
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "HandSpellCastRight", 0.01f);

				Interpolator::GetInstance()->AddFloatInterpolation(&m_animationTimer, 0.0f, 0.3f, 0.3f, 0.0f, nullptr, _AttackAnimationTimerFinished, this);

				m_magic -= 5.0f;
				CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();

				m_canAttackRight = false;
			}
			else if (pLeftHand != nullptr && pLeftHand->m_itemType == InventoryType::WeaponSpellHands && CanAttackLeft())
			{
				m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "HandSpellCastLeft", 0.01f);

				Interpolator::GetInstance()->AddFloatInterpolation(&m_animationTimer, 0.0f, 0.3f, 0.3f, 0.0f, nullptr, _AttackAnimationTimerFinished_Alternative, this);

				m_magic -= 5.0f;
				CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();

				m_canAttackLeft = false;
			}
		}
	}
	else if (IsShield())
	{

	}
	else if (IsTorch())
	{

	}
}

void Player::ReleaseAttack()
{
	if (IsDead())
	{
		return;
	}

	if (IsBow())
	{
		if (m_isChargingAttack == true)
		{
			UnloadWeapon(false);

			Projectile* pProjectile = m_pProjectileManager->CreateProjectile(m_chargeSpawnPosition, m_chargeSpawnVelocity, 0.0f, "Resources/gamedata/items/Arrow/Arrow.item", 0.08f);
			pProjectile->SetProjectileType(true, false, false);
			pProjectile->SetOwner(this, nullptr, nullptr);

			m_isChargingAttack = false;
			m_chargeAmount = 0.0f;
			m_bowAttackDelay = 0.125f;

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

bool Player::CanAttackLeft() const
{
	return m_canAttackLeft;
}

bool Player::CanAttackRight() const
{
	return m_canAttackRight;
}

bool Player::GetAttackEnabled() const
{
	return m_attackEnabled && (m_attackEnabledDelayTimer <= 0.0f);
}

float Player::GetAttackRadius() const
{
	return m_attackRadius;
}

float Player::GetAttackRotation() const
{
	return m_attackRotation;
}

float Player::GetAttackSegmentAngle() const
{
	return m_attackSegmentAngle;
}

void Player::CheckEnemyDamageRadius(Enemy* pEnemy)
{
	if (IsDead())
	{
		return;
	}

	if (pEnemy->GetAttackEnabled() == false)
	{
		// If attack is not enabled, just return
		return;
	}

	glm::vec3 distance = GetCenter() - pEnemy->GetCenter();
	float lengthToEnemy = length(distance);
	if (lengthToEnemy <= m_radius + pEnemy->GetAttackRadius())
	{
		glm::vec3 distance_minus_y = distance;
		distance_minus_y.y = 0.0f;
		glm::vec3 direction = normalize(distance_minus_y);

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

			float knockbackAmount = 16.0f;

			DoDamage(5.0f, damageColor, knockbackDirection, knockbackAmount, true);
		}
	}
}

void Player::CheckProjectileDamageRadius(Projectile* pProjectile)
{
	if (IsDead())
	{
		return;
	}

	if (pProjectile->CanAttackPlayer() == false && pProjectile->IsReturnToPlayer() == false)
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
		if (pProjectile->IsReturnToPlayer())
		{
			if (pProjectile->CanCatch())
			{
				pProjectile->PlayerCatch();

				m_canThrowWeapon = true;

				if (IsBoomerang())
				{
					m_pVoxelCharacter->SetRenderRightWeapon(true);

					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, true, AnimationSections::FullBody, "SwordAttack1", 0.01f);
					m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "SwordAttack1", 0.01f);

					// Start weapon trails
					if (m_pVoxelCharacter->GetRightWeapon())
					{
						if (m_pVoxelCharacter->IsRightWeaponLoaded())
						{
							m_pVoxelCharacter->GetRightWeapon()->StartWeaponTrails();
						}
					}
				}
			}
		}
		else
		{
			glm::vec3 knockbackDirection = (normalize(pProjectile->GetVelocity()) * 2.0f) + glm::vec3(0.0f, 1.0f, 0.0f);
			knockbackDirection = normalize(knockbackDirection);
			Color damageColor = Color(1.0f, 1.0f, 1.0f);

			float knockbackAmount = 16.0f;
			DoDamage(15.0f, damageColor, knockbackDirection, knockbackAmount, false);

			pProjectile->Explode();
		}
	}
}

void Player::DoDamage(float amount, Color textColor, glm::vec3 knockbackDirection, float knockbackAmount, bool createParticleHit)
{
	if (IsDead())
	{
		return;
	}

	if (m_damageTimer <= 0.0f)
	{
		float healthBefore = m_health;

		m_health -= amount;

		// Figure out if we are dead yet
		if (m_health <= 0.0f)
		{
			m_health = 0.0f;

			// Explode the voxel model
			Explode();
		}

		// Update HUD player data
		CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();

		// Play a hit response animation
		m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "HitResponse", 0.01f);

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

		// Set face to sad for a short while (until facial hit timer resets);
		m_pVoxelCharacter->PlayFacialExpression("Sad");
		m_hitFacialExpressionTimer = m_hitFacialExpressionTime;
		m_returnToNormalFacialExpressionAfterHit = false;

		m_damageTimer = m_damageTime;

		// Close the interaction GUI compoenents
		CubbyGame::GetInstance()->CloseInteractionGUI();
	}

	if (IsDead())
	{
		return;
	}

	if (m_knockbackTimer <= 0.0f)
	{
		m_velocity += knockbackDirection * knockbackAmount;

		m_knockbackTimer = m_knockbackTime;
	}
}

void Player::Explode()
{
	if (IsDead())
	{
		return;
	}

	m_dead = true;

	m_isChargingAttack = false;
	m_chargeAmount = 0.0f;

	CubbyGame::GetInstance()->GetHUD()->PlayerDied();
	CubbyGame::GetInstance()->ReleaseEnemyTarget();
	CubbyGame::GetInstance()->CloseAllGUIWindows();

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
					for (int animatedSectionsIndex = 0; animatedSectionsIndex < m_pVoxelCharacter->GetRightWeapon()->GetNumAimatedSections(); animatedSectionsIndex++)
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
					for (int animatedSectionsIndex = 0; animatedSectionsIndex < m_pVoxelCharacter->GetLeftWeapon()->GetNumAimatedSections(); animatedSectionsIndex++)
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

	// Create tombstone
	char tombstoneFileName[64];
	if (GetRandomNumber(0, 100) > 35)
	{
		sprintf(tombstoneFileName, "Resources/gamedata/items/Tombstone/Tombstone1.item");
	}
	else
	{
		sprintf(tombstoneFileName, "Resources/gamedata/items/Tombstone/Tombstone2.item");
	}
	Item* pTombstone = m_pItemManager->CreateItem(GetCenter(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), tombstoneFileName, ItemType::Tombstone, "Tombstone", false, false, 0.08f);
	pTombstone->SetVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
	pTombstone->SetRotation(glm::vec3(0.0f, GetRandomNumber(0, 360, 1), 0.0f));

	// Create a ghost
	m_createGhost = true;
	m_createGhostTimer = 1.5f;
}

void Player::Respawn()
{
	if (IsDead() == false)
	{
		return;
	}

	m_position = m_respawnPosition;

	// Make sure we create a chunk in the respawn position
	UpdateGridPosition();
	m_pChunkManager->CreateNewChunk(GetGridX(), GetGridY(), GetGridZ());

	m_health = m_maxHealth;

	CubbyGame::GetInstance()->GetHUD()->UpdatePlayerData();

	// Also go through all the equipped items and equip them
	for (int i = 0; i < static_cast<int>(EquipSlot::NumSlots); ++i)
	{
		InventoryItem* pItem = m_pInventoryManager->GetInventoryItemForEquipSlot(static_cast<EquipSlot>(i));
		if (pItem != nullptr)
		{
			EquipItem(pItem);
		}
	}

	m_dead = false;
}

// Enemy target
void Player::SetEnemyDied(Enemy* pEnemy)
{
	if (m_pTargetEnemy == pEnemy)
	{
		SetTargetEnemy(nullptr);
	}
}

void Player::SetTargetEnemy(Enemy* pTargetEnemy)
{
	m_pTargetEnemy = pTargetEnemy;

	if (m_pTargetEnemy != nullptr)
	{
		glm::vec3 toTarget = m_pTargetEnemy->GetCenter() - GetCenter();
		toTarget.y = 0.0f;
		m_forward = normalize(toTarget);

		CalculateWorldTransformMatrix();

		if (IsTwoHandedSword())
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "2HandedSwordPose", 0.2f);
		}
		else
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "TargetPose", 0.2f);
		}
	}
	else
	{
		SetRandomLookMode();

		if (IsStaff())
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "StaffPose", 0.15f);
		}
		else if (IsSpellHands())
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::RightArmHand, false, AnimationSections::RightArmHand, "HandSpellCastPose", 0.15f);
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::LeftArmHand, false, AnimationSections::LeftArmHand, "HandSpellCastPose", 0.15f);
		}
		else if (m_isChargingAttack == false)
		{
			m_pVoxelCharacter->BlendIntoAnimation(AnimationSections::FullBody, false, AnimationSections::FullBody, "BindPose", 0.2f);
		}
	}
}

Enemy* Player::GetTargetEnemy() const
{
	return m_pTargetEnemy;
}

// Projectile hitbox
ProjectileHitboxType Player::GetProjectileHitboxType() const
{
	return m_projectileHitboxType;
}

float Player::GetProjectileHitboxRadius() const
{
	return m_projectileHitboxRadius;
}

float Player::GetProjectileHitboxXLength() const
{
	return m_projectileHitboxXLength;
}

float Player::GetProjectileHitboxYLength() const
{
	return m_projectileHitboxYLength;
}

float Player::GetProjectileHitboxZLength() const
{
	return m_projectileHitboxZLength;
}

glm::vec3 Player::GetProjectileHitboxCenter() const
{
	return GetCenter() + m_projectileHitboxCenterOffset;
}

// World editing
void Player::DestroyBlock()
{
	if (m_blockSelection)
	{
		Chunk* pChunk = m_pChunkManager->GetChunkFromPosition(m_blockSelectionPos.x, m_blockSelectionPos.y, m_blockSelectionPos.z);

		if (pChunk != nullptr)
		{
			int blockX, blockY, blockZ;
			glm::vec3 blockPos;
			
			bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(m_blockSelectionPos.x, m_blockSelectionPos.y, m_blockSelectionPos.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);
			
			if (active)
			{
				float r;
				float g;
				float b;
				float a;
				pChunk->GetColor(blockX, blockY, blockZ, &r, &g, &b, &a);

				pChunk->StartBatchUpdate();
				pChunk->SetColor(blockX, blockY, blockZ, 0);
				pChunk->StopBatchUpdate();

				m_pChunkManager->CreateBlockDestroyParticleEffect(r, g, b, a, m_blockSelectionPos);

				// Create the collectible block item
				BlockType blockType = pChunk->GetBlockType(blockX, blockY, blockZ);
				m_pChunkManager->CreateCollectibleBlock(blockType, m_blockSelectionPos);
			}
		}

		m_blockSelection = false;
	}
}

// Static callbacks
void Player::_AttackEnabledTimerFinished(void* pData)
{
	Player* pPlayer = static_cast<Player*>(pData);
	pPlayer->AttackEnabledTimerFinished();
}

void Player::AttackEnabledTimerFinished()
{
	m_attackEnabled = false;
}

void Player::_AttackEnabledDelayTimerFinished(void* pData)
{
	Player* pPlayer = static_cast<Player*>(pData);
	pPlayer->AttackEnabledDelayTimerFinished();
}

void Player::AttackEnabledDelayTimerFinished()
{
	m_attackEnabledDelayTimer = 0.0f;
}

void Player::_AttackAnimationTimerFinished(void* pData)
{
	Player* pPlayer = static_cast<Player*>(pData);
	pPlayer->AttackAnimationTimerFinished();
}

void Player::AttackAnimationTimerFinished()
{
	if (IsBow())
	{

	}
	else if (IsBoomerang())
	{
		glm::vec3 boomerangSpawnPosition = GetCenter() + (m_forward * 0.75f) + (GetRightVector() * -0.4f) + (GetUpVector() * 0.5f);

		float cameraModification = (m_cameraForward.y * 17.5f);
		if (m_cameraForward.y < 0.0f)
		{
			cameraModification = (m_cameraForward.y * 5.0f);
		}

		glm::vec3 boomerangTarget = boomerangSpawnPosition + m_forward * 15.0f + (glm::vec3(0.0f, 1.0f, 0.0f) * cameraModification);
		if (m_pTargetEnemy != nullptr)
		{
			boomerangTarget = m_pTargetEnemy->GetProjectileHitboxCenter();
			if (m_pTargetEnemy->IsMoving())
			{
				boomerangTarget += m_pTargetEnemy->GetForwardVector() * (m_pTargetEnemy->GetMovementSpeed() / 3.0f);
			}
		}

		float curveTime = length(boomerangTarget - boomerangSpawnPosition) / 15.0f;
		if (curveTime <= 0.4f)
		{
			curveTime = 0.4f;
		}

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(boomerangSpawnPosition, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, "Resources/gamedata/weapons/Boomerang/BoomerangThrown.weapon", 0.05f);
		pProjectile->SetProjectileType(true, false, false);
		pProjectile->SetOwner(this, nullptr, nullptr);
		pProjectile->SetGravityMultiplier(0.0f);
		pProjectile->SetReturnToPlayer(true);
		pProjectile->SetProjectileCurveParams(m_forward, boomerangTarget, curveTime);
		pProjectile->SetWorldCollisionEnabled(true);

		m_pVoxelCharacter->SetRenderRightWeapon(false);
	}
	else if (IsStaff())
	{
		float powerAmount = 25.0f;
		float cameraMultiplier = 25.0f;

		glm::vec3 spellSpawnPosition = GetCenter() + (m_forward * 1.25f) + (GetUpVector() * 0.25f);

		if (CubbyGame::GetInstance()->GetCameraMode() == CameraMode::FirstPerson)
		{
			cameraMultiplier = 30.0f;
			spellSpawnPosition.y += 0.75f;
		}
			
		glm::vec3 spellSpawnVelocity = m_forward * powerAmount + glm::vec3(0.0f, 1.0f, 0.0f) * (m_cameraForward.y * cameraMultiplier);

		if (m_pTargetEnemy != nullptr)
		{
			glm::vec3 toTarget = m_pTargetEnemy->GetProjectileHitboxCenter() - GetCenter();
			spellSpawnVelocity = (normalize(toTarget) * powerAmount);
		}

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(spellSpawnPosition, spellSpawnVelocity, 0.0f, "Resources/gamedata/items/Fireball/Fireball.item", 0.05f);
		pProjectile->SetProjectileType(true, false, false);
		pProjectile->SetOwner(this, nullptr, nullptr);
		pProjectile->SetGravityMultiplier(0.0f);
	}
	else if (IsWand())
	{

	}
	else if (IsBomb())
	{
		glm::vec3 bombSpawnPosition = GetCenter() + (m_forward * 0.75f) + (GetUpVector() * 0.5f);

		float liftAmount = 8.0f;
		float powerAmount = 30.0f;
		float cameraMultiplier = 25.0f;

		if (CubbyGame::GetInstance()->GetCameraMode() == CameraMode::FirstPerson)
		{
			cameraMultiplier = 30.0f;
		}

		glm::vec3 bombSpawnVelocity;
		if (m_pTargetEnemy)
		{
			// Enemy target
			glm::vec3 toTarget = m_pTargetEnemy->GetCenter() - GetCenter();
			float toTargetDistance = length(toTarget);
			liftAmount += toTargetDistance * 0.04f;
			bombSpawnVelocity = (normalize(toTarget) * powerAmount) + glm::vec3(0.0f, liftAmount, 0.0f);
		}
		else
		{
			bombSpawnVelocity = (m_forward * powerAmount) + (GetUpVector() * liftAmount) + glm::vec3(0.0f, 1.0f, 0.0f) * (m_cameraForward.y * cameraMultiplier);
		}

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(bombSpawnPosition, bombSpawnVelocity, 0.0f, "Resources/gamedata/items/Bomb/BombThrown.item", 0.05f);
		pProjectile->SetProjectileType(true, false, false);
		pProjectile->SetOwner(this, nullptr, nullptr);
		pProjectile->SetGravityMultiplier(3.5f);
		float explodeRadius = 3.5f - (GetRandomNumber(-150, 0, 2) * 0.01f);
		pProjectile->SetExplodingProjectile(true, explodeRadius);

		InventoryItem* pItem = m_pInventoryManager->GetInventoryItemForEquipSlot(EquipSlot::RightHand);
		if (pItem != nullptr)
		{
			if (pItem->m_quantity != -1)
			{
				pItem->m_quantity -= 1;
			}
			if (pItem->m_quantity == 0)
			{
				// Remove this item from the manager, and remove it from the inventory and GUI
				UnequipItem(EquipSlot::RightHand, false, false);
				m_pInventoryManager->RemoveInventoryItem(EquipSlot::RightHand);
				m_pActionBar->RemoveInventoryItemFromActionBar(pItem->m_title);
			}
		}
	}
	else if (IsConsumable())
	{

	}
	else if (IsDagger())
	{

	}
	else if (IsHammer())
	{

	}
	else if (IsMace())
	{

	}
	else if (IsSickle())
	{

	}
	else if (IsPickaxe())
	{
		Item* pInteractItem = CubbyGame::GetInstance()->GetInteractItem();
		if (pInteractItem != nullptr)
		{
			pInteractItem->Interact();
		}
		else
		{
			DestroyBlock();
		}
	}
	else if (IsAxe())
	{

	}
	else if (IsTwoHandedSword())
	{

	}
	else if (IsSword())
	{

	}
	else if (IsBlockPlacing())
	{

	}
	else if (IsItemPlacing())
	{

	}
	else if (IsSceneryPlacing())
	{

	}
	else if (IsSpellHands())
	{
		float powerAmount = 25.0f;
		float cameraMultiplier = 25.0f;

		glm::vec3 spellSpawnPosition = GetCenter() + (m_forward * 0.5f) + (GetUpVector() * 0.0f);

		// For right hand
		spellSpawnPosition += -(GetRightVector() * 0.4f);

		if (CubbyGame::GetInstance()->GetCameraMode() == CameraMode::FirstPerson)
		{
			cameraMultiplier = 30.0f;
			spellSpawnPosition.y += 0.75f;
		}

		glm::vec3 spellSpawnVelocity = m_forward * powerAmount + glm::vec3(0.0f, 1.0f, 0.0f) * (m_cameraForward.y * cameraMultiplier);

		if (m_pTargetEnemy != nullptr)
		{
			glm::vec3 toTarget = m_pTargetEnemy->GetProjectileHitboxCenter() - GetCenter();
			spellSpawnVelocity = (normalize(toTarget) * powerAmount);
		}

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(spellSpawnPosition, spellSpawnVelocity, 0.0f, "Resources/gamedata/items/Fireball/FireballBlue.item", 0.05f);
		pProjectile->SetProjectileType(true, false, false);
		pProjectile->SetOwner(this, nullptr, nullptr);
		pProjectile->SetGravityMultiplier(0.0f);
	}
	else if (IsShield())
	{

	}
	else if (IsTorch())
	{

	}
}

void Player::_AttackAnimationTimerFinished_Alternative(void* pData)
{
	Player* pPlayer = static_cast<Player*>(pData);
	pPlayer->AttackAnimationTimerFinished_Alternative();
}

void Player::AttackAnimationTimerFinished_Alternative()
{
	if (IsBow())
	{

	}
	else if (IsBoomerang())
	{

	}
	else if (IsStaff())
	{

	}
	else if (IsWand())
	{

	}
	else if (IsBomb())
	{

	}
	else if (IsConsumable())
	{

	}
	else if (IsDagger())
	{

	}
	else if (IsHammer())
	{

	}
	else if (IsMace())
	{

	}
	else if (IsSickle())
	{

	}
	else if (IsPickaxe())
	{

	}
	else if (IsAxe())
	{

	}
	else if (IsTwoHandedSword())
	{

	}
	else if (IsSword())
	{

	}
	else if (IsBlockPlacing())
	{

	}
	else if (IsItemPlacing())
	{

	}
	else if (IsSceneryPlacing())
	{

	}
	else if (IsSpellHands())
	{
		float powerAmount = 25.0f;
		float cameraMultiplier = 25.0f;

		glm::vec3 spellSpawnPosition = GetCenter() + (m_forward * 0.5f) + (GetUpVector() * 0.0f);

		// For left hand
		spellSpawnPosition += (GetRightVector() * 0.4f);

		if (CubbyGame::GetInstance()->GetCameraMode() == CameraMode::FirstPerson)
		{
			cameraMultiplier = 30.0f;
			spellSpawnPosition.y += 0.75f;
		}

		glm::vec3 spellSpawnVelocity = m_forward * powerAmount + glm::vec3(0.0f, 1.0f, 0.0f) * (m_cameraForward.y * cameraMultiplier);

		if (m_pTargetEnemy != nullptr)
		{
			glm::vec3 toTarget = m_pTargetEnemy->GetProjectileHitboxCenter() - GetCenter();
			spellSpawnVelocity = (normalize(toTarget) * powerAmount);
		}

		Projectile* pProjectile = m_pProjectileManager->CreateProjectile(spellSpawnPosition, spellSpawnVelocity, 0.0f, "Resources/gamedata/items/Fireball/FireballBlue.item", 0.05f);
		pProjectile->SetProjectileType(true, false, false);
		pProjectile->SetOwner(this, nullptr, nullptr);
		pProjectile->SetGravityMultiplier(0.0f);
	}
	else if (IsShield())
	{

	}
	else if (IsTorch())
	{

	}
}