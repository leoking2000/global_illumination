#include "OpenGL.h"
#include "VertexBuffer.h"

namespace GL
{
	VertexBuffer::VertexBuffer(const void* data, u32 size)
	{
		glCall(glGenBuffers(1, &m_id));
		glCall(glBindBuffer(GL_ARRAY_BUFFER, m_id));
		glCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

		glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	VertexBuffer::VertexBuffer(VertexBuffer&& other)
		:
		m_id(other.m_id)
	{
		other.m_id = 0;
	}

	VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other)
	{
		glCall(glDeleteBuffers(1, &m_id));

		m_id = other.m_id;
		other.m_id = 0;
		return *this;
	}

	VertexBuffer::~VertexBuffer()
	{
		glCall(glDeleteBuffers(1, &m_id));
	}

	void VertexBuffer::Bind() const
	{
		glCall(glBindBuffer(GL_ARRAY_BUFFER, m_id));
	}

	void VertexBuffer::UnBind() const
	{
		glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
}


