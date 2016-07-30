/*************************************************************************
> File Name: StatAttribute.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Stat attributes are a sub-class that are used by items that modify the
>    player stats when equipped. For example armor that increased defense, or
>    weapons that improve attack strength or dexterity.
> Created Time: 2016/07/30
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "StatAttribute.h"

// Constructor, Destructor
StatAttribute::StatAttribute(AttributeType type, int modifyAmount) :
	m_type(type), m_modifyAmount(modifyAmount)
{

}

StatAttribute::~StatAttribute()
{

}

AttributeType StatAttribute::GetType() const
{
	return m_type;
}

void StatAttribute::SetType(AttributeType type)
{
	m_type = type;
}

int StatAttribute::GetModifyAmount() const
{
	return m_modifyAmount;
}

void StatAttribute::SetModifyAmount(int amount)
{
	m_modifyAmount = amount;
}

// Helper function for GUI display purposes
std::string StatAttribute::GetString() const
{
	char returnString[64];
	char signModifier[2];
	char colourModifier[16];
	
	if (m_modifyAmount > 0)
	{
		sprintf(signModifier, "+");
		sprintf(colourModifier, "Green");
	}
	else
	{
		sprintf(signModifier, "");
		sprintf(colourModifier, "Red");
	}

	switch (m_type)
	{
	case AttributeType::Strength: { sprintf(returnString, "[C=%s]%s%i Strength", colourModifier, signModifier, m_modifyAmount); break; }
	case AttributeType::Dexterity: { sprintf(returnString, "[C=%s]%s%i Dexterity", colourModifier, signModifier, m_modifyAmount); break; }
	case AttributeType::Intelligence: { sprintf(returnString, "[C=%s]%s%i Intelligence", colourModifier, signModifier, m_modifyAmount); break; }
	case AttributeType::Vitality: { sprintf(returnString, "[C=%s]%s%i Vitality", colourModifier, signModifier, m_modifyAmount); break; }
	case AttributeType::Armor: { sprintf(returnString, "[C=%s]%s%i Armor", colourModifier, signModifier, m_modifyAmount); break; }
	case AttributeType::Luck: { sprintf(returnString, "[C=%s]%s%i Luck", colourModifier, signModifier, m_modifyAmount); break; }
	}

	return returnString;
}
