/*************************************************************************
> File Name: MultiTextureIcon.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A multi-textured icon that has a separate graphic for each of the
> 	 rectangles regions, and can scale without graphical distortion.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "MultiTextureIcon.h"

// Constructor, Destructor
MultiTextureIcon::MultiTextureIcon(Renderer* pRenderer) :
	RenderRectangle(pRenderer)
{
	CreateDrawBuffer();
}

MultiTextureIcon::~MultiTextureIcon()
{

}

void MultiTextureIcon::SetTexture(RectangleRegion regionTexture, const std::string &fileName)
{
	int idx = static_cast<int>(regionTexture);
	m_pRenderer->LoadTexture(fileName.c_str(), &m_TextureWidth[idx], &m_TextureHeight[idx], &m_TextureWidthPower2[idx], &m_TextureHeightPower2[idx], &m_textureID[idx]);
}

int MultiTextureIcon::GetTextureWidth(RectangleRegion regionTexture) const
{
	return m_TextureWidth[static_cast<int>(regionTexture)];
}

int MultiTextureIcon::GetTextureHeight(RectangleRegion regionTexture) const
{
	return m_TextureHeight[static_cast<int>(regionTexture)];
}

void MultiTextureIcon::CreateDrawBuffer()
{
	// Create the buffer used for drawing the image
	PositionNormalVertex vertices[4] = {
		//X			Y		Z		nX		nY		nZ
		{ 0.0f,		0.0f,	0.0f,	0.0f,	0.0f,	1.0f },
		{ 1.0f,		0.0f,	0.0f,	0.0f,	0.0f,	1.0f },
		{ 1.0f,		1.0f,	0.0f,	0.0f,	0.0f,	1.0f },
		{ 0.0f,		1.0f,	0.0f,	0.0f,	0.0f,	1.0f }
	};

	UVCoordinate textureCoordinates[4] = {
		//U			V
		{ 0.0f,		1.0f },
		{ 1.0f,		1.0f },
		{ 1.0f,		0.0f },
		{ 0.0f,		0.0f },
	};

	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

	m_pRenderer->CreateStaticBuffer(VertexType::POSITION_NORMAL_UV, -1, -1, 4, 4, 6, &vertices, &textureCoordinates, indices, &mTextureDrawBuffer);
}

ComponentType MultiTextureIcon::GetComponentType() const
{
	return ComponentType::MultiTextureIcon;
}

void MultiTextureIcon::DrawSelf()
{
	int posX = 0;
	int posY = 0;

	int totalWidth = m_dimensions.width;
	int totalHeight = m_dimensions.height;


	// TODO: Need to fix when non-power-of-2 sized textures are used, this will cause an height offset bug and also alpha will be messed up...
	// See the normal Icon class for an example of how to fix this. :)

	// Top left
	int topLeftWidth = m_TextureWidth[static_cast<int>(RectangleRegion::TopLeft)];
	int topLeftHeight = m_TextureHeight[static_cast<int>(RectangleRegion::TopLeft)];
	int topLeftX = posX;
	int topLeftY = posY + totalHeight - topLeftHeight;

	m_pRenderer->PushMatrix();

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->TranslateWorldMatrix(static_cast<float>(topLeftX), static_cast<float>(topLeftY), GetDepth());
	m_pRenderer->ScaleWorldMatrix(static_cast<float>(topLeftWidth), static_cast<float>(topLeftHeight), 1.0f);
	m_pRenderer->BindTexture(m_textureID[static_cast<int>(RectangleRegion::TopLeft)]);
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	//m_pRenderer->RenderFromArray(VertexType::POSITION_NORMAL_UV, -1, 4, 6, &vertices, indices);
	m_pRenderer->RenderStaticBuffer(mTextureDrawBuffer);
	m_pRenderer->DisableTransparency();
	m_pRenderer->DisableTexture();

	m_pRenderer->PopMatrix();

	// Top right
	int topRightWidth = m_TextureWidth[static_cast<int>(RectangleRegion::TopRight)];
	int topRightHeight = m_TextureHeight[static_cast<int>(RectangleRegion::TopRight)];
	int topRightX = posX + totalWidth - topRightWidth;
	int topRightY = posY + totalHeight - topRightHeight;

	m_pRenderer->PushMatrix();

	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->TranslateWorldMatrix(static_cast<float>(topRightX), static_cast<float>(topRightY), GetDepth());
	m_pRenderer->ScaleWorldMatrix(static_cast<float>(topRightWidth), static_cast<float>(topRightHeight), 1.0f);
	m_pRenderer->BindTexture(m_textureID[static_cast<int>(RectangleRegion::TopRight)]);
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	//m_pRenderer->RenderFromArray(VertexType::POSITION_NORMAL_UV, -1, 4, 6, &vertices, indices);
	m_pRenderer->RenderStaticBuffer(mTextureDrawBuffer);
	m_pRenderer->DisableTransparency();
	m_pRenderer->DisableTexture();

	m_pRenderer->PopMatrix();

	// Top center
	int topCenterWidth = totalWidth - topLeftWidth - topRightWidth;
	int topCenterHeight = m_TextureHeight[static_cast<int>(RectangleRegion::TopCenter)];
	int topCenterX = posX + topLeftWidth;
	int topCenterY = posY + totalHeight - topCenterHeight;

	m_pRenderer->PushMatrix();
	
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->TranslateWorldMatrix(static_cast<float>(topCenterX), static_cast<float>(topCenterY), GetDepth());
	m_pRenderer->ScaleWorldMatrix(static_cast<float>(topCenterWidth), static_cast<float>(topCenterHeight), 1.0f);
	m_pRenderer->BindTexture(m_textureID[static_cast<int>(RectangleRegion::TopCenter)]);
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	//m_pRenderer->RenderFromArray(VertexType::POSITION_NORMAL_UV, -1, 4, 6, &vertices, indices);
	m_pRenderer->RenderStaticBuffer(mTextureDrawBuffer);
	m_pRenderer->DisableTransparency();
	m_pRenderer->DisableTexture();
	
	m_pRenderer->PopMatrix();

	// Bottom left
	int bottomLeftWidth = m_TextureWidth[static_cast<int>(RectangleRegion::BottomLeft)];
	int bottomLeftHeight = m_TextureHeight[static_cast<int>(RectangleRegion::BottomLeft)];
	int bottomLeftX = posX;
	int bottomLeftY = posY;

	m_pRenderer->PushMatrix();
	
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->TranslateWorldMatrix(static_cast<float>(bottomLeftX), static_cast<float>(bottomLeftY), GetDepth());
	m_pRenderer->ScaleWorldMatrix(static_cast<float>(bottomLeftWidth), static_cast<float>(bottomLeftHeight), 1.0f);
	m_pRenderer->BindTexture(m_textureID[static_cast<int>(RectangleRegion::BottomLeft)]);
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	//m_pRenderer->RenderFromArray(VertexType::POSITION_NORMAL_UV, -1, 4, 6, &vertices, indices);
	m_pRenderer->RenderStaticBuffer(mTextureDrawBuffer);
	m_pRenderer->DisableTransparency();
	m_pRenderer->DisableTexture();
	
	m_pRenderer->PopMatrix();

	// Bottom right
	int bottomRightWidth = m_TextureWidth[static_cast<int>(RectangleRegion::BottomRight)];
	int bottomRightHeight = m_TextureHeight[static_cast<int>(RectangleRegion::BottomRight)];
	int bottomRightX = posX + totalWidth - bottomRightWidth;
	int bottomRightY = posY;

	m_pRenderer->PushMatrix();
	
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->TranslateWorldMatrix(static_cast<float>(bottomRightX), static_cast<float>(bottomRightY), GetDepth());
	m_pRenderer->ScaleWorldMatrix(static_cast<float>(bottomRightWidth), static_cast<float>(bottomRightHeight), 1.0f);
	m_pRenderer->BindTexture(m_textureID[static_cast<int>(RectangleRegion::BottomRight)]);
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	//m_pRenderer->RenderFromArray(VertexType::POSITION_NORMAL_UV, -1, 4, 6, &vertices, indices);
	m_pRenderer->RenderStaticBuffer(mTextureDrawBuffer);
	m_pRenderer->DisableTransparency();
	m_pRenderer->DisableTexture();
	
	m_pRenderer->PopMatrix();

	// Bottom center
	int bottomCenterWidth = totalWidth - bottomLeftWidth - bottomRightWidth;
	int bottomCenterHeight = m_TextureHeight[static_cast<int>(RectangleRegion::BottomCenter)];
	int bottomCenterX = posX + bottomLeftWidth;
	int bottomCenterY = posY;

	m_pRenderer->PushMatrix();
	
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->TranslateWorldMatrix(static_cast<float>(bottomCenterX), static_cast<float>(bottomCenterY), GetDepth());
	m_pRenderer->ScaleWorldMatrix(static_cast<float>(bottomCenterWidth), static_cast<float>(bottomCenterHeight), 1.0f);
	m_pRenderer->BindTexture(m_textureID[static_cast<int>(RectangleRegion::BottomCenter)]);
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	//m_pRenderer->RenderFromArray(VertexType::POSITION_NORMAL_UV, -1, 4, 6, &vertices, indices);
	m_pRenderer->RenderStaticBuffer(mTextureDrawBuffer);
	m_pRenderer->DisableTransparency();
	m_pRenderer->DisableTexture();
	
	m_pRenderer->PopMatrix();

	// Middle left
	int middleLeftWidth = m_TextureWidth[static_cast<int>(RectangleRegion::MiddleLeft)];
	int middleLeftHeight = totalHeight - topLeftHeight - bottomLeftHeight;
	int middleLeftX = posX;
	int middleLeftY = posY + bottomLeftHeight;

	m_pRenderer->PushMatrix();
	
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->TranslateWorldMatrix(static_cast<float>(middleLeftX), static_cast<float>(middleLeftY), GetDepth());
	m_pRenderer->ScaleWorldMatrix(static_cast<float>(middleLeftWidth), static_cast<float>(middleLeftHeight), 1.0f);
	m_pRenderer->BindTexture(m_textureID[static_cast<int>(RectangleRegion::MiddleLeft)]);
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	//m_pRenderer->RenderFromArray(VertexType::POSITION_NORMAL_UV, -1, 4, 6, &vertices, indices);
	m_pRenderer->RenderStaticBuffer(mTextureDrawBuffer);
	m_pRenderer->DisableTransparency();
	m_pRenderer->DisableTexture();
	
	m_pRenderer->PopMatrix();

	// Middle right
	int middleRightWidth = m_TextureWidth[static_cast<int>(RectangleRegion::MiddleRight)];
	int middleRightHeight = totalHeight - topRightHeight - bottomRightHeight;
	int middleRightX = posX + totalWidth - middleRightWidth;
	int middleRightY = posY + bottomLeftHeight;

	m_pRenderer->PushMatrix();
	
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->TranslateWorldMatrix(static_cast<float>(middleRightX), static_cast<float>(middleRightY), GetDepth());
	m_pRenderer->ScaleWorldMatrix(static_cast<float>(middleRightWidth), static_cast<float>(middleRightHeight), 1.0f);
	m_pRenderer->BindTexture(m_textureID[static_cast<int>(RectangleRegion::MiddleRight)]);
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	//m_pRenderer->RenderFromArray(VertexType::POSITION_NORMAL_UV, -1, 4, 6, &vertices, indices);
	m_pRenderer->RenderStaticBuffer(mTextureDrawBuffer);
	m_pRenderer->DisableTransparency();
	m_pRenderer->DisableTexture();
	
	m_pRenderer->PopMatrix();

	// Middle center
	int middleCenterWidth = totalWidth - middleLeftWidth - middleRightWidth;
	int middleCenterHeight = totalHeight - topCenterHeight - bottomCenterHeight;
	int middleCenterX = posX + middleLeftWidth;
	int middleCenterY = posY + bottomLeftHeight;

	m_pRenderer->PushMatrix();
	
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	m_pRenderer->SetRenderMode(RenderMode::SOLID);
	m_pRenderer->TranslateWorldMatrix(static_cast<float>(middleCenterX), static_cast<float>(middleCenterY), GetDepth());
	m_pRenderer->ScaleWorldMatrix(static_cast<float>(middleCenterWidth), static_cast<float>(middleCenterHeight), 1.0f);
	m_pRenderer->BindTexture(m_textureID[static_cast<int>(RectangleRegion::MiddleCenter)]);
	m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
	//m_pRenderer->RenderFromArray(VertexType::POSITION_NORMAL_UV, -1, 4, 6, &vertices, indices);
	m_pRenderer->RenderStaticBuffer(mTextureDrawBuffer);
	m_pRenderer->DisableTransparency();
	m_pRenderer->DisableTexture();
	
	m_pRenderer->PopMatrix();
}