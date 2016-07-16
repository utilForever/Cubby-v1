/*************************************************************************
> File Name: SoundManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A singleton instance used as an entry point to play sound.
>    (Music and sound effects)
> Created Time: 2016/07/16
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <iostream>

#include <fmod/include/fmod_errors.h>

#include "SoundManager.h"

// Initialize the singleton instance
SoundManager *SoundManager::m_instance = nullptr;

void ErrCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << "\n";
		exit(-1);
	}
}

// Constructor
SoundManager::SoundManager()
{
	SetEnableSound(true);
}

SoundManager* SoundManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new SoundManager;
		m_instance->Setup();
	}

	return m_instance;
}

void SoundManager::Destroy()
{
	if (m_instance)
	{
		Shutdown();
		delete m_instance;
	}
}

void SoundManager::Setup()
{
	m_FMODResult = System_Create(&m_FMODSystem);
	ErrCheck(m_FMODResult);

	m_FMODResult = m_FMODSystem->getVersion(&m_FMODVersion);
	ErrCheck(m_FMODResult);

	if (m_FMODVersion < FMOD_VERSION)
	{
		std::cout << "Error!  You are using an old version of FMOD " << m_FMODVersion << ".  This program requires " << FMOD_VERSION << "\n";
		return;
	}

	m_FMODResult = m_FMODSystem->init(32, FMOD_INIT_NORMAL, 0);
	ErrCheck(m_FMODResult);

	float distanceFactor = 1.0f;
	m_FMODResult = m_FMODSystem->set3DSettings(1.0f, distanceFactor, 1.0f);
	ErrCheck(m_FMODResult);
}

void SoundManager::Shutdown()
{
	m_FMODResult = m_FMODSystem->close();
	ErrCheck(m_FMODResult);

	m_FMODResult = m_FMODSystem->release();
	ErrCheck(m_FMODResult);
}

void SoundManager::Update(glm::vec3 listenerPos, glm::vec3 listenerForward, glm::vec3 listenerUp)
{
	FMOD_VECTOR pos = { listenerPos.x, listenerPos.y, listenerPos.z };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR forward = { listenerForward.x, listenerForward.y, listenerForward.z };
	FMOD_VECTOR up = { listenerUp.x, listenerUp.y, listenerUp.z };

	m_FMODResult = m_FMODSystem->set3DListenerAttributes(0, &pos, &vel, &forward, &up);
	ErrCheck(m_FMODResult);

	m_FMODResult = m_FMODSystem->update();
	ErrCheck(m_FMODResult);
}

void SoundManager::SetEnableSound(bool enable)
{
	m_isSoundEnabled = enable;
}

FMOD::Sound* SoundManager::PlaySound2D(FMOD::Channel** pChannel, const char* fileName, bool looping, bool stream)
{
	if (m_isSoundEnabled)
	{
		FMOD::Sound* sound;
		*pChannel = nullptr;
		FMOD::ChannelGroup *channelGroup = nullptr;

		if (stream)
		{
			m_FMODResult = m_FMODSystem->createStream(fileName, FMOD_DEFAULT | FMOD_2D, 0, &sound);
		}
		else
		{
			m_FMODResult = m_FMODSystem->createSound(fileName, FMOD_DEFAULT | FMOD_2D, 0, &sound);
		}
		ErrCheck(m_FMODResult);

		m_FMODResult = sound->setMode(looping == true ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		ErrCheck(m_FMODResult);

		m_FMODResult = m_FMODSystem->playSound(sound, channelGroup, false, pChannel);
		ErrCheck(m_FMODResult);

		return sound;
	}

	return nullptr;
}

FMOD::Sound* SoundManager::PlaySound3D(FMOD::Channel** pChannel, const char* fileName, glm::vec3 position, bool looping, bool stream)
{
	if (m_isSoundEnabled)
	{
		FMOD::Sound* sound;
		*pChannel = nullptr;
		FMOD::ChannelGroup *channelGroup = nullptr;

		if (stream)
		{
			m_FMODResult = m_FMODSystem->createStream(fileName, FMOD_DEFAULT | FMOD_3D, 0, &sound);
		}
		else
		{
			m_FMODResult = m_FMODSystem->createSound(fileName, FMOD_DEFAULT | FMOD_3D, 0, &sound);
		}
		ErrCheck(m_FMODResult);

		float distanceFactor = 1.0f;
		m_FMODResult = sound->set3DMinMaxDistance(1.0f * distanceFactor, 300.0f * distanceFactor);
		ErrCheck(m_FMODResult);

		m_FMODResult = sound->setMode(looping == true ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		ErrCheck(m_FMODResult);

		m_FMODResult = m_FMODSystem->playSound(sound, channelGroup, false, pChannel);
		ErrCheck(m_FMODResult);

		FMOD_VECTOR pos = { position.x, position.y, position.z };
		FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };

		m_FMODResult = (*pChannel)->set3DAttributes(&pos, &vel);
		ErrCheck(m_FMODResult);

		return sound;
	}

	return nullptr;
}

void SoundManager::StopSound(FMOD::Channel* pChannel) const
{
	if (m_isSoundEnabled)
	{
		pChannel->stop();
	}
}
