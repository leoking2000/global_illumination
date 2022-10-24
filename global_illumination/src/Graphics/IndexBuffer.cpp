#include "OpenGL.h"
#include "IndexBuffer.h"

namespace GL
{
	IndexBuffer::IndexBuffer(const u32* data, u32 count)
		:
		m_count(count)
	{
		glCall(glGenBuffers(1, &m_id));
		glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
		glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), (const void*)data, GL_STATIC_DRAW));

		glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}

	IndexBuffer::IndexBuffer(IndexBuffer&& other)
		:
		m_count(other.m_count),
		m_id(other.m_id)
	{
		other.m_id = 0;
	}

	IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other)
	{
		glCall(glDeleteBuffers(1, &m_id));

		m_id = other.m_id;
		m_count = other.m_count;

		other.m_id = 0;

		return *this;
	}

	IndexBuffer::~IndexBuffer()
	{
		glCall(glDeleteBuffers(1, &m_id));
	}

	void IndexBuffer::Bind() const
	{
		glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
	}

	void IndexBuffer::UnBind() const
	{
		glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
}


