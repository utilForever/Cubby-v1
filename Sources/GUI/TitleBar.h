/*************************************************************************
> File Name: TitleBar.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A title bar that is used for GUI windows, displays the window
>    title and any buttons for close, minimize, etc..
> Created Time: 2016/08/15
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_TITLE_BAR_H
#define CUBBY_TITLE_BAR_H

#include "Container.h"
#include "GUIWindowCloseButton.h"
#include "GUIWindowMinimizeButton.h"
#include "Label.h"

class TitleBar : public Container, public MouseListener
{
public:
	// Constructor, Destructor
	TitleBar(Renderer* pRenderer, unsigned int GUIFont, const std::string& title);
	TitleBar(Renderer* pRenderer, unsigned int GUIFont, const std::string& title, GUIWindowCloseButton* pCloseButton, GUIWindowMinimizeButton* pMinimizeButton);

	~TitleBar();

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	GUIWindowCloseButton* GetCloseButton() const;
	GUIWindowMinimizeButton* GetMinimizeButton() const;

	void SetGUIWindowParent(GUIWindow* pParent);

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	void SetTitleOffset(int xOffset, int yOffset);

	void SetBackgroundIcon(RenderRectangle* icon);

	void SetTitle(const std::string& title);
	std::string GetTitle() const;

	void SetOutlineRender(bool outline);

	void AllowMinimizing(bool val) const;
	void AllowClosing(bool val) const;

	ComponentType GetComponentType() const override;

protected:
	void MouseEntered(const MouseEvent& event) override;
	void MouseExited(const MouseEvent& event) override;
	void MousePressed(const MouseEvent& event) override;
	void MouseReleased(const MouseEvent& event) override;
	void MouseClicked(const MouseEvent& event) override;
	void MouseDragged(const MouseEvent& event) override;
	void MouseDraggedOutside(const MouseEvent& event) override;

	virtual void OnMouseEnter();
	virtual void OnMouseExit();
	virtual void OnMousePressed();
	virtual void OnMouseReleased();
	virtual void OnMouseClicked();
	virtual void OnMouseDragged();
	virtual void OnMouseDraggedOutside();

	void DrawSelf() override;

private:
	GUIWindow* m_pParent;

	Label m_title;
	GUIWindowCloseButton* m_closeButton;
	GUIWindowMinimizeButton* m_minimizeButton;

	RenderRectangle* m_pBackgroundIcon;

	bool m_ownsButtons;

	bool m_outlineRender;

	bool m_firstOutsideDrag;
	int m_lastX;
	int m_lastY;

	bool m_dragging;
	int m_nextX;
	int m_nextY;
};

#endif