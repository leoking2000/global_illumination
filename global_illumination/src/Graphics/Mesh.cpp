#include "OpenGL.h"
#include "Mesh.h"

namespace GL
{
	Mesh::Mesh(VertexArray& va, IndexBuffer& ib)
		:
		m_vertexArray(std::move(va)),
		m_indexBuffer(std::move(ib))
	{
	}

	void Mesh::Draw()
	{
		m_vertexArray.Bind();
		m_indexBuffer.Bind();

		glCall(glDrawElements(GL_TRIANGLES, m_indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr));

		m_vertexArray.UnBind();
		m_indexBuffer.UnBind();
	}
}


