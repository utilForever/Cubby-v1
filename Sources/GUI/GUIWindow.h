/*************************************************************************
> File Name: GUIWindow.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A window container class, defines basic window functionality.
> Created Time: 2016/08/15
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_GUI_WINDOW_H
#define CUBBY_GUI_WINDOW_H

#include "Container.h"
#include "RenderRectangle.h"
#include "TitleBar.h"

// Forward declaration
class GUIWindow;
class OpenGLGUI;

using GUIWindowList = std::vector<GUIWindow*>;

class GUIWindow : public Container, public MouseListener
{
public:
	// Constructor, Destructor
	GUIWindow(Renderer* pRenderer, unsigned int GUIFont, const std::string& title);
	GUIWindow(Renderer* pRenderer, unsigned int GUIFont, TitleBar* ptitleBar);

	~GUIWindow();

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	bool IsRootContainer() const override;

	void AddGUIWindow(GUIWindow* window);
	void RemoveGUIWindow(GUIWindow* window);
	void RemoveAllGUIWindows();

	const GUIWindowList& GetGUIWindows() const;

	TitleBar* GetTitleBar() const;

	void SetTitleBarDimensions(int xOffset, int yOffset, int width, int height) const;
	void SetTitleOffset(int xOffset, int yOffset) const;
	void SetTitlebarBackgroundIcon(RenderRectangle* icon) const;

	void SetBackgroundIcon(RenderRectangle* icon);

	void AddComponent(Component* component);
	void RemoveComponent(Component *component);

	void SetDimensions(int x, int y, int width, int height) override;
	void SetDimensions(const Dimensions& r) override;

	void SetLocation(int x, int y) override;
	void SetLocation(const Point& p) override;

	void SetTitle(const std::string &title) const;
	std::string GetTitle() const;

	void SetDebugRender(bool debug) override;
	void SetOutlineRender(bool outline);

	void Show();
	void Hide();

	bool GetMinimized() const;
	void SetMinimized(bool minimized);

	void SetMinimizedDefaultIcon(RenderRectangle* icon);
	void SetMinimizedSelectedIcon(RenderRectangle* icon);
	void SetMinimizedHoverIcon(RenderRectangle* icon);
	void SetMinimizedDisabledIcon(RenderRectangle* icon);

	void SetCloseDefaultIcon(RenderRectangle* icon);
	void SetCloseSelectedIcon(RenderRectangle* icon);
	void SetCloseHoverIcon(RenderRectangle* icon);
	void SetCloseDisabledIcon(RenderRectangle* icon);

	void AllowMoving(bool val);
	void AllowClosing(bool val);
	void AllowMinimizing(bool val);
	void AllowScrolling(bool val);
	void SnapToApplication(bool val);

	void SetApplicationDimensions(int width, int height);

	void SetApplicationBorder(int left, int right, int top, int bottom);

	void SetRenderTitleBar(bool render);
	void SetRenderWindowBackground(bool render);

	void DepthSortGUIWindowChildren() const;

	ComponentType GetComponentType() const override;

	void SetGUIParent(OpenGLGUI* pParent);
	void SetFocusWindow();

	void Update(float deltaTime) override;

	// < Operator (Used for GUIWindow depth sorting)
	bool operator<(const GUIWindow& w) const;
	static bool DepthLessThan(const GUIWindow* lhs, const GUIWindow* rhs);

protected:
	void MousePressed(const MouseEvent& event) override;

	void DrawSelf() override;
	void DrawChildrenFirst();
	void DrawChildren() override;

private:
	GUIWindowList m_vpGUIWindowList;

	TitleBar* m_titleBar;

	unsigned int m_GUIFont;

	bool m_minimized;

	// Boolean to control functionality of window
	bool m_allowMoving;
	bool m_allowClosing;
	bool m_allowMinimizing;
	bool m_allowScrolling;
	bool m_snapToWindow;

	bool m_bRenderTitleBar;
	bool m_bRenderWindowBackground;

	bool m_ownsTitleBar;

	int m_applicationWidth;
	int m_applicationHeight;

	RenderRectangle* m_pBackgroundIcon;

	bool m_outlineRender;

	int m_applicationBorderLeft;
	int m_applicationBorderRight;
	int m_applicationBorderTop;
	int m_applicationBorderBottom;

	OpenGLGUI* m_pParentGUI;

	// Friend classes
	friend class TitleBar;
	friend class GUIWindowMinimizeButton;
	friend class GUIWindowCloseButton;
};

#endif