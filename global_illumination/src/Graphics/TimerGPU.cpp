#include "TimerGPU.h"
#include "OpenGL.h"

namespace GL
{
	TimerGPU::TimerGPU()
		:
		m_startTime(0),
		m_stopTime(0)
	{
		glCall(glGenQueries(2, m_id));
	}

	TimerGPU::~TimerGPU()
	{
		glCall(glDeleteQueries(2, m_id));
	}

	void TimerGPU::StartTimer()
	{
		glCall(glQueryCounter(m_id[0], GL_TIMESTAMP));
	}

	void TimerGPU::StopTimer()
	{
		glQueryCounter(m_id[1], GL_TIMESTAMP);

		// wait until the results are available
		i32 stopTimerAvailable = 0;
		while (!stopTimerAvailable) {
			glGetQueryObjectiv(m_id[1],GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
		}

		// get query results
		glGetQueryObjectui64v(m_id[0], GL_QUERY_RESULT, &m_startTime);
		glGetQueryObjectui64v(m_id[1], GL_QUERY_RESULT, &m_stopTime);
	}

	f32 TimerGPU::GetDeltaTime_milliseconds()
	{
		return (m_stopTime - m_startTime) / 1000000.0;
	}




}
