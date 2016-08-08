/*************************************************************************
> File Name: RenderRectangle.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A rectangle drawing base class, used to define abstract behavior for
> 	 drawing of rectangular bounded areas.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_RENDER_RECTANGLE_H
#define CUBBY_RENDER_RECTANGLE_H

#include "Container.h"

class RenderRectangle : public Container
{
public:
	// Constructor, Destructor
	RenderRectangle(Renderer* pRenderer);
	virtual ~RenderRectangle();

	void AddText(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color color, int xOffset, int yOffset, bool outline = false, Color outlineColor = Color(1.0f, 1.0f, 1.0f, 1.0f));
	void AddIcon(Renderer* pRenderer, const std::string& fileName, int texWidth, int texHeight, int width, int height, int xOffset, int yOffset, float depth);

	ComponentType GetComponentType() const override;

protected:
	void DrawSelf() override;
	void DrawChildren() override;

	ComponentList m_vpAddedComponentList;
};


#endif