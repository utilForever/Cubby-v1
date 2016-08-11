/*************************************************************************
> File Name: FocusManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
> 	 Manages the focus of components and sends events to the components when
> 	 focus has been lost or gained.
> Created Time: 2016/08/11
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_FOCUS_MANAGER_H
#define CUBBY_FOCUS_MANAGER_H

#include "Component.h"

class FocusManager
{
public:
	static FocusManager* GetInstance();
	static void Destroy();

	void SetFocusOwner(Component* c);
	Component* GetFocusOwner() const;

	void KeyPressed(int keyCode, int modifier) const;
	void KeyReleased(int keyCode, int modifier) const;
	void CharacterEntered(int characterCode) const;

private:
	FocusManager();
	FocusManager(const FocusManager&) = delete;
	FocusManager(FocusManager&&) = delete;
	FocusManager& operator=(const FocusManager&) = delete;
	FocusManager& operator=(FocusManager&&) = delete;

	Component* m_focusedComponent;

	// Singleton instance
	static FocusManager* m_instance;
};

#endif