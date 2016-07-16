/*************************************************************************
> File Name: SoundEffects.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Sound effects.
> Created Time: 2016/07/16
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_SOUND_EFFECTS_H
#define CUBBY_SOUND_EFFECTS_H

#include <string>

enum class SoundEffect
{
	None,

	FootStep01,
	FootStep02,
	FootStep03,
	FootStep04,
	JumpLand,

	EquipCloth,
	EquipSword,
	EquipMove,

	ChestOpen,

	BowDraw,
	ArrowRelease,
	FireballCast,

	MimicJump,
	MimicDie,

	NumSoundEffect,
};

extern std::string g_soundEffectFileNames[static_cast<int>(SoundEffect::NumSoundEffect)];

#endif