#pragma once
#include "Global/Types.h"

namespace GL
{
	/*
	* Represents a VBO (vertex buffer object)
	*/
	class VertexBuffer
	{
	public:
		VertexBuffer(const void* data, u32 size);

		VertexBuffer(const VertexBuffer& other) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		VertexBuffer(VertexBuffer&& other);
		VertexBuffer& operator=(VertexBuffer&& other);

		~VertexBuffer();

		void Bind() const;
		void UnBind() const;

	private:
		u32 m_id;
	};
}