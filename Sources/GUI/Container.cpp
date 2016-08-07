/*************************************************************************
> File Name: Container.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A container to hold multiple components. Includes
>    functionality for adding and removing components.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include "Container.h"

// Constructor, Destructor
Container::Container(Renderer* pRenderer) :
	Component(pRenderer)
{
	// Do nothing
}

Container::~Container()
{
	m_vpComponentList.clear();
}

void Container::AddEventListeners()
{
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		(*iter)->AddEventListeners();
	}
}

void Container::RemoveEventListeners()
{
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		(*iter)->RemoveEventListeners();
	}
}

bool Container::IsContainer() const
{
	return true;
}

bool Container::IsRootContainer() const
{
	return false;
}

bool Container::HasChildren() const
{
	return m_vpComponentList.size() != 0;
}

void Container::SetEnabled(bool enable)
{
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		(*iter)->SetEnabled(enable);
	}

	Component::SetEnabled(enable);
}

void Container::SetDebugRender(bool debug)
{
	Component::SetDebugRender(debug);

	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		(*iter)->SetDebugRender(debug);
	}
}

void Container::Add(Component* component)
{
	// Make this Container the added Components parent.
	component->SetParent(this);

	// Make sure it appears on top of the container
	//component->SetDepth(m_depth + 1.0f);

	// Add the Component to the vector
	m_vpComponentList.push_back(component);

	// Sort the component vector list, by depth
	DepthSortComponentChildren();
}

void Container::Remove(Component* component)
{
	// Find this component in the vector
	auto iter = find(m_vpComponentList.begin(), m_vpComponentList.end(), component);

	if (iter != m_vpComponentList.end())
	{
		(*iter)->SetParent(nullptr);

		// Erase the component
		m_vpComponentList.erase(iter);
	}
}

void Container::RemoveAll()
{
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		(*iter)->SetParent(nullptr);
		(*iter)->RemoveEventListeners();
	}

	m_vpComponentList.clear();
}

const ComponentList& Container::GetChildren() const
{
	return m_vpComponentList;
}

int Container::GetNumberOfChildren() const
{
	return static_cast<int>(m_vpComponentList.size());
}

void Container::DepthSortComponentChildren()
{
	sort(m_vpComponentList.begin(), m_vpComponentList.end(), DepthLessThan);
}

Component* Container::GetChild(int n) const
{
	if (n > GetNumberOfChildren())
	{
		return nullptr;
	}

	int i = 0;

	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if (i == n)
		{
			return *iter;
		}

		i++;
	}

	return nullptr;
}

ComponentType Container::GetComponentType() const
{
	return ComponentType::Container;
}

void Container::SetAudio(bool set)
{
	Component::SetAudio(set);

	if (IsContainer() && HasChildren())
	{
		// Draw all children
		for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
		{
			(*iter)->SetAudio(set);
		}
	}
}

void Container::SetAudioVolume(float volume)
{
	Component::SetAudioVolume(volume);

	if (IsContainer() && HasChildren())
	{
		// Draw all children
		for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
		{
			(*iter)->SetAudioVolume(volume);
		}
	}
}

void Container::Update(float deltaTime)
{
	Component::Update(deltaTime);

	// Update all children
	for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
	{
		if ((*iter)->IsVisible())
		{
			(*iter)->Update(deltaTime);
		}
	}
}

void Container::DrawSelf()
{
	int containerWidth = m_dimensions.width;
	int containerHeight = m_dimensions.height;
	float depth = m_depth;

	float outlineX1 = 0.0f;
	float outlineX2 = static_cast<float>(containerWidth);
	float outlineY1 = 0.0f;
	float outlineY2 = static_cast<float>(containerHeight);

	m_pRenderer->PushMatrix();

	m_pRenderer->SetLineWidth(1.0f);
	m_pRenderer->ImmediateColorAlpha(1.0f, 1.0f, 0.0f, 1.0f);
	m_pRenderer->EnableImmediateMode(ImmediateModePrimitive::LINE_LOOP);
	m_pRenderer->ImmediateVertex(outlineX1, outlineY1, depth);
	m_pRenderer->ImmediateVertex(outlineX2, outlineY1, depth);
	m_pRenderer->ImmediateVertex(outlineX2 + 1, outlineY2, depth);
	m_pRenderer->ImmediateVertex(outlineX1, outlineY2, depth);
	m_pRenderer->DisableImmediateMode();

	m_pRenderer->PopMatrix();
}

void Container::DrawChildren()
{
	// This vector list is depth sorted, so we render in depth order, starting with the lowest depth first
	if (IsContainer() && HasChildren() && IsVisible())
	{
		// Draw all children
		for (auto iter = m_vpComponentList.begin(); iter != m_vpComponentList.end(); ++iter)
		{
			if ((*iter)->IsVisible())
			{
				(*iter)->Draw();
			}
		}
	}
}