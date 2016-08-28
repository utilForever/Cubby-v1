/*************************************************************************
> File Name: OpenGLGUI.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Class to manage everything related to the GUI. Has methods
>    that must be called for the GUI to function properly such as
>    Render() and Update(). Also requires access to input events.
> Created Time: 2016/08/28
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_OPENGL_GUI_H
#define CUBBY_OPENGL_GUI_H

#include "Component.h"
#include "GUIWindow.h"

class OpenGLGUI
{
public:
	// Constructor, Destructor
	OpenGLGUI(Renderer* pRenderer);
	~OpenGLGUI();

	bool IsMouseInteractingWithGUIComponent(bool ignoreGUIWindows) const;
	bool IsKeyboardInteractingWithGUIComponent() const;
	Component* GetCurrentMouseInteractionComponent() const;

	void AddWindow(GUIWindow* window);
	void RemoveWindow(GUIWindow* window);

	void SetFocusedWindow(GUIWindow* window);

	void SetComponentFocus(Component* component) const;
	void ResetFocus() const;

	void AddComponent(Component* component);
	void RemoveComponent(Component* component);

	Component* GetComponent(Component* component);

	void SetDraggingComponentPriority(Component* pDraggingComponentPriority);

	void SetDebugRender(bool debug);
	bool IsDebugRender() const;

	void Render();

	void ResetSelectionManager() const;

	void Update(float deltaTime);

	void ImportMouseMotion(int x, int y);
	void MousePressed(int button) const;
	void MouseReleased(int button) const;

	void KeyPressed(int keyCode, int modifier) const;
	void KeyReleased(int keyCode, int modifier) const;
	void CharacterEntered(int characterCode) const;

	void DepthSortGUIWindowChildren();
	void DepthSortComponentChildren();

	void SetAudio(bool set);
	void SetAudioVolume(float volume);

private:
	int m_mouseX;
	int m_mouseY;

	GUIWindowList m_vpGUIWindowList;

	ComponentList m_vpComponentList;
	Component* m_pDraggingComponentPriority;

	Renderer* m_pRenderer;

	unsigned int m_GUIViewport;

	unsigned int m_GUIFont;

	bool m_isDebugRender;

	float m_currentWindowDepth;

	bool m_audio;
	float m_audioVolume;

	GUIWindow* m_pFocusedWindow;
};

#endif