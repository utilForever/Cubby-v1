/*************************************************************************
> File Name: Icon.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An image component, used to display graphics on other gui components.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Icon.h"
#include "DirectDrawRectangle.h"
#include "MultiTextureIcon.h"

// Constructor, Destructor
Icon::Icon() :
	RenderRectangle(nullptr),
	m_textureWidth(0), m_textureHeight(0),
	m_textureWidthPower2(0), m_textureHeightPower2(0),
	m_textureDrawBuffer(0)
{
	m_pIcon = nullptr;
	m_textureID = -1;
	m_dynamicTextureID = -1;
	m_dynamicTexture = false;
	m_flippedX = false;
	m_flippedY = false;
}

Icon::Icon(Renderer* pRenderer) :
	RenderRectangle(pRenderer),
	m_textureWidth(0), m_textureHeight(0),
	m_textureWidthPower2(0), m_textureHeightPower2(0),
	m_textureDrawBuffer(0)
{
	m_pIcon = nullptr;
	m_textureID = -1;
	m_dynamicTextureID = -1;
	m_dynamicTexture = false;
	m_flippedX = false;
	m_flippedY = false;
}

Icon::Icon(Renderer* pRenderer, const std::string& fileName, int width, int height) :
	RenderRectangle(pRenderer), m_fileName(fileName)
{
	m_textureID = -1;
	m_dynamicTextureID = -1;
	m_pRenderer->LoadTexture(fileName.c_str(), &m_textureWidth, &m_textureHeight, &m_textureWidthPower2, &m_textureHeightPower2, &m_textureID);

	m_pIcon = nullptr;

	// Set dimensions
	Icon::SetDimensions(0, 0, width, height);

	CreateDrawBuffer();

	m_dynamicTexture = false;
	m_flippedX = false;
	m_flippedY = false;
}

void Icon::CreateDrawBuffer()
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
		//u			v
		{ 0.0f,		1.0f },
		{ 1.0f,		1.0f },
		{ 1.0f,		0.0f },
		{ 0.0f,		0.0f },
	};

	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

	m_pRenderer->CreateStaticBuffer(VertexType::POSITION_NORMAL_UV, -1, -1, 4, 4, 6, &vertices, &textureCoordinates, indices, &m_textureDrawBuffer);
}

Icon::~Icon()
{
	if (m_pIcon != nullptr)
	{
		Container::Remove(m_pIcon);
		delete m_pIcon;
	}
}

std::string Icon::GetFileName() const
{
	return m_fileName;
}

void Icon::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	if (m_pIcon != nullptr)
	{
		// Also set the dimensions of the icon
		m_pIcon->SetDimensions(0, 0, width, height);
	}
}

void Icon::SetIcon(RenderRectangle* icon)
{
	MultiTextureIcon* pMulti = dynamic_cast<MultiTextureIcon*>(icon);
	Icon* pIcon = dynamic_cast<Icon*>(icon);
	DirectDrawRectangle* pDirectDraw = dynamic_cast<DirectDrawRectangle*>(icon);

	if (m_pIcon)
	{
		// If we already own an icon, remove it from out component list and also delete it, since we will be replacing it
		Remove(m_pIcon);

		delete m_pIcon;
		m_pIcon = nullptr;
	}

	// Check what type of render rectangle we have been given, and then assign our new data
	if (pMulti)
	{
		MultiTextureIcon* pNewMulti = new MultiTextureIcon(*pMulti);
		m_pIcon = pNewMulti;
	}
	else if (pIcon)
	{
		Icon* pNewIcon = new Icon(*pIcon);
		m_pIcon = pNewIcon;
	}
	else if (pDirectDraw)
	{
		DirectDrawRectangle* pNewDirectDraw = new DirectDrawRectangle(*pDirectDraw);
		m_pIcon = pNewDirectDraw;
	}

	// Re-add the icon to the component list
	Add(m_pIcon);
}

void Icon::SetIcon(const std::string& fileName)
{
	m_pRenderer->LoadTexture(fileName.c_str(), &m_textureWidth, &m_textureHeight, &m_textureWidthPower2, &m_textureHeightPower2, &m_textureID);
}

void Icon::SetDynamicTexture(unsigned int textureID)
{
	m_dynamicTextureID = textureID;
	m_dynamicTexture = true;
}

void Icon::SetFlipped(bool x, bool y)
{
	m_flippedX = x;
	m_flippedY = y;
}

int Icon::GetTextureWidth() const
{
	return m_textureWidth;
}

int Icon::GetTextureHeight() const
{
	return m_textureHeight;
}

int Icon::GetTextureWidthPower2() const
{
	return m_textureWidthPower2;
}

int Icon::GetTextureHeightPower2() const
{
	return m_textureHeightPower2;
}

ComponentType Icon::GetComponentType() const
{
	return ComponentType::Icon;
}

void Icon::DrawSelf()
{
	// TODO: Not used?
	//float paddingWidth = static_cast<float>(m_textureWidthPower2) - static_cast<float>(m_textureWidth); 
	//float paddingHeight = static_cast<float>(m_textureHeightPower2) - static_cast<float>(m_textureHeight);

	float widthRatio = static_cast<float>(m_textureWidthPower2) / static_cast<float>(m_textureWidth);
	float heightRatio = static_cast<float>(m_textureHeightPower2) / static_cast<float>(m_textureHeight);

	float width = m_dimensions.width * widthRatio;
	float height = m_dimensions.height * heightRatio;

	float adjustedPaddingHeight = height - m_dimensions.height;

	if (m_dynamicTexture)
	{
		float dimWidth = static_cast<float>(m_dimensions.width);
		float dimHeight = static_cast<float>(m_dimensions.height);
		float dimDepth = 1.0f;

		float s1 = 0.0f;
		float s2 = 1.0f;
		float t1 = 0.0f;
		float t2 = 1.0f;

		if (m_flippedX)
		{
			s1 = 1.0f;
			s2 = 0.0f;
		}
		if (m_flippedY)
		{
			t1 = 1.0f;
			t2 = 0.0f;
		}

		m_pRenderer->PushMatrix();

		m_pRenderer->TranslateWorldMatrix(0.0f, 0.0f, GetDepth());

		m_pRenderer->SetRenderMode(RenderMode::TEXTURED);
		m_pRenderer->BindRawTextureID(m_dynamicTextureID);
		m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
		m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
		m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);

		m_pRenderer->ImmediateTextureCoordinate(s1, t1);
		m_pRenderer->ImmediateVertex(0.0f, 0.0f, dimDepth);
		m_pRenderer->ImmediateTextureCoordinate(s2, t1);
		m_pRenderer->ImmediateVertex(dimWidth, 0.0f, dimDepth);
		m_pRenderer->ImmediateTextureCoordinate(s2, t2);
		m_pRenderer->ImmediateVertex(dimWidth, dimHeight, dimDepth);
		m_pRenderer->ImmediateTextureCoordinate(s1, t2);
		m_pRenderer->ImmediateVertex(0.0f, dimHeight, dimDepth);

		m_pRenderer->DisableImmediateMode();
		m_pRenderer->DisableTransparency();

		m_pRenderer->PopMatrix();
	}
	else
	{
		if (m_textureID != -1)
		{
			m_pRenderer->PushMatrix();

			m_pRenderer->SetRenderMode(RenderMode::TEXTURED);
			m_pRenderer->SetPrimitiveMode(PrimitiveMode::TRIANGLES);
			m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
			m_pRenderer->TranslateWorldMatrix(0.0f, -adjustedPaddingHeight, GetDepth());
			m_pRenderer->ScaleWorldMatrix(width, height, 0.0f);
			m_pRenderer->BindTexture(m_textureID);
			m_pRenderer->EnableTransparency(BlendFunction::SRC_ALPHA, BlendFunction::ONE_MINUS_SRC_ALPHA);
			m_pRenderer->RenderStaticBuffer(m_textureDrawBuffer);
			m_pRenderer->DisableTransparency();
			m_pRenderer->DisableTexture();

			m_pRenderer->PopMatrix();
		}
	}
}