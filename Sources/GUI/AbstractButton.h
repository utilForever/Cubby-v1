/*************************************************************************
> File Name: AbstractButton.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    An abstract button class used as a base
>    class for all button related components.
> Created Time: 2016/08/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_ABSTRACT_BUTTON_H
#define CUBBY_ABSTRACT_BUTTON_H

#include <functional>

#include "Container.h"
#include "Label.h"
#include "RenderRectangle.h"

class AbstractButton : public Container
{
public:
	// Constructor, Destructor
	AbstractButton(Renderer* pRenderer, unsigned int GUIFont, const std::string& label);
	virtual ~AbstractButton();

	void SetLabel(const std::string& label);

	virtual void RemoveIcon(RenderRectangle* pRemoveIcon);
	virtual void RemoveIcon(const std::string& fileName);
	virtual void SetDefaultIcons(Renderer* pRenderer);
	void SetDefaultIcon(RenderRectangle* icon);
	void SetSelectedIcon(RenderRectangle* icon);
	void SetHoverIcon(RenderRectangle* icon);
	void SetDisabledIcon(RenderRectangle* icon);

	Label GetLabel() const;

	void SetDisplayLabel(bool display);

	void SetSelected(bool val);
	void SetHover(bool val);
	void SetDisabled(bool val);

	bool IsSelected() const;
	bool IsHover() const;
	bool IsDisabled() const;

	void SetCallBackFunction(std::function<void(void*)> func);
	void SetCallBackData(void* pData);

	void SetCallBackFunctionPressed(std::function<void(void*)> func);
	void SetCallBackDataPressed(void* pData);

	void SetCallBackFunctionReleased(std::function<void(void*)> func);
	void SetCallBackDataReleased(void* pData);

	void SetEnterCallBackFunction(std::function<void(void*)> func);
	void SetEnterCallBackData(void* pData);

	void SetExitCallBackFunction(std::function<void(void*)> func);
	void SetExitCallBackData(void* pData);

	ComponentType GetComponentType() const override;

protected:
	RenderRectangle* m_pDefaultIcon;
	RenderRectangle* m_pSelectedIcon;
	RenderRectangle* m_pHoverIcon;
	RenderRectangle* m_pDisabledIcon;

	Label m_label;

	bool m_displayLabel;

	int m_labelOffsetX;
	int m_labelOffsetY;

	// Function callback for when this button is pressed and released
	std::function<void(void*)> m_callback;
	void* m_pCallbackData;

	// Function callback for when this button is pressed
	std::function<void(void*)> m_callbackPressed;
	void* m_pCallbackDataPressed;

	// Function callback for when this button is released
	std::function<void(void*)> m_callbackReleased;
	void* m_pCallbackDataReleased;

	// Function callback for when this button is entered
	std::function<void(void*)> m_EnterCallback;
	void* m_pEnterCallbackData;

	// Function callback for when this button is exited
	std::function<void(void*)> m_ExitCallback;
	void* m_pExitCallbackData;

private:
	bool m_selected;
	bool m_hover;
	bool m_disabled;
};

#endif