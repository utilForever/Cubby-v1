/*************************************************************************
> File Name: 3DMaths.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: 3D maths library.
> Created Time: 2016/06/18
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_3D_MATH_H
#define CUBBY_3D_MATH_H

const float PI = 3.14159265358979323846f;

// Conversions
template <typename T>
T DegreeToRadian(const T& degrees)
{
	return (degrees * PI) / 180;
}

template <typename T>
T RadianToDegree(const T& radians)
{
	return (radians * 180) / PI;
}

template <typename T>
void Swap(T& v1, T& v2)
{
	T temp = v1;
	v1 = v2;
	v2 = temp;
}

#endif