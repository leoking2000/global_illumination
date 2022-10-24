#pragma once
#include "VertexArray.h"
#include "IndexBuffer.h"

namespace GL
{
	class Mesh
	{
	public:
		Mesh(VertexArray& va, IndexBuffer& ib);
		void Draw();
	public:
		VertexArray m_vertexArray;
		IndexBuffer m_indexBuffer;
	};
}
