#pragma once
#include "SceneGraph/DrawStrategy.h"
#include "SceneGraph/Light.h"

namespace GL
{
	class ShadowMapDrawStrategy : public DrawStrategy
	{
	public:
		ShadowMapDrawStrategy(u32 shadowMapSize);
	public:
		void ClearFrameBuffer() override;
		void Draw(const Model& model, const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model_matrix) const override;
		FrameBuffer& GetFrameBuffer() override;
	private:
		FrameBuffer m_shadow_map;
		u32 m_shadow_shader;
	};
}
