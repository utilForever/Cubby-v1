/*************************************************************************
> File Name: DynamicLight.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: Dynamic light class.
> Created Time: 2016/07/08
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_DYNAMIC_LIGHT_H
#define CUBBY_DYNAMIC_LIGHT_H

#include <glm/vec3.hpp>

#include "Renderer/Color.h"

class DynamicLight
{
public:
	// Constructor, Destructor
	DynamicLight();
	~DynamicLight();

	// Getter, Setter
	glm::vec3 GetPosition() const;
	float GetRadius() const;
	float GetDiffuseScale() const;
	Color GetColor() const;
	unsigned int GetLightID() const;
	float GetLifeTime() const;
	float GetMaxLifeTime() const;
	bool IsDyingLight() const;

	void SetPosition(glm::vec3 position);
	void SetRadius(float radius);
	void SetDiffuseScale(float diffuseScale);
	void SetColor(Color color);
	void SetLightID(unsigned int lightID);
	void SetLifeTime(float lifeTime);
	void SetMaxLifeTime(float maxLifeTime);
	void SetDyingLight(bool isDyingLight);

	// Erase operations
	void SetErase(bool isNeedErase);
	bool IsNeedErasing() const;

	// Update
	void Update(float dt);

private:
	unsigned int m_lightID;

	glm::vec3 m_position;

	float m_radius;

	float m_diffuseScale;

	Color m_color;

	float m_lifeTime;
	float m_maxLifeTime;
	bool m_isDyingLight;

	bool m_isNeedErase;
};

#endif