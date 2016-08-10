/*************************************************************************
> File Name: DraggableRenderRectangle.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A Draggable render rectangle widget.
> Created Time: 2016/08/10
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_DRAGGABLE_RENDER_RECTANGLE_H
#define CUBBY_DRAGGABLE_RENDER_RECTANGLE_H

#include <functional>

#include "Container.h"
#include "RenderRectangle.h"

class DraggableRenderRectangle : public Container, public MouseListener
{
public:
	// Constructor, Destructor
	DraggableRenderRectangle(Renderer* pRenderer);
	~DraggableRenderRectangle();

	void SetIcon(RenderRectangle* icon);

	void AddText(Renderer* pRenderer, unsigned int GUIFont, unsigned int outlineGUIFont, const std::string& label, Color color, int xOffset, int yOffset, bool outline = false, Color outlineColor = Color(1.0f, 1.0f, 1.0f, 1.0f));
	void AddIcon(Renderer* pRenderer, const std::string& fileName, int texWidth, int texHeight, int width, int height, int xOffset, int yOffset, float depth);
	void RemoveIcon(const std::string& fileName);

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	void SetPressedCallBackFunction(std::function<void(void*)> func);
	void SetPressedCallBackData(void* pData);

	void SetReleasedCallBackFunction(std::function<void(void*)> func);
	void SetReleasedCallBackData(void* pData);

	void SetEnterCallBackFunction(std::function<void(void*)> func);
	void SetEnterCallBackData(void* pData);

	void SetExitCallBackFunction(std::function<void(void*)> func);
	void SetExitCallBackData(void* pData);

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
	RenderRectangle* m_pIcon;

	ComponentList m_vpAddedComponentList;

	bool m_dragging;
	int m_nextX;
	int m_nextY;

	// Function callback for when this draggable icon is pressed
	std::function<void(void*)> m_pressedCallback;
	void* m_pPressedCallbackData;

	// Function callback for when this draggable icon is released
	std::function<void(void*)> m_releasedCallback;
	void* m_pReleasedCallbackData;

	// Function callback for when this draggable icon is entered
	std::function<void(void*)> m_enterCallback;
	void* m_pEnterCallbackData;

	// Function callback for when this draggable icon is exited
	std::function<void(void*)> m_exitCallback;
	void* m_pExitCallbackData;
};

#endif