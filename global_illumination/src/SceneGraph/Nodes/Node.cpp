#include "Node.h"

namespace GL
{
	void Node::Update(f32 dt)
	{
		UpdateSelf(dt);

		for (auto& child : m_children)
		{
			child->UpdateSelf(dt);
		}

		UpdateWolrdMatrix();
	}

	void Node::Draw(const DrawStrategy& strategy, const glm::mat4& proj, const glm::mat4& view)
	{
		DrawSelf(strategy, proj, view);

		for (auto& child : m_children)
		{
			child->DrawSelf(strategy, proj, view);
		}
	}

	void Node::AddChild(std::unique_ptr<Node> node)
	{
		m_children.push_back(std::move(node));
		m_children.back()->m_parent = this;
	}

	Transform& Node::LocalTransform()
	{
		return m_transform;
	}

	glm::mat4 Node::WolrdMatrix()
	{
		return m_wolrdMatrix;
	}

	void Node::UpdateWolrdMatrix()
	{
		if (m_parent != nullptr)
		{
			m_wolrdMatrix = m_parent->m_transform.ModelMatrix() * m_transform.ModelMatrix();
		}
		else
		{
			m_wolrdMatrix = m_transform.ModelMatrix();
		}

		for (auto& child : m_children)
		{
			child->UpdateWolrdMatrix();
		}
	}

}


