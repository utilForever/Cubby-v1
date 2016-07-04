/*************************************************************************
> File Name: Random.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A selection of helper functions to make generating random numbers easier.
> Created Time: 2016/07/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_RANDOM_H
#define CUBBY_RANDOM_H

#include <random>

// Get a random integer number in the range from lower to higher. INCLUSIVE
inline int GetRandomNumber(int lower, int higher)
{
	std::random_device rd;
	std::mt19937 mtRand(rd());

	if (lower > higher)
	{
		int temp = lower;
		lower = higher;
		higher = temp;
	}

	int diff = (higher + 1) - lower;
	return mtRand() % diff + lower;
}

// Get a random floating point number in the range from lower to higher. INCLUSIVE
// Precision defines how many significant numbers there are after the point
inline float GetRandomNumber(int lower, int higher, int precision)
{
	float precisionPow = pow(10.0f, precision);
	float precisionRand = static_cast<float>(GetRandomNumber(static_cast<int>(lower * precisionPow), static_cast<int>(higher * precisionPow)));

	return precisionRand / precisionPow;
}


#endif