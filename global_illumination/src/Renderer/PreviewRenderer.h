#pragma once
#include "Voxelization/VoxelizerData.h"
#include "Graphics/FrameBuffer.h"

namespace GL
{
	class PreviewRenderer
	{
	public:
		PreviewRenderer();

		PreviewRenderer(const PreviewRenderer&) = delete;
		PreviewRenderer& operator=(const PreviewRenderer&) = delete;
	public:
		void DrawVoxels(const FrameBuffer& geometryBuffer, const glm::mat4& proj_view, const VoxelizerData& data,
								const FrameBuffer& voxelBuffer, bool is_musked, bool show_all);

	private:
		// for Voxels
		u32 m_PreviewCubesShader;
		u32 m_cube;

	};
}

