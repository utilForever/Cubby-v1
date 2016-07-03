/*************************************************************************
> File Name: VoxelWeapon.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Voxel weapon.
> Created Time: 2016/07/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_VOXEL_WEAPON_H
#define CUBBY_VOXEL_WEAPON_H

#include "QubicleBinaryManager.h"

class VoxelObject;

struct VoxelWeaponLight
{
	unsigned int lightID;
	glm::vec3 lightOffset;
	float lightRadius;
	float lightDiffuseMultiplier;
	Color lightColor;
	int connectedToSectionIndex;

	glm::vec3 lightPosition;
};

struct AnimatedSection
{
	std::string fileName;
	VoxelObject* pVoxelObject;
	float renderScale;
	glm::vec3 renderOffset;

	bool autoStart;
	bool playingAnimation;
	bool loopingAnimation;

	// Animated parts
	float translateSpeedX;
	float translateSpeedY;
	float translateSpeedZ;
	float translateRangeXMin;
	float translateRangeXMax;
	float translateRangeYMin;
	float translateRangeYMax;
	float translateRangeZMin;
	float translateRangeZMax;
	float translateSpeedTurnSpeedX;
	float translateSpeedTurnSpeedY;
	float translateSpeedTurnSpeedZ;

	glm::vec3 rotationPoint;
	float rotationSpeedX;
	float rotationSpeedY;
	float rotationSpeedZ;
	float rotationRangeXMin;
	float rotationRangeXMax;
	float rotationRangeYMin;
	float rotationRangeYMax;
	float rotationRangeZMin;
	float rotationRangeZMax;
	float rotationSpeedTurnSpeedX;
	float rotationSpeedTurnSpeedY;
	float rotationSpeedTurnSpeedZ;

	glm::vec3 animatedSectionPosition;

	float translateX;
	float translateY;
	float translateZ;
	float translateMaxSpeedX;
	float translateMaxSpeedY;
	float translateMaxSpeedZ;
	bool translateXUp;
	bool translateXDown;
	bool translateYUp;
	bool translateYDown;
	bool translateZUp;
	bool translateZDown;

	float rotationX;
	float rotationY;
	float rotationZ;
	float rotationMaxSpeedX;
	float rotationMaxSpeedY;
	float rotationMaxSpeedZ;
	bool rotationXUp;
	bool rotationXDown;
	bool rotationYUp;
	bool rotationYDown;
	bool rotationZUp;
	bool rotationZDown;
};

struct ParticleEffect
{
	unsigned int particleEffectID;
	std::string fileName;
	glm::vec3 positionOffset;
	int connectedToSectionIndex;

	glm::vec3 particleEffectPosition;
};

struct WeaponTrailPoint
{
	glm::vec3 startPoint;
	glm::vec3 endPoint;
	bool pointActive;
	float animaionTime;
};

struct WeaponTrail
{
	float trailTime;
	glm::vec3 startOffsetPoint;
	glm::vec3 endOffsetPoint;
	Color trailColor;
	bool followOrigin;

	int numTrailPoints;
	int trailNextAddIndex;
	WeaponTrailPoint* pTrailPoints;
	Matrix4 origin;
	float parentScale;
};

class VoxelWeapon
{
public:
	// Constructor, Destructor
	VoxelWeapon(Renderer* pRenderer, QubicleBinaryManager* pQubicleBinaryManager);
	~VoxelWeapon();

	void Reset();

	bool IsLoaded();

	// Rebuild
	void RebuildVoxelModel(bool faceMerge);

	void LoadWeapon(const char* weaponFileName, bool useManager = true);
	void SaveWeapon(const char* weaponFileName);
	void UnloadWeapon();

	void SetVoxelCharacterParent(VoxelCharacter* pParentCharacter);
	void SetBoneAttachment(const char* boneName);

	void SetRenderOffset(glm::vec3 offset);
	glm::vec3 GetRenderOffset();

	void SetRenderScale(float scale);
	float GetRenderScale();

	glm::vec3 GetCenter();

	// Subsection animations
	void StartSubSectionAnimation();
	void StopSubSectionAnimation();
	bool HasSubSectionAnimationFinished(int index);

	// Weapon trails
	void StartWeaponTrails();
	void StopWeaponTrails();
	bool IsWeaponTrailsActive();

	// Lighting
	int GetNumLights();
	void SetLightingID(int lightIndex, unsigned int lightID);
	void GetLightParams(int lightIndex, unsigned int* lightID, glm::vec3* position, float* radius, float* diffuseMultiplier, Color* color, bool* connectedToSegment);

	// Particle effects
	int GetNumParticleEffects();
	void SetParticleEffectID(int particleEffectIndex, unsigned int particleEffectID);
	void GetParticleEffectParams(int particleEffectIndex, unsigned int* particleEffectID, glm::vec3* position, std::string* name, bool* connectedToSegment);

	// Animated sections
	int GetNumAimatedSections();
	AnimatedSection* GetAnimatedSection(int index);

	// Gameplay params
	float GetWeaponRadius();
	void SetFirstPersonMode(bool firstPerson);

	// Camera settings
	void SetCameraYRotation(float yRot);

	// Rendering modes
	void SetWireFrameRender(bool wireframe);
	void SetMeshAlpha(float alpha);
	void SetMeshSingleColor(float r, float g, float b);

	// Updating
	void SetWeaponTrailsParams(Matrix4 originMatrix, float scale);
	void CreateWeaponTrailPoint();
	void Update(float dt);

	// Rendering
	void Render(bool renderOutline, bool reflection, bool silhouette, Color outlineColor);
	void RenderPaperdoll();
	void RenderWeaponTrails();

private:
	Renderer* m_pRenderer;
	QubicleBinaryManager* m_pQubicleBinaryManager;

	// Loaded flag
	bool m_isLoaded;

	// Parent character we are connected to
	VoxelCharacter* m_pParentCharacter;

	// Bone index for attachment to character
	int m_boneIndex;

	// Matrix information for attachment to character
	std::string m_matrixName;
	int m_matrixIndex;

	// Render offset from center
	glm::vec3 m_renderOffset;

	// Weapon scale
	float m_renderScale;

	// Animated sections
	int m_numAnimatedSections;
	AnimatedSection* m_pAnimatedSections;

	// Lighting on weapons
	int m_numLights;
	VoxelWeaponLight* m_pLights;

	// Particle effects
	int m_numParticleEffects;
	ParticleEffect* m_pParticleEffects;

	// Weapon trails
	int m_numWeaponTrails;
	WeaponTrail* m_pWeaponTrails;
	bool m_isWeaponTrailsStarted;

	// Gameplay params
	float m_weaponRadius;
	bool m_isFirstPersonMode;

	// Camera variables
	float m_cameraYRotation;
};

#endif