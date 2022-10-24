#include "GeometryNode.h"
#include "AssetManagement/AssetManagement.h"

namespace GL
{
	GeometryNode::GeometryNode(u32 model_id)
		:
		m_model_id(model_id)
	{

	}

	void GeometryNode::UpdateSelf(f32 dt)
	{

	}

	void GeometryNode::DrawSelf(const DrawStrategy& strategy, const glm::mat4& proj, const glm::mat4& view)
	{
		strategy.Draw(*AssetManagement::GetModel(m_model_id), proj, view, WolrdMatrix());
	}
}


