/*************************************************************************
> File Name: Color.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A simple 4 float color class.
> Created Time: 2016/06/20
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <cmath>

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
	float min, max, delta;

	min = r < g ? r : g;
	min = min < b ? min : b;

	max = r > g ? r : g;
	max = max > b ? max : b;

	*v = max;
	delta = max - min;

	if (max > 0.0f)
	{
		*s = delta / max;
	}
	else
	{
		// r = g = b = 0
		*s = 0.0f;
		*h = 0.0f;
		return;
	}

	if (r >= max)
	{
		if (g - b == 0.0f)
		{
			*h = 0.0f;
		}
		else
		{
			// Between yellow & magenta
			*h = (g - b) / delta;
		}
	}
	else
	{
		if (g >= max)
		{
			// Between cyan & yellow
			*h = 2.0f + (b - r) / delta;
		}
		else
		{
			// Between magenta & cyan
			*h = 4.0f + (r - g) / delta;
		}
	}

	// Degrees
	*h = 60.0f;

	if (*h < 0.0f)
	{
		*h += 360.0f;
	}
}

void Color::HSVtoRGB(float h, float s, float v, float* r, float* g, float* b)
{
	float hh, p, q, t, ff;
	long i;

	if (s <= 0.0f)
	{
		if (h == NAN)
		{
			*r = v;
			*g = v;
			*b = v;
			return;
		}

		// Error - should never happen
		*r = 0.0f;
		*g = 0.0f;
		*b = 0.0f;
		return;
	}

	hh = h;
	if (hh >= 360.0f)
	{
		hh = 0.0f;
	}
	hh /= 60.0f;

	i = static_cast<long>(hh);
	ff = hh - i;
	
	p = v * (1.0f - s);
	q = v * (1.0f - s * ff);
	t = v * (1.0f - s * (1.0f - ff));

	switch (i)
	{
	case 0:
		*r = v;
		*g = t;
		*b = p;
		break;
	case 1:
		*r = q;
		*g = v;
		*b = p;
		break;
	case 2:
		*r = p;
		*g = v;
		*b = t;
		break;
	case 3:
		*r = p;
		*g = q;
		*b = v;
		break;
	case 4:
		*r = t;
		*g = p;
		*b = v;
		break;
	case 5:
	default:
		*r = v;
		*g = p;
		*b = q;
		break;
	}
}