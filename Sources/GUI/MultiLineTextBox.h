/*************************************************************************
> File Name: MultiLineTextBox.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A multi-line text entry box that accepts focus events and key events.
> Created Time: 2016/08/22
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_MULTI_LINE_TEXT_BOX_H
#define CUBBY_MULTI_LINE_TEXT_BOX_H

#include <functional>

#include "Container.h"
#include "Label.h"
#include "RenderRectangle.h"
#include "ScrollBar.h"

using FunctionCallback = std::function<void(void*)>;

class MultiLineTextBox : public Container, public MouseListener, public FocusListener, public KeyListener
{
public:
	// Constructor, Destructor
	MultiLineTextBox(Renderer* pRenderer, unsigned int GUIFont, const std::string& defaultText);
	~MultiLineTextBox();

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	void SetTextColor(Color color) const;

	void SetBackgroundIcon(RenderRectangle* icon);

	void SetPipHeight(int height);
	void SetTextIndent(int indent);
	void SetMaxTextLength(int length);
	void SetPipeColor(Color pipeColor);

	void SetText(const std::string& label);
	const std::string& GetText() const;

	void SetVerticalAlignment(VerticalAlignment alignment);
	VerticalAlignment GetVerticalAlignment() const;

	void SetHorizontalAlignment(HorizontalAlignment alignment);
	HorizontalAlignment GetHorizontalAlignment() const;

	void SetHiddenText(bool hidden);
	bool IsHiddenText() const;

	void SetHideCharacter(char hideChar);
	char GetHideCharacter() const;

	void SetCallBackFunction_OnReturnPressed(FunctionCallback func);
	void SetCallBackData_OnReturnPressed(void* pData);

	ComponentType GetComponentType() const override;

	void UpdateText();

	void ReAlignText();

	void UpdateScrollbar();

protected:
	void KeyPressed(const KeyEvent& event) override;
	void KeyReleased(const KeyEvent& event) override;
	void KeyTyped(const KeyEvent& event) override;
	void CharacterEntered(const KeyEvent& event) override;

	void FocusLost(const FocusEvent& event) override;
	void FocusGained(const FocusEvent& event) override;

	virtual void OnReturnPressed();

	void ParseText();

	void ClearLabels();

	void DrawChildren() override;
	void DrawSelf() override;

private:
	static void _TogglePipeDisplay(void* pData);
	void TogglePipeDisplay();

	static void _ScrollbarChanged(void* pData);
	void ScrollbarChanged();

protected:
	FunctionCallback m_callbackOnReturnPressed;
	void* m_pCallbackDataOnReturnPressed;

private:
	std::string m_text;

	std::vector<Label> m_labelList;

	int m_numLines;

	VerticalAlignment m_verticalAlignment;
	HorizontalAlignment m_horizontalAlignment;

	RenderRectangle* m_pBackgroundIcon;

	unsigned int m_GUIFont;

	int m_highlightStartIndex;
	int m_highlightEndIndex;

	int m_pipeCharacterIndex;
	bool m_displayPipe;
	CountdownTimer* m_pPipeDisplayCountDown;

	int m_pipeHeight;
	int m_textIndent;
	int m_maxTextLength;
	Color m_pipeColor;
	int m_pipeYOffset;

	bool m_newLinePipeRender;

	bool m_shiftHeld;

	bool m_hiddenText;
	char m_hideCharacter;

	ScrollBar m_scrollBar;
};

#endif