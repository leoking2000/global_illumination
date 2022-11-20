#pragma once
#include "Transform.h"
#include "DrawStrategy.h"

namespace GL
{
	class NodeBehavior
	{
	public:
		virtual void Update(f32 dt, const Transform& transform) {};
		virtual void Draw(const DrawStrategy& strategy,
			const glm::mat4& proj_matrix, const glm::mat4& view_matrix, const glm::mat4& model_matrix) {};
	};

	class GeometryNodeBehavior : public NodeBehavior
	{
	public:
		GeometryNodeBehavior(u32 model_id);
	public:
		void Draw(const DrawStrategy& strategy,
			const glm::mat4& proj_matrix, const glm::mat4& view_matrix, const glm::mat4& model_matrix) override;
	private:
		u32 m_model_id;
	};
}