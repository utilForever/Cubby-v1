/*************************************************************************
> File Name: Interpolator.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An interpolator helper class that will manage all the interpolations for your variables.
> Created Time: 2016/07/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include <glm/detail/type_vec3.hpp>
#include <Maths/Bezier3.h>

#include "Interpolator.h"

#pragma comment (lib, "Winmm.lib")

bool NeedsErasingFloat(FloatInterpolation* f)
{
	bool needsErase = f->erase;

	if (needsErase == true)
	{
		delete f;
	}

	return needsErase;
}

bool NeedsErasingInt(IntInterpolation* i)
{
	bool needsErase = i->erase;

	if (needsErase == true)
	{
		delete i;
	}

	return needsErase;
}

// Initialize the singleton instance
Interpolator* Interpolator::m_instance = nullptr;

Interpolator* Interpolator::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new Interpolator;
	}

	return m_instance;
}

void Interpolator::Destroy()
{
	if (m_instance)
	{
		ClearInterpolators();
		delete m_instance;
	}
}

void Interpolator::ClearInterpolators()
{
	// Float
	for (unsigned int i = 0; i < m_vpFloatInterpolations.size(); ++i)
	{
		FloatInterpolation* pNext = m_vpFloatInterpolations[i]->pNextInterpolation;
		while (pNext != nullptr)
		{
			FloatInterpolation* pThisOne = pNext;
			pNext = pNext->pNextInterpolation;

			if (pThisOne->variable != nullptr)
			{
				delete pThisOne;
				pThisOne->variable = nullptr;
				pThisOne->pNextInterpolation = nullptr;
			}
		}

		if (m_vpFloatInterpolations[i]->variable != nullptr)
		{
			delete m_vpFloatInterpolations[i];
			m_vpFloatInterpolations[i]->variable = nullptr;
			m_vpFloatInterpolations[i]->pNextInterpolation = nullptr;
			m_vpFloatInterpolations[i] = nullptr;
		}
	}
	m_vpFloatInterpolations.clear();

	for (unsigned int i = 0; i < m_vpCreateFloatInterpolations.size(); ++i)
	{
		FloatInterpolation* pNext = m_vpCreateFloatInterpolations[i]->pNextInterpolation;
		while (pNext != nullptr)
		{
			FloatInterpolation* pThisOne = pNext;
			pNext = pNext->pNextInterpolation;

			if (pThisOne->variable != nullptr)
			{
				delete pThisOne;
				pThisOne->variable = nullptr;
				pThisOne->pNextInterpolation = nullptr;
			}
		}

		if (m_vpCreateFloatInterpolations[i]->variable != nullptr)
		{
			delete m_vpCreateFloatInterpolations[i];
			m_vpCreateFloatInterpolations[i]->variable = nullptr;
			m_vpCreateFloatInterpolations[i]->pNextInterpolation = nullptr;
			m_vpCreateFloatInterpolations[i] = nullptr;
		}
	}
	m_vpCreateFloatInterpolations.clear();

	// Int
	for (unsigned int i = 0; i < m_vpIntInterpolations.size(); ++i)
	{
		IntInterpolation* pNext = m_vpIntInterpolations[i]->pNextInterpolation;
		while (pNext != nullptr)
		{
			IntInterpolation* pThisOne = pNext;
			pNext = pNext->pNextInterpolation;

			if (pThisOne->variable != nullptr)
			{
				delete pThisOne;
				pThisOne->variable = nullptr;
				pThisOne->pNextInterpolation = nullptr;
			}
		}

		if (m_vpIntInterpolations[i]->variable != nullptr)
		{
			delete m_vpIntInterpolations[i];
			m_vpIntInterpolations[i]->variable = nullptr;
			m_vpIntInterpolations[i]->pNextInterpolation = nullptr;
			m_vpIntInterpolations[i] = nullptr;
		}
	}
	m_vpIntInterpolations.clear();

	for (unsigned int i = 0; i < m_vpCreateIntInterpolations.size(); ++i)
	{
		IntInterpolation* pNext = m_vpCreateIntInterpolations[i]->pNextInterpolation;
		while (pNext != nullptr)
		{
			IntInterpolation* pThisOne = pNext;
			pNext = pNext->pNextInterpolation;

			if (pThisOne->variable != nullptr)
			{
				delete pThisOne;
				pThisOne->variable = nullptr;
				pThisOne->pNextInterpolation = nullptr;
			}
		}

		if (m_vpCreateIntInterpolations[i]->variable != nullptr)
		{
			delete m_vpCreateIntInterpolations[i];
			m_vpCreateIntInterpolations[i]->variable = nullptr;
			m_vpCreateIntInterpolations[i]->pNextInterpolation = nullptr;
			m_vpCreateIntInterpolations[i] = nullptr;
		}
	}
	m_vpCreateIntInterpolations.clear();
}

FloatInterpolation* Interpolator::CreateFloatInterpolation(float* val, float start, float end, float time, float easing, FloatInterpolation* next, FunctionCallback callback, void* data) const
{
	FloatInterpolation* floatInterp = new FloatInterpolation();
	floatInterp->variable = val;
	floatInterp->start = start;
	floatInterp->end = end;
	floatInterp->time = time;

	floatInterp->easing = easing;

	floatInterp->pNextInterpolation = next;

	floatInterp->elapsed = 0.0f;
	floatInterp->erase = false;

	floatInterp->callback = callback;
	floatInterp->pCallbackData = data;

	return floatInterp;
}

void Interpolator::LinkFloatInterpolation(FloatInterpolation* first, FloatInterpolation* second) const
{
	first->pNextInterpolation = second;
}

void Interpolator::AddFloatInterpolation(FloatInterpolation* interpolation)
{
	// Reset the elapsed time
	interpolation->elapsed = 0.0f;

	// Rest the erase flag
	interpolation->erase = false;

	m_vpCreateFloatInterpolations.push_back(interpolation);
}

void Interpolator::AddFloatInterpolation(float* val, float start, float end, float time, float easing, FloatInterpolation* next, FunctionCallback callback, void* data)
{
	FloatInterpolation* floatInterp = CreateFloatInterpolation(val, start, end, time, easing, next, callback, data);
	AddFloatInterpolation(floatInterp);
}

void Interpolator::RemoveFloatInterpolationByVariable(float* val)
{
	// Creation list
	FloatInterpolationList vpRemoveCreateFloatInterpolations;
	for (unsigned int i = 0; i < m_vpCreateFloatInterpolations.size(); ++i)
	{
		FloatInterpolation* pInterpolator = m_vpCreateFloatInterpolations[i];
		if (pInterpolator->variable == val)
		{
			vpRemoveCreateFloatInterpolations.push_back(pInterpolator);
		}
	}
	for (unsigned int i = 0; i < vpRemoveCreateFloatInterpolations.size(); ++i)
	{
		RemoveCreateFloatInterpolation(vpRemoveCreateFloatInterpolations[i]);
	}

	// Normal list
	FloatInterpolationList vpRemoveFloatInterpolations;
	for (unsigned int i = 0; i < m_vpFloatInterpolations.size(); ++i)
	{
		FloatInterpolation* pInterpolator = m_vpFloatInterpolations[i];
		if (pInterpolator->variable == val)
		{
			vpRemoveFloatInterpolations.push_back(pInterpolator);
		}
	}
	for (unsigned int i = 0; i < vpRemoveFloatInterpolations.size(); ++i)
	{
		RemoveFloatInterpolation(vpRemoveFloatInterpolations[i]);
	}
}

IntInterpolation* Interpolator::CreateIntInterpolation(int* val, int start, int end, float time, float easing, IntInterpolation* next, FunctionCallback callback, void* data) const
{
	IntInterpolation* intInterp = new IntInterpolation();
	intInterp->variable = val;
	intInterp->start = start;
	intInterp->end = end;
	intInterp->time = time;

	intInterp->easing = easing;

	intInterp->pNextInterpolation = next;

	intInterp->elapsed = 0.0f;
	intInterp->erase = false;

	intInterp->callback = callback;
	intInterp->pCallbackData = data;

	return intInterp;
}

void Interpolator::LinkIntInterpolation(IntInterpolation* first, IntInterpolation* second) const
{
	first->pNextInterpolation = second;
}

void Interpolator::AddIntInterpolation(IntInterpolation* interpolation)
{
	// Reset the elapsed time
	interpolation->elapsed = 0.0f;

	// Rest the erase flag
	interpolation->erase = false;

	m_vpCreateIntInterpolations.push_back(interpolation);
}

void Interpolator::AddIntInterpolation(int* val, int start, int end, float time, float easing, IntInterpolation* next, FunctionCallback callback, void* data)
{
	IntInterpolation* intInterp = CreateIntInterpolation(val, start, end, time, easing, next, callback, data);

	AddIntInterpolation(intInterp);
}

void Interpolator::RemoveIntInterpolationByVariable(int* val)
{
	// Creation list
	IntInterpolationList vpRemoveCreateIntInterpolations;
	for (unsigned int i = 0; i < m_vpCreateIntInterpolations.size(); ++i)
	{
		IntInterpolation* pInterpolator = m_vpCreateIntInterpolations[i];
		if (pInterpolator->variable == val)
		{
			vpRemoveCreateIntInterpolations.push_back(pInterpolator);
		}
	}
	for (unsigned int i = 0; i < vpRemoveCreateIntInterpolations.size(); ++i)
	{
		RemoveCreateIntInterpolation(vpRemoveCreateIntInterpolations[i]);
	}

	// Normal list
	IntInterpolationList vpRemoveIntInterpolations;
	for (unsigned int i = 0; i < m_vpIntInterpolations.size(); ++i)
	{
		IntInterpolation* pInterpolator = m_vpIntInterpolations[i];
		if (pInterpolator->variable == val)
		{
			vpRemoveIntInterpolations.push_back(pInterpolator);
		}
	}
	for (unsigned int i = 0; i < vpRemoveIntInterpolations.size(); ++i)
	{
		RemoveIntInterpolation(vpRemoveIntInterpolations[i]);
	}
}

void Interpolator::SetPaused(bool pause)
{
	m_paused = pause;
}

bool Interpolator::IsPaused() const
{
	return m_paused;
}

void Interpolator::Update(float dt)
{
	UpdateFloatInterpolators(dt);
	UpdateIntInterpolators(dt);
}

void Interpolator::UpdateFloatInterpolators(float delta)
{
	// Add any interpolators in the create list
	for (unsigned int i = 0; i < m_vpCreateFloatInterpolations.size(); ++i)
	{
		FloatInterpolation* pInterpolator = m_vpCreateFloatInterpolations[i];

		m_vpFloatInterpolations.push_back(pInterpolator);
	}
	m_vpCreateFloatInterpolations.clear();

	// Remove any interpolations that need to be erased
	m_vpFloatInterpolations.erase(remove_if(m_vpFloatInterpolations.begin(), m_vpFloatInterpolations.end(), NeedsErasingFloat), m_vpFloatInterpolations.end());

	if (m_paused == false)
	{
		FloatInterpolationList m_vpNextFloatInterpolationsAddList;

		// Update the float interpolations
		for (unsigned int i = 0; i < m_vpFloatInterpolations.size(); ++i)
		{
			FloatInterpolation* pInterpolator = m_vpFloatInterpolations[i];
			if (pInterpolator != nullptr)
			{
				if (pInterpolator->elapsed < pInterpolator->time)
				{
					float* pVar = pInterpolator->variable;
					if (pVar == nullptr)
					{
						continue;
					}

					float timeRatio = pInterpolator->elapsed / pInterpolator->time;
					float diff = static_cast<float>(pInterpolator->end - pInterpolator->start);

					// Create a bezier curve to represent the acceleration/deceleration curve of the animation
					// NOTE: 0 = linear, 100 = full acceleration, -100 = full deceleration.
					float x = (pInterpolator->easing * 0.005f) + 0.5f;
					float y = 1.0f - x;

					glm::vec3 start = glm::vec3(0.0f, 0.0f, 0.0f);
					glm::vec3 end = glm::vec3(1.0f, 1.0f, 0.0f);
					glm::vec3 control = glm::vec3(x, y, 0.0f);

					Bezier3 easeBezier = Bezier3(start, end, control);
					glm::vec3 vectorT = easeBezier.GetInterpolatedPoint(timeRatio);

					// Get our real T value that we are going to use to interpolate
					float realT = vectorT.y;

					// Set the variable value
					*pVar = pInterpolator->start + (diff * realT);

					pInterpolator->elapsed += delta;
				}
				else
				{
					float* pVar = pInterpolator->variable;
					if (pVar == nullptr)
					{
						continue;
					}

					*pVar = pInterpolator->end;

					// If we have a callback, do it
					if (pInterpolator->callback != nullptr)
					{
						pInterpolator->callback(pInterpolator->pCallbackData);
					}

					// Are we chained to start another float interpolator?
					if (pInterpolator->pNextInterpolation != nullptr)
					{
						m_vpNextFloatInterpolationsAddList.push_back(pInterpolator->pNextInterpolation);
					}

					// Erase this interpolator since we have finished
					pInterpolator->erase = true;
				}
			}

		}

		// Add any chained interpolators to the list
		for (unsigned int j = 0; j < m_vpNextFloatInterpolationsAddList.size(); ++j)
		{
			FloatInterpolation* pInterpolator = m_vpNextFloatInterpolationsAddList[j];
			AddFloatInterpolation(pInterpolator);
		}
	}
}

void Interpolator::UpdateIntInterpolators(float delta)
{
	// Add any interpolators in the create list
	for (unsigned int i = 0; i < m_vpCreateIntInterpolations.size(); ++i)
	{
		IntInterpolation* pInterpolator = m_vpCreateIntInterpolations[i];

		m_vpIntInterpolations.push_back(pInterpolator);
	}
	m_vpCreateIntInterpolations.clear();

	// Remove any interpolations that need to be erased
	m_vpIntInterpolations.erase(remove_if(m_vpIntInterpolations.begin(), m_vpIntInterpolations.end(), NeedsErasingInt), m_vpIntInterpolations.end());

	if (m_paused == false)
	{
		IntInterpolationList m_vpNextIntInterpolationsAddList;

		// Update the int interpolations
		for (unsigned int i = 0; i < m_vpIntInterpolations.size(); ++i)
		{
			IntInterpolation* pInterpolator = m_vpIntInterpolations[i];
			if (pInterpolator != nullptr)
			{
				if (pInterpolator->elapsed < pInterpolator->time)
				{
					int* pVar = pInterpolator->variable;
					if (pVar == nullptr)
					{
						continue;
					}

					float timeRatio = pInterpolator->elapsed / pInterpolator->time;
					float diff = static_cast<float>(pInterpolator->end - pInterpolator->start);

					// Create a bezier curve to represent the acceleration/deceleration curve of the animation
					// NOTE: 0 = linear, 100 = full acceleration, -100 = full deceleration.
					float x = (pInterpolator->easing * 0.005f) + 0.5f;
					float y = 1.0f - x;

					glm::vec3 start = glm::vec3(0.0f, 0.0f, 0.0f);
					glm::vec3 end = glm::vec3(1.0f, 1.0f, 0.0f);
					glm::vec3 control = glm::vec3(x, y, 0.0f);

					Bezier3 easeBezier = Bezier3(start, end, control);
					glm::vec3 vectorT = easeBezier.GetInterpolatedPoint(timeRatio);

					// Get our real T value that we are going to use to interpolate
					float realT = vectorT.y;

					// Set the variable value
					*pVar = static_cast<int>(pInterpolator->start + (diff * realT));

					pInterpolator->elapsed += delta;
				}
				else
				{
					int* pVar = pInterpolator->variable;
					if (pVar == nullptr)
					{
						continue;
					}

					*pVar = pInterpolator->end;

					// If we have a callback, do it
					if (pInterpolator->callback != nullptr)
					{
						pInterpolator->callback(pInterpolator->pCallbackData);
					}

					// Are we chained to start another int interpolator?
					if (pInterpolator->pNextInterpolation != nullptr)
					{
						m_vpNextIntInterpolationsAddList.push_back(pInterpolator->pNextInterpolation);
					}

					// Erase this interpolator since we have finished
					pInterpolator->erase = true;
				}
			}

		}

		// Add any chained interpolators to the list
		for (unsigned int j = 0; j < m_vpNextIntInterpolationsAddList.size(); ++j)
		{
			IntInterpolation* pInterpolator = m_vpNextIntInterpolationsAddList[j];
			AddIntInterpolation(pInterpolator);
		}
	}
}

Interpolator::Interpolator()
{
	m_paused = false;
}

void Interpolator::RemoveCreateFloatInterpolation(FloatInterpolation* interpolation)
{
	m_vpCreateFloatInterpolations.erase(remove(m_vpCreateFloatInterpolations.begin(), m_vpCreateFloatInterpolations.end(), interpolation), m_vpCreateFloatInterpolations.end());
}

void Interpolator::RemoveFloatInterpolation(FloatInterpolation* aInterpolation)
{
	m_vpFloatInterpolations.erase(remove(m_vpFloatInterpolations.begin(), m_vpFloatInterpolations.end(), aInterpolation), m_vpFloatInterpolations.end());
}

void Interpolator::RemoveCreateIntInterpolation(IntInterpolation* interpolation)
{
	m_vpCreateIntInterpolations.erase(remove(m_vpCreateIntInterpolations.begin(), m_vpCreateIntInterpolations.end(), interpolation), m_vpCreateIntInterpolations.end());
}

void Interpolator::RemoveIntInterpolation(IntInterpolation* aInterpolation)
{
	m_vpIntInterpolations.erase(remove(m_vpIntInterpolations.begin(), m_vpIntInterpolations.end(), aInterpolation), m_vpIntInterpolations.end());
}