/*************************************************************************
> File Name: Icon.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An image component, used to display graphics on other gui components.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ICON_H
#define CUBBY_ICON_H

#include "RenderRectangle.h"

class Icon : public RenderRectangle
{
public:
	// Constructor, Destructor
	Icon();
	Icon(Renderer* pRenderer);
	Icon(Renderer* pRenderer, const std::string& fileName, int width = 32, int height = 32);

	~Icon();

	std::string GetFileName() const;

	void SetDimensions(int x, int y, int width, int height) override;

	void SetIcon(RenderRectangle* icon);
	void SetIcon(const std::string& fileName);

	void SetDynamicTexture(unsigned int textureID);

	void SetFlipped(bool x, bool y);

	void CreateDrawBuffer();

	int GetTextureWidth() const;
	int GetTextureHeight() const;

	int GetTextureWidthPower2() const;
	int GetTextureHeightPower2() const;

	ComponentType GetComponentType() const override;

protected:
	void DrawSelf() override;

private:
	RenderRectangle* m_pIcon;

	std::string m_fileName;

	int m_textureWidth;
	int m_textureHeight;

	int m_textureWidthPower2;
	int m_textureHeightPower2;

	unsigned int m_textureDrawBuffer;

	unsigned int m_textureID;
	unsigned int m_dynamicTextureID;

	bool m_dynamicTexture;

	bool m_flippedX;
	bool m_flippedY;
};

#endif