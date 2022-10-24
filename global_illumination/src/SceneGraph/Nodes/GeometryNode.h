#pragma once
#include "Node.h"
#include "../DrawStrategy.h"

namespace GL
{
	class GeometryNode : public Node
	{
	public:
		GeometryNode(u32 model_id);
	protected:
		void UpdateSelf(f32 dt) override;
		void DrawSelf(const DrawStrategy& strategy, const glm::mat4& proj, const glm::mat4& view) override;
	private:
		u32 m_model_id;
	};
}