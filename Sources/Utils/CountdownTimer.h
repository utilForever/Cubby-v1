/*************************************************************************
> File Name: CountdownTimer.h
> Project Name: Cubby
> Author: Chan-Ho Chris Ohk
> Purpose
>	 A Countdown timer that triggers a function callback upon reaching the
>	 countdown time specified. Can also be a looping timer so that the callback
>	 happens every 'n' amount of time.
> Created Time: 2016/08/07
> Copyright (c) 2016, Chan-Ho Chris Ohk
*************************************************************************/

#ifndef CUBBY_COUNTDOWN_TIMER_H
#define CUBBY_COUNTDOWN_TIMER_H

#include <functional>

class CountdownTimer
{
public:
	// Constructor, Destructor
	CountdownTimer();
	~CountdownTimer();

	void SetCallBackFunction(std::function<void(void*)> func);
	void SetCallBackData(void* pData);

	void StartCountdown();
	void ResetCountdown();

	void PauseCountdown();
	void ResumeCountdown();
	bool IsPaused() const;

	float GetElapsedTime() const;
	float GetRemainingTime() const;

	void SetCountdownTime(float timeOut);

	void SetLooping(bool loop);

	void UpdateCountdown(float dt);

private:
	float m_elapsedTime;

	float m_timeOutTime;

	bool m_started;
	bool m_looping;
	bool m_paused;
	bool m_finished;

	std::function<void(void*)> m_callback;
	void* m_pCallbackData;
};

#endif