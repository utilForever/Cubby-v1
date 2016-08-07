/*************************************************************************
> File Name: Component.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An abstract class for GUI components.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_COMPONENT_H
#define CUBBY_COMPONENT_H

#include <vector>

#include <Renderer/Renderer.h>

#include "Dimensions.h"
#include "Point.h"
#include "FocusListener.h"
#include "KeyListener.h"
#include "MouseListener.h"

struct Point;
using FocusListenerList = std::vector<FocusListener*>;
using KeyListenerList = std::vector<KeyListener*>;
using MouseListenerList = std::vector<MouseListener*>;

enum class VerticalAlignment
{
	Top = 0,
	Center,
	Bottom
};

enum class HorizontalAlignment
{
	Left = 0,
	Center,
	Right
};

enum class ComponentType
{
	Component = 0,
	Container,
	AbstractButton,
	Button,
	CheckBox,
	OptionBox,
	OptionController,
	MenuBar,
	Menu,
	MenuItem,
	Label,
	RenderRectangle,
	Icon,
	MultiTextureIcon,
	DirectDrawRectangle,
	DraggableRenderRectangle,
	Slider,
	TitleBar,
	TextBox,
	MultiLineTextBox,
	ProgressBar,
	ScrollBar,
	PulldownMenu,
	GUIWindow,
	TreeView,

	Num,
};

class Component
{
public:
	// Constructor, Destructor
	Component(Renderer* pRenderer);
	virtual ~Component();

	virtual void SetVisible(bool visible);
	virtual bool IsVisible() const;

	virtual void SetEnabled(bool enabled);
	bool IsEnabled() const;

	bool IsParentVisible() const;
	bool IsParentEnabled() const;

	void SetTransparency(float transparency);
	float GetTransparency() const;

	void SetDepth(float depth);
	float GetDepth() const;

	void SetParent(Component* pParent);
	Component* GetParent() const;

	virtual ComponentType GetComponentType() const;

	virtual void SetLocation(int x, int y);
	virtual void SetLocation(const Point& p);
	void SetX(int x);
	void SetY(int y);
	void GetLocation(int& x, int& y) const;
	Point GetLocation() const;
	Point GetLocationOnScreen() const;

	void SetSize(int width, int height);
	void GetSize(int& width, int& height) const;

	virtual void SetDebugRender(bool debug);
	bool IsDebugRender() const;

	virtual void SetDimensions(int x, int y, int width, int height);
	virtual void SetDimensions(const Dimensions& r);
	virtual void SetWidth(int width);
	virtual void SetHeight(int height);
	Dimensions GetDimensions() const;
	Dimensions* GetDimensionsPointer();

	virtual void AddEventListeners();
	virtual void RemoveEventListeners();

	bool HasMouseListener() const;
	void AddMouseListener(MouseListener* listener);
	void RemoveMouseListener(MouseListener* listener);
	void RemoveMouseListeners();
	void ProcessMouseEvent(const MouseEvent& event);

	bool HasKeyListener() const;
	void AddKeyListener(KeyListener* listener);
	void RemoveKeyListener(KeyListener* listener);
	void RemoveKeyListeners();
	void ProcessKeyEvent(const KeyEvent& event);

	bool HasFocusListener() const;
	void AddFocusListener(FocusListener* listener);
	void RemoveFocusListener(FocusListener* listener);
	void RemoveFocusListeners();
	void ProcessFocusEvent(const FocusEvent& event);

	bool HasFocus() const;
	void SetFocus(bool enable);

	virtual void SetAudio(bool set);
	virtual void SetAudioVolume(float volume);

	virtual bool IsContainer() const;
	virtual bool IsRootContainer() const;

	virtual void Update(float deltaTime);

	void Draw();

	// < Operator (Used for component depth sorting)
	bool operator<(const Component& w) const;
	static bool DepthLessThan(const Component* lhs, const Component* rhs);

protected:
	virtual void DrawSelf();
	virtual void DrawChildren();

private:
	void DrawDebug() const;

protected:
	Dimensions m_dimensions;
	Renderer* m_pRenderer;

	float m_depth;

	bool m_audio;
	float m_audioVolume;

private:
	bool m_visible;
	bool m_enabled;
	bool m_focused;

	bool m_debugRender;

	float m_transparency;

	MouseListenerList m_vpMouseListenerList;
	KeyListenerList m_vpKeyListenerList;
	FocusListenerList m_vpFocusListenerList;

	bool m_mouseListenerRemoved;
	bool m_keyListenerRemoved;
	bool m_focusListenerRemoved;

	Component* m_pParent;
};

#endif