#pragma once
#include "VertexBuffer.h"
#include "Layout.h"
#include <vector>

namespace GL
{
	/*
	* Represents a VAO
	*/
	class VertexArray
	{
	public:
		VertexArray();

		VertexArray(const VertexArray& other) = delete;
		VertexArray& operator=(const VertexArray) = delete;

		VertexArray(VertexArray&& other);
		VertexArray& operator=(VertexArray&& other);

		~VertexArray();

		void Bind() const;
		void UnBind() const;

		template<u32 ELEMENTS_COUNT>
		void AddBuffer(VertexBuffer& vb, const Layout<ELEMENTS_COUNT>& layout)
		{
			Bind();
			vb.Bind();

			u32 offset = 0;

			for (u32 i = 0; i < ELEMENTS_COUNT; i++)
			{
				AddAttrib(i, layout[i], layout.GetStride(), offset);
			}

			UnBind();
			vb.UnBind();

			m_buffers.emplace_back(std::move(vb));
		}

	private:
		void AddAttrib(u32 i, ElementType element_type, u32 stride, u32& offset);
	private:
		u32 m_id;
		std::vector<VertexBuffer> m_buffers;
	};
}