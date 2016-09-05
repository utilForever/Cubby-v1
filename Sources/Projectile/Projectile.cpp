/*************************************************************************
> File Name: Projectile.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Projectile class.
> Created Time: 2016/07/13
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <Lighting/LightingManager.h>
#include <Maths/3DMaths.h>
#include <Models/VoxelObject.h>
#include <Particles/BlockParticleManager.h>
#include <Player/Player.h>
#include <Utils/Interpolator.h>

#include "Projectile.h"

// Constructor, Destructor
Projectile::Projectile(Renderer* pRenderer, ChunkManager* pChunkManager, QubicleBinaryManager* pQubicleBinaryManager, const char* objectFileName, float scale) :
	m_pRenderer(pRenderer), m_pChunkManager(pChunkManager), m_pLightingManager(nullptr),
	m_pBlockParticleManager(nullptr), m_pQubicleBinaryManager(pQubicleBinaryManager),
	m_pGameWindow(nullptr), m_pPlayer(nullptr),
	m_isErase(false), m_radius(0.25f), m_renderScale(scale),
	m_gravityDirection(glm::vec3(0.0f, -1.0f, 0.0f)), m_gravityMultiplier(2.5f), m_forward(glm::vec3(0.0f, 0.0f, 1.0f)),
	m_gridPositionX(0), m_gridPositionY(0), m_gridPositionZ(0),
	m_pCachedGridChunk(nullptr), m_attackEnemies(false), m_attackPlayer(false), m_attackNPCs(false),
	m_worldCollisionEnabled(true), m_explodeWorld(false), m_explosionRadius(1.0f),
	m_returnToPlayer(false), m_catchTimer(0.0f), m_curveTimer(1.0f), m_curveTime(1.0f), m_rightCurve(true), m_returningDirectToPlayer(false),
	m_pOwnedPlayer(nullptr), m_pOwnedNPC(nullptr), m_pOwnedEnemy(nullptr),
	m_pVoxeProjectile(nullptr)
{
	LoadItem(objectFileName);

	m_pVoxeProjectile->StartWeaponTrails();
}

Projectile::~Projectile()
{
	m_pVoxeProjectile->StopWeaponTrails();

	UnloadEffectsAndLights();

	if (m_pVoxeProjectile != nullptr)
	{
		delete m_pVoxeProjectile;
	}
}

void Projectile::SetLightingManager(LightingManager* pLightingManager)
{
	m_pLightingManager = pLightingManager;
}

void Projectile::SetGameWindow(GameWindow* pGameWindow)
{
	m_pGameWindow = pGameWindow;
}

void Projectile::SetBlockParticleManager(BlockParticleManager* pBlockParticleManager)
{
	m_pBlockParticleManager = pBlockParticleManager;
}

void Projectile::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

// Unloading
void Projectile::UnloadEffectsAndLights() const
{
	// Lights
	for (int i = 0; i < m_pVoxeProjectile->GetNumLights(); ++i)
	{
		unsigned int lightID;
		glm::vec3 lightPos;
		float lightRadius;
		float lightDiffuseMultiplier;
		Color lightColor;
		bool connectedToSegment;

		m_pVoxeProjectile->GetLightParams(i, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

		if (lightID != -1)
		{
			m_pLightingManager->RemoveLight(lightID);
			m_pVoxeProjectile->SetLightingID(i, -1);

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

			float scale = m_renderScale;
			unsigned int id;

			m_pLightingManager->AddDyingLight(lightPos, lightRadius * scale, lightDiffuseMultiplier, lightColor, 2.0f, &id);
		}
	}

	// Particle Effects
	for (int i = 0; i < m_pVoxeProjectile->GetNumParticleEffects(); ++i)
	{
		unsigned int particleEffectID;
		glm::vec3 particleEffectPos;
		std::string effectName;
		bool connectedToSegment;

		m_pVoxeProjectile->GetParticleEffectParams(i, &particleEffectID, &particleEffectPos, &effectName, &connectedToSegment);

		if (particleEffectID != -1)
		{
			m_pBlockParticleManager->DestroyParticleEffect(particleEffectID);
			m_pVoxeProjectile->SetParticleEffectID(i, -1);
		}
	}
}

// Erase flag
bool Projectile::GetErase() const
{
	return m_isErase;
}

void Projectile::SetErase(bool isErase)
{
	m_isErase = isErase;
}

// Setup
void Projectile::LoadItem(const char* objectFileName)
{
	if (m_pVoxeProjectile == nullptr)
	{
		m_pVoxeProjectile = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
	}

	m_pVoxeProjectile->SetVoxelCharacterParent(nullptr);
	m_pVoxeProjectile->LoadWeapon(objectFileName, false);

	// Lights
	for (int i = 0; i < m_pVoxeProjectile->GetNumLights(); i++)
	{
		unsigned int lightID;
		glm::vec3 lightPos;
		float lightRadius;
		float lightDiffuseMultiplier;
		Color lightColor;
		bool connectedToSegment;

		m_pVoxeProjectile->GetLightParams(i, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

		if (lightID != -1)
		{
			m_pLightingManager->RemoveLight(lightID);
			m_pVoxeProjectile->SetLightingID(i, -1);
		}
	}

	// Particle effects
	for (int i = 0; i < m_pVoxeProjectile->GetNumParticleEffects(); ++i)
	{
		unsigned int particleEffectID;
		glm::vec3 particleEffectPos;
		std::string effectName;
		bool connectedToSegment;

		m_pVoxeProjectile->GetParticleEffectParams(i, &particleEffectID, &particleEffectPos, &effectName, &connectedToSegment);

		if (particleEffectID != -1)
		{
			m_pBlockParticleManager->DestroyParticleEffect(particleEffectID);
			m_pVoxeProjectile->SetParticleEffectID(i, -1);
		}
	}
}

// Accessors / Setters
void Projectile::SetPosition(glm::vec3 pos)
{
	m_position = pos;
}

glm::vec3 Projectile::GetPosition() const
{
	return m_position;
}

void Projectile::SetVelocity(glm::vec3 vel)
{
	m_velocity = vel;
}

glm::vec3 Projectile::GetVelocity() const
{
	return m_velocity;
}

void Projectile::SetRotation(float rot)
{
	float angle = DegreeToRadian(rot);
	m_forward = glm::vec3(sin(angle), 0.0f, cos(angle));
}

float Projectile::GetRotation() const
{
	float rotationAngle = acos(dot(glm::vec3(0.0f, 0.0f, 1.0f), m_forward));
	rotationAngle = RadianToDegree(rotationAngle);

	if (m_forward.x < 0.0f)
	{
		rotationAngle = (360.0f - rotationAngle);
	}

	return rotationAngle;
}

float Projectile::GetScale() const
{
	return m_renderScale;
}

float Projectile::GetRadius() const
{
	return m_radius;
}

void Projectile::UpdateRadius()
{
	m_radius = m_pVoxeProjectile->GetRenderScale() / 0.14f;
}

glm::vec3 Projectile::GetCenter() const
{
	glm::vec3 center = m_position + glm::vec3(0.0f, m_radius, 0.0f);
	return center;
}

glm::vec3 Projectile::GetForwardVector() const
{
	return m_worldMatrix.GetForwardVector();
}

glm::vec3 Projectile::GetRightVector() const
{
	return m_worldMatrix.GetRightVector();
}

glm::vec3 Projectile::GetUpVector() const
{
	return m_worldMatrix.GetUpVector();
}

void Projectile::SetGravityDirection(glm::vec3 dir)
{
	m_gravityDirection = dir;

	if (length(m_gravityDirection) >= 0.0001f)
	{
		m_gravityDirection = normalize(m_gravityDirection);
	}
}

void Projectile::SetGravityMultiplier(float multiplier)
{
	m_gravityMultiplier = multiplier;
}

void Projectile::SetProjectileCurveParams(glm::vec3 forward, glm::vec3 target, float curveTime)
{
	bool goRight = true;

	glm::vec3 crossRight = cross(forward, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 targetDiff = target - m_position;

	// First part of curve
	m_bezierStartRight = m_position;
	m_bezierEndRight = target;

	if (goRight)
	{
		m_bezierControlRight = target + crossRight * length(targetDiff);
	}
	else
	{
		m_bezierControlRight = target - crossRight * length(targetDiff);
	}

	// Second part of curve
	m_bezierStartLeft = m_position;
	m_bezierEndLeft = target;
	
	if (goRight)
	{
		m_bezierControlLeft = target - crossRight * length(targetDiff);
	}
	else
	{
		m_bezierControlLeft = target + crossRight * length(targetDiff);
	}

	m_curveTime = curveTime;
	m_curveTimer = curveTime;
	m_rightCurve = true;

	Interpolator::GetInstance()->AddFloatInterpolation(&m_curveTimer, m_curveTime, 0.0f, m_curveTime, 0.0f, nullptr, _RightCurveTimerFinished, this);
}

void Projectile::SetWorldCollisionEnabled(bool enabled)
{
	m_worldCollisionEnabled = enabled;
}

// Projectile type
void Projectile::SetProjectileType(bool attackEnemies, bool attackPlayer, bool attackNPCs)
{
	m_attackEnemies = attackEnemies;
	m_attackPlayer = attackPlayer;
	m_attackNPCs = attackNPCs;
}

bool Projectile::CanAttackEnemies() const
{
	return m_attackEnemies;
}

bool Projectile::CanAttackPlayer() const
{
	return m_attackPlayer;
}

bool Projectile::CanAttackNPCs() const
{
	return m_attackNPCs;
}

void Projectile::SetReturnToPlayer(bool returnToPlayer)
{
	m_returnToPlayer = returnToPlayer;

	if (m_returnToPlayer)
	{
		m_catchTimer = 0.75f;
	}
}

bool Projectile::IsReturnToPlayer() const
{
	return m_returnToPlayer;
}

void Projectile::SetExplodingProjectile(bool exploding, float radius)
{
	m_explodeWorld = exploding;
	m_explosionRadius = radius;
}

void Projectile::SetOwner(Player* pPlayer, NPC* pNPC, Enemy* pEnemy)
{
	m_pOwnedPlayer = pPlayer;
	m_pOwnedNPC = pNPC;
	m_pOwnedEnemy = pEnemy;
}

Player* Projectile::GetPlayerOwner() const
{
	return m_pOwnedPlayer;
}

NPC* Projectile::GetNPCOwner() const
{
	return m_pOwnedNPC;
}

Enemy* Projectile::GetEnemyOwner() const
{
	return m_pOwnedEnemy;
}

// Catching
bool Projectile::CanCatch() const
{
	return m_returnToPlayer && m_catchTimer <= 0.0f;
}

void Projectile::PlayerCatch()
{
	m_isErase = true;
}

// Explode
void Projectile::Explode()
{
	CalculateWorldTransformMatrix();

	if (m_pVoxeProjectile != nullptr)
	{
		for (int animatedSectionsIndex = 0; animatedSectionsIndex < m_pVoxeProjectile->GetNumAimatedSections(); ++animatedSectionsIndex)
		{
			AnimatedSection* pAnimatedSection = m_pVoxeProjectile->GetAnimatedSection(animatedSectionsIndex);
			QubicleBinary* pQubicleModel = pAnimatedSection->pVoxelObject->GetQubicleModel();
		
			m_pBlockParticleManager->ExplodeQubicleBinary(pQubicleModel, m_renderScale, 100);
		}
	}

	if (m_explodeWorld)
	{
		m_pChunkManager->ExplodeSphere(GetCenter(), m_explosionRadius);
	}

	m_isErase = true;
}

// Grid
void Projectile::UpdateGridPosition()
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

Chunk* Projectile::GetCachedGridChunkOrFromPosition(glm::vec3 pos) const
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

// Rendering Helpers
void Projectile::CalculateWorldTransformMatrix()
{
	// Make sure we are always pointing towards our velocity
	if (length(m_velocity) > 0.01f)
	{
		m_forward = normalize(m_velocity);
	}

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

	float matrix[16] = {
		right.x, right.y, right.z, 0.0f,
		up.x, up.y, up.z, 0.0f,
		forward.x, forward.y, forward.z, 0.0f,
		GetCenter().x, GetCenter().y, GetCenter().z, 1.0f
	};

	m_worldMatrix.SetValues(matrix);
}

// Updating
void Projectile::Update(float dt)
{
	if (m_pVoxeProjectile != nullptr)
	{
		CalculateWorldTransformMatrix();

		m_pVoxeProjectile->Update(dt);

		m_pVoxeProjectile->SetWeaponTrailsParams(m_worldMatrix, m_renderScale);
		m_pVoxeProjectile->CreateWeaponTrailPoint();
	}

	if (m_isErase == true)
	{
		return;
	}

	// Update catch timer
	if (m_catchTimer > 0.0f)
	{
		m_catchTimer -= dt;
	}

	// Update grid position
	UpdateGridPosition();

	glm::vec3 acceleration = (m_gravityDirection * 9.81f) * m_gravityMultiplier;

	if (m_returnToPlayer)
	{
		float t = 1.0f - (m_curveTimer / m_curveTime);

		m_bezierStartLeft = m_pPlayer->GetCenter();

		Bezier3 curveRight(m_bezierStartRight, m_bezierEndRight, m_bezierControlRight);
		Bezier3 curveLeft(m_bezierStartLeft, m_bezierEndLeft, m_bezierControlLeft);

		glm::vec3 pos, nextPos;

		if (m_rightCurve)
		{
			pos = curveRight.GetInterpolatedPoint(t);
			nextPos = curveRight.GetInterpolatedPoint(t + 0.01f);
		}
		else
		{
			pos = curveLeft.GetInterpolatedPoint(t);
			nextPos = curveLeft.GetInterpolatedPoint(t - 0.01f);
		}

		m_position = pos;
		m_velocity = normalize(nextPos - pos);
	}

	// Integrate velocity and position
	m_velocity += acceleration * dt;
	m_position += m_velocity * dt;

	if (m_worldCollisionEnabled)
	{
		glm::vec3 floorPosition;

		if (m_pChunkManager->FindClosestFloor(GetCenter(), &floorPosition) == false)
		{
			if (m_returnToPlayer)
			{
				if (m_returningDirectToPlayer == false)
				{
					Interpolator::GetInstance()->RemoveFloatInterpolationByVariable(&m_curveTimer);

					// Go straight back to player
					m_bezierStartLeft = m_pPlayer->GetCenter();
					m_bezierEndLeft = m_position;
					m_bezierControlLeft = m_position;
					m_curveTime = 1.0f;
					m_curveTimer = 0.0f;
					m_rightCurve = false;

					Interpolator::GetInstance()->AddFloatInterpolation(&m_curveTimer, 0.0f, m_curveTime, m_curveTime, 0.0f);

					m_returningDirectToPlayer = true;
				}
			}
			else
			{
				Explode();
			}
		}
		else
		{
			int blockX, blockY, blockZ;
			glm::vec3 blockPos;

			Chunk* pChunk = GetCachedGridChunkOrFromPosition(m_position);
			bool active = m_pChunkManager->GetBlockActiveFrom3DPosition(m_position.x, m_position.y, m_position.z, &blockPos, &blockX, &blockY, &blockZ, &pChunk);

			if (pChunk != nullptr && active == true)
			{
				if (m_returnToPlayer)
				{
					if (m_returningDirectToPlayer == false)
					{
						Interpolator::GetInstance()->RemoveFloatInterpolationByVariable(&m_curveTimer);

						// Go straight back to player
						m_bezierStartLeft = m_pPlayer->GetCenter();
						m_bezierEndLeft = m_position;
						m_bezierControlLeft = m_position;
						m_curveTime = 1.0f;
						m_curveTimer = 0.0f;
						m_rightCurve = false;

						Interpolator::GetInstance()->AddFloatInterpolation(&m_curveTimer, 0.0f, m_curveTime, m_curveTime, 0.0f);

						m_returningDirectToPlayer = true;
					}
				}
				else
				{
					// Roll back the integration, since we will intersect the block otherwise
					m_position -= m_velocity * dt;

					m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);

					Explode();
				}
			}
		}
	}

	m_previousPosition = GetCenter();
}

void Projectile::UpdateProjectileLights() const
{
	if (m_isErase)
	{
		return;
	}

	if (m_pVoxeProjectile != nullptr)
	{
		for (int i = 0; i < m_pVoxeProjectile->GetNumLights(); ++i)
		{
			unsigned int lightID;
			glm::vec3 lightPos;
			float lightRadius;
			float lightDiffuseMultiplier;
			Color lightColor;
			bool connectedToSegment;

			m_pVoxeProjectile->GetLightParams(i, &lightID, &lightPos, &lightRadius, &lightDiffuseMultiplier, &lightColor, &connectedToSegment);

			if (lightID == -1)
			{
				m_pLightingManager->AddLight(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 1.0f, Color(1.0f, 1.0f, 1.0f, 1.0f), &lightID);
				m_pVoxeProjectile->SetLightingID(i, lightID);
			}

			if (connectedToSegment == false)
			{
				lightPos *= m_renderScale;

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

			m_pLightingManager->UpdateLightPosition(lightID, lightPos);
			m_pLightingManager->UpdateLightRadius(lightID, lightRadius * m_renderScale);
			m_pLightingManager->UpdateLightDiffuseMultiplier(lightID, lightDiffuseMultiplier);
			m_pLightingManager->UpdateLightColor(lightID, lightColor);
		}
	}
}

void Projectile::UpdateProjectileParticleEffects() const
{
	if (m_isErase)
	{
		return;
	}

	if (m_pVoxeProjectile != nullptr)
	{
		for (int i = 0; i < m_pVoxeProjectile->GetNumParticleEffects(); ++i)
		{
			unsigned int particleEffectID;
			glm::vec3 particleEffectPos;
			std::string effectName;
			bool connectedToSegment;

			m_pVoxeProjectile->GetParticleEffectParams(i, &particleEffectID, &particleEffectPos, &effectName, &connectedToSegment);

			if (particleEffectID == -1)
			{
				m_pBlockParticleManager->ImportParticleEffect(effectName, particleEffectPos, &particleEffectID);
				m_pVoxeProjectile->SetParticleEffectID(i, particleEffectID);
			}

			if (connectedToSegment == false)
			{
				particleEffectPos *= m_renderScale;

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

			m_pBlockParticleManager->UpdateParticleEffectPosition(particleEffectID, particleEffectPos, particleEffectPos);
		}
	}
}

// Rendering
void Projectile::Render() const
{
	if (m_isErase == true)
	{
		return;
	}

	if (m_pVoxeProjectile != nullptr)
	{
		Color outlineColor(1.0f, 1.0f, 0.0f, 1.0f);

		m_pRenderer->PushMatrix();

		m_pRenderer->MultiplyWorldMatrix(m_worldMatrix);
		m_pRenderer->ScaleWorldMatrix(m_renderScale, m_renderScale, m_renderScale);
		m_pVoxeProjectile->Render(false, false, false, outlineColor);

		m_pRenderer->PopMatrix();
	}
}

void Projectile::RenderWeaponTrails() const
{
	m_pRenderer->PushMatrix();

	m_pVoxeProjectile->RenderWeaponTrails();

	m_pRenderer->PopMatrix();
}

void Projectile::RenderDebug() const
{
	if (m_isErase == true)
	{
		return;
	}

	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);

	m_pRenderer->PushMatrix();

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);

	m_pRenderer->RotateWorldMatrix(0.0f, GetRotation(), 0.0f);

	m_pRenderer->DrawSphere(m_radius, 20, 20);

	m_pRenderer->PopMatrix();

	m_pRenderer->PopMatrix();

	RenderForwardDebug();

	RenderDebugReturnCurve();
}

void Projectile::RenderDebugReturnCurve() const
{
	Bezier3 curveRight(m_bezierStartRight, m_bezierEndRight, m_bezierControlRight);
	Bezier3 curveLeft(m_bezierStartLeft, m_bezierEndLeft, m_bezierControlLeft);

	m_pRenderer->PushMatrix();

	m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	m_pRenderer->SetCullMode(CullMode::NOCULL);
	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->DrawBezier(curveRight, 20);
	m_pRenderer->DrawBezier(curveLeft, 20);

	m_pRenderer->PopMatrix();
}

void Projectile::RenderForwardDebug() const
{
	m_pRenderer->PushMatrix();

	m_pRenderer->TranslateWorldMatrix(GetCenter().x, GetCenter().y, GetCenter().z);

	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->SetLineWidth(3.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(m_forward.x * 1.5f, m_forward.y * 1.5f, m_forward.z * 1.5f);

	m_pRenderer->DisableImmediateMode();

	m_pRenderer->PopMatrix();
}

void Projectile::_RightCurveTimerFinished(void* pData)
{
	Projectile* pProjectile = static_cast<Projectile*>(pData);
	pProjectile->RightCurveTimerFinished();
}

void Projectile::RightCurveTimerFinished()
{
	m_curveTimer = 0.0f;
	m_rightCurve = false;

	Interpolator::GetInstance()->AddFloatInterpolation(&m_curveTimer, 0.0f, m_curveTime, m_curveTime, 0.0f);
}
