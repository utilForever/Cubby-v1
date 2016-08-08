/*************************************************************************
> File Name: DirectDrawRectangle.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A rendered rectangle that is drawn directly by OpenGL, using primitives.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_DIRECT_DRAW_RECTANGLE_H
#define CUBBY_DIRECT_DRAW_RECTANGLE_H

#include "RenderRectangle.h"

class DirectDrawRectangle : public RenderRectangle
{
public:
	// Constructor, Destructor
	DirectDrawRectangle(Renderer* pRenderer);
	~DirectDrawRectangle();

	void SetBackgroundColorTopLeft(Color color);
	void SetBackgroundColorTopRight(Color color);
	void SetBackgroundColorBottomLeft(Color color);
	void SetBackgroundColorBottomRight(Color color);

	void SetOutlineColorTop(Color color);
	void SetOutlineColorBottom(Color color);
	void SetOutlineColorLeft(Color color);
	void SetOutlineColorRight(Color color);

	ComponentType GetComponentType() const override;

protected:
	void DrawSelf() override;

private:
	float m_lineSizeTop;
	float m_lineSizeBottom;
	float m_lineSizeLeft;
	float m_lineSizeRight;

	Color m_backgroundColorTopLeft;
	Color m_backgroundColorTopRight;
	Color m_backgroundColorBottomLeft;
	Color m_backgroundColorBottomRight;

	Color m_outlineColorTop;
	Color m_outlineColorBottom;
	Color m_outlineColorLeft;
	Color m_outlineColorRight;
};

#endif