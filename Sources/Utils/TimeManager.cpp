/*************************************************************************
> File Name: TimeManager.cpp
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
> 	 A Manager class to handle all time related functionality. Can be used
> 	 to get elapsed time, current tick count and also for a change in time
> 	 on a frame by frame basis, to allow for time based animations.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#include <algorithm>

#include "TimeManager.h"

// Initialize the singleton instance
TimeManager* TimeManager::m_instance = nullptr;

TimeManager* TimeManager::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new TimeManager;
	}

	return m_instance;
}

void TimeManager::Destroy()
{
	if (m_instance)
	{
		for (size_t i = 0; i < m_vpCountdownTimers.size(); ++i)
		{
			delete m_vpCountdownTimers[i];
			m_vpCountdownTimers[i] = nullptr;
		}

		m_vpCountdownTimers.clear();

		delete m_instance;
	}
}

bool TimeManager::HasCountdownTimers() const
{
	return m_vpCountdownTimers.size() > 0;
}

void TimeManager::AddCountdownTimer(CountdownTimer* countdownTimer)
{
	m_vpCountdownTimers.push_back(countdownTimer);
}

void TimeManager::RemoveCountdownTimer(CountdownTimer* countdownTimer)
{
	m_vpCountdownTimers.erase(remove(m_vpCountdownTimers.begin(), m_vpCountdownTimers.end(), countdownTimer), m_vpCountdownTimers.end());
}

void TimeManager::RemoveCountdownTimers()
{
	m_vpCountdownTimers.clear();
}

// Update
void TimeManager::Update(float dt)
{
	// Also update all our countdown timers
	for (auto iter = m_vpCountdownTimers.begin(); iter != m_vpCountdownTimers.end(); ++iter)
	{
		(*iter)->UpdateCountdown(dt);
	}
}