/*************************************************************************
> File Name: OptionController.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A controller for a group of option boxes.
> Created Time: 2016/08/24
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "OptionController.h"

// Constructor, Destructor
OptionController::OptionController(Renderer* pRenderer, unsigned int GUIFont, const std::string& label) :
	Container(pRenderer), m_label(pRenderer, GUIFont, label, Color(1.0f, 1.0f, 1.0f))
{
	m_label.SetLocation(0, 0);
	Container::Add(&m_label);

	m_labelIndent = 5;

	m_pActiveOption = nullptr;

	m_displayLabel = false;
	m_displayBorder = false;
}

OptionController::OptionController(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color fontColor, Color outlineColor) :
	Container(pRenderer), m_label(pRenderer, GUIFont, label, Color(1.0f, 1.0f, 1.0f))
{
	m_label.SetLocation(0, 0);
	m_label.SetColor(fontColor);
	m_label.SetOutline(true);
	m_label.SetOutlineColor(outlineColor);
	m_label.SetOutlineFont(outlineGUIFont);
	m_label.SetDepth(3.0f);
	Container::Add(&m_label);

	m_labelIndent = 5;

	m_pActiveOption = nullptr;

	m_displayLabel = false;
	m_displayBorder = false;
}

OptionController::~OptionController()
{
	// Do nothing
}

void OptionController::SetVisible(bool visible)
{
	// Also set the visibility of all our children too
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if ((*iter)->GetComponentType() == ComponentType::OptionBox)
		{
			OptionBox* pOptionBox = static_cast<OptionBox*>(*iter);

			pOptionBox->SetVisible(visible);
		}
	}
}

bool OptionController::IsVisible() const
{
	return Component::IsVisible();
}

void OptionController::SetDimensions(int x, int y, int width, int height)
{
	Component::SetDimensions(x, y, width, height);

	// Also set the location of the label
	m_label.SetLocation(m_labelIndent, height);
}

void OptionController::SetDimensions(const Dimensions& r)
{
	SetDimensions(r.x, r.y, r.width, r.height);
}

void OptionController::SetDisplayLabel(bool display)
{
	m_displayLabel = display;
}

void OptionController::SetDisplayBorder(bool display)
{
	m_displayBorder = display;
}

void OptionController::SetActiveOption(OptionBox* pOption)
{
	m_pActiveOption = pOption;
}

OptionBox* OptionController::GetActiveOption() const
{
	return m_pActiveOption;
}

void OptionController::ResetAllOptions()
{
	m_pActiveOption = nullptr;

	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if ((*iter)->GetComponentType() == ComponentType::OptionBox)
		{
			OptionBox* pOptionBox = static_cast<OptionBox*>(*iter);

			pOptionBox->SetToggled(false);
		}
	}
}

ComponentType OptionController::GetComponentType() const
{
	return ComponentType::OptionController;
}

void OptionController::DrawSelf()
{
	if (m_displayBorder)
	{
		int containerWidth = m_dimensions.width;
		int containerHeight = m_dimensions.height;
		float depth = GetDepth();

		float outlineX1 = 0.5f;
		float outlineX2 = containerWidth + 0.5f;
		float outlineY1 = 0.5f;
		float outlineY2 = containerHeight + 0.5f;

		int labelheightAdjust = m_pRenderer->GetFreeTypeTextHeight(m_label.GetFontID(), "%s", m_label.GetText().c_str()) / 2;
		int labelWidth = m_pRenderer->GetFreeTypeTextWidth(m_label.GetFontID(), "%s", m_label.GetText().c_str());

		if (m_displayLabel)
		{
			m_pRenderer->PushMatrix();

			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINES);

			m_pRenderer->ImmediateVertex(outlineX1 + m_labelIndent, outlineY2 + labelheightAdjust, depth);
			m_pRenderer->ImmediateVertex(outlineX1, outlineY2 + labelheightAdjust, depth);

			m_pRenderer->ImmediateVertex(outlineX1, outlineY2 + labelheightAdjust, depth);
			m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);

			m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);
			m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);

			m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);
			m_pRenderer->ImmediateVertex(outlineX2, outlineY2 + labelheightAdjust, depth);

			m_pRenderer->ImmediateVertex(outlineX2, outlineY2 + labelheightAdjust, depth);
			m_pRenderer->ImmediateVertex(outlineX1 + m_labelIndent + labelWidth + 2, outlineY2 + labelheightAdjust, depth);
			
			m_pRenderer->DisableImmediateMode();
			
			m_pRenderer->PopMatrix();
		}
		else
		{
			m_pRenderer->PushMatrix();
			
			m_pRenderer->SetLineWidth(1.0f);
			m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 1.0f, 1.0f);
			
			m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINE_LOOP);
			m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);
			m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);
			m_pRenderer->ImmediateVertex(outlineX2 + 1, outlineY2, depth);
			m_pRenderer->ImmediateVertex(outlineX1, outlineY2, depth);
			
			m_pRenderer->DisableImmediateMode();
			
			m_pRenderer->PopMatrix();
		}

	}

	// Set the label visibility
	m_label.SetVisible(m_displayLabel);
}