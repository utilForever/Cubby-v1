/*************************************************************************
> File Name: MS3DAnimator.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Milk Shape 3D Animator.
> Created Time: 2016/06/28
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <fstream>

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
	return &m_boundingBox;
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