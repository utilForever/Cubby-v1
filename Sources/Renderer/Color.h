/*************************************************************************
> File Name: Color.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A simple 4 float color class.
> Created Time: 2016/06/20
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_COLOR_H
#define CUBBY_COLOR_H

class Color
{
public:
	// Constructors
	Color();
	Color(float r, float g, float b, float a = 1.0f);
	Color(float rgba[]);

	// Getter, Setter
	const float GetRed() const;
	const float GetGreen() const;
	const float GetBlue() const;
	const float GetAlpha() const;
	void SetRed(const float red);
	void SetGreen(const float green);
	void SetBlue(const float blue);
	void SetAlpha(const float alpha);

	const float* GetRGBA() const;

	static void RGBtoHSV(float r, float g, float b, float* h, float* s, float* v);
	static void HSVtoRGB(float h, float s, float v, float* r, float* g, float* b);

private:
	float m_rgba[4];
};

#endif