/*************************************************************************
> File Name: Container.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>    A container to hold multiple components. Includes
>    functionality for adding and removing components.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_CONTAINER_H
#define CUBBY_CONTAINER_H

#include "Component.h"

using ComponentList = std::vector<Component*>;

class Container : public Component
{
public:
	// Constructor, Destructor
	Container(Renderer* pRenderer);
	virtual ~Container();

	void AddEventListeners() override;
	void RemoveEventListeners() override;

	bool IsContainer() const override;
	bool IsRootContainer() const override;

	bool HasChildren() const;

	void SetEnabled(bool enable) override;

	void SetDebugRender(bool debug) override;

	virtual void Add(Component* component);
	virtual void Remove(Component* component);
	void RemoveAll();

	const ComponentList& GetChildren() const;
	Component* GetChild(int n) const;
	int GetNumberOfChildren() const;

	void DepthSortComponentChildren();

	ComponentType GetComponentType() const override;

	void SetAudio(bool set) override;
	void SetAudioVolume(float volume) override;

	void Update(float deltaTime) override;

protected:
	void DrawSelf() override;
	void DrawChildren() override;

	ComponentList m_vpComponentList;
};

#endif