/*************************************************************************
> File Name: BlocksEnum.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An enum list of all of the block types.
> Created Time: 2016/07/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_BLOCKS_ENUM_H
#define CUBBY_BLOCKS_ENUM_H

enum class BlockType
{
	Default = 0,
	UndergroundIndestructable,
	Stone,
	Dirt,
	Grass,
	Rock,
	Snow,
	Wood,
	Sand,
	Cactus,
	Leaf,
	CustomColor,
	NumTypes,
};

#endif