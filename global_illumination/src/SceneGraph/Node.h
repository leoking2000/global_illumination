#pragma once
#include <memory>
#include "NodeBehavior.h"

namespace GL
{
	class Node
	{
	public:
		Node(std::unique_ptr<NodeBehavior> behavior, const Transform& transform = Transform());
	public:
		// Updates this node and it's children
		void Update(f32 dt);

		// Draws this node and it's children
		void Draw(const DrawStrategy& strategy, const glm::mat4& proj, const glm::mat4& view);

		// Adds a pre-made Node as a Child to this node
		void AddChild(Node&& node);
	private:
		// Updates this node's wolrdMatrix and it's children
		void UpdateWolrdMatrix();
	private:
		std::vector<Node> m_children;
		Node* m_parent = nullptr;

		glm::mat4 m_wolrdMatrix = glm::identity<glm::mat4>();
		Transform m_transform;

		std::unique_ptr<NodeBehavior> m_behavior;
	};
}