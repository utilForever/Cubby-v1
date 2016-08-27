/*************************************************************************
> File Name: ProgressBar.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose: A progress bar component to show visual feedback of progress.
> Created Time: 2016/08/27
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_PROGRESS_BAR_H
#define CUBBY_PROGRESS_BAR_H

#include "Container.h"
#include "RenderRectangle.h"

enum class ProgressDirection
{
	Horizontal = 0,
	Vertical,
};

class ProgressBar : public Container
{
public:
	// Constructor, Destructor
	ProgressBar(Renderer* pRenderer, unsigned int GUIFont, float min, float max);
	virtual ~ProgressBar();

	float GetMinValue() const;
	float GetMaxValue() const;
	float GetCurrentValue() const;

	void SetMinValue(float minValue);
	void SetMaxValue(float maxValue);
	void SetCurrentValue(float currentValue);

	ProgressDirection GetProgressDirection() const;
	void SetProgressDirection(ProgressDirection direction);

	void SetProgressFiller(RenderRectangle* icon);
	void SetProgressBackground(RenderRectangle* icon);

	ComponentType GetComponentType() const override;

protected:
	virtual void OnMinValueReached();
	virtual void OnMaxValueReached();

	void DrawSelf() override;

private:
	float m_minValue;
	float m_maxValue;

	float m_currentValue;

	ProgressDirection m_progressDirection;

	RenderRectangle* m_pProgressBackground;
	RenderRectangle* m_pProgressFiller;
};

#endif