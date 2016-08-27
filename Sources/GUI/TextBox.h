/*************************************************************************
> File Name: TextBox.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A text entry box that accepts focus events and key events.
> Created Time: 2016/08/28
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_TEXT_BOX_H
#define CUBBY_TEXT_BOX_H

#include <functional>

#include <Utils/CountdownTimer.h>

#include "Container.h"
#include "Label.h"
#include "RenderRectangle.h"

using FunctionCallback = std::function<void(void*)>;

class TextBox : public Container, public MouseListener, public FocusListener, public KeyListener
{
public:
	// Constructor, Destructor
	TextBox(Renderer* pRenderer, unsigned int GUIFont, const std::string& defaultText, const std::string& name);
	~TextBox();

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	void SetLabelPositionOffset(int offsetX, int offsetY);

	void SetTextColor(Color color);

	void SetBackgroundIcon(RenderRectangle* icon);

	void SetPipHeight(int height);
	void SetTextIndent(int indent);
	void SetMaxTextLength(int length);
	void SetPipeColor(Color pipeColor);

	void SetText(const std::string& label);
	const std::string& GetText() const;

	void SetHiddenText(bool isHidden);
	bool IsHiddenText() const;

	void SetHideCharacter(char hideChar);
	char GetHideCharacter() const;

	void SetDontLoseFocus(bool isDontLoseFocus);
	bool IsDontLoseFocusSet() const;

	void SetCallBackFunction_OnReturnPressed(FunctionCallback func);
	void SetCallBackData_OnReturnPressed(void* pData);

	ComponentType GetComponentType() const override;

protected:
	void KeyPressed(const KeyEvent& event) override;
	void KeyReleased(const KeyEvent& event) override;
	void KeyTyped(const KeyEvent& event) override;
	void CharacterEntered(const KeyEvent& event) override;

	void FocusLost(const FocusEvent& event) override;
	void FocusGained(const FocusEvent& event) override;

	virtual void OnReturnPressed();

	void DrawSelf() override;
	void DrawChildren() override;

private:
	static void _TogglePipeDisplay(void* pData);
	void TogglePipeDisplay();

protected:
	FunctionCallback m_callbackOnReturnPressed;
	void* m_pCallbackDataOnReturnPressed;

private:
	Label m_text;
	Label m_name;

	RenderRectangle* m_pBackgroundIcon;

	unsigned int m_GUIFont;

	int m_highlightStartIndex;
	int m_highlightEndIndex;

	int m_pipeCharacterIndex;
	bool m_isDisplayPipe;
	CountdownTimer* m_pPipeDisplayCountDown;

	int m_pipeHeight;
	int m_textIndent;
	int m_maxTextLength;
	Color m_pipeColor;

	bool m_isShiftHeld;

	bool m_isHiddenText;
	char m_hideCharacter;

	bool m_isDontLoseFocus;
};

#endif