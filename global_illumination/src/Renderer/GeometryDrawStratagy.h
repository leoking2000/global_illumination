#pragma once
#include "SceneGraph/DrawStrategy.h"
#include "Renderer.h"

namespace GL
{
	class GeometryDrawStratagy : public DrawStrategy
	{
	public:
		GeometryDrawStratagy(u32 width, u32 height);
	public:
		void ClearFrameBuffer() override;
		void Draw(const Model& model, const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model_matrix) const override;
		FrameBuffer& GetFrameBuffer() override;
	private:
		FrameBuffer m_geometry_buffer;
		u32 m_geometry_shader;
	};


}
