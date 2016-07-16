/*************************************************************************
> File Name: SoundManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A singleton instance used as an entry point to play sound.
>    (Music and sound effects)
> Created Time: 2016/07/16
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_SOUND_MANAGER_H
#define CUBBY_SOUND_MANAGER_H

#include "../Libraries/glm/vec3.hpp"

#include <fmod/include/fmod.hpp>

class SoundManager
{
public:
	static SoundManager* GetInstance();
	void Destroy();

	void Setup();
	void Shutdown();

	void Update(glm::vec3 listenerPos, glm::vec3 listenerForward, glm::vec3 listenerUp);

	void SetEnableSound(bool enable);

	FMOD::Sound* PlaySound2D(FMOD::Channel** pChannel, const char* fileName, bool isLooping, bool stream = false);
	FMOD::Sound* PlaySound3D(FMOD::Channel** pChannel, const char* fileName, glm::vec3 position, bool isLooping, bool stream = false);

	void StopSound(FMOD::Channel* pChannel) const;

protected:
	// Constructor
	SoundManager();

private:
	FMOD::System *m_FMODSystem;

	FMOD_RESULT m_FMODResult;
	unsigned int m_FMODVersion;

	bool m_isSoundEnabled;

	// Singleton instance
	static SoundManager* m_instance;
};

#endif