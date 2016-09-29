/*************************************************************************
> File Name: SoundEffects.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Sound effects.
> Created Time: 2016/07/16
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "SoundEffects.h"

std::string g_soundEffectFileNames[static_cast<int>(SoundEffect::NumSoundEffect)] =
{
	"",						// None

	"footstep_01.wav",		// FootStep01
	"footstep_02.wav",		// FootStep02
	"footstep_03.wav",		// FootStep03
	"footstep_04.wav",		// FootStep04
	"land.wav",				// JumpLand

	"equip_cloth.wav",		// EquipCloth
	"equip_sword.wav",		// EquipSword
	"equip_move.wav",		// EquipMove

	"open_chest.wav",		// ChestOpen

	"bow_draw.wav",			// BowDraw
	"arrowRelease.wav",		// ArrowRelease
	"fireball_cast.wav",	// FireballCast

	"mimic_jump.wav",		// MimicJump
	"mimic_dropped.wav",	// MimicDie
};