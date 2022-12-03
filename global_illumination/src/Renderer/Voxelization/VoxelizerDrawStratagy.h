#pragma once
#include "Voxelizer.h"
#include <array>

namespace GL
{
	class VoxelizerDrawStratagy : public DrawStrategy
	{
	public:
		VoxelizerDrawStratagy(const VoxelizerData& data);
	public:
		void ClearFrameBuffer() override;
		void Draw(const Model& model, const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model_matrix) const override;
		FrameBuffer& GetFrameBuffer() override;
	private:
		void Init();
	private:
		const VoxelizerData& m_data;

		FrameBuffer m_framebuffer;
		u32 m_treeWayShader;

		glm::mat4 m_proj_view_axis[3];
		glm::mat4 m_proj_axis[3];
		glm::mat4 m_view_axis[3];
		glm::vec4 m_viewport[3];
	};
}