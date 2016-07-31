/*************************************************************************
> File Name: EquipmentEnum.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    An enum list of all of the equipment in the game, used for equipment
>    tracking and equipment specific functionality.
> Created Time: 2016/07/21
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_EQUIPMENT_ENUM_H
#define CUBBY_EQUIPMENT_ENUM_H

enum class EquipmentType
{
	None = 0,

	// Weapons
	NormalPickaxe,
	Torch,
	Hammer,
	MageStaff,
	NecroStaff,
	PriestStaff,
	DruidStaff,
	TwoHandedSword,
	Boomerang,
	Bomb,
	KnifeLeft,
	KnifeRight,
	FireballHandLeft,
	FireballHandRight,
	Mace,
	Sickle,
	DragonBow,

	// Equipment
	BlacksmithGloves,
	NormalBoots,
	NormalGloves,
	NormalShoulders,
	RocketBoots,
	RegrowthBoots,

	// Wooden
	WoodenSword,
	WoodenShield,
	WoodenBow,
	WoodenHelm,
	WoodenArmor,
	WoodenPants,
	WoodenGloves,
	WoodenBoots,
	WoodenShoulders,

	// Cooper
	// TODO: Add equipments

	// Iron
	IronSword,
	IronShield,
	IronHelm,
	IronArmor,
	IronPants,
	IronGloves,
	IronBoots,
	IronShoulders,

	// Steel
	SteelHelm,
	SteelArmor,
	SteelPants,
	SteelGloves,
	SteelBoots,
	SteelShoulders,

	// Silver
	// TODO: Add equipments

	// Gold
	// TODO: Add equipments

	// Ash
	AshSword,
	AshHelm,
	AshArmor,
	AshPants,
	AshGloves,
	AshBoots,
	AshShoulders,

	// Bone
	BoneSword,
	BoneHelm,
	BoneArmor,
	BonePants,
	BoneGloves,
	BoneBoots,
	BoneShoulders,

	// Ice
	IceHelm,
	IceArmor,
	IcePants,
	IceGloves,
	IceBoots,
	IceShoulders,

	// Spike
	SpikeHelm,
	SpikeArmor,
	SpikePants,
	SpikeGloves,
	SpikeBoots,
	SpikeShoulders,

	NUM_EQUIPMENT,
};

#endif