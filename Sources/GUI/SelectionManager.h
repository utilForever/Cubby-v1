/*************************************************************************
> File Name: SelectionManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    Handles the picking of components, events are sent to the currently
>    selected component. This is worked out using component bounding boxes.
> Note
>    The actual algorithm for working out what component is currently
>    selected doesn't have to be mathematically, we could use the OpenGL
>    selection buffer, color picking or a number of other methods.
> Created Time: 2016/08/22
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_SELECTION_MANAGER_H
#define CUBBY_SELECTION_MANAGER_H

#include "Component.h"
#include "Container.h"

class SelectionManager
{
public:
	// Singleton Create, Destroy
	static SelectionManager* GetInstance();
	void Destroy() const;

	void ResetCurrentComponent() const;

	void AddComponent(Component* component);
	void RemoveComponent(Component* component);

	void Update(int x, int y);
	Component* GetComponentAt(int x, int y);

	Component* GetCurrentComponent() const;

	void MouseMotion(int x, int y);
	void MousePressed(int button);
	void MouseReleased(int button);

private:
	SelectionManager();
	SelectionManager(const SelectionManager&) = delete;
	SelectionManager(SelectionManager&&) = delete;
	SelectionManager& operator=(const SelectionManager&) = delete;
	SelectionManager& operator=(SelectionManager&&) = delete;

	ComponentList m_vpComponentList;

	Component* m_foundComponent;
	Component* m_hoverOverComponent;
	Component* m_firstClickedComponent;

	bool m_isPressed;
	int m_buttonPressed;
	
	int m_mouseX;
	int m_mouseY;
	int m_lastX;
	int m_lastY;

	// Singleton instance
	static SelectionManager* m_instance;
};

#endif