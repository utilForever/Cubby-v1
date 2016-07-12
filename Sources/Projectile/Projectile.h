/*************************************************************************
> File Name: Projectile.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Projectile class.
> Created Time: 2016/07/13
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_PROJECTILE_H
#define CUBBY_PROJECTILE_H

#include <Blocks/ChunkManager.h>
#include <Models/VoxelWeapon.h>

// Forward declaration
class LightingManager;
class GameWindow;
class NPC;
class Enemy;
class Player;

enum class ProjectileHitboxType
{
	Sphere = 0,
	Cube,
};

class Projectile
{
public:
	// Constructor, Destructor
	Projectile(Renderer* pRenderer, ChunkManager* pChunkManager, QubicleBinaryManager* pQubicleBinaryManager, const char* objectFileName, float scale);
	~Projectile();

	void SetLightingManager(LightingManager* pLightingManager);
	void SetGameWindow(GameWindow* pGameWindow);
	void SetBlockParticleManager(BlockParticleManager* pBlockParticleManager);
	void SetPlayer(Player* pPlayer);

	// Unloading
	void UnloadEffectsAndLights() const;

	// Erase flag
	bool GetErase() const;
	void SetErase(bool erase);

	// Setup
	void LoadItem(const char* objectFileName);

	// Accessors / Setters
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition() const;
	void SetVelocity(glm::vec3 vel);
	glm::vec3 GetVelocity() const;
	void SetRotation(float rot);
	float GetRotation() const;
	float GetScale() const;
	float GetRadius() const;
	void UpdateRadius();
	glm::vec3 GetCenter() const;
	glm::vec3 GetForwardVector() const;
	glm::vec3 GetRightVector() const;
	glm::vec3 GetUpVector() const;
	void SetGravityDirection(glm::vec3 dir);
	void SetGravityMultiplier(float multiplier);
	void SetProjectileCurveParams(glm::vec3 forward, glm::vec3 target, float curveTime);
	void SetWorldCollisionEnabled(bool enabled);

	// Projectile type
	void SetProjectileType(bool attackEnemies, bool attackPlayer, bool attackNPCs);
	bool CanAttackEnemies() const;
	bool CanAttackPlayer() const;
	bool CanAttackNPCs() const;
	void SetReturnToPlayer(bool returnToPlayer);
	bool IsReturnToPlayer() const;
	void SetExplodingProjectile(bool exploding, float radius);

	void SetOwner(Player* pPlayer, NPC* pNPC, Enemy* pEnemy);
	Player* GetPlayerOwner() const;
	NPC* GetNPCOwner() const;
	Enemy* GetEnemyOwner() const;

	// Catching
	bool CanCatch() const;
	void PlayerCatch();

	// Explode
	void Explode();

	// Grid
	void UpdateGridPosition();
	Chunk* GetCachedGridChunkOrFromPosition(glm::vec3 pos) const;

	// Rendering Helpers
	void CalculateWorldTransformMatrix();

	// Updating
	void Update(float dt);
	void UpdateProjectileLights() const;
	void UpdateProjectileParticleEffects() const;

	// Rendering
	void Render() const;
	void RenderWeaponTrails() const;
	void RenderDebug() const;
	void RenderDebugReturnCurve() const;
	void RenderForwardDebug() const;

protected:
	static void _RightCurveTimerFinished(void* pData);
	void RightCurveTimerFinished();

private:
	Renderer* m_pRenderer;
	ChunkManager* m_pChunkManager;
	LightingManager* m_pLightingManager;
	BlockParticleManager* m_pBlockParticleManager;
	QubicleBinaryManager* m_pQubicleBinaryManager;
	GameWindow* m_pGameWindow;
	Player* m_pPlayer;

	// Erase flag
	bool m_isErase;

	// Projectile radius
	float m_radius;

	// Scale
	float m_renderScale;

	// Projectile position and movement variables
	glm::vec3 m_position;
	glm::vec3 m_previousPosition;
	glm::vec3 m_velocity;
	glm::vec3 m_gravityDirection;
	float m_gravityMultiplier;

	// Forward vector
	glm::vec3 m_forward;

	// World matrix
	Matrix4 m_worldMatrix;

	// Grid position
	int m_gridPositionX;
	int m_gridPositionY;
	int m_gridPositionZ;

	// Cached grid chunk
	Chunk* m_pCachedGridChunk;

	// What type of projectile are we
	bool m_attackEnemies;
	bool m_attackPlayer;
	bool m_attackNPCs;

	// World collision flag
	bool m_worldCollisionEnabled;

	// Explosion
	bool m_explodeWorld;
	float m_explosionRadius;

	// Return to player flag
	bool m_returnToPlayer;
	float m_catchTimer;
	float m_curveTimer;
	float m_curveTime;
	bool m_rightCurve;
	bool m_returningDirectToPlayer;

	// Return to player curve
	glm::vec3 m_bezierStartRight;
	glm::vec3 m_bezierEndRight;
	glm::vec3 m_bezierControlRight;
	glm::vec3 m_bezierStartLeft;
	glm::vec3 m_bezierEndLeft;
	glm::vec3 m_bezierControlLeft;

	// Projectile owner
	Player* m_pOwnedPlayer;
	NPC* m_pOwnedNPC;
	Enemy* m_pOwnedEnemy;

	// Voxel projectile
	VoxelWeapon* m_pVoxeProjectile;
};

#endif