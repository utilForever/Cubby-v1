/*************************************************************************
> File Name: Slider.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A slider GUI component to allow quick selection of range values.
> Created Time: 2016/08/27
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_SLIDER_H
#define CUBBY_SLIDER_H

#include <functional>

#include "Button.h"
#include "Container.h"
#include "Label.h"

enum class SliderDirection
{
	Horizontal = 0,
	Vertical,
};

using FunctionCallback = std::function<void(void*)>;

class Slider : public Container, public MouseListener
{
public:
	// Constructor, Destructor
	Slider(Renderer* pRenderer, unsigned int GUIFont, const std::string& label, float min, float max, float defaultValue, bool addIncrementButtons = false, float incrementAmount = 0.01f);
	Slider(Renderer* pRenderer, unsigned int GUIFont, unsigned int OutlineGUIFont, const std::string& label, float min, float max, float defaultValue, Color fontColor, Color outlineColor, bool addIncrementButtons = false, float incrementAmount = 0.01f);

	virtual ~Slider();

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	void SetScrollerDefaultIcon(RenderRectangle* icon);
	void SetScrollerHoverIcon(RenderRectangle* icon);
	void SetScrollerDraggingIcon(RenderRectangle* icon);
	void SetScrollerDisabledIcon(RenderRectangle* icon);
	void SetScrollBackbarDefault(RenderRectangle* icon);
	void SetScrollBackbarPressed(RenderRectangle* icon);
	void SetScrollBackbarIncrementIconDefault(RenderRectangle* icon) const;
	void SetScrollBackbarIncrementIconHover(RenderRectangle* icon) const;
	void SetScrollBackbarIncrementIconPressed(RenderRectangle* icon) const;
	void SetScrollBackbarIncrementIconDisabled(RenderRectangle* icon) const;
	void SetScrollBackbarDecrementIconDefault(RenderRectangle* icon) const;
	void SetScrollBackbarDecrementIconHover(RenderRectangle* icon) const;
	void SetScrollBackbarDecrementIconPressed(RenderRectangle* icon) const;
	void SetScrollBackbarDecrementIconDisabled(RenderRectangle* icon) const;

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	void SetShowValue(bool show);
	void SetValuePrecision(int precision);
	void SetValuePositionOffset(int offsetX, int offsetY);

	void SetLabelPositionOffset(int offsetX, int offsetY);

	void SetIncrementButtonsOffset(int offsetX, int offsetY);

	void UpdateTextString();

	void SetSliderParams(int barHeight, int sliderWidth, SliderDirection direction);

	float GetMinValue() const;
	float GetMaxValue() const;
	float GetDefaultValue() const;
	float GetCurrentValue() const;

	void SetMinValue(float minValue);
	void SetMaxValue(float maxValue);
	void SetDefaultValue(float defaultValue);
	void SetCurrentValue(float currentValue, bool supressCallback = false);

	void SetRenderBackgroundBar(bool render);
	void SetDynamicBackgroundColor(Color topLeft, Color bottomLeft, Color topRight, Color bottomRight);

	SliderDirection GetSliderDirection() const;
	void SetSliderDirection(SliderDirection direction);

	void SetDisabled(bool val);

	ComponentType GetComponentType() const override;

	void SetVaueChangedCallBackFunction(FunctionCallback func);
	void SetVaueChangedCallBackData(void* pData);

protected:
	void MouseEntered(const MouseEvent& event) override;
	void MouseExited(const MouseEvent& event) override;
	void MousePressed(const MouseEvent& event) override;
	void MouseReleased(const MouseEvent& event) override;
	void MouseReleasedOutside(const MouseEvent& event) override;
	void MouseClicked(const MouseEvent& event) override;
	void MouseDragged(const MouseEvent& event) override;
	void MouseDraggedOutside(const MouseEvent& event) override;
	void MouseMotion(const MouseEvent& event) override;

	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnMousePressed();
	virtual void OnMouseReleased();
	virtual void OnMouseClicked();

	virtual void OnValueChanged();

	void DrawSelf() override;

private:
	void ValueChanged(bool supressCallback = false);

	static void _PlusPressed(void* pData);
	void PlusPressed();

	static void _MinusPressed(void* pData);
	void MinusPressed();

protected:
	Label m_value;
	Label m_name;

private:
	float m_minValue;
	float m_maxValue;
	float m_defaultValue;

	float m_currentValue;

	SliderDirection m_sliderDirection;

	bool m_isShowValue;
	int m_valuePrecision;

	int m_barHeight;
	int m_sliderWidth;

	bool m_isDragging;
	bool m_isAllowDragging;
	bool m_isHover;
	bool m_isDragginOutside;
	bool m_isFirstOutsideDrag;
	bool m_isAllowDragginOutside;
	bool m_isPressedBar;
	bool m_isAllowReleasingOnBar;
	bool m_isOverBar;

	bool m_isRenderBackgroundBar;

	bool m_isDynamicColorBackground;
	Color m_backgroundColorTopLeft;
	Color m_backgroundColorTopRight;
	Color m_backgroundColorBottomLeft;
	Color m_backgroundColorBottomRight;

	int m_lastX;
	int m_lastY;

	bool m_isDisabled;

	unsigned int m_fontID;

	RenderRectangle* m_pScrollerDefault;
	RenderRectangle* m_pScrollerHover;
	RenderRectangle* m_pScrollerDragging;
	RenderRectangle* m_pScrollerDisabled;
	RenderRectangle* m_pScrollBackbarDefault;
	RenderRectangle* m_pScrollBackbarPressed;

	Button* m_pPlusButton;
	Button* m_pMinusButton;

	bool m_incrementButtons;
	float m_incrementAmount;

	int m_labelOffsetX;
	int m_labelOffsetY;
	int m_valueOffsetX;
	int m_valueOffsetY;
	int m_incrementButtonsOffsetX;
	int m_incrementButtonsOffsetY;

	// Function callback for when this slider has a value changed event
	FunctionCallback m_valueChangedCallback;
	void* m_pValueChangedCallbackData;
};

#endif