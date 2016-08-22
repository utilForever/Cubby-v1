/*************************************************************************
> File Name: ScrollBar.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A scrollbar GUI component to allow scrolling horizontal or vertically. Can
>    be used for a variety of purposes, but mainly for scrolling large windows.
> Note
>    When being used in vertical mode, the left arrow is on top and the right
>    arrow is on the bottom.
> Created Time: 2016/08/21
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_SCROLL_BAR_H
#define CUBBY_SCROLL_BAR_H

#include <Utils/CountdownTimer.h>
#include <Utils/Interpolator.h>

#include "Container.h"
#include "MouseListener.h"
#include "Icon.h"

enum class ScrollBarDirection
{
	Horizontal = 0,
	Vertical,
};


class ScrollBar : public Container, public MouseListener
{
public:
	// Constructor, Destructor
	ScrollBar(Renderer* pRenderer);
	ScrollBar(Renderer* pRenderer, const std::string& leftDefaultTexture, const std::string& leftPressedTexture, const std::string& leftHoverTexture, const std::string& leftDisabledTexture,
		const std::string& rightDefaultTexture, const std::string& rightPressedTexture, const std::string& rightHoverTexture, const std::string& rightDisabledTexture,
		const std::string& scrollDefaultTexture, const std::string& scrollPressedTexture, const std::string& scrollHoverTexture, const std::string& scrollDisabledTexture,
		const std::string& backbarTexture);

	virtual ~ScrollBar();

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	void RemoveIcon(RenderRectangle* pRemoveIcon);
	void SetDefaultIcons(Renderer* pRenderer);
	void SetLeftArrowDefaultIcon(RenderRectangle* icon);
	void SetLeftArrowHoverIcon(RenderRectangle* icon);
	void SetLeftArrowSelectedIcon(RenderRectangle* icon);
	void SetLeftArrowDisabledIcon(RenderRectangle* icon);
	void SetRightArrowDefaultIcon(RenderRectangle* icon);
	void SetRightArrowHoverIcon(RenderRectangle* icon);
	void SetRightArrowSelectedIcon(RenderRectangle* icon);
	void SetRightArrowDisabledIcon(RenderRectangle* icon);
	void SetScrollbarDefaultIcon(RenderRectangle* icon);
	void SetScrollbarHoverIcon(RenderRectangle* icon);
	void SetScrollbarSelectedIcon(RenderRectangle* icon);
	void SetScrollbarDisabledIcon(RenderRectangle* icon);
	void SetBackbarIcon(RenderRectangle* icon);

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	void SetArrowDimensions(int width, int height) const;

	void AddScrollAreaItem(Component* component);
	void ClearScrollAreaItems();

	void SetScrollArea(int x, int y, int width, int height);
	Dimensions GetScrollArea() const;

	void SetScrollPosition(float position);
	float GetScrollPosition() const;

	void SetScrollSize(float lSize);
	float GetScrollSize() const;

	float GetScrollRatio() const; // Returns a 0.0f -> 1.0f value based on where the scroll bar is currently positioned

	void UpdateScrollbarDimension() const;

	void ClampScrollBarPosition();

	void SetScissorEnabled(bool scissor);

	ScrollBarDirection GetScrollDirection() const;
	void SetScrollDirection(ScrollBarDirection direction);

	void SetSelectedRight(bool val);	// Bottom when in vertical mode
	void SetHoverRight(bool val);		// Bottom when in vertical mode
	void SetSelectedLeft(bool val);		// Top when in vertical mode
	void SetHoverLeft(bool val);		// Top when in vertical mode
	void SetDisabled(bool val);

	bool IsSelectedRight() const;
	bool IsHoverRight() const;
	bool IsSelectedLeft() const;
	bool IsHoverLeft() const;
	bool IsDisabled() const;

	ComponentType GetComponentType() const override;

	void SetValueChangedCallback(FunctionCallback func);
	void SetValueChangedCallBackData(void* pData);

	void SetAudio(bool set) override;
	void SetAudioVolume(float volume) override;

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
	virtual void OnMouseMotion();

	virtual void OnValueChanged();

	void DrawChildren() override;
	void DrawSelf() override;

private:
	static void _UpdateArrowButtons(void* pData);
	void UpdateArrowButtons();

	RenderRectangle* m_pLeftArrowDefault;		// Top when in vertical mode
	RenderRectangle* m_pLeftArrowHover;
	RenderRectangle* m_pLeftArrowPressed;
	RenderRectangle* m_pLeftArrowDisabled;

	RenderRectangle* m_pRightArrowDefault;		// Bottom when in vertical mode
	RenderRectangle* m_pRightArrowHover;
	RenderRectangle* m_pRightArrowPressed;
	RenderRectangle* m_pRightArrowDisabled;

	RenderRectangle* m_pScrollbarDefault;
	RenderRectangle* m_pScrollbarHover;
	RenderRectangle* m_pScrollbarPressed;
	RenderRectangle* m_pScrollbarDisabled;

	RenderRectangle* m_pBackbar;

	bool m_selectedRight;
	bool m_hoverRight;
	bool m_selectedLeft;
	bool m_hoverLeft;
	bool m_disabled;

	bool m_hoverScrollBar;
	bool m_draggingScrollBar;
	bool m_firstOutsideDrag;

	float m_scrollPosition;
	float m_ScrollSize;

	int m_lastX;
	int m_lastY;

	bool m_scissorEnabled;

	Dimensions m_scrollArea;

	ComponentList m_vpScrollAreaComponentList;

	CountdownTimer* m_pArrowButtonUpdate;

	ScrollBarDirection m_scrollDirection;

	// Function callback for when this scrollbar value is changed
	FunctionCallback m_valueChangedCallback;
	void* m_pValueChangedCallbackData;
};

#endif