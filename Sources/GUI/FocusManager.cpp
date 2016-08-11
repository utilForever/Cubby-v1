/*************************************************************************
> File Name: FocusManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
> 	 Manages the focus of components and sends events to the components when
> 	 focus has been lost or gained.
> Created Time: 2016/08/11
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "FocusManager.h"

// Initialize the singleton instance
FocusManager* FocusManager::m_instance = nullptr;

FocusManager* FocusManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new FocusManager;
	}

	return m_instance;
}

void FocusManager::Destroy()
{
	if (m_instance)
	{
		delete m_instance;
	}
}

void FocusManager::SetFocusOwner(Component* c)
{
	if (c == m_focusedComponent)
	{
		return;
	}

	Component* oldFocus = m_focusedComponent;

	m_focusedComponent = c;

	// The new component has gained focus
	if (m_focusedComponent)
	{
		FocusEvent focusGainEvent(m_focusedComponent, static_cast<int>(FocusEventType::GAINED));
		m_focusedComponent->ProcessFocusEvent(focusGainEvent);
	}

	// The old component has lost focus
	if (oldFocus)
	{
		FocusEvent focusLostEvent(oldFocus, static_cast<int>(FocusEventType::LOST));
		oldFocus->ProcessFocusEvent(focusLostEvent);
	}
}

Component* FocusManager::GetFocusOwner() const
{
	return m_focusedComponent;
}

void FocusManager::KeyPressed(int keyCode, int modifier) const
{
	if (m_focusedComponent != nullptr)
	{
		KeyEvent keyPressedEvent(m_focusedComponent, static_cast<int>(KeyEventType::PRESSED), keyCode, modifier);
		m_focusedComponent->ProcessKeyEvent(keyPressedEvent);
	}
}

void FocusManager::KeyReleased(int keyCode, int modifier) const
{
	if (m_focusedComponent != nullptr)
	{
		KeyEvent keyReleasedEvent(m_focusedComponent, static_cast<int>(KeyEventType::RELEASED), keyCode, modifier);
		m_focusedComponent->ProcessKeyEvent(keyReleasedEvent);

		KeyEvent keyTypedEvent(m_focusedComponent, static_cast<int>(KeyEventType::TYPED), keyCode, modifier);
		m_focusedComponent->ProcessKeyEvent(keyTypedEvent);
	}
}

void FocusManager::CharacterEntered(int characterCode) const
{
	if (m_focusedComponent != nullptr)
	{
		KeyEvent CharacterEnteredEvent(m_focusedComponent, static_cast<int>(KeyEventType::CHARACTER_ENTERED), characterCode, 0);
		m_focusedComponent->ProcessKeyEvent(CharacterEnteredEvent);
	}
}

FocusManager::FocusManager() :
	m_focusedComponent(nullptr)
{
}
