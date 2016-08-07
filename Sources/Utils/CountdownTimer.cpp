/*************************************************************************
> File Name: CountdownTimer.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>	 A Countdown timer that triggers a function callback upon reaching the
>	 countdown time specified. Can also be a looping timer so that the callback
>	 happens every 'n' amount of time.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include "TimeManager.h"
#include "CountdownTimer.h"

// Constructor, Destructor
CountdownTimer::CountdownTimer()
{
	TimeManager::GetInstance()->AddCountdownTimer(this);

	m_elapsedTime = 0.0f;
	m_timeOutTime = 0.0f;

	m_callback = nullptr;
	m_pCallbackData = nullptr;

	m_started = false;

	m_paused = true;

	m_finished = false;
}

CountdownTimer::~CountdownTimer()
{
	//TimeManager::GetInstance()->RemoveCountdownTimer(this);
}

void CountdownTimer::SetCallBackFunction(std::function<void(void*)> func)
{
	m_callback = func;
}

void CountdownTimer::SetCallBackData(void* pData)
{
	m_pCallbackData = pData;
}

void CountdownTimer::StartCountdown()
{
	m_elapsedTime = 0.0f;

	m_paused = false;

	m_finished = false;

	m_started = true;
}

void CountdownTimer::ResetCountdown()
{
	m_elapsedTime = 0.0f;

	m_finished = false;
}

void CountdownTimer::PauseCountdown()
{
	m_paused = true;
}

void CountdownTimer::ResumeCountdown()
{
	m_paused = false;
}

bool CountdownTimer::IsPaused() const
{
	return m_paused;
}

float CountdownTimer::GetElapsedTime() const
{
	return m_elapsedTime;
}

float CountdownTimer::GetRemainingTime() const
{
	return m_timeOutTime - m_elapsedTime;
}

void CountdownTimer::SetCountdownTime(float timeOut)
{
	m_timeOutTime = timeOut;
}

void CountdownTimer::SetLooping(bool loop)
{
	m_looping = loop;
}

void CountdownTimer::UpdateCountdown(float dt)
{
	if (!m_started)
	{
		return;
	}

	if (!m_paused)
	{
		m_elapsedTime += dt;
	}

	if (m_elapsedTime >= m_timeOutTime)
	{
		if (!m_finished)
		{
			// We have reached our countdown time, call our function callback
			if (m_callback)
			{
				m_callback(m_pCallbackData);
			}

			if (m_looping)
			{
				// If we are a looping timer, then just reset the elapsed time
				m_elapsedTime = 0.0f;
			}
			else
			{
				// We are not looping, so set our finished flag
				m_finished = true;
			}
		}
	}
}
