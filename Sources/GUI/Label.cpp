/*************************************************************************
> File Name: Label.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A simple GUI component that simple displays a text label.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "Label.h"

// Constructor, Destructor
Label::Label(Renderer* pRenderer, unsigned int GUIFont, const std::string &label, Color color)
	: Component(pRenderer), m_color(color), m_outline(false), m_GUIFont(GUIFont)
{
	SetText(label);
	SetVerticalAlignment(VerticalAlignment::Center);
	SetHorizontalAlignment(HorizontalAlignment::Left);

	// Set default dimensions
	Component::SetDimensions(0, 0, 1, 1);
}

Label::~Label()
{
	// Do nothing
}

void Label::SetText(const std::string& label)
{
	m_text = label;
}

const std::string& Label::GetText() const
{
	return m_text;
}

void Label::SetFontID(unsigned int fontID)
{
	m_GUIFont = fontID;
}

unsigned int Label::GetFontID() const
{
	return m_GUIFont;
}

void Label::SetColor(Color color)
{
	m_color = color;
}

void Label::SetOutlineColor(Color color)
{
	m_outlineColor = color;
}

void Label::SetOutlineFont(unsigned int outlineGUIFont)
{
	m_outlineGUIFont = outlineGUIFont;
}

void Label::GetColor(float* r, float* g, float* b, float* a) const
{
	*r = m_color.GetRed();
	*g = m_color.GetGreen();
	*b = m_color.GetBlue();
	*a = m_color.GetAlpha();
}

void Label::GetOutlineColor(float* r, float* g, float* b, float* a) const
{
	*r = m_outlineColor.GetRed();
	*g = m_outlineColor.GetGreen();
	*b = m_outlineColor.GetBlue();
	*a = m_outlineColor.GetAlpha();
}

void Label::SetOutline(bool outline)
{
	m_outline = outline;
}

void Label::SetVerticalAlignment(VerticalAlignment alignment)
{
	m_eVerticalAlignment = alignment;
}

VerticalAlignment Label::GetVerticalAlignment() const
{
	return m_eVerticalAlignment;
}

void Label::SetHorizontalAlignment(HorizontalAlignment alignment)
{
	m_eHorizontalAlignment = alignment;
}

HorizontalAlignment Label::GetHorizontalAlignment() const
{
	return m_eHorizontalAlignment;
}

ComponentType Label::GetComponentType() const
{
	return ComponentType::Label;
}

void Label::Insert(int index, char* character)
{
	m_text.insert(index, character);
}

void Label::Erase(int start, int num)
{
	m_text.erase(start, num);
}

void Label::DrawSelf()
{
	int posX = 0;
	int posY = 0;

	m_pRenderer->SetRenderMode(RenderMode::SOLID);

	m_pRenderer->RenderFreeTypeText(m_GUIFont, static_cast<float>(posX), static_cast<float>(posY), GetDepth(), m_color, 1.0f, "%s", m_text.c_str());

	if (m_outline)
	{
		m_pRenderer->RenderFreeTypeText(m_outlineGUIFont, static_cast<float>(posX), static_cast<float>(posY), GetDepth(), m_outlineColor, 1.0f, "%s", m_text.c_str());
	}

	// TODO: Text bounds checking
	//int stringWidth = m_pRenderer->GetFreeTypeTextWidth(m_GUIFont, "%s", m_text.c_str());
	//int stringHeight = m_pRenderer->GetFreeTypeTextHeight(m_GUIFont, "%s", m_text.c_str());

	//int outlineX1 = 0;
	//int outlineX2 = stringWidth;
	//int outlineY1 = 0;
	//int outlineY2 = stringHeight;

	//m_pRenderer->PushMatrix();
	
	//m_pRenderer->SetRenderMode(RenderMode::WIREFRAME);
	//m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::QUADS);
	//m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
	//m_pRenderer->ImmediateVertex(outlineX1, outlineY1, (int)m_depth);
	//m_pRenderer->ImmediateVertex(outlineX2, outlineY1, (int)m_depth);
	//m_pRenderer->ImmediateVertex(outlineX2, outlineY2, (int)m_depth);
	//m_pRenderer->ImmediateVertex(outlineX1, outlineY2, (int)m_depth);
	//m_pRenderer->DisableImmediateMode();
	
	//m_pRenderer->PopMatrix();
}