#pragma once
#include "Global/Types.h"

namespace GL
{
	class TimerGPU
	{
	public:
		TimerGPU();

		TimerGPU(const TimerGPU&) = delete;
		TimerGPU& operator=(const TimerGPU&) = delete;

		//TimerGPU(TimerGPU&& other);
		//TimerGPU& operator=(TimerGPU&& other);

		~TimerGPU();
	public:
		void StartTimer();
		void StopTimer();

		f32 GetDeltaTime_milliseconds();
	private:
		u32 m_id[2];
		u64 m_startTime;
		u64 m_stopTime;
	};
}
