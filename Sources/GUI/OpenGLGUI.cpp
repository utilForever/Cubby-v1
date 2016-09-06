/*************************************************************************
> File Name: OpenGLGUI.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Class to manage everything related to the GUI. Has methods
>    that must be called for the GUI to function properly such as
>    Render() and Update(). Also requires access to input events.
> Created Time: 2016/08/28
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include "FocusManager.h"
#include "SelectionManager.h"
#include "OpenGLGUI.h"

OpenGLGUI::OpenGLGUI(Renderer* pRenderer) :
	m_pRenderer(pRenderer)
{
	m_mouseX = 0;
	m_mouseY = 0;

	m_currentWindowDepth = 1.0f;

	m_GUIViewport = 0;

	m_isDebugRender = false;

	m_pFocusedWindow = nullptr;

	m_pDraggingComponentPriority = nullptr;

	m_audio = true;
	m_audioVolume = 1.0f;

	// Create the GUI font
	m_pRenderer->CreateFreeTypeFont("Resources/fonts/arial.ttf", 17, &m_GUIFont);
}

OpenGLGUI::~OpenGLGUI()
{
	SelectionManager::GetInstance()->Destroy();
	FocusManager::GetInstance()->Destroy();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool OpenGLGUI::IsMouseInteractingWithGUIComponent(bool ignoreGUIWindows) const
{
	Component* currentComponent = SelectionManager::GetInstance()->GetCurrentComponent();
	if (currentComponent != nullptr && currentComponent->IsVisible() && (ignoreGUIWindows == true ? currentComponent->GetComponentType() != ComponentType::GUIWindow : true))
	{
		return true;
	}

	return false;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool OpenGLGUI::IsKeyboardInteractingWithGUIComponent() const
{
	Component* currentComponent = FocusManager::GetInstance()->GetFocusOwner();
	if (currentComponent != nullptr)
	{
		return true;
	}

	return false;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
Component* OpenGLGUI::GetCurrentMouseInteractionComponent() const
{
	Component* currentComponent = SelectionManager::GetInstance()->GetCurrentComponent();
	if (currentComponent != nullptr && currentComponent->IsVisible())
	{
		return currentComponent;
	}

	return nullptr;
}

void OpenGLGUI::AddWindow(GUIWindow* window)
{
	window->SetDepth(m_currentWindowDepth);

	window->SetAudio(m_audio);
	window->SetAudioVolume(m_audioVolume);

	// Windows are separated by a depth of 5
	m_currentWindowDepth += 10.0f;

	window->SetDepth(m_currentWindowDepth);

	m_vpGUIWindowList.push_back(window);

	window->SetGUIParent(this);

	// Make sure the last added window is the focus window
	SetFocusedWindow(window);

	window->AddEventListeners();

	DepthSortGUIWindowChildren();
}

void OpenGLGUI::RemoveWindow(GUIWindow* window)
{
	window->RemoveEventListeners();

	window->SetGUIParent(nullptr);

	m_vpGUIWindowList.erase(remove(m_vpGUIWindowList.begin(), m_vpGUIWindowList.end(), window), m_vpGUIWindowList.end());

	// Windows are separated by a depth of 5
	m_currentWindowDepth -= 10.0f;

	DepthSortGUIWindowChildren();
}

void OpenGLGUI::SetFocusedWindow(GUIWindow* window)
{
	if (window == m_pFocusedWindow)
	{
		// Don't set the same focus window multiple times
		return;
	}

	m_pFocusedWindow = window;

	float previousDepth = window->GetDepth();

	// Draw all the windows which we contain
	for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
	{
		if (*iter == window)
		{
			continue;
		}

		if ((*iter)->GetDepth() >= previousDepth)
		{
			// Reduce the depth of all un-focused windows that had a higher depth
			(*iter)->SetDepth((*iter)->GetDepth() - 10.0f);
		}
	}

	// Make the focused window have the greatest depth
	m_pFocusedWindow->SetDepth(m_currentWindowDepth);

	// Sort the GUIWindow vector list, by depth
	DepthSortGUIWindowChildren();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void OpenGLGUI::SetComponentFocus(Component* component) const
{
	FocusManager::GetInstance()->SetFocusOwner(component);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void OpenGLGUI::ResetFocus() const
{
	FocusManager::GetInstance()->SetFocusOwner(nullptr);
}

void OpenGLGUI::AddComponent(Component* component)
{
	component->SetAudio(m_audio);
	component->SetAudioVolume(m_audioVolume);

	m_vpComponentList.push_back(component);

	// Sort the component vector list, by depth
	DepthSortComponentChildren();

	component->AddEventListeners();
}

void OpenGLGUI::RemoveComponent(Component* component)
{
	component->RemoveEventListeners();

	m_vpComponentList.erase(remove(m_vpComponentList.begin(), m_vpComponentList.end(), component), m_vpComponentList.end());
}

Component* OpenGLGUI::GetComponent(Component* component)
{
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if (component == *iter)
		{
			return component;
		}
	}

	return nullptr;
}

void OpenGLGUI::SetDraggingComponentPriority(Component* pDraggingComponentPriority)
{
	m_pDraggingComponentPriority = pDraggingComponentPriority;
}

void OpenGLGUI::SetDebugRender(bool debug)
{
	m_isDebugRender = debug;

	// Set debug draw on all the windows which we contain
	for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
	{
		(*iter)->SetDebugRender(m_isDebugRender);
	}

	// Then Set debug draw on all the standalone components we contain
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		(*iter)->SetDebugRender(m_isDebugRender);
	}
}

bool OpenGLGUI::IsDebugRender() const
{
	return m_isDebugRender;
}

void OpenGLGUI::Render()
{
	// Sort the GUI window vector list, by depth
	DepthSortGUIWindowChildren();

	// Sort the component vector list, by depth
	DepthSortComponentChildren();

	// Draw all the standalone components we contain
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if (*iter == m_pDraggingComponentPriority)
		{
			continue;
		}

		(*iter)->Draw();
	}

	// Draw all the windows which we contain
	for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
	{
		(*iter)->Draw();
	}

	// Draw the dragging icon with priority last, due to transparency issues with windows that are drawn before.
	if (m_pDraggingComponentPriority != nullptr)
	{
		m_pDraggingComponentPriority->Draw();
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void OpenGLGUI::ResetSelectionManager() const
{
	SelectionManager::GetInstance()->ResetCurrentComponent();
}

void OpenGLGUI::Update(float deltaTime)
{
	// Update the selection manager
	SelectionManager::GetInstance()->Update(m_mouseX, m_mouseY);

	// Update all the windows which we contain
	for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
	{
		if ((*iter)->IsVisible())
		{
			(*iter)->Update(deltaTime);
		}
	}

	// Then update all the standalone components we contain
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if ((*iter)->IsVisible())
		{
			(*iter)->Update(deltaTime);
		}
	}
}

void OpenGLGUI::ImportMouseMotion(int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;

	// Forward to the selection manager
	SelectionManager::GetInstance()->MouseMotion(x, y);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void OpenGLGUI::MousePressed(int button) const
{
	// Forward to the selection manager
	SelectionManager::GetInstance()->MousePressed(button);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void OpenGLGUI::MouseReleased(int button) const
{
	// Forward to the selection manager
	SelectionManager::GetInstance()->MouseReleased(button);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void OpenGLGUI::KeyPressed(int keyCode, int modifier) const
{
	// Forward to the focus manager
	FocusManager::GetInstance()->KeyPressed(keyCode, modifier);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void OpenGLGUI::KeyReleased(int keyCode, int modifier) const
{
	// Forward to the focus manager
	FocusManager::GetInstance()->KeyReleased(keyCode, modifier);
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void OpenGLGUI::CharacterEntered(int characterCode) const
{
	// Forward to the focus manager
	FocusManager::GetInstance()->CharacterEntered(characterCode);
}

void OpenGLGUI::DepthSortGUIWindowChildren()
{
	sort(m_vpGUIWindowList.begin(), m_vpGUIWindowList.end(), GUIWindow::DepthLessThan);
}

void OpenGLGUI::DepthSortComponentChildren()
{
	sort(m_vpComponentList.begin(), m_vpComponentList.end(), Component::DepthLessThan);
}

void OpenGLGUI::SetAudio(bool set)
{
	m_audio = set;

	for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
	{
		(*iter)->SetAudio(set);
	}

	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		(*iter)->SetAudio(set);
	}
}

void OpenGLGUI::SetAudioVolume(float volume)
{
	m_audioVolume = volume;

	for (auto iter = m_vpGUIWindowList.begin(); iter != m_vpGUIWindowList.end(); ++iter)
	{
		(*iter)->SetAudioVolume(volume);
	}

	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		(*iter)->SetAudioVolume(volume);
	}
}