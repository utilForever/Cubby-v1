/*************************************************************************
> File Name: Color.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A simple 4 float color class.
> Created Time: 2016/06/20
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Color.h"

// Constructors
Color::Color()
{
	m_rgba[0] = m_rgba[1] = m_rgba[2] = m_rgba[3] = 0.0f;
}

Color::Color(float r, float g, float b, float a)
{
	m_rgba[0] = r;
	m_rgba[1] = g;
	m_rgba[2] = b;
	m_rgba[3] = a;
}

Color::Color(float rgba[])
{
	m_rgba[0] = rgba[0];
	m_rgba[1] = rgba[1];
	m_rgba[2] = rgba[2];
	m_rgba[3] = rgba[3];
}

// Getter, Setter
const float Color::GetRed() const
{
	return m_rgba[0];
}

const float Color::GetGreen() const
{
	return m_rgba[1];
}

const float Color::GetBlue() const
{
	return m_rgba[2];
}

const float Color::GetAlpha() const
{
	return m_rgba[3];
}

void Color::SetRed(const float red)
{
	m_rgba[0] = red;
}

void Color::SetGreen(const float green)
{
	m_rgba[1] = green;
}

void Color::SetBlue(const float blue)
{
	m_rgba[2] = blue;
}

void Color::SetAlpha(const float alpha)
{
	m_rgba[3] = alpha;
}

const float* Color::GetRGBA() const
{
	return m_rgba;
}

void Color::RGBtoHSV(float r, float g, float b, float* h, float* s, float* v)
{
	
}

void Color::HSVtoRGB(float h, float s, float v, float* r, float* g, float& b)
{
	
}