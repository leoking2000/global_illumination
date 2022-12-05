#pragma once
#include "SceneGraph/DrawStrategy.h"
#include <functional>

namespace GL
{
	class GeometryDrawStratagy : public DrawStrategy
	{
	public:
		GeometryDrawStratagy(FrameBuffer&& buffer, u32 shader, std::function<void(ShaderProgram&)> setExtraUniforms);
		GeometryDrawStratagy(u32 width, u32 height);
	public:
		void ClearFrameBuffer() override;
		void Draw(const Model& model, const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model_matrix) const override;
		FrameBuffer& GetFrameBuffer() override;
	public:
		void SetExtraUniformsFunction(std::function<void(ShaderProgram&)> setExtraUniforms);
	private:
		FrameBuffer m_geometry_buffer;
		u32 m_geometry_shader;
		std::function<void(ShaderProgram&)> m_setExtraUniforms;
	};


}
