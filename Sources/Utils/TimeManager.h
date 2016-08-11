/*************************************************************************
> File Name: TimeManager.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
> 	 A Manager class to handle all time related functionality. Can be used
> 	 to get elapsed time, current tick count and also for a change in time
> 	 on a frame by frame basis, to allow for time based animations.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_TIME_MANAGER_H
#define CUBBY_TIME_MANAGER_H

#include <vector>

#include "CountdownTimer.h"

using CountdownTimerList = std::vector<CountdownTimer*>;

class TimeManager
{
public:
	static TimeManager* GetInstance();
	void Destroy();

	bool HasCountdownTimers() const;
	void AddCountdownTimer(CountdownTimer* countdownTimer);
	void RemoveCountdownTimer(CountdownTimer* countdownTimer);
	void RemoveCountdownTimers();

	// Update
	void Update(float dt);

private:
	TimeManager() = default;
	TimeManager(const TimeManager&) = delete;
	TimeManager(TimeManager&&) = delete;
	TimeManager& operator=(const TimeManager&) = delete;
	TimeManager& operator=(TimeManager&&) = delete;

	// A dynamic array of our countdown timers
	CountdownTimerList m_vpCountdownTimers;

	// Singleton instance
	static TimeManager* m_instance;
};

#endif