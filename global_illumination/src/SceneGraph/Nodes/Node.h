#pragma once
#include <memory>
#include "../DrawStrategy.h"
#include "../Transform.h"

namespace GL
{
	class Node
	{
	public:
		// Updates this node and it's children
		void Update(f32 dt);

		// Draws this node and it's children
		void Draw(const DrawStrategy& strategy, const glm::mat4& proj, const glm::mat4& view);

		// Adds a pre-made Node as a Child to this node
		void AddChild(std::unique_ptr<Node> node);

		Transform& LocalTransform();
	protected:
		glm::mat4 WolrdMatrix();
		virtual void UpdateSelf(f32 dt) = 0;
		virtual void DrawSelf(const DrawStrategy& strategy, const glm::mat4& proj, const glm::mat4& view) = 0;
	private:
		// Updates this node's wolrdMatrix and it's children
		void UpdateWolrdMatrix();
	private:
		std::vector<std::unique_ptr<Node>> m_children;
		Node* m_parent = nullptr;
		glm::mat4 m_wolrdMatrix = glm::identity<glm::mat4>();
		Transform m_transform;
	};
}