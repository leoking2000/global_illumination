#include "Node.h"

namespace GL
{
	Node::Node(std::unique_ptr<NodeBehavior> behavior, const Transform& transform)
		:
		m_behavior(std::move(behavior)),
		m_transform(transform)
	{
	}

	void Node::Update(f32 dt)
	{
		m_behavior->Update(dt, m_transform);

		for (auto& child : m_children)
		{
			child.Update(dt);
		}

		UpdateWolrdMatrix();
	}

	void Node::Draw(const DrawStrategy& strategy, const glm::mat4& proj, const glm::mat4& view)
	{
		m_behavior->Draw(strategy, proj, view, m_wolrdMatrix);

		for (auto& child : m_children)
		{
			child.Draw(strategy, proj, view);
		}
	}

	void Node::AddChild(Node&& node)
	{
		m_children.push_back(std::move(node));
		m_children.back().m_parent = this;
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
			child.UpdateWolrdMatrix();
		}
	}

}


