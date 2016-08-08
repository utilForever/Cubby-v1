/*************************************************************************
> File Name: MultiTextureIcon.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A multi-textured icon that has a separate graphic for each of the
> 	 rectangles regions, and can scale without graphical distortion.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MULTI_TEXTURE_ICON_H
#define CUBBY_MULTI_TEXTURE_ICON_H

#include "RenderRectangle.h"

enum class RectangleRegion
{
	TopLeft = 0,
	TopCenter,
	TopRight,
	MiddleLeft,
	MiddleCenter,
	MiddleRight,
	BottomLeft,
	BottomCenter,
	BottomRight,

	Num,
};

class MultiTextureIcon : public RenderRectangle
{
public:
	// Constructor, Destructor
	MultiTextureIcon(Renderer* pRenderer);
	~MultiTextureIcon();

	void SetTexture(RectangleRegion regionTexture, const std::string& fileName);

	int GetTextureWidth(RectangleRegion regionTexture) const;
	int GetTextureHeight(RectangleRegion regionTexture) const;

	void CreateDrawBuffer();

	ComponentType GetComponentType() const;

protected:
	void DrawSelf();

private:
	std::string m_fileName[static_cast<int>(RectangleRegion::Num)];

	int m_TextureWidth[static_cast<int>(RectangleRegion::Num)];
	int m_TextureHeight[static_cast<int>(RectangleRegion::Num)];

	int m_TextureWidthPower2[static_cast<int>(RectangleRegion::Num)];
	int m_TextureHeightPower2[static_cast<int>(RectangleRegion::Num)];

	unsigned int m_textureID[static_cast<int>(RectangleRegion::Num)];

	unsigned int mTextureDrawBuffer;
};

#endif