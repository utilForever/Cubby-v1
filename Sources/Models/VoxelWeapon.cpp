/*************************************************************************
> File Name: VoxelWeapon.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Voxel weapon.
> Created Time: 2016/07/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>

#include <Maths/3DMaths.h>

#include "VoxelObject.h"
#include "VoxelWeapon.h"
#include "VoxelCharacter.h"

// Constructor, Destructor
VoxelWeapon::VoxelWeapon(Renderer* pRenderer, QubicleBinaryManager* pQubicleBinaryManager) :
	m_pRenderer(pRenderer), m_pQubicleBinaryManager(pQubicleBinaryManager)
{
	Reset();

	m_numLights = 0;
	m_pLights = nullptr;

	m_numAnimatedSections = 0;
	m_pAnimatedSections = nullptr;

	m_numParticleEffects = 0;
	m_pParticleEffects = nullptr;

	m_numWeaponTrails = 0;
	m_pWeaponTrails = nullptr;
}

VoxelWeapon::~VoxelWeapon()
{
	UnloadWeapon();
	Reset();
}

void VoxelWeapon::Reset()
{
	m_pParentCharacter = nullptr;

	m_boneIndex = -1;

	m_matrixName = "";
	m_matrixIndex = -1;

	m_renderOffset = glm::vec3(0.0f, 0.0f, 0.0f);

	m_renderScale = 1.0f;

	m_numLights = 0;
	m_numAnimatedSections = 0;
	m_numParticleEffects = 0;

	m_isWeaponTrailsStarted = false;

	m_weaponRadius = 1.0f;
	m_isFirstPersonMode = false;

	m_cameraYRotation = 0.0f;

	m_isLoaded = false;
}

bool VoxelWeapon::IsLoaded() const
{
	return m_isLoaded;
}

// Rebuild
void VoxelWeapon::RebuildVoxelModel(bool faceMerge) const
{
	for (int i = 0; i < m_numAnimatedSections; ++i)
	{
		m_pAnimatedSections[i].pVoxelObject->RebuildVoxelModel(faceMerge);
	}
}

void VoxelWeapon::LoadWeapon(const char* weaponFileName, bool useManager)
{
	std::ifstream file;

	// Open the file
	file.open(weaponFileName, std::ios::in);

	if (file.is_open())
	{
		std::string tempString;

		file >> tempString >> m_renderOffset.x >> m_renderOffset.y >> m_renderOffset.z;

		file >> tempString >> m_renderScale;

		// Animated sections
		m_numAnimatedSections = 0;
		file >> tempString >> m_numAnimatedSections;

		if (m_numAnimatedSections > 0)
		{
			m_pAnimatedSections = new AnimatedSection[m_numAnimatedSections];
		}

		for (int i = 0; i < m_numAnimatedSections; ++i)
		{
			m_pAnimatedSections[i].pVoxelObject = new VoxelObject();
			m_pAnimatedSections[i].pVoxelObject->SetRenderer(m_pRenderer);
			m_pAnimatedSections[i].pVoxelObject->SetQubicleBinaryManager(m_pQubicleBinaryManager);

			file >> tempString >> m_pAnimatedSections[i].fileName;
			m_pAnimatedSections[i].pVoxelObject->LoadObject(m_pAnimatedSections[i].fileName.c_str(), useManager);

			file >> tempString >> m_pAnimatedSections[i].renderScale;

			float offsetX = 0.0f;
			float offsetY = 0.0f;
			float offsetZ = 0.0f;
			file >> tempString >> offsetX >> offsetY >> offsetZ;
			m_pAnimatedSections[i].renderOffset = glm::vec3(offsetX, offsetY, offsetZ);

			file >> tempString >> m_pAnimatedSections[i].autoStart;
			file >> tempString >> m_pAnimatedSections[i].loopingAnimation;
			m_pAnimatedSections[i].playingAnimation = m_pAnimatedSections[i].autoStart;

			// Translation
			file >> tempString >> m_pAnimatedSections[i].translateSpeedX;
			file >> tempString >> m_pAnimatedSections[i].translateSpeedY;
			file >> tempString >> m_pAnimatedSections[i].translateSpeedZ;

			file >> tempString >> m_pAnimatedSections[i].translateRangeXMin >> m_pAnimatedSections[i].translateRangeXMax;
			file >> tempString >> m_pAnimatedSections[i].translateRangeYMin >> m_pAnimatedSections[i].translateRangeYMax;
			file >> tempString >> m_pAnimatedSections[i].translateRangeZMin >> m_pAnimatedSections[i].translateRangeZMax;

			file >> tempString >> m_pAnimatedSections[i].translateSpeedTurnSpeedX;
			file >> tempString >> m_pAnimatedSections[i].translateSpeedTurnSpeedY;
			file >> tempString >> m_pAnimatedSections[i].translateSpeedTurnSpeedZ;

			m_pAnimatedSections[i].translateX = 0.0f;
			m_pAnimatedSections[i].translateY = 0.0f;
			m_pAnimatedSections[i].translateZ = 0.0f;

			m_pAnimatedSections[i].translateMaxSpeedX = m_pAnimatedSections[i].translateSpeedX;
			m_pAnimatedSections[i].translateMaxSpeedY = m_pAnimatedSections[i].translateSpeedY;
			m_pAnimatedSections[i].translateMaxSpeedZ = m_pAnimatedSections[i].translateSpeedZ;
			m_pAnimatedSections[i].translateXUp = true;
			m_pAnimatedSections[i].translateXDown = false;
			m_pAnimatedSections[i].translateYUp = true;
			m_pAnimatedSections[i].translateYDown = false;
			m_pAnimatedSections[i].translateZUp = true;
			m_pAnimatedSections[i].translateZDown = false;

			// Rotation
			float rotationPointX = 0.0f;
			float rotationPointY = 0.0f;
			float rotationPointZ = 0.0f;
			file >> tempString >> rotationPointX >> rotationPointY >> rotationPointZ;
			m_pAnimatedSections[i].rotationPoint = glm::vec3(rotationPointX, rotationPointY, rotationPointZ);

			file >> tempString >> m_pAnimatedSections[i].rotationSpeedX;
			file >> tempString >> m_pAnimatedSections[i].rotationSpeedY;
			file >> tempString >> m_pAnimatedSections[i].rotationSpeedZ;

			file >> tempString >> m_pAnimatedSections[i].rotationRangeXMin >> m_pAnimatedSections[i].rotationRangeXMax;
			file >> tempString >> m_pAnimatedSections[i].rotationRangeYMin >> m_pAnimatedSections[i].rotationRangeYMax;
			file >> tempString >> m_pAnimatedSections[i].rotationRangeZMin >> m_pAnimatedSections[i].rotationRangeZMax;

			file >> tempString >> m_pAnimatedSections[i].rotationSpeedTurnSpeedX;
			file >> tempString >> m_pAnimatedSections[i].rotationSpeedTurnSpeedY;
			file >> tempString >> m_pAnimatedSections[i].rotationSpeedTurnSpeedZ;

			m_pAnimatedSections[i].rotationX = 0.0f;
			m_pAnimatedSections[i].rotationY = 0.0f;
			m_pAnimatedSections[i].rotationZ = 0.0f;

			m_pAnimatedSections[i].rotationMaxSpeedX = m_pAnimatedSections[i].rotationSpeedX;
			m_pAnimatedSections[i].rotationMaxSpeedY = m_pAnimatedSections[i].rotationSpeedY;
			m_pAnimatedSections[i].rotationMaxSpeedZ = m_pAnimatedSections[i].rotationSpeedZ;
			m_pAnimatedSections[i].rotationXUp = true;
			m_pAnimatedSections[i].rotationXDown = false;
			m_pAnimatedSections[i].rotationYUp = true;
			m_pAnimatedSections[i].rotationYDown = false;
			m_pAnimatedSections[i].rotationZUp = true;
			m_pAnimatedSections[i].rotationZDown = false;
		}

		// Dynamic lights
		m_numLights = 0;
		file >> tempString >> m_numLights;

		if (m_numLights > 0)
		{
			m_pLights = new VoxelWeaponLight[m_numLights];
		}

		for (int i = 0; i < m_numLights; ++i)
		{
			m_pLights[i].lightID = -1;

			float offsetX = 0.0f;
			float offsetY = 0.0f;
			float offsetZ = 0.0f;
			file >> tempString >> offsetX >> offsetY >> offsetZ;
			m_pLights[i].lightOffset = glm::vec3(offsetX, offsetY, offsetZ);

			file >> tempString >> m_pLights[i].lightRadius;

			file >> tempString >> m_pLights[i].lightDiffuseMultiplier;

			float r = 1.0f;
			float g = 1.0f;
			float b = 1.0f;
			float a = 1.0f;
			file >> tempString >> r >> g >> b >> a;
			m_pLights[i].lightColor = Color(r, g, b, a);

			file >> tempString >> m_pLights[i].connectedToSectionIndex;
		}

		// Particle effects
		m_numParticleEffects = 0;
		file >> tempString >> m_numParticleEffects;

		if (m_numParticleEffects > 0)
		{
			m_pParticleEffects = new ParticleEffect[m_numParticleEffects];
		}

		for (int i = 0; i < m_numParticleEffects; ++i)
		{
			m_pParticleEffects[i].particleEffectID = -1;

			file >> tempString >> m_pParticleEffects[i].fileName;
			float offsetX = 0.0f;
			float offsetY = 0.0f;
			float offsetZ = 0.0f;
			file >> tempString >> offsetX >> offsetY >> offsetZ;
			m_pParticleEffects[i].positionOffset = glm::vec3(offsetX, offsetY, offsetZ);

			file >> tempString >> m_pParticleEffects[i].connectedToSectionIndex;
		}

		// Weapon trails
		m_numWeaponTrails = 0;
		file >> tempString >> m_numWeaponTrails;

		if (m_numWeaponTrails > 0)
		{
			m_pWeaponTrails = new WeaponTrail[m_numWeaponTrails];
		}

		for (int i = 0; i < m_numWeaponTrails; ++i)
		{
			file >> tempString >> m_pWeaponTrails[i].trailTime;

			float startOffsetX = 0.0f;
			float startOffsetY = 0.0f;
			float startOffsetZ = 0.0f;
			file >> tempString >> startOffsetX >> startOffsetY >> startOffsetZ;
			m_pWeaponTrails[i].startOffsetPoint = glm::vec3(startOffsetX, startOffsetY, startOffsetZ);

			float endOffsetX = 0.0f;
			float endOffsetY = 0.0f;
			float endOffsetZ = 0.0f;
			file >> tempString >> endOffsetX >> endOffsetY >> endOffsetZ;
			m_pWeaponTrails[i].endOffsetPoint = glm::vec3(endOffsetX, endOffsetY, endOffsetZ);

			float r = 1.0f;
			float g = 1.0f;
			float b = 1.0f;
			file >> tempString >> r >> g >> b;
			m_pWeaponTrails[i].trailColor = Color(r, g, b);

			file >> tempString >> m_pWeaponTrails[i].followOrigin;

			m_pWeaponTrails[i].parentScale = 1.0f;
			m_pWeaponTrails[i].numTrailPoints = 50;
			m_pWeaponTrails[i].pTrailPoints = new WeaponTrailPoint[m_pWeaponTrails[i].numTrailPoints];
			m_pWeaponTrails[i].trailNextAddIndex = 0;
			
			for (int point = 0; point < m_pWeaponTrails[i].numTrailPoints; ++point)
			{
				m_pWeaponTrails[i].pTrailPoints[point].pointActive = false;
				m_pWeaponTrails[i].pTrailPoints[point].animaionTime = 0.0f;
			}
		}

		// Gameplay
		file >> tempString >> m_weaponRadius;

		m_isLoaded = true;

		file.close();
	}
}

void VoxelWeapon::SaveWeapon(const char* weaponFileName) const
{
	std::ofstream file;

	// Open the file
	file.open(weaponFileName, std::ios::out);

	if (file.is_open())
	{
		file << "offset: " << m_renderOffset.x << " " << m_renderOffset.y << " " << m_renderOffset.z << "\n";

		file << "scale: " << m_renderScale << "\n\n";

		// Animated sections
		file << "numAnimatedSections: " << m_numAnimatedSections << "\n";
	
		for (int i = 0; i < m_numAnimatedSections; ++i)
		{
			file << "qubicleFile: " << m_pAnimatedSections[i].fileName << "\n";
			file << "renderScale: " << m_pAnimatedSections[i].renderScale << "\n";
			file << "renderOffset: " << m_pAnimatedSections[i].renderOffset.x << " " << m_pAnimatedSections[i].renderOffset.y << " " << m_pAnimatedSections[i].renderOffset.z << " " << "\n";
			file << "autoStartAnimation: " << m_pAnimatedSections[i].autoStart << "\n";
			file << "loopingAnimation: " << m_pAnimatedSections[i].loopingAnimation << "\n";
			file << "translateXSpeed: " << m_pAnimatedSections[i].translateSpeedX << "\n";
			file << "translateYSpeed: " << m_pAnimatedSections[i].translateSpeedY << "\n";
			file << "translateZSpeed: " << m_pAnimatedSections[i].translateSpeedZ << "\n";
			file << "translateXRange: " << m_pAnimatedSections[i].translateRangeXMin << " " << m_pAnimatedSections[i].translateRangeXMax << "\n";
			file << "translateYRange: " << m_pAnimatedSections[i].translateRangeYMin << " " << m_pAnimatedSections[i].translateRangeYMax << "\n";
			file << "translateZRange: " << m_pAnimatedSections[i].translateRangeZMin << " " << m_pAnimatedSections[i].translateRangeZMax << "\n";
			file << "translateXTurnSpeed: " << m_pAnimatedSections[i].translateSpeedTurnSpeedX << "\n";
			file << "translateYTurnSpeed: " << m_pAnimatedSections[i].translateSpeedTurnSpeedY << "\n";
			file << "translateZTurnSpeed: " << m_pAnimatedSections[i].translateSpeedTurnSpeedZ << "\n";
			file << "rotationPoint: " << m_pAnimatedSections[i].rotationPoint.x << " " << m_pAnimatedSections[i].rotationPoint.y << " " << m_pAnimatedSections[i].rotationPoint.z << "\n";
			file << "rotationXSpeed: " << m_pAnimatedSections[i].rotationSpeedX << "\n";
			file << "rotationYSpeed: " << m_pAnimatedSections[i].rotationSpeedY << "\n";
			file << "rotationZSpeed: " << m_pAnimatedSections[i].rotationSpeedZ << "\n";
			file << "rotationXRange: " << m_pAnimatedSections[i].rotationRangeXMin << " " << m_pAnimatedSections[i].rotationRangeXMax << "\n";
			file << "rotationYRange: " << m_pAnimatedSections[i].rotationRangeYMin << " " << m_pAnimatedSections[i].rotationRangeYMax << "\n";
			file << "rotationZRange: " << m_pAnimatedSections[i].rotationRangeZMin << " " << m_pAnimatedSections[i].rotationRangeZMax << "\n";
			file << "rotationXTurnSpeed: " << m_pAnimatedSections[i].rotationSpeedTurnSpeedX << "\n";
			file << "rotationYTurnSpeed: " << m_pAnimatedSections[i].rotationSpeedTurnSpeedY << "\n";
			file << "rotationZTurnSpeed: " << m_pAnimatedSections[i].rotationSpeedTurnSpeedZ << "\n";
		}
		file << "\n";

		// Dynamic lights
		file << "numLights: " << m_numLights << "\n";

		for (int i = 0; i < m_numLights; ++i)
		{
			file << "lightOffset: " << m_pLights[i].lightOffset.x << " " << m_pLights[i].lightOffset.y << " " << m_pLights[i].lightOffset.z << " " << "\n";
			file << "lightRadius: " << m_pLights[i].lightRadius << "\n";
			file << "lightDiffuseMultiplier: " << m_pLights[i].lightDiffuseMultiplier << "\n";
			file << "lightColor: " << m_pLights[i].lightColor.GetRed() << " " << m_pLights[i].lightColor.GetGreen() << " " << m_pLights[i].lightColor.GetBlue() << " " << m_pLights[i].lightColor.GetAlpha() << "\n";
			file << "connectedToSection: " << m_pLights[i].connectedToSectionIndex << "\n";
		}
		file << "\n";

		// Particle effects
		file << "numParticleEffects: " << m_numParticleEffects << "\n";

		for (int i = 0; i < m_numParticleEffects; ++i)
		{
			file << "particleEffect: " << m_pParticleEffects[i].fileName << "\n";
			file << "position: " << m_pParticleEffects[i].positionOffset.x << " " << m_pParticleEffects[i].positionOffset.y << " " << m_pParticleEffects[i].positionOffset.z << "\n";
			file << "connectedToSection: " << m_pParticleEffects[i].connectedToSectionIndex << "\n";
		}
		file << "\n";

		// Weapon trails
		file << "numWeaponTrails: " << m_numWeaponTrails << "\n";

		for (int i = 0; i < m_numWeaponTrails; i++)
		{
			file << "trailTime: " << m_pWeaponTrails[i].trailTime << "\n";
			file << "startOffsetPoint: " << m_pWeaponTrails[i].startOffsetPoint.x << " " << m_pWeaponTrails[i].startOffsetPoint.y << " " << m_pWeaponTrails[i].startOffsetPoint.z << "\n";
			file << "endOffsetPoint: " << m_pWeaponTrails[i].endOffsetPoint.x << " " << m_pWeaponTrails[i].endOffsetPoint.y << " " << m_pWeaponTrails[i].endOffsetPoint.z << "\n";
			file << "trailColour: " << m_pWeaponTrails[i].trailColor.GetRed() << " " << m_pWeaponTrails[i].trailColor.GetGreen() << " " << m_pWeaponTrails[i].trailColor.GetBlue() << " " << m_pWeaponTrails[i].trailColor.GetAlpha() << "\n";
			file << "followOrigin: " << m_pWeaponTrails[i].followOrigin << "\n";
		}
		file << "\n";

		// Gameplay
		file << "weaponRadius: " << m_weaponRadius << "\n";

		file.close();
	}
}

void VoxelWeapon::UnloadWeapon()
{
	if (m_numLights > 0)
	{
		delete[] m_pLights;
		m_pLights = nullptr;
		m_numLights = 0;
	}

	if (m_numAnimatedSections > 0)
	{
		delete[] m_pAnimatedSections;
		m_pAnimatedSections = nullptr;
		m_numAnimatedSections = 0;
	}

	if (m_numParticleEffects > 0)
	{
		delete[] m_pParticleEffects;
		m_pParticleEffects = nullptr;
		m_numParticleEffects = 0;
	}

	if (m_numWeaponTrails > 0)
	{
		for (int i = 0; i < m_numWeaponTrails; ++i)
		{
			delete[] m_pWeaponTrails[i].pTrailPoints;
			m_pWeaponTrails[i].pTrailPoints = nullptr;
		}

		delete[] m_pWeaponTrails;
		m_pWeaponTrails = nullptr;
		m_numWeaponTrails = 0;
	}

	Reset();

	m_isLoaded = false;
}

void VoxelWeapon::SetVoxelCharacterParent(VoxelCharacter* pParentCharacter)
{
	m_pParentCharacter = pParentCharacter;
}

void VoxelWeapon::SetBoneAttachment(const char* boneName)
{
	if (m_pParentCharacter != nullptr)
	{
		m_boneIndex = m_pParentCharacter->GetBoneIndex(boneName);

		m_matrixName = boneName;
		m_matrixIndex = m_pParentCharacter->GetMatrixIndexForName(boneName);
	}
}

void VoxelWeapon::SetRenderOffset(glm::vec3 offset)
{
	m_renderOffset = offset;
}

glm::vec3 VoxelWeapon::GetRenderOffset() const
{
	return m_renderOffset;
}

void VoxelWeapon::SetRenderScale(float scale)
{
	m_renderScale = scale;
}

float VoxelWeapon::GetRenderScale() const
{
	return m_renderScale;
}

glm::vec3 VoxelWeapon::GetCenter() const
{
	glm::vec3 centerPos;

	for (int i = 0; i < m_numAnimatedSections; ++i)
	{
		centerPos += m_pAnimatedSections[i].renderOffset * m_pAnimatedSections[i].renderScale;
	}

	centerPos /= static_cast<float>(m_numAnimatedSections);

	return centerPos + (m_renderOffset * m_renderScale);
}

// Subsection animations
void VoxelWeapon::StartSubSectionAnimation() const
{
	for (int i = 0; i < m_numAnimatedSections; ++i)
	{
		m_pAnimatedSections[i].playingAnimation = true;
	}
}

void VoxelWeapon::StopSubSectionAnimation() const
{
	for (int i = 0; i < m_numAnimatedSections; ++i)
	{
		m_pAnimatedSections[i].playingAnimation = false;
	}
}

bool VoxelWeapon::HasSubSectionAnimationFinished(int index) const
{
	return m_pAnimatedSections[index].playingAnimation == false;
}

// Weapon trails
void VoxelWeapon::StartWeaponTrails()
{
	m_isWeaponTrailsStarted = true;

	for (int i = 0; i < m_numWeaponTrails; ++i)
	{
		for (int point = 0; point < m_pWeaponTrails[i].numTrailPoints; ++point)
		{
			m_pWeaponTrails[i].pTrailPoints[point].pointActive = false;
			m_pWeaponTrails[i].pTrailPoints[point].animaionTime = 0.0f;
		}
	}
}

void VoxelWeapon::StopWeaponTrails()
{
	m_isWeaponTrailsStarted = false;
}

bool VoxelWeapon::IsWeaponTrailsActive() const
{
	return m_isWeaponTrailsStarted;
}

// Lighting
int VoxelWeapon::GetNumLights() const
{
	return m_numLights;
}

void VoxelWeapon::SetLightingID(int lightIndex, unsigned int lightID) const
{
	m_pLights[lightIndex].lightID = lightID;
}

void VoxelWeapon::GetLightParams(int lightIndex, unsigned int* lightID, glm::vec3* position, float* radius, float* diffuseMultiplier, Color* color, bool* connectedToSegment) const
{
	*lightID = m_pLights[lightIndex].lightID;
	*radius = m_pLights[lightIndex].lightRadius;
	*diffuseMultiplier = m_pLights[lightIndex].lightDiffuseMultiplier;
	*color = m_pLights[lightIndex].lightColor;
	*position = m_pLights[lightIndex].lightPosition;
	*connectedToSegment = m_pLights[lightIndex].connectedToSectionIndex != -1;

	if (m_pLights[lightIndex].connectedToSectionIndex == -1)
	{
		if (m_pParentCharacter != nullptr)
		{
			*position *= m_pParentCharacter->GetCharacterScale();
		}
	}
}

// Particle effects
int VoxelWeapon::GetNumParticleEffects() const
{
	return m_numParticleEffects;
}

void VoxelWeapon::SetParticleEffectID(int particleEffectIndex, unsigned int particleEffectID) const
{
	m_pParticleEffects[particleEffectIndex].particleEffectID = particleEffectID;
}

void VoxelWeapon::GetParticleEffectParams(int particleEffectIndex, unsigned int* particleEffectID, glm::vec3* position, std::string* name, bool* connectedToSegment) const
{
	*particleEffectID = m_pParticleEffects[particleEffectIndex].particleEffectID;
	*position = m_pParticleEffects[particleEffectIndex].particleEffectPosition;
	*name = m_pParticleEffects[particleEffectIndex].fileName;
	*connectedToSegment = m_pParticleEffects[particleEffectIndex].connectedToSectionIndex != -1;

	if (m_pParticleEffects[particleEffectIndex].connectedToSectionIndex == -1)
	{
		if (m_pParentCharacter != nullptr)
		{
			*position *= m_pParentCharacter->GetCharacterScale();
		}
	}
}

// Animated sections
int VoxelWeapon::GetNumAimatedSections() const
{
	return m_numAnimatedSections;
}

AnimatedSection* VoxelWeapon::GetAnimatedSection(int index) const
{
	return &m_pAnimatedSections[index];
}

// Gameplay params
float VoxelWeapon::GetWeaponRadius() const
{
	return m_weaponRadius;
}

void VoxelWeapon::SetFirstPersonMode(bool isFirstPerson)
{
	m_isFirstPersonMode = isFirstPerson;
}

// Camera settings
void VoxelWeapon::SetCameraYRotation(float yRot)
{
	m_cameraYRotation = yRot;
}

// Rendering modes
void VoxelWeapon::SetWireFrameRender(bool isWireframe) const
{
	for (int i = 0; i < m_numAnimatedSections; ++i)
	{
		m_pAnimatedSections[i].pVoxelObject->SetWireFrameRender(isWireframe);
	}
}

void VoxelWeapon::SetMeshAlpha(float alpha) const
{
	for (int i = 0; i < m_numAnimatedSections; ++i)
	{
		m_pAnimatedSections[i].pVoxelObject->SetMeshAlpha(alpha);
	}
}

void VoxelWeapon::SetMeshSingleColor(float r, float g, float b) const
{
	for (int i = 0; i < m_numAnimatedSections; ++i)
	{
		m_pAnimatedSections[i].pVoxelObject->SetMeshSingleColor(r, g, b);
	}
}

// Updating
void VoxelWeapon::SetWeaponTrailsParams(Matrix4 originMatrix, float scale) const
{
	if (m_isLoaded == false)
	{
		return;
	}

	for (int i = 0; i < m_numWeaponTrails; ++i)
	{
		m_pWeaponTrails[i].origin = originMatrix;
		m_pWeaponTrails[i].parentScale = scale;
	}
}

void VoxelWeapon::CreateWeaponTrailPoint() const
{
	for (int i = 0; i < m_numWeaponTrails; ++i)
	{
		int index = m_pWeaponTrails[i].trailNextAddIndex;

		if (m_isWeaponTrailsStarted == false)
		{
			m_pWeaponTrails[i].pTrailPoints[index].pointActive = false;
		}
		else
		{
			glm::vec3 startPosition = m_pWeaponTrails[i].startOffsetPoint;
			glm::vec3 endPosition = m_pWeaponTrails[i].endOffsetPoint;

			// Scale to render size
			// Translate for initial block offset
			startPosition += glm::vec3(m_renderOffset.x, m_renderOffset.y, m_renderOffset.z) * m_renderScale;
			endPosition += glm::vec3(m_renderOffset.x, m_renderOffset.y, m_renderOffset.z) * m_renderScale;

			// Rotation due to the weapon facing forwards for hand directions
			if (m_pParentCharacter != nullptr)
			{
				Matrix4 rotationMatrix;
				rotationMatrix.SetRotation(DegreeToRadian(90.0f), 0.0f, 0.0f);
				startPosition = rotationMatrix * startPosition;
				endPosition = rotationMatrix * endPosition;
			}

			if (m_matrixIndex != -1)
			{
				glm::vec3 handBoneOffset = m_pParentCharacter->GetBoneMatrixRenderOffset(m_matrixName.c_str());

				// Translate for external matrix offset value
				startPosition += glm::vec3(handBoneOffset.x, handBoneOffset.y, handBoneOffset.z);
				endPosition += glm::vec3(handBoneOffset.x, handBoneOffset.y, handBoneOffset.z);
			}

			// Rotation due to 3DS Max export affecting the bone rotations
			if (m_pParentCharacter != nullptr)
			{
				Matrix4 rotationMatrix;
				rotationMatrix.SetRotation(0.0f, 0.0f, DegreeToRadian(-90.0f));
				startPosition = rotationMatrix * startPosition;
				endPosition = rotationMatrix * endPosition;
			}

			// First person mode modifications
			if (m_isFirstPersonMode)
			{
				Matrix4 rotationMatrix;
				float amountX = 0.75f;
				float amountY = 0.75f;
				float amountZ = 0.5f;
				rotationMatrix.SetTranslation(glm::vec3(amountX, amountY, -amountZ));
				rotationMatrix.SetRotation(0.0f, DegreeToRadian(m_cameraYRotation), 0.0f);
				startPosition = rotationMatrix * startPosition;
				endPosition = rotationMatrix * endPosition;
			}

			if (m_pParentCharacter != nullptr)
			{
				if (m_boneIndex != -1)
				{
					AnimationSections animationSection = AnimationSections::FullBody;
					if (m_boneIndex == m_pParentCharacter->GetHeadBoneIndex() ||
						m_boneIndex == m_pParentCharacter->GetBodyBoneIndex())
					{
						animationSection = AnimationSections::HeadBody;
					}
					else if (m_boneIndex == m_pParentCharacter->GetLeftShoulderBoneIndex() ||
						m_boneIndex == m_pParentCharacter->GetLeftHandBoneIndex())
					{
						animationSection = AnimationSections::LeftArmHand;
					}
					else if (m_boneIndex == m_pParentCharacter->GetRightShoulderBoneIndex() ||
						m_boneIndex == m_pParentCharacter->GetRightHandBoneIndex())
					{
						animationSection = AnimationSections::RightArmHand;
					}
					else if (m_boneIndex == m_pParentCharacter->GetLegsBoneIndex() ||
						m_boneIndex == m_pParentCharacter->GetRightFootBoneIndex() ||
						m_boneIndex == m_pParentCharacter->GetLeftFootBoneIndex())
					{
						animationSection = AnimationSections::LegsFeet;
					}

					Matrix4 boneMatrix = m_pParentCharacter->GetBoneMatrix(animationSection, m_boneIndex);

					// Translate by attached bone matrix
					startPosition = boneMatrix * startPosition;
					endPosition = boneMatrix * endPosition;

					// Looking direction, since we are attached to a character who is holding us.
					glm::vec3 vForward = normalize(m_pParentCharacter->GetFaceLookingDirection());
					vForward.y = 0.0f;
					vForward = normalize(vForward);
					glm::vec3 forwardDiff = vForward - glm::vec3(0.0f, 0.0f, 1.0f);
					vForward = normalize(glm::vec3(0.0f, 0.0f, 1.0f) + (forwardDiff * 0.5f));

					glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
					glm::vec3 vRight = normalize(cross(vUp, vForward));
					vUp = normalize(cross(vForward, vRight));

					float matrix[16] =
					{
						vRight.x, vRight.y, vRight.z, 0.0f,
						vUp.x, vUp.y, vUp.z, 0.0f,
						vForward.x, vForward.y, vForward.z, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f
					};
					Matrix4 lookingMat;
					lookingMat.SetValues(matrix);

					startPosition = lookingMat * startPosition;
					endPosition = lookingMat * endPosition;

					// Breathing animation
					float offsetAmount = m_pParentCharacter->GetBreathingAnimationOffsetForBone(m_boneIndex);
					startPosition += glm::vec3(0.0f, offsetAmount, 0.0f);
					endPosition += glm::vec3(0.0f, offsetAmount, 0.0f);
				}
			}

			if (m_pWeaponTrails[i].followOrigin == false)
			{
				startPosition *= m_pWeaponTrails[i].parentScale;
				endPosition *= m_pWeaponTrails[i].parentScale;

				startPosition = m_pWeaponTrails[i].origin * startPosition;
				endPosition = m_pWeaponTrails[i].origin * endPosition;
			}

			m_pWeaponTrails[i].pTrailPoints[index].startPoint = startPosition;
			m_pWeaponTrails[i].pTrailPoints[index].endPoint = endPosition;

			m_pWeaponTrails[i].pTrailPoints[index].pointActive = true;
			m_pWeaponTrails[i].pTrailPoints[index].animaionTime = m_pWeaponTrails[i].trailTime;

			m_pWeaponTrails[i].trailNextAddIndex++;

			if (m_pWeaponTrails[i].trailNextAddIndex == 49)
			{
				m_pWeaponTrails[i].trailNextAddIndex = 0;
			}
		}
	}
}

void VoxelWeapon::Update(float dt) const
{
	if (m_isLoaded == false)
	{
		return;
	}

	if (dt > 0.1f)
	{
		// Temp fix for when dt is large due to a freeze or temp halt, would cause animated sections to get out-of-sync.
		// due to allowing values to go over min/max and then 're-adjust', but since dt was large they would go way over min/max
		return;
	}

	// Update animated sections
	for (int i = 0; i < m_numAnimatedSections; ++i)
	{
		if (m_pAnimatedSections[i].playingAnimation)
		{
			bool noChangeInAnimation = (m_pAnimatedSections[i].translateSpeedX == 0.0f && m_pAnimatedSections[i].translateSpeedY == 0.0f && m_pAnimatedSections[i].translateSpeedZ == 0.0f &&
				m_pAnimatedSections[i].rotationSpeedX == 0.0f && m_pAnimatedSections[i].rotationSpeedY == 0.0f && m_pAnimatedSections[i].rotationSpeedZ == 0.0f);

			// Translation
			if (m_pAnimatedSections[i].translateXUp == true && m_pAnimatedSections[i].translateSpeedX < m_pAnimatedSections[i].translateMaxSpeedX)
			{
				if (m_pAnimatedSections[i].translateSpeedTurnSpeedX != -1)
				{
					m_pAnimatedSections[i].translateSpeedX += m_pAnimatedSections[i].translateSpeedTurnSpeedX * dt;
				}
				else
				{
					m_pAnimatedSections[i].translateX = m_pAnimatedSections[i].translateRangeXMin;
					m_pAnimatedSections[i].translateSpeedX = -m_pAnimatedSections[i].translateSpeedX;
				}
			}
			else if (m_pAnimatedSections[i].translateXDown == true && m_pAnimatedSections[i].translateSpeedX > -m_pAnimatedSections[i].translateMaxSpeedX)
			{
				if (m_pAnimatedSections[i].translateSpeedTurnSpeedX != -1)
				{
					m_pAnimatedSections[i].translateSpeedX -= m_pAnimatedSections[i].translateSpeedTurnSpeedX * dt;
				}
				else
				{
					m_pAnimatedSections[i].translateX = m_pAnimatedSections[i].translateRangeXMax;
					m_pAnimatedSections[i].translateSpeedX = -m_pAnimatedSections[i].translateSpeedX;
				}
			}

			if (m_pAnimatedSections[i].translateYUp == true && m_pAnimatedSections[i].translateSpeedY < m_pAnimatedSections[i].translateMaxSpeedY)
			{
				if (m_pAnimatedSections[i].translateSpeedTurnSpeedY != -1)
				{
					m_pAnimatedSections[i].translateSpeedY += m_pAnimatedSections[i].translateSpeedTurnSpeedY * dt;
				}
				else
				{
					m_pAnimatedSections[i].translateY = m_pAnimatedSections[i].translateRangeYMin;
					m_pAnimatedSections[i].translateSpeedY = -m_pAnimatedSections[i].translateSpeedY;
				}
			}
			else if (m_pAnimatedSections[i].translateYDown == true && m_pAnimatedSections[i].translateSpeedY > -m_pAnimatedSections[i].translateMaxSpeedY)
			{
				if (m_pAnimatedSections[i].translateSpeedTurnSpeedY != -1)
				{
					m_pAnimatedSections[i].translateSpeedY -= m_pAnimatedSections[i].translateSpeedTurnSpeedY * dt;
				}
				else
				{
					m_pAnimatedSections[i].translateY = m_pAnimatedSections[i].translateRangeYMax;
					m_pAnimatedSections[i].translateSpeedY = -m_pAnimatedSections[i].translateSpeedY;
				}
			}

			if (m_pAnimatedSections[i].translateZUp == true && m_pAnimatedSections[i].translateSpeedZ < m_pAnimatedSections[i].translateMaxSpeedZ)
			{
				if (m_pAnimatedSections[i].translateSpeedTurnSpeedZ != -1)
				{
					m_pAnimatedSections[i].translateSpeedZ += m_pAnimatedSections[i].translateSpeedTurnSpeedZ * dt;
				}
				else
				{
					m_pAnimatedSections[i].translateZ = m_pAnimatedSections[i].translateRangeZMin;
					m_pAnimatedSections[i].translateSpeedZ = -m_pAnimatedSections[i].translateSpeedZ;
				}
			}
			else if (m_pAnimatedSections[i].translateZDown == true && m_pAnimatedSections[i].translateSpeedZ > -m_pAnimatedSections[i].translateMaxSpeedZ)
			{
				if (m_pAnimatedSections[i].translateSpeedTurnSpeedZ != -1)
				{
					m_pAnimatedSections[i].translateSpeedZ -= m_pAnimatedSections[i].translateSpeedTurnSpeedZ * dt;
				}
				else
				{
					m_pAnimatedSections[i].translateZ = m_pAnimatedSections[i].translateRangeZMax;
					m_pAnimatedSections[i].translateSpeedZ = -m_pAnimatedSections[i].translateSpeedZ;
				}
			}

			if (m_pAnimatedSections[i].translateX > m_pAnimatedSections[i].translateRangeXMax)
			{
				m_pAnimatedSections[i].translateXUp = false;
				m_pAnimatedSections[i].translateXDown = true;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}
			else if (m_pAnimatedSections[i].translateX < m_pAnimatedSections[i].translateRangeXMin)
			{
				m_pAnimatedSections[i].translateXUp = true;
				m_pAnimatedSections[i].translateXDown = false;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}

			if (m_pAnimatedSections[i].translateY > m_pAnimatedSections[i].translateRangeYMax)
			{
				m_pAnimatedSections[i].translateYUp = false;
				m_pAnimatedSections[i].translateYDown = true;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}
			else if (m_pAnimatedSections[i].translateY < m_pAnimatedSections[i].translateRangeYMin)
			{
				m_pAnimatedSections[i].translateYUp = true;
				m_pAnimatedSections[i].translateYDown = false;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}

			if (m_pAnimatedSections[i].translateZ > m_pAnimatedSections[i].translateRangeZMax)
			{
				m_pAnimatedSections[i].translateZUp = false;
				m_pAnimatedSections[i].translateZDown = true;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}
			else if (m_pAnimatedSections[i].translateZ < m_pAnimatedSections[i].translateRangeZMin)
			{
				m_pAnimatedSections[i].translateZUp = true;
				m_pAnimatedSections[i].translateZDown = false;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}

			m_pAnimatedSections[i].translateX += m_pAnimatedSections[i].translateSpeedX * dt;
			m_pAnimatedSections[i].translateY += m_pAnimatedSections[i].translateSpeedY * dt;
			m_pAnimatedSections[i].translateZ += m_pAnimatedSections[i].translateSpeedZ * dt;

			// Rotation
			if (m_pAnimatedSections[i].rotationXUp == true && m_pAnimatedSections[i].rotationSpeedX < m_pAnimatedSections[i].rotationMaxSpeedX)
			{
				if (m_pAnimatedSections[i].rotationSpeedTurnSpeedX != -1)
				{
					m_pAnimatedSections[i].rotationSpeedX += m_pAnimatedSections[i].rotationSpeedTurnSpeedX * dt;
				}
				else
				{
					m_pAnimatedSections[i].rotationX = m_pAnimatedSections[i].rotationRangeXMin;
					m_pAnimatedSections[i].rotationSpeedX = -m_pAnimatedSections[i].rotationSpeedX;
				}
			}
			else if (m_pAnimatedSections[i].rotationXDown == true && m_pAnimatedSections[i].rotationSpeedX > -m_pAnimatedSections[i].rotationMaxSpeedX)
			{
				if (m_pAnimatedSections[i].rotationSpeedTurnSpeedX != -1)
				{
					m_pAnimatedSections[i].rotationSpeedX -= m_pAnimatedSections[i].rotationSpeedTurnSpeedX * dt;
				}
				else
				{
					m_pAnimatedSections[i].rotationX = m_pAnimatedSections[i].rotationRangeXMax;
					m_pAnimatedSections[i].rotationSpeedX = -m_pAnimatedSections[i].rotationSpeedX;
				}
			}

			if (m_pAnimatedSections[i].rotationYUp == true && m_pAnimatedSections[i].rotationSpeedY < m_pAnimatedSections[i].rotationMaxSpeedY)
			{
				if (m_pAnimatedSections[i].rotationSpeedTurnSpeedY != -1)
				{
					m_pAnimatedSections[i].rotationSpeedY += m_pAnimatedSections[i].rotationSpeedTurnSpeedY * dt;
				}
				else
				{
					m_pAnimatedSections[i].rotationY = m_pAnimatedSections[i].rotationRangeYMin;
					m_pAnimatedSections[i].rotationSpeedY = -m_pAnimatedSections[i].rotationSpeedY;
				}
			}
			else if (m_pAnimatedSections[i].rotationYDown == true && m_pAnimatedSections[i].rotationSpeedY > -m_pAnimatedSections[i].rotationMaxSpeedY)
			{
				if (m_pAnimatedSections[i].rotationSpeedTurnSpeedY != -1)
				{
					m_pAnimatedSections[i].rotationSpeedY -= m_pAnimatedSections[i].rotationSpeedTurnSpeedY * dt;
				}
				else
				{
					m_pAnimatedSections[i].rotationY = m_pAnimatedSections[i].rotationRangeYMax;
					m_pAnimatedSections[i].rotationSpeedY = -m_pAnimatedSections[i].rotationSpeedY;
				}
			}

			if (m_pAnimatedSections[i].rotationZUp == true && m_pAnimatedSections[i].rotationSpeedZ < m_pAnimatedSections[i].rotationMaxSpeedZ)
			{
				if (m_pAnimatedSections[i].rotationSpeedTurnSpeedZ != -1)
				{
					m_pAnimatedSections[i].rotationSpeedZ += m_pAnimatedSections[i].rotationSpeedTurnSpeedZ * dt;
				}
				else
				{
					m_pAnimatedSections[i].rotationZ = m_pAnimatedSections[i].rotationRangeZMin;
					m_pAnimatedSections[i].rotationSpeedZ = -m_pAnimatedSections[i].rotationSpeedZ;
				}
			}
			else if (m_pAnimatedSections[i].rotationZDown == true && m_pAnimatedSections[i].rotationSpeedZ > -m_pAnimatedSections[i].rotationMaxSpeedZ)
			{
				if (m_pAnimatedSections[i].rotationSpeedTurnSpeedZ != -1)
				{
					m_pAnimatedSections[i].rotationSpeedZ -= m_pAnimatedSections[i].rotationSpeedTurnSpeedZ * dt;
				}
				else
				{
					m_pAnimatedSections[i].rotationZ = m_pAnimatedSections[i].rotationRangeZMax;
					m_pAnimatedSections[i].rotationSpeedZ = -m_pAnimatedSections[i].rotationSpeedZ;
				}
			}

			if (m_pAnimatedSections[i].rotationX > m_pAnimatedSections[i].rotationRangeXMax)
			{
				m_pAnimatedSections[i].rotationXUp = false;
				m_pAnimatedSections[i].rotationXDown = true;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}
			else if (m_pAnimatedSections[i].rotationX < m_pAnimatedSections[i].rotationRangeXMin)
			{
				m_pAnimatedSections[i].rotationXUp = true;
				m_pAnimatedSections[i].rotationXDown = false;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}

			if (m_pAnimatedSections[i].rotationY > m_pAnimatedSections[i].rotationRangeYMax)
			{
				m_pAnimatedSections[i].rotationYUp = false;
				m_pAnimatedSections[i].rotationYDown = true;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}
			else if (m_pAnimatedSections[i].rotationY < m_pAnimatedSections[i].rotationRangeYMin)
			{
				m_pAnimatedSections[i].rotationYUp = true;
				m_pAnimatedSections[i].rotationYDown = false;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}

			if (m_pAnimatedSections[i].rotationZ > m_pAnimatedSections[i].rotationRangeZMax)
			{
				m_pAnimatedSections[i].rotationZUp = false;
				m_pAnimatedSections[i].rotationZDown = true;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}
			else if (m_pAnimatedSections[i].rotationZ < m_pAnimatedSections[i].rotationRangeZMin)
			{
				m_pAnimatedSections[i].rotationZUp = true;
				m_pAnimatedSections[i].rotationZDown = false;

				if (m_pAnimatedSections[i].loopingAnimation == false)
				{
					m_pAnimatedSections[i].playingAnimation = false;
				}
			}

			m_pAnimatedSections[i].rotationX += m_pAnimatedSections[i].rotationSpeedX * dt;
			m_pAnimatedSections[i].rotationY += m_pAnimatedSections[i].rotationSpeedY * dt;
			m_pAnimatedSections[i].rotationZ += m_pAnimatedSections[i].rotationSpeedZ * dt;

			if (noChangeInAnimation)
			{
				m_pAnimatedSections[i].playingAnimation = false;
			}
		}
	}

	// Update dynamic lights
	for (int i = 0; i < m_numLights; ++i)
	{
		if (m_pLights[i].connectedToSectionIndex != -1)
		{
			// If we are attached to an animated section, just copy it's position
			m_pLights[i].lightPosition = m_pAnimatedSections[m_pLights[i].connectedToSectionIndex].animatedSectionPosition;

			// Scale to render size
			// Translate for initial block offset
			glm::vec3 lightPosition = m_pLights[i].lightOffset;

			// Rotation due to the weapon facing forwards for hand directions
			if (m_pParentCharacter != nullptr)
			{
				Matrix4 rotationMatrix;
				rotationMatrix.SetRotation(DegreeToRadian(90.0f), 0.0f, 0.0f);
				lightPosition = rotationMatrix * lightPosition;

				lightPosition *= m_pParentCharacter->GetCharacterScale();
			}

			m_pLights[i].lightPosition += lightPosition;

			continue;
		}

		// Get the weapon position
		glm::vec3 lightPosition = m_pLights[i].lightOffset;

		// Scale to render size
		// Translate for initial block offset
		lightPosition += glm::vec3(m_renderOffset.x, m_renderOffset.y, m_renderOffset.z) * m_renderScale;

		// Rotation due to the weapon facing forwards for hand directions
		if (m_pParentCharacter != nullptr)
		{
			Matrix4 rotationMatrix;
			rotationMatrix.SetRotation(DegreeToRadian(90.0f), 0.0f, 0.0f);
			lightPosition = rotationMatrix * lightPosition;
		}

		if (m_matrixIndex != -1)
		{
			glm::vec3 handBoneOffset = m_pParentCharacter->GetBoneMatrixRenderOffset(m_matrixName.c_str());

			// Translate for external matrix offset value
			lightPosition += glm::vec3(handBoneOffset.x, handBoneOffset.y, handBoneOffset.z);
		}

		// Rotation due to 3DS Max export affecting the bone rotations
		if (m_pParentCharacter != nullptr)
		{
			Matrix4 rotationMatrix;
			rotationMatrix.SetRotation(0.0f, 0.0f, DegreeToRadian(-90.0f));
			lightPosition = rotationMatrix * lightPosition;
		}

		// First person mode modifications
		if (m_isFirstPersonMode)
		{
			Matrix4 rotationMatrix;
			float amountX = 0.75f;
			float amountY = 0.75f;
			float amountZ = 0.5f;
			rotationMatrix.SetTranslation(glm::vec3(amountX, amountY, -amountZ));
			rotationMatrix.SetRotation(0.0f, DegreeToRadian(m_cameraYRotation), 0.0f);
			lightPosition = rotationMatrix * lightPosition;
		}

		if (m_pParentCharacter != nullptr)
		{
			if (m_boneIndex != -1)
			{
				AnimationSections animationSection = AnimationSections::FullBody;
				if (m_boneIndex == m_pParentCharacter->GetHeadBoneIndex() ||
					m_boneIndex == m_pParentCharacter->GetBodyBoneIndex())
				{
					animationSection = AnimationSections::HeadBody;
				}
				else if (m_boneIndex == m_pParentCharacter->GetLeftShoulderBoneIndex() ||
					m_boneIndex == m_pParentCharacter->GetLeftHandBoneIndex())
				{
					animationSection = AnimationSections::LeftArmHand;
				}
				else if (m_boneIndex == m_pParentCharacter->GetRightShoulderBoneIndex() ||
					m_boneIndex == m_pParentCharacter->GetRightHandBoneIndex())
				{
					animationSection = AnimationSections::RightArmHand;
				}
				else if (m_boneIndex == m_pParentCharacter->GetLegsBoneIndex() ||
					m_boneIndex == m_pParentCharacter->GetRightFootBoneIndex() ||
					m_boneIndex == m_pParentCharacter->GetLeftFootBoneIndex())
				{
					animationSection = AnimationSections::LegsFeet;
				}

				Matrix4 boneMatrix = m_pParentCharacter->GetBoneMatrix(animationSection, m_boneIndex);

				// Translate by attached bone matrix
				lightPosition = boneMatrix * lightPosition;

				// Looking direction, since we are attached to a character who is holding us.
				glm::vec3 vForward = normalize(m_pParentCharacter->GetFaceLookingDirection());
				vForward.y = 0.0f;
				vForward = normalize(vForward);
				glm::vec3 forwardDiff = vForward - glm::vec3(0.0f, 0.0f, 1.0f);
				vForward = normalize(glm::vec3(0.0f, 0.0f, 1.0f) + (forwardDiff * 0.5f));

				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);

				lightPosition = lookingMat * lightPosition;

				// Breathing animation
				float offsetAmount = m_pParentCharacter->GetBreathingAnimationOffsetForBone(m_boneIndex);
				lightPosition += glm::vec3(0.0f, offsetAmount, 0.0f);
			}
		}

		m_pLights[i].lightPosition = lightPosition;
	}

	// Update particle effects
	for (int i = 0; i < m_numParticleEffects; ++i)
	{
		if (m_pParticleEffects[i].connectedToSectionIndex != -1)
		{
			// If we are attached to an animated section, just copy it's position
			m_pParticleEffects[i].particleEffectPosition = m_pAnimatedSections[m_pParticleEffects[i].connectedToSectionIndex].animatedSectionPosition;

			// Scale to render size
			// Translate for initial block offset
			glm::vec3 particleEffectPosition = m_pParticleEffects[i].positionOffset;

			// Rotation due to the weapon facing forwards for hand directions
			if (m_pParentCharacter != nullptr)
			{
				Matrix4 rotationMatrix;
				rotationMatrix.SetRotation(DegreeToRadian(90.0f), 0.0f, 0.0f);
				particleEffectPosition = rotationMatrix * particleEffectPosition;

				particleEffectPosition *= m_pParentCharacter->GetCharacterScale();
			}

			m_pParticleEffects[i].particleEffectPosition += particleEffectPosition;

			continue;
		}

		glm::vec3 particleEffectPosition = m_pParticleEffects[i].positionOffset;

		// Scale to render size
		// Translate for initial block offset
		particleEffectPosition += glm::vec3(m_renderOffset.x, m_renderOffset.y, m_renderOffset.z) * m_renderScale;

		// Rotation due to the weapon facing forwards for hand directions
		if (m_pParentCharacter != nullptr)
		{
			Matrix4 rotationMatrix;
			rotationMatrix.SetRotation(DegreeToRadian(90.0f), 0.0f, 0.0f);
			particleEffectPosition = rotationMatrix * particleEffectPosition;
		}

		if (m_matrixIndex != -1)
		{
			glm::vec3 handBoneOffset = m_pParentCharacter->GetBoneMatrixRenderOffset(m_matrixName.c_str());

			// Translate for external matrix offset value
			particleEffectPosition += glm::vec3(handBoneOffset.x, handBoneOffset.y, handBoneOffset.z);
		}

		// Rotation due to 3DS Max export affecting the bone rotations
		if (m_pParentCharacter != nullptr)
		{
			Matrix4 rotationMatrix;
			rotationMatrix.SetRotation(0.0f, 0.0f, DegreeToRadian(-90.0f));
			particleEffectPosition = rotationMatrix * particleEffectPosition;
		}

		// First person mode modifications
		if (m_isFirstPersonMode)
		{
			Matrix4 rotationMatrix;
			float amountX = 0.75f;
			float amountY = 0.75f;
			float amountZ = 0.5f;
			rotationMatrix.SetTranslation(glm::vec3(amountX, amountY, -amountZ));
			rotationMatrix.SetRotation(0.0f, DegreeToRadian(m_cameraYRotation), 0.0f);
			particleEffectPosition = rotationMatrix * particleEffectPosition;
		}

		if (m_pParentCharacter != nullptr)
		{
			if (m_boneIndex != -1)
			{
				AnimationSections animationSection = AnimationSections::FullBody;
				if (m_boneIndex == m_pParentCharacter->GetHeadBoneIndex() ||
					m_boneIndex == m_pParentCharacter->GetBodyBoneIndex())
				{
					animationSection = AnimationSections::HeadBody;
				}
				else if (m_boneIndex == m_pParentCharacter->GetLeftShoulderBoneIndex() ||
					m_boneIndex == m_pParentCharacter->GetLeftHandBoneIndex())
				{
					animationSection = AnimationSections::LeftArmHand;
				}
				else if (m_boneIndex == m_pParentCharacter->GetRightShoulderBoneIndex() ||
					m_boneIndex == m_pParentCharacter->GetRightHandBoneIndex())
				{
					animationSection = AnimationSections::RightArmHand;
				}
				else if (m_boneIndex == m_pParentCharacter->GetLegsBoneIndex() ||
					m_boneIndex == m_pParentCharacter->GetRightFootBoneIndex() ||
					m_boneIndex == m_pParentCharacter->GetLeftFootBoneIndex())
				{
					animationSection = AnimationSections::LegsFeet;
				}

				Matrix4 boneMatrix = m_pParentCharacter->GetBoneMatrix(animationSection, m_boneIndex);

				// Translate by attached bone matrix
				particleEffectPosition = boneMatrix * particleEffectPosition;

				// Looking direction, since we are attached to a character who is holding us.
				glm::vec3 vForward = normalize(m_pParentCharacter->GetFaceLookingDirection());
				vForward.y = 0.0f;
				vForward = normalize(vForward);
				glm::vec3 forwardDiff = vForward - glm::vec3(0.0f, 0.0f, 1.0f);
				vForward = normalize(glm::vec3(0.0f, 0.0f, 1.0f) + (forwardDiff * 0.5f));

				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);

				particleEffectPosition = lookingMat * particleEffectPosition;

				// Breathing animation
				float offsetAmount = m_pParentCharacter->GetBreathingAnimationOffsetForBone(m_boneIndex);
				particleEffectPosition += glm::vec3(0.0f, offsetAmount, 0.0f);
			}
		}

		m_pParticleEffects[i].particleEffectPosition = particleEffectPosition;
	}

	// Update weapon trails timers;
	for (int i = 0; i < m_numWeaponTrails; ++i)
	{
		for (int point = 0; point < m_pWeaponTrails[i].numTrailPoints; ++point)
		{
			m_pWeaponTrails[i].pTrailPoints[point].animaionTime -= dt;

			if (m_pWeaponTrails[i].pTrailPoints[point].animaionTime <= 0.0f)
			{
				m_pWeaponTrails[i].pTrailPoints[point].pointActive = false;
			}
		}
	}
}

// Rendering
void VoxelWeapon::Render(bool renderOutline, bool reflection, bool silhouette, Color outlineColor) const
{
	m_pRenderer->PushMatrix();

	if (m_pParentCharacter != nullptr)
	{
		if (m_boneIndex != -1)
		{
			AnimationSections animationSection = AnimationSections::FullBody;
			if (m_boneIndex == m_pParentCharacter->GetHeadBoneIndex() ||
				m_boneIndex == m_pParentCharacter->GetBodyBoneIndex())
			{
				animationSection = AnimationSections::HeadBody;
			}
			else if (m_boneIndex == m_pParentCharacter->GetLeftShoulderBoneIndex() ||
				m_boneIndex == m_pParentCharacter->GetLeftHandBoneIndex())
			{
				animationSection = AnimationSections::LeftArmHand;
			}
			else if (m_boneIndex == m_pParentCharacter->GetRightShoulderBoneIndex() ||
				m_boneIndex == m_pParentCharacter->GetRightHandBoneIndex())
			{
				animationSection = AnimationSections::RightArmHand;
			}
			else if (m_boneIndex == m_pParentCharacter->GetLegsBoneIndex() ||
				m_boneIndex == m_pParentCharacter->GetRightFootBoneIndex() ||
				m_boneIndex == m_pParentCharacter->GetLeftFootBoneIndex())
			{
				animationSection = AnimationSections::LegsFeet;
			}

			Matrix4 boneMatrix = m_pParentCharacter->GetBoneMatrix(animationSection, m_boneIndex);

			// Breathing animation
			float offsetAmount = m_pParentCharacter->GetBreathingAnimationOffsetForBone(m_boneIndex);
			m_pRenderer->TranslateWorldMatrix(0.0f, offsetAmount, 0.0f);

			// Body and hands/shoulders looking direction
			if (m_boneIndex == m_pParentCharacter->GetLeftHandBoneIndex() ||
				m_boneIndex == m_pParentCharacter->GetRightHandBoneIndex())
			{
				glm::vec3 vForward = normalize(m_pParentCharacter->GetFaceLookingDirection());
				vForward.y = 0.0f;
				vForward = normalize(vForward);
				glm::vec3 forwardDiff = vForward - glm::vec3(0.0f, 0.0f, 1.0f);
				vForward = normalize(glm::vec3(0.0f, 0.0f, 1.0f) + (forwardDiff * 0.5f));

				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);
				m_pRenderer->MultiplyWorldMatrix(lookingMat);
			}

			// Translate by attached bone matrix
			m_pRenderer->MultiplyWorldMatrix(boneMatrix);

			// Rotation due to 3dsmax export affecting the bone rotations
			m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f);
		}
	}

	if (m_matrixIndex != -1)
	{
		glm::vec3 handBoneOffset = m_pParentCharacter->GetBoneMatrixRenderOffset(m_matrixName.c_str());

		// Translate for external matrix offset value
		m_pRenderer->TranslateWorldMatrix(handBoneOffset.x, handBoneOffset.y, handBoneOffset.z);

		// Rotation due to the weapon facing forwards for hand directions
		m_pRenderer->RotateWorldMatrix(90.0f, 0.0f, 0.0f);
	}

	// Scale to render size
	m_pRenderer->ScaleWorldMatrix(m_renderScale, m_renderScale, m_renderScale);

	// Translate for initial block offset
	m_pRenderer->TranslateWorldMatrix(m_renderOffset.x, m_renderOffset.y, m_renderOffset.z);

	// Render all animated sections
	for (int i = 0; i < m_numAnimatedSections; ++i)
	{
		m_pRenderer->PushMatrix();

		// Scale to render size
		m_pRenderer->ScaleWorldMatrix(m_pAnimatedSections[i].renderScale, m_pAnimatedSections[i].renderScale, m_pAnimatedSections[i].renderScale);

		// Translate for initial block offset
		m_pRenderer->TranslateWorldMatrix(m_pAnimatedSections[i].renderOffset.x, m_pAnimatedSections[i].renderOffset.y, m_pAnimatedSections[i].renderOffset.z);

		// Animated sections
		m_pRenderer->TranslateWorldMatrix(-m_pAnimatedSections[i].rotationPoint.x, -m_pAnimatedSections[i].rotationPoint.y, -m_pAnimatedSections[i].rotationPoint.z);
		m_pRenderer->RotateWorldMatrix(m_pAnimatedSections[i].rotationX, m_pAnimatedSections[i].rotationY, m_pAnimatedSections[i].rotationZ);
		m_pRenderer->TranslateWorldMatrix(m_pAnimatedSections[i].rotationPoint.x, m_pAnimatedSections[i].rotationPoint.y, m_pAnimatedSections[i].rotationPoint.z);

		m_pRenderer->TranslateWorldMatrix(m_pAnimatedSections[i].translateX, m_pAnimatedSections[i].translateY, m_pAnimatedSections[i].translateZ);

		m_pAnimatedSections[i].pVoxelObject->Render(renderOutline, reflection, silhouette, outlineColor);

		// Store the animated section position, since light might be attached to it
		if (reflection == false)
		{
			Matrix4 pMat = m_pAnimatedSections[i].pVoxelObject->GetModelMatrix(0);
			m_pAnimatedSections[i].animatedSectionPosition = pMat.GetTranslationVector();
		}

		m_pRenderer->PopMatrix();
	}

	m_pRenderer->PopMatrix();
}

void VoxelWeapon::RenderPaperdoll() const
{
	m_pRenderer->PushMatrix();

	if (m_pParentCharacter != nullptr)
	{
		if (m_boneIndex != -1)
		{
			bool leftSide = (m_boneIndex == m_pParentCharacter->GetLeftHandBoneIndex());
			Matrix4 boneMatrix = m_pParentCharacter->GetBoneMatrixPaperdoll(m_boneIndex, leftSide);

			// Breathing animation
			float offsetAmount = m_pParentCharacter->GetBreathingAnimationOffsetForBone(m_boneIndex);
			m_pRenderer->TranslateWorldMatrix(0.0f, offsetAmount, 0.0f);

			// Body and hands/shoulders looking direction
			if (m_boneIndex == m_pParentCharacter->GetLeftHandBoneIndex() ||
				m_boneIndex == m_pParentCharacter->GetRightHandBoneIndex())
			{
				glm::vec3 vForward = normalize(m_pParentCharacter->GetFaceLookingDirection());
				vForward.y = 0.0f;
				vForward = normalize(vForward);
				glm::vec3 forwardDiff = vForward - glm::vec3(0.0f, 0.0f, 1.0f);
				vForward = normalize(glm::vec3(0.0f, 0.0f, 1.0f) + (forwardDiff * 0.5f));

				glm::vec3 vUp = glm::vec3(0.0f, 1.0f, 0.0f);
				glm::vec3 vRight = normalize(cross(vUp, vForward));
				vUp = normalize(cross(vForward, vRight));

				float matrix[16] =
				{
					vRight.x, vRight.y, vRight.z, 0.0f,
					vUp.x, vUp.y, vUp.z, 0.0f,
					vForward.x, vForward.y, vForward.z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
				};
				Matrix4 lookingMat;
				lookingMat.SetValues(matrix);
				m_pRenderer->MultiplyWorldMatrix(lookingMat);
			}

			// Translate by attached bone matrix
			m_pRenderer->MultiplyWorldMatrix(boneMatrix);
		}
	}

	// Rotation due to 3dsmax export affecting the bone rotations
	m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f);

	if (m_matrixIndex != -1)
	{
		glm::vec3 handBoneOffset = m_pParentCharacter->GetBoneMatrixRenderOffset(m_matrixName.c_str());

		// Translate for external matrix offset value
		m_pRenderer->TranslateWorldMatrix(handBoneOffset.x, handBoneOffset.y, handBoneOffset.z);

		// Rotation due to the weapon facing forwards for hand directions
		m_pRenderer->RotateWorldMatrix(90.0f, 0.0f, 0.0f);
	}

	// Scale to render size
	m_pRenderer->ScaleWorldMatrix(m_renderScale, m_renderScale, m_renderScale);

	// Translate for initial block offset
	m_pRenderer->TranslateWorldMatrix(m_renderOffset.x, m_renderOffset.y, m_renderOffset.z);

	// Render all animated sections
	for (int i = 0; i < m_numAnimatedSections; ++i)
	{
		m_pRenderer->PushMatrix();

		// Scale to render size
		m_pRenderer->ScaleWorldMatrix(m_pAnimatedSections[i].renderScale, m_pAnimatedSections[i].renderScale, m_pAnimatedSections[i].renderScale);

		// Translate for initial block offset
		m_pRenderer->TranslateWorldMatrix(m_pAnimatedSections[i].renderOffset.x, m_pAnimatedSections[i].renderOffset.y, m_pAnimatedSections[i].renderOffset.z);

		// Animated sections
		m_pRenderer->TranslateWorldMatrix(-m_pAnimatedSections[i].rotationPoint.x, -m_pAnimatedSections[i].rotationPoint.y, -m_pAnimatedSections[i].rotationPoint.z);
		m_pRenderer->RotateWorldMatrix(m_pAnimatedSections[i].rotationX, m_pAnimatedSections[i].rotationY, m_pAnimatedSections[i].rotationZ);
		m_pRenderer->TranslateWorldMatrix(m_pAnimatedSections[i].rotationPoint.x, m_pAnimatedSections[i].rotationPoint.y, m_pAnimatedSections[i].rotationPoint.z);

		m_pRenderer->TranslateWorldMatrix(m_pAnimatedSections[i].translateX, m_pAnimatedSections[i].translateY, m_pAnimatedSections[i].translateZ);

		Color outlineColor(1.0f, 1.0f, 0.0f, 1.0f);
		m_pAnimatedSections[i].pVoxelObject->Render(false, false, false, outlineColor);
		
		m_pRenderer->PopMatrix();
	}

	m_pRenderer->PopMatrix();
}

void VoxelWeapon::RenderWeaponTrails() const
{
	for (int i = 0; i < m_numWeaponTrails; ++i)
	{
		m_pRenderer->PushMatrix();

		if (m_pWeaponTrails[i].followOrigin)
		{
			m_pRenderer->MultiplyWorldMatrix(m_pWeaponTrails[i].origin);

			m_pRenderer->ScaleWorldMatrix(m_pWeaponTrails[i].parentScale, m_pWeaponTrails[i].parentScale, m_pWeaponTrails[i].parentScale);
		}

		m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		m_pRenderer->SetCullMode(CullMode::NOCULL);
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->SetLineWidth(3.0f);

		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

		for (int j = 0; j < m_pWeaponTrails[i].numTrailPoints - 1; ++j)
		{
			int index1 = j;
			int index2 = j + 1;

			if (index2 >= m_pWeaponTrails[i].numTrailPoints - 1)
			{
				index2 = 0;
			}

			if (m_pWeaponTrails[i].pTrailPoints[index1].pointActive == true &&
				m_pWeaponTrails[i].pTrailPoints[index2].pointActive == true)
			{
				float alpha1 = m_pWeaponTrails[i].pTrailPoints[index1].animaionTime / m_pWeaponTrails[i].trailTime;
				float alpha2 = m_pWeaponTrails[i].pTrailPoints[index2].animaionTime / m_pWeaponTrails[i].trailTime;

				m_pRenderer->ImmediateColorAlpha(m_pWeaponTrails[i].trailColor.GetRed(), m_pWeaponTrails[i].trailColor.GetGreen(), m_pWeaponTrails[i].trailColor.GetBlue(), alpha1);
				m_pRenderer->ImmediateVertex(m_pWeaponTrails[i].pTrailPoints[index1].startPoint.x, m_pWeaponTrails[i].pTrailPoints[index1].startPoint.y, m_pWeaponTrails[i].pTrailPoints[index1].startPoint.z);

				m_pRenderer->ImmediateColorAlpha(m_pWeaponTrails[i].trailColor.GetRed(), m_pWeaponTrails[i].trailColor.GetGreen(), m_pWeaponTrails[i].trailColor.GetBlue(), alpha1);
				m_pRenderer->ImmediateVertex(m_pWeaponTrails[i].pTrailPoints[index1].endPoint.x, m_pWeaponTrails[i].pTrailPoints[index1].endPoint.y, m_pWeaponTrails[i].pTrailPoints[index1].endPoint.z);

				m_pRenderer->ImmediateColorAlpha(m_pWeaponTrails[i].trailColor.GetRed(), m_pWeaponTrails[i].trailColor.GetGreen(), m_pWeaponTrails[i].trailColor.GetBlue(), alpha2);
				m_pRenderer->ImmediateVertex(m_pWeaponTrails[i].pTrailPoints[index2].endPoint.x, m_pWeaponTrails[i].pTrailPoints[index2].endPoint.y, m_pWeaponTrails[i].pTrailPoints[index2].endPoint.z);

				m_pRenderer->ImmediateColorAlpha(m_pWeaponTrails[i].trailColor.GetRed(), m_pWeaponTrails[i].trailColor.GetGreen(), m_pWeaponTrails[i].trailColor.GetBlue(), alpha2);
				m_pRenderer->ImmediateVertex(m_pWeaponTrails[i].pTrailPoints[index2].startPoint.x, m_pWeaponTrails[i].pTrailPoints[index2].startPoint.y, m_pWeaponTrails[i].pTrailPoints[index2].startPoint.z);
			}
		}

		m_pRenderer->DisableImmediateMode();
		m_pRenderer->DisableTransparency();
		m_pRenderer->SetCullMode(CullMode::BACK);
		
		m_pRenderer->PopMatrix();
	}
}