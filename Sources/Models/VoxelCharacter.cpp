/*************************************************************************
> File Name: VoxelCharacter.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Voxel character.
> Created Time: 2016/07/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <string>
#include <fstream>

#include <glm/detail/func_geometric.hpp>

#include <Utils/Interpolator.h>
#include <Utils/Random.h>

#include "VoxelCharacter.h"

// Constructor, Destructor
VoxelCharacter::VoxelCharacter(Renderer* pRenderer, QubicleBinaryManager* pQubicleBinaryManager) :
	m_pRenderer(pRenderer), m_pQubicleBinaryManager(pQubicleBinaryManager)
{
	
	Reset();
}

VoxelCharacter::~VoxelCharacter()
{
	UnloadCharacter();
	Reset();
}

void VoxelCharacter::Reset()
{
	m_isLoaded = false;

	m_isLoadedFaces = false;

	m_updateAnimator = true;

	m_renderRightWeapon = false;
	m_renderLeftWeapon = false;

	m_characterScale = 1.0f;

	m_characterAlpha = 1.0f;

	m_lookRotationAngle = 0.0f;
	m_lookTranslate = 0.0f;

	m_currentFrame = 0;

	m_pVoxelModel = nullptr;
	m_pCharacterModel = nullptr;
	for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
	{
		m_pCharacterAnimator[i] = nullptr;
	}

	m_pRightWeapon = nullptr;
	m_pLeftWeapon = nullptr;
	m_isRightWeaponLoaded = false;
	m_isLeftWeaponLoaded = false;

	// Breathing animation
	m_isBreathingAnimationEnabled = false;
	m_isBreathingAnimationStarted = false;
	m_breathingBodyYOffset = 0.0f;
	m_breathingHandsYOffset = 0.0f;
	Interpolator::GetInstance()->RemoveFloatInterpolationByVariable(&m_breathingBodyYOffset);
	Interpolator::GetInstance()->RemoveFloatInterpolationByVariable(&m_breathingHandsYOffset);
	m_breathingAnimationInitialWaitTime = GetRandomNumber(0, 100, 2) * 0.01f;

	// Facial expressions
	m_numFacialExpressions = 0;
	m_pFacialExpressions = nullptr;
	m_faceEyesTexture = -1;
	m_faceMouthTexture = -1;
	m_eyesOffset = glm::vec3(0.0f, 0.0f, 0.0f);
	m_mouthOffset = glm::vec3(0.0f, 0.0f, 0.0f);
	m_currentFacialExpression = 0;
	m_eyesBoneName = "";
	m_mouthBoneName = "";
	m_eyesBone = -1;
	m_mouthBone = -1;
	m_eyesMatrixIndex = -1;
	m_mouthMatrixIndex = -1;
	m_eyesTextureWidth = 9.0f;
	m_eyesTextureHeight = 9.0f;
	m_mouthTextureWidth = 9.0f;
	m_mouthTextureHeight = 9.0f;

	// Face looking direction
	m_faceLookingDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	m_faceLookingDirection = normalize(m_faceLookingDirection);
	m_faceTargetDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	m_faceTargetDirection = normalize(m_faceTargetDirection);
	m_faceLookToTargetSpeedMultiplier = 1.0f;
	m_headBoneIndex = -1;
	m_bodyBoneIndex = -1;
	m_leftShoulderBoneIndex = -1;
	m_leftHandBoneIndex = -1;
	m_rightShoulderBoneIndex = -1;
	m_rightHandBoneIndex = -1;
	m_legsBoneIndex = -1;
	m_rightFootBoneIndex = -1;
	m_leftFootBoneIndex = -1;
	m_isRandomLookDirectionEnabled = false;

	// Wink animation
	m_isWinkAnimationEnabled = false;
	m_faceEyesWinkTexture = -1;
	m_wink = false;
	m_winkWaitTimer = 4.0f + GetRandomNumber(-2, 2, 2);
	m_winkStayTime = 0.15f;

	// Talking animation
	m_isTalkingAnimationEnabled = false;
	m_numTalkingMouths = 0;
	m_pTalkingAnimations = nullptr;
	m_currentTalkingTexture = 0;
	m_talkingWaitTimer = 0.0f;
	m_talkingWaitTime = 0.12f;
	m_randomMouthSelection = false;
	m_talkingPauseTimer = 0.0f;
	m_talkingPauseTime = 0.45f;
	m_talkingPauseMouthCounter = 0;
	m_talkingPauseMouthAmount = 6;
}

void VoxelCharacter::LoadVoxelCharacter(const char* characterType, const char* qbFileName, const char* modelFileName, const char* animatorFileName, const char* facesFileName, const char* characterFileName, const char* charactersBaseFolder, bool useQubicleManager)
{
	m_usingQubicleManager = useQubicleManager;

	// Qubicle model
	if (useQubicleManager)
	{
		m_pVoxelModel = m_pQubicleBinaryManager->GetQubicleBinaryFile(qbFileName, false);
	}
	else
	{
		m_pVoxelModel = new QubicleBinary(m_pRenderer);
		m_pVoxelModel->Import(qbFileName, true);
	}

	// MS3d model
	m_pCharacterModel = new MS3DModel(m_pRenderer);
	m_pCharacterModel->LoadModel(modelFileName);

	// Animators
	for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
	{
		m_pCharacterAnimator[i] = new MS3DAnimator(m_pRenderer, m_pCharacterModel);
		m_pCharacterAnimator[i]->LoadAnimations(animatorFileName);
	}

	m_pCharacterAnimatorPaperdollLeft = new MS3DAnimator(m_pRenderer, m_pCharacterModel);
	m_pCharacterAnimatorPaperdollLeft->LoadAnimations(animatorFileName);
	m_pCharacterAnimatorPaperdollLeft->PlayAnimation("BindPose");
	m_pCharacterAnimatorPaperdollRight = new MS3DAnimator(m_pRenderer, m_pCharacterModel);
	m_pCharacterAnimatorPaperdollRight->LoadAnimations(animatorFileName);
	m_pCharacterAnimatorPaperdollRight->PlayAnimation("BindPose");

	m_pVoxelModel->SetupMatrixBones(m_pCharacterAnimator[0]);

	// Faces
	LoadFaces(characterType, facesFileName, charactersBaseFolder);
	SetupFacesBones();

	// Character file
	LoadCharacterFile(characterFileName);

	m_pRightWeapon = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);
	m_pLeftWeapon = new VoxelWeapon(m_pRenderer, m_pQubicleBinaryManager);

	m_isRightWeaponLoaded = true;
	m_isLeftWeaponLoaded = true;

	m_isLoaded = true;
}

void VoxelCharacter::SaveVoxelCharacter(const char* qbFileName, const char* facesFileName, const char* characterFileName) const
{
	// Qubicle model
	m_pVoxelModel->Export(qbFileName);

	// Faces
	SaveFaces(facesFileName);

	// Character file
	SaveCharacterFile(characterFileName);
}

void VoxelCharacter::UnloadCharacter()
{
	if (m_isLoaded)
	{
		if (m_usingQubicleManager == false)
		{
			delete m_pVoxelModel;
		}

		m_pVoxelModel = nullptr;
		delete m_pCharacterModel;
		m_pCharacterModel = nullptr;
		for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
		{
			delete m_pCharacterAnimator[i];
			m_pCharacterAnimator[i] = nullptr;
		}

		delete m_pCharacterAnimatorPaperdollLeft;
		delete m_pCharacterAnimatorPaperdollRight;

		delete[] m_pFacialExpressions;
		m_pFacialExpressions = nullptr;
		m_numFacialExpressions = 0;
	}

	if (m_pRightWeapon != nullptr)
	{
		delete m_pRightWeapon;
		m_pRightWeapon = nullptr;
		m_isRightWeaponLoaded = false;
	}
	if (m_pLeftWeapon != nullptr)
	{
		delete m_pLeftWeapon;
		m_pLeftWeapon = nullptr;
		m_isLeftWeaponLoaded = false;
	}

	m_renderRightWeapon = false;
	m_renderLeftWeapon = false;

	m_isLoaded = false;
}

// Rebuild
void VoxelCharacter::RebuildVoxelModel(bool faceMerge) const
{
	m_pVoxelModel->RebuildMesh(faceMerge);

	if (m_pRightWeapon != nullptr)
	{
		m_pRightWeapon->RebuildVoxelModel(faceMerge);
	}
	if (m_pLeftWeapon != nullptr)
	{
		m_pLeftWeapon->RebuildVoxelModel(faceMerge);
	}
}

// Faces
bool VoxelCharacter::LoadFaces(const char* characterType, const char* facesFileName, const char* charactersBaseFolder)
{
	std::ifstream file;

	// Open the file
	file.open(facesFileName, std::ios::in);

	if (file.is_open())
	{
		std::string tempString;
		int textureWidth, textureHeight, textureWidth2, textureHeight2;

		float offsetX;
		float offsetY;
		float offsetZ;

		file >> tempString >> offsetX >> offsetY >> offsetZ;
		m_eyesOffset = glm::vec3(offsetX, offsetY, offsetZ);

		file >> tempString >> offsetX >> offsetY >> offsetZ;
		m_mouthOffset = glm::vec3(offsetX, offsetY, offsetZ);

		file >> tempString >> m_eyesTextureWidth >> m_eyesTextureHeight;
		file >> tempString >> m_mouthTextureWidth >> m_mouthTextureHeight;

		file >> tempString >> m_winkTextureFileName;

		char winkFileName[128];
		sprintf(winkFileName, "%s/%s/%s", charactersBaseFolder, characterType, m_winkTextureFileName.c_str());
		m_pRenderer->LoadTexture(winkFileName, &textureWidth, &textureHeight, &textureWidth2, &textureHeight2, &m_faceEyesWinkTexture);

		file >> tempString >> m_eyesBoneName;
		file >> tempString >> m_mouthBoneName;

		file >> tempString >> m_numFacialExpressions;

		// Create the facial expressions objects
		m_pFacialExpressions = new FacialExpression[m_numFacialExpressions];

		for (int i = 0; i < m_numFacialExpressions; ++i)
		{
			char eyesFileName[128];
			char mouthFileName[128];
			file >> m_pFacialExpressions[i].facialExpressionName >> m_pFacialExpressions[i].eyesTextureFile >> m_pFacialExpressions[i].mouthTextureFile;

			sprintf(eyesFileName, "%s/%s/%s", charactersBaseFolder, characterType, m_pFacialExpressions[i].eyesTextureFile.c_str());
			m_pRenderer->LoadTexture(eyesFileName, &textureWidth, &textureHeight, &textureWidth2, &textureHeight2, &m_pFacialExpressions[i].eyeTexture);

			sprintf(mouthFileName, "%s/%s/%s", charactersBaseFolder, characterType, m_pFacialExpressions[i].mouthTextureFile.c_str());
			m_pRenderer->LoadTexture(mouthFileName, &textureWidth, &textureHeight, &textureWidth2, &textureHeight2, &m_pFacialExpressions[i].mouthTexture);
		}

		if (m_numFacialExpressions > 0)
		{
			m_faceEyesTexture = m_pFacialExpressions[0].eyeTexture;
			m_faceMouthTexture = m_pFacialExpressions[0].mouthTexture;
		}

		file >> tempString >> m_numTalkingMouths;
		m_pTalkingAnimations = new TalkingAnimation[m_numTalkingMouths];

		for (int i = 0; i < m_numTalkingMouths; ++i)
		{
			char talkingMouthFileName[128];
			file >> m_pTalkingAnimations[i].talkingAnimationTextureFile;

			sprintf(talkingMouthFileName, "%s/%s/%s", charactersBaseFolder, characterType, m_pTalkingAnimations[i].talkingAnimationTextureFile.c_str());
			m_pRenderer->LoadTexture(talkingMouthFileName, &textureWidth, &textureHeight, &textureWidth2, &textureHeight2, &m_pTalkingAnimations[i].talkingAnimationTexture);
		}

		file.close();

		m_isLoadedFaces = true;

		return true;
	}

	return false;
}

bool VoxelCharacter::SaveFaces(const char* facesFileName) const
{
	std::ofstream file;

	// Open the file
	file.open(facesFileName, std::ios::out);

	if (file.is_open())
	{
		file << "eyesOffset: " << m_eyesOffset.x << " " << m_eyesOffset.y << " " << m_eyesOffset.z << "\n";
		file << "mouthOffset: " << m_mouthOffset.x << " " << m_mouthOffset.y << " " << m_mouthOffset.z << "\n";

		file << "eyesSize: " << m_eyesTextureWidth << " " << m_eyesTextureHeight << "\n";
		file << "mouthSize: " << m_mouthTextureWidth << " " << m_mouthTextureHeight << "\n";

		file << "eyesWinkTexture: " << m_winkTextureFileName.c_str() << "\n";

		file << "eyesBone: " << m_eyesBoneName.c_str() << "\n";
		file << "mouthBone: " << m_mouthBoneName.c_str() << "\n\n";

		file << "numFaces: " << m_numFacialExpressions << "\n";

		for (int i = 0; i < m_numFacialExpressions; ++i)
		{
			file << m_pFacialExpressions[i].facialExpressionName.c_str() << " " << m_pFacialExpressions[i].eyesTextureFile.c_str() << " " << m_pFacialExpressions[i].mouthTextureFile.c_str() << "\n";
		}
		file << "\n";

		file << "numTalkingMouths: " << m_numTalkingMouths << "\n";

		for (int i = 0; i < m_numTalkingMouths; ++i)
		{
			file << m_pTalkingAnimations[i].talkingAnimationTextureFile.c_str() << "\n";
		}
		file << "\n";

		file.close();

		return true;
	}

	return false;
}

void VoxelCharacter::SetupFacesBones()
{
	const int fullBodyNum = static_cast<int>(AnimationSections::FullBody);

	m_eyesBone = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex(m_eyesBoneName.c_str());
	m_mouthBone = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex(m_mouthBoneName.c_str());

	m_eyesMatrixIndex = m_pVoxelModel->GetMatrixIndexForName(m_eyesBoneName.c_str());
	m_mouthMatrixIndex = m_pVoxelModel->GetMatrixIndexForName(m_mouthBoneName.c_str());

	m_headBoneIndex = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex("Head");
	m_bodyBoneIndex = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex("Body");
	m_leftShoulderBoneIndex = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex("Left_Shoulder");
	m_leftHandBoneIndex = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex("Left_Hand");
	m_rightShoulderBoneIndex = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex("Right_Shoulder");
	m_rightHandBoneIndex = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex("Right_Hand");
	m_legsBoneIndex = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex("Legs");
	m_rightFootBoneIndex = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex("Right_Foot");
	m_leftFootBoneIndex = m_pCharacterAnimator[fullBodyNum]->GetModel()->GetBoneIndex("Left_Foot");
}

void VoxelCharacter::ModifyEyesTextures(const char* charactersBaseFolder, const char* characterType, const char* eyeTextureFolder)
{
	int textureWidth, textureHeight, textureWidth2, textureHeight2;

	char winkFileName[128];

	// For saving to the faces file we need a stripped down version of the full path
	sprintf(winkFileName, "faces/%s/face_eyes_wink.tga", eyeTextureFolder);
	m_winkTextureFileName = winkFileName;

	// Generate full path for texture loading
	sprintf(winkFileName, "%s/%s/faces/%s/face_eyes_wink.tga", charactersBaseFolder, characterType, eyeTextureFolder);
	m_pRenderer->LoadTexture(winkFileName, &textureWidth, &textureHeight, &textureWidth2, &textureHeight2, &m_faceEyesWinkTexture);

	for (int i = 0; i < m_numFacialExpressions; ++i)
	{
		char eyesFileName[128];

		int firstindex = m_pFacialExpressions[i].eyesTextureFile.find_last_of("/");
		int lastindex = m_pFacialExpressions[i].eyesTextureFile.find_last_of(".");
		std::string fileWithoutExtension = m_pFacialExpressions[i].eyesTextureFile.substr(firstindex + 1, lastindex);

		// For saving to the faces file we need a stripped down version of the full path
		sprintf(eyesFileName, "faces/%s/%s", eyeTextureFolder, fileWithoutExtension.c_str());
		m_pFacialExpressions[i].eyesTextureFile = eyesFileName;

		// Generate full path for texture loading
		sprintf(eyesFileName, "%s/%s/faces/%s/%s", charactersBaseFolder, characterType, eyeTextureFolder, fileWithoutExtension.c_str());
		m_pRenderer->LoadTexture(eyesFileName, &textureWidth, &textureHeight, &textureWidth2, &textureHeight2, &m_pFacialExpressions[i].eyeTexture);
	}

	m_faceEyesTexture = m_pFacialExpressions[m_currentFacialExpression].eyeTexture;
	m_faceMouthTexture = m_pFacialExpressions[m_currentFacialExpression].mouthTexture;
}

// Character file
void VoxelCharacter::LoadCharacterFile(const char* characterFileName)
{
	std::ifstream file;

	// Open the file
	file.open(characterFileName, std::ios::in);

	if (file.is_open())
	{
		std::string tempString;
		int numModifiers;

		float xBoneScale;
		float yBoneScale;
		float zBoneScale;
		file >> tempString >> xBoneScale >> yBoneScale >> zBoneScale;
		m_boneScale = glm::vec3(xBoneScale, yBoneScale, zBoneScale);

		file >> tempString >> numModifiers;

		for (int i = 0; i < numModifiers; ++i)
		{
			std::string matrixName;
			float scale;
			float xOffset;
			float yOffset;
			float zOffset;

			file >> tempString >> matrixName;
			file >> tempString >> scale;
			file >> tempString >> xOffset >> yOffset >> zOffset;

			m_pVoxelModel->SetScaleAndOffsetForMatrix(matrixName.c_str(), scale, xOffset, yOffset, zOffset);
		}

		file.close();
	}
}

void VoxelCharacter::SaveCharacterFile(const char* characterFileName) const
{
	std::ofstream file;

	// Open the file
	file.open(characterFileName, std::ios::out);

	if (file.is_open())
	{
		file << "boneScale: " << m_boneScale.x << " " << m_boneScale.y << " " << m_boneScale.z << "\n\n";

		file << "numModifiers: " << m_pVoxelModel->GetNumMatrices() << "\n\n";

		for (int i = 0; i < m_pVoxelModel->GetNumMatrices(); ++i)
		{
			glm::vec3 offset = m_pVoxelModel->GetMatrixOffset(i);

			file << "boneName: " << m_pVoxelModel->GetMatrixName(i) << "\n";
			file << "Scale: " << m_pVoxelModel->GetMatrixScale(i) << "\n";
			file << "Offset: " << offset.x << " " << offset.y << " " << offset.z << "\n\n";
		}

		file.close();
	}
}

void VoxelCharacter::ResetMatrixParamsFromCharacterFile(const char* characterFileName, const char* matrixToReset) const
{
	std::ifstream file;

	// Open the file
	file.open(characterFileName, std::ios::in);

	if (file.is_open())
	{
		std::string tempString;
		int numModifiers;

		file >> tempString >> numModifiers;

		for (int i = 0; i < numModifiers; ++i)
		{
			std::string matrixName;
			float scale;
			float xOffset;
			float yOffset;
			float zOffset;

			file >> tempString >> matrixName;
			file >> tempString >> scale;
			file >> tempString >> xOffset >> yOffset >> zOffset;

			if (strcmp(matrixName.c_str(), matrixToReset) == 0)
			{
				m_pVoxelModel->SetScaleAndOffsetForMatrix(matrixName.c_str(), scale, xOffset, yOffset, zOffset);

				file.close();

				return;
			}
		}

		file.close();
	}
}

// Character scale
void VoxelCharacter::SetCharacterScale(float scale)
{
	m_characterScale = scale;
}

float VoxelCharacter::GetCharacterScale() const
{
	return m_characterScale;
}

// Character alpha
float VoxelCharacter::GetCharacterAlpha() const
{
	return m_characterAlpha;
}

// Upper body and head tilt look, for camera rotation
void VoxelCharacter::SetHeadAndUpperBodyLookRotation(float lookRotationAngle, float lookTranslate)
{
	m_lookRotationAngle = lookRotationAngle;
	m_lookTranslate = lookTranslate;
}

float VoxelCharacter::GetHeadAndUpperBodyLookRotation() const
{
	return m_lookRotationAngle;
}

float VoxelCharacter::GetHeadAndUpperBodyLookTranslate() const
{
	return m_lookTranslate;
}

void VoxelCharacter::SetCharacterMatrixRenderParams(const char* matrixName, float scale, float xOffset, float yOffset, float zOffset) const
{
	m_pVoxelModel->SetScaleAndOffsetForMatrix(matrixName, scale, xOffset, yOffset, zOffset);
}

float VoxelCharacter::GetBoneMatrixRenderScale(const char* matrixName) const
{
	return m_pVoxelModel->GetScale(matrixName);
}

glm::vec3 VoxelCharacter::GetBoneMatrixRenderOffset(const char* matrixName) const
{
	return m_pVoxelModel->GetOffset(matrixName);
}

// Weapons
void VoxelCharacter::LoadRightWeapon(const char* weaponFileName)
{
	if (m_isLoaded)
	{
		m_pRightWeapon->SetVoxelCharacterParent(this);
		m_pRightWeapon->SetBoneAttachment("Right_Hand");
		m_pRightWeapon->LoadWeapon(weaponFileName);

		m_renderRightWeapon = true;
		m_isRightWeaponLoaded = true;
	}
}

void VoxelCharacter::LoadLeftWeapon(const char* weaponFileName)
{
	if (m_isLoaded)
	{
		m_pLeftWeapon->SetVoxelCharacterParent(this);
		m_pLeftWeapon->SetBoneAttachment("Left_Hand");
		m_pLeftWeapon->LoadWeapon(weaponFileName);

		m_renderLeftWeapon = true;
		m_isLeftWeaponLoaded = true;
	}
}

VoxelWeapon* VoxelCharacter::GetRightWeapon() const
{
	return m_pRightWeapon;
}

VoxelWeapon* VoxelCharacter::GetLeftWeapon() const
{
	return m_pLeftWeapon;
}

void VoxelCharacter::UnloadRightWeapon()
{
	m_isRightWeaponLoaded = false;
}

void VoxelCharacter::UnloadLeftWeapon()
{
	m_isLeftWeaponLoaded = false;
}

bool VoxelCharacter::IsRightWeaponLoaded() const
{
	return m_isRightWeaponLoaded;
}

bool VoxelCharacter::IsLeftWeaponLoaded() const
{
	return m_isLeftWeaponLoaded;
}

// Setup animator and bones
void VoxelCharacter::SetUpdateAnimator(bool update)
{
	m_updateAnimator = update;
}

Matrix4 VoxelCharacter::GetBoneMatrix(AnimationSections section, int index) const
{
	if (m_isLoaded)
	{
		return m_pCharacterAnimator[static_cast<int>(section)]->GetBoneMatrix(index);
	}

	Matrix4 empty;
	return empty;
}

Matrix4 VoxelCharacter::GetBoneMatrix(AnimationSections section, const char* boneName) const
{
	if (m_isLoaded)
	{
		int boneIndex = m_pCharacterAnimator[static_cast<int>(section)]->GetModel()->GetBoneIndex(boneName);
		return GetBoneMatrix(section, boneIndex);
	}

	Matrix4 empty;
	return empty;
}

Matrix4 VoxelCharacter::GetBoneMatrixPaperdoll(int index, bool left) const
{
	if (m_isLoaded)
	{
		if (left)
		{
			return m_pCharacterAnimatorPaperdollLeft->GetBoneMatrix(index);
		}
		else
		{
			return m_pCharacterAnimatorPaperdollRight->GetBoneMatrix(index);
		}
	}

	Matrix4 empty;
	return empty;
}

int VoxelCharacter::GetBoneIndex(const char* boneName) const
{
	if (m_isLoaded)
	{
		return m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)]->GetModel()->GetBoneIndex(boneName);
	}

	return -1;
}

int VoxelCharacter::GetMatrixIndexForName(const char* matrixName) const
{
	if (m_isLoaded)
	{
		return m_pVoxelModel->GetMatrixIndexForName(matrixName);
	}

	return -1;
}

MS3DModel* VoxelCharacter::GetMS3DModel() const
{
	return m_pCharacterModel;
}

MS3DAnimator* VoxelCharacter::GetMS3DAnimator(AnimationSections section) const
{
	return m_pCharacterAnimator[static_cast<int>(section)];
}

QubicleBinary* VoxelCharacter::GetQubicleModel() const
{
	return m_pVoxelModel;
}

glm::vec3 VoxelCharacter::GetBoneScale() const
{
	return m_boneScale;
}

void VoxelCharacter::SetBoneScale(float scale)
{
	m_boneScale.x = scale;
	m_boneScale.y = scale;
	m_boneScale.z = scale;
}

// Rendering modes
void VoxelCharacter::SetWireFrameRender(bool wireframe) const
{
	if (m_pVoxelModel != nullptr)
	{
		m_pVoxelModel->SetWireFrameRender(wireframe);
	}

	if (m_pLeftWeapon != nullptr)
	{
		if (m_isLeftWeaponLoaded)
		{
			m_pLeftWeapon->SetWireFrameRender(wireframe);
		}
	}

	if (m_pRightWeapon != nullptr)
	{
		if (m_isRightWeaponLoaded)
		{
			m_pRightWeapon->SetWireFrameRender(wireframe);
		}
	}
}

void VoxelCharacter::SetRenderRightWeapon(bool render)
{
	m_renderRightWeapon = render;
}

void VoxelCharacter::SetRenderLeftWeapon(bool render)
{
	m_renderLeftWeapon = render;
}

void VoxelCharacter::SetMeshAlpha(float alpha, bool force)
{
	if (force == false && alpha == m_characterAlpha)
	{
		// Early out, since we haven't changed alpha (much!)
		return;
	}

	m_characterAlpha = alpha;

	if (m_pVoxelModel)
	{
		m_pVoxelModel->SetMeshAlpha(m_characterAlpha);
	}

	if (m_pLeftWeapon)
	{
		if (m_isLeftWeaponLoaded)
		{
			m_pLeftWeapon->SetMeshAlpha(m_characterAlpha);
		}
	}

	if (m_pRightWeapon)
	{
		if (m_isRightWeaponLoaded)
		{
			m_pRightWeapon->SetMeshAlpha(m_characterAlpha);
		}
	}
}

void VoxelCharacter::SetMeshSingleColor(float r, float g, float b) const
{
	if (m_pVoxelModel)
	{
		m_pVoxelModel->SetMeshSingleColor(r, g, b);
	}

	if (m_pLeftWeapon)
	{
		if (m_isLeftWeaponLoaded)
		{
			m_pLeftWeapon->SetMeshSingleColor(r, g, b);
		}
	}

	if (m_pRightWeapon)
	{
		if (m_isRightWeaponLoaded)
		{
			m_pRightWeapon->SetMeshSingleColor(r, g, b);
		}
	}
}

// Breathing animation
void VoxelCharacter::SetBreathingAnimationEnabled(bool enable)
{
	m_isBreathingAnimationEnabled = enable;
}

bool VoxelCharacter::IsBreathingAnimationEnabled() const
{
	return m_isBreathingAnimationEnabled;
}

bool VoxelCharacter::IsBreathingAnimationStarted() const
{
	return m_isBreathingAnimationStarted;
}

void VoxelCharacter::StartBreathAnimation()
{
	m_isBreathingAnimationStarted = true;

	FloatInterpolation* bodyYInterpolation1 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_breathingBodyYOffset, 0.0f, 0.35f, 1.5f, 100.0f);
	FloatInterpolation* bodyYInterpolation2 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_breathingBodyYOffset, 0.35f, 0.35f, 0.175f, 0.0f);
	FloatInterpolation* bodyYInterpolation3 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_breathingBodyYOffset, 0.35f, 0.0f, 1.5f, -100.0f);
	FloatInterpolation* bodyYInterpolation4 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_breathingBodyYOffset, 0.0f, 0.0f, 0.05f, 0.0f, nullptr, _BreathAnimationFinished, this);
	Interpolator::GetInstance()->LinkFloatInterpolation(bodyYInterpolation1, bodyYInterpolation2);
	Interpolator::GetInstance()->LinkFloatInterpolation(bodyYInterpolation2, bodyYInterpolation3);
	Interpolator::GetInstance()->LinkFloatInterpolation(bodyYInterpolation3, bodyYInterpolation4);

	FloatInterpolation* handsYInterpolation1 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_breathingHandsYOffset, 0.0f, 0.0f, 0.5f, 0.0f);
	FloatInterpolation* handsYInterpolation2 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_breathingHandsYOffset, 0.0f, 0.75f, 1.25f, 100.0f);
	FloatInterpolation* handsYInterpolation3 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_breathingHandsYOffset, 0.75f, 0.75f, 0.125f, 0.0f);
	FloatInterpolation* handsYInterpolation4 = Interpolator::GetInstance()->CreateFloatInterpolation(&m_breathingHandsYOffset, 0.75f, 0.0f, 1.5f, -100.0f);
	Interpolator::GetInstance()->LinkFloatInterpolation(handsYInterpolation1, handsYInterpolation2);
	Interpolator::GetInstance()->LinkFloatInterpolation(handsYInterpolation2, handsYInterpolation3);
	Interpolator::GetInstance()->LinkFloatInterpolation(handsYInterpolation3, handsYInterpolation4);

	Interpolator::GetInstance()->AddFloatInterpolation(bodyYInterpolation1);
	Interpolator::GetInstance()->AddFloatInterpolation(handsYInterpolation1);
}

float VoxelCharacter::GetBreathingAnimationOffsetForBone(int boneIndex) const
{
	const int fullBodyNum = static_cast<int>(AnimationSections::FullBody);

	if (strcmp(m_pCharacterAnimator[fullBodyNum]->GetModel()->GetNameFromBoneIndex(boneIndex), "Head") == 0)
	{
		return m_breathingBodyYOffset * 0.75f;
	}
	if (strcmp(m_pCharacterAnimator[fullBodyNum]->GetModel()->GetNameFromBoneIndex(boneIndex), "Body") == 0)
	{
		return m_breathingBodyYOffset;
	}
	if (strcmp(m_pCharacterAnimator[fullBodyNum]->GetModel()->GetNameFromBoneIndex(boneIndex), "Legs") == 0)
	{
		return m_breathingBodyYOffset * 0.5f;
	}
	if (strcmp(m_pCharacterAnimator[fullBodyNum]->GetModel()->GetNameFromBoneIndex(boneIndex), "Right_Shoulder") == 0)
	{
		return m_breathingHandsYOffset;
	}
	if (strcmp(m_pCharacterAnimator[fullBodyNum]->GetModel()->GetNameFromBoneIndex(boneIndex), "Left_Shoulder") == 0)
	{
		return m_breathingHandsYOffset;
	}
	if (strcmp(m_pCharacterAnimator[fullBodyNum]->GetModel()->GetNameFromBoneIndex(boneIndex), "Right_Hand") == 0)
	{
		return m_breathingHandsYOffset;
	}
	if (strcmp(m_pCharacterAnimator[fullBodyNum]->GetModel()->GetNameFromBoneIndex(boneIndex), "Left_Hand") == 0)
	{
		return m_breathingHandsYOffset;
	}

	return 0.0f;
}

// Facial expressions
int VoxelCharacter::GetNumFacialExpressions() const
{
	return m_numFacialExpressions;
}

const char* VoxelCharacter::GetFacialExpressionName(const int index) const
{
	return m_pFacialExpressions[index].facialExpressionName.c_str();
}

glm::vec3 VoxelCharacter::GetEyesOffset() const
{
	return m_eyesOffset;
}

glm::vec3 VoxelCharacter::GetMouthOffset() const
{
	return m_mouthOffset;
}

void VoxelCharacter::SetEyesOffset(glm::vec3 offset)
{
	m_eyesOffset = offset;
}

void VoxelCharacter::SetMouthOffset(glm::vec3 offset)
{
	m_mouthOffset = offset;
}

void VoxelCharacter::PlayFacialExpression(const char* facialExpressionName)
{
	if (m_isLoadedFaces)
	{
		int expressionIndex = -1;
		for (int i = 0; i < m_numFacialExpressions; ++i)
		{
			#ifdef _WIN32
			if (_strcmpi(facialExpressionName, m_pFacialExpressions[i].facialExpressionName.c_str()) == 0)
			#else
			if (strcasecmp(facialExpressionName, m_pFacialExpressions[i].facialExpressionName.c_str()) == 0)
			#endif //_WIN32
			{
				expressionIndex = i;
				break;
			}
		}

		if (expressionIndex != -1)
		{
			PlayFacialExpression(expressionIndex);
		}
	}
}

void VoxelCharacter::PlayFacialExpression(int facialAnimationIndex)
{
	if (m_isLoadedFaces)
	{
		if (facialAnimationIndex >= 0 && facialAnimationIndex < m_numFacialExpressions)
		{
			m_currentFacialExpression = facialAnimationIndex;

			m_faceEyesTexture = m_pFacialExpressions[m_currentFacialExpression].eyeTexture;
			m_faceMouthTexture = m_pFacialExpressions[m_currentFacialExpression].mouthTexture;
		}
	}
}

int VoxelCharacter::GetCurrentFacialAnimation() const
{
	return m_currentFacialExpression;
}

void VoxelCharacter::SetEyesBone(std::string eyesBoneName)
{
	if (m_isLoadedFaces)
	{
		m_eyesBoneName = eyesBoneName;
		SetupFacesBones();
	}
}

void VoxelCharacter::SetMouthBone(std::string mouthBoneName)
{
	if (m_isLoadedFaces)
	{
		m_mouthBoneName = mouthBoneName;
		SetupFacesBones();
	}
}

std::string VoxelCharacter::GetEyesBoneName() const
{
	return m_eyesBoneName;
}

std::string VoxelCharacter::GetMouthBoneName() const
{
	return m_mouthBoneName;
}

int VoxelCharacter::GetEyesBone() const
{
	return m_eyesBone;
}

int VoxelCharacter::GetMouthBone() const
{
	return m_mouthBone;
}

int VoxelCharacter::GetEyesMatrixIndex() const
{
	return m_eyesMatrixIndex;
}

int VoxelCharacter::GetMouthMatrixIndex() const
{
	return m_mouthMatrixIndex;
}

void VoxelCharacter::SetEyesTextureSize(float width, float height)
{
	m_eyesTextureWidth = width;
	m_eyesTextureHeight = height;
}

void VoxelCharacter::SetMouthTextureSize(float width, float height)
{
	m_mouthTextureWidth = width;
	m_mouthTextureHeight = height;
}

float VoxelCharacter::GetEyeTextureWidth() const
{
	return m_eyesTextureWidth;
}

float VoxelCharacter::GetEyeTextureHeight() const
{
	return m_eyesTextureHeight;
}

float VoxelCharacter::GetMouthTextureWidth() const
{
	return m_mouthTextureWidth;
}

float VoxelCharacter::GetMouthTextureHeight() const
{
	return m_mouthTextureHeight;
}

// Face looking
glm::vec3 VoxelCharacter::GetFaceLookingDirection() const
{
	return m_faceLookingDirection;
}

void VoxelCharacter::SetFaceLookingDirection(glm::vec3 looking)
{
	if (length(looking) > 0.001f)
	{
		m_faceLookingDirection = normalize(looking);
	}
	else
	{
		m_faceLookingDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	}
}

glm::vec3 VoxelCharacter::GetFaceTargetDirection() const
{
	return m_faceTargetDirection;
}

void VoxelCharacter::SetFaceTargetDirection(glm::vec3 target)
{
	if (length(target) > 0.001f)
	{
		m_faceTargetDirection = normalize(target);
	}
	else
	{
		m_faceTargetDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	}
}

float VoxelCharacter::GetFaceLookToTargetSpeedMultiplier() const
{
	return m_faceLookToTargetSpeedMultiplier;
}

void VoxelCharacter::SetFaceLookToTargetSpeedMultiplier(float speedMultiplier)
{
	m_faceLookToTargetSpeedMultiplier = speedMultiplier;
}

int VoxelCharacter::GetHeadBoneIndex() const
{
	return m_headBoneIndex;
}

int VoxelCharacter::GetBodyBoneIndex() const
{
	return m_bodyBoneIndex;
}

int VoxelCharacter::GetLeftShoulderBoneIndex() const
{
	return m_leftShoulderBoneIndex;
}

int VoxelCharacter::GetLeftHandBoneIndex() const
{
	return m_leftHandBoneIndex;
}

int VoxelCharacter::GetRightShoulderBoneIndex() const
{
	return m_rightShoulderBoneIndex;
}

int VoxelCharacter::GetRightHandBoneIndex() const
{
	return m_rightHandBoneIndex;
}

int VoxelCharacter::GetLegsBoneIndex() const
{
	return m_legsBoneIndex;
}

int VoxelCharacter::GetRightFootBoneIndex() const
{
	return m_rightFootBoneIndex;
}

int VoxelCharacter::GetLeftFootBoneIndex() const
{
	return m_leftFootBoneIndex;
}

void VoxelCharacter::SetRandomLookDirection(bool enable)
{
	m_isRandomLookDirectionEnabled = enable;
}

bool VoxelCharacter::IsRandomLookDirectionEnabled() const
{
	return m_isRandomLookDirectionEnabled;
}

// Wink animation
void VoxelCharacter::SetWinkAnimationEnabled(bool enable)
{
	m_isWinkAnimationEnabled = enable;
}

bool VoxelCharacter::IsWinkAnimationEnabled() const
{
	return m_isWinkAnimationEnabled;
}

void VoxelCharacter::UpdateWinkAnimation(float dt)
{
	m_winkWaitTimer -= dt;

	if (m_winkWaitTimer <= 0.0f)
	{
		m_winkWaitTimer = 4.0f + GetRandomNumber(-2, 2, 2);
		m_wink = false;

		// Return eyes back to whatever they were before the wink
		m_faceEyesTexture = m_pFacialExpressions[m_currentFacialExpression].eyeTexture;
	}
	else if (m_winkWaitTimer <= m_winkStayTime)
	{
		m_wink = true;
		m_faceEyesTexture = m_faceEyesWinkTexture;
	}
}

// Talking animation
void VoxelCharacter::SetTalkingAnimationEnabled(bool enable)
{
	m_isTalkingAnimationEnabled = enable;

	if (m_isLoaded && m_isLoadedFaces)
	{
		if (m_isTalkingAnimationEnabled == false)
		{
			m_faceMouthTexture = m_pFacialExpressions[m_currentFacialExpression].mouthTexture;
		}
	}
}

bool VoxelCharacter::IsTalkingAnimationEnabled() const
{
	return m_isTalkingAnimationEnabled;
}

void VoxelCharacter::SetRandomMouthSelection(bool random)
{
	m_randomMouthSelection = random;
}

bool VoxelCharacter::IsRandomMouthSelectionEnabled() const
{
	return m_randomMouthSelection;
}

void VoxelCharacter::UpdateTalkingAnimation(float dt)
{
	m_talkingWaitTimer -= dt;
	m_talkingPauseTimer -= dt;

	if (m_talkingWaitTimer <= 0.0f && m_talkingPauseTimer <= 0.0f)
	{
		if (m_randomMouthSelection)
		{
			// Random mouth selection
			m_currentTalkingTexture = GetRandomNumber(0, m_numTalkingMouths - 1);
		}
		else
		{
			// Ordered mouth selection
			m_currentTalkingTexture++;

			if (m_currentTalkingTexture >= m_numTalkingMouths)
			{
				m_currentTalkingTexture = 0;
			}
		}

		// Should we take a pause from cycling talking mouths, to simulate normal conversation stops...
		m_talkingPauseMouthCounter++;

		if (m_talkingPauseMouthCounter == m_talkingPauseMouthAmount)
		{
			m_talkingPauseMouthCounter = 0;
			m_talkingPauseMouthAmount = 6 + GetRandomNumber(-2, 5);

			float randomTimeAddtion = GetRandomNumber(-25, 40, 2) * 0.01f;
			m_talkingPauseTimer = m_talkingPauseTime + randomTimeAddtion;
		}

		if (m_talkingPauseTimer > 0.0f)
		{
			if (GetRandomNumber(0, 100, 1) > 50)
			{
				// Revert back to the face pose mouth
				m_faceMouthTexture = m_pFacialExpressions[m_currentFacialExpression].mouthTexture;
			}
			else
			{
				// Leave the current mouth on pause for this wait period
			}
		}
		else
		{
			m_faceMouthTexture = m_pTalkingAnimations[m_currentTalkingTexture].talkingAnimationTexture;

			float randomTimeAddtion = GetRandomNumber(-10, 50, 2) * 0.00225f;
			m_talkingWaitTimer = m_talkingWaitTime + randomTimeAddtion;
		}
	}
}

// Animations
int VoxelCharacter::GetNumAnimations() const
{
	if (m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)] == nullptr)
	{
		return 0;
	}

	return m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)]->GetNumAnimations();
}

const char* VoxelCharacter::GetAnimationName(int index) const
{
	if (m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)] == nullptr)
	{
		return "";
	}

	return m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)]->GetAnimationName(index);
}

void VoxelCharacter::PlayAnimation(AnimationSections section, bool waitForComplete, AnimationSections syncWithSection, const char* animationName)
{
	if (m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)] == nullptr)
	{
		return;
	}

	if (section == AnimationSections::FullBody)
	{
		bool syncComplete = (m_pCharacterAnimator[static_cast<int>(syncWithSection)]->HasAnimationFinished() || m_pCharacterAnimator[static_cast<int>(syncWithSection)]->HasAnimationLooped());

		for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
		{
			if (waitForComplete == false || (m_pCharacterAnimator[i]->HasAnimationFinished() || m_pCharacterAnimator[i]->HasAnimationLooped()))
			{
				if (syncWithSection == section || syncComplete)
				{
					m_pCharacterAnimator[i]->PlayAnimation(animationName);

					// Stop weapon trails
					if (m_pRightWeapon != nullptr)
					{
						m_pRightWeapon->StopWeaponTrails();
					}
					if (m_pLeftWeapon != nullptr)
					{
						m_pLeftWeapon->StopWeaponTrails();
					}
				}
			}
		}
	}
	else
	{
		m_pCharacterAnimator[static_cast<int>(section)]->PlayAnimation(animationName);

		// Stop weapon trails
		if (section == AnimationSections::RightArmHand)
		{
			if (m_pRightWeapon != nullptr)
			{
				m_pRightWeapon->StopWeaponTrails();
			}
		}
		if (section == AnimationSections::LeftArmHand)
		{
			if (m_pLeftWeapon != nullptr)
			{
				m_pLeftWeapon->StopWeaponTrails();
			}
		}
	}
}

int VoxelCharacter::GetCurrentAnimationIndex(AnimationSections section) const
{
	if (m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)] == nullptr)
	{
		return -1;
	}

	return m_pCharacterAnimator[static_cast<int>(section)]->GetCurrentAnimationIndex();
}

void VoxelCharacter::SetBlendAnimation(AnimationSections section, bool waitForComplete, AnimationSections syncWithSection, const char* startAnimationName, const char* endAnimationName, float blendTime)
{
	if (m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)] == nullptr)
	{
		return;
	}

	if (section == AnimationSections::FullBody)
	{
		bool syncComplete = (m_pCharacterAnimator[static_cast<int>(syncWithSection)]->HasAnimationFinished() || m_pCharacterAnimator[static_cast<int>(syncWithSection)]->HasAnimationLooped());

		for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
		{
			if (waitForComplete == false || (m_pCharacterAnimator[i]->HasAnimationFinished() || m_pCharacterAnimator[i]->HasAnimationLooped()))
			{
				if (syncWithSection == section || syncComplete)
				{
					m_pCharacterAnimator[i]->StartBlendAnimation(startAnimationName, endAnimationName, blendTime);

					// Stop weapon trails
					if (m_pRightWeapon != nullptr)
					{
						m_pRightWeapon->StopWeaponTrails();
					}
					if (m_pLeftWeapon != nullptr)
					{
						m_pLeftWeapon->StopWeaponTrails();
					}
				}
			}
		}
	}
	else
	{
		m_pCharacterAnimator[static_cast<int>(section)]->StartBlendAnimation(startAnimationName, endAnimationName, blendTime);

		// Stop weapon trails
		if (section == AnimationSections::RightArmHand)
		{
			if (m_pRightWeapon != nullptr)
			{
				m_pRightWeapon->StopWeaponTrails();
			}
		}
		if (section == AnimationSections::LeftArmHand)
		{
			if (m_pLeftWeapon != nullptr)
			{
				m_pLeftWeapon->StopWeaponTrails();
			}
		}
	}
}

void VoxelCharacter::BlendIntoAnimation(AnimationSections section, bool waitForComplete, AnimationSections syncWithSection, const char* animationName, float blendTime)
{
	if (m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)] == nullptr)
	{
		return;
	}

	if (section == AnimationSections::FullBody)
	{
		bool syncComplete = (m_pCharacterAnimator[static_cast<int>(syncWithSection)]->HasAnimationFinished() || m_pCharacterAnimator[static_cast<int>(syncWithSection)]->HasAnimationLooped());

		for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
		{
			if (waitForComplete == false || (m_pCharacterAnimator[i]->HasAnimationFinished() || m_pCharacterAnimator[i]->HasAnimationLooped()))
			{
				if (syncWithSection == section || syncComplete)
				{
					m_pCharacterAnimator[i]->BlendIntoAnimation(animationName, blendTime);

					// Stop weapon trails
					if (m_pRightWeapon != nullptr)
					{
						m_pRightWeapon->StopWeaponTrails();
					}
					if (m_pLeftWeapon != nullptr)
					{
						m_pLeftWeapon->StopWeaponTrails();
					}
				}
			}
		}
	}
	else
	{
		if (waitForComplete == false || (m_pCharacterAnimator[static_cast<int>(section)]->HasAnimationFinished() || m_pCharacterAnimator[static_cast<int>(section)]->HasAnimationLooped()))
		{
			if (syncWithSection == section || (m_pCharacterAnimator[static_cast<int>(syncWithSection)]->HasAnimationFinished() || m_pCharacterAnimator[static_cast<int>(syncWithSection)]->HasAnimationLooped()))
			{
				m_pCharacterAnimator[static_cast<int>(section)]->BlendIntoAnimation(animationName, blendTime);

				// Stop weapon trails
				if (section == AnimationSections::RightArmHand)
				{
					if (m_pRightWeapon != nullptr)
					{
						m_pRightWeapon->StopWeaponTrails();
					}
				}
				if (section == AnimationSections::LeftArmHand)
				{
					if (m_pLeftWeapon != nullptr)
					{
						m_pLeftWeapon->StopWeaponTrails();
					}
				}
			}
		}
	}
}

bool VoxelCharacter::HasAnimationFinished(AnimationSections section) const
{
	if (m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)] == nullptr)
	{
		return false;
	}

	return m_pCharacterAnimator[static_cast<int>(section)]->HasAnimationFinished();
}

bool VoxelCharacter::HasAnimationLooped(AnimationSections section) const
{
	if (m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)] == nullptr)
	{
		return false;
	}

	return m_pCharacterAnimator[static_cast<int>(section)]->HasAnimationLooped();
}

void VoxelCharacter::StepAnimationFrame(float dt)
{
	for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
	{
		if (m_pCharacterAnimator[i] != nullptr)
		{
			m_pCharacterAnimator[i]->Update(dt);
		}
	}
}

int VoxelCharacter::GetStartFrame(const char* animationName) const
{
	return m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)]->GetStartFrame(animationName);
}

int VoxelCharacter::GetEndFrame(const char* animationName) const
{
	return m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)]->GetEndFrame(animationName);
}

int VoxelCharacter::GetCurrentFrame() const
{
	return m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)]->GetCurrentFrame();
}

int VoxelCharacter::GetNumJoints() const
{
	return m_pCharacterModel->GetNumJoints();
}

Joint* VoxelCharacter::GetJoint(int index) const
{
	return m_pCharacterModel->GetJoint(index);
}

Joint* VoxelCharacter::GetJoint(const char* jointName) const
{
	return m_pCharacterModel->GetJoint(jointName);
}

void VoxelCharacter::PlayAnimationOnPaperDoll(const char* animationName, bool isLeft) const
{
	if (isLeft)
	{
		m_pCharacterAnimatorPaperdollLeft->PlayAnimation(animationName);
	}
	else
	{
		m_pCharacterAnimatorPaperdollRight->PlayAnimation(animationName);
	}
}

// Matrices
int VoxelCharacter::GetNumModelMatrices() const
{
	return m_pVoxelModel->GetNumMatrices();
}

const char* VoxelCharacter::GetModelMatrixName(int index) const
{
	return m_pVoxelModel->GetMatrixName(index);
}

// Swapping and adding new matrices
void VoxelCharacter::SwapBodyPart(const char* bodyPartName, QubicleMatrix* pMatrix, bool copyMatrixParams) const
{
	m_pVoxelModel->SwapMatrix(bodyPartName, pMatrix, copyMatrixParams);
}

void VoxelCharacter::AddQubicleMatrix(QubicleMatrix* pNewMatrix, bool copyMatrixParams) const
{
	m_pVoxelModel->AddQubicleMatrix(pNewMatrix, copyMatrixParams);
}

void VoxelCharacter::RemoveQubicleMatrix(const char* matrixName) const
{
	m_pVoxelModel->RemoveQubicleMatrix(matrixName);
}

void VoxelCharacter::SetQubicleMatrixRender(const char* matrixName, bool render) const
{
	m_pVoxelModel->SetQubicleMatrixRender(matrixName, render);
}

// Sub selection of individual body parts
std::string VoxelCharacter::GetSubSelectionName(int pickingID) const
{
	return m_pVoxelModel->GetSubSelectionName(pickingID);
}

// Update
void VoxelCharacter::Update(float dt, float animationSpeed[static_cast<int>(AnimationSections::NumSections)])
{
	if (m_isLoaded == false)
	{
		return;
	}

	// Update skeleton animation
	for (int i = 0; i < static_cast<int>(AnimationSections::NumSections); ++i)
	{
		if (m_pCharacterAnimator[i] != nullptr)
		{
			if (m_updateAnimator)
			{
				m_pCharacterAnimator[i]->Update(dt * animationSpeed[i]);
			}
		}
	}

	// Update paperdoll animator
	if (m_updateAnimator)
	{
		if (m_pCharacterAnimatorPaperdollLeft != nullptr)
		{
			m_pCharacterAnimatorPaperdollLeft->Update(dt);
		}
		if (m_pCharacterAnimatorPaperdollRight != nullptr)
		{
			m_pCharacterAnimatorPaperdollRight->Update(dt);
		}
	}

	// Breathing animations
	if (m_isBreathingAnimationEnabled && m_isBreathingAnimationStarted == false)
	{
		// So we have an initial delay, do all characters are not in sync
		if (m_breathingAnimationInitialWaitTime <= 0.0f)
		{
			StartBreathAnimation();
		}
		else
		{
			m_breathingAnimationInitialWaitTime -= dt;
		}
	}

	// Facial animation
	if (m_isLoadedFaces)
	{
		if (m_isWinkAnimationEnabled || m_wink == true)
		{
			UpdateWinkAnimation(dt);
		}

		if (m_isTalkingAnimationEnabled)
		{
			UpdateTalkingAnimation(dt);
		}
	}

	// Face looking
	if (distance(m_faceLookingDirection, m_faceTargetDirection) <= 0.01f)
	{
		if (m_isRandomLookDirectionEnabled)
		{
			m_faceTargetDirection = glm::vec3(GetRandomNumber(-1, 1, 2) * 0.65f, GetRandomNumber(-1, 1, 2) * 0.175f, GetRandomNumber(0, 3, 2) + 0.35f);
			m_faceTargetDirection = normalize(m_faceTargetDirection);
		}
	}
	else
	{
		glm::vec3 toTarget = m_faceTargetDirection - m_faceLookingDirection;
		m_faceLookingDirection += (toTarget * dt) * m_faceLookToTargetSpeedMultiplier;
		m_faceLookingDirection = normalize(m_faceLookingDirection);
	}

	// Animated weapons
	if (m_pLeftWeapon != nullptr)
	{
		if (m_isLeftWeaponLoaded)
		{
			m_pLeftWeapon->Update(dt);
		}
	}

	if (m_pRightWeapon != nullptr)
	{
		if (m_isRightWeaponLoaded)
		{
			m_pRightWeapon->Update(dt);
		}
	}

	if (m_pLeftWeapon != nullptr)
	{
		m_pLeftWeapon->CreateWeaponTrailPoint();
	}
	if (m_pRightWeapon != nullptr)
	{
		m_pRightWeapon->CreateWeaponTrailPoint();
	}

	// Update the weapon trails, based on the animation frames
	if (m_pCharacterAnimator[static_cast<int>(AnimationSections::LeftArmHand)] != nullptr)
	{
		bool leftWeaponTrailActive = m_pCharacterAnimator[static_cast<int>(AnimationSections::LeftArmHand)]->GetLeftWeaponTrailActive();
		
		if (m_pLeftWeapon->IsWeaponTrailsActive() == false && leftWeaponTrailActive == true)
		{
			m_pLeftWeapon->StartWeaponTrails();
		}
		else if (m_pLeftWeapon->IsWeaponTrailsActive() == true && leftWeaponTrailActive == false)
		{
			m_pLeftWeapon->StopWeaponTrails();
		}
	}

	if (m_pCharacterAnimator[static_cast<int>(AnimationSections::RightArmHand)] != nullptr)
	{
		bool rightWeaponTrailActive = m_pCharacterAnimator[static_cast<int>(AnimationSections::RightArmHand)]->GetRightWeaponTrailActive();
		
		if (m_pRightWeapon->IsWeaponTrailsActive() == false && rightWeaponTrailActive == true)
		{
			m_pRightWeapon->StartWeaponTrails();
		}
		else if (m_pRightWeapon->IsWeaponTrailsActive() == true && rightWeaponTrailActive == false)
		{
			m_pRightWeapon->StopWeaponTrails();
		}
	}
}

void VoxelCharacter::SetWeaponTrailsOriginMatrix(Matrix4 originMatrix)
{
	if (m_pLeftWeapon != nullptr)
	{
		if (m_isLeftWeaponLoaded)
		{
			m_pLeftWeapon->SetWeaponTrailsParams(originMatrix, m_characterScale);
		}
	}

	if (m_pRightWeapon != nullptr)
	{
		if (m_isRightWeaponLoaded)
		{
			m_pRightWeapon->SetWeaponTrailsParams(originMatrix, m_characterScale);
		}
	}
}

// Rendering
void VoxelCharacter::Render(bool renderOutline, bool reflection, bool silhouette, Color outlineColor, bool subSelectionNamePicking)
{
	if (m_pVoxelModel != nullptr)
	{
		m_pRenderer->PushMatrix();
		m_pRenderer->ScaleWorldMatrix(m_characterScale, m_characterScale, m_characterScale);
		m_pVoxelModel->RenderWithAnimator(m_pCharacterAnimator, this, renderOutline, reflection, silhouette, outlineColor, subSelectionNamePicking);
		m_pRenderer->PopMatrix();
	}
}

void VoxelCharacter::RenderSubSelection(std::string subSelection, bool renderOutline, bool silhouette, Color outlineColor)
{
	if (m_pVoxelModel != nullptr)
	{
		m_pRenderer->PushMatrix();
		m_pRenderer->ScaleWorldMatrix(m_characterScale, m_characterScale, m_characterScale);
		m_pVoxelModel->RenderSingleMatrix(m_pCharacterAnimator, this, subSelection, renderOutline, silhouette, outlineColor);
		m_pRenderer->PopMatrix();
	}
}

void VoxelCharacter::RenderBones() const
{
	if (m_pVoxelModel != nullptr)
	{
		m_pRenderer->PushMatrix();
		m_pRenderer->ScaleWorldMatrix(m_boneScale.x, m_boneScale.y, m_boneScale.z);
		m_pRenderer->ScaleWorldMatrix(m_characterScale, m_characterScale, m_characterScale);
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->SetLineWidth(3.0f);
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
		m_pCharacterAnimator[static_cast<int>(AnimationSections::FullBody)]->RenderBones();
		m_pRenderer->PopMatrix();
	}
}

void VoxelCharacter::RenderFace()
{
	if (m_isLoadedFaces == false)
	{
		return;
	}

	if (m_pVoxelModel != nullptr)
	{
		m_pRenderer->PushMatrix();
		m_pRenderer->ScaleWorldMatrix(m_characterScale, m_characterScale, m_characterScale);
		m_pVoxelModel->RenderFace(m_pCharacterAnimator[static_cast<int>(AnimationSections::HeadBody)], this, true);
		m_pRenderer->PopMatrix();
	}
}

void VoxelCharacter::RenderFacingDebug() const
{
	if (m_pVoxelModel != nullptr)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->ScaleWorldMatrix(m_characterScale, m_characterScale, m_characterScale);

		// Debug render facing vector
		m_pRenderer->PushMatrix();

		Matrix4 headMatrix = GetBoneMatrix(AnimationSections::HeadBody, "Head");
		m_pRenderer->MultiplyWorldMatrix(headMatrix);

		// Rotation due to 3DS Max export affecting the bone rotations
		m_pRenderer->RotateWorldMatrix(0.0f, 0.0f, -90.0f);

		m_pRenderer->EnableMaterial(m_pVoxelModel->GetMaterial());
		m_pRenderer->SetRenderMode(RenderMode::SOLID);
		m_pRenderer->SetLineWidth(3.0f);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
		m_pRenderer->ImmediateVertex(m_faceLookingDirection.x * 15.0f, m_faceLookingDirection.y * 15.0f, m_faceLookingDirection.z * 15.0f);

		m_pRenderer->ImmediateColorAlpha(0.0f, 1.0f, 0.0f, 1.0f);
		m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);
		m_pRenderer->ImmediateVertex(m_faceTargetDirection.x * 15.0f, m_faceTargetDirection.y * 15.0f, m_faceTargetDirection.z * 15.0f);
		
		m_pRenderer->DisableImmediateMode();
		
		m_pRenderer->PopMatrix();

		m_pRenderer->PopMatrix();
	}
}

void VoxelCharacter::RenderFaceTextures(bool eyesTexture, bool wireframe, bool transparency) const
{
	if (m_pCharacterModel == nullptr || m_pCharacterAnimator == nullptr)
	{
		return;
	}

	if (m_faceEyesTexture == -1 || m_faceMouthTexture == -1)
	{
		return;
	}

	float textureWidth = 1.0f;
	float textureHeight = 1.0f;

	float width;
	float height;

	if (eyesTexture)
	{
		width = m_eyesTextureWidth;
		height = m_eyesTextureHeight;
	}
	else
	{
		width = m_mouthTextureWidth;
		height = m_mouthTextureHeight;
	}

	m_pRenderer->PushMatrix();

	if (transparency)
	{
		m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	}

	// Store cull mode
	CullMode cullMode = m_pRenderer->GetCullMode();

	if (wireframe)
	{
		m_pRenderer->SetLineWidth(1.0f);
		m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
		m_pRenderer->SetCullMode(CullMode::NOCULL);
	}
	else
	{
		m_pRenderer->SetRenderMode(RenderMode::TEXTURED);
	}

	if (eyesTexture)
	{
		m_pRenderer->BindTexture(m_faceEyesTexture);
	}
	else
	{
		m_pRenderer->BindTexture(m_faceMouthTexture);
	}

	if (transparency)
	{
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, m_characterAlpha);
	}
	else
	{
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	}

	m_pRenderer->EnableMaterial(m_pVoxelModel->GetMaterial());

	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
	
	m_pRenderer->ImmediateTextureCoordinate(0.0f, textureHeight);
	m_pRenderer->ImmediateVertex(0.0f, 0.0f, 0.0f);

	m_pRenderer->ImmediateTextureCoordinate(textureWidth, textureHeight);
	m_pRenderer->ImmediateVertex(width, 0.0f, 0.0f);

	m_pRenderer->ImmediateTextureCoordinate(textureWidth, 0.0f);
	m_pRenderer->ImmediateVertex(width, height, 0.0f);

	m_pRenderer->ImmediateTextureCoordinate(0.0f, 0.0f);
	m_pRenderer->ImmediateVertex(0.0f, height, 0.0f);

	m_pRenderer->DisableImmediateMode();
	m_pRenderer->DisableTexture();

	if (transparency)
	{
		m_pRenderer->DisableTransparency();
	}

	// Restore cull mode
	m_pRenderer->SetCullMode(cullMode);
	m_pRenderer->PopMatrix();
}

void VoxelCharacter::RenderWeapons(bool renderOutline, bool reflection, bool silhouette, Color outlineColor) const
{
	if (m_pLeftWeapon != nullptr)
	{
		if (m_isLeftWeaponLoaded)
		{
			if (m_renderLeftWeapon)
			{
				m_pRenderer->PushMatrix();
				
				m_pRenderer->ScaleWorldMatrix(m_characterScale, m_characterScale, m_characterScale);
				m_pLeftWeapon->Render(renderOutline, reflection, silhouette, outlineColor);
				
				m_pRenderer->PopMatrix();
			}
		}
	}

	if (m_pRightWeapon != nullptr)
	{
		if (m_isRightWeaponLoaded)
		{
			if (m_renderRightWeapon)
			{
				m_pRenderer->PushMatrix();

				m_pRenderer->ScaleWorldMatrix(m_characterScale, m_characterScale, m_characterScale);
				m_pRightWeapon->Render(renderOutline, reflection, silhouette, outlineColor);
				
				m_pRenderer->PopMatrix();
			}
		}
	}
}

void VoxelCharacter::RenderWeaponTrails() const
{
	if (m_pLeftWeapon != nullptr)
	{
		if (m_isLeftWeaponLoaded)
		{
			if (m_renderLeftWeapon)
			{
				m_pRenderer->PushMatrix();

				m_pLeftWeapon->RenderWeaponTrails();

				m_pRenderer->PopMatrix();
			}
		}
	}

	if (m_pRightWeapon != nullptr)
	{
		if (m_isRightWeaponLoaded)
		{
			if (m_renderRightWeapon)
			{
				m_pRenderer->PushMatrix();

				m_pRightWeapon->RenderWeaponTrails();

				m_pRenderer->PopMatrix();
			}
		}
	}
}

void VoxelCharacter::RenderPaperdoll()
{
	if (m_pVoxelModel != nullptr)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->ScaleWorldMatrix(0.08f, 0.08f, 0.08f);
		m_pVoxelModel->RenderPaperdoll(m_pCharacterAnimatorPaperdollLeft, m_pCharacterAnimatorPaperdollRight, this);
		
		m_pRenderer->PopMatrix();
	}
}

void VoxelCharacter::RenderPortrait()
{
	if (m_pVoxelModel != nullptr)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->ScaleWorldMatrix(0.08f, 0.08f, 0.08f);
		m_pVoxelModel->RenderPortrait(m_pCharacterAnimatorPaperdollRight, this, "Head");
		m_pVoxelModel->RenderPortrait(m_pCharacterAnimatorPaperdollRight, this, "Helm");

		m_pRenderer->PopMatrix();
	}
}

void VoxelCharacter::RenderFacePaperdoll()
{
	if (m_isLoadedFaces == false)
	{
		return;
	}

	if (m_pVoxelModel != nullptr)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->ScaleWorldMatrix(0.08f, 0.08f, 0.08f);
		m_pVoxelModel->RenderFace(m_pCharacterAnimatorPaperdollRight, this, true);

		m_pRenderer->PopMatrix();
	}
}

void VoxelCharacter::RenderFacePortrait()
{
	if (m_isLoadedFaces == false)
	{
		return;
	}

	if (m_pVoxelModel != nullptr)
	{
		m_pRenderer->PushMatrix();

		m_pRenderer->ScaleWorldMatrix(0.08f, 0.08f, 0.08f);
		// NOTE: Don't scale for portrait, we want a default size
		m_pVoxelModel->RenderFace(m_pCharacterAnimatorPaperdollRight, this, true, false, true);
		
		m_pRenderer->PopMatrix();
	}
}

void VoxelCharacter::RenderWeaponsPaperdoll() const
{
	if (m_pLeftWeapon != nullptr)
	{
		if (m_isLeftWeaponLoaded)
		{
			if (m_renderLeftWeapon)
			{
				m_pRenderer->PushMatrix();

				m_pRenderer->ScaleWorldMatrix(0.08f, 0.08f, 0.08f);
				m_pLeftWeapon->RenderPaperdoll();

				m_pRenderer->PopMatrix();
			}
		}
	}

	if (m_pRightWeapon != nullptr)
	{
		if (m_isRightWeaponLoaded)
		{
			if (m_renderRightWeapon)
			{
				m_pRenderer->PushMatrix();

				m_pRenderer->ScaleWorldMatrix(0.08f, 0.08f, 0.08f);
				m_pRightWeapon->RenderPaperdoll();

				m_pRenderer->PopMatrix();
			}
		}
	}
}

void VoxelCharacter::_BreathAnimationFinished(void* pData)
{
	VoxelCharacter* pVoxelCharacter = static_cast<VoxelCharacter*>(pData);
	pVoxelCharacter->BreathAnimationFinished();
}

void VoxelCharacter::BreathAnimationFinished()
{
	m_isBreathingAnimationStarted = false;
}