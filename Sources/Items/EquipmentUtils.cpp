/*************************************************************************
> File Name: ItemUtils.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Utility functionality for equipment, mostly a way to associate equipment
>    enums with data like the friendly names and display strings.
> Created Time: 2016/07/31
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Item.h"

std::string GetEquipmentTitleForType(EquipmentType type)
{
	switch (type)
	{
	// Weapons
	case EquipmentType::None: { return ""; }
	case EquipmentType::NormalPickaxe: { return "Normal Pickaxe"; }
	case EquipmentType::Torch: { return "Torch"; }
	case EquipmentType::Hammer: { return "Hammer"; }
	case EquipmentType::MageStaff: { return "Mage Staff"; }
	case EquipmentType::NecroStaff: { return "Necro Staff"; }
	case EquipmentType::PriestStaff: { return "Priest Staff"; }
	case EquipmentType::DruidStaff: { return "Druid Staff"; }
	case EquipmentType::TwoHandedSword: { return "Two Handed Sword"; }
	case EquipmentType::Boomerang: { return "Boomerang"; }
	case EquipmentType::Bomb: { return "Bomb"; }
	case EquipmentType::KnifeLeft: { return "Knife"; }
	case EquipmentType::KnifeRight: { return "Knife"; }
	case EquipmentType::FireballHandLeft: { return "Fireball Hands"; }
	case EquipmentType::FireballHandRight: { return "Fireball Hands"; }
	case EquipmentType::Mace: { return "Mace"; }
	case EquipmentType::Sickle: { return "Sickle"; }
	case EquipmentType::DragonBow: { return "Dragon Bow"; }
	// Equipment
	case EquipmentType::BlacksmithGloves: { return "Blacksmith Gloves"; }
	case EquipmentType::NormalBoots: { return "Normal Boots"; }
	case EquipmentType::NormalGloves: { return "Normal Gloves"; }
	case EquipmentType::NormalShoulders: { return "Normal Shoulders"; }
	case EquipmentType::RocketBoots: { return "Rocket Boots"; }
	case EquipmentType::RegrowthBoots: { return "Regrowth Boots"; }
	// Wood
	case EquipmentType::WoodenSword: { return "Wooden Sword"; }
	case EquipmentType::WoodenShield: { return "Wooden Shield"; }
	case EquipmentType::WoodenBow: { return "Wooden Bow"; }
	case EquipmentType::WoodenHelm: { return "Wooden Helm"; }
	case EquipmentType::WoodenArmor: { return "Wooden Armor"; }
	case EquipmentType::WoodenPants: { return "Wooden Pants"; }
	case EquipmentType::WoodenGloves: { return "Wooden Gloves"; }
	case EquipmentType::WoodenBoots: { return "Wooden Boots"; }
	case EquipmentType::WoodenShoulders: { return "Wooden Shoulders"; }
	// Iron
	case EquipmentType::IronSword: { return "Iron Sword"; }
	case EquipmentType::IronShield: { return "Iron Shield"; }
	case EquipmentType::IronHelm: { return "Iron Helm"; }
	case EquipmentType::IronArmor: { return "Iron Armor"; }
	case EquipmentType::IronPants: { return "Iron Pants"; }
	case EquipmentType::IronGloves: { return "Iron Gloves"; }
	case EquipmentType::IronBoots: { return "Iron Boots"; }
	case EquipmentType::IronShoulders: { return "Iron Shoulders"; }
	// Steel
	case EquipmentType::SteelHelm: { return "Steel Helm"; }
	case EquipmentType::SteelArmor: { return "Steel Armor"; }
	case EquipmentType::SteelPants: { return "Steel Pants"; }
	case EquipmentType::SteelGloves: { return "Steel Gloves"; }
	case EquipmentType::SteelBoots: { return "Steel Boots"; }
	case EquipmentType::SteelShoulders: { return "Steel Shoulders"; }
	// Ash
	case EquipmentType::AshHelm: { return "Ash Helm"; }
	case EquipmentType::AshSword: { return "Ash Sword"; }
	case EquipmentType::AshArmor: { return "Ash Armor"; }
	case EquipmentType::AshPants: { return "Ash Pants"; }
	case EquipmentType::AshGloves: { return "Ash Gloves"; }
	case EquipmentType::AshBoots: { return "Ash Boots"; }
	case EquipmentType::AshShoulders: { return "Ash Shoulders"; }
	// Bone
	case EquipmentType::BoneSword: { return "Bone Edge"; }
	case EquipmentType::BoneHelm: { return "Bone Helm"; }
	case EquipmentType::BoneArmor: { return "Bone Armor"; }
	case EquipmentType::BonePants: { return "Bone Pants"; }
	case EquipmentType::BoneGloves: { return "Bone Gloves"; }
	case EquipmentType::BoneBoots: { return "Bone Boots"; }
	case EquipmentType::BoneShoulders: { return "Bone Shoulders"; }
	// Ice
	case EquipmentType::IceHelm: { return "Ice Helm"; }
	case EquipmentType::IceArmor: { return "Ice Armor"; }
	case EquipmentType::IcePants: { return "Ice Pants"; }
	case EquipmentType::IceGloves: { return "Ice Gloves"; }
	case EquipmentType::IceBoots: { return "Ice Boots"; }
	case EquipmentType::IceShoulders: { return "Ice Shoulders"; }
	// Spike
	case EquipmentType::SpikeHelm: { return "Spike Helm"; }
	case EquipmentType::SpikeArmor: { return "Spike Armor"; }
	case EquipmentType::SpikePants: { return "Spike Pants"; }
	case EquipmentType::SpikeGloves: { return "Spike Gloves"; }
	case EquipmentType::SpikeBoots: { return "Spike Boots"; }
	case EquipmentType::SpikeShoulders: { return "Spike Shoulders"; }
	case EquipmentType::NUM_EQUIPMENT: throw std::logic_error("Invalid EquipmentType!");
	default: throw std::logic_error("Invalid EquipmentType!");
	}
}

std::string GetEquipmentDescriptionForType(EquipmentType type)
{
	switch (type)
	{
	case EquipmentType::None: { return ""; }
	// Weapons
	case EquipmentType::NormalPickaxe: { return "Used for mining and digging the world."; }
	case EquipmentType::Torch: { return "A torch to light up the darkness."; }
	case EquipmentType::Hammer: { return "A hammer to smite your foes."; }
	case EquipmentType::MageStaff: { return "A staff that contains powerful magic."; }
	case EquipmentType::NecroStaff: { return "A necromancer staff once used by powerful mages of undeath."; }
	case EquipmentType::PriestStaff: { return "A holy staff of pure energy."; }
	case EquipmentType::DruidStaff: { return "A staff that is engraved with nature."; }
	case EquipmentType::TwoHandedSword: { return "A sword that requires both hands to wield."; }
	case EquipmentType::Boomerang: { return "Once thrown, the boomerang returns to you."; }
	case EquipmentType::Bomb: { return "An explosive bomb, can destroy enemies and the landscape."; }
	case EquipmentType::KnifeLeft: { return "A stabbing knife."; }
	case EquipmentType::KnifeRight: { return "A stabbing knife."; }
	case EquipmentType::FireballHandLeft: { return "Cast powerful spells using only your hands."; }
	case EquipmentType::FireballHandRight: { return "Cast powerful spells using only your hands."; }
	case EquipmentType::Mace: { return "A deadly mace, to smite down your enemies."; }
	case EquipmentType::Sickle: { return "Very sharp and very deadly."; }
	case EquipmentType::DragonBow: { return "Once used by the dragon riders of old Vox."; }
	// Equipment
	case EquipmentType::BlacksmithGloves: { return "Blacksmith gloves are used during the crafting of items."; }
	case EquipmentType::NormalBoots: { return "Normal boots."; }
	case EquipmentType::NormalGloves: { return "Normal gloves."; }
	case EquipmentType::NormalShoulders: { return "Normal shoulders."; }
	case EquipmentType::RocketBoots: { return "Rocket boots."; }
	case EquipmentType::RegrowthBoots: { return "Regrowth boots."; }
	// Wood
	case EquipmentType::WoodenSword: { return "A wooden sword, not very good in combat."; }
	case EquipmentType::WoodenShield: { return "A wooden shield, cannnot protect against much."; }
	case EquipmentType::WoodenBow: { return "A wooden bow, made for shooting arrows."; }
	case EquipmentType::WoodenHelm: { return "Wooden helm."; }
	case EquipmentType::WoodenArmor: { return "Wooden armor."; }
	case EquipmentType::WoodenPants: { return "Wooden pants."; }
	case EquipmentType::WoodenGloves: { return "Wooden gloves."; }
	case EquipmentType::WoodenBoots: { return "Wooden boots."; }
	case EquipmentType::WoodenShoulders: { return "Wooden shoulders."; }
	// Iron
	case EquipmentType::IronSword: { return "A sturdy sword made of iron."; }
	case EquipmentType::IronShield: { return "A shield of pure iron."; }
	case EquipmentType::IronHelm: { return "Iron helm."; }
	case EquipmentType::IronArmor: { return "Iron armor."; }
	case EquipmentType::IronPants: { return "Iron pants."; }
	case EquipmentType::IronGloves: { return "Iron gloves."; }
	case EquipmentType::IronBoots: { return "Iron boots."; }
	case EquipmentType::IronShoulders: { return "Iron shoulders."; }
	// Steel
	case EquipmentType::SteelHelm: { return "Steel helm."; }
	case EquipmentType::SteelArmor: { return "Steel armor."; }
	case EquipmentType::SteelPants: { return "Steel pants."; }
	case EquipmentType::SteelGloves: { return "Steel gloves."; }
	case EquipmentType::SteelBoots: { return "Steel boots."; }
	case EquipmentType::SteelShoulders: { return "Steel shoulders."; }
	// Ash
	case EquipmentType::AshSword: { return "Ash sword."; }
	case EquipmentType::AshHelm: { return "Ash helm."; }
	case EquipmentType::AshArmor: { return "Ash armor."; }
	case EquipmentType::AshPants: { return "Ash pants."; }
	case EquipmentType::AshGloves: { return "Ash gloves."; }
	case EquipmentType::AshBoots: { return "Ash boots."; }
	case EquipmentType::AshShoulders: { return "Ash shoulders."; }
	// Bone
	case EquipmentType::BoneSword: { return "A sword made entirely out of bones."; }
	case EquipmentType::BoneHelm: { return "Bone helm."; }
	case EquipmentType::BoneArmor: { return "Bone armor."; }
	case EquipmentType::BonePants: { return "Bone pants."; }
	case EquipmentType::BoneGloves: { return "Bone gloves."; }
	case EquipmentType::BoneBoots: { return "Bone boots."; }
	case EquipmentType::BoneShoulders: { return "Bone shoulders."; }
	// Ice
	case EquipmentType::IceHelm: { return "Ice helm."; }
	case EquipmentType::IceArmor: { return "Ice armor."; }
	case EquipmentType::IcePants: { return "Ice pants."; }
	case EquipmentType::IceGloves: { return "Ice gloves."; }
	case EquipmentType::IceBoots: { return "Ice boots."; }
	case EquipmentType::IceShoulders: { return "Ice shoulders."; }
	// Spike
	case EquipmentType::SpikeHelm: { return "Spike helm."; }
	case EquipmentType::SpikeArmor: { return "Spike armor."; }
	case EquipmentType::SpikePants: { return "Spike pants."; }
	case EquipmentType::SpikeGloves: { return "Spike gloves."; }
	case EquipmentType::SpikeBoots: { return "Spike boots."; }
	case EquipmentType::SpikeShoulders: { return "Spike shoulders."; }
	case EquipmentType::NUM_EQUIPMENT: throw std::logic_error("Invalid EquipmentType!");
	default: throw std::logic_error("Invalid EquipmentType!");
	}
}

std::string GetEquipmentFileNameForType(EquipmentType type)
{
	switch (type)
	{
	case EquipmentType::None: { return ""; }
	// Weapons
	case EquipmentType::NormalPickaxe: { return "Resources/gamedata/weapons/Pickaxe/Pickaxe.weapon"; }
	case EquipmentType::Torch: { return "Resources/gamedata/weapons/Torch/Torch.weapon"; }
	case EquipmentType::Hammer: { return "Resources/gamedata/weapons/Hammer/Hammer.weapon"; }
	case EquipmentType::MageStaff: { return "Resources/gamedata/weapons/Staff/Staff.weapon"; }
	case EquipmentType::NecroStaff: { return "Resources/gamedata/weapons/NecroStaff/NecroStaff.weapon"; }
	case EquipmentType::PriestStaff: { return "Resources/gamedata/weapons/PriestStaff/PriestStaff.weapon"; }
	case EquipmentType::DruidStaff: { return "Resources/gamedata/weapons/DruidStaff/DruidStaff.weapon"; }
	case EquipmentType::TwoHandedSword: { return "Resources/gamedata/weapons/TwoHandedSword/TWoHandedSword.weapon"; }
	case EquipmentType::Boomerang: { return "Resources/gamedata/weapons/Boomerang/Boomerang.weapon"; }
	case EquipmentType::Bomb: { return "Resources/gamedata/items/Bomb/Bomb.item"; }
	case EquipmentType::KnifeLeft: { return "Resources/gamedata/weapons/Knife/Knife.weapon"; }
	case EquipmentType::KnifeRight: { return "Resources/gamedata/weapons/Knife/Knife.weapon"; }
	case EquipmentType::FireballHandLeft: { return "Resources/gamedata/weapons/FireballHands/FireballHandsLeft.weapon"; }
	case EquipmentType::FireballHandRight: { return "Resources/gamedata/weapons/FireballHands/FireballHandsRight.weapon"; }
	case EquipmentType::Mace: { return "Resources/gamedata/weapons/Mace/Mace.weapon"; }
	case EquipmentType::Sickle: { return "Resources/gamedata/weapons/Sickle/Sickle.weapon"; }
	case EquipmentType::DragonBow: { return "Resources/gamedata/weapons/DragonBow/DragonBow.weapon"; }
	// Equipment
	case EquipmentType::BlacksmithGloves: { return "Resources/gamedata/equipment/BlacksmithGloves/BlacksmithGloves.equipment"; }
	case EquipmentType::NormalBoots: { return "Resources/gamedata/equipment/NormalBoots/NormalBoots.equipment"; }
	case EquipmentType::NormalGloves: { return "Resources/gamedata/equipment/NormalGloves/NormalGloves.equipment"; }
	case EquipmentType::NormalShoulders: { return "Resources/gamedata/equipment/NormalShoulders/NormalShoulders.equipment"; }
	case EquipmentType::RocketBoots: { return "Resources/gamedata/equipment/RocketBoots/RocketBoots.equipment"; }
	case EquipmentType::RegrowthBoots: { return "Resources/gamedata/equipment/RegrowthBoots/RegrowthBoots.equipment"; }
	// Wood
	case EquipmentType::WoodenSword: { return "Resources/gamedata/weapons/WoodenSword/WoodenSword.weapon"; }
	case EquipmentType::WoodenShield: { return "Resources/gamedata/weapons/WoodenShield/WoodenShield.weapon"; }
	case EquipmentType::WoodenBow: { return "Resources/gamedata/weapons/WoodenBow/WoodenBow.weapon"; }
	case EquipmentType::WoodenHelm: { return "Resources/gamedata/equipment/WoodHelm/WoodHelm.equipment"; }
	case EquipmentType::WoodenArmor: { return "Resources/gamedata/equipment/WoodBody/WoodBody.equipment"; }
	case EquipmentType::WoodenPants: { return "Resources/gamedata/equipment/WoodPants/WoodPants.equipment"; }
	case EquipmentType::WoodenGloves: { return "Resources/gamedata/equipment/WoodGloves/WoodGloves.equipment"; }
	case EquipmentType::WoodenBoots: { return "Resources/gamedata/equipment/WoodBoots/WoodBoots.equipment"; }
	case EquipmentType::WoodenShoulders: { return "Resources/gamedata/equipment/WoodShoulders/WoodShoulders.equipment"; }
	// Iron
	case EquipmentType::IronSword: { return "Resources/gamedata/weapons/IronSword/IronSword.weapon"; }
	case EquipmentType::IronShield: { return "Resources/gamedata/weapons/IronShield/IronShield.weapon"; }
	case EquipmentType::IronHelm: { return "Resources/gamedata/equipment/IronHelm/IronHelm.equipment"; }
	case EquipmentType::IronArmor: { return "Resources/gamedata/equipment/IronBody/IronBody.equipment"; }
	case EquipmentType::IronPants: { return "Resources/gamedata/equipment/IronPants/IronPants.equipment"; }
	case EquipmentType::IronGloves: { return "Resources/gamedata/equipment/IronGloves/IronGloves.equipment"; }
	case EquipmentType::IronBoots: { return "Resources/gamedata/equipment/IronBoots/IronBoots.equipment"; }
	case EquipmentType::IronShoulders: { return "Resources/gamedata/equipment/IronShoulders/IronShoulders.equipment"; }
	// Steel
	case EquipmentType::SteelHelm: { return "Resources/gamedata/equipment/SteelHelm/SteelHelm.equipment"; }
	case EquipmentType::SteelArmor: { return "Resources/gamedata/equipment/SteelBody/SteelBody.equipment"; }
	case EquipmentType::SteelPants: { return "Resources/gamedata/equipment/SteelPants/SteelPants.equipment"; }
	case EquipmentType::SteelGloves: { return "Resources/gamedata/equipment/SteelGloves/SteelGloves.equipment"; }
	case EquipmentType::SteelBoots: { return "Resources/gamedata/equipment/SteelBoots/SteelBoots.equipment"; }
	case EquipmentType::SteelShoulders: { return "Resources/gamedata/equipment/SteelShoulders/SteelShoulders.equipment"; }
	// Ash
	case EquipmentType::AshSword: { return "Resources/gamedata/weapons/AshSword/AshSword.weapon"; }
	case EquipmentType::AshHelm: { return "Resources/gamedata/equipment/AshHelm/AshHelm.equipment"; }
	case EquipmentType::AshArmor: { return "Resources/gamedata/equipment/AshBody/AshBody.equipment"; }
	case EquipmentType::AshPants: { return "Resources/gamedata/equipment/AshPants/AshPants.equipment"; }
	case EquipmentType::AshGloves: { return "Resources/gamedata/equipment/AshGloves/AshGloves.equipment"; }
	case EquipmentType::AshBoots: { return "Resources/gamedata/equipment/AshBoots/AshBoots.equipment"; }
	case EquipmentType::AshShoulders: { return "Resources/gamedata/equipment/AshShoulders/AshShoulders.equipment"; }
	// Bone
	case EquipmentType::BoneSword: { return "Resources/gamedata/weapons/BoneEdge/BoneEdge.weapon"; }
	case EquipmentType::BoneHelm: { return "Resources/gamedata/equipment/BoneHelm/BoneHelm.equipment"; }
	case EquipmentType::BoneArmor: { return "Resources/gamedata/equipment/BoneBody/BoneBody.equipment"; }
	case EquipmentType::BonePants: { return "Resources/gamedata/equipment/BonePants/BonePants.equipment"; }
	case EquipmentType::BoneGloves: { return "Resources/gamedata/equipment/BoneGloves/BoneGloves.equipment"; }
	case EquipmentType::BoneBoots: { return "Resources/gamedata/equipment/BoneBoots/BoneBoots.equipment"; }
	case EquipmentType::BoneShoulders: { return "Resources/gamedata/equipment/BoneShoulders/BoneShoulders.equipment"; }
	// Ice
	case EquipmentType::IceHelm: { return "Resources/gamedata/equipment/IceHelm/IceHelm.equipment"; }
	case EquipmentType::IceArmor: { return "Resources/gamedata/equipment/IceBody/IceBody.equipment"; }
	case EquipmentType::IcePants: { return "Resources/gamedata/equipment/IcePants/IcePants.equipment"; }
	case EquipmentType::IceGloves: { return "Resources/gamedata/equipment/IceGloves/IceGloves.equipment"; }
	case EquipmentType::IceBoots: { return "Resources/gamedata/equipment/IceBoots/IceBoots.equipment"; }
	case EquipmentType::IceShoulders: { return "Resources/gamedata/equipment/IceShoulders/IceShoulders.equipment"; }
	// Spike
	case EquipmentType::SpikeHelm: { return "Resources/gamedata/equipment/SpikeHelm/SpikeHelm.equipment"; }
	case EquipmentType::SpikeArmor: { return "Resources/gamedata/equipment/SpikeBody/SpikeBody.equipment"; }
	case EquipmentType::SpikePants: { return "Resources/gamedata/equipment/SpikePants/SpikePants.equipment"; }
	case EquipmentType::SpikeGloves: { return "Resources/gamedata/equipment/SpikeGloves/SpikeGloves.equipment"; }
	case EquipmentType::SpikeBoots: { return "Resources/gamedata/equipment/SpikeBoots/SpikeBoots.equipment"; }
	case EquipmentType::SpikeShoulders: { return "Resources/gamedata/equipment/SpikeShoulders/SpikeShoulders.equipment"; }
	case EquipmentType::NUM_EQUIPMENT: throw std::logic_error("Invalid EquipmentType!");
	default: throw std::logic_error("Invalid EquipmentType!");
	}
}

std::string GetEquipmentTextureForType(EquipmentType type)
{
	switch (type)
	{
	case EquipmentType::None: { return ""; }
	// Weapons
	case EquipmentType::NormalPickaxe: { return "Resources/textures/items/pickaxe.tga"; }
	case EquipmentType::Torch: { return "Resources/textures/items/torch.tga"; }
	case EquipmentType::Hammer: { return "Resources/textures/items/hammer.tga"; }
	case EquipmentType::MageStaff: { return "Resources/textures/items/staff_mage.tga"; }
	case EquipmentType::NecroStaff: { return "Resources/textures/items/staff_necro.tga"; }
	case EquipmentType::PriestStaff: { return "Resources/textures/items/staff_priest.tga"; }
	case EquipmentType::DruidStaff: { return "Resources/textures/items/staff_druid.tga"; }
	case EquipmentType::TwoHandedSword: { return "Resources/textures/items/2handed_sword.tga"; }
	case EquipmentType::Boomerang: { return "Resources/textures/items/boomerang.tga"; }
	case EquipmentType::Bomb: { return "Resources/textures/items/bomb.tga"; }
	case EquipmentType::KnifeLeft: { return "Resources/textures/items/knife.tga"; }
	case EquipmentType::KnifeRight: { return "Resources/textures/items/knife.tga"; }
	case EquipmentType::FireballHandLeft: { return "Resources/textures/items/fireball_hand.tga"; }
	case EquipmentType::FireballHandRight: { return "Resources/textures/items/fireball_hand.tga"; }
	case EquipmentType::Mace: { return "Resources/textures/items/mace.tga"; }
	case EquipmentType::Sickle: { return "Resources/textures/items/sickle.tga"; }
	case EquipmentType::DragonBow: { return "Resources/textures/items/dragon_bow.tga"; }
	// Equipment
	case EquipmentType::BlacksmithGloves: { return "Resources/textures/items/blacksmith_gloves.tga"; }
	case EquipmentType::NormalBoots: { return "Resources/textures/items/normal_boots.tga"; }
	case EquipmentType::NormalGloves: { return "Resources/textures/items/normal_gloves.tga"; }
	case EquipmentType::NormalShoulders: { return "Resources/textures/items/normal_shoulders.tga"; }
	case EquipmentType::RocketBoots: { return "Resources/textures/items/rocket_boots.tga"; }
	case EquipmentType::RegrowthBoots: { return "Resources/textures/items/regrowth_boots.tga"; }
	// Wood
	case EquipmentType::WoodenSword: { return "Resources/textures/items/wood_sword.tga"; }
	case EquipmentType::WoodenShield: { return "Resources/textures/items/wood_shield.tga"; }
	case EquipmentType::WoodenBow: { return "Resources/textures/items/wood_bow.tga"; }
	case EquipmentType::WoodenHelm: { return "Resources/textures/items/wood_helm.tga"; }
	case EquipmentType::WoodenArmor: { return "Resources/textures/items/wood_armor.tga"; }
	case EquipmentType::WoodenPants: { return "Resources/textures/items/wood_pants.tga"; }
	case EquipmentType::WoodenGloves: { return "Resources/textures/items/wood_gloves.tga"; }
	case EquipmentType::WoodenBoots: { return "Resources/textures/items/wood_boots.tga"; }
	case EquipmentType::WoodenShoulders: { return "Resources/textures/items/wood_shoulders.tga"; }
	// Iron
	case EquipmentType::IronSword: { return "Resources/textures/items/iron_sword.tga"; }
	case EquipmentType::IronShield: { return "Resources/textures/items/iron_shield.tga"; }
	case EquipmentType::IronHelm: { return "Resources/textures/items/iron_helm.tga"; }
	case EquipmentType::IronArmor: { return "Resources/textures/items/iron_armor.tga"; }
	case EquipmentType::IronPants: { return "Resources/textures/items/iron_pants.tga"; }
	case EquipmentType::IronGloves: { return "Resources/textures/items/iron_gloves.tga"; }
	case EquipmentType::IronBoots: { return "Resources/textures/items/iron_boots.tga"; }
	case EquipmentType::IronShoulders: { return "Resources/textures/items/iron_shoulders.tga"; }
	// Steel
	case EquipmentType::SteelHelm: { return "Resources/textures/items/steel_helm.tga"; }
	case EquipmentType::SteelArmor: { return "Resources/textures/items/steel_armor.tga"; }
	case EquipmentType::SteelPants: { return "Resources/textures/items/steel_pants.tga"; }
	case EquipmentType::SteelGloves: { return "Resources/textures/items/steel_gloves.tga"; }
	case EquipmentType::SteelBoots: { return "Resources/textures/items/steel_boots.tga"; }
	case EquipmentType::SteelShoulders: { return "Resources/textures/items/steel_shoulders.tga"; }
	// Ash
	case EquipmentType::AshSword: { return "Resources/textures/items/ash_sword.tga"; }
	case EquipmentType::AshHelm: { return "Resources/textures/items/ash_helm.tga"; }
	case EquipmentType::AshArmor: { return "Resources/textures/items/ash_armor.tga"; }
	case EquipmentType::AshPants: { return "Resources/textures/items/ash_pants.tga"; }
	case EquipmentType::AshGloves: { return "Resources/textures/items/ash_gloves.tga"; }
	case EquipmentType::AshBoots: { return "Resources/textures/items/ash_boots.tga"; }
	case EquipmentType::AshShoulders: { return "Resources/textures/items/ash_shoulders.tga"; }
	// Bone
	case EquipmentType::BoneSword: { return "Resources/textures/items/bone_edge.tga"; }
	case EquipmentType::BoneHelm: { return "Resources/textures/items/bone_helm.tga"; }
	case EquipmentType::BoneArmor: { return "Resources/textures/items/bone_armor.tga"; }
	case EquipmentType::BonePants: { return "Resources/textures/items/bone_pants.tga"; }
	case EquipmentType::BoneGloves: { return "Resources/textures/items/bone_gloves.tga"; }
	case EquipmentType::BoneBoots: { return "Resources/textures/items/bone_boots.tga"; }
	case EquipmentType::BoneShoulders: { return "Resources/textures/items/bone_shoulders.tga"; }
	// Ice
	case EquipmentType::IceHelm: { return "Resources/textures/items/ice_helm.tga"; }
	case EquipmentType::IceArmor: { return "Resources/textures/items/ice_armor.tga"; }
	case EquipmentType::IcePants: { return "Resources/textures/items/ice_pants.tga"; }
	case EquipmentType::IceGloves: { return "Resources/textures/items/ice_gloves.tga"; }
	case EquipmentType::IceBoots: { return "Resources/textures/items/ice_boots.tga"; }
	case EquipmentType::IceShoulders: { return "Resources/textures/items/ice_shoulders.tga"; }
	// Spike
	case EquipmentType::SpikeHelm: { return "Resources/textures/items/spike_helm.tga"; }
	case EquipmentType::SpikeArmor: { return "Resources/textures/items/spike_armor.tga"; }
	case EquipmentType::SpikePants: { return "Resources/textures/items/spike_pants.tga"; }
	case EquipmentType::SpikeGloves: { return "Resources/textures/items/spike_gloves.tga"; }
	case EquipmentType::SpikeBoots: { return "Resources/textures/items/spike_boots.tga"; }
	case EquipmentType::SpikeShoulders: { return "Resources/textures/items/spike_shoulders.tga"; }
	case EquipmentType::NUM_EQUIPMENT: throw std::logic_error("Invalid EquipmentType!");
	default: throw std::logic_error("Invalid EquipmentType!");
	}
}

InventoryType GetInventoryTypeForEquipment(EquipmentType type)
{
	switch (type)
	{
	case EquipmentType::None: { return InventoryType::Nothing; }
	// Weapons
	case EquipmentType::NormalPickaxe: { return InventoryType::WeaponPickaxe; }
	case EquipmentType::Torch: { return InventoryType::WeaponTorch; }
	case EquipmentType::Hammer: { return InventoryType::WeaponHammer; }
	case EquipmentType::MageStaff: { return InventoryType::WeaponStaff; }
	case EquipmentType::NecroStaff: { return InventoryType::WeaponStaff; }
	case EquipmentType::PriestStaff: { return InventoryType::WeaponStaff; }
	case EquipmentType::DruidStaff: { return InventoryType::WeaponStaff; }
	case EquipmentType::TwoHandedSword: { return InventoryType::WeaponTwoHandedSword; }
	case EquipmentType::Boomerang: { return InventoryType::WeaponBoomerang; }
	case EquipmentType::Bomb: { return InventoryType::WeaponBomb; }
	case EquipmentType::KnifeLeft: { return InventoryType::WeaponDagger; }
	case EquipmentType::KnifeRight: { return InventoryType::WeaponDagger; }
	case EquipmentType::FireballHandLeft: { return InventoryType::WeaponSpellHands; }
	case EquipmentType::FireballHandRight: { return InventoryType::WeaponSpellHands; }
	case EquipmentType::Mace: { return InventoryType::WeaponMace; }
	case EquipmentType::Sickle: { return InventoryType::WeaponSickle; }
	case EquipmentType::DragonBow: { return InventoryType::WeaponBow; }
	// Equipment
	case EquipmentType::BlacksmithGloves: { return InventoryType::Clothing; }
	case EquipmentType::NormalBoots: { return InventoryType::Clothing; }
	case EquipmentType::NormalGloves: { return InventoryType::Clothing; }
	case EquipmentType::NormalShoulders: { return InventoryType::Clothing; }
	case EquipmentType::RocketBoots: { return InventoryType::Clothing; }
	case EquipmentType::RegrowthBoots: { return InventoryType::Clothing; }
	// Wood
	case EquipmentType::WoodenSword: { return InventoryType::WeaponSword; }
	case EquipmentType::WoodenShield: { return InventoryType::WeaponShield; }
	case EquipmentType::WoodenBow: { return InventoryType::WeaponBow; }
	case EquipmentType::WoodenHelm: { return InventoryType::Clothing; }
	case EquipmentType::WoodenArmor: { return InventoryType::Clothing; }
	case EquipmentType::WoodenPants: { return InventoryType::Clothing; }
	case EquipmentType::WoodenGloves: { return InventoryType::Clothing; }
	case EquipmentType::WoodenBoots: { return InventoryType::Clothing; }
	case EquipmentType::WoodenShoulders: { return InventoryType::Clothing; }
	// Iron
	case EquipmentType::IronSword: { return InventoryType::WeaponSword; }
	case EquipmentType::IronShield: { return InventoryType::WeaponShield; }
	case EquipmentType::IronHelm: { return InventoryType::Clothing; }
	case EquipmentType::IronArmor: { return InventoryType::Clothing; }
	case EquipmentType::IronPants: { return InventoryType::Clothing; }
	case EquipmentType::IronGloves: { return InventoryType::Clothing; }
	case EquipmentType::IronBoots: { return InventoryType::Clothing; }
	case EquipmentType::IronShoulders: { return InventoryType::Clothing; }
	// Steel
	case EquipmentType::SteelHelm: { return InventoryType::Clothing; }
	case EquipmentType::SteelArmor: { return InventoryType::Clothing; }
	case EquipmentType::SteelPants: { return InventoryType::Clothing; }
	case EquipmentType::SteelGloves: { return InventoryType::Clothing; }
	case EquipmentType::SteelBoots: { return InventoryType::Clothing; }
	case EquipmentType::SteelShoulders: { return InventoryType::Clothing; }
	// Ash
	case EquipmentType::AshSword: { return InventoryType::WeaponSword; }
	case EquipmentType::AshHelm: { return InventoryType::Clothing; }
	case EquipmentType::AshArmor: { return InventoryType::Clothing; }
	case EquipmentType::AshPants: { return InventoryType::Clothing; }
	case EquipmentType::AshGloves: { return InventoryType::Clothing; }
	case EquipmentType::AshBoots: { return InventoryType::Clothing; }
	case EquipmentType::AshShoulders: { return InventoryType::Clothing; }
	// Bone
	case EquipmentType::BoneSword: { return InventoryType::WeaponSword; }
	case EquipmentType::BoneHelm: { return InventoryType::Clothing; }
	case EquipmentType::BoneArmor: { return InventoryType::Clothing; }
	case EquipmentType::BonePants: { return InventoryType::Clothing; }
	case EquipmentType::BoneGloves: { return InventoryType::Clothing; }
	case EquipmentType::BoneBoots: { return InventoryType::Clothing; }
	case EquipmentType::BoneShoulders: { return InventoryType::Clothing; }
	// Ice
	case EquipmentType::IceHelm: { return InventoryType::Clothing; }
	case EquipmentType::IceArmor: { return InventoryType::Clothing; }
	case EquipmentType::IcePants: { return InventoryType::Clothing; }
	case EquipmentType::IceGloves: { return InventoryType::Clothing; }
	case EquipmentType::IceBoots: { return InventoryType::Clothing; }
	case EquipmentType::IceShoulders: { return InventoryType::Clothing; }
	// Spike
	case EquipmentType::SpikeHelm: { return InventoryType::Clothing; }
	case EquipmentType::SpikeArmor: { return InventoryType::Clothing; }
	case EquipmentType::SpikePants: { return InventoryType::Clothing; }
	case EquipmentType::SpikeGloves: { return InventoryType::Clothing; }
	case EquipmentType::SpikeBoots: { return InventoryType::Clothing; }
	case EquipmentType::SpikeShoulders: { return InventoryType::Clothing; }
	case EquipmentType::NUM_EQUIPMENT: throw std::logic_error("Invalid EquipmentType!");
	default: throw std::logic_error("Invalid EquipmentType!");
	}
}

EquipSlot GetEquipSlotForEquipment(EquipmentType type)
{
	switch (type)
	{
	case EquipmentType::None: { return EquipSlot::NoSlot; }
	// Weapons
	case EquipmentType::NormalPickaxe: { return EquipSlot::RightHand; }
	case EquipmentType::Torch: { return EquipSlot::LeftHand; }
	case EquipmentType::Hammer: { return EquipSlot::RightHand; }
	case EquipmentType::MageStaff: { return EquipSlot::RightHand; }
	case EquipmentType::NecroStaff: { return EquipSlot::RightHand; }
	case EquipmentType::PriestStaff: { return EquipSlot::RightHand; }
	case EquipmentType::DruidStaff: { return EquipSlot::RightHand; }
	case EquipmentType::TwoHandedSword: { return EquipSlot::RightHand; }
	case EquipmentType::Boomerang: { return EquipSlot::RightHand; }
	case EquipmentType::Bomb: { return EquipSlot::RightHand; }
	case EquipmentType::KnifeLeft: { return EquipSlot::LeftHand; }
	case EquipmentType::KnifeRight: { return EquipSlot::RightHand; }
	case EquipmentType::FireballHandLeft: { return EquipSlot::LeftHand; }
	case EquipmentType::FireballHandRight: { return EquipSlot::RightHand; }
	case EquipmentType::Mace: { return EquipSlot::RightHand; }
	case EquipmentType::Sickle: { return EquipSlot::RightHand; }
	case EquipmentType::DragonBow: { return EquipSlot::LeftHand; }
	// Equipment
	case EquipmentType::BlacksmithGloves: { return EquipSlot::Hand; }
	case EquipmentType::NormalBoots: { return EquipSlot::Feet; }
	case EquipmentType::NormalGloves: { return EquipSlot::Hand; }
	case EquipmentType::NormalShoulders: { return EquipSlot::Shoulders; }
	case EquipmentType::RocketBoots: { return EquipSlot::Feet; }
	case EquipmentType::RegrowthBoots: { return EquipSlot::Feet; }
	// Wood
	case EquipmentType::WoodenSword: { return EquipSlot::RightHand; }
	case EquipmentType::WoodenShield: { return EquipSlot::LeftHand; }
	case EquipmentType::WoodenBow: { return EquipSlot::LeftHand; }
	case EquipmentType::WoodenHelm: { return EquipSlot::Head; }
	case EquipmentType::WoodenArmor: { return EquipSlot::Body; }
	case EquipmentType::WoodenPants: { return EquipSlot::Legs; }
	case EquipmentType::WoodenGloves: { return EquipSlot::Hand; }
	case EquipmentType::WoodenBoots: { return EquipSlot::Feet; }
	case EquipmentType::WoodenShoulders: { return EquipSlot::Shoulders; }
	// Iron
	case EquipmentType::IronSword: { return EquipSlot::RightHand; }
	case EquipmentType::IronShield: { return EquipSlot::LeftHand; }
	case EquipmentType::IronHelm: { return EquipSlot::Head; }
	case EquipmentType::IronArmor: { return EquipSlot::Body; }
	case EquipmentType::IronPants: { return EquipSlot::Legs; }
	case EquipmentType::IronGloves: { return EquipSlot::Hand; }
	case EquipmentType::IronBoots: { return EquipSlot::Feet; }
	case EquipmentType::IronShoulders: { return EquipSlot::Shoulders; }
	// Steel
	case EquipmentType::SteelHelm: { return EquipSlot::Head; }
	case EquipmentType::SteelArmor: { return EquipSlot::Body; }
	case EquipmentType::SteelPants: { return EquipSlot::Legs; }
	case EquipmentType::SteelGloves: { return EquipSlot::Hand; }
	case EquipmentType::SteelBoots: { return EquipSlot::Feet; }
	case EquipmentType::SteelShoulders: { return EquipSlot::Shoulders; }
	// Ash
	case EquipmentType::AshSword: { return EquipSlot::RightHand; }
	case EquipmentType::AshHelm: { return EquipSlot::Head; }
	case EquipmentType::AshArmor: { return EquipSlot::Body; }
	case EquipmentType::AshPants: { return EquipSlot::Legs; }
	case EquipmentType::AshGloves: { return EquipSlot::Hand; }
	case EquipmentType::AshBoots: { return EquipSlot::Feet; }
	case EquipmentType::AshShoulders: { return EquipSlot::Shoulders; }
	// Bone
	case EquipmentType::BoneSword: { return EquipSlot::RightHand; }
	case EquipmentType::BoneHelm: { return EquipSlot::Head; }
	case EquipmentType::BoneArmor: { return EquipSlot::Body; }
	case EquipmentType::BonePants: { return EquipSlot::Legs; }
	case EquipmentType::BoneGloves: { return EquipSlot::Hand; }
	case EquipmentType::BoneBoots: { return EquipSlot::Feet; }
	case EquipmentType::BoneShoulders: { return EquipSlot::Shoulders; }
	// Ice
	case EquipmentType::IceHelm: { return EquipSlot::Head; }
	case EquipmentType::IceArmor: { return EquipSlot::Body; }
	case EquipmentType::IcePants: { return EquipSlot::Legs; }
	case EquipmentType::IceGloves: { return EquipSlot::Hand; }
	case EquipmentType::IceBoots: { return EquipSlot::Feet; }
	case EquipmentType::IceShoulders: { return EquipSlot::Shoulders; }
	// Spike
	case EquipmentType::SpikeHelm: { return EquipSlot::Head; }
	case EquipmentType::SpikeArmor: { return EquipSlot::Body; }
	case EquipmentType::SpikePants: { return EquipSlot::Legs; }
	case EquipmentType::SpikeGloves: { return EquipSlot::Hand; }
	case EquipmentType::SpikeBoots: { return EquipSlot::Feet; }
	case EquipmentType::SpikeShoulders: { return EquipSlot::Shoulders; }
	case EquipmentType::NUM_EQUIPMENT: throw std::logic_error("Invalid EquipmentType!");
	default: throw std::logic_error("Invalid EquipmentType!");
	}
}

ItemQuality GetItemQualityForEquipment(EquipmentType type)
{
	switch (type)
	{
	case EquipmentType::None: { return ItemQuality::Common; }
	// Weapons
	case EquipmentType::NormalPickaxe: { return ItemQuality::Common; }
	case EquipmentType::Torch: { return ItemQuality::Common; }
	case EquipmentType::Hammer: { return ItemQuality::Common; }
	case EquipmentType::MageStaff: { return ItemQuality::Common; }
	case EquipmentType::NecroStaff: { return ItemQuality::Common; }
	case EquipmentType::PriestStaff: { return ItemQuality::Common; }
	case EquipmentType::DruidStaff: { return ItemQuality::Common; }
	case EquipmentType::TwoHandedSword: { return ItemQuality::Common; }
	case EquipmentType::Boomerang: { return ItemQuality::Common; }
	case EquipmentType::Bomb: { return ItemQuality::Common; }
	case EquipmentType::KnifeLeft: { return ItemQuality::Common; }
	case EquipmentType::KnifeRight: { return ItemQuality::Common; }
	case EquipmentType::FireballHandLeft: { return ItemQuality::Common; }
	case EquipmentType::FireballHandRight: { return ItemQuality::Common; }
	case EquipmentType::Mace: { return ItemQuality::Magical; }
	case EquipmentType::Sickle: { return ItemQuality::Magical; }
	case EquipmentType::DragonBow: { return ItemQuality::Rare; }
	// Equipment
	case EquipmentType::BlacksmithGloves: { return ItemQuality::Uncommon; }
	case EquipmentType::NormalBoots: { return ItemQuality::Uncommon; }
	case EquipmentType::NormalGloves: { return ItemQuality::Uncommon; }
	case EquipmentType::NormalShoulders: { return ItemQuality::Uncommon; }
	case EquipmentType::RocketBoots: { return ItemQuality::Uncommon; }
	case EquipmentType::RegrowthBoots: { return ItemQuality::Uncommon; }
	// Wood
	case EquipmentType::WoodenSword: { return ItemQuality::Common; }
	case EquipmentType::WoodenShield: { return ItemQuality::Common; }
	case EquipmentType::WoodenBow: { return ItemQuality::Common; }
	case EquipmentType::WoodenHelm: { return ItemQuality::Common; }
	case EquipmentType::WoodenArmor: { return ItemQuality::Common; }
	case EquipmentType::WoodenPants: { return ItemQuality::Common; }
	case EquipmentType::WoodenGloves: { return ItemQuality::Common; }
	case EquipmentType::WoodenBoots: { return ItemQuality::Common; }
	case EquipmentType::WoodenShoulders: { return ItemQuality::Common; }
	// Iron
	case EquipmentType::IronSword: { return ItemQuality::Common; }
	case EquipmentType::IronShield: { return ItemQuality::Common; }
	case EquipmentType::IronHelm: { return ItemQuality::Uncommon; }
	case EquipmentType::IronArmor: { return ItemQuality::Uncommon; }
	case EquipmentType::IronPants: { return ItemQuality::Uncommon; }
	case EquipmentType::IronGloves: { return ItemQuality::Uncommon; }
	case EquipmentType::IronBoots: { return ItemQuality::Uncommon; }
	case EquipmentType::IronShoulders: { return ItemQuality::Uncommon; }
	// Steel
	case EquipmentType::SteelHelm: { return ItemQuality::Uncommon; }
	case EquipmentType::SteelArmor: { return ItemQuality::Uncommon; }
	case EquipmentType::SteelPants: { return ItemQuality::Uncommon; }
	case EquipmentType::SteelGloves: { return ItemQuality::Uncommon; }
	case EquipmentType::SteelBoots: { return ItemQuality::Uncommon; }
	case EquipmentType::SteelShoulders: { return ItemQuality::Uncommon; }
	// Ash
	case EquipmentType::AshSword: { return ItemQuality::Epic; }
	case EquipmentType::AshHelm: { return ItemQuality::Rare; }
	case EquipmentType::AshArmor: { return ItemQuality::Rare; }
	case EquipmentType::AshPants: { return ItemQuality::Rare; }
	case EquipmentType::AshGloves: { return ItemQuality::Rare; }
	case EquipmentType::AshBoots: { return ItemQuality::Rare; }
	case EquipmentType::AshShoulders: { return ItemQuality::Rare; }
	// Bone
	case EquipmentType::BoneSword: { return ItemQuality::Rare; }
	case EquipmentType::BoneHelm: { return ItemQuality::Rare; }
	case EquipmentType::BoneArmor: { return ItemQuality::Rare; }
	case EquipmentType::BonePants: { return ItemQuality::Rare; }
	case EquipmentType::BoneGloves: { return ItemQuality::Rare; }
	case EquipmentType::BoneBoots: { return ItemQuality::Rare; }
	case EquipmentType::BoneShoulders: { return ItemQuality::Rare; }
	// Ice
	case EquipmentType::IceHelm: { return ItemQuality::Rare; }
	case EquipmentType::IceArmor: { return ItemQuality::Rare; }
	case EquipmentType::IcePants: { return ItemQuality::Rare; }
	case EquipmentType::IceGloves: { return ItemQuality::Rare; }
	case EquipmentType::IceBoots: { return ItemQuality::Rare; }
	case EquipmentType::IceShoulders: { return ItemQuality::Rare; }
	// Spike
	case EquipmentType::SpikeHelm: { return ItemQuality::Epic; }
	case EquipmentType::SpikeArmor: { return ItemQuality::Epic; }
	case EquipmentType::SpikePants: { return ItemQuality::Epic; }
	case EquipmentType::SpikeGloves: { return ItemQuality::Epic; }
	case EquipmentType::SpikeBoots: { return ItemQuality::Epic; }
	case EquipmentType::SpikeShoulders: { return ItemQuality::Epic; }
	case EquipmentType::NUM_EQUIPMENT: throw std::logic_error("Invalid EquipmentType!");
	default: throw std::logic_error("Invalid EquipmentType!");
	}
}

void GetItemSidesForEquipment(EquipmentType type, bool* left, bool* right)
{
	switch (type)
	{
	case EquipmentType::None: { *left = false; *right = false; return; }
	// Weapons
	case EquipmentType::NormalPickaxe: { *left = false; *right = true; return; }
	case EquipmentType::Torch: { *left = true; *right = false; return; }
	case EquipmentType::Hammer: { *left = false; *right = true; return; }
	case EquipmentType::MageStaff: { *left = true; *right = true; return; }
	case EquipmentType::NecroStaff: { *left = true; *right = true; return; }
	case EquipmentType::PriestStaff: { *left = true; *right = true; return; }
	case EquipmentType::DruidStaff: { *left = true; *right = true; return; }
	case EquipmentType::TwoHandedSword: { *left = true; *right = true; return; }
	case EquipmentType::Boomerang: { *left = false; *right = true; return; }
	case EquipmentType::Bomb: { *left = false; *right = true; return; }
	case EquipmentType::KnifeLeft: { *left = true; *right = false; return; }
	case EquipmentType::KnifeRight: { *left = false; *right = true; return; }
	case EquipmentType::FireballHandLeft: { *left = true; *right = false; return; }
	case EquipmentType::FireballHandRight: { *left = false; *right = true; return; }
	case EquipmentType::Mace: { *left = false; *right = true; return; }
	case EquipmentType::Sickle: { *left = false; *right = true; return; }
	case EquipmentType::DragonBow: { *left = true; *right = true; return; }
	// Equipment
	case EquipmentType::BlacksmithGloves: { *left = true; *right = true; return; }
	case EquipmentType::NormalBoots: { *left = true; *right = true; return; }
	case EquipmentType::NormalGloves: { *left = true; *right = true; return; }
	case EquipmentType::NormalShoulders: { *left = true; *right = true; return; }
	case EquipmentType::RocketBoots: { *left = true; *right = true; return; }
	case EquipmentType::RegrowthBoots: { *left = true; *right = true; return; }
	// Wood
	case EquipmentType::WoodenSword: { *left = false; *right = true; return; }
	case EquipmentType::WoodenShield: { *left = true; *right = false; return; }
	case EquipmentType::WoodenBow: { *left = true; *right = true; return; }
	case EquipmentType::WoodenHelm: { *left = false; *right = false; return; }
	case EquipmentType::WoodenArmor: { *left = false; *right = false; return; }
	case EquipmentType::WoodenPants: { *left = false; *right = false; return; }
	case EquipmentType::WoodenGloves: { *left = true; *right = true; return; }
	case EquipmentType::WoodenBoots: { *left = true; *right = true; return; }
	case EquipmentType::WoodenShoulders: { *left = true; *right = true; return; }
	// Iron
	case EquipmentType::IronSword: { *left = false; *right = true; return; }
	case EquipmentType::IronShield: { *left = true; *right = false; return; }
	case EquipmentType::IronHelm: { *left = false; *right = false; return; }
	case EquipmentType::IronArmor: { *left = false; *right = false; return; }
	case EquipmentType::IronPants: { *left = false; *right = false; return; }
	case EquipmentType::IronGloves: { *left = true; *right = true; return; }
	case EquipmentType::IronBoots: { *left = true; *right = true; return; }
	case EquipmentType::IronShoulders: { *left = true; *right = true; return; }
	// Steel
	case EquipmentType::SteelHelm: { *left = false; *right = false; return; }
	case EquipmentType::SteelArmor: { *left = false; *right = false; return; }
	case EquipmentType::SteelPants: { *left = false; *right = false; return; }
	case EquipmentType::SteelGloves: { *left = true; *right = true; return; }
	case EquipmentType::SteelBoots: { *left = true; *right = true; return; }
	case EquipmentType::SteelShoulders: { *left = true; *right = true; return; }
	// Ash
	case EquipmentType::AshSword: { *left = false; *right = true; return; }
	case EquipmentType::AshHelm: { *left = false; *right = false; return; }
	case EquipmentType::AshArmor: { *left = false; *right = false; return; }
	case EquipmentType::AshPants: { *left = false; *right = false; return; }
	case EquipmentType::AshGloves: { *left = true; *right = true; return; }
	case EquipmentType::AshBoots: { *left = true; *right = true; return; }
	case EquipmentType::AshShoulders: { *left = true; *right = true; return; }
	// Bone
	case EquipmentType::BoneSword: { *left = false; *right = true; return; }
	case EquipmentType::BoneHelm: { *left = false; *right = false; return; }
	case EquipmentType::BoneArmor: { *left = false; *right = false; return; }
	case EquipmentType::BonePants: { *left = false; *right = false; return; }
	case EquipmentType::BoneGloves: { *left = true; *right = true; return; }
	case EquipmentType::BoneBoots: { *left = true; *right = true; return; }
	case EquipmentType::BoneShoulders: { *left = true; *right = true; return; }
	// Ice
	case EquipmentType::IceHelm: { *left = false; *right = false; return; }
	case EquipmentType::IceArmor: { *left = false; *right = false; return; }
	case EquipmentType::IcePants: { *left = false; *right = false; return; }
	case EquipmentType::IceGloves: { *left = true; *right = true; return; }
	case EquipmentType::IceBoots: { *left = true; *right = true; return; }
	case EquipmentType::IceShoulders: { *left = true; *right = true; return; }
	// Spike
	case EquipmentType::SpikeHelm: { *left = false; *right = false; return; }
	case EquipmentType::SpikeArmor: { *left = false; *right = false; return; }
	case EquipmentType::SpikePants: { *left = false; *right = false; return; }
	case EquipmentType::SpikeGloves: { *left = true; *right = true; return; }
	case EquipmentType::SpikeBoots: { *left = true; *right = true; return; }
	case EquipmentType::SpikeShoulders: { *left = true; *right = true; return; }
	case EquipmentType::NUM_EQUIPMENT: throw std::logic_error("Invalid EquipmentType!");
	default: throw std::logic_error("Invalid EquipmentType!");
	}
}

void AddStatsModifiersForType(EquipmentType type, InventoryItem* pItem)
{
	if (type == EquipmentType::MageStaff) { pItem->AddStatAttribute(AttributeType::Intelligence, 5); }
	if (type == EquipmentType::NecroStaff) { pItem->AddStatAttribute(AttributeType::Intelligence, 4); }
	if (type == EquipmentType::PriestStaff) { pItem->AddStatAttribute(AttributeType::Intelligence, 6); }
	if (type == EquipmentType::DruidStaff) { pItem->AddStatAttribute(AttributeType::Intelligence, 3); }

	if (type == EquipmentType::WoodenSword) { pItem->AddStatAttribute(AttributeType::Strength, 2); }
	if (type == EquipmentType::WoodenShield) { pItem->AddStatAttribute(AttributeType::Armor, 2); }
	if (type == EquipmentType::WoodenBow) { pItem->AddStatAttribute(AttributeType::Dexterity, 1); }
	if (type == EquipmentType::WoodenHelm) { pItem->AddStatAttribute(AttributeType::Armor, 1); }
	if (type == EquipmentType::WoodenArmor) { pItem->AddStatAttribute(AttributeType::Armor, 1); }
	if (type == EquipmentType::WoodenPants) { pItem->AddStatAttribute(AttributeType::Armor, 1); }
	if (type == EquipmentType::WoodenGloves) { pItem->AddStatAttribute(AttributeType::Armor, 1); }
	if (type == EquipmentType::WoodenBoots) { pItem->AddStatAttribute(AttributeType::Armor, 1); }
	if (type == EquipmentType::WoodenShoulders) { pItem->AddStatAttribute(AttributeType::Armor, 1); }

	if (type == EquipmentType::IronSword) { pItem->AddStatAttribute(AttributeType::Strength, 2); }
	if (type == EquipmentType::IronShield) { pItem->AddStatAttribute(AttributeType::Armor, 2); }
	if (type == EquipmentType::IronHelm) { pItem->AddStatAttribute(AttributeType::Armor, 2); }
	if (type == EquipmentType::IronArmor) { pItem->AddStatAttribute(AttributeType::Armor, 2); }
	if (type == EquipmentType::IronPants) { pItem->AddStatAttribute(AttributeType::Armor, 2); }
	if (type == EquipmentType::IronGloves) { pItem->AddStatAttribute(AttributeType::Armor, 2); }
	if (type == EquipmentType::IronBoots) { pItem->AddStatAttribute(AttributeType::Armor, 2); }
	if (type == EquipmentType::IronShoulders) { pItem->AddStatAttribute(AttributeType::Armor, 2); }

	if (type == EquipmentType::SteelHelm) { pItem->AddStatAttribute(AttributeType::Armor, 3); }
	if (type == EquipmentType::SteelArmor) { pItem->AddStatAttribute(AttributeType::Armor, 3); }
	if (type == EquipmentType::SteelPants) { pItem->AddStatAttribute(AttributeType::Armor, 3); }
	if (type == EquipmentType::SteelGloves) { pItem->AddStatAttribute(AttributeType::Armor, 3); }
	if (type == EquipmentType::SteelBoots) { pItem->AddStatAttribute(AttributeType::Armor, 3); }
	if (type == EquipmentType::SteelShoulders) { pItem->AddStatAttribute(AttributeType::Armor, 3); }

	if (type == EquipmentType::AshHelm) { pItem->AddStatAttribute(AttributeType::Armor, 4); }
	if (type == EquipmentType::AshArmor) { pItem->AddStatAttribute(AttributeType::Armor, 4); }
	if (type == EquipmentType::AshPants) { pItem->AddStatAttribute(AttributeType::Armor, 4); }
	if (type == EquipmentType::AshGloves) { pItem->AddStatAttribute(AttributeType::Armor, 4); }
	if (type == EquipmentType::AshBoots) { pItem->AddStatAttribute(AttributeType::Armor, 4); }
	if (type == EquipmentType::AshShoulders) { pItem->AddStatAttribute(AttributeType::Armor, 4); }

	if (type == EquipmentType::BoneHelm) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::BoneArmor) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::BonePants) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::BoneGloves) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::BoneBoots) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::BoneShoulders) { pItem->AddStatAttribute(AttributeType::Armor, 5); }

	if (type == EquipmentType::IceHelm) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::IceArmor) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::IcePants) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::IceGloves) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::IceBoots) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::IceShoulders) { pItem->AddStatAttribute(AttributeType::Armor, 5); }

	if (type == EquipmentType::SpikeHelm) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::SpikeArmor) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::SpikePants) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::SpikeGloves) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::SpikeBoots) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
	if (type == EquipmentType::SpikeShoulders) { pItem->AddStatAttribute(AttributeType::Armor, 5); }
}