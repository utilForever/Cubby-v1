/*************************************************************************
> File Name: MS3DAnimator.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Milk Shape 3D Animator.
> Created Time: 2016/06/28
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MS3D_ANIMATOR_H
#define CUBBY_MS3D_ANIMATOR_H

#include <Renderer/Renderer.h>

#include "MS3DModel.h"

// Joint animation structure
typedef struct JointAnimation
{
	int currentTranslationKeyframe;
	int currentRotationKeyframe;

	float startBlendTrans[3];
	float endBlendTrans[3];
	float startBlendRot[3];
	float endBlendRot[3];
	float currentBlendTrans[3];
	float currentBlendRot[3];

	Matrix4 finalMatrix;

} JointAnimation;

// Animation structure
static const int MAX_ANIMATION_NAME = 64;

typedef struct Animation
{
	int startFrame;
	int endFrame;
	int blendFrame;
	double startTime;
	double endTime;
	bool looping;
	int startRightWeaponTrailFrame;
	int endRightWeaponTrailFrame;
	double startRightWeaponTrailTime;
	double endRightWeaponTrailTime;
	int startLeftWeaponTrailFrame;
	int endLeftWeaponTrailFrame;
	double startLeftWeaponTrailTime;
	double endLeftWeaponTrailTime;
	char animationName[MAX_ANIMATION_NAME];
} Animation;

class MS3DAnimator
{
public:
	// Constructor, Destructor
	MS3DAnimator(Renderer* pRenderer, MS3DModel* pModel);
	~MS3DAnimator();

	// Animation
	MS3DModel* GetModel() const;

	void CreateJointAnimations();

	bool LoadAnimations(const char* animationFileName);

	void CalculateBoundingBox();
	BoundingBox* GetBoundingBox();

	void PlayAnimation(int animationIndex);
	void PlayAnimation(const char* animationName);
	void PauseAnimation();
	void ResumeAnimation();

	int GetCurrentAnimationIndex() const;
	int GetNumAnimations() const;
	const char* GetAnimationName(const int index) const;
	bool IsAnimationPaused() const;
	bool HasAnimationLooped() const;
	bool HasAnimationFinished() const;

	int GetStartFrame(const char* animationName) const;
	int GetEndFrame(const char* animationName) const;
	int GetCurrentFrame() const;

	float GetAnimationTimer() const;
	int GetNumKeyframes() const;

	// Blending
	void StartBlendAnimation(int startIndex, int endIndex, float blendTime);
	void StartBlendAnimation(const char* startAnimationName, const char* endAnimationName, float blendTime);
	void BlendIntoAnimation(const char* animationName, float blendTime);

	void GetCurrentBlendTranslation(int jointIndex, float* x, float* y, float* z) const;
	void GetCurrentBlendRotation(int jointIndex, float* x, float* y, float* z) const;

	bool GetRightWeaponTrailActive() const;
	bool GetLeftWeaponTrailActive() const;

	void Restart();

	void SetTimerForStartOfAnimation();

	Matrix4 GetBoneMatrix(int index) const;

	// Update
	void Update(float dt);
	void UpdateBlending(float dt);

	// Rendering
	void Render(bool isMesh, bool isNormal, bool isBone, bool isBoundingBox);
	void RenderMesh() const;
	void RenderNormals() const;
	void RenderBones() const;
	void RenderBoundingBox();

private:
	Renderer* m_pRenderer;

	// The MS3D model for this animator
	MS3DModel* m_pModel;

	// Joint animations
	int m_numJointAnimations;
	JointAnimation* m_pJointAnimations;

	// Animations
	int m_numAnimations;
	Animation* m_pAnimations;

	// Current playing animation
	int m_currentAnimationIndex;
	double m_currentAnimationStartTime;
	double m_currentAnimationEndTime;

	// Current animation timers for the weapon trail start and stop times
	double m_currentAnimationLeftWeaponTrailStartTime;
	double m_currentAnimationLeftWeaponTrailEndTime;
	double m_currentAnimationRightWeaponTrailStartTime;
	double m_currentAnimationRightWeaponTrailEndTime;

	// Current animation timer and local looping variable
	double m_timer;
	bool m_isLooping;

	// Status flags
	bool m_isPaused;
	bool m_isLooped;
	bool m_isFinished;

	// Blending
	bool m_isBlending;
	float m_blendTime;
	double m_blendTimer;
	int m_blendStartAnimationIndex;
	int m_blendEndAnimationIndex;

	// Bounding box
	BoundingBox m_boundingBox;
};

#endif