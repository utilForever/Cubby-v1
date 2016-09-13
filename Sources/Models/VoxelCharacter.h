/*************************************************************************
> File Name: VoxelCharacter.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Voxel character.
> Created Time: 2016/07/02
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_VOXEL_CHARACTER_H
#define CUBBY_VOXEL_CHARACTER_H

#include "QubicleBinaryManager.h"
#include "VoxelWeapon.h"

// Facial expression
typedef struct FacialExpression
{
	std::string facialExpressionName;
	std::string eyesTextureFile;
	std::string mouthTextureFile;
	unsigned int eyeTexture;
	unsigned int mouthTexture;
} FacialExpression;

// Talking animation
typedef struct TalkingAnimation
{
	std::string talkingAnimationTextureFile;
	unsigned int talkingAnimationTexture;
} TalkingAnimation;

// Forward declaration
class VoxelWeapon;

enum class AnimationSections
{
	FullBody = 0,
	HeadBody,
	LegsFeet,
	LeftArmHand,
	RightArmHand,
	NumSections,
};

class VoxelCharacter
{
public:
	// Constructor, Destructor
	VoxelCharacter(Renderer* pRenderer, QubicleBinaryManager* pQubicleBinaryManager);
	~VoxelCharacter();

	void Reset();

	void LoadVoxelCharacter(const char* characterType, const char* qbFileName, const char* modelFileName, const char* animatorFileName, const char* facesFileName, const char* characterFileName, const char* charactersBaseFolder, bool useQubicleManager = true);
	void SaveVoxelCharacter(const char* qbFileName, const char* facesFileName, const char* characterFileName) const;
	void UnloadCharacter();

	// Rebuild
	void RebuildVoxelModel(bool faceMerge) const;

	// Faces
	bool LoadFaces(const char* characterType, const char* facesFileName, const char* charactersBaseFolder);
	bool SaveFaces(const char* facesFileName) const;
	void SetupFacesBones();
	void ModifyEyesTextures(const char* charactersBaseFolder, const char* characterType, const char* eyeTextureFolder);

	// Character file
	void LoadCharacterFile(const char* characterFileName);
	void SaveCharacterFile(const char* characterFileName) const;
	void ResetMatrixParamsFromCharacterFile(const char* characterFileName, const char* matrixToReset) const;

	// Character scale
	void SetCharacterScale(float scale);
	float GetCharacterScale() const;

	// Character alpha
	float GetCharacterAlpha() const;

	// Upper body and head tilt look, for camera rotation
	void SetHeadAndUpperBodyLookRotation(float lookRotationAngle, float lookTranslate);
	float GetHeadAndUpperBodyLookRotation() const;
	float GetHeadAndUpperBodyLookTranslate() const;

	void SetCharacterMatrixRenderParams(const char* matrixName, float scale, float xOffset, float yOffset, float zOffset) const;
	float GetBoneMatrixRenderScale(const char* matrixName) const;
	glm::vec3 GetBoneMatrixRenderOffset(const char* matrixName) const;

	// Weapons
	void LoadRightWeapon(const char* weaponFileName);
	void LoadLeftWeapon(const char* weaponFileName);
	VoxelWeapon* GetRightWeapon() const;
	VoxelWeapon* GetLeftWeapon() const;
	void UnloadRightWeapon();
	void UnloadLeftWeapon();
	bool IsRightWeaponLoaded() const;
	bool IsLeftWeaponLoaded() const;

	// Setup animator and bones
	void SetUpdateAnimator(bool update);
	Matrix4 GetBoneMatrix(AnimationSections section, int index) const;
	Matrix4 GetBoneMatrix(AnimationSections section, const char* boneName) const;
	Matrix4 GetBoneMatrixPaperdoll(int index, bool left) const;
	int GetBoneIndex(const char* boneName) const;
	int GetMatrixIndexForName(const char* matrixName) const;
	MS3DModel* GetMS3DModel() const;
	MS3DAnimator* GetMS3DAnimator(AnimationSections section) const;
	QubicleBinary* GetQubicleModel() const;
	glm::vec3 GetBoneScale() const;
	void SetBoneScale(float scale);

	// Rendering modes
	void SetWireFrameRender(bool wireframe) const;
	void SetRenderRightWeapon(bool render);
	void SetRenderLeftWeapon(bool render);
	void SetMeshAlpha(float alpha, bool force = false);
	void SetMeshSingleColor(float r, float g, float b) const;

	// Breathing animation
	void SetBreathingAnimationEnabled(bool enable);
	bool IsBreathingAnimationEnabled() const;
	bool IsBreathingAnimationStarted() const;
	void StartBreathAnimation();
	float GetBreathingAnimationOffsetForBone(int boneIndex) const;

	// Facial expressions
	int GetNumFacialExpressions() const;
	const char* GetFacialExpressionName(const int index) const;
	glm::vec3 GetEyesOffset() const;
	glm::vec3 GetMouthOffset() const;
	void SetEyesOffset(glm::vec3 offset);
	void SetMouthOffset(glm::vec3 offset);
	void PlayFacialExpression(const char* facialExpressionName);
	void PlayFacialExpression(int facialAnimationIndex);
	int GetCurrentFacialAnimation() const;
	void SetEyesBone(std::string eyesBoneName);
	void SetMouthBone(std::string mouthBoneName);
	std::string GetEyesBoneName() const;
	std::string GetMouthBoneName() const;
	int GetEyesBone() const;
	int GetMouthBone() const;
	int GetEyesMatrixIndex() const;
	int GetMouthMatrixIndex() const;
	void SetEyesTextureSize(float width, float height);
	void SetMouthTextureSize(float width, float height);
	float GetEyeTextureWidth() const;
	float GetEyeTextureHeight() const;
	float GetMouthTextureWidth() const;
	float GetMouthTextureHeight() const;

	// Face looking
	glm::vec3 GetFaceLookingDirection() const;
	void SetFaceLookingDirection(glm::vec3 looking);
	glm::vec3 GetFaceTargetDirection() const;
	void SetFaceTargetDirection(glm::vec3 target);
	float GetFaceLookToTargetSpeedMultiplier() const;
	void SetFaceLookToTargetSpeedMultiplier(float speedMultiplier);
	int GetHeadBoneIndex() const;
	int GetBodyBoneIndex() const;
	int GetLeftShoulderBoneIndex() const;
	int GetLeftHandBoneIndex() const;
	int GetRightShoulderBoneIndex() const;
	int GetRightHandBoneIndex() const;
	int GetLegsBoneIndex() const;
	int GetRightFootBoneIndex() const;
	int GetLeftFootBoneIndex() const;
	void SetRandomLookDirection(bool enable);
	bool IsRandomLookDirectionEnabled() const;

	// Wink animation
	void SetWinkAnimationEnabled(bool enable);
	bool IsWinkAnimationEnabled() const;
	void UpdateWinkAnimation(float dt);

	// Talking animation
	void SetTalkingAnimationEnabled(bool enable);
	bool IsTalkingAnimationEnabled() const;
	void SetRandomMouthSelection(bool random);
	bool IsRandomMouthSelectionEnabled() const;
	void UpdateTalkingAnimation(float dt);

	// Animations
	int GetNumAnimations() const;
	const char* GetAnimationName(int index) const;
	void PlayAnimation(AnimationSections section, bool waitForComplete, AnimationSections syncWithSection, const char* animationName);
	int GetCurrentAnimationIndex(AnimationSections section) const;
	void SetBlendAnimation(AnimationSections section, bool waitForComplete, AnimationSections syncWithSection, const char* startAnimationName, const char* endAnimationName, float blendTime);
	void BlendIntoAnimation(AnimationSections section, bool waitForComplete, AnimationSections syncWithSection, const char* animationName, float blendTime);
	bool HasAnimationFinished(AnimationSections section) const;
	bool HasAnimationLooped(AnimationSections section) const;
	void StepAnimationFrame(float dt);
	int GetStartFrame(const char* animationName) const;
	int GetEndFrame(const char* animationName) const;
	int GetCurrentFrame() const;
	int GetNumJoints() const;
	Joint* GetJoint(int index) const;
	Joint* GetJoint(const char* jointName) const;
	void PlayAnimationOnPaperDoll(const char* animationName, bool isLeft) const;

	// Matrices
	int GetNumModelMatrices() const;
	const char* GetModelMatrixName(int index) const;

	// Swapping and adding new matrices
	void SwapBodyPart(const char* bodyPartName, QubicleMatrix* pMatrix, bool copyMatrixParams) const;
	void AddQubicleMatrix(QubicleMatrix* pNewMatrix, bool copyMatrixParams) const;
	void RemoveQubicleMatrix(const char* matrixName) const;
	void SetQubicleMatrixRender(const char* matrixName, bool render) const;

	// Sub selection of individual body parts
	std::string GetSubSelectionName(int pickingID) const;

	// Update
	void Update(float dt, float animationSpeed[static_cast<int>(AnimationSections::NumSections)]);
	void SetWeaponTrailsOriginMatrix(Matrix4 originMatrix);

	// Rendering
	void Render(bool renderOutline, bool reflection, bool silhouette, Color outlineColor, bool subSelectionNamePicking);
	void RenderSubSelection(std::string subSelection, bool renderOutline, bool silhouette, Color outlineColor);
	void RenderBones() const;
	void RenderFace();
	void RenderFacingDebug() const;
	void RenderFaceTextures(bool eyesTexture, bool wireframe, bool transparency) const;
	void RenderWeapons(bool renderOutline, bool reflection, bool silhouette, Color outlineColor) const;
	void RenderWeaponTrails() const;
	void RenderPaperdoll();
	void RenderPortrait();
	void RenderFacePaperdoll();
	void RenderFacePortrait();
	void RenderWeaponsPaperdoll() const;

protected:
	static void _BreathAnimationFinished(void* pData);
	void BreathAnimationFinished();

private:
	Renderer* m_pRenderer;
	QubicleBinaryManager* m_pQubicleBinaryManager;

	// Loaded flags
	bool m_isLoaded;
	bool m_isLoadedFaces;

	// Flag for updating the animator
	bool m_updateAnimator;
	
	// Flags to control weapon rendering
	bool m_renderRightWeapon;
	bool m_renderLeftWeapon;
	bool m_isRightWeaponLoaded;
	bool m_isLeftWeaponLoaded;

	// Character Scale
	float m_characterScale;

	// Character alpha
	float m_characterAlpha;

	// Bone scale
	glm::vec3 m_boneScale;

	// Tilt the character upper body and head, used for weapon rotation with camera look
	float m_lookRotationAngle;
	float m_lookTranslate;

	// Breathing animation
	bool m_isBreathingAnimationEnabled;
	bool m_isBreathingAnimationStarted;
	float m_breathingBodyYOffset;
	float m_breathingHandsYOffset;
	float m_breathingAnimationInitialWaitTime;

	// Facial expression	
	int m_numFacialExpressions;
	FacialExpression* m_pFacialExpressions;
	unsigned int m_faceEyesTexture;
	unsigned int m_faceMouthTexture;
	glm::vec3 m_eyesOffset;
	glm::vec3 m_mouthOffset;
	int m_currentFacialExpression;
	int m_eyesBone;
	int m_mouthBone;
	int m_eyesMatrixIndex;
	int m_mouthMatrixIndex;
	float m_eyesTextureWidth;
	float m_eyesTextureHeight;
	float m_mouthTextureWidth;
	float m_mouthTextureHeight;

	// Face looking
	glm::vec3 m_faceLookingDirection;
	glm::vec3 m_faceTargetDirection;
	float m_faceLookToTargetSpeedMultiplier;
	int m_headBoneIndex;
	int m_bodyBoneIndex;
	int m_leftShoulderBoneIndex;
	int m_leftHandBoneIndex;
	int m_rightShoulderBoneIndex;
	int m_rightHandBoneIndex;
	int m_legsBoneIndex;
	int m_rightFootBoneIndex;
	int m_leftFootBoneIndex;
	bool m_isRandomLookDirectionEnabled;

	// Wink animation
	bool m_isWinkAnimationEnabled;
	std::string m_winkTextureFileName;
	unsigned int m_faceEyesWinkTexture;
	bool m_wink;
	float m_winkWaitTimer;
	float m_winkStayTime;
	std::string m_eyesBoneName;
	std::string m_mouthBoneName;

	// Talking animation
	bool m_isTalkingAnimationEnabled;
	int m_numTalkingMouths;
	TalkingAnimation *m_pTalkingAnimations;
	int m_currentTalkingTexture;
	float m_talkingWaitTimer;
	float m_talkingWaitTime;
	bool m_randomMouthSelection;
	float m_talkingPauseTimer;
	float m_talkingPauseTime;
	int m_talkingPauseMouthCounter;
	int m_talkingPauseMouthAmount;

	// If we are using the qubicle manager we don't need to delete our QB after use
	bool m_usingQubicleManager;

	// Weapons
	VoxelWeapon* m_pRightWeapon;
	VoxelWeapon* m_pLeftWeapon;

	QubicleBinary* m_pVoxelModel;
	MS3DModel* m_pCharacterModel;
	MS3DAnimator* m_pCharacterAnimator[static_cast<int>(AnimationSections::NumSections)];
	MS3DAnimator* m_pCharacterAnimatorPaperdollLeft;
	MS3DAnimator* m_pCharacterAnimatorPaperdollRight;

	int m_currentFrame;
};

#endif