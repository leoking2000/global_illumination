#include "OpenGL.h"
#include "VertexArray.h"

namespace GL
{
	VertexArray::VertexArray()
	{
		glCall(glGenVertexArrays(1, &m_id));
	}

	VertexArray::VertexArray(VertexArray&& other)
		:
		m_id(other.m_id)
	{
		other.m_id = 0;
		m_buffers = std::move(other.m_buffers);
	}

	VertexArray& VertexArray::operator=(VertexArray&& other)
	{
		glCall(glDeleteVertexArrays(1, &m_id));

		m_id = other.m_id;
		other.m_id = 0;

		m_buffers = std::move(other.m_buffers);

		return *this;
	}

	VertexArray::~VertexArray()
	{
		glCall(glDeleteVertexArrays(1, &m_id));
	}

	void VertexArray::Bind() const
	{
		glCall(glBindVertexArray(m_id));
	}

	void VertexArray::UnBind() const
	{
		glCall(glBindVertexArray(0));
	}

	void VertexArray::AddAttrib(u32 i, ElementType element_type, u32 stride, u32& offset)
	{
		auto normalize = element_type % 2 == 0 ? GL_FALSE : GL_TRUE;
		auto type = GL_FLOAT;
		u32 size = sizeof(float);
		u32 count = 1;

		switch (element_type)
		{
		case FLOAT1:
		case FLOAT1_N:
			type = GL_FLOAT;
			size = sizeof(float);
			count = 1;
			break;
		case FLOAT2:
		case FLOAT2_N:
			type = GL_FLOAT;
			size = sizeof(float);
			count = 2;
			break;
		case FLOAT3:
		case FLOAT3_N:
			type = GL_FLOAT;
			size = sizeof(float);
			count = 3;
			break;
		case FLOAT4:
		case FLOAT4_N:
			type = GL_FLOAT;
			size = sizeof(float);
			count = 4;
			break;

		case UCHAR3:
		case UCHAR3_N:
			type = GL_UNSIGNED_BYTE;
			size = 1;
			count = 3;
			break;
		case UCHAR4:
		case UCHAR4_N:
			type = GL_UNSIGNED_BYTE;
			size = 1;
			count = 4;
			break;
		default:
			assert(false && "VertexArray::AddBuffer error unknowed ElementType!!!");
			break;
		}

		glCall(glEnableVertexAttribArray(i));
		glCall(glVertexAttribPointer(i, count, type, normalize, stride, (const void*)offset));

		offset += count * size;
	}
}


