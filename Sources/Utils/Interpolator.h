/*************************************************************************
> File Name: Interpolator.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: An interpolator helper class that will manage all the interpolations for your variables.
> Created Time: 2016/07/03
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_INTERPOLATOR_H
#define CUBBY_INTERPOLATOR_H

#include <vector>
#include <functional>

using FunctionCallback = std::function<void(void*)>;

struct FloatInterpolation
{
	float* variable;

	float start;
	float end;
	float time;

	float easing;

	float elapsed;

	bool erase;

	FloatInterpolation* pNextInterpolation;

	FunctionCallback callback;
	void* pCallbackData;
};

struct IntInterpolation
{
	int* variable;

	int start;
	int end;
	float time;

	float easing;

	float elapsed;

	bool erase;

	IntInterpolation* pNextInterpolation;

	FunctionCallback callback;
	void* pCallbackData;
};

using FloatInterpolationList = std::vector<FloatInterpolation*>;
using IntInterpolationList = std::vector<IntInterpolation*>;

class Interpolator
{
public:
	static Interpolator* GetInstance();
	void Destroy();

	void ClearInterpolators();

	FloatInterpolation* CreateFloatInterpolation(float* val, float start, float end, float time, float easing, FloatInterpolation* next = nullptr, FunctionCallback callback = nullptr, void* data = nullptr) const;
	void LinkFloatInterpolation(FloatInterpolation* first, FloatInterpolation* second) const;
	void AddFloatInterpolation(FloatInterpolation* interpolation);
	void AddFloatInterpolation(float* val, float start, float end, float time, float easing, FloatInterpolation* next = nullptr, FunctionCallback callback = nullptr, void* data = nullptr);
	void RemoveFloatInterpolationByVariable(float* val);

	IntInterpolation* CreateIntInterpolation(int* val, int start, int end, float time, float easing, IntInterpolation* next = nullptr, FunctionCallback callback = nullptr, void* data = nullptr) const;
	void LinkIntInterpolation(IntInterpolation* first, IntInterpolation* second) const;
	void AddIntInterpolation(IntInterpolation* interpolation);
	void AddIntInterpolation(int* val, int start, int end, float time, float easing, IntInterpolation* next = nullptr, FunctionCallback callback = nullptr, void* data = nullptr);
	void RemoveIntInterpolationByVariable(int* val);

	void SetPaused(bool pause);
	bool IsPaused() const;

	void Update(float dt);
	void UpdateFloatInterpolators(float delta);
	void UpdateIntInterpolators(float delta);

protected:
	Interpolator();

private:
	void RemoveCreateFloatInterpolation(FloatInterpolation* interpolation);
	void RemoveFloatInterpolation(FloatInterpolation* interpolation);

	void RemoveCreateIntInterpolation(IntInterpolation* interpolation);
	void RemoveIntInterpolation(IntInterpolation* interpolation);

	// A dynamic array of our float interpolation variables
	FloatInterpolationList m_vpFloatInterpolations;
	FloatInterpolationList m_vpCreateFloatInterpolations;

	// A dynamic array of our int interpolation variables
	IntInterpolationList m_vpIntInterpolations;
	IntInterpolationList m_vpCreateIntInterpolations;

	// Singleton instance
	static Interpolator *m_instance;

	// Flag to control if we are paused or not
	bool m_paused;
};

#endif