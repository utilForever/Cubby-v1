/*************************************************************************
> File Name: MS3DAnimator.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Milk Shape 3D Animator.
> Created Time: 2016/06/29
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MS3DAnimator.h"

// Constructor, Destructor
MS3DAnimator::MS3DAnimator(Renderer* pRenderer, MS3DModel* pModel) :
	m_pRenderer(pRenderer), m_pModel(pModel),
	m_numJointAnimations(0), m_pJointAnimations(nullptr),
	m_numAnimations(0), m_pAnimations(nullptr),
	m_currentAnimationIndex(0), m_currentAnimationStartTime(0.0), m_currentAnimationEndTime(0.0),
	m_currentAnimationLeftWeaponTrailStartTime(0.0), m_currentAnimationLeftWeaponTrailEndTime(0.0),
	m_currentAnimationRightWeaponTrailStartTime(0.0), m_currentAnimationRightWeaponTrailEndTime(0.0),
	m_timer(0.0), m_isLooping(false), m_isPaused(false), m_isLooped(false), m_isFinished(false),
	m_isBlending(false), m_blendTime(0.0f), m_blendTimer(0.0f),
	m_blendStartAnimationIndex(0), m_blendEndAnimationIndex(0)
{
	// Once we have some model data, create out joint animations
	CreateJointAnimations();

	// Calculate the initial bounding box
	CalculateBoundingBox();
}

MS3DAnimator::~MS3DAnimator()
{
	m_numJointAnimations = 0;
	if (m_pJointAnimations != nullptr)
	{
		delete[] m_pJointAnimations;
		m_pJointAnimations = nullptr;
	}

	m_numAnimations = 0;
	if (m_pAnimations != nullptr)
	{
		delete[] m_pAnimations;
		m_pAnimations = nullptr;
	}
}

// Animation
MS3DModel* MS3DAnimator::GetModel() const
{
	return m_pModel;
}

void MS3DAnimator::CreateJointAnimations()
{
	m_numJointAnimations = m_pModel->m_numJoints;
	m_pJointAnimations = new JointAnimation[m_numJointAnimations];

	for (int i = 0; i < m_numJointAnimations; ++i)
	{
		m_pJointAnimations[i].currentTranslationKeyframe = 0;
		m_pJointAnimations[i].currentRotationKeyframe = 0;

		m_pJointAnimations[i].finalMatrix = m_pModel->m_pJoints[i].absolute;
	}
}

bool MS3DAnimator::LoadAnimations(const char *animationFileName)
{
	std::ifstream file;

	// Open the file
	file.open(animationFileName, std::ios::in);
	if (file.is_open())
	{
		std::string tempString;

		// Read in the number of animations
		file >> tempString >> m_numAnimations;

		// Create the animation storage space
		m_pAnimations = new Animation[m_numAnimations];

		// Read in each animation
		for (int i = 0; i < m_numAnimations; ++i)
		{
			// Animation name
			file >> tempString >> m_pAnimations[i].animationName;

			// Looping
			file >> tempString >> m_pAnimations[i].looping;

			// Start frame
			file >> tempString >> m_pAnimations[i].startFrame;

			// End frame
			file >> tempString >> m_pAnimations[i].endFrame;

			// Blend frame
			file >> tempString >> m_pAnimations[i].blendFrame;

			// Start right weapon trail
			file >> tempString >> m_pAnimations[i].startRightWeaponTrailFrame;

			// End right weapon trail
			file >> tempString >> m_pAnimations[i].endRightWeaponTrailFrame;

			// Start left weapon trail
			file >> tempString >> m_pAnimations[i].startLeftWeaponTrailFrame;

			// End left weapon trail
			file >> tempString >> m_pAnimations[i].endLeftWeaponTrailFrame;

			// Work out the start time and end time
			m_pAnimations[i].startTime = m_pAnimations[i].startFrame * 1000.0 / m_pModel->m_animationFPS;
			m_pAnimations[i].endTime = m_pAnimations[i].endFrame * 1000.0 / m_pModel->m_animationFPS;
			m_pAnimations[i].startRightWeaponTrailTime = m_pAnimations[i].startRightWeaponTrailFrame * 1000.0 / m_pModel->m_animationFPS;
			m_pAnimations[i].endRightWeaponTrailTime = m_pAnimations[i].endRightWeaponTrailFrame * 1000.0 / m_pModel->m_animationFPS;
			m_pAnimations[i].startLeftWeaponTrailTime = m_pAnimations[i].startLeftWeaponTrailFrame * 1000.0 / m_pModel->m_animationFPS;
			m_pAnimations[i].endLeftWeaponTrailTime = m_pAnimations[i].endLeftWeaponTrailFrame * 1000.0 / m_pModel->m_animationFPS;
		}

		// Close the file
		file.close();

		return true;
	}

	return false;
}

void MS3DAnimator::CalculateBoundingBox()
{
	for (int i = 0; i < m_pModel->m_numVertices; i++)
	{
		if (m_pModel->m_pVertices[i].boneID == -1)
		{
			// Don't use this vertex if it doesn't have a valid boneID, since this will mess up the matrix math
			continue;
		}

		Matrix4& finalMatrix = m_pJointAnimations[m_pModel->m_pVertices[i].boneID].finalMatrix;
		glm::vec3 newVertex(m_pModel->m_pVertices[i].location[0], m_pModel->m_pVertices[i].location[1], m_pModel->m_pVertices[i].location[2]);

		newVertex = finalMatrix * newVertex;

		float tempVertex[3];
		tempVertex[0] = newVertex.x;
		tempVertex[1] = newVertex.y;
		tempVertex[2] = newVertex.z;

		// Always set the first vertex values
		if (i == 0)
		{
			m_boundingBox.SetMinX(tempVertex[0]);
			m_boundingBox.SetMinY(tempVertex[1]);
			m_boundingBox.SetMinZ(tempVertex[2]);

			m_boundingBox.SetMaxX(tempVertex[0]);
			m_boundingBox.SetMaxY(tempVertex[1]);
			m_boundingBox.SetMaxZ(tempVertex[2]);
		}
		else
		{
			if (tempVertex[0] < m_boundingBox.GetMinX())
			{
				m_boundingBox.SetMinX(tempVertex[0]);
			}

			if (tempVertex[1] < m_boundingBox.GetMinY())
			{
				m_boundingBox.SetMinY(tempVertex[1]);
			}

			if (tempVertex[2] < m_boundingBox.GetMinZ())
			{
				m_boundingBox.SetMinZ(tempVertex[2]);
			}

			if (tempVertex[0] > m_boundingBox.GetMaxX())
			{
				m_boundingBox.SetMaxX(tempVertex[0]);
			}

			if (tempVertex[1] > m_boundingBox.GetMaxY())
			{
				m_boundingBox.SetMaxY(tempVertex[1]);
			}

			if (tempVertex[2] > m_boundingBox.GetMaxZ())
			{
				m_boundingBox.SetMaxZ(tempVertex[2]);
			}
		}
	}
}

BoundingBox* MS3DAnimator::GetBoundingBox()
{
	return& m_boundingBox;
}

void MS3DAnimator::PlayAnimation(int animationIndex)
{
	assert(animationIndex >= 0 && animationIndex < m_numAnimations);

	// Keep track of the current animation index
	m_currentAnimationIndex = animationIndex;

	// Set the animation params
	m_currentAnimationStartTime = m_pAnimations[animationIndex].startTime;
	m_currentAnimationEndTime = m_pAnimations[animationIndex].endTime;
	m_isLooping = m_pAnimations[animationIndex].looping;

	// Set the weapon trail start and end params
	m_currentAnimationRightWeaponTrailStartTime = m_pAnimations[animationIndex].startRightWeaponTrailTime;
	m_currentAnimationRightWeaponTrailEndTime = m_pAnimations[animationIndex].endRightWeaponTrailTime;
	m_currentAnimationLeftWeaponTrailStartTime = m_pAnimations[animationIndex].startLeftWeaponTrailTime;
	m_currentAnimationLeftWeaponTrailEndTime = m_pAnimations[animationIndex].endLeftWeaponTrailTime;

	// Reset the timer to the start of the animation
	Restart();

	m_isPaused = false;
	m_isLooped = false;
	m_isFinished = false;
	m_isBlending = false;
}

void MS3DAnimator::PlayAnimation(const char *animationName)
{
	int animationIndex = -1;
	for (int i = 0; i < m_numAnimations; ++i)
	{
		#ifdef _WIN32
		if (_strcmpi(animationName, m_pAnimations[i].animationName) == 0)
		#else
		if (strcasecmp(animationName, m_pAnimations[i].animationName) == 0)
		#endif //_WIN32
		{
			animationIndex = i;
			break;
		}
	}

	if (animationIndex >= 0 && animationIndex < m_numAnimations)
	{
		PlayAnimation(animationIndex);
	}
}

void MS3DAnimator::PauseAnimation()
{
	m_isPaused = true;
}

void MS3DAnimator::ResumeAnimation()
{
	m_isPaused = false;
}

int MS3DAnimator::GetCurrentAnimationIndex() const
{
	return m_currentAnimationIndex;
}

int MS3DAnimator::GetNumAnimations() const
{
	return m_numAnimations;
}

const char* MS3DAnimator::GetAnimationName(const int index) const
{
	if (index < 0 || index >= m_numAnimations)
	{
		return "";
	}

	return m_pAnimations[index].animationName;
}

bool MS3DAnimator::IsAnimationPaused() const
{
	return m_isPaused;
}

bool MS3DAnimator::HasAnimationLooped() const
{
	return m_isLooped;
}

bool MS3DAnimator::HasAnimationFinished() const
{
	return m_isFinished;
}

int MS3DAnimator::GetStartFrame(const char *animationName) const
{
	for (int i = 0; i < m_numAnimations; ++i)
	{
		#ifdef _WIN32
		if (_strcmpi(animationName, m_pAnimations[i].animationName) == 0)
		#else
		if (strcasecmp(animationName, m_pAnimations[i].animationName) == 0)
		#endif //_WIN32
		{
			return m_pAnimations[i].startFrame;
		}
	}

	return -1;
}

int MS3DAnimator::GetEndFrame(const char* animationName) const
{
	for (int i = 0; i < m_numAnimations; ++i)
	{
		#ifdef _WIN32
		if (_strcmpi(animationName, m_pAnimations[i].animationName) == 0)
		#else
		if (strcasecmp(animationName, m_pAnimations[i].animationName) == 0)
		#endif //_WIN32
		{
			return m_pAnimations[i].endFrame;
		}
	}

	return -1;
}

int MS3DAnimator::GetCurrentFrame() const
{
	Joint* pJoint = &(m_pModel->m_pJoints[0]);
	JointAnimation* pJointAnimation = &(m_pJointAnimations[0]);

	int frame = pJointAnimation->currentTranslationKeyframe;
	while (frame < pJoint->numTranslationKeyframes && pJoint->pTranslationKeyframes[frame].time <= m_timer)
	{
		frame++;
	}

	return frame;
}

float MS3DAnimator::GetAnimationTimer() const
{
	return static_cast<float>(m_timer);
}

int MS3DAnimator::GetNumKeyframes() const
{
	Joint* pJoint = &(m_pModel->m_pJoints[0]);
	int numFrames = pJoint->numTranslationKeyframes;

	return numFrames;
}

// Blending
void MS3DAnimator::StartBlendAnimation(int startIndex, int endIndex, float blendTime)
{
	m_isBlending = true;
	m_isPaused = false;
	m_isLooped = false;
	m_isFinished = false;
	m_blendTime = blendTime;
	m_blendTimer = 0.0f;
	m_blendStartAnimationIndex = startIndex;
	m_blendEndAnimationIndex = endIndex;

	double startTime = 0.0;

	if (startIndex != -1)
	{
		startTime = m_pAnimations[startIndex].startTime;
	}

	for (int i = 0; i < m_pModel->m_numJoints; ++i)
	{
		int frame;
		Joint* pJoint = &(m_pModel->m_pJoints[i]);
		JointAnimation* pJointAnimation = &(m_pJointAnimations[i]);

		if (pJoint->numRotationKeyframes == 0 && pJoint->numTranslationKeyframes == 0)
		{
			pJointAnimation->startBlendTrans[0] = pJoint->localTranslation[0];
			pJointAnimation->startBlendTrans[1] = pJoint->localTranslation[1];
			pJointAnimation->startBlendTrans[2] = pJoint->localTranslation[2];
			pJointAnimation->endBlendTrans[0] = pJoint->localTranslation[0];
			pJointAnimation->endBlendTrans[1] = pJoint->localTranslation[1];
			pJointAnimation->endBlendTrans[2] = pJoint->localTranslation[2];
			pJointAnimation->startBlendRot[0] = pJoint->localRotation[0];
			pJointAnimation->startBlendRot[1] = pJoint->localRotation[1];
			pJointAnimation->startBlendRot[2] = pJoint->localRotation[2];
			pJointAnimation->endBlendRot[0] = pJoint->localRotation[0];
			pJointAnimation->endBlendRot[1] = pJoint->localRotation[1];
			pJointAnimation->endBlendRot[2] = pJoint->localRotation[2];

			continue;
		}

		// Translation
		// Start
		if (startIndex == -1)
		{
			// We don't want to set the start trans values, since we will use the stored values of the current animation state
			pJointAnimation->startBlendTrans[0] = pJointAnimation->currentBlendTrans[0];
			pJointAnimation->startBlendTrans[1] = pJointAnimation->currentBlendTrans[1];
			pJointAnimation->startBlendTrans[2] = pJointAnimation->currentBlendTrans[2];
		}
		else
		{
			frame = 1;
			while (frame < pJoint->numTranslationKeyframes && pJoint->pTranslationKeyframes[frame].time < startTime)
			{
				frame++;
			}
			if (frame == pJoint->numTranslationKeyframes)
			{
				frame--;
			}

			if (pJoint->numTranslationKeyframes == 0)
			{
				pJointAnimation->startBlendTrans[0] = 0.0f;
				pJointAnimation->startBlendTrans[1] = 0.0f;
				pJointAnimation->startBlendTrans[2] = 0.0f;
			}
			else
			{
				memcpy(pJointAnimation->startBlendTrans, pJoint->pTranslationKeyframes[frame - 1].parameter, sizeof(float) * 3);
			}
		}

		// End
		frame = m_pAnimations[endIndex].blendFrame;

		if (pJoint->numTranslationKeyframes == 0)
		{
			pJointAnimation->endBlendTrans[0] = 0.0f;
			pJointAnimation->endBlendTrans[1] = 0.0f;
			pJointAnimation->endBlendTrans[2] = 0.0f;
		}
		else
		{
			memcpy(pJointAnimation->endBlendTrans, pJoint->pTranslationKeyframes[frame].parameter, sizeof(float) * 3);
		}

		// Rotation
		// Start
		if (startIndex == -1)
		{
			// We don't want to set the start rot values, since we will use the stored values of the current animation state
			pJointAnimation->startBlendRot[0] = pJointAnimation->currentBlendRot[0];
			pJointAnimation->startBlendRot[1] = pJointAnimation->currentBlendRot[1];
			pJointAnimation->startBlendRot[2] = pJointAnimation->currentBlendRot[2];
		}
		else
		{
			frame = 1;
			while (frame < pJoint->numRotationKeyframes && pJoint->pRotationKeyframes[frame].time < startTime)
			{
				frame++;
			}
			if (frame == pJoint->numRotationKeyframes)
			{
				frame--;
			}

			if (pJoint->numRotationKeyframes == 0)
			{
				pJointAnimation->startBlendRot[0] = 0.0f;
				pJointAnimation->startBlendRot[1] = 0.0f;
				pJointAnimation->startBlendRot[2] = 0.0f;
			}
			else
			{
				memcpy(pJointAnimation->startBlendRot, pJoint->pRotationKeyframes[frame - 1].parameter, sizeof(float) * 3);
			}
		}


		// End
		frame = m_pAnimations[endIndex].blendFrame;

		if (pJoint->numRotationKeyframes == 0)
		{
			pJointAnimation->endBlendRot[0] = 0.0f;
			pJointAnimation->endBlendRot[1] = 0.0f;
			pJointAnimation->endBlendRot[2] = 0.0f;
		}
		else
		{
			memcpy(pJointAnimation->endBlendRot, pJoint->pRotationKeyframes[frame].parameter, sizeof(float) * 3);
		}
	}
}

void MS3DAnimator::StartBlendAnimation(const char* startAnimationName, const char* endAnimationName, float blendTime)
{
	int startIndex = -1;
	int endIndex = -1;
	for (int i = 0; i < m_numAnimations; ++i)
	{
		#ifdef _WIN32
		if (_strcmpi(startAnimationName, m_pAnimations[i].animationName) == 0)
		#else
		if (strcasecmp(startAnimationName, m_pAnimations[i].animationName) == 0)
		#endif //_WIN32
		{
			startIndex = i;
		}

		#ifdef _WIN32
		if (_strcmpi(endAnimationName, m_pAnimations[i].animationName) == 0)
		#else
		if (strcasecmp(endAnimationName, m_pAnimations[i].animationName) == 0)
		#endif //_WIN32
		{
			endIndex = i;
		}
	}

	if (startIndex >= 0 && startIndex < m_numAnimations && endIndex >= 0 && endIndex < m_numAnimations)
	{
		StartBlendAnimation(startIndex, endIndex, blendTime);
	}
}

void MS3DAnimator::BlendIntoAnimation(const char* animationName, float blendTime)
{
	int index = -1;
	for (int i = 0; i < m_numAnimations; ++i)
	{
		#ifdef _WIN32
		if (_strcmpi(animationName, m_pAnimations[i].animationName) == 0)
		#else
		if (strcasecmp(animationName, m_pAnimations[i].animationName) == 0)
		#endif //_WIN32
		{
			index = i;
			break;
		}
	}

	if (index >= 0 && index < m_numAnimations)
	{
		StartBlendAnimation(-1, index, blendTime);
	}
}

void MS3DAnimator::GetCurrentBlendTranslation(int jointIndex, float* x, float* y, float* z) const
{
	JointAnimation* pJointAnimation = &(m_pJointAnimations[jointIndex]);
	*x = pJointAnimation->currentBlendTrans[0];
	*y = pJointAnimation->currentBlendTrans[1];
	*z = pJointAnimation->currentBlendTrans[2];
}

void MS3DAnimator::GetCurrentBlendRotation(int jointIndex, float* x, float* y, float* z) const
{
	JointAnimation* pJointAnimation = &(m_pJointAnimations[jointIndex]);
	*x = pJointAnimation->currentBlendRot[0];
	*y = pJointAnimation->currentBlendRot[1];
	*z = pJointAnimation->currentBlendRot[2];
}

bool MS3DAnimator::GetRightWeaponTrailActive() const
{
	return (m_timer > m_currentAnimationRightWeaponTrailStartTime) && (m_timer < m_currentAnimationRightWeaponTrailEndTime);
}

bool MS3DAnimator::GetLeftWeaponTrailActive() const
{
	return (m_timer > m_currentAnimationLeftWeaponTrailStartTime) && (m_timer < m_currentAnimationLeftWeaponTrailEndTime);
}

void MS3DAnimator::Restart()
{
	for (int i = 0; i < m_numJointAnimations; ++i)
	{
		m_pJointAnimations[i].currentRotationKeyframe = m_pJointAnimations[i].currentTranslationKeyframe = 0;
	}

	SetTimerForStartOfAnimation();
}

void MS3DAnimator::SetTimerForStartOfAnimation()
{
	m_timer = m_pAnimations[m_currentAnimationIndex].startTime;
}

Matrix4 MS3DAnimator::GetBoneMatrix(int index) const
{
	Matrix4& finalMatrix = m_pJointAnimations[index].finalMatrix;

	return finalMatrix;
}

// Update
void MS3DAnimator::Update(float dt)
{
	if (m_isBlending)
	{
		UpdateBlending(dt);
		return;
	}

	if (!m_isPaused)
	{
		m_timer += dt * 1000.0;
	}

	if (m_timer > m_currentAnimationEndTime)
	{
		if (m_isLooping)
		{
			Restart();
			m_timer = m_currentAnimationStartTime;

			m_isLooped = true;
		}
		else
		{
			m_timer = m_currentAnimationEndTime;

			m_isFinished = true;
		}
	}

	for (int i = 0; i < m_pModel->m_numJoints; i++)
	{
		float transVec[3];
		float rotVec[3];
		Matrix4 transform;
		int frame;
		Joint* pJoint = &(m_pModel->m_pJoints[i]);
		JointAnimation* pJointAnimation = &(m_pJointAnimations[i]);

		if (pJoint->numRotationKeyframes == 0 && pJoint->numTranslationKeyframes == 0)
		{
			pJointAnimation->finalMatrix = pJoint->absolute;

			continue;
		}

		// Translation
		frame = pJointAnimation->currentTranslationKeyframe;
		while (frame < pJoint->numTranslationKeyframes && pJoint->pTranslationKeyframes[frame].time < m_timer)
		{
			frame++;
		}
		pJointAnimation->currentTranslationKeyframe = frame;

		if (pJoint->numTranslationKeyframes == 0)
		{
			transVec[0] = 0.0f;
			transVec[1] = 0.0f;
			transVec[2] = 0.0f;
		}
		else
		{
			if (frame == 0)
			{
				memcpy(transVec, pJoint->pTranslationKeyframes[0].parameter, sizeof(float) * 3);
			}
			else if (frame == pJoint->numTranslationKeyframes)
			{
				memcpy(transVec, pJoint->pTranslationKeyframes[frame - 1].parameter, sizeof(float) * 3);
			}
			else
			{
				const Keyframe& curFrame = pJoint->pTranslationKeyframes[frame];
				const Keyframe& prevFrame = pJoint->pTranslationKeyframes[frame - 1];

				float timeDelta = curFrame.time - prevFrame.time;
				float interpValue = (m_timer - prevFrame.time) / timeDelta;

				transVec[0] = prevFrame.parameter[0] + (curFrame.parameter[0] - prevFrame.parameter[0]) * interpValue;
				transVec[1] = prevFrame.parameter[1] + (curFrame.parameter[1] - prevFrame.parameter[1]) * interpValue;
				transVec[2] = prevFrame.parameter[2] + (curFrame.parameter[2] - prevFrame.parameter[2]) * interpValue;
			}
		}

		// Rotation
		frame = pJointAnimation->currentRotationKeyframe;
		while (frame < pJoint->numRotationKeyframes && pJoint->pRotationKeyframes[frame].time < m_timer)
		{
			frame++;
		}
		pJointAnimation->currentRotationKeyframe = frame;

		if (pJoint->numRotationKeyframes == 0)
		{
			// Do nothing
		}
		else
		{
			if (frame == 0)
			{
				rotVec[0] = pJoint->pRotationKeyframes[0].parameter[0];
				rotVec[1] = pJoint->pRotationKeyframes[0].parameter[1];
				rotVec[2] = pJoint->pRotationKeyframes[0].parameter[2];

				transform.SetRotationRadians(rotVec);
			}
			else if (frame == pJoint->numRotationKeyframes)
			{
				rotVec[0] = pJoint->pRotationKeyframes[frame - 1].parameter[0];
				rotVec[1] = pJoint->pRotationKeyframes[frame - 1].parameter[1];
				rotVec[2] = pJoint->pRotationKeyframes[frame - 1].parameter[2];

				transform.SetRotationRadians(rotVec);
			}
			else
			{
				const Keyframe& curFrame = pJoint->pRotationKeyframes[frame];
				const Keyframe& prevFrame = pJoint->pRotationKeyframes[frame - 1];

				float timeDelta = curFrame.time - prevFrame.time;
				float interpValue = (m_timer - prevFrame.time) / timeDelta;

				glm::quat q1(glm::vec3(prevFrame.parameter[0], prevFrame.parameter[1], prevFrame.parameter[2]));
				glm::quat q2(glm::vec3(curFrame.parameter[0], curFrame.parameter[1], curFrame.parameter[2]));
				glm::quat q3 = slerp(q1, q2, interpValue);

				glm::mat4 trans = mat4_cast(q3);
				float* pM = value_ptr(trans);
				transform.SetValues(pM);

				// To preserve blending, since the matrix-to-angles functionality is broken
				rotVec[0] = prevFrame.parameter[0] + (curFrame.parameter[0] - prevFrame.parameter[0]) * interpValue;
				rotVec[1] = prevFrame.parameter[1] + (curFrame.parameter[1] - prevFrame.parameter[1]) * interpValue;
				rotVec[2] = prevFrame.parameter[2] + (curFrame.parameter[2] - prevFrame.parameter[2]) * interpValue;
			}
		}

		// Combine and create the final animation matrix
		transform.SetTranslation(transVec);

		Matrix4 relativeFinal(pJoint->relative);
		relativeFinal.PostMultiply(transform);

		if (pJoint->parent == -1)
		{
			pJointAnimation->finalMatrix = relativeFinal;
		}
		else
		{
			pJointAnimation->finalMatrix = m_pJointAnimations[pJoint->parent].finalMatrix;

			pJointAnimation->finalMatrix.PostMultiply(relativeFinal);
		}

		// Also store the current trans and rot values in the start blend variables, in case we want to start a new blend.
		pJointAnimation->currentBlendTrans[0] = transform.GetTranslationVector().x;
		pJointAnimation->currentBlendTrans[1] = transform.GetTranslationVector().y;
		pJointAnimation->currentBlendTrans[2] = transform.GetTranslationVector().z;
		pJointAnimation->currentBlendRot[0] = rotVec[0];
		pJointAnimation->currentBlendRot[1] = rotVec[1];
		pJointAnimation->currentBlendRot[2] = rotVec[2];
	}

	// Also recalculate the bounding box, since vertices *might* now have new positions, given that we have updated all the bones!
	CalculateBoundingBox();
}

void MS3DAnimator::UpdateBlending(float dt)
{
	if (!m_isPaused)
	{
		m_blendTimer += dt;
	}

	if (m_blendTimer > m_blendTime)
	{
		// Finished blending
		m_isBlending = false;
		PlayAnimation(m_blendEndAnimationIndex);
	}

	for (int i = 0; i < m_pModel->m_numJoints; ++i)
	{
		float transVec[3];
		float rotVec[3];
		Matrix4 transform;
		Joint *pJoint = &(m_pModel->m_pJoints[i]);
		JointAnimation *pJointAnimation = &(m_pJointAnimations[i]);

		float interpValue = static_cast<float>(m_blendTimer / m_blendTime);

		transVec[0] = pJointAnimation->startBlendTrans[0] + (pJointAnimation->endBlendTrans[0] - pJointAnimation->startBlendTrans[0])*interpValue;
		transVec[1] = pJointAnimation->startBlendTrans[1] + (pJointAnimation->endBlendTrans[1] - pJointAnimation->startBlendTrans[1])*interpValue;
		transVec[2] = pJointAnimation->startBlendTrans[2] + (pJointAnimation->endBlendTrans[2] - pJointAnimation->startBlendTrans[2])*interpValue;
		// To preserve blending, since the matrix-to-angles functionality is broken
		rotVec[0] = pJointAnimation->startBlendRot[0] + (pJointAnimation->endBlendRot[0] - pJointAnimation->startBlendRot[0])*interpValue;
		rotVec[1] = pJointAnimation->startBlendRot[1] + (pJointAnimation->endBlendRot[1] - pJointAnimation->startBlendRot[1])*interpValue;
		rotVec[2] = pJointAnimation->startBlendRot[2] + (pJointAnimation->endBlendRot[2] - pJointAnimation->startBlendRot[2])*interpValue;

		glm::quat q1(glm::vec3(pJointAnimation->startBlendRot[0], pJointAnimation->startBlendRot[1], pJointAnimation->startBlendRot[2]));
		glm::quat q2(glm::vec3(pJointAnimation->endBlendRot[0], pJointAnimation->endBlendRot[1], pJointAnimation->endBlendRot[2]));
		glm::quat q3 = slerp(q1, q2, interpValue);

		glm::mat4 trans = mat4_cast(q3);
		float* pM = value_ptr(trans);
		transform.SetValues(pM);

		transform.SetTranslation(transVec);

		Matrix4 relativeFinal(pJoint->relative);
		relativeFinal.PostMultiply(transform);

		if (pJoint->parent == -1)
		{
			pJointAnimation->finalMatrix = relativeFinal;
		}
		else
		{
			pJointAnimation->finalMatrix = m_pJointAnimations[pJoint->parent].finalMatrix;

			pJointAnimation->finalMatrix.PostMultiply(relativeFinal);
		}

		// Also store the current trans and rot values in the start blend variables, in case we want to start a new blend.
		pJointAnimation->currentBlendTrans[0] = transform.GetTranslationVector().x;
		pJointAnimation->currentBlendTrans[1] = transform.GetTranslationVector().y;
		pJointAnimation->currentBlendTrans[2] = transform.GetTranslationVector().z;
		pJointAnimation->currentBlendRot[0] = rotVec[0];
		pJointAnimation->currentBlendRot[1] = rotVec[1];
		pJointAnimation->currentBlendRot[2] = rotVec[2];
	}
}

// Rendering
void MS3DAnimator::Render(bool isMesh, bool isNormal, bool isBone, bool isBoundingBox)
{
	if (isMesh)
	{
		RenderMesh();
	}
	if (isNormal)
	{
		RenderNormals();
	}
	if (isBone)
	{
		RenderBones();
	}
	if (isBoundingBox)
	{
		RenderBoundingBox();
	}
}

void MS3DAnimator::RenderMesh() const
{
	// Draw by group
	for (int i = 0; i < m_pModel->m_numMeshes; ++i)
	{
		int materialIndex = m_pModel->m_pMeshes[i].materialIndex;
		m_pRenderer->BindTexture(m_pModel->m_pMaterials[materialIndex].texture);

		glBegin(GL_TRIANGLES);
		{
			for (int j = 0; j < m_pModel->m_pMeshes[i].numTriangles; ++j)
			{
				int triangleIndex = m_pModel->m_pMeshes[i].pTriangleIndices[j];
				const Triangle* pTri = &m_pModel->m_pTriangles[triangleIndex];

				for (int k = 0; k < 3; ++k)
				{
					int index = pTri->vertexIndices[k];

					if (m_pModel->m_pVertices[index].boneID == -1)
					{
						// Don't draw the foot steps!
					}
					else
					{
						// Rotate according to transformation matrix
						Matrix4& finalMatrix = m_pJointAnimations[m_pModel->m_pVertices[index].boneID].finalMatrix;

						// Texture coordinate
						glTexCoord2f(pTri->s[k], pTri->t[k]);

						// Normal
						glm::vec3 newNormal(pTri->vertexNormals[k][0], pTri->vertexNormals[k][1], pTri->vertexNormals[k][2]);

						newNormal = finalMatrix * newNormal;

						newNormal = normalize(newNormal);

						float tempNormal[3];
						tempNormal[0] = newNormal.x;
						tempNormal[1] = newNormal.y;
						tempNormal[2] = newNormal.z;
						glNormal3fv(tempNormal);

						// Vertex
						glm::vec3 newVertex(m_pModel->m_pVertices[index].location[0], m_pModel->m_pVertices[index].location[1], m_pModel->m_pVertices[index].location[2]);

						newVertex = finalMatrix * newVertex;

						float tempVertex[3];
						tempVertex[0] = newVertex.x;
						tempVertex[1] = newVertex.y;
						tempVertex[2] = newVertex.z;
						glVertex3fv(tempVertex);
					}
				}
			}
		}
		glEnd();

		m_pRenderer->DisableTexture();
	}
}

void MS3DAnimator::RenderNormals() const
{
	// Make the color cyan
	glColor3ub(0, 255, 255);

	glDisable(GL_LIGHTING);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);

	for (int i = 0; i < m_pModel->m_numMeshes; ++i)
	{
		glBegin(GL_LINES);
		{
			for (int j = 0; j < m_pModel->m_pMeshes[i].numTriangles; ++j)
			{
				int triangleIndex = m_pModel->m_pMeshes[i].pTriangleIndices[j];
				const Triangle* pTri = &m_pModel->m_pTriangles[triangleIndex];

				for (int k = 0; k < 3; ++k)
				{
					int index = pTri->vertexIndices[k];

					if (m_pModel->m_pVertices[index].boneID == -1)
					{
						// Don't draw the foot steps!
					}
					else
					{
						// Rotate according to transformation matrix
						Matrix4& finalMatrix = m_pModel->m_pJoints[m_pModel->m_pVertices[index].boneID].absolute;

						// Normal
						glm::vec3 newNormal(pTri->vertexNormals[k][0], pTri->vertexNormals[k][1], pTri->vertexNormals[k][2]);

						newNormal = finalMatrix * newNormal;

						newNormal = normalize(newNormal);
						newNormal *= 0.3f; // Scale normal down

						float tempNormal[3];
						tempNormal[0] = newNormal.x;
						tempNormal[1] = newNormal.y;
						tempNormal[2] = newNormal.z;

						// Vertex
						glm::vec3 newVertex(m_pModel->m_pVertices[index].location[0], m_pModel->m_pVertices[index].location[1], m_pModel->m_pVertices[index].location[2]);

						newVertex = finalMatrix * newVertex;

						float tempVertex[3];
						tempVertex[0] = newVertex.x;
						tempVertex[1] = newVertex.y;
						tempVertex[2] = newVertex.z;

						// Draw a line for the normal
						glVertex3f(tempVertex[0], tempVertex[1], tempVertex[2]);
						glVertex3f(tempVertex[0] + tempNormal[0], tempVertex[1] + tempNormal[1], tempVertex[2] + tempNormal[2]);
					}
				}
			}
		}
		glEnd();
	}
}

void MS3DAnimator::RenderBones() const
{
	// Make the color white
	glColor3ub(255, 255, 255);

	for (int i = 0; i < m_numJointAnimations; ++i)
	{
		glBegin(GL_LINES);
		{
			glm::vec3 newVertex;

			newVertex = m_pJointAnimations[i].finalMatrix * newVertex;

			float tempVertex[3];
			tempVertex[0] = newVertex.x;
			tempVertex[1] = newVertex.y;
			tempVertex[2] = newVertex.z;
			glVertex3fv(tempVertex);

			if (m_pModel->m_pJoints[i].parent != -1)
			{
				Matrix4& finalMatrix = m_pJointAnimations[m_pModel->m_pJoints[i].parent].finalMatrix;
				glm::vec3 newPVertex;

				newPVertex = finalMatrix * newPVertex;

				float tempPVertex[3];
				tempPVertex[0] = newPVertex.x;
				tempPVertex[1] = newPVertex.y;
				tempPVertex[2] = newPVertex.z;
				glVertex3fv(tempPVertex);
			}
		}

		glEnd();
	}
}

void MS3DAnimator::RenderBoundingBox()
{
	m_pRenderer->PushMatrix();
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);

	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

	// Bottom
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());

	// Top
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());

	// Sides
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMinZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMinZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMaxX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());

	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMinY(), m_boundingBox.GetMaxZ());
	m_pRenderer->ImmediateVertex(m_boundingBox.GetMinX(), m_boundingBox.GetMaxY(), m_boundingBox.GetMaxZ());

	m_pRenderer->DisableImmediateMode();
	m_pRenderer->PopMatrix();
}