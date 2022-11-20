#include "NodeBehavior.h"
#include "AssetManagement/AssetManagement.h"

namespace GL
{

	GeometryNodeBehavior::GeometryNodeBehavior(u32 model_id)
		:
		m_model_id(model_id)
	{
	}

	void GeometryNodeBehavior::Draw(const DrawStrategy& strategy, 
		const glm::mat4& proj_matrix, const glm::mat4& view_matrix, const glm::mat4& model_matrix)
	{
		Model& model = *AssetManagement::GetModel(m_model_id);
		strategy.Draw(model, proj_matrix, view_matrix, model_matrix);
	}


}


