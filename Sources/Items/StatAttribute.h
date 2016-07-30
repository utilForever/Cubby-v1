/*************************************************************************
> File Name: StatAttribute.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Stat attributes are a sub-class that are used by items that modify the
>    player stats when equipped. For example armor that increased defense, or
>    weapons that improve attack strength or dexterity.
> Created Time: 2016/07/30
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_STAT_ATTRIBUTE_H
#define CUBBY_STAT_ATTRIBUTE_H

#include <string>

enum class AttributeType
{
	Strength = 0,
	Dexterity,
	Intelligence,
	Vitality,
	Armor,
	Luck,
};

class StatAttribute
{
public:
	// Constructor, Destructor
	StatAttribute(AttributeType type, int modifyAmount);
	~StatAttribute();

	AttributeType GetType() const;
	void SetType(AttributeType type);

	int GetModifyAmount() const;
	void SetModifyAmount(int amount);

	// Helper function for GUI display purposes
	std::string GetString() const;

private:
	AttributeType m_type;
	int m_modifyAmount;
};

#endif