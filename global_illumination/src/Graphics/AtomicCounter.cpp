#include "OpenGL.h"
#include "AtomicCounter.h"


namespace GL
{
	AtomicCounter::AtomicCounter()
	{
		glCall(glGenBuffers(1, &m_id));
		glCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_id));

		// define its initial storage capacity to be 4 bytes
		glCall(glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(u32) * 1, NULL, GL_DYNAMIC_DRAW));


		glCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
	}

	AtomicCounter::~AtomicCounter()
	{
		glCall(glDeleteBuffers(1, &m_id));
	}

	void AtomicCounter::Bind() const
	{
		glCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_id));
	}

	void AtomicCounter::UnBind() const
	{
		glCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
	}

	u32 AtomicCounter::Get()
	{
		glCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_id));

		u32 userCounter;
		glCall(glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(u32) * 1, &userCounter)); // NOTE: capacity hard coded to be 4 bytes

		glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, m_id);

		glCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));

		return userCounter;
	}

	
	void AtomicCounter::Reset()
	{
		glCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, m_id));

		u32 a = 0;
		glCall(glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(u32) * 1, &a)); // NOTE: capacity hard coded to be 4 bytes

		glCall(glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0));
	}
}


